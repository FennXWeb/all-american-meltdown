#include "LWGraphics33.h"
#include "LWLighting.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"
namespace LWGraphics33 { void Load58();void Change58(int);void Custom58(int);FString Label58(int);
namespace {
int AA=0;bool Blur=false,Bloom=true,AO=true;int Anisotropy=8;float Sharpness=.4f;
void Set(const TCHAR* N,float V){if(auto* C=IConsoleManager::Get().FindConsoleVariable(N))C->Set(V,ECVF_SetByCode);}
void Effects(){Set(TEXT("r.AntiAliasingMethod"),AA);Set(TEXT("r.MotionBlurQuality"),Blur?3:0);Set(TEXT("r.DefaultFeature.MotionBlur"),Blur?1:0);Set(TEXT("r.Tonemapper.Sharpen"),Sharpness);Set(TEXT("r.Upscale.Quality"),AA==4?3:1);Set(TEXT("r.BloomQuality"),Bloom?5:0);Set(TEXT("r.AmbientOcclusionLevels"),AO?-1:0);Set(TEXT("r.MaxAnisotropy"),Anisotropy);}
using Setter=void(UGameUserSettings::*)(int32);using Getter=int32(UGameUserSettings::*)()const;
const Setter Setters[]={&UGameUserSettings::SetViewDistanceQuality,&UGameUserSettings::SetTextureQuality,&UGameUserSettings::SetShadowQuality,&UGameUserSettings::SetVisualEffectQuality,&UGameUserSettings::SetFoliageQuality,&UGameUserSettings::SetPostProcessingQuality,&UGameUserSettings::SetGlobalIlluminationQuality,&UGameUserSettings::SetReflectionQuality,&UGameUserSettings::SetShadingQuality,&UGameUserSettings::SetAntiAliasingQuality};
const Getter Getters[]={&UGameUserSettings::GetViewDistanceQuality,&UGameUserSettings::GetTextureQuality,&UGameUserSettings::GetShadowQuality,&UGameUserSettings::GetVisualEffectQuality,&UGameUserSettings::GetFoliageQuality,&UGameUserSettings::GetPostProcessingQuality,&UGameUserSettings::GetGlobalIlluminationQuality,&UGameUserSettings::GetReflectionQuality,&UGameUserSettings::GetShadingQuality,&UGameUserSettings::GetAntiAliasingQuality};
const TCHAR* Names[]={TEXT("VIEW DISTANCE"),TEXT("TEXTURES"),TEXT("SHADOWS"),TEXT("EFFECTS"),TEXT("FOLIAGE"),TEXT("POST PROCESS"),TEXT("GLOBAL ILLUMINATION"),TEXT("REFLECTIONS"),TEXT("SHADING"),TEXT("AA QUALITY")};
const TCHAR* Tiers[]={TEXT("LOW"),TEXT("MEDIUM"),TEXT("HIGH"),TEXT("EPIC"),TEXT("CINEMATIC")};
const int Limits[]={0,30,60,90,120,144,165,240};
void Save(){if(!GConfig)return;const TCHAR* S=TEXT("LethalWorld.Graphics");GConfig->SetFloat(S,TEXT("Sharpness"),Sharpness,GGameUserSettingsIni);GConfig->SetInt(S,TEXT("AA"),AA,GGameUserSettingsIni);GConfig->SetInt(S,TEXT("Anisotropy"),Anisotropy,GGameUserSettingsIni);GConfig->SetBool(S,TEXT("Blur"),Blur,GGameUserSettingsIni);GConfig->SetBool(S,TEXT("Bloom"),Bloom,GGameUserSettingsIni);GConfig->SetBool(S,TEXT("AO"),AO,GGameUserSettingsIni);GConfig->Flush(false,GGameUserSettingsIni);}
}
void Load(){if(GConfig){const TCHAR* S=TEXT("LethalWorld.Graphics");GConfig->GetFloat(S,TEXT("Sharpness"),Sharpness,GGameUserSettingsIni);GConfig->GetInt(S,TEXT("AA"),AA,GGameUserSettingsIni);GConfig->GetInt(S,TEXT("Anisotropy"),Anisotropy,GGameUserSettingsIni);GConfig->GetBool(S,TEXT("Blur"),Blur,GGameUserSettingsIni);GConfig->GetBool(S,TEXT("Bloom"),Bloom,GGameUserSettingsIni);GConfig->GetBool(S,TEXT("AO"),AO,GGameUserSettingsIni);}AA=FMath::Clamp(AA,0,4);if(AA==3)AA=4;Sharpness=FMath::Clamp(Sharpness,0.f,2.f);Anisotropy=FMath::Clamp(Anisotropy,1,16);if(auto* S=UGameUserSettings::GetGameUserSettings()){S->ApplyNonResolutionSettings();Set(TEXT("t.MaxFPS"),S->GetFrameRateLimit());Set(TEXT("r.ScreenPercentage"),FMath::Clamp(S->ScalabilityQuality.ResolutionQuality,35.f,150.f));}Effects();LWLighting::Apply(LWLighting::Mode());Load58();}
void Change(int I){if(I==10||I==11||I==21||I>=23){Change58(I);return;}auto* S=UGameUserSettings::GetGameUserSettings();if(!S||I<0||I>=Count)return;Custom58(I);
 if(I<10)(S->*Setters[I])(((S->*Getters[I])()+1)%5);
 else if(I==10)S->SetOverallScalabilityLevel((S->GetOverallScalabilityLevel()+1)%5);
 else if(I==11){float V=S->ScalabilityQuality.ResolutionQuality;S->SetResolutionScaleValueEx(V>=149?35:FMath::Min(150.f,V+5));}
 else if(I==12){int Index=0;for(int J=0;J<UE_ARRAY_COUNT(Limits);J++)if(FMath::IsNearlyEqual(S->GetFrameRateLimit(),float(Limits[J])))Index=J;S->SetFrameRateLimit(Limits[(Index+1)%UE_ARRAY_COUNT(Limits)]);}
 else if(I==13)S->SetVSyncEnabled(!S->IsVSyncEnabled());
 else if(I==14){AA=(AA+1)%5;if(AA==3)AA=4;}
 else if(I==15)Blur=!Blur;else if(I==16)Bloom=!Bloom;else if(I==17)AO=!AO;
 else if(I==18)Anisotropy=Anisotropy>=16?1:Anisotropy*2;
 else if(I==19)LWLighting::Cycle();
 else if(I==20)Sharpness=Sharpness>=1.99f?0:Sharpness+.2f;
 else if(I==21){S->SetOverallScalabilityLevel(2);S->SetResolutionScaleValueEx(75);S->SetFrameRateLimit(90);S->SetVSyncEnabled(false);S->SetDynamicResolutionEnabled(false);AA=4;Blur=false;Bloom=AO=true;Anisotropy=8;LWLighting::Apply(1,true);}
 else if(I==22)S->SetLandscapeQuality((S->GetLandscapeQuality()+1)%5);
 S->ApplyNonResolutionSettings();Set(TEXT("t.MaxFPS"),S->GetFrameRateLimit());Set(TEXT("r.ScreenPercentage"),FMath::Clamp(S->ScalabilityQuality.ResolutionQuality,35.f,150.f));Effects();LWLighting::Apply(LWLighting::Mode());Apply58();S->SaveSettings();Save();
}
FString Label(int I){if(I==10||I==11||I>=23)return Label58(I);auto* S=UGameUserSettings::GetGameUserSettings();if(!S)return TEXT("UNAVAILABLE");if(I<10)return FString(Names[I])+TEXT(": ")+Tiers[FMath::Clamp((S->*Getters[I])(),0,4)];
 switch(I){case 10:{int Q=S->GetOverallScalabilityLevel();return FString(TEXT("PRESET: "))+(Q<0?TEXT("CUSTOM"):Tiers[FMath::Clamp(Q,0,3)]);}case 11:return FString::Printf(TEXT("RENDER SCALE: %.0f%%"),S->ScalabilityQuality.ResolutionQuality);case 12:return S->GetFrameRateLimit()==0?TEXT("FRAME LIMIT: UNLIMITED"):FString::Printf(TEXT("FRAME LIMIT: %.0f FPS"),S->GetFrameRateLimit());case 13:return S->IsVSyncEnabled()?TEXT("VSYNC: ON"):TEXT("VSYNC: OFF");case 14:{const TCHAR* N[]={TEXT("OFF"),TEXT("FXAA"),TEXT("TAA"),TEXT("MSAA (FORWARD ONLY)"),TEXT("TSR")};return FString(TEXT("ANTI ALIASING: "))+N[AA];}case 15:return Blur?TEXT("MOTION BLUR: ON"):TEXT("MOTION BLUR: OFF");case 16:return Bloom?TEXT("BLOOM: ON"):TEXT("BLOOM: OFF");case 17:return AO?TEXT("AMBIENT OCCLUSION: ON"):TEXT("AMBIENT OCCLUSION: OFF");case 18:return FString::Printf(TEXT("TEXTURE FILTERING: %dX"),Anisotropy);case 19:return LWLighting::Label();case 20:return FString::Printf(TEXT("SHARPNESS: %.1f"),Sharpness);case 21:return TEXT("RESTORE BALANCED DEFAULTS");case 22:return FString(TEXT("LANDSCAPE: "))+Tiers[FMath::Clamp(S->GetLandscapeQuality(),0,4)];}return TEXT("");}
}
