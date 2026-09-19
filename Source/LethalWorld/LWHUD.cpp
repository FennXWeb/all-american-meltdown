#include "LWPlayerInput51.h"
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

namespace {const FLinearColor Bone(.79f,.80f,.66f),Muted(.39f,.45f,.37f),Amber(.85f,.48f,.20f),Red(.62f,.17f,.11f),Ink(.018f,.029f,.022f,.89f);}
void ALWHUD::Rect(float X,float Y,float W,float H,FLinearColor C){
 if(W<=0||H<=0)return;
 if(W>=110&&H>=35&&C.A>.45f&&FMath::Max3(C.R,C.G,C.B)>.003f&&FMath::Max3(C.R,C.G,C.B)<.32f){SurfacePanel55(X,Y,W,H,C);return;}
 DrawRect(C,X*Scale,Y*Scale,W*Scale,H*Scale);
}
void ALWHUD::Line(float X,float Y,float X2,float Y2,FLinearColor C,float Width){DrawLine(X*Scale,Y*Scale,X2*Scale,Y2*Scale,C,FMath::Max(1.f,Width*Scale));}
void ALWHUD::Text(const FString& InStr,float X,float Y,float Size,FLinearColor Color)
{
    FString Str=InStr;
    if(Str.Contains(TEXT("["))){auto* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(P&&(!P->IsUIOpen()||P->bMap))if(auto* K=ULWPlayerInput51::Get51(this)){K->Load51();FString Out;for(int I=0;I<InStr.Len();++I){if(InStr[I]=='['){int End=I+1;while(End<InStr.Len()&&InStr[End]!=']')++End;if(End<InStr.Len()){FString Token=InStr.Mid(I+1,End-I-1);for(const auto& B:K->Bindings)if(Token==B.Logical.GetDisplayName().ToString().ToUpper()){Token=B.Physical.GetDisplayName().ToString().ToUpper();break;}Out+=TEXT("[")+Token+TEXT("]");I=End;continue;}}Out.AppendChar(InStr[I]);}Str=MoveTemp(Out);}}
    Color.A*=.65f+.35f*Reveal55;
    Size=FMath::Max(Size,.72f);
    if(!UIFont55){DrawText(Str,Color,X*Scale,Y*Scale,nullptr,Size*Scale);return;}
    TArray<FString> Lines;Str.ParseIntoArray(Lines,TEXT("\n"),false);
    for(const auto& S:Lines){
      if(!S.IsEmpty()){
       float TW=0,TH=0;GetTextSize(S,TW,TH,UIFont55,1);
       FCanvasTextItem Item(FVector2D(X,Y)*Scale,FText::FromString(S),UIFont55,Color);
       Item.Scale=FVector2D(9*Size*S.Len()*Scale/FMath::Max(1.f,TW),17*Size*Scale/FMath::Max(1.f,TH));
       Item.EnableShadow(FLinearColor(0,0,0,.65f),FVector2D(1,1));Canvas->DrawItem(Item);
      }Y+=20*Size;
    }
}
void ALWHUD::Button(FName Id,const FString& Label,float X,float Y,float Width){
 if(auto* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());P&&P->BaseUI45&&P->BunkerManager45){P->BunkerManager45->TerminalButton46(*this,Id,Label,X,Y,Width);return;}
 Control55(Id,Label,X,Y,Width);
}
void ALWHUD::NotifyHitBoxClick(FName N)
{
    Super::NotifyHitBoxClick(N);ALWCharacter* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(!P)return;
    // Own this press before a callback can close or replace its panel.
    P->ConsumeUIAttack();
    if(UIClick55)UGameplayStatics::PlaySound2D(this,UIClick55,.22f,1.1f);
    if(P->bInventory&&(N==TEXT("inv_sort55")||N==TEXT("inv_other55")||N==TEXT("inv_all55"))){DragId.Invalidate();if(N==TEXT("inv_sort55"))P->SortInventory();else if(N==TEXT("inv_all55"))P->TakeAll54();else P->SortInventory(IsValid(P->OpenObject)&&P->OpenObject->Kind==ELWObjectKind::Stash?1:2);return;}
    if(N==TEXT("ui_motion55")){ReducedMotion55=!ReducedMotion55;SaveUI55();return;}
    if(N==TEXT("ui_contrast55")){HighContrast55=!HighContrast55;SaveUI55();return;}
if(P->BaseUI45&&P->BunkerManager45){P->BunkerManager45->Action(N);return;}
    if(P->Workbench39){WorkbenchClick39(P,N);return;}
    if(N==TEXT("mission_checkpoint")){P->RecoverMission37(0);return;}
    if(N==TEXT("mission_restart")){P->RecoverMission37(1);return;}
    if(N==TEXT("mission_cancel")){P->RecoverMission37(2);return;}
    if(P->Story&&P->Story->Locked()){if(N==TEXT("story_skip")&&P->Story->InScene)P->Story->FinishScene();else if(N.ToString().StartsWith(TEXT("story_choice_")))P->Story->Choose(FCString::Atoi(*N.ToString().Mid(13)));return;}
    if(N==TEXT("story_start")){bool Resume=P->RPG.Story.Stage>0;P->RPG.Story.Enabled=true;ALWStoryDirector::Ensure(P)->Start(!Resume);P->RequestSave40();return;}
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
    if(P->bSettings&&KeysClick51(N))return;
    if(P->bSettings&&N==TEXT("Graphics")){GraphicsPage=1;return;}
    if(P->bSettings&&N==TEXT("GraphicsBack")){GraphicsPage=0;return;}
    if(P->bSettings&&N==TEXT("GraphicsNext")){GraphicsPage=GraphicsPage==1?2:1;return;}
    if(P->bSettings&&N.ToString().StartsWith(TEXT("gfx_"))){LWGraphics33::Change(FCString::Atoi(*N.ToString().Mid(4)));return;}
    if(N==TEXT("Start"))P->StartGame(true);
    if(N==TEXT("New"))P->BeginWorldSetup();
    if(N==TEXT("Settings"))P->ToggleSettings();
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
    ALWCharacter* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(!P)return;
    Scale=FMath::Min(Canvas->ClipY/720.f,Canvas->ClipX/1280.f);
    const float OldClipX=Canvas->ClipX,OldClipY=Canvas->ClipY;const int OldSizeX=Canvas->SizeX,OldSizeY=Canvas->SizeY;
    const bool Modal=P->IsUIOpen()||!P->bStarted||P->Health<=0;
    const bool WideMenu56=(!P->bStarted||P->bMenu)&&!P->bSettings&&!P->OpeningMode;
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
    if((P->BaseUI45||P->BuildMode45)&&P->BunkerManager45){P->BunkerManager45->Draw(*this);return;}
    if(P->Workbench39&&P->Health>0){WorkbenchScreen39(P);return;}
    if(!P->bStarted&&!P->OpeningMode)MenuBackdrop46();
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
    if(P->bSettings)
    {
        Rect(0,0,Width,720,Ink);Text(TEXT("SETTINGS"),60,60,3,Amber);Line(60,120,Width-60,120,Muted);
        if(KeysPage51>=0){KeysScreen51();return;}
        if(GraphicsPage){
            Text(GraphicsPage==1?TEXT("GRAPHICS / QUALITY"):TEXT("GRAPHICS / DISPLAY & EFFECTS"),60,152,1.3f,Bone);
            const int Quality[]={10,0,1,2,3,4,5,6,7,8,9,22},Display[]={11,12,13,14,15,16,17,18,19,20,21};
            for(int Row=0;Row<(GraphicsPage==1?12:11);Row++){int I=GraphicsPage==1?Quality[Row]:Display[Row];Button(FName(*FString::Printf(TEXT("gfx_%d"),I)),LWGraphics33::Label(I),60+(Row/6)*600,205+(Row%6)*54,555);}
            Text(TEXT("CHANGES APPLY AND SAVE IMMEDIATELY"),60,548,.9f,Muted);
            Text(TEXT("LUMEN CONTROLS INDIRECT LIGHT AND REFLECTIONS. TSR IMPROVES LOW RENDER SCALES."),60,574,.8f,Muted);
            Button(TEXT("GraphicsBack"),TEXT("BACK"),60,620,350);Button(TEXT("GraphicsNext"),GraphicsPage==1?TEXT("DISPLAY & EFFECTS >"):TEXT("< QUALITY"),660,620,555);return;
        }
        Text(TEXT("INPUT / PRESENTATION"),60,160,1.2f,Bone);Text(TEXT("VIDEO OUTPUT"),680,160,1.2f,Bone);
        Text(FString::Printf(TEXT("MOUSE SENSITIVITY   %.3f"),P->Sensitivity),60,205,1.2f,Amber);
        Button(TEXT("SensitivityDown"),TEXT("- LOWER"),60,246,230);Button(TEXT("Sensitivity"),TEXT("+ HIGHER"),310,246,230);
        Text(TEXT("RANGE 0.025 - 5.000 // SAVED AUTOMATICALLY"),60,302,.8f,Muted);
        SettingsSlider55(P,0,60,326,480);
        Button(TEXT("Volume"),FString::Printf(TEXT("MASTER VOLUME: %d%%"),FMath::RoundToInt(P->MasterVolume*100)),60,346,480);
        SettingsSlider55(P,1,60,395,480);
        Button(TEXT("Crust"),FString(TEXT("CAMERA DECAY: "))+(P->bCrust?TEXT("ON"):TEXT("OFF")),60,404,480);
        Button(TEXT("Resolution"),FString::Printf(TEXT("RESOLUTION: %d X %d"),P->PendingResolution.X,P->PendingResolution.Y),680,204,510);
        Button(TEXT("WindowMode"),TEXT("MODE: ")+P->WindowModeName(),680,263,510);
        Button(TEXT("ApplyVideo"),TEXT("APPLY VIDEO SETTINGS"),680,342,510);
        Text(TEXT("BORDERLESS USES THE DESKTOP RESOLUTION."),680,402,.85f,Muted);
        Text(TEXT("VIDEO CHANGES REVERT AFTER 15 SECONDS\nUNLESS YOU CONFIRM THE NEW DISPLAY."),680,432,.85f,Muted);
        Button(TEXT("LightingQuality"),LWLighting::Label(),680,493,510);
        Text(LWLighting::HardwareAvailable()?TEXT("HARDWARE RAY TRACING AVAILABLE"):TEXT("HARDWARE RAY TRACING UNAVAILABLE"),680,542,.8f,Muted);
        Button(TEXT("Graphics"),TEXT("ADVANCED GRAPHICS"),60,493,480);
        Button(TEXT("keys51"),TEXT("KEY BINDINGS"),60,542,480);
        Button(TEXT("Settings"),TEXT("RETURN"),60,650,480);
        Button(TEXT("ui_motion55"),ReducedMotion55?TEXT("UI MOTION: REDUCED"):TEXT("UI MOTION: FULL"),60,595,480);
        Button(TEXT("ui_contrast55"),HighContrast55?TEXT("UI CONTRAST: HIGH"):TEXT("UI CONTRAST: STANDARD"),680,595,510);
        if(P->bVideoConfirm){
            Rect(350,205,580,330,FLinearColor(.04,.065,.04,.99));
            Text(TEXT("KEEP THIS DISPLAY MODE?"),380,240,1.7f,Amber);
            Text(FString::Printf(TEXT("REVERTING IN %02d SECONDS"),FMath::Max(0,FMath::CeilToInt(P->VideoConfirmTimer-GetWorld()->GetRealTimeSeconds()))),380,291,1,Bone);
            Button(TEXT("ConfirmVideo"),TEXT("KEEP CHANGES"),390,350,500);Button(TEXT("RevertVideo"),TEXT("REVERT"),390,415,500);
        }
        return;
    }
    if(P->bWorldSetup){
        Rect(0,0,Width,720,FLinearColor(.025f,.035f,.025f,.98f));Text(TEXT("NEW WORLD"),80,65,2.5f,Bone);
        Text(TEXT("YOUR CURRENT SAVE IS REPLACED WHEN YOU FINISH CHARACTER CREATION"),80,120,.85f,Amber);
        Button(TEXT("seed"),TEXT("SEED: ")+P->SeedText+(P->bSeedEdit?TEXT("_ (TYPE DIGITS)"):TEXT("")),80,170,620);
        Button(TEXT("randomseed"),TEXT("RANDOM SEED"),740,170,350);
        const TCHAR* Density[]={TEXT("SPARSE"),TEXT("STANDARD"),TEXT("DENSE")};const TCHAR* Terrain[]={TEXT("LOWLANDS"),TEXT("ROLLING"),TEXT("RUGGED")};const TCHAR* Difficulty[]={TEXT("EASY"),TEXT("NORMAL"),TEXT("HARD")};
        Button(TEXT("towns"),FString(TEXT("SETTLEMENTS: "))+Density[P->SetupTowns],80,230,620);
        Button(TEXT("pois"),FString(TEXT("BUILDINGS: "))+Density[P->SetupPOIs],80,290,620);
        Button(TEXT("terrain"),FString(TEXT("TERRAIN: "))+Terrain[P->SetupTerrain],80,350,620);
        Button(TEXT("difficulty"),FString(TEXT("DIFFICULTY: "))+Difficulty[P->SetupDifficulty],80,410,620);
        Text(TEXT("Difficulty affects damage, survival needs and enemy numbers."),80,470,.8f,Muted);
        Button(TEXT("createworld"),TEXT("CONTINUE"),80,530,400);Button(TEXT("setupback"),TEXT("BACK"),520,530,300);return;
    }
    if(!P->bStarted||P->bMenu)
    {
        Rect(0,0,Width,720,FLinearColor(.025f,.04f,.024f,.26f));
        for(int32 I=0;I<16;I++)DrawRect(FLinearColor(.006f,.012f,.018f,.48f-I*.028f),I*32*Scale,0,33*Scale,720*Scale);
        Line(56,389,446,389,FLinearColor(.72,.47,.22,.5));
        Text(P->bStarted?TEXT("PAUSED"):TEXT("MAIN MENU"),56,26,.85f,Muted);
        
        Logo(42,68,550,280,!P->bStarted&&!ReducedMotion55);
        if(P->bSettings)
        {
            Button(TEXT("Crust"),FString(TEXT("CAMERA DECAY: "))+(P->bCrust?TEXT("ON"):TEXT("OFF")),56,417);
            Button(TEXT("Volume"),FString::Printf(TEXT("MASTER VOLUME: %d%%"),FMath::RoundToInt(P->MasterVolume*100)),56,467);
            Button(TEXT("Sensitivity"),FString::Printf(TEXT("MOUSE SENSITIVITY: %.3f"),P->Sensitivity),56,517);
            Button(TEXT("Settings"),TEXT("RETURN"),56,567);
        }
        else
        {
            Button(TEXT("Start"),P->bStarted?TEXT("RESUME"):TEXT("CONTINUE"),56,417);
            Button(TEXT("New"),TEXT("NEW SURVIVOR"),56,467);
            Button(TEXT("Settings"),TEXT("SETTINGS"),56,517);
            Button(TEXT("Quit"),TEXT("QUIT"),56,567);
        }
        Text(TEXT("ARROWS SELECT     ENTER CONFIRM"),56,637,.8f,Muted);
        if(P->bStarted)Text(P->Identity.Name,Width-320,630,1.2f,Bone);
        return;
    }
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

    const bool MountedOptic=P->bAim&&(LWMods::OpenSight(P->ActiveGun())||LWMods::Has(P->ActiveGun(),TEXT("att_scope4"))||LWMods::Has(P->ActiveGun(),TEXT("att_scope8")));
    if(!MountedOptic&&!P->Vehicle&&P->Weapon>=2&&P->ActiveGun()){const float D=(P->bAim?10:21)*P->Spread54();for(int32 S:{-1,1}){Line(CX+S*D,CY-4,CX+S*D,CY+4,Bone);Line(CX-4,CY+S*D,CX+4,CY+S*D,Bone);}}
    else if(!MountedOptic){Rect(CX-1,CY-1,3,3,Bone);}
    if(P->bAim&&!P->Vehicle&&P->ActiveGun()){
    if(LWMods::Has(P->ActiveGun(),TEXT("att_scope4"))||LWMods::Has(P->ActiveGun(),TEXT("att_scope8"))){
      const float Radius=235;const FLinearColor Black(0,0,0,1);
      for(int Y=0;Y<720;Y+=4){float DY=Y-CY;float DX=FMath::Abs(DY)<Radius?FMath::Sqrt(Radius*Radius-DY*DY):0;Rect(0,Y,CX-DX,4,Black);Rect(CX+DX,Y,Width-CX-DX,4,Black);}
      Line(CX-Radius,CY,CX-10,CY,Black);Line(CX+10,CY,CX+Radius,CY,Black);Line(CX,CY-Radius,CX,CY-10,Black);Line(CX,CY+10,CX,CY+Radius,Black);for(int I=1;I<5;I++){Line(CX-6,CY+I*32,CX+6,CY+I*32,Black);}
      Rect(CX-1,CY-1,2,2,Red);
    }else if(LWMods::OpenSight(P->ActiveGun())){
      Rect(CX-3,CY-3,6,6,FLinearColor(0,0,0,.8f));
      Rect(CX-1.5f,CY-1.5f,3,3,FLinearColor(1,.08f,.025f,1));
    }
    }
    if(P->HitMarker>0)for(int32 S:{-1,1})for(int32 D:{-1,1})Line(CX+S*5,CY+D*5,CX+S*11,CY+D*11,Amber,2);
    const FString Prompt=P->FocusPrompt();if(!Prompt.IsEmpty())Text(Prompt,CX-Prompt.Len()*4.05,410,.9f,Amber);
    if(!P->DungeonStatus.IsEmpty()&&!P->bInventory&&!P->bMap&&!P->RPGPanel)Text(P->DungeonStatus,Width*.5f-P->DungeonStatus.Len()*3.2f,87,.75f,Amber);
 if(P->DiscoveryAlert>0&&!P->bInventory&&!P->bMap){const float A=FMath::Clamp(P->DiscoveryAlert,0.f,1.f);Text(TEXT("LOCATION DISCOVERED"),CX-100,142,1.1f,FLinearColor(.9,.8,.5,A));Text(P->DiscoveryTitle,CX-P->DiscoveryTitle.Len()*6.3f,173,1.4f,FLinearColor(.92,.9,.73,A));Text(TEXT("+35 XP"),CX-28,206,.9f,FLinearColor(.9,.8,.5,A));}
    if(P->MessageTime>0)Text(P->Message,CX-P->Message.Len()*3.85,490,.85f,Amber);
    // Thin edge strips retain exact values without the old large opaque cards.
    Rect(18,615,268,77,FLinearColor(.015,.025,.019,.60));
    auto Bar=[&](const TCHAR* Label,float Value,float Max,float Y,FLinearColor C){Text(Label,27,Y,.8,C);Rect(82,Y+6,128,3,Muted*.35);Rect(82,Y+6,128*Animate55(FName(Label),FMath::Clamp(Value/Max,0.f,1.f),12),3,C);Text(FString::Printf(TEXT("%d"),FMath::CeilToInt(Value)),224,Y,.8,C);};
    Bar(TEXT("HP"),P->Health,P->MaxHealth(),623,P->Health<30?Red:Bone);Bar(TEXT("STA"),P->Stamina,P->MaxStamina(),645,Muted);
    Text(FString::Printf(TEXT("FOOD %03d   WATER %03d"),FMath::CeilToInt(P->Hunger),FMath::CeilToInt(P->Thirst)),27,670,.8,Muted);
    const float WX=Width-420;
    Rect(WX-10,615,412,77,FLinearColor(.015,.025,.019,.60));Rect(WX-10,588,165,24,FLinearColor(.015,.025,.019,.65));Text(FString::Printf(TEXT("$ %lld"),P->Money),WX,593,.9,Bone);
    Text(P->Vehicle?FString(P->Vehicle->Spec().Name):P->WeaponName(),WX,623,.85,P->Vehicle?Amber:LWMods::TierColor(LWMods::Tier(P->ActiveGun())));
    if(!P->Vehicle&&P->Weapon>=2){FString Status=P->AmmoStatus();int32 Split=Status.Find(TEXT(" // "),ESearchCase::CaseSensitive,ESearchDir::FromEnd);if(Status.Len()>50&&Split>=0){Text(Status.Left(Split),WX,646,.8,Bone);Text(Status.Mid(Split+4),WX,670,.8,Muted);}else Text(Status,WX,650,.8,Bone);}
    if(!P->Vehicle)WeaponSlots46(P);
    else Text(P->Vehicle->PlayerSeat==-2?TEXT("WASD WALK  E USE  1-5 SEATS  F EXIT"):P->Vehicle->PlayerSeat==-1?TEXT("W/S DRIVE  A/D STEER  SPACE BRAKE  R IGNITION"):TEXT("E USE  1-8 / WHEEL SEATS  F STOP / EXIT"),24,703,.8,Muted);
    Text(P->Vehicle?TEXT("I GEAR  [TAB] MAP  B HOME"):TEXT("I GEAR  V FIRE MODE  [TAB] MAP  B HOME"),Width-320,703,.8,Muted);
    MiniMap(P);RPGOverlay(P);QuickLoot54(P);if(P->Vehicle)VehicleOverlay(P);
    if(P->HurtFlash>0){Rect(0,0,Width,13,FLinearColor(.55f,.02f,0,P->HurtFlash));Rect(0,707,Width,13,FLinearColor(.55f,.02f,0,P->HurtFlash));}
}

void ALWHUD::PlayerTabs(ALWCharacter* P){float Left=FMath::Max(0.f,(Canvas->ClipX/Scale-1280.f)*.5f);Rect(Left+16,14,1248,48,FLinearColor(.02f,.035f,.025f,1));const TCHAR* Names[]={TEXT("INVENTORY"),TEXT("MAP"),TEXT("SKILLS"),TEXT("CONTRACTS"),TEXT("CREW"),TEXT("COLLECTION")};int Active=P->bInventory?0:P->bMap?1:P->RPGPanel==6?5:P->RPGPanel+1;for(int I=0;I<6;I++){float X=Left+28+I*176;Button(FName(*FString::Printf(TEXT("tab_%d"),I)),Names[I],X,16,168);}Rect(Animate55(TEXT("tab_position55"),Left+28+Active*176,20),57,168,3,FLinearColor(.95,.65,.30));Button(TEXT("tabs_close"),TEXT("CLOSE"),Left+1100,16,150);}
