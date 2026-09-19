#include "LWDungeon.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWWeaponMods.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
ALWDungeon::ALWDungeon(){PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.45f;}
void ALWDungeon::EndPlay(const EEndPlayReason::Type Reason){if(WasInside)if(auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))P->DungeonStatus.Empty();Super::EndPlay(Reason);}
FName ALWDungeon::Key(const TCHAR* Suffix)const{return FName(*FString::Printf(TEXT("dungeon_%u_%s"),Site.Id,Suffix));}
FVector ALWDungeon::At(FVector Local)const{return FVector(Site.Position,Chunk?Chunk->GetActorLocation().Z:0)+FRotator(0,Site.Yaw,0).RotateVector(Local);}
void ALWDungeon::Setup(ALWWorld* W,ALWChunk* C,const LWGen::FSite& S){World=W;Chunk=C;Site=S;const auto& P=LWDungeons::Profile(S.Type);FRandomStream R(S.Id^0x27100u);
 for(int F=0;F<P.Floors;F++){const auto D=LWDungeons::Layout(S.Type,S.Id,F);for(int I=1;I<D.Links.Num();I++){
  // Clear landings and objective alcoves; patrol pairs occupy designated open room centers.
  int Count=1+(P.Scale>0&&I%3==0?1:0);for(int N=0;N<Count;N++){FLWDungeonGuard G;G.Position=At(LWDungeons::Room(S.Type,F,I)+FVector(N?260:-100,100,96));G.Floor=F;G.Kind=R.FRand()<.77?P.MainEnemy:(S.Type==24||S.Type==25||S.Type==29||S.Type==31?2:0);G.Id=LWDungeons::EnemyId(S.Id,Guards.Num());Guards.Add(G);}}
 }
 const auto D=LWDungeons::Layout(S.Type,S.Id,P.Floors-1);for(int I=0;I<2+P.Scale;I++){FLWDungeonGuard G;G.Position=At(LWDungeons::Room(S.Type,P.Floors-1,D.End)+FVector(I==1?-300:I==2?300:0,I==3?300:0,96));G.Kind=I==0?P.Elite:P.MainEnemy;G.Floor=P.Floors-1;G.Warden=true;G.LegendaryGuardian=I==0;G.Id=LWDungeons::EnemyId(S.Id,Guards.Num());Guards.Add(G);}
}
int ALWDungeon::Relays()const{int M=World->PropStates.FindRef(Key(TEXT("relays"))),N=0;for(int I=0;I<3;I++)N+=(M>>I)&1;return N;}
int ALWDungeon::WardensLeft()const{int N=0;for(const auto& G:Guards)if(G.Warden&&!World->KilledZombies.Contains(G.Id))N++;return N;}
bool ALWDungeon::Ready()const{return Relays()==3&&WardensLeft()==0;}
void ALWDungeon::Tick(float Dt){Super::Tick(Dt);auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!World||!Chunk||!P->bStarted||P->bMenu)return;
 const FVector Local=FRotator(0,-Site.Yaw,0).RotateVector(P->GetActorLocation()-FVector(Site.Position,Chunk->GetActorLocation().Z));bool Inside=FMath::Abs(Local.X)<Site.Size.X*.5+200&&FMath::Abs(Local.Y)<Site.Size.Y*.5+200&&Local.Z>-100&&Local.Z<LWDungeons::Profile(Site.Type).Floors*LWDungeons::Deck+200;
 if(Inside){P->DungeonStatus=World->PropStates.Contains(Key(TEXT("claimed")))?TEXT("COMPLEX CLEARED"):FString::Printf(TEXT("SECURITY CIRCUITS %d / 3    VAULT GUARDS %d"),Relays(),WardensLeft());if(!WasInside)P->Notify(TEXT("Find three security controls. A legendary guardian defends the final reserve."),8);}else if(WasInside)P->DungeonStatus.Empty();WasInside=Inside;
 if(P->Health<=0)return;for(auto& G:Guards)if(G.Actor.IsValid()&&!G.Actor->bDead&&FMath::Abs(G.Position.Z-P->GetActorLocation().Z)>900){G.SavedHealth=G.Actor->Health;G.Actor->Destroy();G.Actor.Reset();}
 int Active=0;for(const auto& G:Guards)if(G.Actor.IsValid()&&!G.Actor->bDead)Active++;int Spawned=0;
 for(auto& G:Guards){if(G.Actor.IsValid()||World->KilledZombies.Contains(G.Id)||Active>=24||World->ZombieCount>=130||Spawned>=3)continue;
  if(FMath::Abs(G.Position.Z-P->GetActorLocation().Z)>750||FVector::Dist2D(G.Position,P->GetActorLocation())>7200)continue;
  if(FVector::DistSquared(G.Position,P->GetActorLocation())<FMath::Square(550.f))continue;
  // Do not materialize inside geometry; guarded points remain pending and retry.
  FCollisionQueryParams Q;const float Half=G.Kind==5?152:G.Kind==6?135:G.Kind==7?70:G.Kind==2?43:88;const float Radius=G.Kind==5||G.Kind==6?55:G.Kind==7?70:G.Kind==2?26:29;FVector Spawn=G.Position;Spawn.Z+=Half-88;
  if(GetWorld()->OverlapBlockingTestByChannel(Spawn,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius,Half),Q))continue;
  FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
  auto* Z=GetWorld()->SpawnActor<ALWZombie>(G.Position,FRotator(0,Site.Yaw+180,0),Params);if(!Z)continue;Z->PersistentId=G.Id;Z->ConfigureKind(ELWEnemyKind(G.Kind));Z->Home=Z->GetActorLocation();if(G.Warden)Z->Health*=1.55f;if(G.LegendaryGuardian){Z->Stars=3;Z->Health*=1.6f;}Z->EnsureLegendary();if(G.SavedHealth>0)Z->Health=FMath::Min(G.SavedHealth,Z->MaximumHealth);G.Actor=Z;Chunk->Residents.Add(Z);World->ZombieCount++;Active++;Spawned++;
 }
}
bool ALWDungeon::Activate(int I,ALWCharacter* P){if(I<0||I>2||!P||P->Health<=0)return false;int& Mask=World->PropStates.FindOrAdd(Key(TEXT("relays")));if(Mask&(1<<I)){P->Notify(TEXT("CONTROL ALREADY RESTORED"));return false;}Mask|=1<<I;World->Sound(TEXT("Generator"),P->GetActorLocation(),.25f);World->Noise(P->GetActorLocation(),3500);P->Notify(FString::Printf(TEXT("SECURITY CONTROL %d / 3 RESTORED"),Relays()),5);P->PersistWorldChange();return true;}
void ALWDungeon::MakeReward(){const FName Id=Key(TEXT("reserve"));if(World->Containers.Contains(Id))return;FLWContainerRecord C;C.Id=Id;C.Width=16;C.Height=16;C.Unlocked=true;C.Context=TEXT("dungeon_reserve");C.Position=At(LWDungeons::Room(Site.Type,LWDungeons::Profile(Site.Type).Floors-1,LWDungeons::Layout(Site.Type,Site.Id,LWDungeons::Profile(Site.Type).Floors-1).End));
 FRandomStream R(Site.Id^0x27ff55);int Serial=0;auto Add=[&](FName Name,int Count,int Tier=-1){const auto& Def=LWItems::Def(Name);if(Def.Id.IsNone()){UE_LOG(LogTemp,Error,TEXT("Unknown dungeon reward %s"),*Name.ToString());return;}while(Count>0){const int Stack=FMath::Min(Count,Def.MaxStack);Count-=Stack;auto Item=LWItems::Make(Name,Stack);Item.Id=FGuid::NewDeterministicGuid(Id.ToString()+FString::FromInt(Serial++));if(Tier>=0){Item.WeaponTier=Tier;LWMods::RollFinish(Item,R);}if(Def.Category==TEXT("Magazine"))Item.Rounds=Def.Capacity;if(!LWItems::Place(C.Items,Item,C.Width,C.Height)){UE_LOG(LogTemp,Error,TEXT("Dungeon reward placement failed %s"),*Name.ToString());return;}}};
 const int Scale=LWDungeons::Profile(Site.Type).Scale;const FName Guns[]={TEXT("shotgun"),TEXT("rifle"),TEXT("sniper"),TEXT("lmg"),TEXT("m4")};for(int I=0;I<2+Scale;I++){FName Gun=Guns[R.RandRange(0,4)];Add(Gun,1,I==0&&Scale>0?4:Scale==0?2:3);const auto& Def=LWItems::Def(Gun);if(!Def.MagazineType.IsNone())Add(Def.MagazineType,2);if(!Def.AmmoType.IsNone())Add(Def.AmmoType,Def.Capacity*2);}
 Add(TEXT("medkit"),4+Scale*3);Add(TEXT("ammo_12g"),24+Scale*12);Add(TEXT("ammo_556"),60);Add(TEXT("lockpick"),8+Scale*4);Add(TEXT("att_laser"),1);Add(TEXT("att_light"),1);World->Containers.Add(Id,C);
}
bool ALWDungeon::Claim(ALWCharacter* P){if(!P||P->Health<=0)return false;if(!Ready()){P->Notify(FString::Printf(TEXT("RESERVE SEALED: %d / 3 CONTROLS, %d GUARDS REMAIN"),Relays(),WardensLeft()),6);return false;}MakeReward();if(!World->PropStates.Contains(Key(TEXT("claimed")))){World->PropStates.Add(Key(TEXT("claimed")),1);P->Money+=LWDungeons::Credits(Site.Type);P->GainXP(LWDungeons::XP(Site.Type));P->QuestEvent(TEXT("dungeon"));World->Sound(TEXT("LevelUp"),P->GetActorLocation());P->Notify(FString::Printf(TEXT("COMPLEX CLEARED   +%d CREDITS   +%d XP"),LWDungeons::Credits(Site.Type),LWDungeons::XP(Site.Type)),8);P->PersistWorldChange();}return true;}
FString ALWDungeonDevice::Prompt()const{if(!Dungeon)return TEXT("");const auto& P=LWDungeons::Profile(Dungeon->Site.Type);return Index==4?TEXT("[E] READ SITE DIRECTORY"):Index==3?(Dungeon->Ready()?TEXT("[E] OPEN SECURED RESERVE"):TEXT("[E] CHECK RESERVE SECURITY")):FString(TEXT("[E] "))+P.Objective;}
void ALWDungeonDevice::Use(ALWCharacter* P){if(!Dungeon||!P)return;if(Index==4){const auto& D=LWDungeons::Profile(Dungeon->Site.Type);P->Notify(FString::Printf(TEXT("%s: %d LEVELS. Security controls on levels %d, %d, %d. %s on level %d."),D.Name,D.Floors,LWDungeons::RelayFloor(Dungeon->Site.Type,0)+1,LWDungeons::RelayFloor(Dungeon->Site.Type,1)+1,LWDungeons::RelayFloor(Dungeon->Site.Type,2)+1,D.Finale,D.Floors),15);return;}if(Index<3){Dungeon->Activate(Index,P);return;}if(Dungeon->Claim(P))P->OpenContainer(this);}
