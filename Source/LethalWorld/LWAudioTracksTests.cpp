#include "LWAudioCatalog.h"
#include "Sound/SoundBase.h"
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWAllAudioTracksTest,"LethalWorld.Audio.AllEventTracks",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWAllAudioTracksTest::RunTest(const FString&)
{
 auto* Catalog=NewObject<ULWAudioCatalog>();
 auto* A=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/S_StepRoad.S_StepRoad"));
 auto* B=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/S_StepEarth.S_StepEarth"));
 if(!TestNotNull(TEXT("first sound fixture"),A)||!TestNotNull(TEXT("second sound fixture"),B))return false;
 for(FName Name:ULWAudioCatalog::GetDefaultSlotNames())
 {
  auto Slot=ULWAudioCatalog::GetDefaultSlot(Name);Slot.Source=A;Slot.Tracks={B};Slot.Volume=.7f;Slot.Pitch=.9f;
  Catalog->Slots.Add(Name,Slot);FLWResolvedAudioSlot Resolved;
  TestTrue(*Name.ToString(),Catalog->ResolveSlot(Name,Resolved,false));
  TestTrue(TEXT("tracks override source for every default event"),Resolved.Sound==B&&!Resolved.bUsedFallback);
  TestEqual(TEXT("music flag unchanged"),Resolved.Settings.bMusic,Slot.bMusic);
  TestEqual(TEXT("loop unchanged"),Resolved.Settings.bLoop,Slot.bLoop);
  TestTrue(TEXT("attenuation unchanged"),Resolved.Settings.Attenuation==Slot.Attenuation);
  TestEqual(TEXT("volume unchanged"),Resolved.Settings.Volume,.7f);TestEqual(TEXT("pitch unchanged"),Resolved.Settings.Pitch,.9f);
 }
 auto& Slot=Catalog->Slots.Add(TEXT("CustomTracks"),FLWAudioSlot());Slot.Description=TEXT("Custom positional effect");Slot.Source.Reset();Slot.Tracks={A,B,B};Slot.Attenuation=ELWAudioAttenuation::Spatial;
 TSet<USoundBase*> Seen;
 for(int I=0;I<80;I++){FLWResolvedAudioSlot R;TestTrue(TEXT("custom non-music playlist resolves"),Catalog->ResolveSlot(TEXT("CustomTracks"),R,false));Seen.Add(R.Sound);}
 TestEqual(TEXT("both SFX variants selected"),Seen.Num(),2);
 TArray<FString> Errors,Warnings;Catalog->ValidateCatalog(Errors,Warnings);
 TestFalse(TEXT("tracks-only non-music event validates without source or legacy asset"),Errors.ContainsByPredicate([](const FString& E){return E.StartsWith(TEXT("CustomTracks:"));}));
 TestFalse(TEXT("no obsolete music-only warning"),Warnings.ContainsByPredicate([](const FString& E){return E.Contains(TEXT("Tracks are ignored"));}));
 Slot.Tracks={TSoftObjectPtr<USoundBase>(),TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/Audio/MissingTrack.MissingTrack"))),B};
 FLWResolvedAudioSlot R;TestTrue(TEXT("empty and missing tracks skipped"),Catalog->ResolveSlot(TEXT("CustomTracks"),R,false)&&R.Sound==B);
 Slot.Tracks.Pop();Slot.Source=A;TestTrue(TEXT("invalid tracks fall back to Source"),Catalog->ResolveSlot(TEXT("CustomTracks"),R,false)&&R.Sound==A&&!R.bUsedFallback);
 Slot.Source.Reset();TestFalse(TEXT("unresolvable event fails cleanly"),Catalog->ResolveSlot(TEXT("CustomTracks"),R,false));
 auto& Legacy=Catalog->Slots.FindChecked(TEXT("StepRoad"));Legacy.Tracks=Slot.Tracks;Legacy.Source.Reset();
 TestTrue(TEXT("legacy fallback still works"),Catalog->ResolveSlot(TEXT("StepRoad"),R,true)&&R.Sound==A&&R.bUsedFallback);
 TestFalse(TEXT("disabled legacy fallback respected"),Catalog->ResolveSlot(TEXT("StepRoad"),R,false));
#if WITH_EDITOR
 const FProperty* Tracks=FLWAudioSlot::StaticStruct()->FindPropertyByName(TEXT("Tracks"));
 TestTrue(TEXT("editor exposes Tracks without a Music condition"),Tracks&&!Tracks->HasMetaData(TEXT("EditCondition")));
#endif
 return true;
}
#endif
