#include "LWSettlement82.h"
namespace LWBuilding82 {
const TArray<FLWBuild82>& Catalog(){static TArray<FLWBuild82> C=[](){TArray<FLWBuild82> A;
 auto Add=[&](const TCHAR* Id,const TCHAR* Name,int Group,int Scrap,const TCHAR* Model,const TCHAR* Use,FVector Size){A.Add({Id,Model,Use,Name,Group,Scrap,Size});};
 Add(TEXT("foundation"),TEXT("Concrete foundation"),0,16,TEXT("Cube"),TEXT("foundation"),{400,400,120});
 Add(TEXT("floor"),TEXT("Upper floor"),0,10,TEXT("Cube"),TEXT("floor"),{400,400,20});
 Add(TEXT("wall"),TEXT("Solid wall"),0,12,TEXT("Cube"),TEXT("wall"),{400,20,300});
 Add(TEXT("door"),TEXT("Doorway + hinged door"),0,18,TEXT("Cube"),TEXT("door"),{400,20,300});
 Add(TEXT("window"),TEXT("Window wall + breakable glass"),0,16,TEXT("Cube"),TEXT("window"),{400,20,300});
 Add(TEXT("roof"),TEXT("Flat roof / ceiling"),0,12,TEXT("Cube"),TEXT("roof"),{400,400,20});
 Add(TEXT("stairs"),TEXT("Staircase"),0,22,TEXT("Cube"),TEXT("stairs"),{160,400,320});
 Add(TEXT("porch"),TEXT("Porch foundation"),0,14,TEXT("Cube"),TEXT("foundation"),{400,400,120});
 Add(TEXT("fence"),TEXT("Garden fence"),0,8,TEXT("Cube"),TEXT("fence"),{400,15,100});
 Add(TEXT("ramp"),TEXT("Entrance ramp"),0,10,TEXT("Cube"),TEXT("ramp"),{160,240,120});
 Add(TEXT("railing"),TEXT("Balcony railing"),0,7,TEXT("Cube"),TEXT("fence"),{400,15,100});
 auto Furniture=[&](const TCHAR* Id,const TCHAR* N,int Cost,const TCHAR* M,const TCHAR* U,FVector S){Add(Id,N,1,Cost,M,U,S);};
 Furniture(TEXT("bed"),TEXT("Single bed"),18,TEXT("HomeBedV13"),TEXT("bed"),{110,210,100});
 Furniture(TEXT("bunk"),TEXT("Guest bed"),16,TEXT("MotelBedV3"),TEXT("bed"),{110,210,100});
 Furniture(TEXT("chair"),TEXT("Dining chair"),5,TEXT("Chair65"),TEXT("chair"),{60,60,95});
 Furniture(TEXT("sofa"),TEXT("Sofa"),18,TEXT("Sofa65"),TEXT("chair"),{200,85,90});
 Furniture(TEXT("armchair"),TEXT("Armchair"),10,TEXT("Armchair65"),TEXT("chair"),{90,90,95});
 Furniture(TEXT("table"),TEXT("Dining table"),12,TEXT("DiningTable65"),TEXT("decor"),{160,90,78});
 Furniture(TEXT("coffee_table"),TEXT("Coffee table"),6,TEXT("CoffeeTable65"),TEXT("decor"),{110,65,45});
 Furniture(TEXT("desk"),TEXT("Desk"),14,TEXT("Desk65"),TEXT("locker"),{150,75,85});
 Furniture(TEXT("locker"),TEXT("Personal locker"),15,TEXT("Locker65"),TEXT("locker"),{70,65,185});
 Furniture(TEXT("shelf"),TEXT("Storage shelves"),12,TEXT("Shelf65"),TEXT("locker"),{140,45,190});
 Furniture(TEXT("cabinet"),TEXT("Cabinet"),12,TEXT("Cabinet65"),TEXT("locker"),{110,55,90});
 Furniture(TEXT("crate"),TEXT("Storage crate"),8,TEXT("Crate65"),TEXT("locker"),{80,75,70});
 Furniture(TEXT("nightstand"),TEXT("Bedside drawer"),7,TEXT("Nightstand65"),TEXT("locker"),{55,50,60});
 Furniture(TEXT("bookcase"),TEXT("Bookcase"),12,TEXT("Bookcase65"),TEXT("locker"),{100,35,185});
 Furniture(TEXT("wardrobe"),TEXT("Wardrobe"),16,TEXT("PantryV13"),TEXT("locker"),{120,60,190});
 auto Utility=[&](const TCHAR* Id,const TCHAR* N,int Cost,const TCHAR* M,const TCHAR* U,FVector S){Add(Id,N,2,Cost,M,U,S);};
 Utility(TEXT("ham"),TEXT("HAM recruitment radio"),45,TEXT("RadioV4"),TEXT("ham"),{120,70,125});
 Utility(TEXT("workbench"),TEXT("Weapon workbench"),35,TEXT("WeaponBench39"),TEXT("workbench"),{180,85,110});
 Utility(TEXT("cooker"),TEXT("Stove"),25,TEXT("Stove65"),TEXT("cooker"),{80,70,90});
 Utility(TEXT("sink"),TEXT("Water filter + sink"),25,TEXT("Sink65"),TEXT("sink"),{100,65,90});
 Utility(TEXT("fridge"),TEXT("Refrigerator"),20,TEXT("Fridge65"),TEXT("locker"),{85,75,185});
 Utility(TEXT("supplies"),TEXT("Shared supplies cabinet"),20,TEXT("ToolShelf65"),TEXT("supplies"),{150,60,180});
 Utility(TEXT("lamp"),TEXT("Standing lamp"),8,TEXT("TableLampV13"),TEXT("light"),{50,50,100});
 Utility(TEXT("ceiling_light"),TEXT("Ceiling pendant"),8,TEXT("Pendant65"),TEXT("ceilinglight"),{65,65,50});
 Utility(TEXT("generator"),TEXT("Generator"),35,TEXT("Generator"),TEXT("decor"),{150,95,120});
 auto Job=[&](const TCHAR* Id,const TCHAR* N,int Cost,const TCHAR* M,FVector S){Add(Id,N,3,Cost,M,Id,S);};
 Job(TEXT("farm"),TEXT("Farm plot"),28,TEXT("Cube"),{240,180,65});
 Job(TEXT("scavenge"),TEXT("Scrap sorting station"),35,TEXT("ToolShelf65"),{150,80,185});
 Job(TEXT("recruit"),TEXT("Recruitment desk"),40,TEXT("Workstation65"),{160,80,140});
 Job(TEXT("sell"),TEXT("Trading counter"),45,TEXT("Counter65"),{180,90,110});
 Job(TEXT("contracts"),TEXT("Contract office"),40,TEXT("Desk65"),{160,85,110});
 Job(TEXT("loot"),TEXT("Expedition staging station"),45,TEXT("ToolTrolley65"),{130,80,110});
 Job(TEXT("medical"),TEXT("Medical station"),45,TEXT("ExamCart65"),{140,80,120});
 Job(TEXT("water"),TEXT("Water purification station"),30,TEXT("Sink65"),{140,85,110});
 Job(TEXT("guard"),TEXT("Guard post"),25,TEXT("Workstation65"),{160,80,140});
 Add(TEXT("parking"),TEXT("Insured parking lot / 10 bays"),4,180,TEXT("Cube"),TEXT("parking"),{4000,2600,32});
 auto Decor=[&](const TCHAR* Id,const TCHAR* N,int Cost,const TCHAR* M,FVector S,const TCHAR* U=TEXT("decor")){Add(Id,N,5,Cost,M,U,S);};
 Decor(TEXT("rug"),TEXT("Woven rug"),4,TEXT("Rug65"),{180,130,2});
 Decor(TEXT("mat"),TEXT("Doormat"),2,TEXT("Mat65"),{80,55,2});
 Decor(TEXT("planter"),TEXT("Potted plant"),6,TEXT("Planter65"),{55,55,110});
 Decor(TEXT("books"),TEXT("Books"),3,TEXT("Books65"),{50,25,25});
 Decor(TEXT("bottles"),TEXT("Kitchen bottles"),2,TEXT("Bottles65"),{40,20,35});
 Decor(TEXT("papers"),TEXT("Paperwork"),2,TEXT("Papers65"),{45,30,3});
 Decor(TEXT("bin"),TEXT("Waste bin"),4,TEXT("WasteBin65"),{45,45,65});
 Decor(TEXT("coat_rack"),TEXT("Coat rack"),5,TEXT("CoatRack65"),{55,55,170});
 Decor(TEXT("microwave"),TEXT("Microwave"),8,TEXT("Microwave65"),{60,45,38});
 Decor(TEXT("coffee"),TEXT("Coffee station"),7,TEXT("CoffeeStation65"),{70,45,65});
 Decor(TEXT("art"),TEXT("Framed artwork"),5,TEXT("Artwork65"),{100,8,75},TEXT("walldecor"));
 Decor(TEXT("clock"),TEXT("Wall clock"),4,TEXT("Clock65"),{45,8,45},TEXT("walldecor"));
 Decor(TEXT("noticeboard"),TEXT("Noticeboard"),6,TEXT("Noticeboard65"),{120,10,80},TEXT("walldecor"));
 Decor(TEXT("radiator"),TEXT("Radiator"),6,TEXT("Radiator65"),{100,20,65},TEXT("walldecor"));
 Decor(TEXT("extinguisher"),TEXT("Fire extinguisher"),5,TEXT("Extinguisher65"),{25,20,60},TEXT("walldecor"));
 return A;}();return C;}
const FLWBuild82* Find(FName Id){return Catalog().FindByPredicate([&](const auto& D){return D.Id==Id;});}
FName Finish(int I){static const FName N[]={TEXT("Brick"),TEXT("Plaster65"),TEXT("WallpaperV7"),TEXT("Wood"),TEXT("TileV7"),TEXT("Steel"),TEXT("Concrete")};return N[FMath::Clamp(I,0,6)];}
const TCHAR* FinishName(int I){static const TCHAR* N[]={TEXT("Brick"),TEXT("Painted plaster"),TEXT("Wallpaper"),TEXT("Wood paneling"),TEXT("Tile"),TEXT("Sheet metal"),TEXT("Concrete")};return N[FMath::Clamp(I,0,6)];}
FLinearColor Color(int I){static const FLinearColor C[]={FLinearColor(1,1,1),FLinearColor(.72,.30,.22),FLinearColor(.25,.42,.58),FLinearColor(.32,.48,.31),FLinearColor(.72,.61,.38),FLinearColor(.22,.24,.27),FLinearColor(.62,.42,.59),FLinearColor(.88,.83,.70)};return C[FMath::Clamp(I,0,7)];}
const TCHAR* ColorName(int I){static const TCHAR* C[]={TEXT("Natural"),TEXT("Clay"),TEXT("Blue"),TEXT("Sage"),TEXT("Ochre"),TEXT("Charcoal"),TEXT("Mauve"),TEXT("Ivory")};return C[FMath::Clamp(I,0,7)];}
FVector Extent(const FLWBuild82& D){return D.Size*.5;}
FVector Offset(const FLWBuild82& D){return FVector(0,0,D.Deck()||D.Use==TEXT("parking")?-D.Size.Z*.5:D.Size.Z*.5);}
bool Overlap(const FTransform& A,FVector HA,const FTransform& B,FVector HB,double M){
 if(FMath::Abs(A.GetLocation().Z-B.GetLocation().Z)>=HA.Z+HB.Z-M)return false;
 const FVector2D AX(A.GetUnitAxis(EAxis::X)),AY(A.GetUnitAxis(EAxis::Y)),BX(B.GetUnitAxis(EAxis::X)),BY(B.GetUnitAxis(EAxis::Y)),Delta(B.GetLocation()-A.GetLocation());
 for(auto N:{AX,AY,BX,BY}){double RA=HA.X*FMath::Abs(FVector2D::DotProduct(N,AX))+HA.Y*FMath::Abs(FVector2D::DotProduct(N,AY));double RB=HB.X*FMath::Abs(FVector2D::DotProduct(N,BX))+HB.Y*FMath::Abs(FVector2D::DotProduct(N,BY));if(FMath::Abs(FVector2D::DotProduct(Delta,N))>=RA+RB-M)return false;}return true;
}
int Beds(const FLWClaim82& C){int N=0;for(const auto& P:C.Pieces)if(const auto* D=Find(P.Catalog))N+=D->Use==TEXT("bed");return N;}
bool DependsOn(const FLWClaim82& C,FName Id){return C.Pieces.ContainsByPredicate([&](const auto& P){return P.Support==Id;});}
FTransform RoadSnap(FVector Aim,const LWGen::FRoad& R,FVector Size){FVector2D Q;LWGen::DistanceToSegment(FVector2D(Aim),R,&Q);FVector2D T=(R.B-R.A).GetSafeNormal(),N(-T.Y,T.X);if(FVector2D::DotProduct(FVector2D(Aim)-Q,N)<0)N=-N;return FTransform(FRotator(0,FMath::RadiansToDegrees(FMath::Atan2(T.Y,T.X)),0),FVector(Q+N*(R.Width*.5+Size.Y*.5+120),Aim.Z));}
}
