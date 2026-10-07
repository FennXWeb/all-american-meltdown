#include "LWDebugMenu67.h"
#include "LWConsole47.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"

namespace {
const FLinearColor Ink67(.025f,.037f,.045f,1.f),Panel67(.045f,.065f,.075f,1.f),Gold67(.97f,.68f,.32f,1.f),Text67(.91f,.94f,.94f,1.f),Dim67(.57f,.67f,.70f,1.f);
TSharedRef<STextBlock> Label67(const FString& Text,int Size=16,FLinearColor Color=Text67){return SNew(STextBlock).Text(FText::FromString(Text)).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),Size)).ColorAndOpacity(Color);}
}

class SLWDebugPanel67 : public SCompoundWidget {
public:
 SLATE_BEGIN_ARGS(SLWDebugPanel67){} SLATE_ARGUMENT(ULWDebugMenu67*,Owner) SLATE_END_ARGS()
 TWeakObjectPtr<ULWDebugMenu67> Owner;
 TSharedPtr<SSearchBox> SearchBox;
 TSharedPtr<SListView<FLWDebugRow67>> List;
 TSharedPtr<SComboBox<TSharedPtr<FString>>> CategoryBox;
 TSharedPtr<SScrollBox> DetailsScroll;
 FSearchBoxStyle SearchStyle;
 bool Updating=false;
 virtual bool SupportsKeyboardFocus()const override{return true;}
 virtual FReply OnPreviewKeyDown(const FGeometry& G,const FKeyEvent& E)override{
  if(!Owner.IsValid())return FReply::Handled();
  if(E.GetKey()==EKeys::Escape||E.GetKey()==EKeys::Tilde||E.GetKey()==EKeys::Gamepad_FaceButton_Right){Owner->Close();return FReply::Handled();}
  if(E.IsControlDown()&&E.GetKey()==EKeys::F){FSlateApplication::Get().SetKeyboardFocus(SearchBox);return FReply::Handled();}
  if(E.IsControlDown()&&E.GetKey()==EKeys::Enter){if(!E.IsRepeat())Owner->Execute();return FReply::Handled();}
  return SCompoundWidget::OnPreviewKeyDown(G,E);
 }
 virtual FReply OnMouseButtonDown(const FGeometry&,const FPointerEvent&)override{return FReply::Handled();}
 virtual FReply OnMouseButtonUp(const FGeometry&,const FPointerEvent&)override{return FReply::Handled();}
 void Construct(const FArguments& Args){
  Owner=Args._Owner; SearchStyle=FCoreStyle::Get().GetWidgetStyle<FSearchBoxStyle>(TEXT("SearchBox")); SearchStyle.TextBoxStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16)); SearchStyle.SetActiveFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16));
  auto Tabs=SNew(SHorizontalBox);const TCHAR* Names[]={TEXT("Cheats"),TEXT("Items"),TEXT("NPCs"),TEXT("Vehicles"),TEXT("POIs")};
  for(int I=0;I<5;I++)Tabs->AddSlot().FillWidth(1).Padding(0,0,8,0)[SNew(SButton).ContentPadding(FMargin(14,11)).ButtonColorAndOpacity_Lambda([this,I]{return Owner.IsValid()&&int(Owner->Section)==I?Gold67:Panel67;}).OnClicked_Lambda([this,I]{if(Owner.IsValid())Owner->SetSection(ELWDebugSection67(I));return FReply::Handled();})[Label67(Names[I],18)]];
  auto Headers=SNew(SHorizontalBox);const TCHAR* Columns[]={TEXT("Name"),TEXT("ID"),TEXT("Category")};const int Keys[]={0,1,2};
  for(int I=0;I<3;I++)Headers->AddSlot().FillWidth(I==0?.46f:I==1?.28f:.26f).Padding(0,0,2,0)[SNew(SButton).ButtonColorAndOpacity(Panel67).ContentPadding(FMargin(10,7)).OnClicked_Lambda([this,K=Keys[I]]{if(Owner.IsValid())Owner->SetSort(K);return FReply::Handled();})[SNew(STextBlock).Text_Lambda([this,I,Name=FString(Columns[I])]{return FText::FromString(Name+(Owner.IsValid()&&Owner->Sort==I?(Owner->Descending?TEXT("  v"):TEXT("  ^")):TEXT("")));}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"),14)).ColorAndOpacity(Gold67)]];
  ChildSlot[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"))).BorderBackgroundColor(Ink67).Padding(24)
   [SNew(SVerticalBox)
    +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,18)[SNew(SHorizontalBox)
     +SHorizontalBox::Slot().FillWidth(1)[SNew(SVerticalBox)
      +SVerticalBox::Slot().AutoHeight()[Label67(TEXT("DEBUG MENU"),28,Gold67)]
      +SVerticalBox::Slot().AutoHeight().Padding(0,4)[Label67(TEXT("ALL AMERICAN MELTDOWN  /  WORLD PAUSED"),13,Dim67)]]
     +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SButton).ContentPadding(FMargin(20,12)).OnClicked_Lambda([this]{if(Owner.IsValid())Owner->Close();return FReply::Handled();})[Label67(TEXT("Close  [Esc]"))]]]
    +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,16)[Tabs]
    +SVerticalBox::Slot().AutoHeight().Padding(0,0,0,12)[SNew(SHorizontalBox)
     +SHorizontalBox::Slot().FillWidth(1).Padding(0,0,14,0)[SAssignNew(SearchBox,SSearchBox).Style(&SearchStyle).DelayChangeNotificationsWhileTyping(false).HintText(FText::FromString(TEXT("Search names, IDs or details...  [Ctrl+F]"))).OnTextChanged_Lambda([this](const FText& T){if(!Updating&&Owner.IsValid())Owner->SetSearch(T.ToString());}).OnTextCommitted_Lambda([this](const FText&,ETextCommit::Type How){if(How==ETextCommit::OnEnter&&List)FSlateApplication::Get().SetKeyboardFocus(List);})]
     +SHorizontalBox::Slot().AutoWidth()[SNew(SBox).WidthOverride(235)[SAssignNew(CategoryBox,SComboBox<TSharedPtr<FString>>).OptionsSource(&Owner->Categories).OnGenerateWidget_Lambda([](TSharedPtr<FString> S){return Label67(*S);}).OnSelectionChanged_Lambda([this](TSharedPtr<FString> S,ESelectInfo::Type){if(!Updating&&Owner.IsValid()&&S)Owner->SetCategory(*S==TEXT("All categories")?FString():*S);})[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(!Owner.IsValid()||Owner->Category.IsEmpty()?TEXT("All categories"):Owner->Category);}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),15)).ColorAndOpacity(Text67)]]]
     +SHorizontalBox::Slot().AutoWidth().Padding(12,0,0,0)[SNew(SButton).ContentPadding(FMargin(14,7)).OnClicked_Lambda([this]{if(Owner.IsValid()){Owner->Search.Empty();Owner->Category.Empty();Owner->Refresh();}return FReply::Handled();})[Label67(TEXT("Reset filters"),14)]]]
    +SVerticalBox::Slot().FillHeight(1)[SNew(SHorizontalBox)
     +SHorizontalBox::Slot().FillWidth(.66f).Padding(0,0,20,0)[SNew(SVerticalBox)
      +SVerticalBox::Slot().AutoHeight()[Headers]
      +SVerticalBox::Slot().FillHeight(1)[SNew(SOverlay)
       +SOverlay::Slot()[SAssignNew(List,SListView<FLWDebugRow67>).ListItemsSource(&Owner->Visible).SelectionMode(ESelectionMode::Single).ItemHeight(43).OnGenerateRow(this,&SLWDebugPanel67::Row).OnSelectionChanged_Lambda([this](FLWDebugRow67 E,ESelectInfo::Type){if(!Updating&&Owner.IsValid()&&E)Owner->Select(E->Id);}).OnMouseButtonDoubleClick_Lambda([this](FLWDebugRow67 E){if(Owner.IsValid()&&E){Owner->Select(E->Id);Owner->Execute();}})]
       +SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(STextBlock).Text(FText::FromString(TEXT("No matches. Clear the search or change the category."))).ColorAndOpacity(Dim67).Visibility_Lambda([this]{return Owner.IsValid()&&Owner->Visible.IsEmpty()?EVisibility::HitTestInvisible:EVisibility::Collapsed;})]]
      +SVerticalBox::Slot().AutoHeight().Padding(0,10)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()?FString::Printf(TEXT("%d / %d entries  |  Click a column heading to sort"),Owner->Visible.Num(),Owner->Entries.Num()):FString());}).ColorAndOpacity(Dim67)]]
     +SHorizontalBox::Slot().FillWidth(.34f)[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"))).BorderBackgroundColor(Panel67).Padding(20)
      [SNew(SVerticalBox)
       +SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()&&Owner->Selected?Owner->Selected->Name:TEXT("Select an entry"));}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"),23)).ColorAndOpacity(Gold67).AutoWrapText(true)]
       +SVerticalBox::Slot().AutoHeight().Padding(0,8,0,16)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()&&Owner->Selected?Owner->Selected->Category+TEXT("  /  ")+Owner->Selected->Id:FString());}).ColorAndOpacity(Dim67).AutoWrapText(true)]
       +SVerticalBox::Slot().FillHeight(1).Padding(0,0,0,8)[SAssignNew(DetailsScroll,SScrollBox)+SScrollBox::Slot()[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()&&Owner->Selected?Owner->Selected->Details:TEXT("Use the list or type a search to find what you need."));}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16)).ColorAndOpacity(Text67).AutoWrapText(true)]]
       +SVerticalBox::Slot().AutoHeight().Padding(0,10)[SNew(SHorizontalBox).Visibility_Lambda([this]{return Owner.IsValid()&&Owner->Selected&&Owner->Selected->HasAmount?EVisibility::Visible:EVisibility::Collapsed;})
        +SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()&&Owner->Section==ELWDebugSection67::POIs?TEXT("Search radius (km)"):Owner.IsValid()&&Owner->Selected&&Owner->Selected->Id==TEXT("time")?TEXT("Hour (0-23)"):TEXT("Amount"));}).ColorAndOpacity(Text67)]
        +SHorizontalBox::Slot().AutoWidth()[SNew(SBox).WidthOverride(145)[SNew(SSpinBox<int32>).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16)).Value_Lambda([this]{return Owner.IsValid()?Owner->Amount:1;}).MinValue_Lambda([this]{return Owner.IsValid()?Owner->Minimum():1;}).MaxValue_Lambda([this]{return Owner.IsValid()?Owner->Maximum():1;}).MinSliderValue_Lambda([this]{return Owner.IsValid()?Owner->Minimum():1;}).MaxSliderValue_Lambda([this]{return Owner.IsValid()?Owner->Maximum():1;}).Delta(1).OnValueChanged_Lambda([this](int32 V){if(Owner.IsValid())Owner->Amount=FMath::Clamp(V,Owner->Minimum(),Owner->Maximum());})]]]
       +SVerticalBox::Slot().AutoHeight().Padding(0,6)[SNew(SButton).Tag(TEXT("Debug67Action")).ButtonColorAndOpacity(Gold67).ContentPadding(FMargin(18,14)).IsEnabled_Lambda([this]{return Owner.IsValid()&&Owner->Selected.IsValid();}).OnClicked_Lambda([this]{if(Owner.IsValid())Owner->Execute();return FReply::Handled();})[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()?Owner->ActionLabel():TEXT("Execute"));}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"),18)).ColorAndOpacity(Text67)]]
       +SVerticalBox::Slot().AutoHeight()[SNew(SButton).Visibility_Lambda([this]{return Owner.IsValid()&&Owner->LocateRequest?EVisibility::Visible:EVisibility::Collapsed;}).OnClicked_Lambda([this]{if(Owner.IsValid())Owner->CancelSearch();return FReply::Handled();})[Label67(TEXT("Cancel location search"),14)]]
       +SVerticalBox::Slot().AutoHeight().Padding(0,10,0,0)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()&&Owner->Player.IsValid()?FString::Printf(TEXT("Credits: %lld    God mode: %s"),Owner->Player->Money,Owner->Player->bGod47?TEXT("ON"):TEXT("OFF")):FString());}).ColorAndOpacity(Dim67).AutoWrapText(true)]
      ]]]
    +SVerticalBox::Slot().AutoHeight().Padding(0,14,0,10)[SNew(SBox).HeightOverride(98)[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"))).BorderBackgroundColor(Panel67).Padding(14)[SNew(SVerticalBox)
     +SVerticalBox::Slot().AutoHeight()[Label67(TEXT("RESULT"),12,Gold67)]
     +SVerticalBox::Slot().FillHeight(1).Padding(0,6)[SNew(STextBlock).Text_Lambda([this]{return FText::FromString(Owner.IsValid()?Owner->Status:FString());}).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16)).ColorAndOpacity(Text67).AutoWrapText(true)]]]]
    +SVerticalBox::Slot().AutoHeight()[Label67(TEXT("Double-click or Ctrl+Enter: execute   /   Esc: close   /   Inventory and progression cheats use normal saves."),13,Dim67)]
   ]];
  RefreshView();
 }
 TSharedRef<ITableRow> Row(FLWDebugRow67 E,const TSharedRef<STableViewBase>& Table){
  return SNew(STableRow<FLWDebugRow67>,Table).Padding(FMargin(10,9))[SNew(SHorizontalBox)
   +SHorizontalBox::Slot().FillWidth(.46f)[SNew(STextBlock).Text(FText::FromString(E->Name)).Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"),16)).ColorAndOpacity(Text67).OverflowPolicy(ETextOverflowPolicy::Ellipsis).ToolTipText(FText::FromString(E->Name))]
   +SHorizontalBox::Slot().FillWidth(.28f)[SNew(STextBlock).Text(FText::FromString(E->Id)).ColorAndOpacity(Dim67).OverflowPolicy(ETextOverflowPolicy::Ellipsis).ToolTipText(FText::FromString(E->Id))]
   +SHorizontalBox::Slot().FillWidth(.26f)[SNew(STextBlock).Text(FText::FromString(E->Category)).ColorAndOpacity(Dim67).OverflowPolicy(ETextOverflowPolicy::Ellipsis)]];
 }
 void RefreshView(){if(!Owner.IsValid()||!List)return;Updating=true;List->RequestListRefresh();List->SetSelection(Owner->Selected);if(Owner->Selected)List->RequestScrollIntoView(Owner->Selected);if(SearchBox->GetText().ToString()!=Owner->Search)SearchBox->SetText(FText::FromString(Owner->Search));DetailsScroll->ScrollToStart();CategoryBox->RefreshOptions();for(const auto& C:Owner->Categories)if(*C==(Owner->Category.IsEmpty()?TEXT("All categories"):Owner->Category)){CategoryBox->SetSelectedItem(C);break;}Updating=false;}
};

UWorld* ULWDebugMenu67::GetWorld()const{return Player.IsValid()?Player->GetWorld():nullptr;}
bool ULWDebugMenu67::IsOpen()const{return RootWidget.IsValid()&&Player.IsValid()&&Player->bDebug67;}
FString ULWDebugMenu67::OpenFor(ALWCharacter* P){
 if(!P||!P->bStarted||!P->World||P->OpeningMode||P->bWorldSetup||P->Health<=0)return TEXT("Start or load a living character before opening the debug menu.");
 if(!P->GetWorld()->GetGameViewport()||!FSlateApplication::IsInitialized())return TEXT("The debug menu requires a game viewport.");
 auto* PC=Cast<APlayerController>(P->Controller);if(!PC)return TEXT("No local player controller.");
 auto* C=Cast<ULWConsole47>(P->GetWorld()->GetGameViewport()->ViewportConsole);if(!C)return TEXT("Game console is unavailable.");
 if(!P->DebugMenu67)P->DebugMenu67=NewObject<ULWDebugMenu67>(P);auto* M=P->DebugMenu67.Get();
 if(M->IsOpen())return TEXT("Debug menu is already open.");
 // Restore the console's previous pause state before taking ownership ourselves.
 if(C->ConsoleActive())C->FakeGotoState(NAME_None);
 M->Player=P;M->Console=C;M->WasPaused=UGameplayStatics::IsGamePaused(P);M->WasUIInput=P->bUIInputActive;
 P->bDebug67=true;P->ConsumeUIAttack();P->CancelReload();P->StopAttack();P->bAim=P->bSprint=false;P->LeanLeft40=P->LeanRight40=false;P->GetCharacterMovement()->StopMovementImmediately();
 if(PC->PlayerInput)PC->PlayerInput->FlushPressedKeys();if(P->Vehicle){P->Vehicle->Throttle=P->Vehicle->Steer=0;P->Vehicle->CabinMove=FVector2D::ZeroVector;}
 P->SetMenuInput(true);UGameplayStatics::SetGamePaused(P,true);M->Refresh();
 M->LocateHandle=C->LocateResult67.AddWeakLambda(M,[M](int32 Serial,const FString& Result){if(M->LocateRequest==Serial){M->Status=Result;M->LocateRequest=0;}});
 M->RootWidget=SNew(SOverlay)
  +SOverlay::Slot()[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"))).BorderBackgroundColor(FLinearColor(.008f,.013f,.018f,.97f)).OnMouseButtonDown_Lambda([](const FGeometry&,const FPointerEvent&){return FReply::Handled();})]
  +SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(SDPIScaler).DPIScale_Lambda([Weak=TWeakObjectPtr<ALWCharacter>(P)]{FVector2D Size(1440,900);if(Weak.IsValid()&&Weak->GetWorld()->GetGameViewport())Weak->GetWorld()->GetGameViewport()->GetViewportSize(Size);return FMath::Clamp(float(FMath::Min(Size.X/1440.,Size.Y/900.)),.25f,2.f);})[SNew(SBox).WidthOverride(1360).HeightOverride(830)[SAssignNew(M->Panel,SLWDebugPanel67).Owner(M)]]];
 P->GetWorld()->GetGameViewport()->AddViewportWidgetContent(M->RootWidget.ToSharedRef(),10000);
 // The viewport applies this focus reply on its next tick. Target the search
 // itself, or that deferred reply would steal focus back from the text field.
 FInputModeUIOnly Input;Input.SetWidgetToFocus(M->Panel->SearchBox);Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);PC->SetInputMode(Input);PC->bShowMouseCursor=true;
 FSlateApplication::Get().SetKeyboardFocus(M->Panel->SearchBox);return TEXT("Debug menu opened. Escape closes it.");
}
void ULWDebugMenu67::Close(bool RestoreInput){
 if(Closing||!RootWidget)return;Closing=true;CancelSearch();if(Console.IsValid())Console->LocateResult67.Remove(LocateHandle);LocateHandle.Reset();
 if(auto* W=GetWorld())if(auto* V=W->GetGameViewport())V->RemoveViewportWidgetContent(RootWidget.ToSharedRef());RootWidget.Reset();Panel.Reset();
 if(auto* P=Player.Get()){P->bDebug67=false;if(RestoreInput){P->ConsumeUIAttack();if(auto* PC=Cast<APlayerController>(P->Controller))if(PC->PlayerInput)PC->PlayerInput->FlushPressedKeys();UGameplayStatics::SetGamePaused(P,WasPaused);P->SetMenuInput(WasUIInput);if(FSlateApplication::IsInitialized())FSlateApplication::Get().SetAllUserFocusToGameViewport();}}
 Closing=false;
}
void ULWDebugMenu67::Refresh(){
 Entries=LWDebug67::Catalog(Section);TSet<FString> Groups;for(const auto& E:Entries)Groups.Add(E->Category);TArray<FString> Sorted=Groups.Array();Sorted.Sort();Categories.Empty();Categories.Add(MakeShared<FString>(TEXT("All categories")));for(const auto& C:Sorted)Categories.Add(MakeShared<FString>(C));if(!Category.IsEmpty()&&!Groups.Contains(Category))Category.Empty();UpdateRows();
}
void ULWDebugMenu67::UpdateRows(){
 Visible=LWDebug67::Filter(Entries,Search,Category,Sort,Descending);auto Before=Selected;if(!Visible.Contains(Selected))Selected=Visible.IsEmpty()?nullptr:Visible[0];if(Before!=Selected)Amount=Selected?Selected->Default:1;if(Panel)Panel->RefreshView();
}
void ULWDebugMenu67::SetSection(ELWDebugSection67 Value){if(Section==Value)return;Section=Value;Search.Empty();Category.Empty();Selected.Reset();Refresh();}
void ULWDebugMenu67::SetSearch(const FString& Value){Search=Value.Left(128);UpdateRows();}
void ULWDebugMenu67::SetCategory(const FString& Value){Category=Value;UpdateRows();}
void ULWDebugMenu67::SetSort(int Column){Column=FMath::Clamp(Column,0,2);if(Sort==Column)Descending=!Descending;else{Sort=Column;Descending=false;}UpdateRows();}
bool ULWDebugMenu67::Select(const FString& Id){for(const auto& E:Visible)if(E->Id==Id){if(Selected!=E){Selected=E;Amount=E->Default;}if(Panel)Panel->RefreshView();return true;}return false;}
int32 ULWDebugMenu67::Minimum()const{return Selected?Selected->Min:1;}
int32 ULWDebugMenu67::Maximum()const{return Selected?Selected->Max:1;}
FString ULWDebugMenu67::ActionLabel()const{if(!Selected)return TEXT("Select an entry");switch(Section){case ELWDebugSection67::Items:return TEXT("Add to inventory");case ELWDebugSection67::NPCs:return TEXT("Spawn NPCs");case ELWDebugSection67::Vehicles:return TEXT("Spawn vehicle");case ELWDebugSection67::POIs:return TEXT("Locate / set waypoint");default:return Selected->Name;}}
void ULWDebugMenu67::Execute(){
 if(!IsOpen()||!Selected||!Console.IsValid())return;Player->ConsumeUIAttack();Amount=FMath::Clamp(Amount,Minimum(),Maximum());const FString Cmd=LWDebug67::Command(*Selected,Amount);
 if(Section==ELWDebugSection67::POIs)CancelSearch();const int Serial=Console->LocateSerial60;
 Status=Console->Execute47(Cmd);Console->OutputText(TEXT("> ")+Cmd);Console->OutputText(Status);
 if(Section==ELWDebugSection67::POIs&&Console->LocateSerial60!=Serial){LocateRequest=Console->LocateSerial60;Status=TEXT("Searching generated locations. You can browse other tabs while this runs. The result and waypoint will appear here.");}
}
void ULWDebugMenu67::CancelSearch(){if(!LocateRequest)return;if(Console.IsValid()&&Console->LocateSerial60==LocateRequest)++Console->LocateSerial60;LocateRequest=0;Status=TEXT("Location search cancelled.");}
