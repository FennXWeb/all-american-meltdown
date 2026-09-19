#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWGenerationTest,"LethalWorld.Generation.DeterminismAndConnectivity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWGenerationTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Negative chunk coordinate floors instead of truncating"),LWGen::ChunkAt(FVector2D(-1,-12801)),FIntPoint(-1,-2));
    TestEqual(TEXT("Exact chunk boundary"),LWGen::ChunkAt(FVector2D(12800,0)),FIntPoint(1,0));
    for(int32 Seed:{0,198706,-97})for(FIntPoint Region:{FIntPoint(0,0),FIntPoint(-5,-8),FIntPoint(12000,9000)})
    {
        TArray<LWGen::FRoad> A,B;TArray<LWGen::FSite> SA,SB;
        LWGen::Region(Region,Seed,A,SA);LWGen::Region(Region,Seed,B,SB);
        TestEqual(TEXT("Road count stable"),A.Num(),B.Num());TestEqual(TEXT("POI count stable"),SA.Num(),SB.Num());
        for(int32 I=0;I<A.Num();I++){TestTrue(TEXT("Deterministic endpoints"),A[I].A==B[I].A&&A[I].B==B[I].B);TestTrue(TEXT("Nonzero segment"),(A[I].A-A[I].B).Size()>1);}
        const FVector2D East=LWGen::Hub(Region+FIntPoint(1,0),Seed),North=LWGen::Hub(Region+FIntPoint(0,1),Seed);
        TestTrue(TEXT("East arterial reaches neighboring hub"),A.ContainsByPredicate([East](const auto& R){return R.B.Equals(East,.1);}));
        if(Region.X%3==0)TestTrue(TEXT("North spine reaches neighboring hub"),A.ContainsByPredicate([North](const auto& R){return R.B.Equals(North,.1);}));
        // Missing local north links must still connect through the regional backbone.
        TMap<FIntPoint,TArray<FIntPoint>> Graph;auto Key=[](FVector2D P){return FIntPoint(FMath::RoundToInt(P.X),FMath::RoundToInt(P.Y));};
        for(int Y=0;Y<=1;Y++)for(int X=-3;X<=3;X++){TArray<LWGen::FRoad> Links;TArray<LWGen::FSite> Parcels;LWGen::Region(Region+FIntPoint(X,Y),Seed,Links,Parcels);for(const auto& Link:Links){Graph.FindOrAdd(Key(Link.A)).Add(Key(Link.B));Graph.FindOrAdd(Key(Link.B)).Add(Key(Link.A));}}
        TSet<FIntPoint> Seen;TArray<FIntPoint> Queue;Queue.Add(Key(LWGen::Hub(Region,Seed)));for(int I=0;I<Queue.Num();I++){FIntPoint Current=Queue[I];if(Seen.Contains(Current))continue;Seen.Add(Current);if(const auto* Neighbors=Graph.Find(Current))for(auto Next:*Neighbors)if(!Seen.Contains(Next))Queue.Add(Next);}
        TestTrue(TEXT("Regional graph reaches north hub through connected spines"),Seen.Contains(Key(North)));

        TSet<uint32> IDs;
        for(const auto& S:SA)
        {
            TestFalse(TEXT("Unique parcel id within region"),IDs.Contains(S.Id));IDs.Add(S.Id);
            TestTrue(TEXT("Every site has road access"),A.ContainsByPredicate([S](const auto& R){return R.B.Equals(LWGen::Entrance(S),.1);}));
            TestEqual(TEXT("POI foundations are level"),LWGen::Height(S.Position,A,SA),0.f);
        }
    }
    TArray<LWGen::FRoad> RA,RB;TArray<LWGen::FSite> SA,SB;
    LWGen::Gather(FVector2D(12799,100),198706,RA,SA);LWGen::Gather(FVector2D(12801,100),198706,RB,SB);
    const FVector2D Bunker(-1700,1700);
    TestTrue(TEXT("Bunker driveway joins the settlement street"),RA.ContainsByPredicate([Bunker](const auto& R){return R.A.Equals(FVector2D(-1700,2800))&&R.B.Equals(Bunker);}));
    for(const FVector2D Offset:{FVector2D::ZeroVector,FVector2D(0,250),FVector2D(-450,-450),FVector2D(450,450)})
        TestEqual(TEXT("Bunker entrance and exit apron are level"),LWGen::Height(Bunker+Offset,RA,SA),0.f);
    for(int32 Y=-10;Y<10;Y++)
    {FVector2D P(12800,Y*625);TestEqual(TEXT("Adjacent chunks agree on shared terrain vertices"),LWGen::Height(P,RA,SA),LWGen::Height(P,RB,SB));}
    return true;
}
#endif
