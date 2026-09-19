#pragma once
#include "CoreMinimal.h"
namespace LWDungeons {
constexpr int First=22,Count=10;
constexpr float Cell=1600,Deck=500,Apron=1200;
struct FProfile {const TCHAR* Name;const TCHAR* Theme;int Columns,Rows,Floors,Scale;int MainEnemy,Elite;const TCHAR* Wall;const TCHAR* Floor;const TCHAR* Objective;const TCHAR* Finale;};
inline const FProfile& Profile(int Type){static const FProfile Underground[]={
 {TEXT("DEEPWELL SURVIVAL BUNKER"),TEXT("Shelter / dormitories / life support"),3,3,2,0,1,5,TEXT("Concrete"),TEXT("Steel"),TEXT("RESTORE LIFE SUPPORT"),TEXT("CONTINUITY SUPPLY VAULT")},
 {TEXT("FRACTURE CAVERNS"),TEXT("Limestone caves / abandoned excavations"),4,3,2,1,7,6,TEXT("Concrete"),TEXT("Concrete"),TEXT("POWER EXCAVATION HOIST"),TEXT("LOST EXPEDITION CACHE")},
 {TEXT("INTERSTATE SERVICE TUNNELS"),TEXT("Utility corridors / rail sidings / substations"),6,2,3,1,0,5,TEXT("BrickV7"),TEXT("Concrete"),TEXT("RESTORE SUBSTATION POWER"),TEXT("EMERGENCY TRANSPORT RESERVE")}};if(Type>=53&&Type<=55)return Underground[Type-53];static const FProfile P[]={
 {TEXT("REDLINE PUMPWORKS"),TEXT("Municipal water / storm control"),3,3,2,0,0,7,TEXT("Concrete"),TEXT("Steel"),TEXT("RESTART PUMP CONTROL"),TEXT("EMERGENCY SUPPLY RESERVE")},
 {TEXT("SAINT MERCY QUARANTINE"),TEXT("Isolation wards / surgical wing"),3,3,3,0,0,5,TEXT("TileV7"),TEXT("TileV7"),TEXT("RESTORE DECONTAMINATION"),TEXT("SEALED PHARMACY")},
 {TEXT("BLACKWATER REMAND"),TEXT("Intake / cell blocks / execution wing"),4,3,2,0,1,5,TEXT("Concrete"),TEXT("Concrete"),TEXT("OVERRIDE CELL SECURITY"),TEXT("WARDEN'S ARMORY")},
 {TEXT("FEDERAL BULLION DEPOSITORY"),TEXT("Audit halls / counting rooms / vaults"),4,4,3,1,1,1,TEXT("BrickV7"),TEXT("CasinoMarble21"),TEXT("AUTHENTICATE VAULT CIRCUIT"),TEXT("FEDERAL STRONGROOM")},
 {TEXT("DEAD AIR BROADCAST CENTER"),TEXT("Studios / newsrooms / transmitter decks"),3,3,4,1,1,6,TEXT("Wood"),TEXT("CasinoVelvet21"),TEXT("ROUTE EMERGENCY SIGNAL"),TEXT("BROADCAST SECURITY CACHE")},
 {TEXT("RUSTHAVEN STEELWORKS"),TEXT("Casting beds / machine shops / furnaces"),5,4,3,1,0,5,TEXT("Rust"),TEXT("Steel"),TEXT("ISOLATE FURNACE FEED"),TEXT("UNION WAR CHEST")},
 {TEXT("EDEN RESEARCH ANNEX"),TEXT("Clean rooms / containment / specimen labs"),4,4,4,1,0,6,TEXT("TileV7"),TEXT("TileV7"),TEXT("DISABLE CONTAINMENT LOCK"),TEXT("DIRECTOR'S PROTOTYPE VAULT")},
 {TEXT("PATRIOT MISSILE COMMAND"),TEXT("Launch control / silos / hardened barracks"),5,4,5,2,1,5,TEXT("Concrete"),TEXT("Steel"),TEXT("CANCEL LAUNCH INTERLOCK"),TEXT("STRATEGIC ARSENAL")},
 {TEXT("UNION CENTRAL INTERCHANGE"),TEXT("Concourse / platforms / maintenance / freight"),5,5,5,2,0,7,TEXT("BrickV7"),TEXT("Concrete"),TEXT("SWITCH TRACTION POWER"),TEXT("EVACUATION CONVOY PAYLOAD")},
 {TEXT("EXECUTIVE CONTINUITY ARCOLOGY"),TEXT("Public lobby / residences / command / executive refuge"),5,5,7,2,1,6,TEXT("CasinoMarble21"),TEXT("CasinoVelvet21"),TEXT("REVOKE EXECUTIVE SEAL"),TEXT("PRESIDENTIAL CONTINUITY RESERVE")}};return P[FMath::Clamp(Type-First,0,Count-1)];}
inline bool IsDungeon(int T){return T>=First&&T<First+Count;}
inline FVector2D Size(int T){const auto& P=Profile(T);return {P.Columns*Cell+120,P.Rows*Cell+Apron+120};}
struct FDeck {TArray<uint8> Links;TArray<int> Distance;int End=0;};
// Connected seeded spanning tree plus sparse loops. Different type/floor salts give different plans.
inline FDeck Layout(int T,uint32 Id,int Floor){const auto& P=Profile(T);FDeck D;int N=P.Columns*P.Rows;D.Links.Init(0,N);D.Distance.Init(-1,N);FRandomStream R(Id^uint32(Floor*7919+T*271));TArray<int> Stack{0};TSet<int> Seen{0};
 const int DX[]={1,0,-1,0},DY[]={0,1,0,-1};while(Stack.Num()){int A=Stack.Last(),X=A%P.Columns,Y=A/P.Columns;TArray<int> Choices;for(int K=0;K<4;K++){int U=X+DX[K],V=Y+DY[K];if(U>=0&&U<P.Columns&&V>=0&&V<P.Rows&&!Seen.Contains(V*P.Columns+U))Choices.Add(K);}if(!Choices.Num()){Stack.Pop();continue;}int K=Choices[R.RandRange(0,Choices.Num()-1)],B=(Y+DY[K])*P.Columns+X+DX[K];D.Links[A]|=1<<K;D.Links[B]|=1<<((K+2)%4);Seen.Add(B);Stack.Add(B);}
 for(int A=0;A<N;A++)for(int K=0;K<2;K++){int X=A%P.Columns+DX[K],Y=A/P.Columns+DY[K];if(X<P.Columns&&Y<P.Rows&&R.FRand()<.12f){D.Links[A]|=1<<K;D.Links[Y*P.Columns+X]|=1<<((K+2)%4);}}
 TArray<int> Queue{0};D.Distance[0]=0;for(int I=0;I<Queue.Num();I++){int A=Queue[I];for(int K=0;K<4;K++)if(D.Links[A]&(1<<K)){int B=A+DX[K]+DY[K]*P.Columns;if(D.Distance[B]<0){D.Distance[B]=D.Distance[A]+1;Queue.Add(B);if(D.Distance[B]>D.Distance[D.End])D.End=B;}}}return D;}
inline FVector Room(int T,int Floor,int Index){const auto& P=Profile(T);return FVector((Index%P.Columns+.5f-P.Columns*.5f)*Cell,(Index/P.Columns+.5f-P.Rows*.5f)*Cell+Apron*.5f,36+Floor*Deck);}
inline uint32 EnemyId(uint32 Site,int Index){return Site^(0x91e10da5u*uint32(Index+1))^0xd0270000u;}
inline int RelayFloor(int T,int Relay){return FMath::RoundToInt((Profile(T).Floors-1)*Relay*.5f);}
inline int RelayRoom(int T,uint32 Id,int Relay){const auto D=Layout(T,Id,RelayFloor(T,Relay));if(Relay!=1)return D.End;int Best=0;for(int I=1;I<D.Distance.Num();I++)if(I!=D.End&&D.Distance[I]>D.Distance[Best])Best=I;return Best;}
inline int Credits(int T){return Profile(T).Scale==0?1800:Profile(T).Scale==1?4500:10000;}
inline int XP(int T){return Profile(T).Scale==0?500:Profile(T).Scale==1?1200:2500;}
}
