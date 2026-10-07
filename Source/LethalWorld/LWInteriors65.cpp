#include "LWStreaming68.h"
#include "LWInteriors65.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWZombie.h"
#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "HAL/PlatformTime.h"

namespace LWInteriors65 {
static FScope* Active=nullptr;
void RecordStep68(FScope& Scope,TFunctionRef<void()> Work){FScope* Previous=Active;Active=&Scope;Work();Active=Previous;}
void Warm(ALWWorld* W){
 const TArray<FName> Names={TEXT("Artwork65"),TEXT("Booth65"),TEXT("CafeTable65"),TEXT("Desk65"),TEXT("Desktop65"),TEXT("Workstation65"),TEXT("Chair65"),TEXT("Sofa65"),TEXT("Armchair65"),TEXT("CoffeeTable65"),TEXT("DiningTable65"),TEXT("Bed65"),TEXT("Nightstand65"),TEXT("Sideboard65"),TEXT("Cabinet65"),TEXT("Bookcase65"),TEXT("Shelf65"),TEXT("ToolShelf65"),TEXT("Locker65"),TEXT("Counter65"),TEXT("Fridge65"),TEXT("Stove65"),TEXT("Sink65"),TEXT("Toilet65"),TEXT("WallCabinet65"),TEXT("Washer65"),TEXT("Dryer65"),TEXT("ServerRack65"),TEXT("ExamCart65"),TEXT("ToolTrolley65"),TEXT("CleaningCart65"),TEXT("LuggageCart65"),TEXT("WasteBin65"),TEXT("Planter65"),TEXT("Extinguisher65"),TEXT("Radiator65"),TEXT("Noticeboard65"),TEXT("Frame65"),TEXT("Mirror65"),TEXT("Clock65"),TEXT("ExitLight65"),TEXT("Vent65"),TEXT("CeilingTile65"),TEXT("Pendant65"),TEXT("TubeLight65"),TEXT("Duct65"),TEXT("Pipe65"),TEXT("CableTray65"),TEXT("Rug65"),TEXT("Mat65"),TEXT("Books65"),TEXT("Bottles65"),TEXT("Papers65"),TEXT("Crate65"),TEXT("CoatRack65"),TEXT("Microwave65"),TEXT("CoffeeStation65"),TEXT("CeilingFan65"),TEXT("Pallet65"),TEXT("ClothesRail65"),TEXT("WaitingBench65")};TArray<FSoftObjectPath> Paths;
 for(FName N:Names)Paths.Add(FSoftObjectPath(FString::Printf(TEXT("/Game/Art/Interiors65/SM_%s.SM_%s"),*N.ToString(),*N.ToString())));
 for(int I=0;I<16;I++)Paths.Add(FSoftObjectPath(FString::Printf(TEXT("/Game/Art/Interiors65/M_Art%02d_65.M_Art%02d_65"),I,I)));
 TWeakObjectPtr<ALWWorld> Weak=W;
 UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths,FStreamableDelegate::CreateLambda([Weak,Names](){if(auto* World=Weak.Get())for(FName N:Names)World->Mesh(N);}));
}
TArray<FReport> Reports;
FName Surface(FName N){if(!Active)return N;if(N==TEXT("PlasterV7")||N==TEXT("WallpaperV7"))return TEXT("Plaster65");if(N==TEXT("TileV7")&&Active->S.Type!=3&&Active->S.Type!=13)return TEXT("Terrazzo65");return N;}
FName Replacement(FName N){
 if(!Active)return N;
 static const TMap<FName,FName> Map={{TEXT("StoveV4"),TEXT("Stove65")},{TEXT("FridgeV4"),TEXT("Fridge65")},{TEXT("SinkV4"),TEXT("Sink65")},{TEXT("ToiletV4"),TEXT("Toilet65")},{TEXT("HomeBedV13"),TEXT("Bed65")},{TEXT("MotelBedV3"),TEXT("Bed65")},{TEXT("DinerBoothV9"),TEXT("Booth65")},{TEXT("DinerTableV9"),TEXT("CafeTable65")},{TEXT("Desk"),TEXT("Desk65")},{TEXT("ChairV3"),TEXT("Chair65")},{TEXT("SofaV13"),TEXT("Sofa65")},{TEXT("CoffeeTableV13"),TEXT("CoffeeTable65")},{TEXT("DiningTableV13"),TEXT("DiningTable65")},{TEXT("NightstandV13"),TEXT("Nightstand65")},{TEXT("SideboardV13"),TEXT("Sideboard65")},{TEXT("BookcaseV13"),TEXT("Bookcase65")},{TEXT("CabinetV3"),TEXT("Cabinet65")},{TEXT("LockerV4"),TEXT("Locker65")},{TEXT("WasteBinV13"),TEXT("WasteBin65")},{TEXT("RadiatorV13"),TEXT("Radiator65")},{TEXT("DeskSetV13"),TEXT("Desktop65")}};
 if(const auto* R=Map.Find(N))return *R;return N;
}
FScope::FScope(ALWChunk* Chunk,ALWWorld* World,const LWGen::FSite& Site,bool Capture):C(Chunk),W(World),S(Site){Capturing68=Capture;Previous=Active;if(Capture)Active=Previous&&Previous->C==C?Previous:this;Frame=FTransform(FRotator(0,S.Yaw,0),FVector(S.Position,0));StartResident=C->Residents.Num();WasSurfaces=C->BuildingSurfaces;C->BuildingSurfaces=true;Report.Type=S.Type;}
FScope::~FScope(){
 if(!Capturing68)return;
 if(C->Plan68&&!Parts.IsEmpty()){
  auto Work=MakeShared<FScope>(C,W,S,false);Work->Parts=MoveTemp(Parts);Work->StartResident=StartResident;Work->EndResident68=C->Residents.Num();
  C->Plan68->Furnishing.Add([Work](){return Work->Step68();});
 }else Finish();
 C->BuildingSurfaces=WasSurfaces;if(!WasSurfaces)C->FlushSurfaces();Active=Previous;
}
FBox FScope::LocalBounds(const FBox& B,const FTransform& T)const{FBox Out(ForceInit);for(int I=0;I<8;I++)Out+=Frame.InverseTransformPosition(T.TransformPosition(FVector(I&1?B.Max.X:B.Min.X,I&2?B.Max.Y:B.Min.Y,I&4?B.Max.Z:B.Min.Z)));return Out;}
void FScope::Record(FPart P){Parts.Add(MoveTemp(P));}
void CaptureBox(ALWChunk* C,FName Mat,FVector P,FVector Size,FRotator R,bool Solid){
 if(!Active||Active->C!=C||Active->Finishing)return;auto& A=*Active;
 auto B=A.LocalBounds(FBox(-Size*.5,Size*.5),FTransform(R,C->GetActorLocation()+P));const bool Flat=Size.Z<=80&&Size.X>=150&&Size.Y>=150&&FMath::Abs(R.Pitch)<.1&&FMath::Abs(R.Roll)<.1;
 A.Record({B,Mat,Flat&&Solid,Solid&&B.GetSize().Z>=220&&FMath::Min(B.GetSize().X,B.GetSize().Y)<65,false});
}
void CaptureMesh(ALWChunk* C,ALWWorld* W,FName N,FVector P,FVector Scale,FRotator R,bool Solid){
 if(!Active||Active->C!=C||Active->Finishing||N==TEXT("Cube"))return;
 if(auto* M=W->Mesh(N))Active->Record({Active->LocalBounds(M->GetBoundingBox(),FTransform(R,P+C->GetActorLocation(),Scale)),N,false,false,false});
}
FName SupportedModel(ALWChunk* C,FName N,FVector P){
 if(!Active||Active->C!=C||N!=TEXT("DeskSetV13"))return N;
 const auto Q=Active->Frame.InverseTransformPosition(C->GetActorLocation()+P);
 for(const auto& E:Active->Parts)if(E.Slab&&Q.X>=E.Bounds.Min.X&&Q.X<=E.Bounds.Max.X&&Q.Y>=E.Bounds.Min.Y&&Q.Y<=E.Bounds.Max.Y&&FMath::Abs(Q.Z-E.Bounds.Max.Z)<22)return TEXT("Workstation65");
 return N;
}
static FIntVector Cell(FVector V){return FIntVector(FMath::FloorToInt(V.X/400),FMath::FloorToInt(V.Y/400),FMath::FloorToInt(V.Z/400));}
void FScope::Reindex(){Index.Empty();Large.Empty();for(int I=0;I<Parts.Num();I++){const auto A=Cell(Parts[I].Bounds.Min),B=Cell(Parts[I].Bounds.Max);if(int64(B.X-A.X+1)*(B.Y-A.Y+1)*(B.Z-A.Z+1)>12000){Large.Add(I);continue;}for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)for(int Z=A.Z;Z<=B.Z;Z++)Index.FindOrAdd(FIntVector(X,Y,Z)).Add(I);}}
TArray<int> FScope::Query(const FBox& Box)const{TSet<int> Found;for(int I:Large)if(Parts[I].Bounds.Intersect(Box))Found.Add(I);const auto A=Cell(Box.Min),B=Cell(Box.Max);for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)for(int Z=A.Z;Z<=B.Z;Z++)if(auto* List=Index.Find(FIntVector(X,Y,Z)))for(int I:*List)if(Parts[I].Bounds.Intersect(Box))Found.Add(I);return Found.Array();}
bool FScope::Free(const FBox& B,int Ignore)const{for(int I:Query(B))if(I!=Ignore&&Parts[I].Bounds.Intersect(B))return false;return true;}
bool FScope::Place(FName Name,FVector P,float Yaw,FName Interaction,bool Ground,int Ignore){
 auto* M=W->Mesh(Name);if(!M)return false;auto MB=M->GetBoundingBox();if(Ground)P.Z-=MB.Min.Z;
 const FTransform T(FRotator(0,Yaw,0),P);FBox Bounds=MB.TransformBy(T);Bounds.Min.Z+=.8;Bounds.Max.Z-=.3;
 if(!Ground&&Parts.IsValidIndex(Ignore)){
  const auto& Support=Parts[Ignore];const auto& B=Support.Bounds;bool Fits=true;
  if(Support.Slab)Fits=Bounds.Min.X>=B.Min.X&&Bounds.Max.X<=B.Max.X&&Bounds.Min.Y>=B.Min.Y&&Bounds.Max.Y<=B.Max.Y;
  if(Support.Wall){Fits=Bounds.Min.Z>=B.Min.Z&&Bounds.Max.Z<=B.Max.Z;
   if(B.GetSize().X<B.GetSize().Y)Fits&=Bounds.Min.Y>=B.Min.Y&&Bounds.Max.Y<=B.Max.Y;
   else Fits&=Bounds.Min.X>=B.Min.X&&Bounds.Max.X<=B.Max.X;
  }
  if(!Fits){Report.Rejected++;return false;}
 }
 // Every corner and the centre need support. A clear bounding box alone does not
 // prevent a prop overhanging a stair aperture or the edge of a mezzanine.
 if(Ground){for(int K=0;K<5;K++){const FVector Q=K==4?Bounds.GetCenter():FVector(K&1?Bounds.Max.X:Bounds.Min.X,K&2?Bounds.Max.Y:Bounds.Min.Y,0);bool Supported=false;const float Foot=Bounds.Min.Z-.8f;
  for(int J:Query(FBox(FVector(Q.X-.1,Q.Y-.1,Foot-4),FVector(Q.X+.1,Q.Y+.1,Foot+1)))){const auto& E=Parts[J];if(E.Slab&&FMath::Abs(E.Bounds.Max.Z-Foot)<4&&Q.X>=E.Bounds.Min.X&&Q.X<=E.Bounds.Max.X&&Q.Y>=E.Bounds.Min.Y&&Q.Y<=E.Bounds.Max.Y){Supported=true;break;}}
  if(!Supported){Report.Rejected++;return false;}
 }}
 if(!Free(Bounds,Ignore)){Report.Rejected++;return false;}
 const FVector World=Frame.TransformPosition(P);const FRotator Rot=FRotator(0,S.Yaw+Yaw,0);
 if(!Interaction.IsNone()){
  const FName Key(*FString::Printf(TEXT("interior65_%u_%d_%d_%d_%s"),S.Id,FMath::RoundToInt(P.X),FMath::RoundToInt(P.Y),FMath::RoundToInt(P.Z),*Name.ToString()));
  auto* O=W->SpawnObject(Interaction==TEXT("storage")?ELWObjectKind::Container:ELWObjectKind::Furniture,Key,World,Rot);
  if(!O){Report.Rejected++;return false;}if(O){O->SetActorTickEnabled(false);O->Body->SetCullDistance(10000);if(Interaction!=TEXT("storage"))O->SetFurniture(Interaction);O->Body->SetStaticMesh(M);O->Body->SetRelativeRotation(FRotator::ZeroRotator);if(Interaction==TEXT("storage"))W->EnsureSiteContainer(Key,S,World);C->Residents.Add(O);}
 }else C->Add(W,Name,NAME_None,World-C->GetActorLocation(),FVector(1),Rot,Ground);
 if(Name==TEXT("Frame65")){int Picture=LWGen::Hash(FMath::RoundToInt(P.X),FMath::RoundToInt(P.Y),S.Id,FMath::RoundToInt(P.Z))%16;if(S.Type==4||S.Type==14||S.Type==19)Picture=4+Picture%4;else if(S.Type==7||S.Type==8||S.Type==9||S.Type==56)Picture%=4;C->Add(W,TEXT("Artwork65"),FName(*FString::Printf(TEXT("Art%02d_65"),Picture)),World-C->GetActorLocation(),FVector(1),Rot,false);}
 const int I=Parts.Add({Bounds,Name,false,false,false});auto A=Cell(Bounds.Min),B=Cell(Bounds.Max);for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)for(int Z=A.Z;Z<=B.Z;Z++)Index.FindOrAdd(FIntVector(X,Y,Z)).Add(I);
 return true;
}
static int Theme(int T){
 if(T==1||T==6||T==7||T==8||T==9||T==56)return 0; // dwelling
 if(T==2||T==23||T==28||T==57)return 1; // medical
 if(T==3||T==13||T==22||T==27||T==29||T==30||T==53||T==54||T==55||T==64||T==66)return 2; // service/industrial
 if(T==4||T==14||T==19||T==21||T==60)return 3; // hospitality
 if(T==5||T==10||T==11||T==12||T==18||T==58||T==62||T==63||T==67)return 4; // retail
 return 5; // civic/office
}
void FScope::Finish(){while(!Step68()){} }
void FScope::Prepare68(){
 Prepared68=Finishing=true;
 // Existing interactive objects are obstacles too, including rotated meshes and door swing space.
 for(int I=StartResident;I<FMath::Min(EndResident68,C->Residents.Num());I++)if(auto* O=Cast<ALWWorldObject>(C->Residents[I])){
  if(!O->Body||!O->Body->GetStaticMesh())continue;
  FBox B=LocalBounds(O->Body->GetStaticMesh()->GetBoundingBox(),O->Body->GetComponentTransform());
  const bool Door=O->Kind==ELWObjectKind::Door;const bool Car=Cast<ALWVehicle>(O)!=nullptr;
  if(Door){B=B.ExpandBy(FVector(120,120,0));B.Min.Z-=10;B.Max.Z+=15;}
  if(Car)B=B.ExpandBy(FVector(90,90,0));
  Parts.Add({B,O->Body->GetStaticMesh()->GetFName(),false,false,Door||Car});
 }
 // Doorless arches need the same protected approach as hinged doors. Their
 // lintels describe the actual opening, even in rotated modular rooms.
 const int Built=Parts.Num();for(int I=0;I<Built;I++){
  const FBox Header=Parts[I].Bounds;const FVector Size=Header.GetSize();
  if(Size.Z<55||Size.Z>350||FMath::Min(Size.X,Size.Y)>45||FMath::Max(Size.X,Size.Y)<150||FMath::Max(Size.X,Size.Y)>450)continue;
  const FVector Mid=Header.GetCenter();float Floor=-FLT_MAX;
  for(int J=0;J<Built;J++){const auto& E=Parts[J];if(E.Slab&&Mid.X>=E.Bounds.Min.X&&Mid.X<=E.Bounds.Max.X&&Mid.Y>=E.Bounds.Min.Y&&Mid.Y<=E.Bounds.Max.Y&&Header.Min.Z-E.Bounds.Max.Z>=190&&Header.Min.Z-E.Bounds.Max.Z<430)Floor=FMath::Max(Floor,float(E.Bounds.Max.Z));}
  if(Floor>-FLT_MAX){FBox B=Header.ExpandBy(FVector(100,100,0));B.Min.Z=Floor+1;B.Max.Z=Header.Min.Z;Parts.Add({B,TEXT("doorway clearance"),false,false,true});}
 }
 for(int I=StartResident;I<FMath::Min(EndResident68,C->Residents.Num());I++)if(auto* NPC=Cast<ALWZombie>(C->Residents[I])){FVector Origin,Extent;NPC->GetActorBounds(true,Origin,Extent);const FBox B=LocalBounds(FBox(Origin-Extent,Origin+Extent),FTransform::Identity).ExpandBy(FVector(70,70,0));Parts.Add({B,TEXT("NPC clearance"),false,false,true});}
 Reindex();const int Original=Parts.Num();Report.Surfaces=Original;
 for(int I=0;I<Original;I++){
  const FPart Floor=Parts[I];if(!Floor.Slab||Floor.Bounds.GetSize().X<280||Floor.Bounds.GetSize().Y<280)continue;
  const float Z=Floor.Bounds.Max.Z;const int DeckKey=FMath::RoundToInt(Z/100);
  if(LWPlaces::IsTower(S.Type)&&Z>=FMath::Max(0,S.AccessibleFloors)*400-5)continue;
  // Samples share a site grid, so overlapping finishes cannot duplicate furnishings.
  // Spend the finite furnishing budget across the entire deck, rather than
  // filling its west edge first and leaving the rest of a large room empty.
  TArray<FIntPoint> Samples;
  for(int X=FMath::CeilToInt((Floor.Bounds.Min.X+150)/430);X*430<Floor.Bounds.Max.X-150;X++)for(int Y=FMath::CeilToInt((Floor.Bounds.Min.Y+150)/430);Y*430<Floor.Bounds.Max.Y-150;Y++)Samples.Add(FIntPoint(X,Y));
  Samples.Sort([this,DeckKey](const FIntPoint& A,const FIntPoint& B){const uint32 HA=LWGen::Hash(A.X,A.Y,S.Id,DeckKey+650),HB=LWGen::Hash(B.X,B.Y,S.Id,DeckKey+650);return HA==HB?(A.X==B.X?A.Y<B.Y:A.X<B.X):HA<HB;});
  for(const auto& Point:Samples)Samples68.Add({I,Point});
 }
}
void FScope::Decorate68(int I,FIntPoint Sample){
 const float Z=Parts[I].Bounds.Max.Z;const int DeckKey=FMath::RoundToInt(Z/100);
 const float X=Sample.X*430,Y=Sample.Y*430;
   const FIntVector Key(FMath::RoundToInt(X/430),FMath::RoundToInt(Y/430),FMath::RoundToInt(Z/35));if(Visited68.Contains(Key))return;
   if(++FloorBudget68>18000)return;
   FVector Point(X,Y,Z+1);const auto Nearby=Query(FBox(Point-FVector(1700,1700,1),Point+FVector(1700,1700,1250)));
   float Ceiling=FLT_MAX,WallDistance=FLT_MAX;int CeilingId=-1,WallId=-1;FVector WallPoint=FVector::ZeroVector,ToWall=FVector::ZeroVector;int Walls=0;
   for(int J:Nearby){if(J==I)continue;const auto& E=Parts[J];const auto& B=E.Bounds;
    if(E.Slab&&B.Min.Z>Z+210&&B.Min.Z<Ceiling&&X>B.Min.X+65&&X<B.Max.X-65&&Y>B.Min.Y+65&&Y<B.Max.Y-65){Ceiling=B.Min.Z;CeilingId=J;}
    if(E.Wall&&FMath::Max(B.GetSize().X,B.GetSize().Y)>180&&B.Min.Z<Z+90&&B.Max.Z>Z+250){FVector Q=Point;Q.Z=Z+160;Q.X=FMath::Clamp(X,B.Min.X,B.Max.X);Q.Y=FMath::Clamp(Y,B.Min.Y,B.Max.Y);const float D=FVector::Dist2D(Point,Q);if(D<1500)Walls++;if(D<WallDistance){WallDistance=D;WallPoint=Q;WallId=J;ToWall=(Q-FVector(X,Y,Q.Z)).GetSafeNormal();}}
   }
   if(CeilingId<0||Ceiling-Z>1250)return;
   // An open canopy or a stair riser is not a furnished room.
   if(Walls<2&&S.Type!=21&&S.Type!=32&&S.Type!=58)return;
   bool Covered=false;for(int J:Nearby){const auto& B=Parts[J].Bounds;if(Parts[J].Slab&&B.Max.Z>Z+1&&B.Max.Z<Z+50&&X>B.Min.X&&X<B.Max.X&&Y>B.Min.Y&&Y<B.Max.Y){Covered=true;break;}}if(Covered)return;
   Visited68.Add(Key);Report.FloorCells++;
   const uint32 H=LWGen::Hash(Key.X,Key.Y,S.Id,uint32(Key.Z)*7919+65);int Role=Theme(S.Type);
   // Use nearby room furniture as a stronger cue than the building's overall category.
   float Nearest=600;for(int J:Nearby)if(!Parts[J].Slab&&!Parts[J].Wall){const float D=FVector::Dist(Point,Parts[J].Bounds.GetCenter());if(D<Nearest){const FString N=Parts[J].Name.ToString();int R=-1;if(N.Contains(TEXT("Clinic")))R=1;else if(N.Contains(TEXT("Bed"))||N.Contains(TEXT("Sofa")))R=0;else if(N.Contains(TEXT("Kitchen"))||N.Contains(TEXT("Dining"))||N.Contains(TEXT("Diner")))R=3;else if(N.Contains(TEXT("Rack"))||N.Contains(TEXT("Shelf")))R=Role==2?2:4;else if(N.Contains(TEXT("Desk"))||N.Contains(TEXT("Dispatch")))R=5;if(R>=0){Nearest=D;Role=R;}}}
   const int ResortFloor=FMath::Max(0,FMath::FloorToInt((Z-36)/420));
   if(S.Type==21){if(ResortFloor==4||ResortFloor==5)Role=0;else if(ResortFloor==2)Role=4;else Role=3;}
   if(Report.CeilingDecor<700&&DeckCeiling68.FindRef(DeckKey)<(S.Type==21?100:700)&&(H%3==0)){
    const FName Models[]={TEXT("Vent65"),TEXT("TubeLight65"),TEXT("CeilingTile65"),TEXT("CeilingTile65")};FName N=Role==2?(H%2?TEXT("Pipe65"):TEXT("CableTray65")):Models[H%4];
    if(Place(N,FVector(X,Y,Ceiling-1),H%2?90:0,NAME_None,false,CeilingId)){Report.CeilingDecor++;DeckCeiling68.FindOrAdd(DeckKey)++;}
   }
   if(WallId>=0&&WallDistance<600&&WallDistance>100&&Report.WallDecor<500&&(H%2==0)){
    const float Yaw=FMath::RadiansToDegrees(FMath::Atan2(-ToWall.X,ToWall.Y));FVector Mount=WallPoint-ToWall*7;Mount.Z=Z+145;
    const FName Decor[]={TEXT("Frame65"),TEXT("Noticeboard65"),TEXT("Clock65"),TEXT("Extinguisher65")};
    if(Place(Decor[(Role+H/3)%4],Mount,Yaw,NAME_None,false,WallId))Report.WallDecor++;
   }
   // Keep the entrance axis, dungeon combat cross and stair approaches usable.
   if(FMath::Abs(X)<190)return;
   if(S.Type==21&&(FMath::Abs(Y-2000)<650||X>3350))return;
   if(LWPlaces::IsTower(S.Type)&&(X>790||FMath::Abs(Y)<190))return;
   if(S.Type==58&&FMath::Abs(X)<1700)return;
   if(S.Type==32&&X<8000&&Y>-5000&&Y<-4000)return;
   if(LWDungeons::IsDungeon(S.Type)||LWPlaces::Underground(S.Type)){const float CX=FMath::Fmod(X+LWDungeons::Profile(S.Type).Columns*LWDungeons::Cell*.5f,LWDungeons::Cell)-LWDungeons::Cell*.5f;const float CY=FMath::Fmod(Y-LWDungeons::Apron*.5f+LWDungeons::Profile(S.Type).Rows*LWDungeons::Cell*.5f,LWDungeons::Cell)-LWDungeons::Cell*.5f;if(FMath::Abs(CX)<220||FMath::Abs(CY)<220)return;}
   if(Report.Furniture>=(S.Type==21?595:S.Type==32?480:420)||DeckFurniture68.FindRef(DeckKey)>=(S.Type==21?85:S.Type==32?160:LWDungeons::IsDungeon(S.Type)?90:420)||(H>>8)%5==0)return;
   const float Yaw=WallDistance<650?FMath::RoundToFloat(FMath::RadiansToDegrees(FMath::Atan2(-ToWall.X,ToWall.Y))/90)*90:float(H%4)*90;
   const FName Picks[6][5]={
    {TEXT("Armchair65"),TEXT("Sideboard65"),TEXT("Bookcase65"),TEXT("CoatRack65"),TEXT("CoffeeStation65")},
    {TEXT("ExamCart65"),TEXT("Cabinet65"),TEXT("WaitingBench65"),TEXT("CleaningCart65"),TEXT("Desk65")},
    {TEXT("ToolTrolley65"),TEXT("Pallet65"),TEXT("ToolShelf65"),TEXT("Locker65"),TEXT("ServerRack65")},
    {TEXT("DiningTable65"),TEXT("CoffeeStation65"),TEXT("Armchair65"),TEXT("Sideboard65"),TEXT("Planter65")},
    {TEXT("Shelf65"),TEXT("Counter65"),TEXT("WaitingBench65"),TEXT("ClothesRail65"),TEXT("Bookcase65")},
    {TEXT("Workstation65"),TEXT("Bookcase65"),TEXT("WaitingBench65"),TEXT("ServerRack65"),TEXT("CoffeeStation65")}};
   FName N=Picks[Role][H%5];
   if(S.Type==21&&ResortFloor<=1){const FName Lounge[]={TEXT("Armchair65"),TEXT("Sideboard65"),TEXT("WaitingBench65"),TEXT("Planter65"),TEXT("Armchair65")};N=Lounge[H%5];}
   if(S.Type==32&&X<8000&&Z<1050){const FName Terminal[]={TEXT("WaitingBench65"),TEXT("Planter65"),TEXT("LuggageCart65"),TEXT("WasteBin65"),TEXT("WaitingBench65")};N=Terminal[H%5];}
   const bool Backed=N==TEXT("Bookcase65")||N==TEXT("Cabinet65")||N==TEXT("Locker65")||N==TEXT("ServerRack65")||N==TEXT("Sideboard65")||N==TEXT("CoffeeStation65")||N==TEXT("Shelf65")||N==TEXT("ToolShelf65")||N==TEXT("Workstation65");
   if(Backed){if(WallId<0||WallDistance>650)return;auto* Model=W->Mesh(N);if(!Model)return;const FVector Q=WallPoint-ToWall*(Model->GetBoundingBox().Max.Y+14);Point.X=Q.X;Point.Y=Q.Y;}
   else {Point.X+=float((H>>16)%101)-50;Point.Y+=float((H>>23)%101)-50;}
   FName Use=TEXT("storage");if(N==TEXT("Armchair65")||N==TEXT("WaitingBench65"))Use=TEXT("chair");if(N==TEXT("DiningTable65")||N==TEXT("Planter65")||N==TEXT("CoatRack65"))Use=NAME_None;
   if(Place(N,Point,Yaw,Use)){
    Report.Furniture++;DeckFurniture68.FindOrAdd(DeckKey)++;
    // Low floor dressing is placed beside the grouping, never across its feet.
    if((Role==0||Role==3)&&(H>>12)%3==0)Place(TEXT("Rug65"),Point+FRotator(0,Yaw,0).RotateVector(FVector(190,0,0)),Yaw);
    else if((H>>12)%5==0)Place(TEXT("Mat65"),Point+FRotator(0,Yaw,0).RotateVector(FVector(0,-170,0)),Yaw);
    const FRotator Facing(0,Yaw,0);
    if(N==TEXT("DiningTable65")||N==TEXT("Desk65")||N==TEXT("Workstation65")){
     if(N!=TEXT("Workstation65")){FVector Top=Point;Top.Z+=W->Mesh(N)->GetBoundingBox().GetSize().Z+.3;Place(N==TEXT("Desk65")?TEXT("Desktop65"):TEXT("Bottles65"),Top,Yaw,NAME_None,false);}
     for(int Side:{-1,1}){if(Side>0&&N!=TEXT("DiningTable65"))continue;Place(TEXT("Chair65"),Point+Facing.RotateVector(FVector(0,Side*(N==TEXT("DiningTable65")?105:85),0)),Yaw+(Side<0?180:0),TEXT("chair"));}
    }
    if(N==TEXT("Armchair65")){const FVector Table=Point+Facing.RotateVector(FVector(0,-145,0));if(Place(TEXT("CoffeeTable65"),Table,Yaw))Place(TEXT("Papers65"),Table+FVector(0,0,W->Mesh(TEXT("CoffeeTable65"))->GetBoundingBox().GetSize().Z+.3),Yaw,NAME_None,false);}
   }
}
bool FScope::Step68(){
 if(Completed68||Parts.IsEmpty())return true;
 const double Start=FPlatformTime::Seconds();
 if(!Prepared68){Prepare68();Report.Milliseconds+=(FPlatformTime::Seconds()-Start)*1000;return false;}
 int Count=0;while(NextSample68<Samples68.Num()&&FloorBudget68<18000&&Count++<8){auto Sample=Samples68[NextSample68++];Decorate68(Sample.Floor,Sample.Point);if(FPlatformTime::Seconds()-Start>.0015)break;}
 Report.Milliseconds+=(FPlatformTime::Seconds()-Start)*1000;
 if(NextSample68<Samples68.Num()&&FloorBudget68<18000)return false;
 Completed68=true;
 if(Reports.Num()>512)Reports.RemoveAt(0,256);Reports.Add(Report);
 UE_LOG(LogTemp,Display,TEXT("INTERIOR65 type=%d surfaces=%d cells=%d furniture=%d wall=%d ceiling=%d rejected=%d ms=%.1f"),S.Type,Report.Surfaces,Report.FloorCells,Report.Furniture,Report.WallDecor,Report.CeilingDecor,Report.Rejected,Report.Milliseconds);
 return true;
}
}
