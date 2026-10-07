#include "LWHair61.h"
#include "LWCreator35.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Misc/App.h"

ULWHair61::ULWHair61(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.bTickEvenWhenPaused=true;SetCollisionEnabled(ECollisionEnabled::NoCollision);SetCanEverAffectNavigation(false);}
bool ULWHair61::Supports(const UStaticMeshComponent* Source){return Source&&Source->GetStaticMesh()&&(Source->GetStaticMesh()->GetPathName().Contains(TEXT("/Characters61/SM_Hair"))||Source->GetStaticMesh()->GetPathName().Contains(TEXT("/Models63/SM_Hair")));}
void ULWHair61::Initialize(UStaticMeshComponent* Source,const FLWIdentity& Identity,bool Visible,bool Morph){
 if(!Supports(Source)||!Source->GetStaticMesh()->bAllowCPUAccess)return;
 const auto& LOD=Source->GetStaticMesh()->GetRenderData()->LODResources[0];
 const auto& Vertices=LOD.VertexBuffers.StaticMeshVertexBuffer;
 const auto Indices=LOD.IndexBuffer.GetArrayView();
 for(int S=0;S<LOD.Sections.Num();S++){
  const auto& Input=LOD.Sections[S];FProcMeshSection Output;TMap<uint32,int32> Remap;
  for(uint32 I=Input.FirstIndex;I<Input.FirstIndex+Input.NumTriangles*3;I++){
   const uint32 Original=Indices[I];int32* Existing=Remap.Find(Original);int32 Local;
   if(Existing)Local=*Existing;
   else{
    Local=Output.ProcVertexBuffer.Num();Remap.Add(Original,Local);FProcMeshVertex V;
    V.Position=FVector(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(Original));
    V.Normal=FVector(Vertices.VertexTangentZ(Original));
    const FVector4 Tangent(Vertices.VertexTangentX(Original));V.Tangent=FProcMeshTangent(FVector(Tangent),Tangent.W<0);
    if(Vertices.GetNumTexCoords()>0)V.UV0=FVector2D(Vertices.GetVertexUV(Original,0));
    if(Vertices.GetNumTexCoords()>1)V.UV1=FVector2D(Vertices.GetVertexUV(Original,1));
    if(Vertices.GetNumTexCoords()>2)V.UV2=FVector2D(Vertices.GetVertexUV(Original,2));
    Output.ProcVertexBuffer.Add(V);
   }
   Output.ProcIndexBuffer.Add(Local);
  }
  Output.bEnableCollision=false;SetProcMeshSection(S,Output);
 }
 for(int S=0;S<GetNumSections();S++){
  auto Section=*GetProcMeshSection(S);FSection61 Buffer;
  for(auto& V:Section.ProcVertexBuffer){if(Morph)V.Position=LWCreator35::Morph(V.Position,1,Identity);Buffer.Rest.Add(V.Position);Buffer.Position.Add(V.Position);Buffer.Normals.Add(V.Normal);Buffer.UV.Add(V.UV0);Buffer.Tangents.Add(V.Tangent);Buffer.Compliance.Add(FMath::Clamp(float(V.UV2.X),0.f,1.f));}
  Section.SectionLocalBox=FBox(ForceInit);for(const auto& V:Section.ProcVertexBuffer)Section.SectionLocalBox+=V.Position;
  SetProcMeshSection(S,Section);SetMaterial(S,Source->GetMaterial(Source->GetStaticMesh()->GetRenderData()->LODResources[0].Sections[S].MaterialIndex));Sections61.Add(MoveTemp(Buffer));
 }
 SetVisibility(Visible);Source->SetVisibility(false,false);BoundsScale=1.15f;
}
void ULWHair61::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function){
 Super::TickComponent(Dt,Type,Function);
 Accumulated61+=FMath::Min(float(FApp::GetDeltaTime()),.1f);if(Accumulated61<1.f/30)return;Dt=Accumulated61;Accumulated61=0;
 const FVector Position=GetComponentLocation();const FQuat Rotation=GetComponentQuat();auto* Camera=UGameplayStatics::GetPlayerCameraManager(this,0);
 if(!IsVisible()||!Camera||FVector::DistSquared(Position,Camera->GetCameraLocation())>FMath::Square(2500.f)){HasHistory61=false;return;}
 if(!HasHistory61||Dt>.2f||FVector::DistSquared(Position,PreviousPosition61)>FMath::Square(250.f)){Displacement61=Velocity61=PreviousVelocity61=FVector::ZeroVector;PreviousPosition61=Position;PreviousRotation61=Rotation;HasHistory61=true;}
 const float Step=FMath::Clamp(Dt,.001f,.1f);Clock61+=Step;
 FVector Motion=(Position-PreviousPosition61)/Step;
 FVector Acceleration=(Motion-PreviousVelocity61)/Step;
 FVector Angular=Rotation.UnrotateVector((Rotation*PreviousRotation61.Inverse()).ToRotationVector()/Step);
 FVector Force=-Rotation.UnrotateVector(Acceleration).GetClampedToMaxSize(1200.f)*.006f+FVector(-Angular.Y,Angular.X,0)*.8f;
 // A small irregular breeze supplements inertia. Roots have zero compliance.
 Force+=FVector(FMath::Sin(Clock61*1.7f)*.75f,FMath::Sin(Clock61*2.3f+.8f)*1.1f,-.20f);
 const int Steps=FMath::Max(1,FMath::CeilToInt(Step*120));const float H=Step/Steps;
 for(int I=0;I<Steps;I++){Velocity61+=(Force-Displacement61*28.f-Velocity61*8.f)*H;Displacement61+=Velocity61*H;Displacement61=Displacement61.GetClampedToMaxSize(2.f);}
 for(int S=0;S<Sections61.Num();S++){
  auto& B=Sections61[S];for(int I=0;I<B.Rest.Num();I++){
   FVector Offset=Displacement61*B.Compliance[I];const FVector Radial=FVector(B.Rest[I].X,B.Rest[I].Y,0).GetSafeNormal();const float Inward=FVector::DotProduct(Offset,Radial);
   if(Inward<-.15f)Offset+=Radial*(-.15f-Inward);B.Position[I]=B.Rest[I]+Offset;
  }
  UpdateMeshSection_LinearColor(S,B.Position,B.Normals,B.UV,TArray<FLinearColor>(),B.Tangents);
 }
 PreviousPosition61=Position;PreviousVelocity61=Motion;PreviousRotation61=Rotation;
}

bool ULWHair61::ValidateDeformation61() const {
 int Flexible=0,Roots=0;bool Moved=false;
 for(int S=0;S<Sections61.Num();S++){
  const auto& B=Sections61[S];for(int I=0;I<B.Rest.Num();I++){
   if(B.Compliance[I]>.01f){Flexible++;Moved|=!B.Position[I].Equals(B.Rest[I],.00001f);}
   else if(B.Compliance[I]==0){Roots++;if(!B.Position[I].Equals(B.Rest[I],.00001f))return false;}
  }
 }
 return Flexible>0&&Roots>0&&Moved;
}
