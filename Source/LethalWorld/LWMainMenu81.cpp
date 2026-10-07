#include "LWMainMenu81.h"
#include "LWAppearance.h"
#include "LWCampaign76.h"
#include "LWSaveGame.h"
#include "LWSaveSlots62.h"
#include "LWWorld.h"
#include "LWNewYork69.h"
#include "LWSyracuse73.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

FString LWMenu81::Mission(const FLWRPGState& State){
 if(!State.Campaign76.Paused&&State.Campaign76.Started&&State.Campaign76.Ending.IsNone())
  if(const auto* Stage=LWCampaign76::Stage(State.Campaign76.Stage))return Stage->Title;
 if(State.Quests.ContainsByPredicate([&](const auto& Q){return Q.Id==State.TrackedQuest&&!Q.Rewarded;}))
  if(const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& Q){return Q.Id==State.TrackedQuest;}))return Q->Title;
 return TEXT("No active mission");
}
FString LWMenu81::Location(const ULWSaveGame& Save){
 if(!Save.MenuLocation81.IsEmpty())return Save.MenuLocation81;
 if(ALWWorld::IsSafePosition(Save.Position))return TEXT("Bunker");
 // Legacy saves have no location label. Resolve the nearest authored town without
 // generating POIs, road networks, navmeshes or live chunks behind the menu.
 double Best=DBL_MAX;FString Town=TEXT("Upstate New York");
 for(const auto& T:LWNY69::Towns()){const double D=FVector2D::DistSquared(FVector2D(Save.Position),LWNY69::Project(T.Latitude,T.Longitude));if(D<Best){Best=D;Town=T.Name;}}
 return Town;
}
const TArray<LWMenu81::FRelease>& LWMenu81::Releases(){
 static const TArray<FRelease> Data={
  {TEXT("UPDATE 84"),TEXT("SEPTEMBER 26, 2026"),TEXT("Across New York, above the clouds"),{
   TEXT("Mapped roads, lakes, towns and northern story destinations connect western and northern New York. Toronto and the border crossings follow their geographic locations."),
   TEXT("Twelve populated communities offer shops, daily routines and recruits at the bar."),
   TEXT("Syracuse Hancock has its surveyed terminal footprint, two full-length runways, baggage claim, check-in, security and gate lounges."),
   TEXT("Fly a private jet, a 100-seat airliner or a luxury airliner. Use the flight computer to claim it and engage autopilot, then walk around the cabin."),
   TEXT("Autopilot takes off, flies to an airport near your waypoint and lands. Without a waypoint it roams. Full tanks provide one hour of manual flight or five hours on autopilot."),
   TEXT("Individual luggage bins, reclining seats, window shades, suites, minibars, cabin lighting and live-camera TVs. Recover claimed aircraft and their luggage at airport service terminals."),
   TEXT("Reworked explosions leave smoke, sparks and lingering flames. High-altitude flights rise above layered clouds into a darkening sky."),
   TEXT("Map data: OpenStreetMap contributors (ODbL) and Natural Earth. See Docs/Geography84-Sources.md for sources and adaptations.")}},
  {TEXT("UPDATE 83"),TEXT("SEPTEMBER 26, 2026"),TEXT("Clearer roads, wider menus"),{
   TEXT("Electric motorhomes now have a neutral pearl-white factory finish."),
   TEXT("Far fewer stop signs: through roads keep priority and side roads stop."),
   TEXT("Smaller traffic-light heads with more clearance. Vehicles no longer collide with signal masts or climb onto their housings."),
   TEXT("Every Tab menu page now uses your screen width, with responsive inventory panes, a wider map and more collection columns.")}},
  {TEXT("UPDATE 82"),TEXT("SEPTEMBER 26, 2026"),TEXT("A place to call home"),{
   TEXT("Buy or find a settlement flag, claim land, and build with scrap. Structures snap together and foundations align with roads."),
   TEXT("Customize both wall faces, furnish rooms, and build functional doors, windows, lighting and storage."),
   TEXT("Beds and a HAM radio attract residents. Assign jobs, collect supplies and contracts, or bring residents into your crew."),
   TEXT("Insured parking protects owned vehicles and cargo. Manage your settlements from the new Settlements tab."),
   TEXT("World scrap supplies increased. All settlement construction, residents and parking are saved with your game.")}},
  {TEXT("UPDATE 81"),TEXT("SEPTEMBER 26, 2026"),TEXT("A new front door"),{
   TEXT("A rebuilt main menu with animated lakeside artwork, drifting mist and foreground parallax."),
   TEXT("Continue now shows your saved survivor, money, level, location and active mission."),
   TEXT("Tile navigation supports mouse, keyboard and controller, including ultrawide displays."),
   TEXT("Read recent changes here. Reduced Motion disables background drift and portrait animation.")}},
  {TEXT("UPDATE 80"),TEXT("SEPTEMBER 25, 2026"),TEXT("Back on the road"),{
   TEXT("Menu buttons respond to the first click. Save confirmations no longer click through to the slot list."),
   TEXT("Save inside vehicles and queue a manual save while an autosave is being written."),
   TEXT("Wiper controls work with keyboard remapping and controller LB + X."),
   TEXT("Both RVs received repaired windows, sealed dashboard areas and fitted controls."),
   TEXT("Louder vehicle engines, tires and brakes, with clearer sound inside the cabin.")}},
  {TEXT("UPDATE 79"),TEXT("SEPTEMBER 25, 2026"),TEXT("Stories between journeys"),{
   TEXT("Loading screens cycle through six illustrations with smooth transitions."),
   TEXT("A collection of 81 gameplay tips and lore entries covers survival, vehicles, combat and the world.")}},
  {TEXT("UPDATE 78"),TEXT("SEPTEMBER 2026"),TEXT("Out into the world"),{
   TEXT("Reorganized settings, survivor tabs and the in-game HUD."),
   TEXT("Interaction prompts follow your chosen key bindings, and opening conversations support the mouse."),
   TEXT("Small wilderness scenes and supplies make the approach to populated areas more varied."),
   TEXT("Mounted signs replace floating labels around POIs.")}}
 };return Data;
}

ALWMenuPortrait81::ALWMenuPortrait81(){
 PrimaryActorTick.bCanEverTick=false;SetActorEnableCollision(false);
 SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Studio")));
 PoseRoot=CreateDefaultSubobject<USceneComponent>(TEXT("SurvivorPose"));PoseRoot->SetupAttachment(RootComponent);
 Capture=CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SurvivorCapture"));Capture->SetupAttachment(RootComponent);
 Capture->PrimitiveRenderMode=ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
 Capture->CaptureSource=ESceneCaptureSource::SCS_FinalColorLDR;Capture->FOVAngle=29;
 Capture->bCaptureEveryFrame=false;Capture->bCaptureOnMovement=false;Capture->bAlwaysPersistRenderingState=true;
 Capture->ShowFlags.SetAtmosphere(false);Capture->ShowFlags.SetFog(false);Capture->ShowFlags.SetMotionBlur(false);Capture->ShowFlags.SetTemporalAA(false);
 auto& PP=Capture->PostProcessSettings;PP.bOverride_AutoExposureMethod=true;PP.AutoExposureMethod=AEM_Manual;PP.bOverride_AutoExposureApplyPhysicalCameraExposure=true;PP.AutoExposureApplyPhysicalCameraExposure=false;PP.bOverride_AutoExposureBias=true;PP.AutoExposureBias=-1;
 PP.bOverride_FilmGrainIntensity=true;PP.FilmGrainIntensity=0;PP.bOverride_SceneFringeIntensity=true;PP.SceneFringeIntensity=0;Capture->ShowFlags.SetPostProcessMaterial(false);
 PP.bOverride_VignetteIntensity=true;PP.VignetteIntensity=0;PP.bOverride_BloomIntensity=true;PP.BloomIntensity=.12f;
 const FVector Eye(178,-78,163);Capture->SetRelativeLocation(Eye);Capture->SetRelativeRotation((FVector(0,0,145)-Eye).Rotation());
 for(int I=0;I<3;I++){auto* L=CreateDefaultSubobject<UPointLightComponent>(*FString::Printf(TEXT("PortraitLight%d"),I));L->SetupAttachment(RootComponent);L->SetRelativeLocation(I==0?FVector(140,-140,235):I==1?FVector(90,160,160):FVector(-70,50,220));L->SetIntensity(I==0?24000:I==1?18000:6500);L->SetAttenuationRadius(650);L->SetLightColor(I==0?FLinearColor(1,.94f,.88f):FLinearColor(.62f,.78f,1));L->SetLightingChannels(false,false,true);L->SetCastShadows(false);}
}
void ALWMenuPortrait81::Build(const FLWIdentity& Identity){
 LWAppearance::Build(this,PoseRoot,Identity,Parts);
 auto* Backdrop=NewObject<UStaticMeshComponent>(this);Backdrop->SetupAttachment(RootComponent);Backdrop->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));Backdrop->SetRelativeLocation(FVector(-85,0,135));Backdrop->SetRelativeScale3D(FVector(.08,8,6));Backdrop->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Art/Menu81/M_Studio81.M_Studio81")));Backdrop->SetCollisionEnabled(ECollisionEnabled::NoCollision);Backdrop->RegisterComponent();
 TArray<UPrimitiveComponent*> All;GetComponents(All);for(auto* C:All){C->SetLightingChannels(false,false,true);C->SetCanEverAffectNavigation(false);}
 Capture->ShowOnlyActorComponents(this);
 Target=NewObject<UTextureRenderTarget2D>(this);Target->RenderTargetFormat=RTF_RGBA8;Target->ClearColor=FLinearColor(.014f,.025f,.032f,1);Target->InitAutoFormat(480,600);Capture->TextureTarget=Target;
 Render(0,true);
}
void ALWMenuPortrait81::Render(double Time,bool ReducedMotion){
 if(CaptureCount>0&&(ReducedMotion||Time<NextFrame))return;
 NextFrame=Time+1./20.;PoseRoot->SetRelativeRotation(FRotator(0,-10+(ReducedMotion?0:FMath::Sin(Time*.23)*3),0));PoseRoot->SetRelativeLocation(FVector(0,0,ReducedMotion?0:FMath::Sin(Time*1.3)*.3));Capture->CaptureScene();CaptureCount++;
}
void ULWMainMenu81::Refresh(const FString& ExactSlot){
 StopPortrait();Requested=true;Ready=false;Loading=false;Error.Empty();Credits=CanadianDollars=0;Place.Empty();Mission.Empty();const uint32 Token=++Revision;
 Slot=ExactSlot.IsEmpty()?LWSaves62::Latest():ExactSlot;
 if(Slot.IsEmpty()){
  const FString Legacy=FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))?TEXT("AllAmericanMeltdown_AutomationV17"):TEXT("LethalWorld_Survivor");
  if(UGameplayStatics::DoesSaveGameExist(Legacy,0))Slot=Legacy;
 }
 if(Slot.IsEmpty())return;Loading=true;
 UGameplayStatics::AsyncLoadGameFromSlot(Slot,0,FAsyncLoadGameFromSlotDelegate::CreateWeakLambda(this,[this,Token](const FString&,const int32,USaveGame* Data){if(Token!=Revision)return;Accept(Cast<ULWSaveGame>(Data));}));
}
void ULWMainMenu81::Accept(ULWSaveGame* Save){
 Loading=false;Ready=Save&&Save->Version<=2;
 if(!Ready){Error=TEXT("Save unavailable. Choose Load game.");return;}
 Identity=Save->Identity;Credits=Save->Money;CanadianDollars=Save->RPG.Canada68.CanadianDollars;Level=Save->RPG.Level;Day=Save->DayNumber;
 Place=LWMenu81::Location(*Save);Mission=Save->MenuMission81.IsEmpty()?LWMenu81::Mission(Save->RPG):Save->MenuMission81;
}
void ULWMainMenu81::EnsurePortrait(UWorld* World){if(Ready&&!Portrait&&World){Portrait=World->SpawnActor<ALWMenuPortrait81>(FVector(0,0,-120000),FRotator::ZeroRotator);if(Portrait)Portrait->Build(Identity);}}
void ULWMainMenu81::StopPortrait(){if(IsValid(Portrait))Portrait->Destroy();Portrait=nullptr;}
