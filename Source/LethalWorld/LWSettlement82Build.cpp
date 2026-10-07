#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWCampaign76.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWPlayerInput51.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
namespace {
FTransform Center82(const FLWBuild82& D,FTransform T){T.SetLocation(T.TransformPosition(LWBuilding82::Offset(D)));return T;}
}
bool ALWSettlement82::Validate(const FLWBuild82& D,const FTransform& T,FName Parent,FName Ignore,FString& Why)const{
 const auto* C=State();auto Fail=[&](const TCHAR* S){Why=S;return false;};if(!C)return Fail(TEXT("No settlement selected."));
 if(!Ignore.IsNone()&&LWBuilding82::DependsOn(*C,Ignore))return Fail(TEXT("Move attached pieces first."));
 if(C->Pieces.Num()>=600&&Ignore.IsNone())return Fail(TEXT("This settlement has reached its 600-piece budget."));
 if(D.Use==TEXT("bed")&&LWBuilding82::Beds(*C)>=100&&Ignore.IsNone())return Fail(TEXT("This settlement already has 100 beds."));
 FVector At=T.GetLocation();for(float X:{-.5f,.5f})for(float Y:{-.5f,.5f}){auto P=T.TransformPosition({D.Size.X*X,D.Size.Y*Y,0});if(FVector::Dist2D(C->Center,P)>C->Radius)return Fail(TEXT("Outside the settlement boundary."));if(LWCampaign76::Reserved(FVector2D(P)))return Fail(TEXT("Campaign grounds are protected."));}
 if(At.Z<C->Center.Z-400||At.Z>C->Center.Z+2200)return Fail(TEXT("Outside the build height limit."));
 if(LWNY69::WaterDepth(FVector2D(At))>2)return Fail(TEXT("Cannot build in deep water."));
 LWGen::FSite Foot;Foot.Position=FVector2D(At);Foot.Yaw=T.Rotator().Yaw;Foot.Size=FVector2D(D.Size);Foot.Type=0;
 for(const auto& R:Roads)if(LWGen::RoadOverlaps(Foot,R))return Fail(TEXT("Keep roads and their shoulders clear."));
 for(const auto& S:Sites)if(LWBuilding82::Overlap(FTransform(T.Rotator(),FVector(At.X,At.Y,0)),{D.Size.X*.5,D.Size.Y*.5,100},FTransform(FRotator(0,S.Yaw,0),FVector(S.Position,0)),{S.Size.X*.5+60,S.Size.Y*.5+60,100}))return Fail(TEXT("Existing POI grounds are protected."));
 const auto* Base=C->Pieces.FindByPredicate([&](const auto& P){return P.Id==Parent&&P.Id!=Ignore;});const auto* BD=Base?LWBuilding82::Find(Base->Catalog):nullptr;
 if((D.Wall()||D.Use==TEXT("floor")||D.Use==TEXT("roof")||D.Use==TEXT("ceilinglight")||D.Use==TEXT("walldecor"))&&!Base)return Fail(TEXT("Snap this piece to a supporting structure."));
 if(D.Use==TEXT("foundation")||D.Use==TEXT("parking")){
  for(float X:{-.48f,.48f})for(float Y:{-.48f,.48f}){auto Corner=T.TransformPosition({D.Size.X*X,D.Size.Y*Y,0});float Gap=Corner.Z-World->HeightAt(FVector2D(Corner));if(Gap<2||Gap>(D.Use==TEXT("parking")?28:118))return Fail(D.Use==TEXT("parking")?TEXT("Parking needs level land. Choose a flatter spot."):TEXT("Foundation must meet the ground at every corner."));}
 }else if(!Base&&At.Z-World->HeightAt(FVector2D(At))>35)return Fail(TEXT("This item needs a supporting surface."));
 // Explicit parent checks are authoritative, including calls that do not originate in the preview.
 if(Base){
  const FVector Local=Base->Transform.InverseTransformPosition(At);bool Supports=false;
  if(D.Wall())Supports=BD&&(BD->Deck()||BD->Wall())&&(FMath::Abs(Local.Z)<3||(BD->Wall()&&FMath::Abs(Local.Z-300)<3))&&FMath::Abs(Local.X)<=401&&FMath::Abs(Local.Y)<=201;
  else if(D.Use==TEXT("floor")||D.Use==TEXT("roof"))Supports=BD&&((BD->Wall()&&FMath::Abs(Local.Z-320)<3)||(BD->Deck()&&FMath::Abs(Local.Z)<3)||(BD->Use==TEXT("stairs")&&FMath::Abs(Local.Z-320)<3))&&FMath::Abs(Local.X)<=401&&FMath::Abs(Local.Y)<=401;
  else if(D.Use==TEXT("ceilinglight"))Supports=BD&&(BD->Use==TEXT("floor")||BD->Use==TEXT("roof"))&&FMath::Abs(Local.X)<200&&FMath::Abs(Local.Y)<200;
  else if(D.Use==TEXT("walldecor"))Supports=BD&&BD->Wall()&&FMath::Abs(Local.X)+D.Size.X*.5<=200&&Local.Z>=0&&Local.Z+D.Size.Z<=300;
  else Supports=BD&&FMath::Abs(Local.X)<=BD->Size.X*.5+200&&FMath::Abs(Local.Y)<=BD->Size.Y*.5+200&&FMath::Abs(Local.Z-(D.Use==TEXT("foundation")?0.:BD->Deck()?2.:BD->Size.Z+2.))<3;
  if(!Supports)return Fail(TEXT("The selected support does not fit this piece."));
 }
 const FTransform Box=Center82(D,T);
 for(const auto& P:C->Pieces){if(P.Id==Ignore)continue;const auto* Other=LWBuilding82::Find(P.Catalog);if(!Other)continue;
  if(FVector::DistSquared(P.Transform.GetLocation(),At)<4&&P.Catalog==D.Id)return Fail(TEXT("A piece is already placed here."));
  if(P.Id==Parent&&(D.Use==TEXT("walldecor")||D.Use==TEXT("ceilinglight")))continue;
  if(LWBuilding82::Overlap(Box,D.Size*.5,Center82(*Other,P.Transform),Other->Size*.5,D.Wall()&&Other->Wall()?12:3))return Fail(TEXT("Overlaps another settlement piece."));
  if(Other->Use==TEXT("door")&&!D.Structural()){
   FTransform Clear=P.Transform;Clear.SetLocation(P.Transform.TransformPosition({0,0,110}));if(LWBuilding82::Overlap(Box,D.Size*.5,Clear,{72,130,110},2))return Fail(TEXT("Leave space to open and walk through the door."));
  }
 }
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);for(const auto& Pair:Actors)if(IsValid(Pair.Value))Q.AddIgnoredActor(Pair.Value);
 // Below-ground foundations are intentional; test only their usable upper surface.
 FVector TestHalf=D.Size*.5-FVector(3);FVector Center=Box.GetLocation();if(D.Use==TEXT("foundation")||D.Use==TEXT("parking")){Center=At+FVector(0,0,25);TestHalf.Z=22;}
 if(GetWorld()->OverlapBlockingTestByChannel(Center,T.GetRotation(),ECC_Pawn,FCollisionShape::MakeBox(TestHalf.ComponentMax(FVector(1))),Q))return Fail(TEXT("Blocked by terrain, a person, vehicle or world prop."));
 Why=Ignore.IsNone()?FString::Printf(TEXT("%s / %d scrap"),*D.Name,D.Cost):TEXT("Move here");return true;
}
void ALWSettlement82::Aim(FVector Point,FVector Normal){
 Valid=false;Support=NAME_None;
 if(Claiming){Placement=FTransform(FRotator(0,Yaw,0),Point);Valid=Normal.Z>.8f&&ValidateClaim(Point,Message);return;}
 const auto* C=State();if(!C||!LWBuilding82::Catalog().IsValidIndex(SelectedCatalog))return;const auto& D=LWBuilding82::Catalog()[SelectedCatalog];
 FTransform Best(FRotator(0,Yaw,0),Point);double Distance=260;bool Snapped=false;
 auto Offer=[&](FTransform T,FName Parent){double V=FVector::Dist(T.GetLocation(),Point);if(V<Distance){Distance=V;Best=T;Support=Parent;Snapped=true;}};
 if(Snap)for(const auto& P:C->Pieces){if(P.Id==Moving)continue;const auto* B=LWBuilding82::Find(P.Catalog);if(!B)continue;
  auto At=[&](FVector Offset,float Angle=0){return FTransform(FRotator(0,P.Transform.Rotator().Yaw+Angle,0),P.Transform.TransformPosition(Offset));};
  if(D.Use==TEXT("foundation")&&B->Use==TEXT("foundation"))for(auto V:{FVector(400,0,0),FVector(-400,0,0),FVector(0,400,0),FVector(0,-400,0)})Offer(At(V),P.Id);
  if(D.Wall()&&B->Deck()){Offer(At({0,-200,0}),P.Id);Offer(At({0,200,0},180),P.Id);Offer(At({-200,0,0},-90),P.Id);Offer(At({200,0,0},90),P.Id);}
  if(D.Wall()&&B->Wall()){Offer(At({400,0,0}),P.Id);Offer(At({-400,0,0}),P.Id);Offer(At({0,0,300}),P.Id);}
  if(D.Use==TEXT("floor")||D.Use==TEXT("roof")){
   if(B->Wall())for(float Side:{-1.f,1.f})Offer(At({0,Side*200,320}),P.Id);
   if(B->Use==D.Use)for(auto V:{FVector(400,0,0),FVector(-400,0,0),FVector(0,400,0),FVector(0,-400,0)})Offer(At(V),P.Id);
   if(B->Use==TEXT("stairs"))Offer(At({0,400,320}),P.Id);
  }
 }
 if(!Snapped&&RoadsSnap&&D.Use==TEXT("foundation"))for(const auto& R:Roads){auto T=LWBuilding82::RoadSnap(Point,R,D.Size);T.SetLocation(FVector(FVector2D(T.GetLocation()),World->HeightAt(FVector2D(T.GetLocation()))+30+Height));Offer(T,NAME_None);}
 if(!Snapped){
  FVector At=Point;if(Snap){At.X=FMath::GridSnap(At.X,20.);At.Y=FMath::GridSnap(At.Y,20.);}At.Z+=D.Use==TEXT("foundation")?30+Height:D.Use==TEXT("parking")?18:2;
  // Furniture keeps the actual surface under the cursor, including tables and upper floors.
  double Nearest=MAX_dbl;for(const auto& P:C->Pieces){if(P.Id==Moving)continue;const auto* B=LWBuilding82::Find(P.Catalog);if(!B)continue;FVector Local=P.Transform.InverseTransformPosition(Point);
   if(D.Use==TEXT("walldecor")&&B->Wall()&&FMath::Abs(Local.Y)<30&&FMath::Abs(Local.X)<200){float Side=Local.Y<0?-1:1;Local.Y=Side*(12+D.Size.Y*.5);Local.Z=FMath::Clamp(Local.Z-D.Size.Z*.5,0.,300.-D.Size.Z);Best=FTransform(FRotator(0,P.Transform.Rotator().Yaw+(Side>0?180:0),0),P.Transform.TransformPosition(Local));Support=P.Id;Snapped=true;break;}
   if(D.Use==TEXT("ceilinglight")&&(B->Use==TEXT("roof")||B->Use==TEXT("floor"))&&FMath::Abs(Local.X)<190&&FMath::Abs(Local.Y)<190&&FMath::Abs(Local.Z+20)<30){Local.Z=-20-D.Size.Z;Best=FTransform(FRotator(0,Yaw,0),P.Transform.TransformPosition(Local));Support=P.Id;Snapped=true;break;}
   const float Top=B->Deck()?0:B->Size.Z;if(FMath::Abs(Local.X)<=B->Size.X*.5&&FMath::Abs(Local.Y)<=B->Size.Y*.5&&FMath::Abs(Local.Z-Top)<8&&FMath::Abs(Local.Z-Top)<Nearest){Support=P.Id;Nearest=FMath::Abs(Local.Z-Top);At.Z=P.Transform.GetLocation().Z+Top+2;}
  }
  if(!Snapped)Best=FTransform(FRotator(0,Snap?FMath::GridSnap(Yaw,15.f):Yaw,0),At);
 }
 Placement=Best;Valid=Validate(D,Placement,Support,Moving,Message);if(Valid&&Moving.IsNone()&&Scrap()<D.Cost){Valid=false;Message=FString::Printf(TEXT("Need %d scrap / available %d"),D.Cost,Scrap());}
}
void ALWSettlement82::TickBuild(float Dt){auto* PC=Cast<APlayerController>(Player->Controller);if(!PC)return;
 float X=0,Y=0;PC->GetInputMouseDelta(X,Y);FRotator R=Camera->GetComponentRotation();float Gain=FMath::Clamp(Player->Sensitivity,.025f,5.f)*2.5f;R.Yaw+=X*Gain;R.Pitch=FMath::Clamp(R.Pitch+Y*Gain,-85.f,85.f);Camera->SetWorldRotation(R);
 auto Down=[&](FKey K){if(auto* Input=ULWPlayerInput51::Get51(Player);Input&&Input->Controller58)return PC->IsInputKeyDown(K);else if(Input)for(const auto& B:Input->Bindings)if(B.Logical==K)return PC->IsInputKeyDown(B.Physical);return PC->IsInputKeyDown(K);};
 FRotator Flat(0,R.Yaw,0);FVector Move=Flat.Vector()*float(Down(EKeys::W)-Down(EKeys::S))+Flat.RotateVector(FVector::RightVector)*float(Down(EKeys::D)-Down(EKeys::A))+FVector::UpVector*float((PC->IsInputKeyDown(EKeys::SpaceBar)||(ULWPlayerInput51::Get51(Player)->Controller58&&PC->IsInputKeyDown(EKeys::E)))-(PC->IsInputKeyDown(EKeys::LeftControl)||(ULWPlayerInput51::Get51(Player)->Controller58&&PC->IsInputKeyDown(EKeys::MiddleMouseButton))));
 FVector At=Camera->GetComponentLocation()+Move.GetClampedToMaxSize(1)*Dt*(PC->IsInputKeyDown(EKeys::LeftShift)?1500:650);FVector Center=State()&&!Claiming?State()->Center:Player->GetActorLocation();float Radius=State()&&!Claiming?State()->Radius+500:4000;FVector2D Delta=FVector2D(At-Center).GetClampedToMaxSize(Radius);At.X=Center.X+Delta.X;At.Y=Center.Y+Delta.Y;At.Z=FMath::Clamp(At.Z,Center.Z+70,Center.Z+2500);Camera->SetWorldLocation(At);
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);if(auto* A=Actors.FindRef(Moving).Get())Q.AddIgnoredActor(A);FHitResult H;const FVector Start=Camera->GetComponentLocation();if(GetWorld()->LineTraceSingleByChannel(H,Start,Start+Camera->GetForwardVector()*6000,ECC_Visibility,Q))Aim(H.ImpactPoint,H.ImpactNormal);else{Valid=false;Message=TEXT("Aim at a surface or a structural connection.");}
 Preview->SetVisibility(true);Preview->SetStaticMesh(World->Mesh(TEXT("Cube")));Preview->SetMaterial(0,World->Material(TEXT("Glass")));FVector Size(60,60,270),Offset(0,0,135);
 if(!Claiming&&LWBuilding82::Catalog().IsValidIndex(SelectedCatalog)){const auto& D=LWBuilding82::Catalog()[SelectedCatalog];Size=D.Size;Offset=LWBuilding82::Offset(D);if(!D.Structural()&&D.Use!=TEXT("parking")&&D.Use!=TEXT("farm")&&D.Use!=TEXT("ham")){auto* Mesh=World->Mesh(D.Model);if(Mesh){Preview->SetStaticMesh(Mesh);auto B=Mesh->GetBoundingBox();Preview->SetWorldScale3D(Size/B.GetSize().ComponentMax(FVector(1)));Preview->SetWorldLocationAndRotation(Placement.TransformPosition(-FVector(B.GetCenter().X,B.GetCenter().Y,B.Min.Z)*Preview->GetComponentScale()),Placement.GetRotation());}}}
 if(Preview->GetStaticMesh()==World->Mesh(TEXT("Cube"))){Preview->SetWorldLocationAndRotation(Placement.TransformPosition(Offset),Placement.GetRotation());Preview->SetWorldScale3D(Size/100);}
 DrawDebugBox(GetWorld(),Placement.TransformPosition(Offset),Size*.5,Placement.GetRotation(),Valid?FColor(72,220,154):FColor(250,85,65),false,0,0,2);
 DrawDebugCircle(GetWorld(),Center+FVector(0,0,20),State()&&!Claiming?State()->Radius:6000,96,FColor(80,180,200),false,0,0,2,FVector::ForwardVector,FVector::RightVector,false);
 if(!PC->IsInputKeyDown(EKeys::LeftMouseButton))WaitClick=false;
}
bool ALWSettlement82::Input(FKey Key,bool Down){if(!Building)return false;auto* K=ULWPlayerInput51::Get51(Player);if(K&&Key.IsGamepadKey())K->Controller58=true;FKey Logical=K?K->Translate51(Key):Key;
 if(Key==EKeys::Escape||Key==EKeys::Gamepad_FaceButton_Right){if(Down){if(Palette){Close();Player->SwitchTab(6);}else if(!Moving.IsNone()){Moving=NAME_None;Message=TEXT("Move cancelled.");}else if(Claiming)Close();else SetPalette(true);}return true;}
 if(Key==EKeys::Tab||Key==EKeys::Gamepad_Special_Right){if(Down&&!Claiming)SetPalette(!Palette);return true;}
 if(Palette)return false;
 if(Down){
  if(Key==EKeys::LeftMouseButton||Key==EKeys::Gamepad_RightTrigger||Key==EKeys::Gamepad_FaceButton_Bottom){if(!WaitClick)Place();return true;}
  if(Key==EKeys::RightMouseButton||Key==EKeys::Gamepad_LeftTrigger){Moving=NAME_None;return true;}
  if(Logical==EKeys::E||Key==EKeys::Gamepad_RightShoulder){SelectPiece();return true;}
  if(Key==EKeys::R||Key==EKeys::Gamepad_FaceButton_Left){Yaw=FMath::UnwindDegrees(Yaw+15);return true;}
  if(Key==EKeys::G||Key==EKeys::Gamepad_DPad_Left){Snap=!Snap;return true;}
  if(Key==EKeys::B||Key==EKeys::Gamepad_DPad_Right){RoadsSnap=!RoadsSnap;return true;}
  if(Key==EKeys::Delete||Key==EKeys::Gamepad_LeftShoulder){Dismantle();return true;}
  if(Key==EKeys::PageUp||Key==EKeys::Gamepad_DPad_Up){Height=FMath::Min(88.f,Height+10);return true;}
  if(Key==EKeys::PageDown||Key==EKeys::Gamepad_DPad_Down){Height=FMath::Max(0.f,Height-10);return true;}
  if(Key==EKeys::T){Paint();return true;}
 }return false;
}
void ALWSettlement82::SelectPiece(){FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);auto At=Camera->GetComponentLocation();if(!GetWorld()->LineTraceSingleByChannel(H,At,At+Camera->GetForwardVector()*6000,ECC_Visibility,Q))return;
 for(const auto& A:Actors)if(A.Value==H.GetActor()&&A.Value->Claim==Selected){auto* C=State();if(!C)return;auto* P=C->Pieces.FindByPredicate([&](const auto& X){return X.Id==A.Key;});if(!P)return;Moving=P->Id;SelectedCatalog=LWBuilding82::Catalog().IndexOfByPredicate([&](const auto& D){return D.Id==P->Catalog;});Outside=P->Outside;Inside=P->Inside;OutsideColor=P->OutsideColor;InsideColor=P->InsideColor;Yaw=P->Transform.Rotator().Yaw;Message=TEXT("Move, repaint, or dismantle the selected piece.");return;}
}
bool ALWSettlement82::Place(){
 if(!Building||!Valid)return false;if(Claiming)return ClaimAt(Placement.GetLocation());auto* C=State();if(!C||!LWBuilding82::Catalog().IsValidIndex(SelectedCatalog))return false;const auto& D=LWBuilding82::Catalog()[SelectedCatalog];
 if(!Validate(D,Placement,Support,Moving,Message))return false;
 FLWConstruction82* Existing=C->Pieces.FindByPredicate([&](const auto& P){return P.Id==Moving;});
 if(Existing)for(const auto& V:World->Vehicles)if(V.Value.HomeParking82==Moving){Message=TEXT("Release vehicles before moving this parking lot.");return false;}
 if(!Existing&&!Spend(D.Cost))return false;
 FLWConstruction82 P=Existing?*Existing:FLWConstruction82();if(!Existing){P.Id=FName(*(Selected.ToString()+TEXT("_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)));P.Paid=D.Cost;}P.Catalog=D.Id;P.Transform=Placement;P.Support=Support;P.Outside=Outside;P.Inside=Inside;P.OutsideColor=OutsideColor;P.InsideColor=InsideColor;
 if(Existing)*Existing=P;else C->Pieces.Add(P);Refresh(P.Id);Moving=NAME_None;Player->RequestSave40();World->Sound(TEXT("LootPickup"),Placement.GetLocation(),.65f);Message=TEXT("Placed");return true;
}
bool ALWSettlement82::Dismantle(){auto* C=State();if(!Building||!C||Moving.IsNone())return false;auto* P=C->Pieces.FindByPredicate([&](const auto& X){return X.Id==Moving;});if(!P)return false;
 if(LWBuilding82::DependsOn(*C,Moving)){Message=TEXT("Remove attached pieces first.");return false;}
 if(const auto* D=LWBuilding82::Find(P->Catalog);D&&D->Use==TEXT("bed")&&LWBuilding82::Beds(*C)<=C->Residents.Num()){Message=TEXT("Every resident must keep a bed. Build a replacement first.");return false;}
 if(auto* Store=World->Containers.Find(Moving);Store&&!Store->Items.IsEmpty()){Message=TEXT("Empty this container before dismantling it.");return false;}
 for(const auto& V:World->Vehicles)if(V.Value.HomeParking82==Moving){Message=TEXT("Release the vehicles assigned to this parking lot first.");return false;}
 if(auto* A=Actors.FindRef(Moving).Get()){if(Player->SittingChair==A||Player->RestBed==A||Player->OpenObject==A){Message=TEXT("This furniture is in use.");return false;}A->Destroy();}Actors.Remove(Moving);
 C->Scrap+=FMath::Max(1,P->Paid/2);for(auto& N:C->Residents){if(N.Station==Moving)N.Station=NAME_None;if(N.Bed==Moving)N.Bed=NAME_None;}C->Pieces.RemoveAll([&](const auto& X){return X.Id==Moving;});World->Containers.Remove(Moving);World->PropStates.Remove(Moving);Moving=NAME_None;Player->RequestSave40();Message=TEXT("Dismantled. Half the scrap returned to the stockpile.");return true;
}
bool ALWSettlement82::Paint(){auto* C=State();if(!Building||!C)return false;auto* P=C->Pieces.FindByPredicate([&](const auto& X){return X.Id==Moving;});if(!P){Message=TEXT("Select a building piece first.");return false;}const auto* D=LWBuilding82::Find(P->Catalog);if(!D||!D->Structural()){Message=TEXT("Paint and wallpaper apply to structural pieces.");return false;}
 if(P->Outside==Outside&&P->Inside==Inside&&P->OutsideColor==OutsideColor&&P->InsideColor==InsideColor){Message=TEXT("Choose finishes in the catalog first.");return false;}if(!Spend(2))return false;P->Outside=Outside;P->Inside=Inside;P->OutsideColor=OutsideColor;P->InsideColor=InsideColor;Refresh(P->Id);Player->RequestSave40();Message=TEXT("Finishes applied to both sides.");return true;
}
