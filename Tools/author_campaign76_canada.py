"""Offline authored chapters 5–7: three alternative routes and a safe Canadian stay."""
import runpy,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
ns=runpy.run_path(str(ROOT/'Tools/author_campaign76_opening.py'));D=ns['D']
for k in D:D[k].clear()
b,c,a,scene,stage,person=[ns[k] for k in ['b','c','a','scene','stage','person']]
for args in [('chen','Dr. Leila Chen',True),('ada','Ada Quinn',True),('milo','Milo Sato'),('claire','Claire Boudreau',True),('jules','Jules Mercier')]:person(*args)
scene('northbank_intake',[
 b('chen','Leila Chen, Northbank Mission. Sit wherever there is a chair. If there is not a chair, tell me and I will stop using one for boxes.'),
 b('chen','We can receive patients by protected convoy. A witness with verifiable evidence can request a confidential transfer. Neither is the only way to get north.'),
 b('ada','And if the road is watched, I know the islands. There is a boat. There is a limit to what it carries. We will discuss that before anyone brings a wardrobe.'),
 b('chen','An unofficial arrival still gets processed in Canada. A person does not stop needing help because they used the wrong boat. Our problem is the American approach.'),
 b('chen','Choose the route that suits the people with you. If it closes, come back. We do not cross your name out for trying.')],[
 c('Help reconnect the patient station and take the humanitarian convoy.','route_patients',stage='route_patients',effects=['entry_route=1']),
 c('Request a protected transfer for the witness and evidence.','route_witness',stage='route_witness',needs=['testimony'],effects=['entry_route=2']),
 c('Request a protected transfer using corroborating records.','route_witness',stage='route_witness',needs=['evidence_authentication'],effects=['entry_route=2']),
 c('Arrange Ada\'s crossing through the islands.','route_ferry',stage='route_ferry',effects=['entry_route=3'])])
scene('route_patients',[b('chen','The station lost its line to our dispatcher. Reconnect it, check the patient roster, and secure the oxygen cart. Everyone on that roster travels, including the people who cannot walk.'),b('mara','Give me the triage list. And someone else the heavy end of the cart.',needs=['!dead_mara']),b('chen','When you leave, the rear stretcher goes first at each obstruction. Otherwise it becomes the one everybody forgets.')])
scene('route_witness',[b('chen','I need a sealed copy, an independently checkable date, and the name of the person who can explain it. Only the receiving officer sees the full witness name.'),b('simon','And the family. My agreement still includes the family.',needs=['witness_travels','!dead_simon']),b('chen','I have written them on the same manifest. You can watch me seal it.'),b('lena','My dispatch copies can verify the dates even if the original auditor is not welcome at a government desk.',needs=['!dead_lena'])])
scene('route_ferry',[b('ada','Six benches. Cargo below the tape line. Anything higher and we are arguing with the water, and the water is a poor negotiator.'),b('ada','Before you hear it from somebody angry: I left another boat behind last year. Engine fire. I got my passengers out. I did not go back for theirs.'),b('milo','My sister was on it. Do not make that a story about how hard Ada felt.'),b('ada','It is his story too. Ask. Then decide whether you trust me with this crossing.')],[
 c('Ask Milo what happened.','ada_history'),c('Reserve space for stranded passengers; leave surplus cargo.','ferry_rescue',effects=['ferry_rescue_space','trust_ada+=1']),c('Keep the cargo and take only the listed group.','ferry_cargo',effects=['ferry_cargo','winter_food+=1'])])
scene('ada_history',[b('milo','She came back the next morning with a tow line. I know. I was still on the bank. My sister was not.'),b('ada','I do not get a cleaner version. This time I have a second line, spare fuel, and someone watching the stern. You can check all three.')],[c('Check the rescue equipment and leave room for passengers.','ferry_rescue',effects=['ferry_rescue_space','ada_past_known','trust_milo+=2']),c('Keep the manifest limited to the agreed group.','ferry_cargo',effects=['ferry_cargo','ada_past_known'])])
scene('ferry_rescue',[b('ada','The crates stay. People get the benches. Milo, you check the spare line. You do not have to trust my hands with it.')])
scene('ferry_cargo',[b('milo','Then if we find someone out there, you explain the empty space above the crates.'),b('ada','We will know the limit before we cast off. That is at least more honest than inventing it in the dark.')])
scene('lastmile_patients',[b('chen','The lead truck is blocked. Two patients are still behind it. Move the obstruction and get the rear group under cover; our receiving team can see the signal light.'),b('mara','I have the front stretcher. Go to the one you cannot see from here.',needs=['!dead_mara']),b('','Across the crossing, streetlights remain steady. The shooting is behind the American staging sheds.')],next='crossing_patients')
scene('lastmile_witness',[b('imani','That is not a customs vehicle. They came through the side gate without stopping.'),b('simon','They used my old call sign. Someone told them which transfer this was.',needs=['witness_travels','!dead_simon']),b('chen','Keep the sealed copy apart from the witness. Either can make it across and bring attention to the other. Do not let them take both.')],next='crossing_witness')
scene('lastmile_ferry',[b('ada','Lines off. No—hold. People on the last landing.'),b('milo','I can reach them with the spare line if someone keeps that searchlight off us.'),b('ada','The far shore is ready. We choose whether to make another approach here. Not whether Canada will let them be people.')],next='crossing_ferry')
scene('border_arrival',[
 b('claire','Hello. I am Claire. Before the questions: is anyone having trouble breathing?',pause=1),
 b('mara','Only when I stop to notice it.',needs=['!dead_mara']),
 b('claire','All right. We will take it slowly. Your weapons are in sealed storage. This is the receipt. Keep it; they are yours to collect if you go back.'),
 b('claire','There is hot water through that door. It takes a moment to run warm. You do not have to fill every bottle before you use it.',move=[160,100,0],pause=1.6),
 b('mara','You have the lights on in the empty room.',needs=['!dead_mara'],pause=1.2),
 b('claire','Someone will use it later. There is a clean bed for each person on your group record. We can talk about tomorrow after you have slept.'),
 b('','The processing questions stop. Claire leaves the wash station and rest bays open. There is no alarm and no order to leave.')],next='canada_settle')
scene('tuesday',[
 b('jules','Could you hold this shelf while I tighten it? The books keep arriving faster than the brackets.'),
 b('mara','People still return library books?',needs=['!dead_mara']),
 b('jules','Late, mostly. But yes. We waived the fines for arrivals. It seemed a strange hill to die on.'),
 b('mara','I used to think I wanted a whole day without anyone needing me. Now I have one and I keep checking the door.',needs=['!dead_mara'],pause=1),
 b('jules','You can help for ten minutes and leave. We have enough people for someone to take a break.'),
 b('chen','Your prescription is ready, Mara. No emergency queue. Just the pharmacy desk.',needs=['!dead_mara'])],next='canada_routine')
scene('tracing',[
 b('chen','We matched Tomas\'s case with Elena\'s inquiry. They can speak over the mission line.',needs=['tomas_released','!dead_tomas']),
 b('tomas','Luis? ...Yes. By the heater is a good place for them. Keep the shoes there until I come.',needs=['tomas_released','!dead_tomas'],pause=1.7),
 b('chen','Tomas\'s intake is confirmed, but we cannot locate him after the annex. I will not tell Elena that no news is good news.',needs=['!tomas_released']),
 b('ruth','The ring in that freight sack is ours. Simon took it off before they searched him. I thought he had sold it to get home.',needs=['witness_family','!dead_ruth']),
 b('milo','They found my sister\'s name in the shoreline recovery ledger. I asked them to check the spelling. It was right.',needs=['ada_past_known'],pause=1.6),
 b('chen','Some cases are open. Some have an answer nobody wanted. Each keeps a person\'s name. Nothing gets reduced to the number of passengers a boat carried.')],next='canada_investigate')
scene('world_records',[
 b('chen','These are independent broadcast archives, intercepted warning records, and relief reports. None alone tells the whole story. Start with the dates.'),
 b('','SEPTEMBER 2028: foreign target reports record attempted American launches that failed to reach their targets. Independently timed imagery records subsequent attacks on American cities. The sequence is corroborated; individual decisions abroad remain incompletely documented.'),
 b('','A dated ceasefire channel required Richardson\'s surrender. The acknowledgement tape rejects those terms and orders the preservation of continuity personnel. Separate command testimony records officers preventing another outward escalation.'),
 b('simon','That is the acknowledgement I heard. Not a man on television claiming it later. The reply on our circuit that night.',needs=['testimony','!dead_simon','!witness_surrendered']),
 b('lena','The emergency-stock transfers started before the attacks. I can match the requisition signatures to the continuity compounds. They chose who would have supplies.',needs=['!dead_lena']),
 b('chen','A recent Meridian medical requisition bears the same live authentication chain. A defector identifies Richardson in a current secure attendance record. With the earlier evidence, we can say he survived. We cannot say he controlled every event that followed.'),
 b('mara','He let a country burn rather than surrender himself. And now he has lists of who gets a room in the part he kept.',needs=['!dead_mara'],pause=1.3)],next='canada_decide')
scene('canada_choice',[
 b('mara','I am safe here. I need you to hear that before you ask what comes next. I can go a day without deciding who gets the last sterile needle.',needs=['!dead_mara']),
 b('chen','You may remain. You may return. Neither decision changes your right to a bed here. We cannot send an army south with you.'),
 b('mara','If you go back, give me an honest job and an honest way out. Do not ask me to call every sacrifice worthwhile.',needs=['!dead_mara'])],[
 c('Stay in Canada. Build a life on this shore.','other_shore',stage='ending_other',effects=['remain_canada','ending_other','journal:I chose to remain in Canada. Safety did not require another war.']),
 c('Return for the missing people. Ask Mara to stay safe here.','return_mara_stays',stage='homecoming',effects=['return_chosen','mara_stayed','evidence_secured','humanitarian_permit']),
 c('Return and ask Mara to help with medical evacuation, with a guaranteed way back.','return_mara_joins',stage='homecoming',needs=['!dead_mara','trust_mara>=1'],effects=['return_chosen','mara_returns','evidence_secured','humanitarian_permit']),
 c('Return to investigate Richardson\'s offer of power.','return_ambition',stage='homecoming',effects=['return_chosen','mara_stayed','ambition','evidence_secured','humanitarian_permit'])])
scene('return_mara_stays',[b('mara','Thank you for letting that be a complete answer. Write if you can. I will still answer.',needs=['!dead_mara']),b('chen','Your crossing record stays open. Collect your stored equipment at the American checkpoint. The mission will keep a copy of the evidence here.')])
scene('return_mara_joins',[b('mara','Medical evacuation. Not revenge dressed as medicine. If you make me choose between your campaign and patients, you already know my answer.'),b('chen','I will keep her return space and the evidence copy. Do not use either as a promise you can casually spend.')])
scene('return_ambition',[b('mara','Then name it ambition. Do not borrow the people we lost to make it sound better.',needs=['!dead_mara']),b('chen','The archive keeps its independent copy. Whatever you choose south of here cannot make the record disappear.')])
scene('other_shore',[b('jules','There is a vacancy at the workshop. Trial shift on Monday. It is all right if you have forgotten what a Monday feels like.'),b('mara','I put a plant by the window. Something that needs water and is allowed to be a small problem.',needs=['!dead_mara'],pause=1.5),b('chen','News still comes from the south. Bellwether needs roof repairs.',needs=['bell_condition=1']),b('chen','The people who left Bellwether have found another room together. They sent a list of names.',needs=['bell_condition=3']),b('','You keep the names. You work. Sometimes you answer a letter. Canada remains safe. The world you left continues beyond your choice to stop fighting.')])
stage('watertown_arrive','manifest','watertown','A Place on the Manifest','Discuss a crossing route with Northbank and Ada.','chen ada mara simon lena',arrival='northbank_intake')
stage('route_patients','waitingroom','watertown','The Waiting Room','Reconnect dispatch and prepare the patient convoy.','chen mara',[
 a('dispatch_line','Reconnect the receiving station line',[-310,190,0],mesh='RelayConsole52',work=6,effects=['dispatch_ready'],needs=['!dispatch_ready']),
 a('patient_roster','Check every patient against the transport roster',[290,190,0],work=4,effects=['patients_counted'],needs=['dispatch_ready','!patients_counted']),
 a('oxygen_cart','Secure the oxygen cart and leave with the convoy',[230,-220,0],next='crossing_arrive',mesh='MedicalCase52',work=5,needs=['patients_counted'],effects=['patient_convoy_ready'])])
stage('route_witness','materialwitness','watertown','Material Witness','Seal the evidence and protect the transfer identity.','chen simon ruth lena',[
 a('witness_seal','Seal the corroborated evidence packet',[-280,190,0],work=4,effects=['sealed_evidence'],needs=['!sealed_evidence']),
 a('witness_manifest','Separate the public manifest from the confidential witness record',[270,190,0],next='crossing_arrive',work=4,effects=['witness_confidential'],needs=['sealed_evidence'])])
stage('route_ferry','nightcrossing','ferry','Night Crossing','Check Ada\'s rescue equipment and passenger arrangements.','ada milo mara',[
 a('talk_ada','Decide the ferry load',[0,170,0],scene='route_ferry',mesh=''),
 a('ferry_lines','Check the spare line and fuel',[310,160,0],mesh='Scrap52',work=5,effects=['ferry_repaired'],needs=['!ferry_repaired']),
 a('ferry_leave','Join the ferry staging group',[0,-300,0],next='crossing_arrive',mesh='Keys52',needs=['ferry_repaired'],effects=['ferry_ready'])])
stage('crossing_arrive','lastmile','crossing','All Those Lights','Regroup at the American staging area.','chen ada imani mara simon milo',[
 a('route_patients_begin','Help the stranded convoy',[-300,160,0],scene='lastmile_patients',mesh='MedicalCase52',needs=['entry_route=1']),
 a('route_witness_begin','Protect the confidential transfer',[0,160,0],scene='lastmile_witness',needs=['entry_route=2']),
 a('route_ferry_begin','Reach the last landing',[300,160,0],scene='lastmile_ferry',mesh='Keys52',needs=['entry_route=3'])])
for kind,count in [('patients',4),('witness',4),('ferry',3)]:
 stage('crossing_'+kind,'lastmile','crossing','The Last Mile',{'patients':'Rescue the rear patients and clear the blocked approach.','witness':'Recover the intercepted evidence and protect the witness group.','ferry':'Secure the landing and recover stranded passengers.'}[kind],'chen ada imani mara simon milo',[
  a('rescue_'+kind,{'patients':'Move the rear patients under cover','witness':'Recover the separate evidence case','ferry':'Bring the stranded passengers to the line'}[kind],[-360,260,0],mesh='MedicalCase52',work=6,effects=['crossing_rescued','civilian_rescues+=2'],needs=['!crossing_rescued']),
  a('clear_'+kind,{'patients':'Clear the convoy obstruction','witness':'Secure the witness escape gate','ferry':'Release the last mooring'}[kind],[320,260,0],mesh='Scrap52',work=4,effects=['crossing_open'],needs=['!crossing_open']),
  a('cross_'+kind,'Enter humanitarian processing',[0,450,0],next='canada_arrival',mesh='Keys52',needs=['crossing_open'],effects=['humanitarian_permit','travel_canada']),
  a('retreat_'+kind,'Fall back and choose another route',[0,-500,0],next='watertown_arrive',mesh='Dossier52',effects=['crossing_retreat'])],enemies=count)
stage('canada_arrival','arrival','canada','All Those Lights','Complete processing and take a clean bed.','claire chen mara lena',arrival='border_arrival')
stage('canada_settle','arrival','canada','A Room With a Door','Wash at the sink. Rest in the right-hand bay, or continue when you are ready.','claire mara',[
 a('canada_wash','Use the wash station',[-950,240,0],mesh='Sink65',work=4,effects=['washed'],needs=['!washed']),
 a('canada_sleep','Sleep for four hours in your clean bed',[760,600,0],mesh='Bed65',next='canada_tuesday',needs=['washed'],effects=['canada_rested','journal:Northbank gave us a safe room. I slept before meeting the community workshop.']),
 a('canada_continue','Continue to the workshop without sleeping',[0,-650,0],mesh='Keys52',next='canada_tuesday',needs=['washed'],effects=['journal:Northbank gave us a safe room. I chose to visit the workshop before sleeping.'])])
stage('canada_tuesday','tuesday','canada','A Normal Tuesday','Spend time in the community workshop.','jules chen mara',arrival='tuesday')
stage('canada_routine','tuesday','canada','Things People Still Do','Help with an ordinary repair, then collect the clinic prescription.','jules chen mara',[
 a('library_shelf','Hold and secure the workshop shelf',[-300,200,0],mesh='Scrap52',work=5,effects=['ordinary_work'],needs=['!ordinary_work']),
 a('prescription','Collect the prepared prescription',[280,200,0],next='canada_tracing',mesh='MedicalCase52',needs=['ordinary_work'],effects=['canada_rest','give:medkit:2'])])
stage('canada_tracing','names','canada','Names, Not Numbers','Meet the family-tracing team.','chen tomas ruth milo mara',arrival='tracing')
stage('canada_investigate','worldsaw','canada','What the World Saw','Compare the independent records with your evidence.','chen simon lena mara',[a('archive_timeline','Read the corroborated September timeline',[0,190,0],scene='world_records',effects=['richardson_revealed','evidence_secured'])])
stage('canada_decide','choice','canada','What We Owe','Decide whether to remain safe or return south.','chen mara',[a('talk_chen','Discuss what comes next',[0,170,0],scene='canada_choice',mesh='')])
stage('ending_other','ending','canada','The Other Shore','You have chosen a life in Canada.','jules chen mara',arrival='other_shore')
(ROOT/'Data/Campaign76/03_Crossing_Canada.json').write_text(json.dumps(D,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
