#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWInventory.h"
#include "LWWorkbench39.generated.h"
USTRUCT()
struct FLWBenchDraft39 {
 GENERATED_BODY()
 UPROPERTY() TArray<FLWItemInstance> Bank;
 UPROPERTY() FLWItemInstance Gun;
 UPROPERTY() int32 Selected=-1;
};
UCLASS()
class LETHALWORLD_API ALWWorkbench39 : public AActor {
 GENERATED_BODY()
public:
 ALWWorkbench39();
 UPROPERTY() TObjectPtr<class ALWCharacter> Player;
 UPROPERTY() TObjectPtr<class ALWWorldObject> Station;
 UPROPERTY() TObjectPtr<class USceneCaptureComponent2D> Capture;
 UPROPERTY() TObjectPtr<class UTextureRenderTarget2D> Target;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Meshes;
 UPROPERTY() TObjectPtr<class ULWMystic62> Mystic62;
 UPROPERTY() FLWBenchDraft39 Draft;
 UPROPERTY() TArray<FLWBenchDraft39> UndoStack;
 UPROPERTY() TArray<FLWBenchDraft39> RedoStack;
 FName Mount62=TEXT("Optic"); int Tab62=0;
 bool SetCamo62(int C);bool Detach62();
 uint32 Original=0; FString Message; int Page=0,PartPage=0,Filter=0,Mode=0,Axis=3;
 float Yaw=120,Pitch=12,Distance=170; FVector Center=FVector(20,0,0); bool Snap=true;
 FVector2D LastMouse,DragOrigin; bool LeftDown=false,Dragging=false,EditingName=false,PendingDrop=false; FTransform DragStart;
 void Begin(class ALWCharacter* P,class ALWWorldObject* S);
 virtual void Tick(float Dt) override;
 void Snapshot();void Undo(bool Redo=false);void Refresh();void UpdateCamera();void FrameAll();
 bool Load(FGuid Id);bool Add(FGuid Id);bool Remove();bool Action();bool Dismantle();bool Apply();
 void Transform(FVector Delta);void SetBuildName39(const FString& Name);
 TArray<FGuid> Available() const;
 FVector2D Project(FVector Local) const;int Pick(FVector2D UV) const;
};

