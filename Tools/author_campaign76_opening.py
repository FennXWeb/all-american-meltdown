"""Authoring source for the first chronological campaign section (offline).
Outputs editable narrative JSON; campaign76.py validates/compiles that JSON.
No API calls, asset generation, or runtime dependencies.
"""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
D=dict(cast=[],scenes=[],stages=[])
def person(id,name,female=False):D['cast'].append(dict(id=id,name=name,female=female,voice='F04' if female else 'M04'))
def b(who,text,**kw):return dict(who=who,text=text,**kw)
def c(text,next='',stage='',needs=(),effects=()):return dict(text=text,next=next,stage=stage,needs=list(needs),effects=list(effects))
def scene(id,beats,choices=(),next=''):D['scenes'].append(dict(id=id,beats=beats,choices=list(choices),next=next))
def a(id,label,at,scene='',next='',mesh='Dossier52',needs=(),effects=(),work=0):return dict(id=id,label=label,at=at,scene=scene,next=next,mesh=mesh,needs=list(needs),effects=list(effects),work=work)
def stage(id,mission,site,title,goal,cast,actions=(),**kw):D['stages'].append(dict(id=id,mission=mission,site=site,title=title,goal=goal,cast=cast.split(),actions=list(actions),**kw))
for args in [('mara','Mara Finch',True),('lena','Lena Ortiz',True),('ivo','Ivo Bell'),('tessa','Tessa Rowan',True),('nadia','Nadia Sayegh',True),('samir','Samir Bell'),('keene','Marshal Adrian Keene'),('mercer','June Mercer',True),('elena','Elena Paredes',True)]:person(*args)

scene('bell_intro',[
 b('mara','Morning. Before you ask: no, the blanket over the kettle is not a repair. Ivo says the heater has opinions.',gesture='wry',pause=.6),
 b('ivo','It has a cracked contact. Opinions would be cheaper. Breaker is off; seat the contact, then prime it. No sparks near the fuel.'),
 b('mara','Three sealed cans left. The heater, clinic sterilizer, and aerial generator all drink from that pile. The allocation board shows what each can buys.'),
 b('tessa','And put the heat where people actually sleep. Last night the pipes were warm and my shoes froze.',move=[-210,160,0]),
 b('mara','We have known worse mornings. We have also had enough of them. Help me get this room through one more.')],
 [c('I will repair it.','',effects=['met_bellwether','journal:Bellwether has three cans of fuel. Heat, sterilization, and communications compete for them.'])])
scene('fuel',[
 b('ivo','Two cans keep the heater running through the night. One gives us a warm room now, but cold beds later.'),
 b('mara','One for the clinic lets me sterilize instruments. Two also powers the suction unit. Neither makes us a hospital.'),
 b('ivo','One for the aerial lets Tessa reach our road watch. No aerial, no early warning. We can survive any split, but call the cost what it is.')],
 [c('Two heat, one clinic. Keep the sleeping room warm.','fuel_heat',effects=['heat=2','clinic=1','comms=0','fuel_assigned','trust_ivo+=1']),
 c('Two clinic, one communications. Prioritize treatment and warning.','fuel_clinic',effects=['heat=0','clinic=2','comms=1','fuel_assigned','trust_mara+=1']),
 c('One each. Share the shortage.','fuel_shared',effects=['heat=1','clinic=1','comms=1','fuel_assigned','trust_tessa+=1'])])
scene('fuel_heat',[b('ivo','I will bank it for tonight. Tessa will have to watch the road herself.'),b('tessa','I heard. Save me a place by the vent when I come back.')])
scene('fuel_clinic',[b('mara','Thank you. I will move the patients together and bring the blankets. I will not pretend the cold is harmless.'),b('ivo','I can warm the room briefly while we test the contact. After that, we shut it down.')])
scene('fuel_shared',[b('mara','A little margin everywhere. I can work with that.'),b('tessa','I will take the aerial shift. Someone tell my father that turning it louder does not make the signal travel further.')])
scene('warm_room',[
 b('ivo','There. Hear the fan? That is the bearing I could not find in three counties.',cue='Click',pause=1.6,gesture='point'),
 b('tessa','My gloves are steaming. That cannot be good for them.'),
 b('mara','Give them here. Slowly. You put numb hands over that vent and you will burn them before you feel it.',move=[-170,110,0],pause=1),
 b('ivo','We will have this all night.',needs=['heat>=2']),
 b('mara','Then move Mrs. Vale closer before she insists she is not cold.',needs=['heat>=2']),
 b('ivo','Only the test run. Clinic gets the cans after this.',needs=['heat=0']),
 b('mara','I know. Leave the blankets here. Nobody owes us a brave face.',needs=['heat=0']),
 b('tessa','I will sit here until the aerial warms up. Five minutes.',needs=['heat=1']),
 b('mara','Five minutes, then I need someone to help with the road gate. ...I used to complain about radiators I could not turn down.',pause=1.4),
 b('ivo','You still can. I will take it as a professional compliment.')],next='bell_prepare')
scene('ivo_break',[
 b('ivo','My son Samir is at the market clinic. We argue by courier. He says I cannot repair everyone. I say he picked a poor profession to make that argument.'),
 b('ivo','If you get north before I do, there is a parcel under my bench. His mother\'s keys. There is no house to open. That is not the point.')],
 [c('I will carry them.','ivo_parcel',effects=['parcel_accepted','journal:Ivo asked me to deliver his wife\'s keys to Samir at the market clinic.']),c('I cannot promise that yet.')])
scene('ivo_parcel',[b('ivo','Thank you. Tell him I put the blue tag back on. He will know.')])
scene('tessa_break',[
 b('tessa','The road-watch notebook has two columns: arrivals and departures. They stopped matching last winter.'),
 b('tessa','I still write both. You can decide it is pointless after you have looked for someone who never got written down.')])
scene('prepare_go',[
 b('mara','Before we settle in: brace the side gate, check the aerial, or put the clinic kit within reach. You choose how much time to spend. Come back when you are ready.'),
 b('tessa','There is someone on the west approach. A cart. They are waving a shirt.')],
 [c('Let them in.','',stage='bell_arrival'),c('I need to finish preparing.')])
scene('lena_arrival',[
 b('tessa','Three at the gate. Two walking. One on the cart. Nobody behind them that I can see.',gesture='point'),
 b('mara','You with the cart, take that corner. Easy. Easy. What is your name?',move=[80,-100,0],pause=.7),
 b('lena','Lena Ortiz. Do not throw the lining away. My coat. Please.'),
 b('mara','I am trying to keep the rest of you. Tessa, scissors.'),
 b('lena','They looked through the bags. Left the tins. They kept asking who had the transfer sheets. I copied them at work. I did not know what else to do.'),
 b('mara','We will talk after I stop the bleeding. You can help at this medical case. Or cover the gate. I cannot do both.'),
 b('ivo','Two vehicles stopped short of the forecourt. Their lights are off.',pause=.8)],
 [c('Hold the plaza. I will help where I can.','',stage='bell_attack',effects=['defend_bellwether','journal:Armed strangers followed Lena. They searched luggage instead of taking food.']),
 c('Prepare an evacuation as well. Nobody is trapped here.','',stage='bell_attack',effects=['evac_plan','journal:Mara has an evacuation route behind the plaza if the defense fails.'])])
scene('lena_treat',[
 b('mara','Press here. Keep pressing. Good. The suction unit is doing the rest.',needs=['clinic>=2']),
 b('mara','Clean instruments. I can close this. Hold the light steady.',needs=['clinic=1']),
 b('mara','No power for the sterilizer. The kit we set aside will do; do not open anything else.',needs=['clinic=0','medical_ready']),
 b('mara','We are short of sterile dressings. A field medkit will cover it.',needs=['clinic=0','!medical_ready']),
 b('lena','My coat. Inside seam. Blue stamps. Those are the copies, not the originals.',pause=.6)],next='bell_attack')
scene('bell_after',[
 b('tessa','They went for my intake book. Not the till. Not the pantry. One shouted names I had only just written down.',needs=['!dead_tessa']),
 b('ivo','Gate held. I can mend the rest without losing the room.',needs=['bell_condition=1','!dead_ivo']),
 b('ivo','Clinic window is gone. I have plywood. We keep one room heated until I can close it.',needs=['bell_condition=2','!dead_ivo']),
 b('mara','We left the plaza. I counted everyone I could reach. I will not call that saving the building.',needs=['bell_condition=3','!dead_mara']),
 b('mara','Lena is alive. She needs a powered suction unit and clean tubing from the market clinic.',needs=['!dead_lena','!dead_mara']),
 b('mara','Lena died. I have written her name down. The clinic still needs equipment; the others are not out of danger.',needs=['dead_lena','!dead_mara'],pause=1.8),
 b('tessa','Lena\'s coat had transfer copies sewn inside. I kept them dry. If she cannot explain them, the stamps still name a Syracuse office.',needs=['dead_lena','!dead_tessa']),
 b('lena','I audited emergency deliveries. These sheets do not prove what happened to the people. They do prove someone kept moving their supplies.',needs=['!dead_lena']),
 b('mara','Samir works the old shopping complex clinic. Ask him about Northbank, too. I want an actual road to Canada. Not another promise.',needs=['!dead_mara']),
 b('','The survivors have marked the market clinic and the municipal relocation office. The damaged transfer copies remain readable.',needs=['dead_mara'])],next='market_arrive')

stage('bell_lights','lights','bellwether','Keep the Lights On','Repair the heater contact and allocate the three cans of fuel.','mara ivo tessa',[
 a('heater','Seat and prime the heater contact',[-210,200,0],mesh='ControlPanel52',effects=['heater_fixed'],work=4,needs=['!heater_fixed']),
 a('fuel','Read the fuel allocation board',[210,200,0],scene='fuel',effects=[],needs=['!fuel_assigned']),
 a('talk_ivo','Talk to Ivo',[-300,-60,0],scene='ivo_break',mesh=''),a('talk_tessa','Talk to Tessa',[280,-50,0],scene='tessa_break',mesh='')],arrival='bell_intro',needs=['heater_fixed','fuel_assigned'],next='bell_warm')
stage('bell_warm','lights','bellwether','Keep the Lights On','Join the others by the heater.','mara ivo tessa',arrival='warm_room')
stage('bell_prepare','door','bellwether','Someone at the Door','Prepare the plaza, then meet Mara at the gate.','mara ivo tessa',[
 a('brace_gate','Brace the side gate',[-350,-380,0],mesh='Scrap52',work=5,effects=['gate_braced'],needs=['!gate_braced']),
 a('road_watch','Tune the road-watch aerial',[370,180,0],mesh='RelayConsole52',work=4,effects=['watch_ready'],needs=['comms','!watch_ready']),
 a('stage_medical','Set aside sterile dressings',[210,100,0],mesh='MedicalCase52',work=3,effects=['medical_ready'],needs=['!medical_ready']),
 a('talk_mara','Meet Mara',[80,-160,0],scene='prepare_go',mesh=''),a('talk_ivo','Talk to Ivo',[-300,-60,0],scene='ivo_break',mesh='',needs=['!parcel_accepted']),a('talk_tessa','Talk to Tessa',[280,-50,0],scene='tessa_break',mesh='')])
stage('bell_arrival','door','bellwether','Someone at the Door','Help bring the injured travelers inside.','mara ivo tessa lena',arrival='lena_arrival')
stage('bell_attack','door','bellwether','Someone at the Door','Defend the plaza or evacuate. Treat Lena and protect the intake records.','mara ivo tessa lena',[
 a('treat_lena','Help Mara stabilize Lena',[100,80,0],mesh='MedicalCase52',work=5,effects=['lena_treated'],needs=['!lena_treated','!dead_lena']),
 a('save_intake','Carry the intake book to safety',[-170,80,0],effects=['intake_saved'],needs=['!intake_saved','!intake_burned']),
 a('isolate_power','Isolate the damaged clinic circuit',[290,220,0],mesh='ControlPanel52',work=3,effects=['fire_contained'],needs=['!fire_contained']),
 a('evacuate','Lead the evacuation behind the plaza',[-350,420,0],next='bell_after',mesh='Keys52',effects=['bell_condition=3','evacuated'])],enemies=4,duration=150,next='bell_after')
stage('bell_after','door','bellwether','Someone at the Door','Check on the survivors.','mara ivo tessa lena',arrival='bell_after')

scene('market_intro',[
 b('samir','Bellwether? Father wrote that the heater ate another contact. ...You are bleeding. Sit down.'),
 b('mara','An attack. They were looking for papers. We need powered suction and proper tubing.',needs=['!dead_mara']),
 b('samir','The service annex has a portable unit. Nadia kept it running when the lower corridor flooded. She needs it for her brother, too. Ask her. Do not just carry it off.'),
 b('lena','I can tell you which cabinet the service manual used to live in. That does not mean the cabinet is still there.',needs=['!dead_lena']),
 b('samir','And yes, Northbank is real. Their receiving station is north, around Watertown. They take people, not just people with useful résumés. Getting to them is the hard part.')],next='market_claim')
scene('parcel',[
 b('samir','The blue tag. He kept it.'),b('samir','She used to label everything except her own keys. He said the tag was ugly. It was. ...I will write him. Thank you.',pause=1.8),
 b('samir','There are two clean field dressings in that drawer. Take them. That is me thanking you, not the clinic selling you anything.')])
scene('nadia_claim',[
 b('nadia','Stop there. That pump is connected to my brother. We do not have another. If you are looking for a convenient abandoned thing, this is the wrong room.',gesture='stop'),
 b('nadia','There is a second unit under the service platform. Drain the water, isolate the dead feeder, and I can test it. You cannot power the drain and the socket at the same time.'),
 b('samir','Or we share this one and make a treatment schedule. It means someone waits, and someone has to carry it back.')],
 [c('I will help recover the second unit. Nobody loses treatment.','nadia_work',stage='market_drain',effects=['market_cooperation','trust_nadia+=2']),
 c('Let us share it. I will return the pump after treatment.','nadia_share',stage='market_return',effects=['pump_loan','equipment_ready','trust_nadia+=1']),
 c('Offer 60 credits toward her replacement, with Samir taking over care.','nadia_buy',stage='market_resolve',needs=['credits>=60'],effects=['spend:60','market_paid','equipment_ready']),
 c('Threaten to take it by force. Her patients will lose it.','nadia_warning')])
scene('nadia_work',[b('nadia','Drain valve first. Disconnect the feeder before you touch the platform. I will meet you at the test socket.'),b('nadia','I am not asking you to be a hero. I am asking you not to elect somebody else expendable.')])
scene('nadia_share',[b('nadia','Samir gets the case number. I get your word and a return time. If it does not come back, I will tell every clinic north of here.'),b('samir','I will bring the patient here for this treatment cycle. Carry the case to the clinic chair, then return it here. Nobody needs to walk to Bellwether with a tube in their chest.')])
scene('nadia_buy',[b('nadia','That buys tubing and a battery. Samir has agreed to keep my brother until I have the replacement. You are paying for a workable alternative. Not for him to stop mattering.')])
scene('nadia_warning',[b('nadia','If you pull a weapon, the corridor watch will fight you. Say what you mean. Say you are taking it from a person.')],
 [c('Take the pump. I understand this means violence.','',stage='market_force',effects=['market_violence','trust_nadia=-3','civilian_harm+=1']),c('Back down. We can work something out.','nadia_claim')])
scene('pump_test',[b('nadia','Pressure holds. Leave that cracked hose; use mine. Here, put your hand over the outlet. Feel that? Your people get a pump. My brother keeps his.'),b('samir','This is what I mean when I say the market is a town. It is not the shops. It is whether this happens.')],next='market_resolve')
scene('market_aftermath',[
 b('samir','Treatment is underway. I cannot promise everyone recovers. I can promise we are no longer improvising suction with a kettle.'),
 b('nadia','You brought it back. Next time, just say who sent you.',needs=['pump_returned']),
 b('samir','The corridor watch will bury their own. You got equipment. That does not make what happened in there necessary.',needs=['market_violence']),
 b('mara','Thank you for staying with the practical part. I needed something I could actually put my hands on.',needs=['!dead_mara','!market_violence']),
 b('samir','Directorate table at the old municipal office has sealed antibiotics and referral forms. Keene is here in person. Their medicine has worked. The questions on their forms are another matter.')],next='aid_arrive')
stage('market_arrive','customer','market','Customer Service','Find Samir at the market clinic entrance.','samir mara lena',[
 a('talk_samir','Give Samir the parcel',[0,0,0],scene='parcel',mesh='',needs=['parcel_accepted','!parcel_delivered'],effects=['parcel_delivered','trust_samir+=2','give:medkit:2'])],arrival='market_intro')
stage('market_claim','customer','market','Customer Service','Talk to Nadia about the service-area pump.','samir nadia mara lena',[
 a('talk_nadia','Speak with Nadia',[-160,180,0],scene='nadia_claim',mesh=''),a('talk_samir','Give Samir the parcel',[200,-60,0],scene='parcel',mesh='',needs=['parcel_accepted','!parcel_delivered'],effects=['parcel_delivered','trust_samir+=2','give:medkit:2'])])
stage('market_drain','customer','market','Customer Service','Drain the service annex before opening the feeder.','nadia samir',[
 a('drain','Open the service drain',[-320,100,0],mesh='Scrap52',work=6,effects=['annex_drained'],needs=['!annex_drained']),
 a('feeder','Isolate the flooded feeder',[220,220,0],mesh='ControlPanel52',work=4,effects=['feeder_isolated'],needs=['annex_drained','!feeder_isolated']),
 a('recover_pump','Recover the second pump',[-210,210,0],mesh='MedicalCase52',work=4,effects=['pump_recovered'],needs=['feeder_isolated','!pump_recovered']),
 a('test_pump','Reconnect the isolated test socket',[200,100,0],scene='pump_test',mesh='ControlPanel52',work=4,effects=['equipment_ready'],needs=['pump_recovered'])])
stage('market_return','customer','market','Customer Service','Complete the treatment cycle, then return Nadia\'s pump.','nadia samir mara',[
 a('treatment','Assist with the treatment cycle',[220,-100,0],mesh='MedicalCase52',work=8,effects=['treatment_done'],needs=['!treatment_done']),
 a('return_pump','Return the pump to Nadia',[-160,180,0],next='market_resolve',mesh='MedicalCase52',needs=['treatment_done'],effects=['pump_returned','trust_nadia+=1'])])
stage('market_force','customer','market','Customer Service','Take the pump through the corridor watch.','nadia samir',[
 a('seize_pump','Take the pump',[-160,180,0],next='market_resolve',mesh='MedicalCase52',needs=['enemies_clear'],effects=['equipment_ready'])],enemies=2)
stage('market_resolve','customer','market','Customer Service','Check the clinic before leaving.','samir nadia mara lena',arrival='market_aftermath')

scene('aid_intro',[
 b('keene','Marshal Adrian Keene. Restoration Directorate. Do not mind the seals: the medicine inside is current, and the clinic can test a dose before signing anything.'),
 b('mercer','The northern referral has a space for every household member, including dependents. Registration is not a guarantee of a seat.'),
 b('keene','We need an accurate survey around Bellwether: trades, relatives, chronic conditions. We cannot plan with guesses. In return, your clinic gets antibiotics and you get a named contact on the northern road.'),
 b('lena','Who receives the household sheet after you?',needs=['!dead_lena']),
 b('mercer','The transfer office. The copy stays here until the next dispatch. Names and skills can determine placement. Read that part before you agree.')],
 [c('Explain what registration means.','aid_questions'),c('Supply accurate household records.','aid_accept',stage='housing_visit',effects=['registration=1','aid_received','give:medkit:2','trust_keene+=1','journal:Keene supplied medicine for Bellwether\'s household survey. The duplicate remains at the municipal office until dispatch.']),
 c('Submit a falsified survey that omits residents and skills.','aid_false',stage='housing_visit',effects=['registration=2','aid_received','give:medkit:2','directorate_suspicion+=1']),
 c('Refuse the survey. Keep people off the register.','aid_refuse',stage='housing_visit',effects=['registration=3']),
 c('Offer to check the duplicate records before deciding.','aid_investigate',stage='aid_records',effects=['registration=4'])])
scene('aid_questions',[
 b('keene','It means a location, a head count, and somebody responsible for correcting it. Some trades are urgently needed. Families move together where possible.'),
 b('mercer','Where possible is not a promise. You can inspect your duplicate or withdraw it here before dispatch.'),
 b('keene','June is right. If you know a cleaner way to deliver medicine across three counties, bring it to me. Until then, we will use lists.')],
 [c('Return to the offer.','aid_intro')])
scene('aid_accept',[b('mercer','Sign below the household count. Keep this receipt. The red duplicate is the one to correct if anything is wrong.'),b('keene','You will find the antibiotics do more good than the argument about who sent them. I hope we can keep it that way.')])
scene('aid_false',[b('mercer','This is unusually sparse. I will mark it provisional. The transport office may ask to verify it.'),b('keene','Provisional is better than nothing. June, release the clinic pack.')])
scene('aid_refuse',[b('keene','That is your decision. The package is tied to the survey; I cannot keep making private exceptions.'),b('mercer','The market clinic can still refer patients through Northbank. Ask Samir. Humanitarian intake is not ours to deny.')])
scene('aid_investigate',[b('mercer','Duplicates are in the side office. Read only what the household authorized. If a name is missing, bring me a case number.'),b('keene','Accuracy is the point. If you find a mistake, I want it on paper.')])
scene('duplicates',[
 b('','DUPLICATE 14 / TOMAS PAREDES / water technician. Dependents: Elena Paredes, spouse; Luis Paredes, child. Destination on applicant copy: FAMILY HOUSING. Destination on dispatch carbon: PROCESSING / SEPARATE TRANSPORT.'),
 b('','The carbon is dated two days before the housing inspection was signed. A marginal note reads: retain trade classification; no forwarding address.'),
 b('mercer','That is not a typo I can fix at this desk. Elena still lives at the listed address. She came here yesterday asking where they took her husband.')],next='housing_visit')
scene('elena_intro',[
 b('elena','If you are here for the rest of the house, go away. They already measured the kitchen.'),
 b('mara','We are not taking anything. Samir sent us.',needs=['!dead_mara']),
 b('elena','Tomas repaired water pumps. They offered us housing where the pipes worked. The transport came for him first. They said Luis and I would follow.'),
 b('elena','He left his wedding ring because the pump casings catch it. It is not a clue he planted. It is just what he always did.',pause=1.4),
 b('elena','The driver asked me to pack our documents separately. Tomas made a rubbing of the cargo tag while they waited. It is on the kitchen table. Please find an address, not a rumor.')],next='housing_search')
scene('cargo_tag',[
 b('','A pencil rubbing: ROME FREIGHT / TRANSFER 14 / RETAIN TECHNICIAN. Beneath it, Tomas wrote: ask D. Voss at canal terminal if the housing office loses us.'),
 b('elena','Della knew him before any of this. He helped her get the old pumps turning. If she has seen that tag, she will know where it went.'),
 b('elena','Take the family photograph, if you need it. Leave the ring. I need one thing to stay where he left it.')],next='records_remedy')
scene('remedy',[
 b('mercer','I checked transfer fourteen. The transport office will not give me a forwarding address. They say the family is not authorized to ask.'),
 b('mercer','Your Bellwether duplicate is still here. I warned you about placement. I did not understand it could mean this.',needs=['registration=1']),
 b('mercer','Your provisional survey has not been verified. I can still withdraw it.',needs=['registration=2']),
 b('mercer','There is no Bellwether submission from you. There are still other families on these sheets.',needs=['registration>=3']),
 b('mercer','If you take the dispatch carbon, people can compare the two destinations. If you burn every copy, fewer names leave this room, but so does part of the trail. Decide while the courier is away.')],
 [c('Take the carbon; withdraw my survey and hide the dependent addresses.','remedy_protect',effects=['records_recovered','registration_withdrawn','evidence_transfer','trust_mercer+=2','journal:June withdrew the Bellwether survey. I retained the conflicting transfer carbon without the dependent addresses.']),
 c('Destroy the dispatch bundle so these families cannot be traced from it.','remedy_destroy',effects=['records_destroyed','registration_withdrawn','evidence_transfer=0','trust_mercer+=1','journal:The dispatch bundle was destroyed. Elena\'s rubbing remains an alternate lead to Rome.']),
 c('Keep the evidence, but leave registration active to maintain access.','remedy_keep',effects=['evidence_transfer','registration_active','leverage+=1','journal:I kept the carbon but left registration active. Listed families remain exposed to placement orders.'])])
scene('remedy_protect',[b('mercer','I will mark those files withdrawn. Do not tell Elena I solved this. Her husband is still missing.')],next='syracuse_after')
scene('remedy_destroy',[b('mercer','The stove is hot enough. I will say the pipe burst. Tomas\'s wife still has the tag; do not let anyone take that from her.')],next='syracuse_after')
scene('remedy_keep',[b('mercer','Then understand what you are leaving in their hands. An open file is not a disguise for the families written in it.')],next='syracuse_after')
scene('syracuse_after',[
 b('lena','Rome is where several emergency consignments vanished from my ledger. I can explain that part to Voss. Nothing I have says who gave the order.',needs=['!dead_lena']),
 b('mara','Lena would have known the ledger. We have her copies and a family willing to put names to one transfer. That is enough to ask a real question.',needs=['dead_lena','!dead_mara']),
 b('elena','If you find Tomas, tell him Luis still puts his shoes by the heater. He says we should be ready when his dad comes home.'),
 b('mara','I want Canada. I have not changed my mind. But I will not step over these people on the way to it.',needs=['!dead_mara']),
 b('samir','I sent word ahead to the canal crews. Bellwether will hear from us, too. You do not have to disappear to travel north.')],next='rome_arrive')
stage('aid_arrive','tomorrow','records','A Better Tomorrow','Meet the Directorate at the municipal aid desk.','keene mercer mara lena',[
 a('talk_keene','Discuss the household survey',[-150,150,0],scene='aid_intro',mesh='')],arrival='aid_intro')
stage('aid_records','tomorrow','records','A Better Tomorrow','Compare the applicant copy with the dispatch carbon.','mercer',[
 a('read_duplicates','Read transfer fourteen',[210,130,0],scene='duplicates',effects=['evidence_transfer'])])
stage('housing_visit','address','housing','No Forwarding Address','Visit Elena Paredes at the address on transfer fourteen.','elena mara lena',arrival='elena_intro')
stage('housing_search','address','housing','No Forwarding Address','Examine Tomas\'s cargo-tag rubbing.','elena mara lena',[
 a('rubbing','Examine the cargo-tag rubbing',[150,120,0],scene='cargo_tag',effects=['evidence_rubbing','journal:Tomas Paredes was sent through Rome Freight. His tag names Della Voss at the canal terminal.'])])
stage('records_remedy','address','records','No Forwarding Address','Meet June before the dispatch courier leaves.','mercer',[
 a('talk_mercer','Decide what to do with the dispatch bundle',[0,100,0],scene='remedy',mesh='')],arrival='remedy')
stage('syracuse_after','address','market','No Forwarding Address','Bring the confirmed route back to the clinic.','samir elena mara lena',arrival='syracuse_after')
stage('syracuse_complete','address','market','The Road North','The Syracuse investigation is complete. Della Voss is the next lead.','samir nadia mara lena',[
 a('talk_samir','Talk to Samir',[200,-60,0],scene='clinic_quiet',mesh='')])
scene('clinic_quiet',[b('samir','You can sit. Nobody has a form for sitting.'),b('samir','Father wrote back. Three pages about the heater bearing. Two lines about himself. I am choosing to call that good news.',needs=['parcel_delivered','!dead_ivo']),b('samir','I kept the parcel. I know he meant to bring it himself.',needs=['parcel_delivered','dead_ivo'],pause=1.2),b('samir','The clinic will be here when you need it. Bring clean water if you have some. Bring yourself even if you do not.')])
(ROOT/'Data/Campaign76/01_Bellwether_Syracuse.json').write_text(json.dumps(D,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
print(len(D['stages']),'stages;',len(D['scenes']),'scenes;',sum(len(s['beats']) for s in D['scenes']),'beats')
