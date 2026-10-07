"""Offline authored chapters 8–10. The offer is explicit; no ambiguous atrocity commitment."""
import runpy,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
ns=runpy.run_path(str(ROOT/'Tools/author_campaign76_opening.py'));D=ns['D']
for k in D:D[k].clear()
b,c,a,scene,stage,person=[ns[k] for k in ['b','c','a','scene','stage','person']]
person('richardson','President Ronald Richardson');person('vera','Vera Cho',True)
scene('homecoming',[
 b('ivo','You came back. Door still catches, but the room is warm.',needs=['bell_condition=1','!dead_ivo']),
 b('tessa','We closed the clinic with plywood. I left your name in the departure book. Now I get to cross it out.',needs=['bell_condition=2','!dead_tessa']),
 b('tessa','We came back for the things we could carry. Nobody sleeps here now. We use the market rooms.',needs=['bell_condition=3','!dead_tessa']),
 b('mara','Canada is still there. I need everyone here to know that. We did not dream it.',needs=['mara_returns','!dead_mara']),
 b('ivo','The Directorate came asking for skilled households. Their list already had ours on it.',needs=['registration_active','!dead_ivo']),
 b('tessa','They brought a list with the names blanked out. Somebody saved us a difficult conversation.',needs=['registration_withdrawn','!dead_tessa']),
 b('','At the intake desk, the empty places retain the names of the people who died here.'),
 b('lena','The latest labor orders point into the Adirondack supply corridor. We can identify the missing workers by the belongings we catalogued.',needs=['!dead_lena'])],next='proof_life')
scene('labor_names',[
 b('tomas','Elena kept the ring? ...Then she believed I was coming back.',needs=['!dead_tomas','!tomas_released']),
 b('vera','I teach in the resort school. These are the parents of children upstairs. The children were told their parents volunteered for a separate work district.'),
 b('vera','I believed it until one of them asked why volunteers needed a gate code to visit. Her father works this shift.'),
 b('vera','The roster has names, sleeping blocks, and the next transfer time. Take a copy. The service gate can be opened from the shift desk. Do not sound the general alarm with families in the yard.')],[
 c('Open the service gate and escort the workers out.','labor_rescue',effects=['labor_rescued','tomas_released','civilian_rescues+=3','trust_vera+=2']),
 c('Secure the roster quietly and prepare a later evacuation.','labor_wait',effects=['prisoner_roster','insider_access','leverage+=1'])])
scene('labor_rescue',[b('vera','I will count them against the classroom register. That tells us which children we still have to find.'),b('tomas','Give me the back end of that stretcher. I have been carried far enough.',needs=['!dead_tomas'])],next='voice_carries')
scene('labor_wait',[b('vera','I can delay the transfer once. After that, I have to tell them I was lying. Remember there are people inside the time you are buying.')],next='voice_carries')
scene('evidence_decision',[
 b('della','A public copy can stop crews obeying the next transfer order. It also tells Meridian exactly what we know.',needs=['!dead_della']),
 b('imani','Private distribution gives the guard something to check before it acts. Silence gives the Directorate another unchallenged briefing.'),
 b('ada','You can sell access, too. Say that out loud if that is the plan. The buyers will know who paid for your silence.',needs=['!dead_ada'])],[
 c('Publish the corroborated evidence with protected witness identities.','evidence_publish',effects=['evidence_public','defections=2','meridian_alert=2','coalition+=1']),
 c('Share verified copies privately with the surviving allies.','evidence_private',effects=['evidence_private','defections=1','meridian_alert=1']),
 c('Withhold the American copies and preserve surprise.','evidence_withhold',effects=['evidence_withheld','meridian_alert=0']),
 c('Sell exclusive access to the Directorate. The Canadian archive remains independent.','evidence_sell',effects=['evidence_sold','directorate_access','trust_mara-=4','trust_hannah-=3','credits_reward:300'])])
scene('evidence_publish',[b('imani','I will include the source dates and what we cannot confirm. People deserve something better than a new certainty shouted from the old loudspeaker.')],next='meridian_access')
scene('evidence_private',[b('imani','Separate copies. Separate couriers. If one is caught, the others still arrive.')],next='meridian_access')
scene('evidence_withhold',[b('ada','Then I will stop promising the people south of here an explanation. Surprise has a price; they are paying part of it.')],next='meridian_access')
scene('evidence_sell',[b('ada','You got paid. That part worked. Do not expect the families on those sheets to call it a negotiation.')],next='meridian_access')
scene('access',[
 b('vera','Meridian has guest lists, labor lists, and maintenance rosters. Being on one is not the same as being welcome everywhere.'),
 b('vera','If you come as a guest, they will search you and escort you. If you use my maintenance route, the service corridor is your way out as well as in. A prisoner exchange happens at the guarded reception gate.')],[
 c('Use the Directorate invitation and submit to the escorted reception.','access_guest',needs=['directorate_access'],effects=['meridian_entry=1']),
 c('Negotiate an exchange of the recovered transfer records for family access.','access_exchange',needs=['evidence_transfer'],effects=['meridian_entry=2']),
 c('Use Vera\'s service route under an assumed maintenance assignment.','access_insider',needs=['insider_access'],effects=['meridian_entry=3','suspicion=1']),
 c('Observe the deliveries and infiltrate through the maintenance gate.','access_infiltrate',effects=['meridian_entry=4','suspicion=2'])])
scene('access_guest',[b('vera','They know you are coming. Expect courtesy, guards, and no room with an unobserved exit.')],next='meridian_life')
scene('access_exchange',[b('vera','I will transmit the case numbers first. They cannot pretend they do not know which families you mean.')],next='meridian_life')
scene('access_insider',[b('vera','Memorize the shift name. Do not memorize a lie about the people you meet there. Most of them do not know what is downstairs.')],next='meridian_life')
scene('access_infiltrate',[b('vera','The kitchen delivery leaves the service gate open for a trolley, not a squad. Watch its next cycle and go through before the latch resets.')],next='meridian_service')
scene('functioning_country',[
 b('vera','The school has heat. The infirmary has medicine. The vegetables came from covered beds here, not a warehouse nobody else can reach.'),
 b('vera','That is why people defend this place. It saved their children. Some cannot afford to ask what paid for it.'),
 b('','A school notice lists visiting hours for WORK DISTRICT PARENTS. A handwritten request beneath it asks who can authorize a visit outside those hours.'),
 b('vera','Come to the identity office. It is not on the guest tour.')],next='meridian_unregistered')
scene('unregistered',[
 b('','IDENTITY OFFICE: travel papers surrendered on admission. Reassignment requires command approval. Refusal of labor allocation suspends dependent housing privileges.'),
 b('vera','There is the choice. Work where you are told, or your family loses the room. They call it voluntary because the gate is not always locked.'),
 b('keene','The president has asked to speak to you. Yes. The president. You will be escorted. Whatever you think you have learned, listen before you decide what it means.',needs=['!dead_keene']),
 b('','An escorted audience order is delivered from the executive office.',needs=['dead_keene'])],next='meridian_offer')
scene('offer_open',[
 b('richardson','Please. The coffee is fresh. You have spent a great deal of time among people who cannot offer that without giving something up.',pause=1),
 b('richardson','Your arrival was expected. Keene\'s office kept me informed.',needs=['meridian_entry=1']),
 b('richardson','You negotiated your way to the door. I prefer a person who knows what their information is worth.',needs=['meridian_entry=2']),
 b('richardson','The maintenance supervisor noticed an extra worker. We will leave the question of whose idea that was for later.',needs=['meridian_entry=3']),
 b('richardson','You entered through a service gate. You are now in a guarded room because I requested a conversation instead of an arrest.',needs=['meridian_entry=4']),
 b('richardson','Bellwether. A shortage of fuel. A heater you chose to keep running. People remember the person who makes a cold room warm.',needs=['heat>=1','registration=1']),
 b('richardson','Keene sent me the clinic referral. You understand that medicine delivered today persuades more reliably than a promise about next year.',needs=['aid_received']),
 b('richardson','I do not need you because you are unique. I need you because people let you through their doors. I can offer food, secure transport, a medical allocation, and regional authority. In return, independent armed networks become part of one administration.')],[
 c('Ask about the people forbidden to leave.','offer_labor'),c('Ask about the ceasefire he rejected in 2028.','offer_ceasefire'),c('Ask what he expects you to do to your allies.','offer_cost'),c('Give your answer.','offer_answer')])
scene('offer_labor',[b('richardson','I cannot operate a water plant with everyone free to leave their shift at once.'),b('vera','Their children lose housing if they refuse the shift. They surrendered their identities. That is not a scheduling problem.'),b('richardson','It is coercion. A government uses it. The question is whether it builds something that lasts. I will not flatter you by pretending otherwise.')],[c('Return to the offer.','offer_open')])
scene('offer_ceasefire',[b('richardson','They demanded my surrender in exchange for a pause they could end whenever it suited them.'),b('richardson','I chose to preserve a national government. Millions died while I made that choice. You want an admission? There it is. You still have to decide who can keep the next million alive.'),b('vera','The personnel who stopped your next launch kept people alive without preserving your command.'),b('richardson','And now you are standing in the part they did not build.')],[c('Return to the offer.','offer_open')])
scene('offer_cost',[b('richardson','Names, routes, meeting places. Then disarmament, incorporation, and removal of leaders who organize violent refusal. Registration. Consolidation. Removal. Project Inheritance is a sequence, not a slogan.'),b('richardson','You will not be asked merely to smile beside a flag. If you accept command, you will arrest people who trusted you. Some will refuse to submit. I expect you to carry out lawful sentences.'),b('richardson','Consider it carefully. A reluctant signature is of no use to either of us.')],[c('Give your answer.','offer_answer')])
scene('offer_answer',[b('richardson','I have explained the position. Ask for time if you need it. Do not agree to a responsibility you intend to pretend was unclear.')],[
 c('Refuse. People are not his property.','offer_refuse',stage='coalition_table',effects=['resistance']),
 c('Accept a provisional commission. Review the first order before any commitment to violence.','offer_provisional',stage='terms',effects=['provisional','directorate_access','leverage+=1']),
 c('Feign acceptance to expose the transfer system, without promising an execution.','offer_deception',stage='terms',needs=['insider_access'],effects=['double_agent','directorate_access','suspicion+=1'])])
scene('offer_refuse',[b('richardson','Then leave by the guest route while it is still available to you. The escort will make sure you do. I do not intend to keep offering the same terms.')])
scene('offer_provisional',[b('richardson','Review it. I prefer useful agreement to theatrical loyalty. Keene\'s office will show you what regional authority can accomplish.')])
scene('offer_deception',[b('richardson','You are careful about promises. I respect that. I will be equally careful about what access you receive.')])
scene('coalition',[
 b('della','I can move freight and repair the route. I cannot promise a clean fight. Ask somebody who commands one.',needs=['!dead_della']),
 b('rook','I can coordinate units and issue a credible stand-down instruction. They will need proof and a place to surrender.',needs=['rook_cooperates','!dead_rook']),
 b('imani','The guard I command answers to the ward and the corridor communities. We will not trade one private chain of command for another.',needs=['imani_leads']),
 b('hannah','I can get families through the woods. Put a marching column on that route and you will lose both the cover and the families.',needs=['!dead_hannah']),
 b('ada','Extraction, decoys, the last vehicle out. Do not assign me a heroic final stand. I have spent too long learning why it is a bad plan.',needs=['!dead_ada']),
 b('mara','I take the wounded. I do not choose who deserves treatment by their uniform.',needs=['mara_returns','!dead_mara'])],[
 c('Agree to a joint relief council with public accounts and divided keys.','keys_shared',effects=['postwar_control=0','coalition+=1','support+=2']),
 c('Give the Lockkeepers central control of supply distribution.','keys_della',needs=['!dead_della'],effects=['postwar_control=1','logistics+=1','support+=1']),
 c('Give the Civic Guard emergency authority over supplies and security.','keys_guard',needs=['guard_support'],effects=['postwar_control=2','guard_support+=1','support+=1']),
 c('Put the Freeholds in charge of food and local access, without a central council.','keys_freehold',needs=['!dead_hannah'],effects=['postwar_control=3','winter_food+=1','support+=1'])])
scene('keys_shared',[b('imani','Then put the rules on paper before we have a building worth arguing over. Two signatures to release reserves, no family loses food for refusing a job.')],next='prisoner_plan')
scene('keys_della',[b('della','A ledger, a timetable, and a single dispatch desk. It will work. People will also have to ask us before they move what they need.')],next='prisoner_plan')
scene('keys_guard',[b('imani','An emergency mandate needs an end date. If the guard will not sign that date, people will have reason to fear what we built.')],next='prisoner_plan')
scene('keys_freehold',[b('hannah','Food stays with the communities that raise it. I will not pretend that makes every village generous. We will have our own arguments to answer for.')],next='prisoner_plan')
scene('terms',[
 b('keene','Your commission clears supply requisitions and commands one regional detachment. It does not give you the president\'s personal troops.',needs=['!dead_keene']),
 b('richardson','The first order is to identify the opposition\'s gathering place and deliver an incorporation demand. Review the personnel roster and reserve allocation. You are holding authority now, not merely being shown it.'),
 b('vera','If you are going to turn on this system, these are the transport codes. You can evacuate the people on the next list instead of delivering them. That will cost your disguise.')],[
 c('Audit the detachment roster and keep an independent reserve allocation.','terms_leverage',effects=['loyal_personnel','reserve_control','leverage+=2','support+=1']),
 c('Leave personnel and reserves entirely under Richardson\'s control.','terms_dependent',effects=['leverage=0','support=0']),
 c('Use the codes to evacuate the targeted households; expose the deception now.','terms_defect',stage='coalition_table',effects=['resistance','double_exposed','civilian_rescues+=2','loyalist=0','suspicion+=3'])])
scene('terms_leverage',[b('richardson','You want a command that can function without asking permission for every crate. Sensible. I will hold you responsible for it.')],next='necessary_measures')
scene('terms_dependent',[b('richardson','Then requests come through this office. Keep the chain intact and it will keep you supplied.')],next='necessary_measures')
scene('terms_defect',[b('vera','The families are moving. They will know who opened the gate, even if nobody at your old meetings trusts you yet.')])
scene('commitment',[b('richardson','The community will surrender its weapons, register every household, and accept appointed leadership. You will arrest the people who organize refusal. If you use their trust to bring them here, you will not be permitted to set them free afterward.'),b('richardson','This is the commitment. You may resign the commission before you carry it out. Afterward, do not call yourself an observer.')],[
 c('Carry out incorporation and the arrests. I understand this betrays the community.','commit_loyal',stage='loyal_hunt',effects=['loyalist','betrayal_committed','trust_mara-=8','trust_hannah-=8','civilian_harm+=2']),
 c('Resign and take the order to the resistance.','commit_resist',stage='coalition_table',effects=['resistance','loyalist=0','defected','evidence_order']),
 c('Warn the community and falsify the departure count; accept that this exposes me.','commit_double',stage='coalition_table',needs=['double_agent'],effects=['resistance','loyalist=0','double_exposed','civilian_rescues+=2','evidence_order'])])
scene('commit_loyal',[b('richardson','Then proceed. Your detachment will follow a clear order. Make certain you give one.')])
scene('commit_resist',[b('richardson','You have seen what we can preserve. Remember that when you choose what to destroy.')])
scene('commit_double',[b('vera','The departure count buys an hour. I have already told the families what that hour is for. Now go before Keene checks the gate.')])
stage('homecoming','homecoming','bellwether','The People You Left Behind','Return to Bellwether and learn what changed.','ivo tessa mara lena',arrival='homecoming')
stage('proof_life','proof','labor','Proof of Life','Identify the workers behind the processing numbers.','vera tomas lena',[a('labor_roster','Compare the worker roster with the family records',[200,180,0],scene='labor_names',effects=['prisoner_roster'])])
stage('voice_carries','broadcast','rome','A Voice That Carries','Choose how to use the corroborated evidence.','della imani ada',[a('talk_imani','Decide how the evidence travels',[0,160,0],scene='evidence_decision',mesh='')])
stage('meridian_access','access','meridian','The President Will See You Now','Choose a credible way into Meridian.','vera',[a('talk_vera','Plan the approach',[0,170,0],scene='access',mesh='')])
stage('meridian_service','access','meridian','The Service Gate','Observe a delivery cycle and pass the maintenance gate.','vera',[a('delivery_cycle','Observe and use the delivery access',[280,-220,0],next='meridian_life',mesh='Keys52',work=7,effects=['service_access'])])
stage('meridian_life','country','meridian','A Functioning Country','Walk the continuity resort and speak with Vera.','vera',arrival='functioning_country')
stage('meridian_unregistered','unregistered','meridian','The Unregistered','Read the admission terms in the identity office.','vera keene',[a('admission_terms','Read the identity retention and labor terms',[220,170,0],scene='unregistered',effects=['inheritance_evidence'])])
stage('meridian_offer','offer','meridian','The Offer','Answer Richardson\'s guarded invitation.','richardson vera keene',[a('talk_richardson','Speak to Richardson',[0,230,0],scene='offer_open',mesh='')],arrival='offer_open')
stage('coalition_table','tableenemies','rome','A Table for Enemies','Negotiate responsibilities and control of the reserves.','della rook imani hannah ada mara',arrival='coalition')
stage('prisoner_plan','gatesclose','labor','Before the Gates Close','Copy the transfer roster and prepare a civilian exit.','vera hannah',[
 a('last_roster','Identify the remaining families',[-300,170,0],effects=['prisoner_roster']),a('escape_route','Prepare the service exit',[300,170,0],mesh='Keys52',work=5,effects=['evac_route'],needs=['prisoner_roster'])],needs=['evac_route'],next='resistance_evacuate')
stage('terms','terms','meridian','Terms of Service','Review the authority and dependencies of the commission.','richardson keene vera',arrival='terms')
stage('necessary_measures','measures','meridian','Necessary Measures','Decide whether to enforce the incorporation order.','richardson',arrival='commitment')
(ROOT/'Data/Campaign76/04_Return_Meridian.json').write_text(json.dumps(D,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
