#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
void ALWVehicle::AddDent(FVector Point,FVector Direction,float Damage){
 auto* R=Record();if(!R||!Body||!Body->GetStaticMesh()||R->Exploded||Damage<2||!FMath::IsFinite(Damage)||Point.ContainsNaN()||Direction.ContainsNaN()||Direction.IsNearlyZero())return;
 FLWVehicleDent D;D.Point=Body->GetComponentTransform().InverseTransformPosition(Point);D.Direction=Body->GetComponentTransform().InverseTransformVectorNoScale(Direction).GetSafeNormal();D.Depth=FMath::Clamp(Damage*.25f,1.f,28.f);D.Radius=FMath::Clamp(40.f+Damage,45.f,150.f);
 // Chassis contacts can sit just outside the painted shell; project onto its bounds.
 D.Point=Body->GetStaticMesh()->GetBoundingBox().GetClosestPointTo(D.Point);
 if(FVector::DotProduct(D.Direction,D.Point-Body->GetStaticMesh()->GetBoundingBox().GetCenter())>0)D.Direction*=-1;
 auto* Old=R->Dents.FindByPredicate([&](const auto& A){return FVector::DistSquared(A.Point,D.Point)<2500;});
 if(Old){Old->Depth=FMath::Min(35.f,Old->Depth+D.Depth*.5f);Old->Radius=FMath::Max(Old->Radius,D.Radius);}
 else if(R->Dents.Num()<12)R->Dents.Add(D);else {int Nearest=0;for(int I=1;I<R->Dents.Num();I++)if(FVector::DistSquared(R->Dents[I].Point,D.Point)<FVector::DistSquared(R->Dents[Nearest].Point,D.Point))Nearest=I;R->Dents[Nearest].Depth=FMath::Min(35.f,R->Dents[Nearest].Depth+D.Depth*.25f);}
 bDentsDirty=true;if(!DentedBody||GetWorld()->GetTimeSeconds()-LastDentBuild>=.2f)RebuildDents();
}
void ALWVehicle::RebuildDents(){
 auto* R=Record();if(!R||R->Dents.IsEmpty()||!Body||!Body->GetStaticMesh()||!Body->GetStaticMesh()->bAllowCPUAccess)return;
 if(!DentedBody){
  DentedBody=NewObject<UProceduralMeshComponent>(this);DentedBody->SetupAttachment(Body);DentedBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);DentedBody->SetCanEverAffectNavigation(false);DentedBody->RegisterComponent();
  UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(Body,0,DentedBody,false);
  // Subdivide long triangles so impacts deform the middle of broad low-poly panels.
  for(int S=0;S<DentedBody->GetNumSections();S++){
   auto* Section=DentedBody->GetProcMeshSection(S);if(!Section)continue;
   FProcMeshSection Mesh=*Section;
   for(int Pass=0;Pass<3&&Mesh.ProcVertexBuffer.Num()<12000;Pass++){
    TArray<uint32> Indices;
    for(int I=0;I+2<Mesh.ProcIndexBuffer.Num();I+=3){uint32 A=Mesh.ProcIndexBuffer[I],B=Mesh.ProcIndexBuffer[I+1],C=Mesh.ProcIndexBuffer[I+2];
     if(FVector::DistSquared(Mesh.ProcVertexBuffer[A].Position,Mesh.ProcVertexBuffer[B].Position)<2500&&FVector::DistSquared(Mesh.ProcVertexBuffer[B].Position,Mesh.ProcVertexBuffer[C].Position)<2500&&FVector::DistSquared(Mesh.ProcVertexBuffer[C].Position,Mesh.ProcVertexBuffer[A].Position)<2500){Indices.Append({A,B,C});continue;}
     auto Mid=[&](uint32 X,uint32 Y){FProcMeshVertex V=Mesh.ProcVertexBuffer[X];const auto Other=Mesh.ProcVertexBuffer[Y];V.Position=(V.Position+Other.Position)*.5;V.Normal=(V.Normal+Other.Normal).GetSafeNormal();V.UV0=(V.UV0+Other.UV0)*.5;return uint32(Mesh.ProcVertexBuffer.Add(V));};
     uint32 AB=Mid(A,B),BC=Mid(B,C),CA=Mid(C,A);Indices.Append({A,AB,CA,AB,B,BC,CA,BC,C,AB,BC,CA});
    }Mesh.ProcIndexBuffer=MoveTemp(Indices);
   }
   DentedBody->SetProcMeshSection(S,Mesh);TArray<FVector> Base;for(const auto& V:Mesh.ProcVertexBuffer)Base.Add(V.Position);DentOriginal.Add(MoveTemp(Base));
  }
  Body->SetVisibility(false,false);
 }
 for(int S=0;S<DentedBody->GetNumSections();S++){
  auto* Section=DentedBody->GetProcMeshSection(S);if(!Section||!DentOriginal.IsValidIndex(S))continue;FProcMeshSection Mesh=*Section;
  TArray<FVector> Pos,Normals;TArray<FVector2D> UV;TArray<int32> Indices;TArray<FProcMeshTangent> Tangents;
  for(int I=0;I<Mesh.ProcVertexBuffer.Num();I++){const FVector Base=DentOriginal[S][I];FVector Delta=FVector::ZeroVector;
   for(const auto& D:R->Dents){float Distance=FVector::Distance(Base,D.Point),Radius=FMath::Clamp(D.Radius,10.f,180.f);if(Distance<Radius){float Fall=1-Distance/Radius;Delta+=D.Direction.GetSafeNormal()*FMath::Clamp(D.Depth,0.f,35.f)*Fall*Fall;}}
   Pos.Add(Base+Delta.GetClampedToMaxSize(40));UV.Add(Mesh.ProcVertexBuffer[I].UV0);
  }
  for(uint32 I:Mesh.ProcIndexBuffer)Indices.Add(int32(I));UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Pos,Indices,UV,Normals,Tangents);
  DentedBody->UpdateMeshSection_LinearColor(S,Pos,Normals,UV,TArray<FLinearColor>(),Tangents);
 }
 DentedBody->SetVisibility(true);bDentsDirty=false;LastDentBuild=GetWorld()->GetTimeSeconds();
}
