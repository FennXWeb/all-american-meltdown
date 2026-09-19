#include "LWCreator35.h"
#include "LWCharacter.h"
#include "LWAppearancePreset35.h"
#include "Kismet/GameplayStatics.h"
bool LWCreator35::Click(ALWCharacter* P,FName N){
 FString K=N.ToString();if(!K.StartsWith(TEXT("c35_")))return false;K.RightChopInline(4);auto& V=P->DraftIdentity;Normalize(V);P->NameEditing=false;
 if(K.StartsWith(TEXT("tab_"))){P->CreatorTab35=FMath::Clamp(FCString::Atoi(*K.Mid(4)),0,6);P->CreatorPage35=0;P->CreatorZoom35=P->CreatorTab35==1||P->CreatorTab35==2?1:0;}
 else if(K==TEXT("page"))P->CreatorPage35=1-P->CreatorPage35;
 else if(K==TEXT("body"))V.Body=1-V.Body;
 else if(K==TEXT("skin"))V.Skin=(V.Skin+1)%6;
 else if(K==TEXT("hair"))V.Hair=(V.Hair+1)%20;
 else if(K==TEXT("hair_prev"))V.Hair=(V.Hair+19)%20;
 else if(K==TEXT("beard"))V.Beard35=(V.Beard35+1)%11;
 else if(K==TEXT("beard_prev"))V.Beard35=(V.Beard35+10)%11;
 else if(K==TEXT("haircolor"))V.HairColor=(V.HairColor+1)%12;
 else if(K==TEXT("beardcolor"))V.BeardColor35=(V.BeardColor35+1)%12;
 else if(K==TEXT("eyecolor"))V.EyeColor35=(V.EyeColor35+1)%12;
 else if(K==TEXT("top"))V.Top35=(V.Top35+1)%9;
 else if(K==TEXT("bottom"))V.Bottom35=(V.Bottom35+1)%6;
 else if(K==TEXT("shoes"))V.Shoes35=(V.Shoes35+1)%4;
 else if(K==TEXT("headwear"))V.Headwear35=(V.Headwear35+1)%6;
 else if(K==TEXT("gloves"))V.Gloves35=(V.Gloves35+1)%4;
 else if(K==TEXT("eyewear"))V.Eyewear35=(V.Eyewear35+1)%4;
 else if(K==TEXT("topcolor"))V.TopColor35=(V.TopColor35+1)%12;
 else if(K==TEXT("bottomcolor"))V.BottomColor35=(V.BottomColor35+1)%12;
 else if(K==TEXT("shoecolor"))V.ShoesColor35=(V.ShoesColor35+1)%12;
 else if(K==TEXT("zone")){P->TattooZone35=(P->TattooZone35+1)%6;P->CreatorZoom35=P->TattooZone35<2?1:2;P->PortraitYaw=P->TattooZone35==3?180:0;}
 else if(K==TEXT("tattoo"))V.Tattoos35[P->TattooZone35]=(V.Tattoos35[P->TattooZone35]+1)%17;
 else if(K==TEXT("tattoo_prev"))V.Tattoos35[P->TattooZone35]=(V.Tattoos35[P->TattooZone35]+16)%17;
 else if(K==TEXT("clear_tattoos"))for(auto& I:V.Tattoos35)I=0;
 else if(K==TEXT("left"))P->PortraitYaw=FMath::Fmod(P->PortraitYaw-22.5f,360);
 else if(K==TEXT("right"))P->PortraitYaw=FMath::Fmod(P->PortraitYaw+22.5f,360);
 else if(K==TEXT("zoom"))P->CreatorZoom35=(int(P->CreatorZoom35)+1)%3;
 else if(K==TEXT("reset_shape")){for(int I=0;I<ShapeCount;I++)SetShape(V,I,.5f);}
 else if(K==TEXT("random_face")){for(int I=10;I<ShapeCount;I++)SetShape(V,I,FMath::FRandRange(.15f,.85f));}
 else if(K==TEXT("random_all")){FString Name=V.Name;V=ResidentIdentity(FName(*FGuid::NewGuid().ToString()),V.Body!=0,false,false);V.Name=Name;V.Weight=FMath::FRandRange(.25f,.75f);V.Height=FMath::FRandRange(.25f,.75f);}
 else if(K==TEXT("preset"))P->CreatorPreset35=P->CreatorPreset35%5+1;
 else if(K==TEXT("save")){auto* S=NewObject<ULWAppearancePreset35>();S->Identity=V;P->CreationMessage=UGameplayStatics::SaveGameToSlot(S,FString::Printf(TEXT("AAM_Appearance35_%d"),P->CreatorPreset35),0)?TEXT("LOOK SAVED"):TEXT("COULD NOT SAVE LOOK");}
 else if(K==TEXT("load")){auto* S=Cast<ULWAppearancePreset35>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("AAM_Appearance35_%d"),P->CreatorPreset35),0));if(S){V=S->Identity;Normalize(V);P->CreationMessage=TEXT("LOOK LOADED");}else P->CreationMessage=TEXT("THIS PRESET IS EMPTY");}
 P->CreatorDirty35=true;return true;
}
