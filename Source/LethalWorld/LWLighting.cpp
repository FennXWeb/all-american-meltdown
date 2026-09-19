#include "LWLighting.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "RHI.h"
namespace LWLighting {
static int Quality=1;
bool HardwareAvailable(){return GRHISupportsRayTracing;}
int Mode(){return Quality;}
void Apply(int Requested,bool Save){
 Quality=FMath::Clamp(Requested,0,2);if(Quality==2&&!HardwareAvailable())Quality=1;
 auto Set=[](const TCHAR* Name,int Value){if(auto* C=IConsoleManager::Get().FindConsoleVariable(Name))C->Set(Value,ECVF_SetByCode);};
 Set(TEXT("r.DynamicGlobalIlluminationMethod"),Quality?1:0);Set(TEXT("r.ReflectionMethod"),Quality?1:2);
 Set(TEXT("r.Lumen.HardwareRayTracing"),Quality==2);Set(TEXT("r.RayTracing.Enable"),Quality==2);
 Set(TEXT("r.Lumen.TranslucencyReflections.FrontLayer.Enable"),Quality>0);Set(TEXT("r.Lumen.TranslucencyReflections.FrontLayer.EnableForProject"),Quality>0);
 Set(TEXT("r.Lumen.HardwareRayTracing.LightingMode"),Quality==2?3:0);Set(TEXT("r.SSR.Quality"),Quality?3:2);
 if(Save&&GConfig){GConfig->SetInt(TEXT("LethalWorld.Settings"),TEXT("LightingQuality"),Quality,GGameUserSettingsIni);GConfig->Flush(false,GGameUserSettingsIni);}
}
void Load(){int Q=1;if(GConfig)GConfig->GetInt(TEXT("LethalWorld.Settings"),TEXT("LightingQuality"),Q,GGameUserSettingsIni);Apply(Q);}
void Cycle(){Apply((Quality+1)%(HardwareAvailable()?3:2),true);}
FString Label(){return Quality==2?TEXT("LIGHTING: HARDWARE RAY TRACING"):Quality==1?TEXT("LIGHTING: LUMEN"):TEXT("LIGHTING: PERFORMANCE");}
}
