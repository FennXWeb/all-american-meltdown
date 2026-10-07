#include "LWNewYork69.h"
#include "LWGeneration.h"
#include "LWNavigation.h"
#include "LWStreaming68.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNY69Atlas,"LethalWorld.Update69.Atlas",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWNY69Atlas::RunTest(const FString&){
 const auto& Sites=LWNY69::Sites();const auto& Roads=LWNY69::Roads();
 TestEqual(TEXT("Western and northern New York town coverage"),LWNY69::Towns().Num(),36);
 TestTrue(TEXT("Substantial fixed building catalogue"),Sites.Num()>400);
 TMap<FIntPoint,TSet<uint32>> Visible;TMap<FIntPoint,TArray<LWGen::FRoad>> LocalRoads;
 TSet<uint32> IDs;int Stadium=0,Mall=0,Overlap=0;TSet<int> Types;
 for(const auto& S:Sites){TestFalse(TEXT("Unique persistent site ID"),IDs.Contains(S.Id));IDs.Add(S.Id);Types.Add(S.Type);Stadium+=S.Type==68;Mall+=S.Type==69;
  FIntPoint K(LWGen::FloorDiv(S.Position.X+25600,51200),LWGen::FloorDiv(S.Position.Y+25600,51200));
  if(!Visible.Contains(K)){TArray<LWGen::FRoad> R;TArray<LWGen::FSite> A,B;LWGen::Gather(S.Position,17,R,A);LocalRoads.Add(K,R);R.Empty();LWGen::Gather(S.Position,89731,R,B);TestEqual(TEXT("Seed cannot move towns or buildings"),A.Num(),B.Num());auto& Set=Visible.Add(K);TSet<uint32> Other;for(const auto& V:B)Other.Add(V.Id);for(const auto& V:A){Set.Add(V.Id);TestTrue(TEXT("Both seeds retain the same site identities"),Other.Contains(V.Id));}}
  if(LocalRoads[K].ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);})){Overlap++;AddError(FString::Printf(TEXT("Road footprint conflict: %s [%u] at %.0f, %.0f"),*S.PlaceName69,S.Id,S.Position.X,S.Position.Y));}
  TestTrue(FString::Printf(TEXT("Catalogue site survives runtime filtering: %s [%u]"),*S.PlaceName69,S.Id),Visible[K].Contains(S.Id));
 }
 TestEqual(TEXT("One Highmark Stadium"),Stadium,1);TestEqual(TEXT("One Destiny USA"),Mall,1);TestEqual(TEXT("Roads avoid every developed footprint"),Overlap,0);
 for(int I=22;I<53;I++)TestTrue(TEXT("Existing major destinations retained"),Types.Contains(I));
 TestTrue(TEXT("Syracuse is east of Rochester"),LWNY69::Project(43.0481,-76.1474).Y>LWNY69::Project(43.1566,-77.6088).Y);
 for(int Type:{68,69}){const auto* S=Sites.FindByPredicate([&](const auto& P){return P.Type==Type;});if(!S)continue;auto A=LWStreaming68::Plan(LWGen::ChunkAt(S->Position),17,43,1,1);auto B=LWStreaming68::Plan(LWGen::ChunkAt(S->Position)+FIntPoint(1,0),17,43,1,1);for(int Y=0;Y<=24;Y++)TestEqual(TEXT("Landmark terrain seams"),A->Vertices[Y*25+24].Z,B->Vertices[Y*25].Z);}
 UE_LOG(LogTemp,Display,TEXT("NY69 atlas: %d roads, %d sites, %d footprint conflicts"),Roads.Num(),Sites.Num(),Overlap);
 TArray<FString> Lines;for(const auto& R:Roads)Lines.Add(FString::Printf(TEXT("R,%.1f,%.1f,%.1f,%.1f,%.0f"),R.A.X,R.A.Y,R.B.X,R.B.Y,R.Width));for(const auto& S:Sites)Lines.Add(FString::Printf(TEXT("S,%.1f,%.1f,%.1f,%.1f,%d,%s"),S.Position.X,S.Position.Y,S.Size.X,S.Size.Y,S.Type,*S.PlaceName69));FFileHelper::SaveStringArrayToFile(Lines,*(FPaths::ProjectSavedDir()/TEXT("NY69Atlas.csv")));
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWNY69Routes,"LethalWorld.Update69.Routes",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWNY69Routes::RunTest(const FString&){
 for(int I=1;I<LWNY69::Towns().Num();I++){const auto& T=LWNY69::Towns()[I];bool Complete=false;auto Path=LWNavigation::FindPath(LWNY69::Project(42.8864,-78.8784),LWNY69::Project(T.Latitude,T.Longitude),17,&Complete);TestTrue(FString::Printf(TEXT("Connected complete route Buffalo to %s"),T.Name),Path.Num()>2&&Complete);}
 return true;
}
#endif
