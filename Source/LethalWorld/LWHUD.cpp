#include "LWGeography84.h"
#include "LWSettlement82.h"
#include "LWCampaign76.h"
#include "LWCurrency70.h"
#include "LWPlayerInput51.h"
#include "LWPrompts78.h"
#include "LWBunker45.h"
#include "LWGraphics33.h"
#include "LWZombie.h"
#include "LWStory.h"
#include "LWLighting.h"
#include "LWWeaponMods.h"
#include "LWVehicle.h"
#include "LWHUD.h"
#include "Engine/World.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWInteractable.h"
#include "LWWorldObject.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/App.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/Font.h"
#include "Misc/ScopeExit.h"

namespace {const FLinearColor Bone(.90f,.92f,.90f),Muted(.51f,.63f,.65f),Amber(.98f,.59f,.28f),Red(.87f,.22f,.17f),Ink(.008f,.016f,.022f,.94f);}
void ALWHUD::Rect(float X,float Y,float W,float H,FLinearColor C){
 if(W<=0||H<=0)return;
 if(W>=110&&H>=35&&C.A>.45f&&FMath::Max3(C.R,C.G,C.B)>.003f&&FMath::Max3(C.R,C.G,C.B)<.32f){SurfacePanel55(X,Y,W,H,C);return;}
 DrawRect(C,X*Scale,Y*Scale,W*Scale,H*Scale);
}
void ALWHUD::Line(float X,float Y,float X2,float Y2,FLinearColor C,float Width){DrawLine(X*Scale,Y*Scale,X2*Scale,Y2*Scale,C,FMath::Max(1.f,Width*Scale));}
void ALWHUD::Text(const FString& InStr,float X,float Y,float Size,FLinearColor Color)
{
    FString Str=InStr;if(auto* K=ULWPlayerInput51::Get51(this);K&&K->Controller58){Str.ReplaceInline(TEXT("LMB"),TEXT("A"));Str.ReplaceInline(TEXT("RMB"),TEXT("RB"));Str.ReplaceInline(TEXT("MMB"),TEXT("LB+RB"));}
    if(Str.Contains(TEXT("[")))if(auto* K=ULWPlayerInput51::Get51(this)){K->Load51();Str=LWPrompts78::Format(*K,Str);}

    Color.A*=.65f+.35f*Reveal55;
    Size=FMath::Max(Size,.72f);
    if(!UIFont55){DrawText(Str,Color,X*Scale,Y*Scale,nullptr,Size*Scale);return;}
    TArray<FString> Lines;Str.ParseIntoArray(Lines,TEXT("\n"),false);
    for(const auto& S:Lines){
      if(!S.IsEmpty()){
       float TW=0,TH=0;GetTextSize(S,TW,TH,UIFont55,1);
       FCanvasTextItem Item(FVector2D(X,Y)*Scale,FText::FromString(S),UIFont55,Color);
       // Preserve glyph proportions; long labels may shrink to their existing layout budget.
       const float FontScale=FMath::Min(9*Size*S.Len()*Scale/FMath::Max(1.f,TW),17*Size*Scale/FMath::Max(1.f,TH));
       Item.Scale=FVector2D(FontScale);
       const auto* Player=Cast<ALWCharacter>(PlayerOwner->GetPawn());
       if(Player&&Player->bStarted&&!Player->IsUIOpen())Item.EnableShadow(FLinearColor(0,0,0,.45f),FVector2D(1,1));
       Canvas->DrawItem(Item);
      }Y+=20*Size;
    }
}
void ALWHUD::Button(FName Id,const FString& Label,float X,float Y,float Width){
 if(auto* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());P&&P->BaseUI45&&P->BunkerManager45){P->BunkerManager45->TerminalButton46(*this,Id,Label,X,Y,Width);return;}
 Control55(Id,Label,X,Y,Width);
}
void ALWHUD::NotifyHitBoxClick(FName N)
{
    Super::NotifyHitBoxClick(N);ALWCharacter* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(!P||P->bDebug67)return;
    // Own this press before a callback can close or replace its panel.
    P->ConsumeUIAttack();
    if(UIClick55)UGameplayStatics::PlaySound2D(this,UIClick55,.22f,1.1f);
    if(P->Settlement82&&(P->RPGPanel==7||P->SettlementBuild82)&&P->Settlement82->Action(N))return;
    if(MenuClick81(P,N))return;
    if(P->bInventory&&(N==TEXT("inv_sort55")||N==TEXT("inv_other55")||N==TEXT("inv_all55"))){DragId.Invalidate();if(N==TEXT("inv_sort55"))P->SortInventory();else if(N==TEXT("inv_all55"))P->TakeAll54();else P->SortInventory(IsValid(P->OpenObject)&&P->OpenObject->Kind==ELWObjectKind::Stash?1:2);return;}
    if(N==TEXT("ui_motion55")){ReducedMotion55=!ReducedMotion55;SaveUI55();return;}
    if(N==TEXT("ui_contrast55")){HighContrast55=!HighContrast55;SaveUI55();return;}
if(P->BaseUI45&&P->BunkerManager45){P->BunkerManager45->Action(N);return;}
    if(P->SavePanel62){SaveClick62(P,N);return;}if(N==TEXT("Save62")){P->OpenSaves62(true);return;}if(N==TEXT("Load62")){P->OpenSaves62(false);return;}
    if(P->Workbench39){WorkbenchClick39(P,N);return;}
    if(N==TEXT("mission_checkpoint")){P->RecoverMission37(0);return;}
    if(N==TEXT("mission_restart")){P->RecoverMission37(1);return;}
    if(N==TEXT("mission_cancel")){P->RecoverMission37(2);return;}
    if(N.ToString().StartsWith(TEXT("c76_"))){ALWCampaign76::Ensure(P)->Click(N);return;}
    if(P->Story&&P->Story->Locked()){if(N==TEXT("story_skip")&&P->Story->InScene)P->Story->FinishScene();else if(N.ToString().StartsWith(TEXT("story_choice_")))P->Story->Choose(FCString::Atoi(*N.ToString().Mid(13)));return;}
    if(N==TEXT("story_journal_toggle")){P->ChapterJournal=!P->ChapterJournal;return;}
    if(N==TEXT("story_route")){if(P->Story)P->Story->Route();return;}
    if(P->OpeningMode){P->OpeningClick(N);return;}
    if(P->bWorldSetup){P->WorldSetupClick(N);return;}
    if(P->SecurityMode){if(N==TEXT("security_close"))P->ClosePanels();else if(N.ToString().StartsWith(TEXT("wire_")))P->ChooseWire(FCString::Atoi(*N.ToString().Mid(5)));return;}
    if(N.ToString().StartsWith(TEXT("tab_"))){P->SwitchTab(FCString::Atoi(*N.ToString().Mid(4)));DragId.Invalidate();return;}if(N==TEXT("tabs_close")){P->ClosePanels();return;}
    if(P->RPGPanel==6){CollectionClick36(P,N);return;}
    if(P->RPGPanel==5){CardClick(P,N);return;}
    if(RPGClick(P,N))return;
    if(P->bVideoConfirm&&N!=TEXT("ConfirmVideo")&&N!=TEXT("RevertVideo"))return;
    if(P->bSettings&&N.ToString().StartsWith(TEXT("settings78_"))){SettingsSection78=FCString::Atoi(*N.ToString().Mid(11));GraphicsPage=0;KeysPage51=-1;SettingsDrag55=-1;return;}
    if(P->bSettings&&N==TEXT("Settings")){P->ToggleSettings();GraphicsPage=0;KeysPage51=-1;return;}
    if(P->bSettings&&KeysClick51(N))return;
    if(P->bSettings&&N==TEXT("Graphics")){GraphicsPage=1;return;}
    if(P->bSettings&&N==TEXT("GraphicsBack")){GraphicsPage=0;return;}
    if(P->bSettings&&N==TEXT("GraphicsNext")){GraphicsPage=GraphicsPage>=4?1:GraphicsPage+1;return;}
    if(P->bSettings&&N.ToString().StartsWith(TEXT("pad58_"))){if(auto* K=ULWPlayerInput51::Get51(this))K->Change58(FCString::Atoi(*N.ToString().Mid(6)));return;}
    if(P->bSettings&&N.ToString().StartsWith(TEXT("gfx_"))){LWGraphics33::Change(FCString::Atoi(*N.ToString().Mid(4)));return;}
    if(N==TEXT("Start"))P->StartGame(true);
    if(N==TEXT("New"))P->BeginWorldSetup();
    if(N==TEXT("Settings")){P->ToggleSettings();GraphicsPage=0;KeysPage51=-1;}
    if(N==TEXT("Crust"))P->ToggleCrust();
    if(N==TEXT("Volume"))P->CycleVolume();
    if(N==TEXT("Sensitivity"))P->CycleSensitivity();
    if(N==TEXT("SensitivityDown"))P->LowerSensitivity();
    if(N==TEXT("LightingQuality"))LWLighting::Cycle();
    if(N==TEXT("Resolution"))P->CycleResolution();
    if(N==TEXT("WindowMode"))P->CycleWindowMode();
    if(N==TEXT("ApplyVideo"))P->ApplyVideo();
    if(N==TEXT("ConfirmVideo"))P->ConfirmVideo();
    if(N==TEXT("RevertVideo"))P->RevertVideo();
    if(N==TEXT("Quit")){P->SaveProgress();UKismetSystemLibrary::QuitGame(this,PlayerOwner,EQuitPreference::Quit,false);}
    if(N==TEXT("Restore"))P->Interact();
}
void ALWHUD::DrawHUD()
{
    Super::DrawHUD();if(!Canvas)return;
    ALWCharacter* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(!P||P->bDebug67)return;
    Scale=FMath::Min(Canvas->ClipY/720.f,Canvas->ClipX/1280.f);
    const float OldClipX=Canvas->ClipX,OldClipY=Canvas->ClipY;const int OldSizeX=Canvas->SizeX,OldSizeY=Canvas->SizeY;
    const bool Modal=P->IsUIOpen()||!P->bStarted||P->Health<=0;
    const bool TabMenu83=P->bInventory||P->bMap||P->RPGPanel==1||P->RPGPanel==2||P->RPGPanel==3||P->RPGPanel==6||P->RPGPanel==7;
    const bool WideMenu56=P->SettlementBuild82||TabMenu83||(!P->bStarted||P->bMenu)&&!P->bSettings&&!P->OpeningMode&&!P->SavePanel62;
    const bool Framed56=Modal&&!WideMenu56;
    Origin55=FVector2D(Framed56?(OldClipX-1280*Scale)*.5f:0,(OldClipY-720*Scale)*.5f);
    if(Framed56){
      const FLinearColor Matte(.007,.011,.014,1);
      if(Origin55.X>0){Super::DrawRect(Matte,0,0,Origin55.X,OldClipY);Super::DrawRect(Matte,OldClipX-Origin55.X,0,Origin55.X,OldClipY);}
      if(Origin55.Y>0){Super::DrawRect(Matte,0,0,OldClipX,Origin55.Y);Super::DrawRect(Matte,0,OldClipY-Origin55.Y,OldClipX,Origin55.Y);}
      Canvas->ClipX=1280*Scale;Canvas->ClipY=720*Scale;Canvas->SizeX=FMath::RoundToInt(Canvas->ClipX);Canvas->SizeY=FMath::RoundToInt(Canvas->ClipY);}
    Canvas->ClipY=720*Scale;Canvas->SizeY=FMath::RoundToInt(Canvas->ClipY);
    Frame55(P);
    Canvas->Canvas->PushRelativeTransform(FTranslationMatrix(FVector(Origin55.X,Origin55.Y,0)));
    ON_SCOPE_EXIT{Finish55();Canvas->Canvas->PopTransform();Canvas->ClipX=OldClipX;Canvas->ClipY=OldClipY;Canvas->SizeX=OldSizeX;Canvas->SizeY=OldSizeY;};
    const float Width=Canvas->ClipX/Scale;
    if(P->SettlementBuild82&&P->Settlement82){P->Settlement82->Draw(*this);return;}
    if(P->RPGPanel==7){ALWSettlement82::Ensure(P)->Draw(*this);PlayerTabs(P);return;}
    if((P->BaseUI45||P->BuildMode45)&&P->BunkerManager45){P->BunkerManager45->Draw(*this);return;}
    if(P->Workbench39&&P->Health>0){WorkbenchScreen39(P);return;}
    if(!P->bStarted&&!P->OpeningMode)MenuBackdrop81();
    if(P->OpeningMode==2){OpeningScreen(P);return;}
    // Pixel-raster overlays are drawn after the 3D filter to keep the display readable.
    if(!Modal)for(int32 Y=0;Y<720;Y+=4)Rect(0,Y,Width,1,FLinearColor(0,0,0,.025f));
    const float T=GetWorld()->GetRealTimeSeconds();
    
    if(P->OpeningMode){OpeningScreen(P);return;}
    if((P->Health<=0||(P->Story&&P->Story->Failed))&&P->HasMissionRecovery37()){
        Rect(0,0,Width,720,FLinearColor(.035f,.018f,.016f,.96f));
        Text(TEXT("MISSION FAILED"),Width*.5f-245,155,3.5f,Red);
        const auto& Recovery=P->MissionRecovery37.FindChecked(P->ActiveMission37());
        Text(Recovery.Title,Width*.5f-300,238,1.2f,Bone);
        Text(TEXT("RETRY RESTORES SAVED GEAR AND WORLD PROGRESS"),Width*.5f-300,281,.9f,Muted);
        Button(TEXT("mission_checkpoint"),TEXT("RESTART FROM CHECKPOINT"),Width*.5f-310,340,620);
        Button(TEXT("mission_restart"),TEXT("RESTART MISSION"),Width*.5f-310,399,620);
        Button(TEXT("mission_cancel"),TEXT("CANCEL MISSION / RETURN TO BUNKER"),Width*.5f-310,458,620);
        Text(TEXT("CANCEL RESTORES THE MISSION'S STARTING STATE"),Width*.5f-300,526,.9f,Muted);return;
    }
    if(StoryScreen(P))return;
    if(P->bSettings){Settings78(P);return;}
    if(P->bWorldSetup){NewSurvivor78(P);return;}
    if(P->SavePanel62){SaveScreen62(P);return;}
    if(!P->bStarted){MainMenu81(P);return;}
    if(P->bMenu){Menu78(P);return;}
    if(P->Health<=0)
    {
        Rect(0,0,Width,720,FLinearColor(.06f,.012f,.008f,.8f));
        Text(TEXT("SIGNAL LOST"),Width*.5f-320,230,5,Red);
        Text(TEXT("CARRIED GEAR DROPPED. BUNKER STASH SECURED."),Width*.5f-235,349,1.1f,Bone);
        Button(TEXT("Restore"),TEXT("[ENTER] RESPAWN IN BUNKER"),Width*.5f-225,425,450);
        return;
    }

    if(P->SecurityMode){SecurityScreen(P);return;}
    if(P->RPGPanel==6){CollectionScreen36(P);PlayerTabs(P);return;}
    if(P->RPGPanel==5){CardScreen(P);return;}
    if(P->RPGPanel){RPGScreen(P);if(P->RPGPanel<4)PlayerTabs(P);return;}
    if(P->bInventory){InventoryScreen(P);PlayerTabs(P);return;}
    if(P->bMap){Map(P);PlayerTabs(P);return;}
    const float Yaw=FMath::Fmod(P->GetControlRotation().Yaw+360,360.f);
    const TCHAR* Cardinal[]={TEXT("N"),TEXT("E"),TEXT("S"),TEXT("W")};
    for(int32 I=-4;I<=4;I++)
    {float X=Width*.5+I*38;Line(X,29,X,34,Muted);}
    Text(FString::Printf(TEXT("%s  %03d"),Cardinal[FMath::RoundToInt(Yaw/90)%4],FMath::RoundToInt(Yaw)),Width*.5f-31,42,.78f,Bone);

    const FString Place=P->World?P->World->LocationName:TEXT("EXCLUSION ZONE");
    Text(Place,24,25,.82f,Bone);
    if(P->World)
    {
        const int32 Hour=FMath::FloorToInt(P->World->TimeOfDay),Min=FMath::FloorToInt(FMath::Frac(P->World->TimeOfDay)*60);
        Text(FString::Printf(TEXT("%02d:%02d / DAY %02d"),Hour,Min,P->World->DayNumber),Width-181,26,.9f,Bone);
        if(P->World->WeatherType>=5){const TCHAR* WeatherNames[]={TEXT("DUST STORM"),TEXT("TORNADO WARNING"),TEXT("DENSE FOG"),TEXT("NUCLEAR WINTER"),TEXT("ASH FALL"),TEXT("BLIZZARD")};Text(WeatherNames[FMath::Clamp(P->World->WeatherType-5,0,5)],Width-181,43,.8f,Muted);}
    }
    const float CX=Width*.5,CY=360;
    if(auto* Enemy=P->CombatTarget.Get();IsValid(Enemy)&&!Enemy->bDead&&P->CombatTargetTime>0){FString Name=Enemy->EnemyName();Text(Name,CX-Name.Len()*4.2f,54,.95f,Enemy->Stars?Amber:Bone);Rect(CX-190,76,380,6,Muted*.3f);Rect(CX-190,76,380*FMath::Clamp(Enemy->Health/FMath::Max(1.f,Enemy->MaximumHealth),0.f,1.f),6,Red);}

    const bool MountedOptic=P->bAim&&(LWMods::OpenSight(P->ActiveGun())||LWMods::Scope(P->ActiveGun()));
    if(!MountedOptic&&!P->Vehicle&&P->Weapon>=2&&P->ActiveGun()){const float D=(P->bAim?10:21)*P->Spread54();for(int32 S:{-1,1}){Line(CX+S*D,CY-4,CX+S*D,CY+4,Bone);Line(CX-4,CY+S*D,CX+4,CY+S*D,Bone);}}
    else if(!MountedOptic){Rect(CX-1,CY-1,3,3,Bone);}
    if(P->bAim&&!P->Vehicle&&P->ActiveGun()){
    if(LWMods::Scope(P->ActiveGun())){
      const float Radius=235;const FLinearColor Black(0,0,0,1);
      for(int Y=0;Y<720;Y+=4){float DY=Y-CY;float DX=FMath::Abs(DY)<Radius?FMath::Sqrt(Radius*Radius-DY*DY):0;Rect(0,Y,CX-DX,4,Black);Rect(CX+DX,Y,Width-CX-DX,4,Black);}
      Line(CX-Radius,CY,CX-10,CY,Black);Line(CX+10,CY,CX+Radius,CY,Black);Line(CX,CY-Radius,CX,CY-10,Black);Line(CX,CY+10,CX,CY+Radius,Black);for(int I=1;I<5;I++){Line(CX-6,CY+I*32,CX+6,CY+I*32,Black);}
      if(LWMods::Has(P->ActiveGun(),TEXT("att_prism63"))){Line(CX-7,CY+6,CX,CY,Red,2);Line(CX,CY,CX+7,CY+6,Red,2);}
      else if(LWMods::Has(P->ActiveGun(),TEXT("att_marksman63"))){for(int K=-5;K<=5;K++)if(K){Rect(CX+K*28-2,CY-2,4,4,Black);Rect(CX-2,CY+K*28-2,4,4,Black);}}
      else if(LWMods::Has(P->ActiveGun(),TEXT("att_combat63"))){for(int K=1;K<5;K++)Line(CX-16+K*2,CY+K*30,CX+16-K*2,CY+K*30,Red);}
      Rect(CX-1,CY-1,2,2,Red);
    }else if(LWMods::OpenSight(P->ActiveGun())){
      if(LWMods::Has(P->ActiveGun(),TEXT("att_tube63")))for(int K=0;K<32;K++){float A=K*2*PI/32,B=(K+1)*2*PI/32;Line(CX+FMath::Cos(A)*12,CY+FMath::Sin(A)*12,CX+FMath::Cos(B)*12,CY+FMath::Sin(B)*12,Red,1);}
      Rect(CX-3,CY-3,6,6,FLinearColor(0,0,0,.8f));
      Rect(CX-1.5f,CY-1.5f,3,3,FLinearColor(1,.08f,.025f,1));
    }
    }
    if(P->HitMarker>0)for(int32 S:{-1,1})for(int32 D:{-1,1})Line(CX+S*5,CY+D*5,CX+S*11,CY+D*11,Amber,2);
    const FString Prompt=P->FocusPrompt();if(!Prompt.IsEmpty())Text(Prompt,CX-Prompt.Len()*4.05,410,.9f,Amber);
    if(!P->DungeonStatus.IsEmpty()&&!P->bInventory&&!P->bMap&&!P->RPGPanel)Text(P->DungeonStatus,Width*.5f-P->DungeonStatus.Len()*3.2f,87,.75f,Amber);
 if(P->DiscoveryAlert>0&&!P->bInventory&&!P->bMap){const float A=FMath::Clamp(P->DiscoveryAlert,0.f,1.f);Text(TEXT("LOCATION DISCOVERED"),CX-100,142,1.1f,FLinearColor(.9,.8,.5,A));Text(P->DiscoveryTitle,CX-P->DiscoveryTitle.Len()*6.3f,173,1.4f,FLinearColor(.92,.9,.73,A));Text(TEXT("+35 XP"),CX-28,206,.9f,FLinearColor(.9,.8,.5,A));}
    if(P->MessageTime>0)Text(P->Message,CX-P->Message.Len()*3.85,490,.85f,Amber);
    Rect(24,617,238,75,FLinearColor(.009,.017,.022,.68));
    auto Bar=[&](const TCHAR* Label,float Value,float Max,float Y,FLinearColor C){Text(Label,36,Y,.76,C);Rect(75,Y+6,132,4,FLinearColor(.1,.16,.18,.8));Rect(75,Y+6,132*Animate55(FName(Label),FMath::Clamp(Value/Max,0.f,1.f),12),4,C);Text(FString::Printf(TEXT("%d"),FMath::CeilToInt(Value)),218,Y,.76,C);};
    Bar(TEXT("HP"),P->Health,P->MaxHealth(),628,P->Health<30?Red:Bone);Bar(TEXT("STA"),P->Stamina,P->MaxStamina(),650,Muted);
    Text(FString::Printf(TEXT("FOOD %d   WATER %d"),FMath::CeilToInt(P->Hunger),FMath::CeilToInt(P->Thirst)),36,677,.72,Muted);
    const float WX=Width-383;
    Rect(WX-12,617,359,75,FLinearColor(.009,.017,.022,.68));
    Text(FString::Printf(TEXT("%s %lld"),LWGeography84::Canada(FVector2D(P->GetActorLocation()))?TEXT("CAD $"):TEXT("CR"),LWGeography84::Canada(FVector2D(P->GetActorLocation()))?P->RPG.Canada68.CanadianDollars:P->Money),WX,596,.82,Bone);
    const FString Equipped=P->Vehicle?FString(P->Vehicle->Spec().Name):P->WeaponName();
    Text(Equipped,WX,628,FMath::Min(.88f,333.f/FMath::Max(1.f,Equipped.Len()*9.f)),P->Vehicle?Amber:LWMods::TierColor(LWMods::Tier(P->ActiveGun())));
    if(!P->Vehicle&&P->Weapon>=2){FString Status=P->AmmoStatus();int32 Split=Status.Find(TEXT(" // "),ESearchCase::CaseSensitive,ESearchDir::FromEnd);if(Status.Len()>43&&Split>=0){Text(Status.Left(Split),WX,651,.76,Bone);Text(Status.Mid(Split+4),WX,676,.72,Muted);}else Text(Status,WX,657,.76,Bone);}
    if(!P->Vehicle)WeaponSlots46(P);
    else Text(P->Vehicle->IsHelicopter57()?TEXT("[SPACE BAR] ASCEND   [LEFT CTRL] DESCEND   [R] ENGINE"):P->Vehicle->HasTurret57()&&P->Vehicle->PlayerSeat==0?TEXT("[LEFT MOUSE BUTTON] FIRE   [R] RELOAD"):P->Vehicle->PlayerSeat==-2?TEXT("WASD WALK   [E] USE   1-5 SEATS"):P->Vehicle->PlayerSeat==-1?TEXT("W/S DRIVE   A/D STEER   SPACE BRAKE   [R] IGNITION"):TEXT("[E] USE   1-8 / WHEEL SEATS"),Width*.5f-260,703,.72,Muted);
    MiniMap(P);RPGOverlay(P);QuickLoot54(P);if(P->Vehicle)VehicleOverlay(P);
    if(P->HurtFlash>0){Rect(0,0,Width,13,FLinearColor(.55f,.02f,0,P->HurtFlash));Rect(0,707,Width,13,FLinearColor(.55f,.02f,0,P->HurtFlash));}
}

void ALWHUD::PlayerTabs(ALWCharacter* P){
 const float W=Canvas->ClipX/Scale;Rect(16,12,W-32,50,FLinearColor(.009,.017,.022,1));
 const TCHAR* Names[]={TEXT("Inventory"),TEXT("Map"),TEXT("Skills"),TEXT("Contracts"),TEXT("Crew"),TEXT("Collection"),TEXT("Settlements")};
 const int Active=P->bInventory?0:P->bMap?1:P->RPGPanel==7?6:P->RPGPanel==6?5:P->RPGPanel+1;
 const float Cell=(W-186)/7;
 for(int I=0;I<7;I++)Control55(FName(*FString::Printf(TEXT("tab_%d"),I)),Names[I],24+I*Cell,16,Cell-8,38,I==Active);
 Rect(Animate55(TEXT("tab_position55"),24+Active*Cell,20),57,Cell-8,2,FLinearColor(.98,.59,.28));
 Control55(TEXT("tabs_close"),TEXT("Close"),W-146,16,122,38);
}
