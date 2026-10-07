#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LWHUD.generated.h"
struct FLWUIHit55 { FName Id; FVector2D Center; int Priority=0; };
struct FLWUIAnim55 { float Value=0; uint64 Frame=0; };
UCLASS()
class LETHALWORLD_API ALWHUD : public AHUD
{
    GENERATED_BODY()
public:
 void SaveScreen62(class ALWCharacter* P);void SaveClick62(class ALWCharacter* P,FName N);
public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    void MainMenu81(class ALWCharacter* P);void MenuBackdrop81();
    bool MenuClick81(class ALWCharacter* P,FName Id);
    void MenuTile81(FName Id,const FString& Label,float X,float Y,float W,float H,int Icon,bool Enabled=true);
    UPROPERTY() TObjectPtr<class ULWMainMenu81> Menu81;
    UPROPERTY() TObjectPtr<class UTexture2D> Skyline81;
    UPROPERTY() TObjectPtr<class UTexture2D> Foreground81;
    UPROPERTY() TObjectPtr<class UTexture2D> Mist81;
    bool News81=false;int Release81=0;
    void Menu78(class ALWCharacter* P);
    void Settings78(class ALWCharacter* P);
    void NewSurvivor78(class ALWCharacter* P);
    int SettingsSection78=0;
    UPROPERTY() TObjectPtr<class USoundBase> UIClick55;
    FString Tooltip55;
    FVector2D TooltipAt55;
    void Finish55();
    void ControllerOverlay58();
    int SettingsDrag55=-1;
    void SettingsSlider55(class ALWCharacter* P,int Kind,float X,float Y,float W);
    UPROPERTY() TObjectPtr<class UFont> UIFont55;
    UPROPERTY() TObjectPtr<class UTexture2D> Backdrop55;
    UPROPERTY() TObjectPtr<class UTexture2D> Surface55;
    FVector2D Origin55=FVector2D::ZeroVector;
    TArray<FLWUIHit55> Controls55;
    TMap<FName,FLWUIAnim55> Motion55;
    FString Screen55;
    FName Focus55;
    float Delta55=.016f,Reveal55=1;
    double LastTime55=0;
    bool Keyboard55=false,ReducedMotion55=false,HighContrast55=false;
    int CrewPage55=0,DialoguePage55=0;
    FString DialogueSeen55;
    FVector2D LastPointer55=FVector2D::ZeroVector;
    FString Context55(class ALWCharacter* P) const;
    void Frame55(class ALWCharacter* P);
    bool UIKey55(FKey Key);
    bool Mouse55(float& X,float& Y) const;
    void AddHitBox(FVector2D Position,FVector2D Size,FName Name,bool Consume=false,int32 Priority=0);
    float Animate55(FName Id,float Target,float Rate=16);
    void Control55(FName Id,const FString& Label,float X,float Y,float W,float H=41,bool Selected=false,bool Register=true);
    void SurfacePanel55(float X,float Y,float W,float H,FLinearColor C);
    void SaveUI55();
    void WorkbenchScreen39(class ALWCharacter* P);void WorkbenchClick39(class ALWCharacter* P,FName N);
    bool StoryScreen(class ALWCharacter* P);
    void StoryJournal(class ALWCharacter* P);
    void CreatorScreen35(class ALWCharacter* P);
    bool CreatorMouse35=false;int CreatorDrag35=-1;
    void OpeningScreen(class ALWCharacter* P);void Logo(float X,float Y,float W,float H,bool Animate=false);
    UPROPERTY() TObjectPtr<class UTexture2D> TitleLogo;
    void CollectionScreen36(class ALWCharacter* P);void CollectionClick36(class ALWCharacter* P,FName Name);
    void PlayerTabs(class ALWCharacter* P);
    virtual void DrawHUD() override;
    virtual void NotifyHitBoxClick(FName Name) override;
    UPROPERTY() TObjectPtr<class UTexture2D> FontAtlas;
    UPROPERTY() TObjectPtr<class UTexture2D> CardAtlas;
    UPROPERTY() TObjectPtr<class UTexture2D> POIAtlas;
    int CardBet=20,CardSeenSerial=0,CardDrag=-1,CardBatchFirst=0;
    FGuid CardSeenRound;
    double CardMoveStart=0;
    bool CardMouseDown=false;
    int GraphicsPage=0;
    UPROPERTY() TObjectPtr<class UTexture2D> MenuDistant51;
    UPROPERTY() TObjectPtr<class UTexture2D> MenuForeground51;
    int KeysPage51=-1; void KeysScreen51(); bool KeysClick51(FName N);
    void MenuBackdrop46();void WeaponSlots46(class ALWCharacter* P);
    float Scale=1;
    UCanvas* GetCanvas45()const{return Canvas;}
    void Text(const FString& S,float X,float Y,float Size=1,FLinearColor Color=FLinearColor(.78f,.8f,.66f));
    void Rect(float X,float Y,float W,float H,FLinearColor C);
    void Line(float X,float Y,float X2,float Y2,FLinearColor C,float Width=1);
    void Button(FName Id,const FString& Label,float X,float Y,float Width=390);
    void Map(class ALWCharacter* P);
    void MiniMap(class ALWCharacter* P);
    void SecurityScreen(class ALWCharacter* P);void VehicleOverlay(class ALWCharacter* P);
    void CardScreen(class ALWCharacter* P);bool CardClick(class ALWCharacter* P,FName N);
    void RPGScreen(class ALWCharacter* P);void RPGOverlay(class ALWCharacter* P);bool RPGClick(class ALWCharacter* P,FName N);
    void QuickLoot54(class ALWCharacter* P);
    void InventoryScreen(class ALWCharacter* P);
    FGuid DragId,SelectedId;
    int32 DragPanel=0;
    bool bDragRotated=false,MouseWasDown=false;
    FVector2D DragOffset=FVector2D::ZeroVector;
    int32 InventoryScroll=0; int32 PlayerInventoryScroll=0;
    int32 SelectedPanel=0;
    uint64 InventoryLastFrame=0;
    FName InventoryContext;
    float MapZoom=.005f;
    FVector2D MapPan=FVector2D::ZeroVector;
};
