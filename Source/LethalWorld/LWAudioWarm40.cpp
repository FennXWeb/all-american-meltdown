#include "LWWorld.h"
#include "LWAudioCatalog.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/PackageName.h"
void ALWWorld::WarmAudio40(){TArray<FSoftObjectPath> Paths;auto Add=[&](FSoftObjectPath P){if(!P.IsNull()&&FPackageName::DoesPackageExist(P.GetLongPackageName()))Paths.AddUnique(P);};
 for(FName N:ULWAudioCatalog::GetDefaultSlotNames())Add(FSoftObjectPath(ULWAudioCatalog::GetLegacyObjectPath(N)));
 if(AudioCatalog)for(const auto& S:AudioCatalog->Slots){Add(S.Value.Source.ToSoftObjectPath());Add(S.Value.CustomAttenuation.ToSoftObjectPath());for(const auto& T:S.Value.Tracks)Add(T.ToSoftObjectPath());}
 Add(FSoftObjectPath(TEXT("/Game/Materials/M_Blood22.M_Blood22")));Add(FSoftObjectPath(TEXT("/Game/Materials/M_WeaponFX24.M_WeaponFX24")));Add(FSoftObjectPath(TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 AudioWarm40=UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths,FStreamableDelegate(),FStreamableManager::DefaultAsyncLoadPriority);
}
