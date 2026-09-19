#if WITH_DEV_AUTOMATION_TESTS
#include "LWWeaponMods.h"
#include "LWWorldTextComponent.h"
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWOpenSightSmoke,"LethalWorld.Weapons.OpenSightApertureSmoke",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWOpenSightSmoke::RunTest(const FString& Parameters)
{
    const FName Guns[]={TEXT("shotgun"),TEXT("revolver"),TEXT("sniper"),TEXT("smg"),TEXT("rifle"),TEXT("lmg"),TEXT("desert_eagle"),TEXT("m4")};
    const FTransform Meshes[]={FTransform::Identity,FTransform(FRotator(8,23,-5),FVector(3,-2,4),FVector(1.2,.9,1.1))};
    for(FName Gun:Guns) for(FName Optic:{FName(TEXT("att_reflex")),FName(TEXT("att_holo"))})
    {
        auto G=LWItems::Make(Gun);G.Attachments.Add(TEXT("Optic"),Optic);
        TestTrue(TEXT("Supported gun enables open-sight reticle"),LWMods::OpenSight(&G));
        for(const FTransform& Mesh:Meshes)
        {
            FVector Rest(65,11,-16);FRotator Rotation(3,4,1);
            const int W=LWItems::Def(Gun).WeaponIndex;
            LWMods::AlignOpenSight(&G,W,Mesh,Rest,Rotation);
            // Independent authored geometry: base 0.8cm; window 4.1/5.2cm high.
            const float WindowCenter=.8f+(Optic==TEXT("att_holo")?5.2f:4.1f)*.5f;
            const FVector Aperture=Rest+Rotation.RotateVector(Mesh.TransformPosition(LWMods::Position(W,TEXT("Optic"))+FVector(0,0,WindowCenter)));
            TestTrue(TEXT("Aperture lies on camera firing ray"),FMath::Abs(Aperture.Y)<.001 && FMath::Abs(Aperture.Z)<.001);
            TestTrue(TEXT("Aperture stays ahead of camera near plane"),Aperture.X>10);
            TestTrue(TEXT("Sight bore points along firing direction"),Rotation.RotateVector(Mesh.TransformVectorNoScale(FVector::ForwardVector)).Equals(FVector::ForwardVector,.001));
        }
    }
    auto Incompatible=LWItems::Make(TEXT("missile_launcher"));Incompatible.Attachments.Add(TEXT("Optic"),TEXT("att_reflex"));
    TestFalse(TEXT("Invalid mount cannot suppress normal reticle"),LWMods::OpenSight(&Incompatible));
    TestFalse(TEXT("Unarmed has no optic"),LWMods::OpenSight(nullptr));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWorldTextSmoke,"LethalWorld.World.TextFacingAndBoundsSmoke",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWorldTextSmoke::RunTest(const FString& Parameters)
{
    for(EHorizTextAligment Alignment:{EHTA_Left,EHTA_Center,EHTA_Right}) for(float Yaw:{0.f,37.f,90.f,180.f})
    {
        TStrongObjectPtr<ULWWorldTextComponent> Text(NewObject<ULWWorldTextComponent>());
        Text->SetText(FText::FromString(TEXT("ASYMMETRIC SIGN 123")));
        Text->SetHorizontalAlignment(Alignment);
        Text->SetWorldTransform(FTransform(FRotator(0,Yaw,0),FVector(250,350,150),FVector(1,1.3,.8)));
        Text->UpdateBounds();
        const FTransform Before=Text->GetComponentTransform();
        const auto Bounds=Text->CalcBounds(Before);
        TestTrue(TEXT("Real glyph bounds available"),Bounds.BoxExtent.Size()>1);
        TestFalse(TEXT("Labels have no individual tick"),Text->PrimaryComponentTick.bCanEverTick);
        const FVector Front=Before.GetUnitAxis(EAxis::X);
        Text->UpdateFacing(Bounds.Origin-Front*13000);
        TestTrue(TEXT("Distant back-facing signs do not update"),Before.Equals(Text->GetComponentTransform(),.001));
        Text->UpdateFacing(Bounds.Origin-Front);
        TestTrue(TEXT("Edge-on dead zone does not flip"),Before.Equals(Text->GetComponentTransform(),.001));
        Text->UpdateFacing(Bounds.Origin-Front*100);
        const auto After=Text->CalcBounds(Text->GetComponentTransform());
        TestTrue(TEXT("Back reader sees readable face"),FVector::DotProduct(Text->GetForwardVector(),-Front)>.999);
        TestTrue(TEXT("Left/center/right glyph center stays fixed"),After.Origin.Equals(Bounds.Origin,.01));
        TestTrue(TEXT("Occupied sign bounds stay fixed"),After.BoxExtent.Equals(Bounds.BoxExtent,.01));
        Text->UpdateFacing(Bounds.Origin+Front*100);
        TestTrue(TEXT("Return to front restores authored transform without drift"),Before.Equals(Text->GetComponentTransform(),.01));
    }
    return true;
}
#endif
