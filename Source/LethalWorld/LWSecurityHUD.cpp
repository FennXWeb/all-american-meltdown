#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
void ALWHUD::SecurityScreen(ALWCharacter* P){
 const float W=Canvas->SizeX/Scale;FLinearColor Bone(.8f,.81f,.66f),Amber(.9f,.56f,.2f),Red(.8f,.18f,.08f),Green(.4f,.75f,.4f);Rect(0,0,W,720,FLinearColor(.015f,.025f,.018f,.96f));
 Text(P->SecurityMode==1?TEXT("MECHANICAL BYPASS"):TEXT("IGNITION HARNESS"),55,48,2.2f,Amber);Button(TEXT("security_close"),TEXT("CANCEL"),W-225,45,170);
 auto Segment=[&](FVector2D A,FVector2D B,FLinearColor C,float T=2){Line(A.X,A.Y,B.X,B.Y,C,T);};
 if(P->SecurityMode==1){
 Text(FString::Printf(TEXT("%s LOCK // %d PICKS // CONDITION %03d"),LWSecurity::Tier(P->SecurityTier),P->CountSupply(TEXT("lockpick")),FMath::CeilToInt(P->PickHealth)),55,110,1.1f,Bone);
 FVector2D C(W*.5,350);for(int Ring:{148,155,172})for(int I=0;I<64;I++){float A=I*2*PI/64,B=(I+1)*2*PI/64;Segment(C+FVector2D(FMath::Cos(A),FMath::Sin(A))*Ring,C+FVector2D(FMath::Cos(B),FMath::Sin(B))*Ring,I%3?Bone*.5f:Amber,3);}
 for(int I=0;I<12;I++){float A=I*PI/6;Segment(C+FVector2D(FMath::Cos(A),FMath::Sin(A))*128,C+FVector2D(FMath::Cos(A),FMath::Sin(A))*145,Bone,2);}
 float R=FMath::DegreesToRadians(P->LockTurn);FVector2D D(FMath::Sin(R),-FMath::Cos(R));Segment(C-D*90,C+D*90,Bone,14);Segment(C,C+FVector2D(FMath::Cos(R),FMath::Sin(R))*170,Amber,9);
 float A=FMath::DegreesToRadians(P->PickAngle);FVector2D Tip=C+FVector2D(FMath::Sin(A),-FMath::Cos(A))*245;Segment(C,Tip,P->PickHealth<30?Red:Green,5);Segment(Tip,Tip+FVector2D(17,-8),Bone,4);
 Text(TEXT("ROTATE PICK: A / D OR MOUSE    APPLY TORQUE: HOLD LMB / SPACE"),W*.5-330,565,.9f,Bone);
 Text(TEXT("FEEL FOR THE SWEET SPOT. RELEASE TORQUE WHEN THE CYLINDER BINDS."),W*.5-330,596,.85f,Amber);
 Rect(W*.5-250,638,500,9,Bone*.2f);Rect(W*.5-250,638,500*P->PickHealth/100,9,P->PickHealth<30?Red:Green);
 }else{
 static const TCHAR* Names[]={TEXT("RED"),TEXT("BLUE"),TEXT("AMBER"),TEXT("GREEN"),TEXT("WHITE"),TEXT("VIOLET")};const FLinearColor Colors[]={Red,FLinearColor(.2f,.4f,.9f),Amber,Green,Bone,FLinearColor(.7f,.3f,.8f)};
 if(P->WireOrder.Num()!=6)return;
 Text(TEXT("SERVICE PLATE // TRACE TERMINALS, THEN BRIDGE IN ORDER"),55,111,1,Bone);
 Text(FString::Printf(TEXT("BATTERY +12V: PIN %d    IGNITION: PIN %d    STARTER: PIN %d"),P->WireOrder[0]+1,P->WireOrder[1]+1,P->WireOrder[2]+1),55,151,1.05f,Amber);
 for(int I=0;I<6;I++){float X=75+I*190;Line(X+65,223,X+65,380,Colors[I],7);Rect(X+40,202,50,30,Bone*.3f);Text(FString::Printf(TEXT("%d"),I+1),X+59,207,1,Bone);Button(FName(*FString::Printf(TEXT("wire_%d"),I)),Names[I],X,391,172);bool Joined=false;for(int J=0;J<FMath::Min(P->WireStage,3);J++)Joined|=P->WireOrder[J]==I;if(Joined)Text(TEXT("JOINED"),X+34,445,.9f,Green);}
 Text(P->WireStage<3?FString::Printf(TEXT("SELECT %s LEAD"),P->WireStage==0?TEXT("BATTERY"):P->WireStage==1?TEXT("IGNITION"):TEXT("STARTER")):TEXT("HOLD SPACE / LMB ONLY WHILE THE NEEDLE IS IN THE GREEN BAND"),55,494,1,Bone);
 Rect(80,549,1100,30,Bone*.15f);Rect(80+1100*.7f/2.4f,549,1100*.75f/2.4f,30,Green*.7f);float Phase=FMath::Fmod(P->WireClock,2.4f)/2.4f;Rect(80+1096*Phase,544,4,40,Amber);
 Text(TEXT("STARTER"),80,614,.9f,Bone);Rect(200,619,360*FMath::Clamp(P->WireProgress/1.3f,0.f,1.f),10,Green);
 Text(TEXT("HEAT"),690,614,.9f,Bone);Rect(780,619,360*P->WireHeat/100,10,Red);
 }
}
void ALWHUD::VehicleOverlay(ALWCharacter* P){auto* C=P->Vehicle.Get();if(!C||!C->Record())return;float W=Canvas->SizeX/Scale;
 Rect(304,630,W-745,61,FLinearColor(.015f,.03f,.02f,.60f));Text(FString::Printf(TEXT("%03d KM/H  %s  %s"),FMath::RoundToInt(FMath::Abs(C->Speed)*.036f),C->Speed< -5?TEXT("REV"):TEXT("DRV"),C->EngineOn?TEXT("ENGINE ON"):TEXT("IGNITION OFF")),315,636,.85);
 const float Capacity=C->FuelCapacity(),Fuel=C->FuelLitres();
 const float Fraction=Capacity>0?FMath::Clamp(Fuel/Capacity,0.f,1.f):0.f;
 const FLinearColor FuelColor=Fuel<=0?FLinearColor(.95f,.25f,.12f):Fraction<=.15f?FLinearColor(.95f,.65f,.2f):FLinearColor(.55f,.8f,.45f);
 Text(FString::Printf(TEXT("FUEL %.1f / %.0f L%s"),Fuel,Capacity,Fuel<=0?TEXT(" // EMPTY"):Fraction<=.15f?TEXT(" // LOW"):TEXT("")),315,655,.75f,FuelColor);
 const float GaugeWidth=FMath::Max(0.f,W-1053.f);
 Rect(600,660,GaugeWidth,6,FLinearColor(.18f,.23f,.19f,.9f));
 if(Fraction>0)Rect(600,660,GaugeWidth*Fraction,6,FuelColor);
 Text(FString::Printf(TEXT("VIN %s // %s"),*C->Record()->VIN.ToString(EGuidFormats::Digits).Left(12),C->Record()->Hotwired?TEXT("BYPASSED"):P->HasKey(C->Record()->VIN)?TEXT("KEY MATCH"):TEXT("NO KEY")),315,675,.8f);
 Text(TEXT("[G] GLOVE [T] RADIO [F] LIGHTS [V] WIPERS [Z] / [X] SIGNALS [E] USE / EXIT"),304,597,.8f);
 if(C->Signal)Text(C->Signal<0?TEXT("<<"):TEXT(">>"),W*.5,80,2,FLinearColor(.5f,.9f,.3f));
}
