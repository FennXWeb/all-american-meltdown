#pragma once
#include "CoreMinimal.h"
class UCanvas;class UTexture2D;
namespace LWCardArt {
 void Chip(UCanvas* C,FVector2D Center,float Radius,FLinearColor Color);
 void Tile(UCanvas* C,UTexture2D* Atlas,int Cell,FVector2D Center,FVector2D Size,float Angle=0,FLinearColor Tint=FLinearColor::White);
 void Card(UCanvas* C,UTexture2D* Atlas,UTexture2D* Font,FVector2D Center,float Width,int Value,int Game,float Angle=0,float Alpha=1);
}
