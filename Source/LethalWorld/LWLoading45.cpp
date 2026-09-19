#include "LWLoading45.h"
#include "MoviePlayer.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SThrobber.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
namespace {
bool Enabled45(){return !IsRunningCommandlet()&&(!FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWLoading45Smoke")))&&IsMoviePlayerEnabled();}
FLoadingScreenAttributes Screen45(const FString& Label,bool Manual){
 FLoadingScreenAttributes A;A.bAutoCompleteWhenLoadingCompletes=!Manual;A.bWaitForManualStop=Manual;A.bAllowEngineTick=false;A.MinimumLoadingScreenDisplayTime=Manual?0:.5f;
 static TSharedPtr<FSlateDynamicImageBrush> Art;
 if(!Art){const FString File=FPaths::ProjectContentDir()/TEXT("UI55/T_Menu55.png");Art=MakeShared<FSlateDynamicImageBrush>(FName(*File),FVector2D(1672,941));}
 const TCHAR* Tips[]={TEXT("Store supplies in your bunker before a dangerous trip."),TEXT("Cover breaks line of sight. Move before the enemy flanks you."),TEXT("A loaded magazine stays with its weapon when transferred."),TEXT("Aim at an unlocked container to loot without opening your inventory.")};
 const FString Tip=Tips[int(FPlatformTime::Seconds())%4];
 A.WidgetLoadingScreen=SNew(SOverlay)
 +SOverlay::Slot()[SNew(SImage).Image(Art.Get())]
 +SOverlay::Slot()[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")).BorderBackgroundColor(FLinearColor(.006,.012,.018,.67))]
 +SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Bottom).Padding(64,40,64,64)
 [SNew(SVerticalBox)
 +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,10)[SNew(STextBlock).Text(FText::FromString(TEXT("ALL AMERICAN"))).Font(FCoreStyle::GetDefaultFontStyle("Regular",22)).ColorAndOpacity(FLinearColor(.81,.86,.84))]
 +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,38)[SNew(STextBlock).Text(FText::FromString(TEXT("MELTDOWN"))).Font(FCoreStyle::GetDefaultFontStyle("Bold",52)).ColorAndOpacity(FLinearColor(.94,.67,.34))]
 +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,18)[SNew(STextBlock).Text(FText::FromString(Label)).Font(FCoreStyle::GetDefaultFontStyle("Regular",20)).ColorAndOpacity(FLinearColor(.89,.92,.88))]
 +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Left).Padding(0,0,0,25)[SNew(SThrobber).NumPieces(5)]
 +SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromString(Tip)).Font(FCoreStyle::GetDefaultFontStyle("Regular",15)).ColorAndOpacity(FLinearColor(.64,.72,.73))]];

 return A;
}
}
void ULWLoading45::Init(){Super::Init();FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ULWLoading45::BeforeMap);}
void ULWLoading45::Shutdown(){FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);Super::Shutdown();}
void ULWLoading45::BeforeMap(const FString&){if(Enabled45())GetMoviePlayer()->SetupLoadingScreen(Screen45(TEXT("Loading world"),false));}
FLWLoadingScope45::FLWLoadingScope45(const FString& Label){
 if(Enabled45()&&!GetMoviePlayer()->IsMovieCurrentlyPlaying()){GetMoviePlayer()->SetupLoadingScreen(Screen45(Label,true));Active=GetMoviePlayer()->PlayMovie();}
}
FLWLoadingScope45::~FLWLoadingScope45(){if(Active){GetMoviePlayer()->StopMovie();GetMoviePlayer()->WaitForMovieToFinish(false);}}

