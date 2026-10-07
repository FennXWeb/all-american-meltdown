#include "LWGeography84.h"
#include "LWSaveGame.h"
#include "LWWorld.h"
#include "LWCampaign76Corridor.h"
#include "LWCampaignProduction77.h"
#include "LWGeneration.h"
void LWGeography84::Migrate(ULWSaveGame* S){if(!S||S->Geography84>=84)return;
 const FVector2D Old[]={ {360000,2820000},{535000,2840000},{592000,2890000},{770000,2900000},{800000,2840000},{875000,3020000},{1110000,2870000},{1200000,3000000},{1240000,3000000},{918000,3260000},{990000,3380000},{1008000,3406000},{1244200,3000000} };
 const FVector2D OldToronto(LWBorder51::North+307200,6400);
 auto Move=[&](FVector P){if(ALWWorld::IsSafePosition(P))return P;FVector2D At(P);if(FVector2D::Distance(At,OldToronto)<90000)return P+FVector(LWGen::CanadaCity68()-OldToronto,0);int Best=-1;double D=23000;for(int I=0;I<UE_ARRAY_COUNT(Old);I++){double Dist=FVector2D::Distance(At,Old[I]);if(Dist<D){D=Dist;Best=I;}}if(Best>=0&&LWCampaign76::Corridor().IsValidIndex(Best+1))P+=FVector(LWCampaign76::Corridor()[Best+1].Position-Old[Best],0);return P;};
 S->Position=Move(S->Position);if(S->HasWaypoint)S->Waypoint=FVector2D(Move(FVector(S->Waypoint,0)));for(auto& V:S->Vehicles)if(!V.Value.Stored45)V.Value.Position=Move(V.Value.Position);for(auto& C:S->WorldContainers)C.Value.Position=Move(C.Value.Position);for(auto& K:S->RPG.KnownPlaces)K.Value=Move(K.Value);S->RPG.Respawn.Position=Move(S->RPG.Respawn.Position);for(auto& Town:S->RPG.Settlements)Town.Value.Center=Move(Town.Value.Center);
 for(auto& Claim:S->RPG.Claims82){const FVector Delta=Move(Claim.Value.Center)-Claim.Value.Center;Claim.Value.Center+=Delta;for(auto& Piece:Claim.Value.Pieces)Piece.Transform.AddToTranslation(Delta);}
 if(S->RPG.Campaign76.Sailing77)S->Position=LWProduction77::FerryPose(S->RPG.Campaign76.FerrySeconds77).TransformPosition(FVector(0,-350,113));
 // Serialized mission checkpoint payloads migrate through this same entry point
 // when restored; do not discard earned progress, inventories or branch choices.
 S->Geography84=84;
}
