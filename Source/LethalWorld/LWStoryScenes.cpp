#include "LWChapter52.h"
#include "LWVoice44.h"
#include "LWStory.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"

void ALWStoryDirector::Scene(int S){
 Player->ClosePanels();Player->CancelReload();Player->StandFromChair(true);if(Player->Vehicle)Player->Vehicle->Exit(true);
 Player->bStoryLocked=true;Player->bTrigger=Player->bAim=Player->bSprint=false;Player->GetCharacterMovement()->StopMovementImmediately();Player->SetMenuInput(true);InScene=true;Choosing=false;Shots.Empty();ShotIndex=0;ShotTime=0;Conversation=FName(*FString::FromInt(S));
 for(auto& E:Enemies)if(IsValid(E))E->SetActorTickEnabled(false);
 const int Area=LWStory::Missions()[State().Stage].Site;FVector Base=Area<0?ALWWorld::BunkerSpawn():At(Area);
 auto Shot=[&](const TCHAR* Who,const TCHAR* Line,FVector From,FVector To,FVector Focus,float Seconds=7){FLWStoryShot B;B.Speaker=Who;B.Line=Line;B.From=Base+From;B.To=Base+To;B.Look=Base+Focus;B.Duration=Seconds;Shots.Add(B);};
 if(S==0){
  Base=ALWWorld::BedroomPosition(0);Player->SetActorLocation(Base+FVector(100,50,0));
  Shot(TEXT("Shelter 01"),TEXT("6:00 AM. The generator has stopped rattling. For a moment, the silence feels like an answer."),FVector(-130,-40,15),FVector(-100,-20,55),FVector(100,60,100),7);
  Shot(TEXT(""),TEXT("You dreamed of traffic. A kettle. Someone calling from another room. Above the concrete ceiling, none of it remains."),FVector(-100,-20,55),FVector(100,50,60),FVector(280,0,50),8);
  Shot(TEXT(""),TEXT("The emergency receiver is still on. You should try the dial again."),FVector(100,50,60),FVector(120,70,65),FVector(300,100,0),5);
 }else if(S==1){
  Shot(TEXT("Emergency Receiver"),TEXT("...static..."),FVector(140,-250,30),FVector(160,-220,35),FVector(230,-150,-20),5);
  Shot(TEXT(""),TEXT("No station identification. No weather report. No one counting survivors. The last voice on this frequency promised another update tomorrow."),FVector(160,-220,35),FVector(170,-210,35),FVector(230,-150,-20),8);
  Shot(TEXT(""),TEXT("Tomorrow has arrived without them. You open the shelter log and write: GOING OUTSIDE."),FVector(170,-210,35),FVector(80,-100,70),FVector(230,-150,-20),7);
 }else if(S==8){
  State().Flags.Add(TEXT("mercy_burned"));
  Transport=NewObject<UStaticMeshComponent>(this);Transport->SetupAttachment(RootComponent);Transport->SetStaticMesh(World->Mesh(TEXT("Vehicle_boxtruck_V9")));Transport->SetWorldLocation(Base+FVector(-250,-2050,35));Transport->SetWorldRotation(FRotator(0,-90,0));Transport->SetCollisionEnabled(ECollisionEnabled::NoCollision);Transport->RegisterComponent();
  for(int K=0;K<10;K++){auto* F=NewObject<UStaticMeshComponent>(this);F->SetupAttachment(RootComponent);F->SetStaticMesh(World->Mesh(TEXT("IntroFlameV15")));F->SetMaterial(0,World->Material(TEXT("IntroFire")));F->SetWorldLocation(Base+FVector((K%2?1:-1)*2300,-700+(K/2)*650,90));F->SetWorldScale3D(FVector(2,2,4));F->SetCollisionEnabled(ECollisionEnabled::NoCollision);F->RegisterComponent();Fire.Add(F);}
  World->Sound(TEXT("FuelExplosion"),Base+FVector(2200,0,50),.55f);
  Shot(TEXT("Mara"),TEXT("They're not taking supplies. They're counting us. Jonah! Get everyone through the culvert!"),FVector(1200,-3300,650),FVector(1050,-3100,600),FVector(-250,-1900,150),8);
  Shot(TEXT("Cinder Choir Collector"),TEXT("Rusk asked for a contribution. You gave him a sermon. The medic comes with us. The rest of you can keep the ashes."),FVector(350,-1200,170),FVector(150,-1000,160),FVector(-250,-2350,90),9);
  Shot(TEXT("Mara"),TEXT("Don't follow the truck into the fire! Help Jonah. Find the transfer lists. I'll leave you something to follow."),FVector(1000,-3100,230),FVector(1100,-3600,300),FVector(-250,-2600,130),8);
  Shot(TEXT("Jonah"),TEXT("We have wounded under the bridge. Mara bought us a minute. Don't waste it trying to hold a house that's already gone."),FVector(-2400,-3600,900),FVector(-1800,-3200,800),FVector(0,0,150),8);
 }else if(S==9){
  Shot(TEXT("Jonah's note"),TEXT("Alive: Jonah, Elsie, Tomas, Nia. Missing: Mara. The truck carried a clinic placard. St. Agnes still has trauma dressings. Bring those first. Then we find her."),FVector(-170,1150,155),FVector(-80,1200,140),FVector(0,1500,85),10);
 }else if(S==10){
  Shot(TEXT("Field Clinic Log"),TEXT("The Cinder Choir requisitioned every surgeon on this road. Their receipt is stamped CINDER TOLLHOUSE, COLLECTION OFFICE THREE."),FVector(-150,1100,160),FVector(100,1200,160),FVector(0,1500,85),8);
  Shot(TEXT("Jonah, on the recovered handset"),TEXT("The dressings will keep Elsie alive. I won't forget it. Inez used to drive that route. If she's still breathing, she'll know where the trucks stop."),FVector(100,1200,160),FVector(200,1100,160),FVector(0,1500,85),8);
 }else if(S==11){
  Shot(TEXT("Transfer Ledger"),TEXT("VALE, M. MEDICAL. HOLD FOR SPECIAL COLLECTION. ROUTE SIX. DESTINATION REDACTED. Every signature below it is the same: S. RUSK."),FVector(-200,1100,170),FVector(0,1200,145),FVector(0,1500,85),9);
  Shot(TEXT(""),TEXT("A frequency is penciled into the margin. Relay Six can reach it, if you restore the generator and align the transmitter."),FVector(0,1200,145),FVector(180,1100,160),FVector(0,1500,85),7);
 }else if(S==12){
  Shot(TEXT("Inez, over the relay"),TEXT("Whoever's asking about the medic: stop using her name on an open channel. Dry Creek. Maintenance shelter. Come without a convoy, and don't shoot the woman with the red scarf."),FVector(-400,-700,300),FVector(-200,-550,240),FVector(1600,500,100),10);
 }else if(S==14){
  Shot(TEXT("Dispatch Manifest"),TEXT("Destination: KENNELS, BLOCK C. Vale is marked for the next collection. Beside her name: PROFESSIONAL ASSET. Beside the others: DISPOSABLE."),FVector(-200,1100,170),FVector(100,1200,150),FVector(0,1500,85),9);
  Shot(TEXT("Inez"),TEXT("I told myself driving wasn't choosing. Every name on that paper proves otherwise. I'm going back for Jonah. You get Mara."),FVector(-600,-2800,180),FVector(-400,-2650,170),FVector(0,-2200,100),8);
 }else if(S==15){
  Shot(TEXT("Warden's Orders"),TEXT("Keep the medic intact. Director Mercer's men are paying for trained labor, not bodies. Rusk gets his weapons when the collection clears."),FVector(-200,1100,160),FVector(100,1200,150),FVector(0,1500,80),9);
 }else if(S==16){
  Shot(TEXT("Mara"),TEXT("You found the lists. I knew you'd read the lists."),FVector(1600,3050,165),FVector(1580,3100,165),FVector(1600,3400,140),6);
  Shot(TEXT("Mara"),TEXT("They called us inventory. Then they made me treat the people they'd beaten. There are others in the next shipment. We don't get to walk away and call this finished."),FVector(1580,3100,165),FVector(1550,3150,165),FVector(1600,3400,140),10);
 }else if(S==18){
  Shot(TEXT("Mara"),TEXT("Both relays are dead. Rusk's men will fall back to the furnace hall. He likes to stand above the people who work for him. Let's change that."),FVector(-1400,-3000,450),FVector(-1000,-2500,350),FVector(0,100,100),9);
 }else if(S==20){
  auto* Rusk=Enemy(50,At(7,FVector(0,100,110)),false,1);if(Rusk){Rusk->SetActorTickEnabled(false);Rusk->SetActorLocation(At(7,FVector(0,100,65)));if(Rusk->Parts.Num())Rusk->Parts[0]->SetRelativeRotation(FRotator(25,0,0));}
  Shot(TEXT("Silas Rusk"),TEXT("You think I invented this? People wanted order. I charged for it. Mercer writes his own receipts. Ask him what a citizen costs."),FVector(-100,-400,160),FVector(-60,-300,160),FVector(0,100,95),9);
  Shot(TEXT("Captain Voss"),TEXT("Silas Rusk. Your contract is terminated."),FVector(650,-450,180),FVector(500,-350,170),FVector(0,100,95),5);
  Shot(TEXT(""),TEXT("The armored squad breaches the hall. A burst of gunfire cuts off Rusk's answer."),FVector(500,-350,170),FVector(300,-250,160),FVector(0,100,65),6);
  Shot(TEXT("Captain Adrienne Voss"),TEXT("Liberty's Heroes. We are restoring this country to glory. Your weapons are unregistered. Put them down and come with us. Cooperation will be remembered."),FVector(0,-100,160),FVector(0,0,165),FVector(0,700,150),10);
 }else if(S==-2){
  Shot(TEXT("Captain Voss"),TEXT("Weapons secured. No citizenship record. No sponsor. Disposition: nonessential."),FVector(-250,-150,160),FVector(-200,-100,140),FVector(0,700,150),8);
  Shot(TEXT(""),TEXT("You handed them your weapons. They never promised to let you live."),FVector(-200,-100,140),FVector(-180,-80,30),FVector(0,500,220),7);
 }else if(S==-3){
  RestorePrison();Base=At(8);
  Shot(TEXT("Captain Voss"),TEXT("Resistance recorded. Keep this one for processing. Director Mercer likes to meet the difficult ones."),FVector(-2450,-2450,100),FVector(-2400,-2300,70),FVector(-2050,-1700,130),8);
  Shot(TEXT("Mara, through the wall"),TEXT("You're alive. Good. Evidence storage is across the service corridor. Look under the bunk. Whoever was here before us loosened a fastener."),FVector(-2400,-2300,70),FVector(-2500,-2250,100),FVector(-2450,-2050,85),9);
 }else if(S==23){
  Shot(TEXT(""),TEXT("Your equipment is here, tagged and counted. A second shelf is labeled CIVILIAN FORFEITURES. Beneath it sits Jonah's coat."),FVector(-2650,-400,155),FVector(-2500,-300,150),FVector(-2200,100,90),8);
 }else if(S==24){
  Shot(TEXT("Jonah"),TEXT("You opened the cells. We'll open the gates. Inez cut the west searchlights. Elsie found an armory. This time we leave together."),FVector(-600,-2800,175),FVector(-400,-2650,170),FVector(250,-2100,150),9);
  Shot(TEXT("Mara"),TEXT("Mercer is in the command court. He runs his armor off the relay cabinets. Cut both feeds or you'll burn every bullet you have."),FVector(350,-2850,180),FVector(200,-2700,170),FVector(-250,-2350,150),8);
 }else if(S==26){
  Shot(TEXT("Director Elias Mercer"),TEXT("I gave these people a country when all you offered was a campfire. A nation needs one voice, one will, and someone prepared to punish disobedience."),FVector(-600,-600,240),FVector(-400,-400,200),FVector(0,100,150),10);
  Shot(TEXT("Mara"),TEXT("You put children in cells and called it a census. We are done asking permission to exist."),FVector(400,-2800,180),FVector(200,-2700,170),FVector(-250,-2350,145),8);
  Shot(TEXT("Director Mercer"),TEXT("Then let history record your choice. Seal the court. Fire on anyone who stands with them."),FVector(0,-500,180),FVector(0,-300,180),FVector(0,100,150),8);
 }else if(S==27){
  Shot(TEXT("Mara"),TEXT("Mercer is dead. Put the rifles down. Nobody else has to die for his portrait."),FVector(-1100,-1400,230),FVector(-850,-1000,220),FVector(0,600,100),8);
  Shot(TEXT("Liberty's Heroes Sergeant"),TEXT("Stand down. All posts, stand down. We were told there was nothing outside these walls."),FVector(600,-700,190),FVector(400,-500,180),FVector(0,600,100),8);
  Shot(TEXT("Jonah"),TEXT("There are people outside. There always were. Open the gate and help us bring them in."),FVector(-2400,-3800,1100),FVector(-1900,-3300,900),FVector(0,0,150),8);
  Shot(TEXT("Mara"),TEXT("Mercy Crossing was never the buildings. It was what we agreed to do for each other. We can do it here. With better walls, and doors that open both ways."),FVector(300,-2850,180),FVector(200,-2700,170),FVector(-250,-2350,145),10);
  for(int K=0;K<5;K++){auto* N=Person(FName(*FString::Printf(TEXT("surrender_%d"),K)),TEXT("Disarmed Trooper"),At(8,FVector((K-2)*240,600,100)));if(N&&N->Gun)N->Gun->SetVisibility(false);}
 }else{
  Shot(TEXT(""),TEXT("Clean water, a sealed dressing, and a tin of food. Mara checks the seals before packing them. Behind the diner, something strikes the service door. The supplies will have to last until Mercy Crossing."),FVector(-300,1000,180),FVector(100,1200,180),FVector(0,1500,80),6);
 }
 if(S==4||S==9||S==10||S==11||S==14||S==15||S==23){const FVector Focus=Objective52(LWStory::Missions()[State().Stage].Action);for(auto& B:Shots){B.From=Focus+FVector(-160,-220,95);B.To=Focus+FVector(100,-180,85);B.Look=Focus;}}
 if(S==12){for(auto& B:Shots){const FVector F=Objective52(TEXT("relay_b"));B.From=F+FVector(-180,-250,100);B.To=F+FVector(120,-240,80);B.Look=F;}}
 if(S==24){for(auto& B:Shots){B.From=At(8,FVector(2500,-2800,190));B.To=At(8,FVector(2600,-2700,180));B.Look=At(8,FVector(2500,-2100,140));}}
 if(S==26){for(auto& B:Shots){B.From=At(8,FVector(0,2250,180));B.To=At(8,FVector(150,2300,170));B.Look=At(8,FVector(0,3100,150));}}
 SceneLight->SetIntensity(FMath::Clamp(float(FVector::DistSquared(Shots[0].From,Shots[0].Look)*.05),4000.f,90000.f));SceneLight->SetVisibility(true);Speech=Shots[0].Line;VoiceName=Shots[0].Speaker;Camera->SetWorldLocation(Shots[0].From);Camera->SetWorldRotation((Shots[0].Look-Shots[0].From).Rotation());if(auto* PC=Cast<APlayerController>(Player->Controller))PC->SetViewTarget(this);SceneAudio=World->Sound(S==1?FName(TEXT("CarRadio")):LWVoice44::Story(VoiceName),Camera->GetComponentLocation(),.5f);Player->RequestSave40();
}
void ALWStoryDirector::FinishScene(){SceneLight->SetVisibility(false);if(SceneAudio){SceneAudio->Stop();SceneAudio->DestroyComponent();SceneAudio=nullptr;}int S=FCString::Atoi(*Conversation.ToString());InScene=false;Shots.Empty();CombatGrace52=GetWorld()->GetTimeSeconds()+4;Player->bStoryLocked=false;if(auto* PC=Cast<APlayerController>(Player->Controller))PC->SetViewTarget(Player);Player->SetMenuInput(false);State().SceneFinished=true;
 if(S==-2){State().Flags.Add(TEXT("execution_ending"));Failed=true;Choosing=false;Speech=TEXT("You were executed. Liberty's Heroes recorded you as nonessential.");VoiceName=TEXT("NO CITIZENSHIP RECORD");Player->bStoryLocked=true;Player->SetMenuInput(true);Player->RequestSave40();return;}
 if(S==20){for(auto& E:Enemies)if(IsValid(E)&&!E->bDead){FDamageEvent Damage;E->Die(Damage,this,500,nullptr);}State().Flags.Add(TEXT("rusk_executed"));Choosing=true;Player->bStoryLocked=true;Player->SetMenuInput(true);VoiceName=TEXT("Captain Adrienne Voss");Speech=TEXT("Give up your weapons. Come with us. Now.");Choices={TEXT("Surrender my weapons and follow."),TEXT("Refuse. They will take me by force.")};Player->RequestSave40();return;}
 if(S==-3){RestorePrison();Player->RequestSave40();return;}if(S==26){for(auto& E:Enemies)if(IsValid(E))E->SetActorTickEnabled(true);Player->RequestSave40();return;}Advance();
}

void ALWStoryDirector::AnimateScene(float Dt){
 const int S=FCString::Atoi(*Conversation.ToString());
 for(int K=0;K<Fire.Num();K++)if(Fire[K])Fire[K]->SetWorldScale3D(FVector(2,2,3.5f+FMath::Sin(GetWorld()->GetTimeSeconds()*7+K)*.7f));
 if(S==8&&Transport){float T=ShotIndex<2?0:ShotIndex==2?ShotTime/8.f:1+ShotTime/8.f;FVector P=At(1,FVector(-250,-2050-T*1700,35));Transport->SetWorldLocation(P);if(auto* N=People.FindRef(TEXT("story_mara")).Get();N&&ShotIndex>=2){N->SetActorLocation(P+FVector(0,180,110));N->SetActorRotation(FRotator(0,90,0));}}
 if(S==20){for(int K=0;K<4;K++){if(auto* N=People.FindRef(FName(*FString::Printf(TEXT("hero_%d"),K))).Get()){float T=ShotIndex==0?0:ShotIndex==1?FMath::Clamp(ShotTime/4.f,0.f,1.f):1;N->SetActorLocation(At(7,FMath::Lerp(FVector((K-1.5f)*180,-1800,100),FVector((K-1.5f)*180,350,100),T)));}}
  if(ShotIndex>=2&&!State().Flags.Contains(TEXT("rusk_executed"))){State().Flags.Add(TEXT("rusk_executed"));World->Sound(TEXT("RifleFire"),At(7,FVector(0,350,140)));for(auto& E:Enemies)if(IsValid(E)&&!E->bDead){FDamageEvent Hit;E->Die(Hit,this,1000,nullptr);}}
 }
}
