#include "LWLoading45.h"
#include "LWLoading79.h"
#include "MoviePlayer.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
namespace {
bool Enabled45(){return !IsRunningCommandlet()&&(!FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWLoading45Smoke")))&&IsMoviePlayerEnabled();}
FLoadingScreenAttributes Screen45(const FString& Label,bool Manual){
 FLoadingScreenAttributes A;A.bAutoCompleteWhenLoadingCompletes=!Manual;A.bWaitForManualStop=Manual;A.bAllowEngineTick=false;A.MinimumLoadingScreenDisplayTime=Manual?0:.5f;
 A.WidgetLoadingScreen=LWLoading79::Create(Label);
 return A;
}
}
void ULWLoading45::Init(){Super::Init();if(Enabled45())LWLoading79::Preload();FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ULWLoading45::BeforeMap);}
void ULWLoading45::Shutdown(){FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);Super::Shutdown();}
void ULWLoading45::BeforeMap(const FString&){if(Enabled45())GetMoviePlayer()->SetupLoadingScreen(Screen45(TEXT("Loading world"),false));}
FLWLoadingScope45::FLWLoadingScope45(const FString& Label){
 if(Enabled45()&&!GetMoviePlayer()->IsMovieCurrentlyPlaying()){GetMoviePlayer()->SetupLoadingScreen(Screen45(Label,true));Active=GetMoviePlayer()->PlayMovie();}
}
FLWLoadingScope45::~FLWLoadingScope45(){if(Active){GetMoviePlayer()->StopMovie();GetMoviePlayer()->WaitForMovieToFinish(false);}}

