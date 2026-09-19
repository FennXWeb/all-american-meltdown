#include "LWWorld.h"
#include "LWSurface26.h"
#include "ProceduralMeshComponent.h"
void ALWChunk::AddSlab(ALWWorld* W,FName Mat,FVector P,FVector Size,FRotator R,bool Collision){
 using namespace LWSurface26;Polygon Outline;for(FVector2D V:{FVector2D(-1,-1),FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1)})Outline.Add(FVector2D(P)+FVector2D(V.X*Size.X*.5,V.Y*Size.Y*.5).GetRotated(R.Yaw));
 const float Top=P.Z+Size.Z*.5,Bottom=P.Z-Size.Z*.5;auto& Previous=SlabFootprints.FindOrAdd(FMath::RoundToInt(Top*10));TArray<Polygon> Pieces{Outline};
 FBox2D Bounds(Outline);for(const auto& Old:Previous){if(!Bounds.Intersect(FBox2D(Old)))continue;TArray<Polygon> Next;for(const auto& Piece:Pieces)Next.Append(Subtract(Piece,Old));Pieces=MoveTemp(Next);if(Pieces.IsEmpty())return;}
 Previous.Add(Outline);FName Key(*(Mat.ToString()+(Collision?TEXT("_C"):TEXT("_N"))));auto& B=SurfaceData.FindOrAdd(Key);
 auto Face=[&](const TArray<FVector>& V,FVector Normal){int Start=B.Vertices.Num();for(auto At:V){B.Vertices.Add(At);B.Normals.Add(Normal);B.UV.Add(FVector2D(At.X,At.Y)/200);}for(int I=1;I<V.Num()-1;I++)if(FVector::CrossProduct(V[I]-V[0],V[I+1]-V[0]).SizeSquared()>.0001)B.Triangles.Append({Start,Start+I+1,Start+I});};
 for(const auto& Piece:Pieces){TArray<FVector> V;for(auto XY:Piece)V.Add(FVector(XY,Top));Face(V,FVector::UpVector);V.Empty();for(int I=Piece.Num()-1;I>=0;I--)V.Add(FVector(Piece[I],Bottom));Face(V,-FVector::UpVector);}
 for(int I=0;I<4;I++){auto A=Outline[I],C=Outline[(I+1)%4];FVector N(C.Y-A.Y,A.X-C.X,0);Face({FVector(A,Bottom),FVector(C,Bottom),FVector(C,Top),FVector(A,Top)},N.GetSafeNormal());}
 if(!SurfaceMeshes.Contains(Key)){auto* M=NewObject<UProceduralMeshComponent>(this);M->SetupAttachment(RootComponent);M->bUseAsyncCooking=false;M->SetCollisionProfileName(Collision?TEXT("BlockAll"):TEXT("NoCollision"));M->SetCanEverAffectNavigation(false);M->SetMaterial(0,W->Material(Mat));if(Mat==TEXT("Asphalt"))M->ComponentTags.Add(TEXT("RoadSurface"));M->RegisterComponent();SurfaceMeshes.Add(Key,M);}
 if(!BuildingSurfaces)FlushSurfaces();
}
void ALWChunk::FlushSurfaces(){for(auto& Pair:SurfaceData){auto* M=SurfaceMeshes.FindRef(Pair.Key).Get();if(!M)continue;auto& B=Pair.Value;M->CreateMeshSection(0,B.Vertices,B.Triangles,B.Normals,B.UV,TArray<FColor>(),TArray<FProcMeshTangent>(),M->GetCollisionEnabled()!=ECollisionEnabled::NoCollision);}}
