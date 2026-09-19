#include "LWNavigation.h"
#include "LWGeneration.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include <limits>

namespace LWNavigationTests
{
    constexpr double Tolerance = 0.25;

    bool ContainsPoint(const TArray<FVector2D>& Path, FVector2D Point)
    {
        return Path.ContainsByPredicate([Point](FVector2D P) { return P.Equals(Point, Tolerance); });
    }

    // Check complete edges against generated centerlines, not just the route's vertices.
    // The only allowed non-centerline road edges are the small driveway/chord repairs.
    bool RoadLeg(FVector2D A, FVector2D B, int32 Seed)
    {
        TArray<LWGen::FRoad> Roads;
        TArray<LWGen::FSite> Sites;
        LWGen::Gather((A + B) * 0.5, Seed, Roads, Sites);
        for (const LWGen::FRoad& Road : Roads)
        {
            if (LWGen::DistanceToSegment(A, Road) <= Tolerance &&
                LWGen::DistanceToSegment(B, Road) <= Tolerance)
            {
                return true;
            }
        }
        if ((A - B).Size() > 64.0 + Tolerance)
        {
            return false;
        }
        for (const LWGen::FRoad& Driveway : Roads)
        {
            if (Driveway.Width > 400.0f)
            {
                continue;
            }
            for (FVector2D End : {Driveway.A, Driveway.B})
            {
                for (const LWGen::FRoad& Parent : Roads)
                {
                    if (Parent.Width <= Driveway.Width)
                    {
                        continue;
                    }
                    FVector2D Q;
                    if (LWGen::DistanceToSegment(End, Parent, &Q) <= 64.0 &&
                        ((A.Equals(End, Tolerance) && B.Equals(Q, Tolerance)) ||
                         (B.Equals(End, Tolerance) && A.Equals(Q, Tolerance))))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool FollowsRoads(const TArray<FVector2D>& Path, int32 Seed, bool OffroadEnds = false)
    {
        if (Path.Num() < 2 || (OffroadEnds && Path.Num() < 4))
        {
            return false;
        }
        const int32 Begin = OffroadEnds ? 2 : 1;
        const int32 End = OffroadEnds ? Path.Num() - 1 : Path.Num();
        for (int32 I = Begin; I < End; ++I)
        {
            if (Path[I] == Path[I - 1] || !RoadLeg(Path[I - 1], Path[I], Seed))
            {
                return false;
            }
        }
        return true;
    }

    FVector2D NearestRoad(FVector2D P, int32 Seed)
    {
        TArray<LWGen::FRoad> Roads;
        TArray<LWGen::FSite> Sites;
        LWGen::Gather(P, Seed, Roads, Sites);
        double Best = TNumericLimits<double>::Max();
        FVector2D Result = P;
        for (const LWGen::FRoad& Road : Roads)
        {
            FVector2D Q;
            const double Distance = LWGen::DistanceToSegment(P, Road, &Q);
            if (Distance < Best)
            {
                Best = Distance;
                Result = Q;
            }
        }
        return Result;
    }

    double PathLength(const TArray<FVector2D>& Path)
    {
        double Length = 0.0;
        for (int32 I = 1; I < Path.Num(); ++I)
        {
            Length += (Path[I] - Path[I - 1]).Size();
        }
        return Length;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNavigationServiceTest, "LethalWorld.Navigation.ServiceTJunction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWNavigationServiceTest::RunTest(const FString& Parameters)
{
    using namespace LWNavigationTests;
    for (int32 Seed : {0, 198706, -97})
    {
        const FVector2D H = LWGen::Hub(FIntPoint::ZeroValue, Seed);
        const FVector2D Start = H + FVector2D(0, 1400);
        const FVector2D Goal = NearestRoad(H + FVector2D(-4200, 4200),Seed);
        bool Complete = false;
        const TArray<FVector2D> Path = LWNavigation::FindPath(Start, Goal, Seed, &Complete);
        TestTrue(TEXT("Settlement route is complete"), Complete);
        if (!TestTrue(TEXT("Settlement route exists"), Path.Num() >= 4))
        {
            continue;
        }
        TestEqual(TEXT("Exact start"), Path[0], Start);
        TestEqual(TEXT("Exact goal"), Path.Last(), Goal);
        TestTrue(TEXT("Turn at the interior service T junction"), ContainsPoint(Path, H + FVector2D(0, 2800)));
        TestTrue(TEXT("Split service street at the settlement driveway"), ContainsPoint(Path, H + FVector2D(-4200, 2800)));
        TestTrue(TEXT("Every service route edge follows an actual road"), FollowsRoads(Path, Seed));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNavigationCrossingTest, "LethalWorld.Navigation.InteriorRoadCrossing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWNavigationCrossingTest::RunTest(const FString& Parameters)
{
    using namespace LWNavigationTests;
    constexpr int32 Seed = 198706;
    TArray<LWGen::FRoad> Roads;
    TArray<LWGen::FSite> Sites;
    LWGen::Region(FIntPoint::ZeroValue, Seed, Roads, Sites);
    bool FoundCrossing = false;
    for (const LWGen::FRoad& Road : Roads)
    {
        if (Road.Width < 780.0f || FMath::Abs(Road.B.Y - Road.A.Y) < 1.0)
        {
            continue;
        }
        const double T = (2800.0 - Road.A.Y) / (Road.B.Y - Road.A.Y);
        const FVector2D Crossing = Road.A + (Road.B - Road.A) * T;
        if (T <= 0.01 || T >= 0.99 || FMath::Abs(Crossing.X) >= 4700.0)
        {
            continue;
        }
        FoundCrossing = true;
        // Both query points are inside their roads. The shortest road route must turn
        // at a crossing that is an endpoint of neither generated segment.
        // Sampling density can shorten the segment before the crossing. Keep the fixture on it.
        const double ApproachDistance = FMath::Min(500.0, (Road.A - Crossing).Size() * .5);
        const FVector2D Start = Crossing + (Road.A - Crossing).GetSafeNormal() * ApproachDistance;
        const FVector2D Goal = Crossing + FVector2D(Crossing.X >= 0.0 ? 600.0 : -600.0, 0);
        bool Complete = false;
        const TArray<FVector2D> Path = LWNavigation::FindPath(Start, Goal, Seed, &Complete);
        TestTrue(TEXT("Crossing route is complete"), Complete);
        TestTrue(TEXT("Route turns at the actual interior crossing"), ContainsPoint(Path, Crossing));
        TestTrue(TEXT("Crossing route follows road segments"), FollowsRoads(Path, Seed));
        TestTrue(TEXT("No detour to segment endpoints"), FMath::Abs(PathLength(Path) - (ApproachDistance + 600.0)) < 0.5);
        break;
    }
    TestTrue(TEXT("Fixture contains a generated arterial/service crossing"), FoundCrossing);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNavigationDrivewayTest, "LethalWorld.Navigation.CurvedRoadDrivewayAttachments",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWNavigationDrivewayTest::RunTest(const FString& Parameters)
{
    using namespace LWNavigationTests;
    bool SawChordGap = false;
    for (int32 Seed : {0, 198706, -97})
    {
        for (FIntPoint Region : {FIntPoint(0, 0), FIntPoint(-5, -8)})
        {
            TArray<LWGen::FRoad> Roads;
            TArray<LWGen::FSite> Sites;
            LWGen::Region(Region, Seed, Roads, Sites);
            for (const LWGen::FRoad& Driveway : Roads)
            {
                if (Driveway.Width != 400.0f)
                {
                    continue;
                }
                // The bunker driveway joins a straight service street, not an arterial chord.
                if (Driveway.B.Equals(FVector2D(-1700,1700),.1))
                {
                    bool Complete=false;
                    const auto Path=LWNavigation::FindPath(LWGen::Hub(Region,Seed),Driveway.B,Seed,&Complete);
                    TestTrue(TEXT("Bunker driveway route is complete"),Complete);
                    TestTrue(TEXT("Bunker route follows generated roads"),FollowsRoads(Path,Seed));
                    TestTrue(TEXT("Bunker route joins the service street at its T junction"),ContainsPoint(Path,Driveway.A));
                    continue;
                }
                double Gap = TNumericLimits<double>::Max();
                FVector2D Attachment;
                for (const LWGen::FRoad& Parent : Roads)
                {
                    if (Parent.Width <= 400.0f)
                    {
                        continue;
                    }
                    FVector2D Q;
                    const double Distance = LWGen::DistanceToSegment(Driveway.A, Parent, &Q);
                    if (Distance < Gap)
                    {
                        Gap = Distance;
                        Attachment = Q;
                    }
                }
                bool Complete = false;
                const TArray<FVector2D> Path = LWNavigation::FindPath(
                    LWGen::Hub(Region, Seed), Driveway.B, Seed, &Complete);
                TestTrue(TEXT("Rural driveway route is complete"), Complete);
                TestTrue(TEXT("Driveway route follows generated roads and explicit attachments"), FollowsRoads(Path, Seed));
                SawChordGap = true;
                TestTrue(TEXT("Driveway begins exactly on its parent polyline"), Gap < .1);

            }
        }
    }
    TestTrue(TEXT("Fixtures exercise exact polyline attachments"), SawChordGap);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNavigationEndpointTest, "LethalWorld.Navigation.NegativeCoordinatesAndEndpoints",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWNavigationEndpointTest::RunTest(const FString& Parameters)
{
    using namespace LWNavigationTests;
    constexpr int32 Seed = -97;
    for (FVector2D Start : {FVector2D(-1, -1), FVector2D(-25601, -25601), FVector2D(-282000, -437000)})
    {
        const FVector2D Goal = Start + FVector2D(47000, -31000);
        bool Complete = false;
        const TArray<FVector2D> Path = LWNavigation::FindPath(Start, Goal, Seed, &Complete);
        TestTrue(TEXT("Negative-coordinate route is complete"), Complete);
        if (!TestTrue(TEXT("Road route includes two offroad legs"), Path.Num() >= 4))
        {
            continue;
        }
        TestEqual(TEXT("Offroad start is retained exactly"), Path[0], Start);
        TestEqual(TEXT("Offroad goal is retained exactly"), Path.Last(), Goal);
        TestTrue(TEXT("Start projects to the nearest generated road"), Path[1].Equals(NearestRoad(Start, Seed), Tolerance));
        TestTrue(TEXT("Goal projects to the nearest generated road"), Path[Path.Num() - 2].Equals(NearestRoad(Goal, Seed), Tolerance));
        TestTrue(TEXT("All legs between projections stay on roads"), FollowsRoads(Path, Seed, true));
        const TArray<FVector2D> Again = LWNavigation::FindPath(Start, Goal, Seed);
        TestTrue(TEXT("Recomputing the same query is deterministic"), Path == Again);
    }

    TArray<LWGen::FRoad> Roads;
    TArray<LWGen::FSite> Sites;
    LWGen::Region(FIntPoint(-2, -3), Seed, Roads, Sites);
    const FVector2D A = Roads[2].A + (Roads[2].B - Roads[2].A) * 0.2;
    const FVector2D B = Roads[2].A + (Roads[2].B - Roads[2].A) * 0.75;
    for (bool Reverse : {false, true})
    {
        const FVector2D Start = Reverse ? B : A, Goal = Reverse ? A : B;
        bool Complete = false;
        const TArray<FVector2D> Path = LWNavigation::FindPath(Start, Goal, Seed, &Complete);
        TestTrue(TEXT("Two projections on one segment are connected in either direction"), Complete && Path.Num() >= 2);
        if (Path.Num() >= 2)
        {
            TestEqual(TEXT("Same-segment exact start"), Path[0], Start);
            TestEqual(TEXT("Same-segment exact goal"), Path.Last(), Goal);
            TestTrue(TEXT("Same-segment route follows the road"), FollowsRoads(Path, Seed));
            TestTrue(TEXT("Same-segment route has no endpoint detour"), FMath::Abs(PathLength(Path) - (A - B).Size()) < 0.5);
        }
    }

    const FVector2D Same(-25001, -34002);
    bool Complete = false;
    const TArray<FVector2D> Stationary = LWNavigation::FindPath(Same, Same, Seed, &Complete);
    TestTrue(TEXT("Start equals goal returns one exact point and completes"), Complete && Stationary.Num() == 1 && Stationary[0] == Same);
    Complete = true;
    const TArray<FVector2D> Invalid = LWNavigation::FindPath(
        FVector2D(std::numeric_limits<double>::quiet_NaN(), 0), Same, Seed, &Complete);
    TestTrue(TEXT("Invalid coordinates return empty and reset completion"), Invalid.IsEmpty() && !Complete);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNavigationProgressTest, "LethalWorld.Navigation.BoundedRouteProgress",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWNavigationProgressTest::RunTest(const FString& Parameters)
{
    using namespace LWNavigationTests;
    constexpr int32 Seed = 198706;
    struct FCorridorCase { FIntPoint GoalRegion; bool ExpectedComplete; };
    const FCorridorCase CorridorCases[] = {
        {FIntPoint(9, 0), true}, {FIntPoint(10, 0), false},
        {FIntPoint(5, 5), true}, {FIntPoint(6, 5), false},
        {FIntPoint(7, 3), true}, {FIntPoint(8, 3), false},
        {FIntPoint(0, -10), false}, {FIntPoint(-5, -6), false}
    };
    for (const FCorridorCase& Case : CorridorCases)
    {
        bool Reached = !Case.ExpectedComplete;
        const FVector2D Goal = LWGen::Hub(Case.GoalRegion, Seed);
        const TArray<FVector2D> Path = LWNavigation::FindPath(FVector2D::ZeroVector, Goal, Seed, &Reached);
        TestTrue(TEXT("Per-axis and total-region caps select full versus cropped routes"), Reached == Case.ExpectedComplete);
        TestTrue(TEXT("Routes at corridor budget boundaries remain connected"), FollowsRoads(Path, Seed));
        if (!Path.IsEmpty())
        {
            TestTrue(TEXT("Only complete routes append the final destination"), (Path.Last() == Goal) == Reached);
        }
    }

    const FVector2D Start = LWGen::Hub(FIntPoint(-3, 2), Seed);
    const FVector2D FarGoal = LWGen::Hub(FIntPoint(5000, -7000), Seed);
    bool Complete = true;
    const TArray<FVector2D> Prefix = LWNavigation::FindPath(Start, FarGoal, Seed, &Complete);
    TestFalse(TEXT("Distant destination reports a cropped route"), Complete);
    if (TestTrue(TEXT("Distant route still returns a connected prefix"), Prefix.Num() >= 2))
    {
        TestEqual(TEXT("Cropped route starts at the player"), Prefix[0], Start);
        TestTrue(TEXT("Cropped endpoint makes strict progress"), (FarGoal - Prefix.Last()).Size() < (FarGoal - Start).Size());
        TestTrue(TEXT("No offroad shortcut to the distant destination"), FollowsRoads(Prefix, Seed));
        TestTrue(TEXT("Prefix remains local despite the huge destination distance"), PathLength(Prefix) < 32 * LWGen::RegionSize);
        const FIntPoint EndRegion(LWGen::FloorDiv(Prefix.Last().X + LWGen::RegionSize * 0.5, LWGen::RegionSize),
            LWGen::FloorDiv(Prefix.Last().Y + LWGen::RegionSize * 0.5, LWGen::RegionSize));
        TestTrue(TEXT("Cropped endpoint is an exact regional hub"), Prefix.Last() == LWGen::Hub(EndRegion, Seed));

        // Recompute while moving along the prefix, before reaching its temporary hub.
        const FVector2D MovingStart = Prefix[Prefix.Num() / 2];
        const TArray<FVector2D> Moving = LWNavigation::FindPath(MovingStart, FarGoal, Seed, &Complete);
        TestTrue(TEXT("Moving recompute still returns a cropped road route"), !Complete && FollowsRoads(Moving, Seed));
        if (!Moving.IsEmpty())
        {
            TestEqual(TEXT("Moving recompute starts at the new player position"), Moving[0], MovingStart);
            TestTrue(TEXT("Moving recompute makes progress toward the original goal"),
                (FarGoal - Moving.Last()).Size() < (FarGoal - MovingStart).Size());
        }
    }

    for (FIntPoint GoalRegion : {FIntPoint(27, -19), FIntPoint(-30, 2), FIntPoint(-3, 35)})
    {
        const FVector2D Goal = NearestRoad(LWGen::Hub(GoalRegion, Seed) + FVector2D(-4200, 4200),Seed);
        FVector2D Current = Start;
        Complete = false;
        for (int32 Step = 0; Step < 20 && !Complete; ++Step)
        {
            const TArray<FVector2D> Path = LWNavigation::FindPath(Current, Goal, Seed, &Complete);
            if (!TestTrue(TEXT("Every hierarchical step has a route"), Path.Num() >= 2))
            {
                break;
            }
            TestTrue(TEXT("Every hierarchical step follows roads"), FollowsRoads(Path, Seed));
            TestTrue(TEXT("Every hierarchical step strictly reduces remaining distance"),
                (Goal - Path.Last()).Size() < (Goal - Current).Size());
            Current = Path.Last();
        }
        TestTrue(TEXT("Repeated recomputes eventually complete"), Complete);
        TestEqual(TEXT("Final route reaches the original exact goal"), Current, Goal);
    }
    return true;
}
#endif
