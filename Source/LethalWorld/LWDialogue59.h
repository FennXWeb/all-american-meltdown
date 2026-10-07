#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/ActorComponent.h"
#include "LWDialogue75.h"
#include "LWDialogue59.generated.h"

USTRUCT(BlueprintType)
struct FLWVoiceProfile59 {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) FString DisplayName;
 UPROPERTY(EditAnywhere) FString ElevenLabsVoiceId;
 UPROPERTY(EditAnywhere) bool Female=false;
 UPROPERTY(EditAnywhere) FString Model=TEXT("eleven_multilingual_v2");
};
USTRUCT(BlueprintType)
struct FLWSpokenLine59 {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) FString Text;
 UPROPERTY(EditAnywhere) FName Profile;
 UPROPERTY(EditAnywhere) TSoftObjectPtr<class USoundBase> Audio;
 UPROPERTY(EditAnywhere) float Duration=0;
};
UCLASS(BlueprintType)
class LETHALWORLD_API ULWDialogueCatalog59 : public UDataAsset {
 GENERATED_BODY()
public:
 // Enabled only after the selected batch is validated. Unrecorded lines remain subtitled.
 UPROPERTY(EditAnywhere) bool Enabled=false;
 UPROPERTY(EditAnywhere) TMap<FName,FLWVoiceProfile59> Profiles;
 UPROPERTY(EditAnywhere) TMap<FString,FLWSpokenLine59> Lines;
 UPROPERTY(EditAnywhere) TMap<FName,FName> Cast;
 UPROPERTY(EditAnywhere) FName ProceduralMale75;
 UPROPERTY(EditAnywhere) FName ProceduralFemale75;
 UPROPERTY(EditAnywhere) TArray<FName> ProceduralProfiles75;
 UPROPERTY(EditAnywhere) TMap<FName,FName> ProceduralReplacements75;
 static ULWDialogueCatalog59* Get();
 static FString Normalize(const FString& Text);
 static FString Key(FName Profile,const FString& Text);
 static FName DefaultProfile(FName Identity,bool Female);
 FName ProceduralProfile75(FName Identity,bool Female) const;
 static FName StoryIdentity(const FString& Name);
};

// One cancellable voice channel per speaker; loading never blocks the game thread.
UCLASS()
class LETHALWORLD_API ULWDialogue59 : public UActorComponent {
 GENERATED_BODY()
public:
 ULWDialogue59();
 static ULWDialogue59* Channel(AActor* Actor);
 static bool Available();
 FName Assign(FName Identity,bool Female);
 void Say(FName Identity,bool Female,const FString& Text,bool ListenerRelative=false);
 bool Chatter75(FName Identity,bool Female,FName Group,bool Combat=false);
 void Stop();
 bool Busy() const;
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 virtual void TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function) override;
 UPROPERTY(VisibleAnywhere) FName Profile;
 UPROPERTY(Transient) TObjectPtr<class UAudioComponent> Audio;
 UPROPERTY(Transient) TObjectPtr<ULWDialogueCatalog59> Catalog;
private:
 TSharedPtr<struct FStreamableHandle> Pending;
 uint32 Serial=0;
 FLWChatterHistory75 ChatterHistory75;
 FRandomStream ChatterRandom75;
 bool ChatterSeeded75=false;
};
