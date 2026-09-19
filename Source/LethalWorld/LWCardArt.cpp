#include "LWCardArt.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "TextureResource.h"
#include "GlobalRenderResources.h"
namespace LWCardArt {
void Chip(UCanvas* C,FVector2D Center,float Radius,FLinearColor Color){TArray<FCanvasUVTri> Tri;for(int I=0;I<24;I++){FCanvasUVTri T;T.V0_Pos=Center;T.V1_Pos=Center+FVector2D(FMath::Cos(I*PI/12),FMath::Sin(I*PI/12))*Radius;T.V2_Pos=Center+FVector2D(FMath::Cos((I+1)*PI/12),FMath::Sin((I+1)*PI/12))*Radius;T.V0_UV=T.V1_UV=T.V2_UV=FVector2D::ZeroVector;T.V0_Color=Color;T.V1_Color=T.V2_Color=I%4==0?FLinearColor(.8f,.74f,.58f):Color;Tri.Add(T);}FCanvasTriangleItem Item(Tri,GWhiteTexture);Item.BlendMode=SE_BLEND_Translucent;C->DrawItem(Item);}

void Tile(UCanvas* C,UTexture2D* Atlas,int Cell,FVector2D Center,FVector2D Size,float Angle,FLinearColor Tint){if(!C||!Atlas)return;float U=(Cell%4)*.25f,V=(Cell/4)*.5f;FCanvasTileItem T(Center-Size*.5,Atlas->GetResource(),Size,FVector2D(U+.002f,V+.002f),FVector2D(U+.248f,V+.498f),Tint);T.BlendMode=SE_BLEND_Translucent;T.Rotation=FRotator(0,Angle,0);T.PivotPoint=FVector2D(.5,.5);C->DrawItem(T);}
void Card(UCanvas* C,UTexture2D* Atlas,UTexture2D* Font,FVector2D Center,float Width,int Value,int Game,float Angle,float Alpha){
 if(!C||!Atlas)return;const float H=Width*4.f/3.f;auto At=[&](float X,float Y){return Center+FVector2D(X*Width,Y*H).GetRotated(Angle);};
 auto Poly=[&](TArray<FVector2D> P,FLinearColor Ink){Ink.A*=Alpha;TArray<FCanvasUVTri> Tri;for(int I=1;I<P.Num()-1;I++){FCanvasUVTri T;T.V0_Pos=At(P[0].X,P[0].Y);T.V1_Pos=At(P[I].X,P[I].Y);T.V2_Pos=At(P[I+1].X,P[I+1].Y);T.V0_UV=T.V1_UV=T.V2_UV=FVector2D::ZeroVector;T.V0_Color=T.V1_Color=T.V2_Color=Ink;Tri.Add(T);}FCanvasTriangleItem Item(Tri,GWhiteTexture);Item.BlendMode=SE_BLEND_Translucent;C->DrawItem(Item);};
 auto Glyph=[&](FString S,float X,float Y,float Size,FLinearColor Ink,float Rotation=0){if(!Font)return;Ink.A*=Alpha;float Step=.065f*Size;for(int I=0;I<S.Len();I++){int Id=FMath::Clamp(int(S[I])-32,0,95);FVector2D P=At(X+I*Step,Y);FCanvasTileItem T(P,Font->GetResource(),FVector2D(Width*.085f*Size,H*.095f*Size),FVector2D((Id%16)/16.f,(Id/16)/6.f),FVector2D((Id%16+1)/16.f,(Id/16+1)/6.f),Ink);T.BlendMode=SE_BLEND_Translucent;T.Rotation=FRotator(0,Angle+Rotation,0);C->DrawItem(T);}};
 auto Pip=[&](int Suit,float X,float Y,float S,FLinearColor Ink){TArray<FVector2D> P;
  if(Suit==1)P={{0,-1},{.72f,0},{0,1},{-.72f,0}};
  else if(Suit==2)P={{0,1},{-.83f,.15f},{-.92f,-.38f},{-.62f,-.78f},{-.23f,-.8f},{0,-.5f},{.23f,-.8f},{.62f,-.78f},{.92f,-.38f},{.83f,.15f}};
  else if(Suit==3)P={{0,-1},{.85f,-.02f},{.92f,.42f},{.57f,.65f},{.17f,.45f},{.3f,.96f},{-.3f,.96f},{-.17f,.45f},{-.57f,.65f},{-.92f,.42f},{-.85f,-.02f}};
  else P={{0,-1},{.42f,-.8f},{.43f,-.35f},{.86f,-.32f},{1,.12f},{.67f,.58f},{.18f,.48f},{.3f,1},{-.3f,1},{-.18f,.48f},{-.67f,.58f},{-1,.12f},{-.86f,-.32f},{-.43f,-.35f},{-.42f,-.8f}};
  for(auto& V:P){V.X=X+V.X*S;V.Y=Y+V.Y*S*.75f;}Poly(P,Ink);
 };
 Tile(C,Atlas,Value<0?(Game==2?6:1):Game==2?5:0,Center,FVector2D(Width,H),Angle,FLinearColor(1,1,1,Alpha));if(Value<0)return;
 if(Game==2){int Rank=Value%15,Color=Value/15;const FLinearColor Colors[]={FLinearColor(.78f,.12f,.08f),FLinearColor(.98f,.74f,.12f),FLinearColor(.18f,.64f,.27f),FLinearColor(.12f,.45f,.88f),FLinearColor(.82f,.78f,.61f)};FLinearColor Ink=Colors[FMath::Clamp(Color,0,4)];
  Poly({{-.43f,-.4f},{-.31f,-.4f},{-.31f,.4f},{-.43f,.4f}},Ink);Poly({{.31f,-.4f},{.43f,-.4f},{.43f,.4f},{.31f,.4f}},Ink);
  FString Symbol=Rank<10?FString::FromInt(Rank):Rank==10?TEXT("X"):Rank==11?TEXT("<>"):Rank==12?TEXT("+2"):Rank==13?TEXT("W"):TEXT("+4");
  Glyph(Symbol,-.075f*Symbol.Len(),-.095f,2.35f,Ink);Glyph(Symbol,-.4f,-.41f,.95f,Ink);Glyph(Symbol,.36f,.40f,.95f,Ink,180);
  if(Color==4)for(int I=0;I<4;I++){float X=-.2f+I*.1f;Poly({{X,.21f},{X+.09f,.21f},{X+.09f,.28f},{X,.28f}},Colors[I]);}return;
 }
 const int Suit=Value/13,Rank=Value%13;FLinearColor Ink=Suit==1||Suit==2?FLinearColor(.48f,.035f,.025f):FLinearColor(.025f,.035f,.027f);
 FString R=Rank<9?FString::FromInt(Rank+2):Rank==9?TEXT("J"):Rank==10?TEXT("Q"):Rank==11?TEXT("K"):TEXT("A");
 Glyph(R,-.405f,-.43f,1.1f,Ink);Pip(Suit,-.365f,-.26f,.07f,Ink);Glyph(R,.40f,.43f,1.1f,Ink,180);Pip(Suit,.365f,.26f,.07f,Ink);
 if(Rank>=9&&Rank<=11){Tile(C,Atlas,Rank-7,Center,FVector2D(Width*.62f,H*.7f),Angle,FLinearColor(1,1,1,Alpha));return;}
 int Count=Rank==12?1:Rank+2;if(Count==1){Pip(Suit,0,0,.23f,Ink);return;}if(Count<=3){for(int I=0;I<Count;I++)Pip(Suit,0,-.25f+I*.5f/(Count-1),.115f,Ink);return;}
 int Rows=Count<=5?2:Count<=7?3:4;for(int I=0;I<Rows;I++)for(int Side:{-1,1})Pip(Suit,Side*.18f,-.28f+I*.56f/(Rows-1),.085f,Ink);if(Count%2)Pip(Suit,0,0,.085f,Ink);if(Count==10){Pip(Suit,0,-.18f,.085f,Ink);Pip(Suit,0,.18f,.085f,Ink);}
}
}

