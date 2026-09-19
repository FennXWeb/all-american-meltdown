#pragma once
#include "LWOpening.h"
#include "ProceduralMeshComponent.h"
class ALWCharacter;
namespace LWCreator35 {
 constexpr int ShapeCount=24;
 bool Click(ALWCharacter* P,FName N);
 void Normalize(FLWIdentity& V);
 float Shape(const FLWIdentity& V,int I);
 void SetShape(FLWIdentity& V,int I,float Value);
 const TCHAR* ShapeName(int I);
 const TCHAR* HairName(int I);
 const TCHAR* BeardName(int I);
 const TCHAR* TopName(int I);
 const TCHAR* BottomName(int I);
 const TCHAR* ColorName(int I);
 FLinearColor Color(int I);
 FVector Morph(FVector P,int Part,const FLWIdentity& V);
 void Surface(FProcMeshSection& Section,int Part,const FLWIdentity& V);
 void Materials(class UMeshComponent* C,int Part,const FLWIdentity& V,bool Dead=false);
 void PreviewPart(class UStaticMeshComponent* Part,int Index,const FLWIdentity& V,bool Visible);
 void Cleanup(AActor* Owner);
 FLWIdentity ResidentIdentity(FName Id,bool Female,bool Raider,bool Undead);
}
