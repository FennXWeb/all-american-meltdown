// Include after FLWV2SmokeState and the existing smoke helpers in LWGameMode.cpp.
// Dispatch with -LWV17Smoke -LWPOI30Smoke (rendered, never -NullRHI).
#include "LWSiteIdentity.h"
#include "LWPOISurvivors.h"
void ALWGameMode::BuildPOI30Smoke(ALWCharacter& Initial){
#include "LWGameplay54Smoke.inl"
#include "LWWorld53Smoke.inl"
 // Failed interaction prerequisites must not abort the remaining rendered gallery.
 auto CheckReady=[this](bool Passed,const TCHAR* Label){Check(Passed,Label);return Passed;};
 auto Add=[this](FString Name,double Wait,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){
  V2->Steps.Add({Name,Wait,120,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){
#if WITH_EDITOR
   return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
   return true;
#endif
  }});
 };
 struct FState{TWeakObjectPtr<ALWChunk> Chunk;TWeakObjectPtr<ALWDungeon> Dungeon;TWeakObjectPtr<ALWResident> Recruit;LWGen::FSite Site;};
 auto S=MakeShared<FState>();
 auto View=[](ALWCharacter& P,FVector Eye,FVector Target){P.ClosePanels();LWV2Teleport(P,Eye);P.GetCharacterMovement()->DisableMovement();const FRotator Look=(Target-P.Camera->GetComponentLocation()).Rotation();P.Controller->SetControlRotation(Look);P.Camera->SetWorldRotation(Look);};
 auto Clear=[S](ALWCharacter& P){P.ClosePanels();P.CombatTarget=nullptr;P.DungeonStatus.Empty();P.RPG.Crew.RemoveAll([](const auto& Crew){return Crew.Id.ToString().StartsWith(TEXT("poi30_"));});if(S->Recruit.IsValid())S->Recruit->Destroy();S->Recruit.Reset();if(S->Chunk.IsValid())S->Chunk->Destroy();S->Chunk.Reset();S->Dungeon.Reset();};
 Add(TEXT("POI30 isolated fixture setup"),1,[this](ALWCharacter& P){P.NewGame();P.LeaveSafehouse();P.World->EnableEncounters=false;P.World->WeatherOverride=0;P.World->TimeOfDay=12;P.Health=10000;P.Money=20000;P.Flashlight->SetVisibility(false);P.World->TickWeather(0,&P);P.GetCharacterMovement()->DisableMovement();UE_LOG(LogTemp,Display,TEXT("POI30_BEGIN rendered fixtures; synthetic site IDs; not a combat or natural-generation test"));});
 Add(TEXT("Map30 generated site atlas"),2,[this](ALWCharacter& P){
  P.ClosePanels();TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;
  LWGen::Gather(FVector2D(P.GetActorLocation()),P.World->Seed,Roads,Sites);
  for(const auto& Site:Sites){P.RPG.KnownPlaces.Add(int64(Site.Id),FVector(LWGen::Entrance(Site),100));P.RPG.PlaceNames.Add(int64(Site.Id),LWSites::Name(Site));}
  Check(Sites.Num()>0,TEXT("Map30 actual generated sites available for atlas review"));P.bMap=true;
 },[this](ALWCharacter&){CaptureV2(TEXT("Map30_Icons"));});
 // The following build step closes the map through Clear before changing the view.
 for(int Type:{58,59,60,61,62,63,1,4,13}){
  Add(FString::Printf(TEXT("POI30 build %d exterior"),Type),2,[this,S,Clear,View,Type,CheckReady](ALWCharacter& P){
   Clear(P);auto& Site=S->Site;Site=LWGen::FSite();Site.Type=Type;// Construct away from loaded terrain before lifting the isolated stage. Each type
   // gets its own remote address, so the preceding view cannot stream over this build.
   Site.Position=FVector2D(2000000+Type*100000,2000000);Site.Size=LWPlaces::Size(Type);Site.Friendly=false;Site.Id=303000+Type*1000;
   // Select an actual survivor-enabled seed; do not inject mock residents.
   int Attempts=0;while(!LWPOISurvivors::Selected(Site,P.World->Seed)&&Attempts++<10000)Site.Id++;
   if(!CheckReady(Attempts<10000,TEXT("POI30 survivor site seed found")))return;
   auto* C=GetWorld()->SpawnActor<ALWChunk>();if(!CheckReady(C!=nullptr,TEXT("POI30 fixture chunk spawned")))return;S->Chunk=C;
   UE_LOG(LogTemp,Display,TEXT("POI30_BUILD site=%u type=%d seed=%d selected=%d location=%s"),Site.Id,Type,P.World->Seed,LWPOISurvivors::Selected(Site,P.World->Seed)?1:0,*Site.Position.ToString());
   // Flat supporting terrain for the isolated stage, including exterior survivor apron.
   C->Box(P.World,TEXT("Concrete"),FVector(Site.Position,-10),FVector(Site.Size+FVector2D(1200),20));
   C->BuildingSurfaces=true;C->Building(P.World,Site);C->BuildingSurfaces=false;C->FlushSurfaces();C->SetActorLocation(FVector(0,0,5000));
   int Storage=0,Residents=0,Hostiles=0;TSet<FString> Names;for(auto A:C->Residents)if(IsValid(A)){A->AddActorWorldOffset(FVector(0,0,5000));if(auto* N=Cast<ALWResident>(A)){N->Home=N->GetActorLocation();N->ActivitySpots={N->Home};N->SetActorTickEnabled(false);N->GetCharacterMovement()->DisableMovement();Residents++;Names.Add(N->DisplayName);}else if(auto* Z=Cast<ALWZombie>(A)){Hostiles++;Z->SetActorTickEnabled(false);Z->GetCharacterMovement()->DisableMovement();}if(auto* O=Cast<ALWWorldObject>(A))Storage+=O->Kind==ELWObjectKind::Container;}
   Check(Residents==3,TEXT("POI30 generated merchant recruit and mission courier"));Check(Names.Num()==3,TEXT("POI30 survivor roles have distinct proper names"));Check(Hostiles==0,TEXT("POI30 selected stop has no generic hostile overlap"));Check(Storage>=2,TEXT("POI30 detailed layout contains persistent storage"));
   bool ShortSign=false;TArray<UTextRenderComponent*> Signs;C->GetComponents(Signs);for(auto* Sign:Signs)ShortSign|=Sign->Text.ToString()==LWSites::Label(Site);if(LWPlaces::Expanded(Type))Check(ShortSign,TEXT("POI30 entrance uses short site label"));
   if(Type==58){
    FCollisionQueryParams Q;Q.AddIgnoredActor(&P);FHitResult Hit;const FVector Base(Site.Position,5012);
    Check(GetWorld()->LineTraceSingleByChannel(Hit,Base+FVector(0,-2500,900),Base+FVector(0,-2500,1500),ECC_WorldStatic,Q),TEXT("POI30 mall axial concourse has an overhead roof/skylight"));
    for(int Side:{-1,1})Check(GetWorld()->LineTraceSingleByChannel(Hit,Base+FVector(0,0,900),Base+FVector(Side*9000,0,900),ECC_WorldStatic,Q),TEXT("POI30 mall has enclosing side wall above shops"));
    Check(GetWorld()->LineTraceSingleByChannel(Hit,Base+FVector(0,0,900),Base+FVector(0,7200,900),ECC_WorldStatic,Q),TEXT("POI30 mall rear envelope is enclosed"));
    Check(!GetWorld()->SweepSingleByChannel(Hit,Base+FVector(0,-5800,125),Base+FVector(0,-4900,125),FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(32,86),Q),TEXT("POI30 mall front portal is clear for player capsule"));
    for(int Side:{-1,1})for(int Row=0;Row<6;Row++){
     const float DoorY=-3800+Row*1650;
     Check(!GetWorld()->SweepSingleByChannel(Hit,Base+FVector(Side*2900,DoorY,125),Base+FVector(Side*3500,DoorY,125),FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(32,86),Q),TEXT("POI30 mall storefront opens inward onto concourse"));
    }
    Check(!GetWorld()->SweepSingleByChannel(Hit,Base+FVector(0,-4900,125),Base+FVector(0,-2500,125),FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(32,86),Q),TEXT("POI30 welcome furniture leaves axial aisle clear"));
   }
   const FVector Center(Site.Position,5024);const float Extent=FMath::Max(Site.Size.X,Site.Size.Y);
   View(P,Center+FVector(Extent*.50,-Extent*.85,Extent*.70),Center+FVector(0,0,250));
   UE_LOG(LogTemp,Display,TEXT("POI30_LAYOUT type=%d id=%u storage=%d residents=%d size=%.0fx%.0f"),Type,Site.Id,Storage,Residents,Site.Size.X,Site.Size.Y);
  },[this,Type](ALWCharacter&){CaptureV2(*FString::Printf(TEXT("POI30_%d_Exterior"),Type));});
  if(LWPlaces::Expanded(Type))Add(FString::Printf(TEXT("POI30 interior %d"),Type),2,[this,S,View,Type](ALWCharacter& P){
   if(!S->Chunk.IsValid())return;
   const FVector Positions[]={FVector(0,-4200,125),FVector(0,-2200,125),FVector(0,-1550,125),FVector(-1300,-1300,125),FVector(-500,-1800,125),FVector(0,1800,125)};
   const FVector Targets[]={FVector(0,1600,250),FVector(3400,0,230),FVector(0,1700,340),FVector(-1300,1500,180),FVector(700,1800,210),FVector(4000,5300,1700)};
   const FVector Base(S->Site.Position,5012);View(P,Base+Positions[Type-58],Base+Targets[Type-58]);
   FHitResult Hit;FCollisionQueryParams Q;Q.AddIgnoredActor(&P);const FVector Start=Base+Positions[Type-58];Check(GetWorld()->LineTraceSingleByChannel(Hit,Start,Start-FVector(0,0,200),ECC_WorldStatic,Q),TEXT("POI30 interior viewpoint has supporting floor"));
  },[this,Type](ALWCharacter&){CaptureV2(*FString::Printf(TEXT("POI30_%d_Interior"),Type));});
  if(Type==58)for(int RoleIndex=0;RoleIndex<3;RoleIndex++){
   Add(FString::Printf(TEXT("POI30 survivor interaction %d"),RoleIndex),2,[this,S,View,RoleIndex,CheckReady](ALWCharacter& P){
    if(!S->Chunk.IsValid())return;const FName Roles[]={TEXT("merchant"),TEXT("recruit"),TEXT("warden")};ALWResident* NPC=nullptr;for(auto A:S->Chunk->Residents)if(auto* N=Cast<ALWResident>(A))if(N->NpcRole==Roles[RoleIndex]){NPC=N;break;}
    if(!CheckReady(NPC!=nullptr,TEXT("POI30 expected survivor role exists")))return;
    View(P,NPC->GetActorLocation()+FVector(0,-230,0),NPC->GetActorLocation()+FVector(0,0,40));P.Talk(NPC);Check(P.Speaker==NPC,TEXT("POI30 generated survivor opens dialogue"));
    const FName Action=RoleIndex==0?TEXT("trade"):RoleIndex==1?TEXT("hire"):TEXT("contracts");int Index=P.DialogueActions.IndexOfByKey(Action);if(!CheckReady(Index!=INDEX_NONE,TEXT("POI30 role exposes intended action")))return;
    const int64 Before=P.Money;P.ChooseDialogue(Index);
    if(RoleIndex==0){Check(NPC->Shop&&P.OpenObject==NPC->Shop,TEXT("POI30 merchant opens actual trader inventory"));if(NPC->Shop){const auto* Record=P.World->Containers.Find(NPC->Shop->RecordId);Check(Record&&Record->bTrader&&Record->Items.Num()>0,TEXT("POI30 trader stock is populated and persistent"));}}
    if(RoleIndex==1){S->Recruit=NPC;Check(P.RPG.Crew.ContainsByPredicate([&](const auto& Crew){return Crew.Id==NPC->ResidentId;}),TEXT("POI30 hire adds generated resident to crew"));Check(P.Money<Before,TEXT("POI30 recruitment charges credits"));const int64 Paid=P.Money;Check(!P.Recruit(NPC)&&P.Money==Paid,TEXT("POI30 duplicate recruitment rejected without second charge"));}
    if(RoleIndex==2){Check(P.DialogueActions.ContainsByPredicate([](const FName& A){return A.ToString().StartsWith(TEXT("quest:"));}),TEXT("POI30 courier exposes real mission catalog"));}
   },[this,RoleIndex](ALWCharacter&){const TCHAR* Names[]={TEXT("POI30_Survivor_Trade"),TEXT("POI30_Survivor_Recruit"),TEXT("POI30_Survivor_Missions")};CaptureV2(Names[RoleIndex]);});
  }
 }
 Add(TEXT("POI30 final-room guardian production spawn"),2,[this,S,Clear,View,CheckReady](ALWCharacter& P){
  Clear(P);LWGen::FSite Site;Site.Type=24;Site.Id=3039024;Site.Position=FVector2D(12000000,2000000);Site.Size=LWPlaces::Size(24);S->Site=Site;
  auto* C=GetWorld()->SpawnActor<ALWChunk>();if(!CheckReady(C!=nullptr,TEXT("POI30 dungeon fixture chunk exists")))return;S->Chunk=C;C->BuildingSurfaces=true;C->Building(P.World,Site);C->BuildingSurfaces=false;C->FlushSurfaces();C->SetActorLocation(FVector(0,0,5000));
  for(auto A:C->Residents)if(IsValid(A)){A->AddActorWorldOffset(FVector(0,0,5000));if(auto* D=Cast<ALWDungeon>(A)){S->Dungeon=D;D->SetActorTickEnabled(false);for(auto& G:D->Guards)G.Position.Z+=5000;}}
  auto* D=S->Dungeon.Get();if(!CheckReady(D!=nullptr,TEXT("POI30 production dungeon controller exists")))return;
  int Legendary=0;for(const auto& G:D->Guards){if(G.LegendaryGuardian)Legendary++;else P.World->KilledZombies.Add(G.Id);}Check(Legendary==1,TEXT("POI30 exactly one legendary end guardian"));
  // Isolate the final guardian by marking other guards defeated in this smoke save.
  const auto& Profile=LWDungeons::Profile(24);const FVector Room=D->At(LWDungeons::Room(24,Profile.Floors-1,LWDungeons::Layout(24,Site.Id,Profile.Floors-1).End));
  View(P,Room+FVector(0,-650,96),Room+FVector(0,0,140));for(int I=0;I<3;I++)D->Activate(I,&P);
  for(int I=0;I<4;I++)D->Tick(.45f);auto* Guard=D->Guards.FindByPredicate([](const auto& G){return G.LegendaryGuardian;});auto* Z=Guard?Guard->Actor.Get():nullptr;
  if(!CheckReady(Z!=nullptr,TEXT("POI30 guardian spawns through director in authored end room")))return;
  Z->SetActorTickEnabled(false);Z->GetCharacterMovement()->DisableMovement();P.CombatTarget=Z;P.CombatTargetTime=30;
  Check(Z->Stars==3&&Z->LegendaryInitialized,TEXT("POI30 guaranteed initialized three-star guardian"));Check(FMath::IsNearlyEqual(Z->MaximumHealth,700.f*1.55f*1.6f*4.75f,1.f),TEXT("POI30 titan guardian health includes warden and legendary multipliers"));Check(Z->Health==Z->MaximumHealth&&Z->LegendaryDamage()>2,TEXT("POI30 guardian starts healthy with legendary damage"));Check(!D->Ready()&&!D->Claim(&P),TEXT("POI30 living guardian blocks reserve after all controls"));
  UE_LOG(LogTemp,Display,TEXT("POI30_GUARDIAN id=%u kind=%d stars=%d hp=%.1f max=%.1f damage=%.2f end=(%s)"),Z->PersistentId,int(Z->Kind),Z->Stars,Z->Health,Z->MaximumHealth,Z->LegendaryDamage(),*Room.ToString());
 },[this](ALWCharacter&){CaptureV2(TEXT("POI30_Guardian_EndRoom"));});
 Add(TEXT("POI30 guardian gate and reward idempotence"),2,[this,S](ALWCharacter& P){
  auto* D=S->Dungeon.Get();if(!D)return;auto* G=D->Guards.FindByPredicate([](const auto& Guard){return Guard.LegendaryGuardian;});if(!G||!G->Actor.IsValid())return;
  // Synthetic defeated flag tests the gate, not weapon balance or death animation.
  P.World->KilledZombies.Add(G->Id);G->Actor->Destroy();P.CombatTarget=nullptr;Check(D->Ready(),TEXT("POI30 defeated guardian releases completed reserve"));const int64 Before=P.Money;Check(D->Claim(&P),TEXT("POI30 reserve can be claimed"));const int64 Paid=P.Money;Check(Paid==Before+LWDungeons::Credits(24),TEXT("POI30 reserve credits match profile"));Check(D->Claim(&P)&&P.Money==Paid,TEXT("POI30 reserve cannot pay twice"));
 },[this](ALWCharacter&){CaptureV2(TEXT("POI30_Guardian_ReserveUnlocked"));});
 Add(TEXT("POI30 screenshot files and cleanup"),2,FLWV2Action(),[this,S,Clear](ALWCharacter& P){
  int Count=0;for(const FString& File:V2->Screenshots)if(FPaths::GetCleanFilename(File).StartsWith(TEXT("POI30_"))){Count++;Check(IFileManager::Get().FileSize(*File)>0,TEXT("POI30 rendered screenshot exists and is nonempty"));}Check(Count==20,TEXT("POI30 captured twelve new layout three legacy three survivor and two guardian views"));
  Clear(P);P.World->WeatherOverride=-1;P.GetCharacterMovement()->SetMovementMode(MOVE_Walking);LWV2Teleport(P,FVector(0,0,300));UE_LOG(LogTemp,Display,TEXT("POI30_DONE failures=%d screenshots=%d"),TestFailures,Count);
 });
}
