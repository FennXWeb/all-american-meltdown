#include "LWLoading79.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Misc/ConfigCacheIni.h"
#include "Serialization/Csv/CsvParser.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SLeafWidget.h"
#include <atomic>

namespace LWLoading79 {
TArray<FMessage> ParseMessages(const FString& Csv){
 TArray<FMessage> Result; TSet<FString> Ids;
 const FCsvParser Parser(Csv);
 const auto& Rows=Parser.GetRows();
 for(int32 I=1;I<Rows.Num();++I){
  const auto& Row=Rows[I]; if(Row.Num()!=3)continue;
  FMessage M{FString(Row[0]).TrimStartAndEnd(),FString(Row[1]).TrimStartAndEnd(),FString(Row[2]).TrimStartAndEnd()};
  if(M.Id.IsEmpty()||M.Category.IsEmpty()||M.Text.IsEmpty()||Ids.Contains(M.Id)||M.Text.Len()>400)continue;
  Ids.Add(M.Id);Result.Add(MoveTemp(M));
 }
 return Result;
}
FBank ReadBank(const FString& Directory){
 FBank Bank; FString Csv;
 if(FFileHelper::LoadFileToString(Csv,*(Directory/TEXT("Messages.csv"))))Bank.Messages=ParseMessages(Csv);
 if(Bank.Messages.IsEmpty())Bank.Messages.Add({TEXT("fallback"),TEXT("SURVIVAL"),TEXT("Store supplies in your bunker before heading into dangerous territory.")});
 if(FFileHelper::LoadFileToString(Csv,*(Directory/TEXT("Images.csv")))){
  const FCsvParser Parser(Csv);const auto& Rows=Parser.GetRows();TSet<FString> Ids,Files;
  for(int32 I=1;I<Rows.Num();++I){
   const auto& Row=Rows[I];if(Row.Num()!=2)continue;
   const FString Id=FString(Row[0]).TrimStartAndEnd(),File=FString(Row[1]).TrimStartAndEnd();
   // Artwork stays inside the staged directory. Invalid/missing entries never block a load.
   if(Id.IsEmpty()||Ids.Contains(Id)||Files.Contains(File)||File!=FPaths::GetCleanFilename(File)||!File.EndsWith(TEXT(".png"))||File.Contains(TEXT(".."))||!IFileManager::Get().FileExists(*(Directory/File)))continue;
   Ids.Add(Id);Files.Add(File);Bank.Images.Add({Id,Directory/File});
  }
 }
 return Bank;
}
TArray<int32> ShuffledOrder(int32 Count,int32 Seed){
 TArray<int32> Order;for(int32 I=0;I<Count;++I)Order.Add(I);
 FRandomStream Random(Seed);for(int32 I=Order.Num()-1;I>0;--I)Order.Swap(I,Random.RandRange(0,I));
 return Order;
}
FFrame FrameAt(double Seconds,double Interval,double Fade,bool ReducedMotion){
 FFrame R;if(Interval<=0||!FMath::IsFinite(Seconds))return R;
 Seconds=FMath::Max(0.,Seconds);R.Current=int64(Seconds/Interval);R.Previous=FMath::Max(int64(0),R.Current-1);
 R.Blend=(ReducedMotion||R.Current==0||Fade<=0)?1.f:float(FMath::Clamp(FMath::Fmod(Seconds,Interval)/FMath::Min(Fade,Interval),0.,1.));
 return R;
}
FVector2D CoverSize(FVector2D Image,FVector2D Viewport){
 if(Image.X<=0||Image.Y<=0)return Viewport;
 return Image*FMath::Max(Viewport.X/Image.X,Viewport.Y/Image.Y);
}
namespace {
struct FLibrary {
 FBank Bank;
 TArray<TSharedPtr<FSlateDynamicImageBrush>> Brushes;
 TArray<int32> ArtOrder,MessageOrder;
};
TSharedPtr<FLibrary,ESPMode::ThreadSafe> Library;
std::atomic<int64> LastArt{-1},LastMessage{-1},Paints{0};

class SLoading79 final:public SLeafWidget {
public:
 SLATE_BEGIN_ARGS(SLoading79):_Preview(false){} SLATE_ARGUMENT(bool,Preview) SLATE_END_ARGS()
 void Construct(const FArguments& Args,TSharedRef<FLibrary,ESPMode::ThreadSafe> InLibrary,const FString& InLabel,double Age,bool InReduced,bool InContrast){
  Data=InLibrary;Label=InLabel;Start=FPlatformTime::Seconds()-Age;Reduced=InReduced;Contrast=InContrast;IsPreview=Args._Preview;
  FirstArt=LastArt.load()+1;FirstMessage=LastMessage.load()+1;
  SetCanTick(false);SetClipping(EWidgetClipping::ClipToBounds);SetVisibility(EVisibility::HitTestInvisible);
 }
 virtual FVector2D ComputeDesiredSize(float)const override{return FVector2D(1600,900);}
 virtual bool ComputeVolatility()const override{return true;}
 virtual int32 OnPaint(const FPaintArgs&,const FGeometry& G,const FSlateRect&,FSlateWindowElementList& Out,int32 Layer,const FWidgetStyle&,bool)const override{
  const double Age=FMath::Max(0.,FPlatformTime::Seconds()-Start);
  const auto Art=FrameAt(Age,18.,.9f,Reduced),Tip=FrameAt(Age,14.,.35f,Reduced);
  if(!IsPreview){LastArt.store(FirstArt+Art.Current);LastMessage.store(FirstMessage+Tip.Current);++Paints;}
  const FVector2D Size=G.GetLocalSize();const float S=FMath::Min(Size.X/1600.,Size.Y/900.);const float Margin=64*S;
  auto Geo=[&](FVector2D At,FVector2D Extent){return G.ToPaintGeometry(Extent,FSlateLayoutTransform(At));};
  auto Box=[&](FVector2D At,FVector2D Extent,FLinearColor Color){FSlateDrawElement::MakeBox(Out,Layer++,Geo(At,Extent),FCoreStyle::Get().GetBrush("WhiteBrush"),ESlateDrawEffect::None,Color);};
  auto Gradient=[&](float Y,float H,float Top,float Bottom){
   TArray<FSlateGradientStop> Stops;Stops.Emplace(FVector2D(0,0),FLinearColor(.006f,.011f,.016f,Top));
   if(Bottom>Top)Stops.Emplace(FVector2D(0,H*.43f),FLinearColor(.006f,.011f,.016f,Bottom*.86f));
   Stops.Emplace(FVector2D(0,H),FLinearColor(.006f,.011f,.016f,Bottom));
   // Slate describes the orientation of the color bands, not the direction of interpolation.
   FSlateDrawElement::MakeGradient(Out,Layer++,Geo(FVector2D(0,Y),FVector2D(Size.X,H)),Stops,Orient_Horizontal);
  };
  Box(FVector2D::ZeroVector,Size,FLinearColor(.009f,.017f,.022f,1));
  auto DrawArt=[&](int64 Serial,float Alpha){
   if(Data->ArtOrder.IsEmpty()||Alpha<=0)return;
   const auto& Brush=Data->Brushes[Data->ArtOrder[Serial%Data->ArtOrder.Num()]];
   FVector2D Extent=CoverSize(Brush->ImageSize,Size);
   // A small pan stays inside cover-fit bounds; reduced-motion mode is completely still.
   const float Zoom=Reduced?1.f:1.025f;Extent*=Zoom;
   FVector2D At=(Size-Extent)*.5f;
   if(!Reduced)At+=FVector2D(FMath::Sin(Age*.035f)*Size.X*.005f,FMath::Cos(Age*.027f)*Size.Y*.004f);
   FSlateDrawElement::MakeBox(Out,Layer++,Geo(At,Extent),Brush.Get(),ESlateDrawEffect::None,FLinearColor(1,1,1,Alpha));
  };
  if(Art.Blend<1)DrawArt(FirstArt+Art.Previous,1);
  DrawArt(FirstArt+Art.Current,Art.Blend);
  Gradient(0,240*S,.78f,0);
  Gradient(Size.Y-365*S,365*S,0,Contrast?1.f:.98f);
  if(Contrast)Box(FVector2D(0,Size.Y-247*S),FVector2D(Size.X,247*S),FLinearColor(.005f,.009f,.014f,.93f));
  auto Text=[&](const FString& Value,FVector2D At,int FontSize,bool Bold,FLinearColor Color){
   const auto Font=FCoreStyle::GetDefaultFontStyle(Bold?"Bold":"Regular",FMath::Max(8,FMath::RoundToInt(FontSize*S)));
   FSlateDrawElement::MakeText(Out,Layer++,Geo(At,Size),Value,Font,ESlateDrawEffect::None,Color);
  };
  Text(TEXT("ALL AMERICAN MELTDOWN"),FVector2D(Margin,43*S),22,true,FLinearColor(.94f,.94f,.9f));
  const int32 Index=Data->MessageOrder[(FirstMessage+Tip.Current)%Data->MessageOrder.Num()];
  const auto& Message=Data->Bank.Messages[Index];
  Text(Message.Category,FVector2D(Margin,Size.Y-227*S),14,true,FLinearColor(.96f,.69f,.39f));
  const auto Font=FCoreStyle::GetDefaultFontStyle("Regular",FMath::Max(8,FMath::RoundToInt(23*S)));
  const float WrapWidth=FMath::Min(Size.X-2*Margin,1080*S);
  if(CachedMessage!=Index||!FMath::IsNearlyEqual(CachedWidth,WrapWidth)||CachedFont!=Font.Size){
   CachedMessage=Index;CachedWidth=WrapWidth;CachedFont=Font.Size;Lines.Reset();
   const auto Measure=FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
   TArray<FString> Words;Message.Text.ParseIntoArrayWS(Words);FString Line;
   for(const auto& Word:Words){const FString Candidate=Line.IsEmpty()?Word:Line+TEXT(" ")+Word;
    if(!Line.IsEmpty()&&Measure->Measure(Candidate,Font).X>WrapWidth){Lines.Add(Line);Line=Word;}else Line=Candidate;}
   if(!Line.IsEmpty())Lines.Add(Line);
  }
  for(int32 I=0;I<Lines.Num();++I)Text(Lines[I],FVector2D(Margin,Size.Y-194*S+I*32*S),23,false,FLinearColor(.96f,.97f,.94f,Tip.Blend));
  Box(FVector2D(Margin,Size.Y-72*S),FVector2D(Size.X-2*Margin,1*S),FLinearColor(.65f,.75f,.75f,.24f));
  Text(Label,FVector2D(Margin,Size.Y-49*S),15,false,FLinearColor(.74f,.81f,.81f));
  for(int32 I=0;I<3;++I){const float A=Reduced?.8f:.25f+.65f*FMath::Pow(.5f+.5f*FMath::Sin(Age*3.5-I*.7f),2);
   Box(FVector2D(Size.X-Margin-(3-I)*13*S,Size.Y-41*S),FVector2D(5*S,5*S),FLinearColor(.96f,.69f,.39f,A));}
  return Layer;
 }
private:
 TSharedPtr<FLibrary,ESPMode::ThreadSafe> Data;
 FString Label;double Start=0;int64 FirstArt=0,FirstMessage=0;bool Reduced=false,Contrast=false,IsPreview=false;
 mutable int32 CachedMessage=-1,CachedFont=0;mutable float CachedWidth=-1;mutable TArray<FString> Lines;
};
}
void Preload(){
 check(IsInGameThread());
 if(Library||!FSlateApplication::IsInitialized())return;
 Library=MakeShared<FLibrary,ESPMode::ThreadSafe>();
 Library->Bank=ReadBank(FPaths::ProjectContentDir()/TEXT("Loading79"));
 for(const auto& Image:Library->Bank.Images){
  const FName Name(*Image.File);const FIntPoint Size=FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(Name);
  if(Size.X>0&&Size.Y>0)Library->Brushes.Add(MakeShared<FSlateDynamicImageBrush>(Name,FVector2D(Size)));
  else UE_LOG(LogTemp,Warning,TEXT("Loading screen artwork unavailable: %s"),*Image.File);
 }
 const int32 Seed=int32(FPlatformTime::Cycles());
 Library->ArtOrder=ShuffledOrder(Library->Brushes.Num(),Seed);
 Library->MessageOrder=ShuffledOrder(Library->Bank.Messages.Num(),Seed^0x79AB4);
 UE_LOG(LogTemp,Display,TEXT("LW_LOADING79_READY images=%d messages=%d"),Library->Brushes.Num(),Library->Bank.Messages.Num());
}
TSharedRef<SWidget> Create(const FString& Label){
 Preload();bool Reduced=false,Contrast=false;
 GConfig->GetBool(TEXT("LWInterface55"),TEXT("ReducedMotion"),Reduced,GGameUserSettingsIni);
 GConfig->GetBool(TEXT("LWInterface55"),TEXT("HighContrast"),Contrast,GGameUserSettingsIni);
 return SNew(SLoading79,Library.ToSharedRef(),Label,0.,Reduced,Contrast);
}
FTelemetry Telemetry(){return {Paints.load(),LastArt.load(),LastMessage.load()};}
#if WITH_DEV_AUTOMATION_TESTS
TSharedRef<SWidget> Preview(const FString& Label,double Age,bool ReducedMotion,bool HighContrast){
 Preload();return SNew(SLoading79,Library.ToSharedRef(),Label,Age,ReducedMotion,HighContrast).Preview(true);
}
#endif
}
