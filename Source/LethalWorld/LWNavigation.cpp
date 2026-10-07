#include "LWNavigation.h"
#include "LWGeneration.h"

#include <queue>
#include <vector>

namespace LWNavigation
{
namespace
{
    constexpr int32 MaxRegionsPerAxis = 12;
    constexpr int32 MaxRegions = 64;
    constexpr int32 RegionPadding = 1;
    constexpr double JunctionTolerance = 0.1;
    constexpr double NodeCellSize = 1.0;
    constexpr double RoadCellSize = LWGen::ChunkSize;
    // LWGen places rural driveways on a sine curve, but renders eight straight chords.
    // The maximum curve/chord gap is <35 units (1800 * (PI/8)^2 / 8).
    // Bridge that gap explicitly; do not merge nearby parallel road centerlines.
    constexpr double DrivewayReach = 64.0;

    struct FRegionBounds
    {
        FIntPoint Min, Max;

        FRegionBounds(FIntPoint A, FIntPoint B)
            : Min(FMath::Min(A.X, B.X) - RegionPadding, FMath::Min(A.Y, B.Y) - RegionPadding)
            , Max(FMath::Max(A.X, B.X) + RegionPadding, FMath::Max(A.Y, B.Y) + RegionPadding)
        {}

        bool Fits() const
        {
            const int64 Width = int64(Max.X) - Min.X + 1;
            const int64 Height = int64(Max.Y) - Min.Y + 1;
            return Width <= MaxRegionsPerAxis && Height <= MaxRegionsPerAxis && Width * Height <= MaxRegions;
        }
    };

    bool RegionAt(FVector2D Position, FIntPoint& Result)
    {
        // Leave room for bounds, outgoing edges, and LWGen's signed R.X + R.Y.
        constexpr double Limit = double(MAX_int32 / 2 - MaxRegionsPerAxis - 2);
        const double X = Position.X / LWGen::RegionSize + 0.5;
        const double Y = Position.Y / LWGen::RegionSize + 0.5;
        if (!FMath::IsFinite(X) || !FMath::IsFinite(Y) ||
            FMath::Abs(X) > Limit || FMath::Abs(Y) > Limit)
        {
            return false;
        }
        // Match Gather's centered regions, including the negative half of the world.
        Result = FIntPoint(FMath::FloorToInt(X), FMath::FloorToInt(Y));
        return true;
    }

    bool SelectTarget(FVector2D Start, FVector2D Goal, FIntPoint StartRegion,
        FIntPoint GoalRegion, int32 Seed, FVector2D& Target, FRegionBounds& Bounds)
    {
        Bounds = FRegionBounds(StartRegion, GoalRegion);
        Target = Goal;
        if (Bounds.Fits())
        {
            return true;
        }

        // Coarse tier: choose a hub toward the final destination whose entire padded
        // corridor fits. Enumerating these fixed-size offsets never gathers distant regions.
        constexpr int32 Advance = MaxRegionsPerAxis - 2 * RegionPadding - 1;
        const FIntPoint Delta = GoalRegion - StartRegion;
        double BestDistance = (Goal - Start).SizeSquared();
        Bounds = FRegionBounds(StartRegion, StartRegion);
        Target = Start;
        for (int32 Y = FMath::Clamp(Delta.Y, -Advance, 0); Y <= FMath::Clamp(Delta.Y, 0, Advance); ++Y)
        {
            for (int32 X = FMath::Clamp(Delta.X, -Advance, 0); X <= FMath::Clamp(Delta.X, 0, Advance); ++X)
            {
                if (X == 0 && Y == 0)
                {
                    continue;
                }
                const FIntPoint CandidateRegion = StartRegion + FIntPoint(X, Y);
                const FRegionBounds CandidateBounds(StartRegion, CandidateRegion);
                if (!CandidateBounds.Fits())
                {
                    continue;
                }
                const FVector2D Candidate = LWGen::Hub(CandidateRegion, Seed);
                const double Distance = (Goal - Candidate).SizeSquared();
                if (Distance < BestDistance)
                {
                    BestDistance = Distance;
                    Target = Candidate;
                    Bounds = CandidateBounds;
                }
            }
        }
        return false;
    }

    double Cross(FVector2D A, FVector2D B)
    {
        return A.X * B.Y - A.Y * B.X;
    }

    double Project(FVector2D P, const LWGen::FRoad& Road, FVector2D& Closest)
    {
        const FVector2D D = Road.B - Road.A;
        const double LengthSquared = D.SizeSquared();
        const double T = LengthSquared > 0.0
            ? FMath::Clamp(FVector2D::DotProduct(P - Road.A, D) / LengthSquared, 0.0, 1.0) : 0.0;
        Closest = Road.A + D * T;
        return T;
    }

    struct FCut
    {
        double T;
        int32 Node;
    };

    struct FSegment
    {
        LWGen::FRoad Road;
        TArray<FCut> Cuts;
    };

    struct FArc
    {
        int32 To;
        double Cost;
    };

    struct FNode
    {
        FVector2D Position;
        TArray<FArc> Edges;
    };

    struct FGraph
    {
        FVector2D Origin;
        TArray<FSegment> Segments;
        TArray<FNode> Nodes;
        TMap<FIntPoint, TArray<int32>> NodeCells;
        TMap<FIntPoint, TArray<int32>> RoadCells;

        explicit FGraph(FVector2D InOrigin) : Origin(InOrigin) {}

        FIntPoint Cell(FVector2D P, double Size) const
        {
            // Relative coordinates keep spatial hashes small even far from world origin.
            return FIntPoint(FMath::FloorToInt((P.X - Origin.X) / Size),
                FMath::FloorToInt((P.Y - Origin.Y) / Size));
        }

        int32 NodeAt(FVector2D P)
        {
            const FIntPoint Key = Cell(P, NodeCellSize);
            for (int32 Y = -1; Y <= 1; ++Y)
            {
                for (int32 X = -1; X <= 1; ++X)
                {
                    if (const TArray<int32>* Bucket = NodeCells.Find(Key + FIntPoint(X, Y)))
                    {
                        for (int32 Id : *Bucket)
                        {
                            if ((Nodes[Id].Position - P).SizeSquared() <= JunctionTolerance * JunctionTolerance)
                            {
                                return Id;
                            }
                        }
                    }
                }
            }
            const int32 Id = Nodes.Num();
            Nodes.Add({P, {}});
            NodeCells.FindOrAdd(Key).Add(Id);
            return Id;
        }

        void Link(int32 A, int32 B)
        {
            if (A == B)
            {
                return;
            }
            const double Cost = (Nodes[A].Position - Nodes[B].Position).Size();
            Nodes[A].Edges.Add({B, Cost});
            Nodes[B].Edges.Add({A, Cost});
        }

        void AddCut(int32 Segment, double T, int32 Node)
        {
            Segments[Segment].Cuts.Add({FMath::Clamp(T, 0.0, 1.0), Node});
        }

        void RoadCellBounds(const LWGen::FRoad& Road, FIntPoint& Min, FIntPoint& Max) const
        {
            Min = Cell(FVector2D(FMath::Min(Road.A.X, Road.B.X) - DrivewayReach,
                FMath::Min(Road.A.Y, Road.B.Y) - DrivewayReach), RoadCellSize);
            Max = Cell(FVector2D(FMath::Max(Road.A.X, Road.B.X) + DrivewayReach,
                FMath::Max(Road.A.Y, Road.B.Y) + DrivewayReach), RoadCellSize);
        }

        void Gather(const FRegionBounds& Bounds, int32 Seed,bool NewYork=false)
        {
            TArray<LWGen::FRoad> Roads;
            if(NewYork){Roads=LWNY69::Roads();const auto City=LWGen::CanadaCity68();const FIntPoint Center(LWGen::FloorDiv(City.X+25600,51200),LWGen::FloorDiv(City.Y+25600,51200));TArray<LWGen::FSite> Ignored;for(int X=-2;X<=2;X++)for(int Y=-2;Y<=2;Y++)LWGen::CanadaRegion68(Center+FIntPoint(X,Y),Seed,Roads,Ignored);}
            else for(int32 Y=Bounds.Min.Y;Y<=Bounds.Max.Y;++Y)for(int32 X=Bounds.Min.X;X<=Bounds.Max.X;++X){TArray<LWGen::FSite> Sites;LWGen::Region(FIntPoint(X,Y),Seed,Roads,Sites);}
            for(const auto& Road:Roads){
                const int32 Id=Segments.Num();Segments.Add({Road,{}});
                AddCut(Id,0.,NodeAt(Road.A));AddCut(Id,1.,NodeAt(Road.B));
                FIntPoint Min,Max;RoadCellBounds(Road,Min,Max);
                for(int32 CY=Min.Y;CY<=Max.Y;++CY)for(int32 CX=Min.X;CX<=Max.X;++CX)RoadCells.FindOrAdd({CX,CY}).Add(Id);
            }
        }

        void Candidates(const LWGen::FRoad& Road, TArray<int32>& Result) const
        {
            FIntPoint Min, Max;
            RoadCellBounds(Road, Min, Max);
            TSet<int32> Unique;
            for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
            {
                for (int32 X = Min.X; X <= Max.X; ++X)
                {
                    if (const TArray<int32>* Bucket = RoadCells.Find(FIntPoint(X, Y)))
                    {
                        for (int32 Id : *Bucket)
                        {
                            Unique.Add(Id);
                        }
                    }
                }
            }
            Result = Unique.Array();
            Result.Sort(); // TSet iteration must not influence node positions or route ties.
        }

        void EndpointOnSegment(int32 From, double T, int32 Onto)
        {
            const LWGen::FRoad& Road = Segments[From].Road;
            const FVector2D P = T == 0.0 ? Road.A : Road.B;
            FVector2D Q;
            const double U = Project(P, Segments[Onto].Road, Q);
            if ((P - Q).SizeSquared() <= JunctionTolerance * JunctionTolerance)
            {
                const int32 Node = NodeAt(P);
                AddCut(From, T, Node);
                AddCut(Onto, U, Node);
            }
        }

        void Intersect(int32 A, int32 B)
        {
            const LWGen::FRoad& RA = Segments[A].Road;
            const LWGen::FRoad& RB = Segments[B].Road;
            const FVector2D DA = RA.B - RA.A, DB = RB.B - RB.A;
            const double Denominator = Cross(DA, DB);
            const double LengthA = DA.Size(), LengthB = DB.Size();
            if (FMath::Abs(Denominator) > 1.e-10 * LengthA * LengthB)
            {
                const FVector2D Offset = RB.A - RA.A;
                const double T = Cross(Offset, DB) / Denominator;
                const double U = Cross(Offset, DA) / Denominator;
                const double TA = JunctionTolerance / FMath::Max(LengthA, 1.0);
                const double TB = JunctionTolerance / FMath::Max(LengthB, 1.0);
                if (T >= -TA && T <= 1.0 + TA && U >= -TB && U <= 1.0 + TB)
                {
                    const int32 Node = NodeAt(RA.A + DA * FMath::Clamp(T, 0.0, 1.0));
                    AddCut(A, T, Node);
                    AddCut(B, U, Node);
                }
            }
            else
            {
                // Collinear overlaps need cuts at both overlap ends, not just endpoint merging.
                EndpointOnSegment(A, 0.0, B);
                EndpointOnSegment(A, 1.0, B);
                EndpointOnSegment(B, 0.0, A);
                EndpointOnSegment(B, 1.0, A);
            }
        }

        void AttachDriveway(int32 Id, double T, const TArray<int32>& Nearby)
        {
            const LWGen::FRoad& Road = Segments[Id].Road;
            if (Road.Width > 400.0f)
            {
                return;
            }
            const FVector2D P = T == 0.0 ? Road.A : Road.B;
            double Best = DrivewayReach * DrivewayReach;
            double BestT = 0.0;
            FVector2D BestPoint = P;
            int32 BestRoad = INDEX_NONE;
            for (int32 Other : Nearby)
            {
                if (Segments[Other].Road.Width <= Road.Width)
                {
                    continue;
                }
                FVector2D Q;
                const double U = Project(P, Segments[Other].Road, Q);
                const double Distance = (P - Q).SizeSquared();
                if (Distance < Best)
                {
                    Best = Distance;
                    BestRoad = Other;
                    BestT = U;
                    BestPoint = Q;
                }
            }
            if (BestRoad != INDEX_NONE)
            {
                const int32 From = NodeAt(P), To = NodeAt(BestPoint);
                AddCut(Id, T, From);
                AddCut(BestRoad, BestT, To);
                Link(From, To);
            }
        }

        void SplitJunctions()
        {
            for (int32 A = 0; A < Segments.Num(); ++A)
            {
                TArray<int32> Nearby;
                Candidates(Segments[A].Road, Nearby);
                for (int32 B : Nearby)
                {
                    if (B > A)
                    {
                        Intersect(A, B);
                    }
                }
                AttachDriveway(A, 0.0, Nearby);
                AttachDriveway(A, 1.0, Nearby);
            }
        }

        int32 ProjectOntoRoads(FVector2D P)
        {
            double Best = TNumericLimits<double>::Max();
            double BestT = 0.0;
            FVector2D BestPoint = P;
            int32 BestRoad = INDEX_NONE;
            for (int32 Id = 0; Id < Segments.Num(); ++Id)
            {
                FVector2D Q;
                const double T = Project(P, Segments[Id].Road, Q);
                const double Distance = (P - Q).SizeSquared();
                if (Distance < Best)
                {
                    Best = Distance;
                    BestT = T;
                    BestPoint = Q;
                    BestRoad = Id;
                }
            }
            if (BestRoad == INDEX_NONE)
            {
                return INDEX_NONE;
            }
            const int32 Node = NodeAt(BestPoint);
            AddCut(BestRoad, BestT, Node);
            return Node;
        }

        void BuildEdges()
        {
            for (FSegment& Segment : Segments)
            {
                Segment.Cuts.Sort([](const FCut& A, const FCut& B)
                {
                    return A.T == B.T ? A.Node < B.Node : A.T < B.T;
                });
                for (int32 I = 1; I < Segment.Cuts.Num(); ++I)
                {
                    Link(Segment.Cuts[I - 1].Node, Segment.Cuts[I].Node);
                }
            }
        }

        bool Search(int32 Start, int32 Goal, TArray<int32>& ReversePath) const
        {
            struct FOpenEntry { int32 Node; double Cost, Estimate; };
            struct FLeastCostFirst
            {
                bool operator()(const FOpenEntry& A, const FOpenEntry& B) const
                {
                    return A.Estimate == B.Estimate ? A.Node > B.Node : A.Estimate > B.Estimate;
                }
            };
            std::priority_queue<FOpenEntry, std::vector<FOpenEntry>, FLeastCostFirst> Open;
            TArray<double> Costs;
            TArray<int32> Parents;
            Costs.Init(TNumericLimits<double>::Max(), Nodes.Num());
            Parents.Init(INDEX_NONE, Nodes.Num());
            Costs[Start] = 0.0;
            Open.push({Start, 0.0, (Nodes[Start].Position - Nodes[Goal].Position).Size()});
            while (!Open.empty())
            {
                const FOpenEntry Current = Open.top();
                Open.pop();
                if (Current.Cost > Costs[Current.Node])
                {
                    continue;
                }
                if (Current.Node == Goal)
                {
                    for (int32 Node = Goal; Node != INDEX_NONE; Node = Parents[Node])
                    {
                        ReversePath.Add(Node);
                    }
                    return true;
                }
                for (const FArc& Arc : Nodes[Current.Node].Edges)
                {
                    const double Cost = Current.Cost + Arc.Cost;
                    if (Cost < Costs[Arc.To])
                    {
                        Costs[Arc.To] = Cost;
                        Parents[Arc.To] = Current.Node;
                        const double Remaining = (Nodes[Arc.To].Position - Nodes[Goal].Position).Size();
                        Open.push({Arc.To, Cost, Cost + Remaining});
                    }
                }
            }
            return false;
        }
    };
}

TArray<FVector2D> FindPath(FVector2D Start, FVector2D Goal, int32 Seed, bool* Complete)
{
    if (Complete)
    {
        *Complete = false;
    }
    FIntPoint StartRegion, GoalRegion;
    if (!RegionAt(Start, StartRegion) || !RegionAt(Goal, GoalRegion))
    {
        return {};
    }
    if (Start == Goal)
    {
        if (Complete)
        {
            *Complete = true;
        }
        return {Start};
    }

    FVector2D Target;
    FRegionBounds Bounds(StartRegion, StartRegion);
    const bool NewYork=true;
    bool ReachesGoal;
    if(NewYork){Target=Goal;ReachesGoal=true;Bounds.Min=FIntPoint(-30,-30);Bounds.Max=FIntPoint(26,70);}
    else ReachesGoal=SelectTarget(Start,Goal,StartRegion,GoalRegion,Seed,Target,Bounds);
    if ((!NewYork&&!Bounds.Fits()) || (!ReachesGoal && Target == Start))
    {
        return {};
    }
    // Cache only immutable topology. Query projections must never accumulate in it.
    // Bound memory to four corridors; changing the seed or generation settings invalidates a hit.
    struct FCorridor {
        FIntPoint Min, Max; int32 Seed, Towns, Parcels; float Ruggedness;
        uint64 Used; FGraph Graph;
    };
    static TArray<FCorridor> Corridors;
    static uint64 Clock = 0;
    ++Clock;
    FCorridor* Cached = Corridors.FindByPredicate([&](const FCorridor& C) {
        return C.Min==Bounds.Min && C.Max==Bounds.Max && C.Seed==Seed &&
            C.Towns==LWGen::TownDensity && C.Parcels==LWGen::ParcelLevel && C.Ruggedness==LWGen::Ruggedness;
    });
    if (!Cached) {
        FGraph Base(FVector2D(Bounds.Min.X*LWGen::RegionSize,Bounds.Min.Y*LWGen::RegionSize));
        Base.Gather(Bounds,Seed,NewYork); Base.SplitJunctions();
        if(Corridors.Num()>=4) {
            int32 Oldest=0; for(int32 I=1;I<Corridors.Num();++I)if(Corridors[I].Used<Corridors[Oldest].Used)Oldest=I;
            Corridors.RemoveAt(Oldest);
        }
        Corridors.Add({Bounds.Min,Bounds.Max,Seed,LWGen::TownDensity,LWGen::ParcelLevel,LWGen::Ruggedness,Clock,MoveTemp(Base)});
        Cached=&Corridors.Last();
    }
    Cached->Used=Clock;
    FGraph Graph=Cached->Graph;
    const int32 StartNode = Graph.ProjectOntoRoads(Start);
    const int32 GoalNode = Graph.ProjectOntoRoads(Target);
    if (StartNode == INDEX_NONE || GoalNode == INDEX_NONE)
    {
        return {};
    }
    // Query projections also split their segments, including when both lie on one road.
    Graph.BuildEdges();
    TArray<int32> ReversePath;
    if (!Graph.Search(StartNode, GoalNode, ReversePath))
    {
        return {};
    }
    TArray<FVector2D> Path;
    Path.Reserve(ReversePath.Num() + 2);
    Path.Add(Start);
    for (int32 I = ReversePath.Num() - 1; I >= 0; --I)
    {
        const FVector2D P = Graph.Nodes[ReversePath[I]].Position;
        if (P != Path.Last())
        {
            Path.Add(P);
        }
    }
    if (Target != Path.Last())
    {
        Path.Add(Target);
    }
    if (Complete)
    {
        *Complete = ReachesGoal;
    }
    return Path;
}
}
