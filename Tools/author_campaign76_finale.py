"""Offline authored resistance and collaboration finales and conditional epilogues."""
import runpy,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
ns=runpy.run_path(str(ROOT/'Tools/author_campaign76_opening.py'));D=ns['D']
for k in D:D[k].clear()
b,c,a,scene,stage,person=[ns[k] for k in ['b','c','a','scene','stage','person']]
scene('evacuation_orders',[b('vera','They move the labor families tonight. The service exit you prepared is still clear. Open it before you touch the security office.'),b('hannah','I marked a route that keeps them out of the vehicle yard. Walk the first group to the shelter marker. The rest will follow people, not a shouted coordinate.'),b('mara','I will take anyone who cannot walk. Do not send them down a stairwell and call them evacuated.',needs=['mara_returns','!dead_mara'])],next='resistance_exit')
scene('standdown',[b('imani','We have the signed order and a place to process surrender. Their lieutenant has asked who guarantees his people will not be shot after they put their rifles down.'),b('rook','I will give that guarantee in my own name, under the ward\'s authority.',needs=['rook_cooperates','!dead_rook']),b('imani','If Rook is not speaking for us, the civilian relief roster can. It has to be more than a threat with a polite ending.')],[
 c('Transmit the verified orders and the signed surrender guarantee.','standdown_yes',needs=['evidence_authentication','guard_support'],effects=['unit_surrender','defections+=1']),
 c('Use the public evidence and an independently witnessed guarantee.','standdown_yes',needs=['evidence_public','evidence_secured'],effects=['unit_surrender','defections+=1']),
 c('They will not accept our guarantee. Fight through the security line.','',stage='resistance_line',effects=['security_fight'])])
scene('standdown_yes',[b('imani','Rifles down. Keep the lane open. They are walking out. Nobody settles a private score in this line.')],next='winter_stores')
scene('winter_plan',[b('della','Those tanks feed kitchens, not just generators. That warehouse is seed and medicine. Blow the compound open from this side and there is no clever way to unburn it.',needs=['!dead_della']),b('vera','The isolation valves can cut the hall defense feed without cutting the refrigeration. The manual bypass is slower. Either saves the stores.')],[
 c('Isolate the defense feed and preserve the winter reserves.','',stage='winter_secure',effects=['preserve_stores']),
 c('Open a demolition breach through the reserve block. The food and medicine will be lost.','winter_loss',effects=['reserves_lost','civilian_losses+=2'])])
scene('winter_loss',[b('vera','Then we evacuate the store crews first. The people who survive the battle will still have to survive the winter you just made harder.')],next='meridian_assault')
scene('assault_approach',[b('hannah','Storm is covering the service ridge. Small groups on my markers. Keep the wounded route separate.',needs=['!dead_hannah','wilderness_routes']),b('della','Freight diversion is moving. You have the maintenance approach until they notice the second truck is empty.',needs=['!dead_della','logistics']),b('imani','Use the covered entrance. Civilians to the refuge rooms. Anyone who lays down a weapon goes to the surrender point.'),b('','The approach remains possible without allied guides, but the exposed lane has no diversion.')],next='meridian_inside')
scene('keene_decision',[b('keene','You have reached the inhabited sector. The families here are not a shield I intend to die behind.'),b('keene','I implemented the lists. I signed transfers. I will not tell you I never knew. I can order my detachment off the family corridor. Tell me what happens after.')],[
 c('Surrender to civilian custody. Give the stand-down order and face an inquiry.','keene_custody',effects=['keene_captured','unit_surrender','civilian_rescues+=1']),
 c('Help evacuate the families, then surrender with your detachment.','keene_help',needs=['prisoner_roster'],effects=['keene_defects','civilian_rescues+=2']),
 c('Refuse his terms and fight his detachment.','',stage='keene_fight',effects=['keene_fights'])])
scene('keene_custody',[b('keene','I will transmit it on the open channel. You will have my voice on the order. Keep it for the inquiry.')],next='carrier_hall')
scene('keene_help',[b('keene','I know which gate leads to the school. Put someone beside me who will stop me if I turn the wrong way.')],next='carrier_hall')
scene('carrier_reveal',[
 b('richardson','You restored a canal terminal, and now you intend to dismantle the government that can supply it.',needs=['terminal_working']),
 b('richardson','You kept the household files active. You understood their value when they opened a door for you.',needs=['registration_active']),
 b('richardson','The carrier contains a functioning command office. It can leave this hall while you argue over who gets the warehouses. Stand down.'),
 b('imani','Its escorts keep the service controls covered. The hall shutters and external feed give it overlapping defenses. Break either system and the other still works.'),
 b('della','The refrigeration stays on the separate feed we preserved. I can keep the winter stores running while you cut the carrier loose.',needs=['!dead_della','reserves_secured']),
 b('','Through the armored glass: a clean desk, a national flag, a lit reading lamp. The command office travels inside the machine.')],next='carrier_battle')
scene('broken_office',[
 b('','The command platform stops. Its protected office opens to the transport hall: the desk lamp still burns beside torn wall panels. A guard drops the evacuation case and pulls Richardson toward the service suite.',pause=1.4),
 b('richardson','Close that door. Get the door closed.'),
 b('imani','The carrier is disabled. Richardson is moving with the last escort. The civilian exit stays open. Do not follow him into the refuge rooms.')],next='richardson_final')
scene('everyone_out',[
 b('vera','The school group is out. The workers are still coming through the service passage. Count them against the roster.'),
 b('mara','Richardson is dead. That does not make this patient breathe. Bring me the kit and keep the passage clear.',needs=['mara_returns','!dead_mara']),
 b('ada','Last vehicle is at the extraction marker. I am staying until the count matches.',needs=['!dead_ada','evac_route']),
 b('della','Stores are on the isolated feed. We can get the food out after the people.',needs=['!dead_della','reserves_secured']),
 b('vera','The reserve block is gone. We are taking every usable dressing and blanket from the ward.',needs=['reserves_lost'])],next='final_evacuation')
scene('resistance_epilogue',[
 b('','Richardson is dead. The joint relief council opens its accounts, the Canadian archive releases its evidence, and the preserved reserves reach the surrounding communities.',needs=['ending_open']),
 b('','The Lockkeepers control distribution from a single dispatch desk. Trains run and kitchens reopen. Every independent settlement now negotiates with the people holding the freight keys.',needs=['ending_uniforms','postwar_control=1']),
 b('','The Civic Guard keeps the roads open under an emergency mandate. The promised expiration date becomes the first real test of the new command.',needs=['ending_uniforms','postwar_control=2']),
 b('','The Freeholds keep food and local access under village councils. Some welcome travelers; others close their gates. No president orders them to open.',needs=['ending_uniforms','postwar_control=3']),
 b('','Without a durable coalition, control settles around the strongest surviving transport and security crews. Their uniforms change. The arguments about who may pass do not.',needs=['ending_uniforms','postwar_control=0']),
 b('','Richardson dies, but destroyed reserves and civilian losses define the winter. Freedom arrives with empty shelves and more names to trace. The survivors refuse to call the cost inevitable.',needs=['ending_ash']),
 b('mara','We saved people. We also lost people. I will keep both lists.',needs=['mara_returns','!dead_mara']),
 b('','Mara remains in Canada. Her letters ask after patients and friends. She has not mistaken staying safe for forgetting them.',needs=['mara_stayed','!dead_mara']),
 b('','Lena continues the audit under independent supervision. Her account stays limited to what she can prove.',needs=['!dead_lena']),
 b('','Lena\'s copies survive her. The audit names the missing witness instead of pretending the surviving paperwork can replace her.',needs=['dead_lena']),
 b('','Keene faces an inquiry with the transfer orders he signed entered into evidence.',needs=['keene_captured']),
 b('','Keene\'s evacuation order is entered alongside his earlier transfers. Helping at the end does not erase what he authorized before it.',needs=['keene_defects']),
 b('','Keene died before he could answer for the transfers. The surviving records identify his part without inventing a final confession.',needs=['dead_keene']),
 b('','Bellwether remains a warm room with a repaired gate and a departure book people still bother to keep.',needs=['bell_condition=1']),
 b('','Bellwether rebuilds a room at a time. The boarded clinic window is replaced before anyone restores the sign.',needs=['bell_condition=2']),
 b('','The Bellwether families retain their name in the market refuge. They decide together whether the abandoned plaza is worth reclaiming.',needs=['bell_condition=3'])])
stage('resistance_evacuate','longwayout','labor','The Long Way Out','Get the labor families out before the scheduled transfer.','vera hannah mara',arrival='evacuation_orders')
stage('resistance_exit','longwayout','labor','The Long Way Out','Open the service passage and lead the first group to shelter.','vera hannah mara',[
 a('labor_gate','Unlock the labor settlement service gate',[-320,180,0],mesh='Keys52',work=4,effects=['labor_exit_open']),a('shelter_count','Count the first families at the sheltered exit',[320,-250,0],next='standdown',mesh='MedicalCase52',work=6,needs=['labor_exit_open'],effects=['labor_rescued','civilian_rescues+=3'])])
stage('standdown','standdown','guard','An Order to Stand Down','Offer the security unit a credible surrender guarantee.','imani rook',arrival='standdown')
stage('resistance_line','standdown','guard','An Order to Stand Down','Defeat the unit holding the approach.','imani',[a('security_clear','Open the secured approach',[0,260,0],next='winter_stores',mesh='Keys52',needs=['enemies_clear'],effects=['security_cleared'])],enemies=5)
stage('winter_stores','winterstores','labor','Winter Stores','Choose an approach that accounts for the food and medicine.','della vera',arrival='winter_plan')
stage('winter_secure','winterstores','labor','Winter Stores','Separate the defense feed from the cold stores.','della vera',[a('feed_isolate','Isolate the transport-hall defense feed',[-300,170,0],mesh='ControlPanel52',work=5,effects=['defense_feed_isolated']),a('coldstore_test','Test the independent cold-store supply',[300,170,0],next='meridian_assault',mesh='RelayConsole52',work=4,needs=['defense_feed_isolated'],effects=['reserves_secured'])])
stage('meridian_assault','lastpresident','meridian','The Last President','Approach Meridian through the severe weather.','imani hannah della',arrival='assault_approach')
stage('meridian_inside','lastpresident','meridian','The Inhabited Sector','Separate the defenders from the families seeking shelter.','vera keene imani',[a('family_route','Open the family corridor',[-300,180,0],mesh='Keys52',work=4,effects=['family_corridor']),a('talk_keene','Confront Marshal Keene',[200,180,0],scene='keene_decision',mesh='',needs=['!dead_keene']),a('keene_orders','Read the dead marshal\'s last routing order',[200,180,0],next='carrier_hall',needs=['dead_keene'],effects=['keene_resolved'])])
stage('keene_fight','lastpresident','meridian','The Marshal\'s Detachment','Fight the detachment while keeping the family corridor open.','vera imani',[a('detachment_clear','Secure the transport-hall access',[0,230,0],next='carrier_hall',mesh='Keys52',needs=['enemies_clear'],effects=['dead_keene','keene_resolved'])],enemies=5)
stage('carrier_hall','lastpresident','meridian','The Executive Carrier','Enter the transport-hall approach.','richardson imani della',arrival='carrier_reveal')
stage('carrier_battle','lastpresident','meridian','The Executive Carrier','Disable the defense feed and shutters under escort fire.','imani della',[
 a('carrier_feed','Disconnect the external carrier defense feed',[-700,200,0],mesh='ControlPanel52',work=5,effects=['carrier_feed_cut'],needs=['!carrier_feed_cut']),
 a('carrier_shutters','Drop the transport shutters across the carrier route',[700,200,0],mesh='RelayConsole52',work=5,effects=['carrier_trapped'],needs=['!carrier_trapped']),
 a('carrier_breach','Open the disabled command platform',[0,480,0],scene='broken_office',mesh='Keys52',needs=['carrier_feed_cut','carrier_trapped','enemies_clear'],effects=['carrier_disabled'])],enemies=6)
stage('richardson_final','lastpresident','meridian','The Broken Office','Defeat the last escort and kill Richardson.','richardson imani',[
 a('final_clear','Return to the civilian evacuation',[0,320,0],scene='everyone_out',mesh='MedicalCase52',needs=['dead_richardson','enemies_clear'],effects=['richardson_dead'])],enemies=3)
stage('final_evacuation','everyoneout','meridian','Everyone Out','Finish the evacuation count and secure surviving supplies.','vera imani ada mara della',[
 a('evac_count','Account for the families at the exit',[-350,-200,0],mesh='Dossier52',work=5,effects=['evac_counted']),a('final_supplies','Dispatch the final medical and supply transport',[350,-200,0],next='ending_resistance',mesh='MedicalCase52',work=5,needs=['evac_counted'],effects=['evac_complete'])])
stage('ending_resistance','ending','meridian','The Country We Left Behind','Live with what the victory preserved and what it cost.','mara vera imani',arrival='resistance_epilogue')

scene('old_friends',[b('keene','The first safehouse is one you used on the northern journey. Your detachment has the perimeter. You identify the room and the meeting route.',needs=['!dead_keene']),b('','The familiar departure board has new chalk marks. Someone has reserved the room where travelers used to dry their coats.'),b('richardson','Your knowledge makes this operation possible. Do not waste it pretending these people are strangers.')],next='loyal_safehouse')
scene('meeting_arrest',[b('hannah','You asked for a meeting. There are soldiers on both roads.',needs=['!dead_hannah']),b('della','I counted the transport vehicles. More seats than your delegation needs.',needs=['!dead_della']),b('ada','If this is an arrest, say arrest. We have all had enough trouble caused by polite words.',needs=['!dead_ada'])],[
 c('Order the arrests. These are people who trusted the invitation.','arrest_confirmed',effects=['leaders_arrested','betrayal_meeting','civilian_harm+=2','support-=1']),
 c('Warn them and stand down the detachment. Defect; the earlier victims remain harmed.','arrest_defect',stage='coalition_table',effects=['defected','resistance','loyalist=0','suspicion+=4','trust_hannah=-2'])])
scene('arrest_confirmed',[b('della','The terminal ran because people trusted a schedule. You have just taught them what your appointments are worth.',needs=['!dead_della']),b('ada','I will remember the exit you stood nearest. You always leave yourself one.',needs=['!dead_ada'])],next='ally_example')
scene('arrest_defect',[b('hannah','We will use the open road. Do not call that forgiving you. There are people missing because of what you already did.',needs=['!dead_hannah'])])
scene('example_order',[b('richardson','A leader has refused incorporation in front of the detainees. If that refusal succeeds, every order becomes a negotiation. Carry out the sentence. You know the person. That is why it must be you.'),b('','This is an execution order. Carrying it out kills the selected former ally permanently. Refusing it means open defection.')],[c('Speak to the condemned person before deciding.','example_selected'),c('Refuse the execution and defect.','example_refuse',stage='coalition_table',effects=['defected','resistance','loyalist=0','suspicion+=4'])])
scene('example_selected',[
 b('mara','You held the light while I stopped Lena bleeding. I trusted your hands because I had seen what they could do. I am looking at those same hands now.',needs=['condemned=1','!dead_mara']),
 b('mara','I came back to take patients out. You brought them another man who can decide whether they get a room. Do not tell me this is keeping me safe.',needs=['condemned=1','!dead_mara']),
 b('della','The crane lock. Remember? Nobody put a hand under the load until you set it. We built a rule because a person was worth the delay.',needs=['condemned=2','!dead_della']),
 b('della','You can run every dispatch desk in this country and still be the person who ignored that rule when the load was mine.',needs=['condemned=2','!dead_della']),
 b('hannah','At the winter table, we counted mouths and sacks. I thought you understood neither number was an excuse to stop seeing a face.',needs=['condemned=3','!dead_hannah']),
 b('hannah','Look at mine. I will not give you the comfort of shouting a threat you can use afterward.',needs=['condemned=3','!dead_hannah'],pause=1.5),
 b('ada','You heard what happened to the boat I left. I gave you the ugly version because I wanted one person not to make a legend out of it.',needs=['condemned=4','!dead_ada']),
 b('ada','This time there is no fire, no tide, no choice forced by the engine. There is just you with time to stop.',needs=['condemned=4','!dead_ada']),
 b('rook','I obeyed orders I should have questioned. You know that because you made me read my signature aloud. Do not pretend this one arrives without a name attached.',needs=['condemned=5','!dead_rook']),
 b('ivo','You brought my wife\'s keys to Samir. I thanked you. I will not take that thanks back to make this simpler for you.',needs=['condemned=6','!dead_ivo']),
 b('tessa','I wrote your return in the book. There was room beside your name. There is still room to write what you chose today.',needs=['condemned=7','!dead_tessa']),
 b('vera','I opened the service route because you said people would get out. You knew which classroom their children were in. That is what makes this betrayal, not a misunderstanding.',needs=['condemned=8','!dead_vera'])],[
 c('Carry out the execution. The death is real and permanent.','execution_after',effects=['execute_ally','ally_executed','civilian_harm+=2','loyalist']),
 c('Lower the weapon, release the prisoner, and openly defect.','example_refuse',stage='coalition_table',effects=['defected','resistance','loyalist=0','suspicion+=5'])])
scene('example_refuse',[b('','The sentence is refused. The living prisoner can leave. Other prisoners, earlier arrests, and deaths remain part of what the player must answer for.')])
scene('execution_after',[b('','The shot ends the conversation. The detachment records the sentence as carried out. The dead ally does not return when allegiance changes.',pause=2),b('richardson','The incorporation of Bellwether is next. Submit the household count with the confiscation report.')],next='loyal_bellwether')
scene('incorporation',[b('tessa','That is the same intake desk they tried to burn. Now you want us to write the names ourselves.',needs=['!dead_tessa']),b('ivo','The heater is not payment for owning the room. You were here when we shared what we had.',needs=['!dead_ivo']),b('','Weapons, work classifications, and family addresses are entered on the incorporation sheet. The transfer list includes people the player met before the first attack.')],next='loyal_border')
scene('border_mirror',[
 b('richardson','The remaining opposition is gathering on the American side of the crossing. Prevent the witnesses and records from leaving. Canada is not your objective.'),
 b('rook','The transport lane stays open. Defensive positions only. Get civilians behind the sheds.',needs=['!dead_rook','rook_cooperates']),
 b('della','Use the loading doors. I have put the freight carts where their vehicles cannot pass.',needs=['!dead_della']),
 b('hannah','Small groups through the trees. Keep away from the road lights.',needs=['!dead_hannah']),
 b('ada','Decoy lamp to the west. Actual passengers to the last landing.',needs=['!dead_ada']),
 b('mara','I am with the stretcher group. We are taking civilians across. If you still know my voice, let them go.',needs=['!dead_mara','!mara_stayed']),
 b('','Across the boundary, the lights remain steady. The player is now blocking the route they once fought to reach.')],next='loyal_final')
scene('loyal_outcome',[b('richardson','The regional opposition is broken. Your command now controls its former routes. There is a secure residence, a paid staff, and an allocation that will not disappear when winter comes.')],[
 c('Remain Richardson\'s regional enforcer.','loyal_rewards',stage='ending_loyal',effects=['enforcer']),
 c('Use the independent detachment and reserve control to move against Richardson.','takeover',stage='loyal_takeover',needs=['leverage>=3','loyal_personnel','reserve_control'],effects=['takeover']),
 c('Stand down the remaining blockade and defect even now.','late_defection',stage='coalition_table',effects=['loyalist=0','defected','resistance','support=-2'])])
scene('loyal_rewards',[b('','Orders are processed under the regional office\'s new seal. The player receives real authority, security, and income. Independent communities lose the right to refuse that office.',needs=['support']),b('','Having surrendered independent personnel and reserves, the player receives a reassignment order with no command attached. The escorts who arrive answer only to the executive office.',needs=['!support','!leverage'])])
scene('takeover',[b('','The detachment accepts the reserve codes the player secured earlier. Meridian\'s local administration loses access to its own supply authorization. The final move still requires taking Richardson\'s guarded office by force.')])
scene('late_defection',[b('','The road opens to survivors. They use it without saluting. No one comes back from the grave, and the people arrested at the meeting do not suddenly trust its host.')])
scene('loyal_epilogue',[b('','The player rules as First Citizen under Richardson\'s administration. Food reaches registered households. Unregistered families discover how far those same roads can keep them from help.',needs=['ending_citizen']),b('','Useful Until Dawn: the commission ends in a locked administrative room. There is no detachment that owes its position to the player, no independent stock to negotiate with, and no ally willing to intervene.',needs=['ending_discarded']),b('','The Inheritance: Richardson is dead, and the player holds his personnel files, reserve keys, and enforcement routes. Removing him has not undone the arrests or the executed ally. The machinery of control has a new signature.',needs=['ending_inheritance']),b('','Canada remains safe. Fewer people reach it. The family-tracing office keeps the unanswered inquiries open.'),b('','Mara remains in Canada and refuses further official contact. Her work turns toward the people displaced by the player\'s administration.',needs=['mara_stayed','!dead_mara']),b('','Bellwether is administered through a household register and compulsory transfers. Its old departure book survives in a private collection of names.')])
stage('loyal_hunt','oldfriends','freehold','Old Friends','Identify the former safehouse and its meeting routes.','keene',arrival='old_friends')
stage('loyal_safehouse','oldfriends','freehold','Old Friends','Search the known refuge and send the leadership invitation.','keene',[a('safehouse_records','Identify the occupied refuge rooms',[-300,170,0],effects=['safehouse_identified']),a('meeting_invitation','Send the trusted invitation',[300,170,0],next='loyal_meeting',mesh='RelayConsole52',work=5,needs=['safehouse_identified'],effects=['meeting_bait'])])
stage('loyal_meeting','comehome','rome','Come Home','Meet the leaders who answered the invitation.','della hannah ada',arrival='meeting_arrest')
stage('ally_example','example','meridian','A Necessary Example','Confront the condemned former ally.','richardson mara della hannah ada rook ivo tessa vera',arrival='example_order')
stage('loyal_bellwether','hearthhome','bellwether','Hearth and Home','Enforce the confiscations and compulsory household register.','ivo tessa',[a('confiscate','Confiscate the community arms',[-300,170,0],mesh='Crate',work=4,effects=['bell_confiscated']),a('register_bell','Sign the forced-transfer register',[300,170,0],scene='incorporation',needs=['bell_confiscated'],effects=['bell_incorporated','civilian_harm+=2'])])
stage('loyal_border','countryyours','crossing','The Country Is Yours','Command the blockade on the American side.','richardson rook della hannah ada mara',arrival='border_mirror')
stage('loyal_final','countryyours','crossing','The Last American Landing','Defeat the resistance defense and seize its evacuation records.','rook della hannah ada mara',[a('blockade_records','Seize the evacuation register',[0,260,0],scene='loyal_outcome',needs=['enemies_clear'],effects=['coalition_defeated'])],enemies=7)
stage('loyal_takeover','inheritance','meridian','The Inheritance','Use the prepared takeover and defeat Richardson\'s guard.','richardson',[a('take_control','Assume control of the executive office',[0,260,0],next='ending_loyal',mesh='Keys52',needs=['dead_richardson','enemies_clear'],effects=['richardson_dead'])],enemies=4)
stage('ending_loyal','ending','meridian','The Country Is Yours','Live with the order you helped impose.','richardson',arrival='loyal_epilogue')
(ROOT/'Data/Campaign76/05_Finale_Epilogues.json').write_text(json.dumps(D,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
