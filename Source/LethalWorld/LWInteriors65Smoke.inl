if(FParse::Param(FCommandLine::Get(),TEXT("LWInteriors65Smoke"))){
 struct FState65{TWeakObjectPtr<ALWChunk> Chunk;TWeakObjectPtr<ACameraActor> Camera;};auto S=MakeShared<FState65>();
 Add(TEXT("65 audit setup"),1,[this,S](ALWCharacter& P){P.NewGame();P.ClosePanels();P.bMenu=false;P.SetMenuInput(false);P.RPG.Story.Enabled=false;P.World->EnableEncounters=false;P.bGod47=true;P.SetActorTickEnabled(false);P.World->SetActorTickEnabled(false);P.GetCharacterMovement()->DisableMovement();P.World->TimeOfDay=12;P.World->TickWeather(0,&P);auto* PC=Cast<APlayerController>(P.Controller);PC->ConsoleCommand(TEXT("r.ScreenPercentage 100"));PC->GetHUD()->bShowHUD=false;PC->ConsoleCommand(TEXT("r.SetRes 1600x1000w"));PC->ConsoleCommand(TEXT("DisableAllScreenMessages"));S->Camera=GetWorld()->SpawnActor<ACameraActor>();PC->SetViewTarget(S->Camera.Get());
  for(const TCHAR* N:{TEXT("Chair65"),TEXT("Workstation65"),TEXT("Bookcase65"),TEXT("Vent65"),TEXT("Noticeboard65"),TEXT("DiningTable65")})Check(P.World->Mesh(N)&&P.World->Mesh(N)->GetPathName().Contains(TEXT("Interiors65")),TEXT("detailed interior asset is installed"));
  // Construct a floor with an open stair edge and prove corner support is enforced.
  auto* C=GetWorld()->SpawnActor<ALWChunk>();LWGen::FSite Site;Site.Position=FVector2D(500000,500000);Site.Id=650001;
  {LWInteriors65::FScope A(C,P.World,Site);A.Record({FBox(FVector(-200,-200,-20),FVector(200,200,0)),TEXT("floor"),true,false,false});A.Reindex();A.Finishing=true;
   Check(A.Place(TEXT("Chair65"),FVector(0,0,1),0,TEXT("chair")),TEXT("supported chair accepted"));
   Check(!A.Place(TEXT("Chair65"),FVector(190,150,1),0),TEXT("stair edge overhang rejected"));
   Check(!A.Place(TEXT("Chair65"),FVector(0,0,1),0),TEXT("overlapping furniture rejected"));
  }C->Destroy();
 });
 for(int T=0;T<LWPlaces::Count;T++){
  Add(FString::Printf(TEXT("65 POI %d"),T),1.2,[this,S,T](ALWCharacter& P){
   if(S->Chunk.IsValid())S->Chunk->Destroy();auto* C=GetWorld()->SpawnActor<ALWChunk>();S->Chunk=C;LWGen::FSite Site;Site.Type=T;Site.Position=FVector2D(200000,200000);Site.Id=650100+T;Site.Size=LWPlaces::Size(T);Site.Floors=12;Site.AccessibleFloors=12;Site.Yaw=0;
   LWInteriors65::Reports.Empty();const double Start=FPlatformTime::Seconds();C->Building(P.World,Site);C->FlushSurfaces();Check(!LWInteriors65::Reports.IsEmpty(),TEXT("POI completes captured construction"));
   // Lift every independent child actor, including underground child residents.
   TFunction<void(AActor*)> Lift=[&](AActor* A){A->AddActorWorldOffset(FVector(0,0,50000));A->SetActorTickEnabled(false);if(auto* Chunk=Cast<ALWChunk>(A))for(auto R:Chunk->Residents)if(IsValid(R))Lift(R);};Lift(C);
   FCollisionQueryParams Query(NAME_None,false,&P);
   auto Tread=[&](FVector At){FHitResult Foot,Head;return GetWorld()->LineTraceSingleByChannel(Foot,At+FVector(0,0,40),At-FVector(0,0,15),ECC_Visibility,Query)&&FMath::Abs(Foot.ImpactPoint.Z-At.Z)<3&&!GetWorld()->LineTraceSingleByChannel(Head,At+FVector(0,0,20),At+FVector(0,0,196),ECC_Visibility,Query);};
   if(T==21){int Errors=0;for(int F=0;F<6;F++)for(int I=0;I<12;I++)for(int Flight=0;Flight<2;Flight++){const FVector At(200000+(Flight?4660:3975),202000+(Flight?-36-(I+.5f)*32:-420+(I+.5f)*32),50036+F*420+(I+1)*17.5f+(Flight?210:0));if(!Tread(At))Errors++;}Check(Errors==0,TEXT("all 144 casino stair treads retain support and headroom"));}
   if(T==32){int Errors=0;for(int F=0;F<2;F++)for(int I=0;I<32;I++)if(!Tread(FVector(193100,200000-3500+I*45,50028+F*520+(I+1)*520.f/32)))Errors++;Check(Errors==0,TEXT("all 64 airport stair treads retain support and headroom"));
    for(int F=0;F<3;F++){FHitResult H;const FVector At(207000,195200,50028+F*520);Check(GetWorld()->LineTraceSingleByChannel(H,At+FVector(0,0,30),At-FVector(0,0,20),ECC_Visibility,Query),TEXT("airport east terminal floor edge is solid"));}
   }

   int Added=0,Wall=0,Ceiling=0,Cells=0;for(const auto& R:LWInteriors65::Reports){Added+=R.Furniture;Wall+=R.WallDecor;Ceiling+=R.CeilingDecor;Cells+=R.FloorCells;}
   if(T<15||T==17||T==18||T==19||T==58||T==59||T==60||T==61||T==62)Check(Cells>0,TEXT("finished ground floor receives interior pass"));
   UE_LOG(LogTemp,Display,TEXT("INTERIOR65_AUDIT type=%d name=%s cells=%d furniture=%d wall=%d ceiling=%d total_ms=%.1f actors=%d"),T,LWPlaces::Name(T),Cells,Added,Wall,Ceiling,(FPlatformTime::Seconds()-Start)*1000,C->Residents.Num());
   FVector View(-450,-600,210);FRotator Aim(-5,35,0);if(T==17){View=FVector(-520,-1280,190);Aim=FRotator(-3,110,0);}
   if(T==21){View=FVector(-1100,-1100,200);Aim=FRotator(-3,55,0);}if(T==58){View=FVector(-900,-4900,195);Aim=FRotator(-2,65,0);}if(T==32){View=FVector(-2500,-5500,210);Aim=FRotator(-2,30,0);}if(T==13){View=FVector(-1100,-900,205);Aim=FRotator(-3,40,0);}if(LWPlaces::IsTower(T)){View=FVector(250,400,180);Aim=FRotator(-3,175,0);}if(LWDungeons::IsDungeon(T)){View=LWDungeons::Room(T,0,0)+FVector(-350,-250,210);Aim=FRotator(-3,35,0);}if(LWPlaces::Underground(T)){View=LWDungeons::Room(T,0,0)+FVector(-350,-250,-8790);Aim=FRotator(-3,35,0);}
   S->Camera->SetActorLocation(FVector(Site.Position,50000)+View);S->Camera->SetActorRotation(Aim);P.SetActorLocation(S->Camera->GetActorLocation()+FVector(0,0,1000));P.SetActorHiddenInGame(true);
  },[this,T](ALWCharacter&){if(T==4||T==7||T==13||T==17||T==20||T==21||T==22||T==32||T==58)CaptureV2(*FString::Printf(TEXT("Interior65_%02d"),T));});
  V2->Steps.Last().Ready=[](ALWCharacter&){
#if WITH_EDITOR
   return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
   return true;
#endif
  };
 }
 for(int I=0;I<9;I++)Add(FString::Printf(TEXT("65 story site %d"),I),.25,[this,I](ALWCharacter& P){auto* D=ALWStoryDirector::Ensure(&P);D->SetActorTickEnabled(false);LWInteriors65::Reports.Empty();D->BuildSite(I);Check(D->Sites.Contains(I)&&!LWInteriors65::Reports.IsEmpty(),TEXT("story site retains furnished construction and reservations"));if(auto* C=D->Sites.FindRef(I).Get()){C->Destroy();D->Sites.Remove(I);D->SiteRevisions38.Remove(I);}});
 Add(TEXT("65 cleanup"),.25,[S](ALWCharacter& P){if(S->Chunk.IsValid())S->Chunk->Destroy();if(S->Camera.IsValid())S->Camera->Destroy();P.SetActorHiddenInGame(false);});return;
}
