#include "LWAppearance.h"
#include "LWHair61.h"
#include "Misc/PackageName.h"
#include "LWCreator35.h"
#include "LWZombie.h"
#include "Misc/Crc.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
namespace { FLinearColor FaceTone32(int I){auto C=LWOpening::Skin(I),B=LWOpening::Skin(I>=2?2:0);return FLinearColor(C.R/B.R,C.G/B.G,C.B/B.B);} }
UStaticMesh* LWAppearance::Mesh61(const FString& Name){
 static TMap<FString,TWeakObjectPtr<UStaticMesh>> Cache;
 if(auto* Found=Cache.Find(Name))if(Found->IsValid())return Found->Get();
 const FString NewPackage=TEXT("/Game/Art/Models63/SM_")+Name;
 const FString Package=FPackageName::DoesPackageExist(NewPackage)?NewPackage:TEXT("/Game/Art/Characters61/SM_")+Name;
 const FString Path=FPackageName::DoesPackageExist(Package)?Package+TEXT(".SM_")+Name:FString::Printf(TEXT("/Game/Art/Meshes/SM_%s.SM_%s"),*Name,*Name);
 auto* Mesh=LoadObject<UStaticMesh>(nullptr,*Path);Cache.Add(Name,Mesh);return Mesh;
}
void LWAppearance::Build(AActor* Owner,USceneComponent* Parent,const FLWIdentity& In,TArray<TObjectPtr<UStaticMeshComponent>>& Out,bool FirstPerson){
 FLWIdentity V=In;LWCreator35::Normalize(V);const float H=FMath::Lerp(.90f,1.10f,V.Height);const FString Sex=V.Body?TEXT("Female"):TEXT("Male");
 auto Part=[&](FString Name,FVector At,int Index,bool Hide=false){auto* C=NewObject<UStaticMeshComponent>(Owner);C->SetupAttachment(Parent);C->SetStaticMesh(Mesh61(Name));C->SetRelativeLocation(At*H);C->SetRelativeScale3D(FVector(1,1,H));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCastShadow(!FirstPerson);C->RegisterComponent();Out.Add(C);LWCreator35::Materials(C,Index,V);if(Index>=0&&Index<=6)LWCreator35::PreviewPart(C,Index,V,!Hide);else C->SetVisibility(!Hide);return C;};
 Part(Sex+FString::Printf(TEXT("Top%d35"),V.Top35),FVector(0,0,151),0,FirstPerson);
 Part(Sex+FString::Printf(TEXT("Waist%d35"),V.Bottom35),FVector(0,0,100),2,FirstPerson);
 Part(Sex+TEXT("AnatomicalHead35"),FVector(0,0,170.4),1,FirstPerson);
 if(V.Hair>0&&!V.Headwear35)Part(FString::Printf(TEXT("Hair%02d35"),V.Hair),FVector(0,0,170.4),1,FirstPerson);
 if(V.Beard35){auto B=V;B.HairColor=V.BeardColor35;auto* C=Part(FString::Printf(TEXT("Beard%02d35"),V.Beard35),FVector(0,0,170.4),1,FirstPerson);LWCreator35::Materials(C,1,B);}
 if(V.Headwear35)Part(FString::Printf(TEXT("Hat%d35"),V.Headwear35),FVector(0,0,170.4),1,FirstPerson);
 if(V.Eyewear35)Part(FString::Printf(TEXT("Eyewear%d35"),V.Eyewear35),FVector(0,0,170.4),1,FirstPerson);
 for(int Side=0;Side<2;Side++){float Sign=Side?1:-1;
  Part(FString::Printf(TEXT("Sleeve%d%s35"),V.Top35,Side?TEXT("R"):TEXT("L")),FVector(0,Sign*18*(1+(V.Shoulders-.5f)*.22f),146),3+Side,FirstPerson);
  auto* Leg=Part(FString::Printf(TEXT("Trousers%d%s35"),V.Bottom35,Side?TEXT("R"):TEXT("L")),FVector(0,Sign*9*(1+(LWCreator35::Shape(V,6)-.5f)*.2f),87),5+Side);Leg->ComponentTags.Add(Side?TEXT("RightLeg"):TEXT("LeftLeg"));
  auto* Shoe=Part(FString::Printf(TEXT("Footwear%d%s35"),V.Shoes35,Side?TEXT("R"):TEXT("L")),FVector(0,Sign*9,87),7);Shoe->AttachToComponent(Leg,FAttachmentTransformRules::KeepWorldTransform);Shoe->SetRelativeLocation(FVector::ZeroVector);Shoe->SetRelativeScale3D(FVector::OneVector);
 }
}
void LWAppearance::StyleNPC(AActor* Owner,const TArray<TObjectPtr<UStaticMeshComponent>>& Parts,FName Id,bool Female,bool Raider,bool Undead){
 if(Parts.Num()!=7)return;auto V=LWCreator35::ResidentIdentity(Id,Female,Raider,Undead);if(auto* N=Cast<ALWZombie>(Owner))N->Appearance35=V;
 const FString Sex=Female?TEXT("Female"):TEXT("Male");FString Names[]={Sex+FString::Printf(TEXT("Top%d35"),V.Top35),Sex+TEXT("AnatomicalHead35"),Sex+FString::Printf(TEXT("Waist%d35"),V.Bottom35),FString::Printf(TEXT("Sleeve%dL35"),V.Top35),FString::Printf(TEXT("Sleeve%dR35"),V.Top35),FString::Printf(TEXT("NPCLeg%dL35"),V.Bottom35),FString::Printf(TEXT("NPCLeg%dR35"),V.Bottom35)};
 for(int I=0;I<7;I++){auto* Mesh=Mesh61(Names[I]);if(Mesh)Parts[I]->SetStaticMesh(Mesh);Parts[I]->EmptyOverrideMaterials();LWCreator35::Materials(Parts[I],I,V,Undead);}
 Parts[1]->SetRelativeLocation(FVector(0,0,82.4));
 for(auto* C:Owner->GetComponentsByTag(USceneComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();
 auto Detail=[&](FString Name,bool Beard){auto* C=NewObject<UStaticMeshComponent>(Owner);C->SetupAttachment(Parts[1]);C->SetStaticMesh(Mesh61(Name));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->ComponentTags.Add(TEXT("CharacterHair32"));C->SetCanEverAffectNavigation(false);C->RegisterComponent();auto Groom=V;if(Beard)Groom.HairColor=V.BeardColor35;LWCreator35::Materials(C,1,Groom,Undead);C->SetRelativeScale3D(FVector(1+(LWCreator35::Shape(V,23)-.5f)*.24f,1+(LWCreator35::Shape(V,10)-.5f)*.28f,1+(LWCreator35::Shape(V,11)-.5f)*.2f));if(ULWHair61::Supports(C)){auto* Hair=NewObject<ULWHair61>(Owner);Hair->SetupAttachment(C);Hair->ComponentTags.Add(TEXT("CharacterHair32"));Hair->RegisterComponent();Hair->Initialize(C,V,true,false);}};
 if(V.Hair)Detail(FString::Printf(TEXT("Hair%02d35"),V.Hair),false);if(V.Beard35)Detail(FString::Printf(TEXT("Beard%02d35"),V.Beard35),true);
}
