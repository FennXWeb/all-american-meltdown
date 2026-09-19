// Included after the existing ascending-stair assertion, while its two-level fixture is alive.
Add(TEXT("stacked destination requires stair traversal"),18,[S](ALWCharacter& P){
 auto* N=S->Crew.Get();N->SetActorLocation(FVector(21000,20000,5091));
 N->GetCharacterMovement()->StopMovementImmediately();
 N->ResetCompanionNavigation();N->ActivitySpots={FVector(21000,20000,5491)};N->ActivityTime=60;
},[this,S](ALWCharacter& P){
 UE_LOG(LogTemp,Display,TEXT("NAV30 stacked final=%s target=(21000,20000,5491)"),*S->Crew->GetActorLocation().ToString());
 Check(S->Crew->GetActorLocation().Z>5420&&FVector::Dist2D(S->Crew->GetActorLocation(),FVector(21000,20000,5491))<150,
  TEXT("same XY on lower floor is not arrival; companion finds stairs to upper floor"));
});
Add(TEXT("descend stairs"),15,[S](ALWCharacter& P){
 S->Crew->ActivitySpots={FVector(19500,20000,5091)};S->Crew->ActivityTime=60;S->Crew->ResetCompanionNavigation();
},[this,S](ALWCharacter& P){
 const FVector At=S->Crew->GetActorLocation();
 UE_LOG(LogTemp,Display,TEXT("NAV30 descent final=%s"),*At.ToString());
 Check(At.X<19700&&FMath::Abs(At.Z-5091)<50,TEXT("companion descends staircase to lower landing"));
});
Add(TEXT("formation point over void falls back up stairs"),18,[S](ALWCharacter& P){
 auto* N=S->Crew.Get();FLWCrewRecord Crew;Crew.Id=N->ResidentId;Crew.Name=TEXT("Ada");Crew.Bedroom=0;Crew.Following=true;P.RPG.Crew.Add(Crew);
 LWV2Teleport(P,FVector(21000,20000,5491));
 // 5 m sideways lies outside the upper landing, with its lower floor too far below to project.
 N->FollowTarget=FVector(21000,20500,5491);N->FollowTimer=60;N->ResetCompanionNavigation();
},[this,S](ALWCharacter& P){
 UE_LOG(LogTemp,Display,TEXT("NAV30 fallback final=%s player=%s"),*S->Crew->GetActorLocation().ToString(),*P.GetActorLocation().ToString());
 Check(S->Crew->GetActorLocation().Z>5420&&FVector::Dist2D(S->Crew->GetActorLocation(),P.GetActorLocation())<200,
  TEXT("fallback search survives multiple frames and climbs to player landing"));
 P.RPG.Crew.Empty();S->Crew->ActivityTime=60;S->Crew->ActivitySpots={FVector(21000,20000,5491)};
});
