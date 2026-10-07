# The Country We Left Behind — integrated author script

Generated from the executable campaign data. Contains all branches and spoilers.

Performance status: temporary first-person staging with existing faces, gestures, movement marks, lights, props and audio cues. Unrecorded lines are subtitled. Narrative descriptions of hand actions or destruction are not bespoke animation assets.

## 01 Bellwether Syracuse

### Playable mission sequence

#### Keep the Lights On · `bell_lights`

Mission: `lights`. Location: `bellwether`.

Repair the heater contact and allocate the three cans of fuel.

Cast: Mara Finch, Ivo Bell, Tessa Rowan.

Arrival conversation: `bell_intro`.

- **Seat and prime the heater contact** (`heater`). Requires: `!heater_fixed`. Applies: `heater_fixed`. Conversation: `—`; next stage: `—`.
- **Read the fuel allocation board** (`fuel`). Requires: `!fuel_assigned`. Applies: none. Conversation: `fuel`; next stage: `—`.
- **Talk to Ivo** (`talk_ivo`). Requires: none. Applies: none. Conversation: `ivo_break`; next stage: `—`.
- **Talk to Tessa** (`talk_tessa`). Requires: none. Applies: none. Conversation: `tessa_break`; next stage: `—`.

Automatic continuation: `bell_warm` after `heater_fixed`, `fuel_assigned`.

#### Keep the Lights On · `bell_warm`

Mission: `lights`. Location: `bellwether`.

Join the others by the heater.

Cast: Mara Finch, Ivo Bell, Tessa Rowan.

Arrival conversation: `warm_room`.


#### Someone at the Door · `bell_prepare`

Mission: `door`. Location: `bellwether`.

Prepare the plaza, then meet Mara at the gate.

Cast: Mara Finch, Ivo Bell, Tessa Rowan.

- **Brace the side gate** (`brace_gate`). Requires: `!gate_braced`. Applies: `gate_braced`. Conversation: `—`; next stage: `—`.
- **Tune the road-watch aerial** (`road_watch`). Requires: `comms`, `!watch_ready`. Applies: `watch_ready`. Conversation: `—`; next stage: `—`.
- **Set aside sterile dressings** (`stage_medical`). Requires: `!medical_ready`. Applies: `medical_ready`. Conversation: `—`; next stage: `—`.
- **Meet Mara** (`talk_mara`). Requires: none. Applies: none. Conversation: `prepare_go`; next stage: `—`.
- **Talk to Ivo** (`talk_ivo`). Requires: `!parcel_accepted`. Applies: none. Conversation: `ivo_break`; next stage: `—`.
- **Talk to Tessa** (`talk_tessa`). Requires: none. Applies: none. Conversation: `tessa_break`; next stage: `—`.

#### Someone at the Door · `bell_arrival`

Mission: `door`. Location: `bellwether`.

Help bring the injured travelers inside.

Cast: Mara Finch, Ivo Bell, Tessa Rowan, Lena Ortiz.

Arrival conversation: `lena_arrival`.


#### Someone at the Door · `bell_attack`

Mission: `door`. Location: `bellwether`.

Defend the plaza or evacuate. Treat Lena and protect the intake records.

Cast: Mara Finch, Ivo Bell, Tessa Rowan, Lena Ortiz.

Authored encounter: 4 base opponents; preparation and saved defeats modify the live count.

- **Help Mara stabilize Lena** (`treat_lena`). Requires: `!lena_treated`, `!dead_lena`. Applies: `lena_treated`. Conversation: `lena_treat`; next stage: `—`.
- **Carry the intake book to safety** (`save_intake`). Requires: `!intake_saved`, `!intake_burned`. Applies: `intake_saved`. Conversation: `—`; next stage: `—`.
- **Isolate the damaged clinic circuit** (`isolate_power`). Requires: `!fire_contained`. Applies: `fire_contained`. Conversation: `—`; next stage: `—`.
- **Lead the evacuation behind the plaza** (`evacuate`). Requires: none. Applies: `bell_condition=3`, `evacuated`. Conversation: `—`; next stage: `bell_after`.

Automatic continuation: `bell_after` after none.

#### Someone at the Door · `bell_after`

Mission: `door`. Location: `bellwether`.

Check on the survivors.

Cast: Mara Finch, Ivo Bell, Tessa Rowan, Lena Ortiz.

Arrival conversation: `bell_after`.


#### Customer Service · `market_arrive`

Mission: `customer`. Location: `market`.

Find Samir at the market clinic entrance.

Cast: Samir Bell, Mara Finch, Lena Ortiz.

Arrival conversation: `market_intro`.

- **Give Samir the parcel** (`talk_samir`). Requires: `parcel_accepted`, `!parcel_delivered`. Applies: `parcel_delivered`, `trust_samir+=2`, `give:medkit:2`. Conversation: `parcel`; next stage: `—`.

#### Customer Service · `market_claim`

Mission: `customer`. Location: `market`.

Talk to Nadia about the service-area pump.

Cast: Samir Bell, Nadia Sayegh, Mara Finch, Lena Ortiz.

- **Speak with Nadia** (`talk_nadia`). Requires: none. Applies: none. Conversation: `nadia_claim`; next stage: `—`.
- **Give Samir the parcel** (`talk_samir`). Requires: `parcel_accepted`, `!parcel_delivered`. Applies: `parcel_delivered`, `trust_samir+=2`, `give:medkit:2`. Conversation: `parcel`; next stage: `—`.
- **Sit with Samir between patients** (`clinic_break`). Requires: none. Applies: none. Conversation: `clinic_quiet`; next stage: `—`.

#### Customer Service · `market_drain`

Mission: `customer`. Location: `market`.

Drain the service annex before opening the feeder.

Cast: Nadia Sayegh, Samir Bell.

- **Open the service drain** (`drain`). Requires: `!annex_drained`. Applies: `annex_drained`. Conversation: `—`; next stage: `—`.
- **Isolate the flooded feeder** (`feeder`). Requires: `annex_drained`, `!feeder_isolated`. Applies: `feeder_isolated`. Conversation: `—`; next stage: `—`.
- **Recover the second pump** (`recover_pump`). Requires: `feeder_isolated`, `!pump_recovered`. Applies: `pump_recovered`. Conversation: `—`; next stage: `—`.
- **Reconnect the isolated test socket** (`test_pump`). Requires: `pump_recovered`. Applies: `equipment_ready`. Conversation: `pump_test`; next stage: `—`.

#### Customer Service · `market_return`

Mission: `customer`. Location: `market`.

Complete the treatment cycle, then return Nadia's pump.

Cast: Nadia Sayegh, Samir Bell, Mara Finch.

- **Assist with the treatment cycle** (`treatment`). Requires: `!treatment_done`. Applies: `treatment_done`. Conversation: `—`; next stage: `—`.
- **Return the pump to Nadia** (`return_pump`). Requires: `treatment_done`. Applies: `pump_returned`, `trust_nadia+=1`. Conversation: `—`; next stage: `market_resolve`.

#### Customer Service · `market_force`

Mission: `customer`. Location: `market`.

Take the pump through the corridor watch.

Cast: Nadia Sayegh, Samir Bell.

Authored encounter: 2 base opponents; preparation and saved defeats modify the live count.

- **Take the pump** (`seize_pump`). Requires: `enemies_clear`. Applies: `equipment_ready`. Conversation: `—`; next stage: `market_resolve`.

#### Customer Service · `market_resolve`

Mission: `customer`. Location: `market`.

Check the clinic before leaving.

Cast: Samir Bell, Nadia Sayegh, Mara Finch, Lena Ortiz.

Arrival conversation: `market_aftermath`.


#### A Better Tomorrow · `aid_arrive`

Mission: `tomorrow`. Location: `records`.

Meet the Directorate at the municipal aid desk.

Cast: Marshal Adrian Keene, June Mercer, Mara Finch, Lena Ortiz.

Arrival conversation: `aid_intro`.

- **Discuss the household survey** (`talk_keene`). Requires: none. Applies: none. Conversation: `aid_intro`; next stage: `—`.

#### A Better Tomorrow · `aid_records`

Mission: `tomorrow`. Location: `records`.

Compare the applicant copy with the dispatch carbon.

Cast: June Mercer.

- **Read transfer fourteen** (`read_duplicates`). Requires: none. Applies: `evidence_transfer`. Conversation: `duplicates`; next stage: `—`.

#### No Forwarding Address · `housing_visit`

Mission: `address`. Location: `housing`.

Visit Elena Paredes at the address on transfer fourteen.

Cast: Elena Paredes, Mara Finch, Lena Ortiz.

Arrival conversation: `elena_intro`.


#### No Forwarding Address · `housing_search`

Mission: `address`. Location: `housing`.

Examine Tomas's cargo-tag rubbing.

Cast: Elena Paredes, Mara Finch, Lena Ortiz.

- **Examine the cargo-tag rubbing** (`rubbing`). Requires: none. Applies: `evidence_rubbing`, `journal:Tomas Paredes was sent through Rome Freight. His tag names Della Voss at the canal terminal.`. Conversation: `cargo_tag`; next stage: `—`.

#### No Forwarding Address · `records_remedy`

Mission: `address`. Location: `records`.

Meet June before the dispatch courier leaves.

Cast: June Mercer.

Arrival conversation: `remedy`.

- **Decide what to do with the dispatch bundle** (`talk_mercer`). Requires: none. Applies: none. Conversation: `remedy`; next stage: `—`.

#### No Forwarding Address · `syracuse_after`

Mission: `address`. Location: `market`.

Bring the confirmed route back to the clinic.

Cast: Samir Bell, Elena Paredes, Mara Finch, Lena Ortiz.

Arrival conversation: `syracuse_after`.


### Complete conversations

#### `bell_intro`

**Mara Finch:** Morning. Before you ask: no, the blanket over the kettle is not a repair. Ivo says the heater has opinions.

*Move to local mark [-40, 70, 0] cm; Existing gesture: wry; Minimum beat hold: 0.65 s.*

**Ivo Bell:** It has a cracked contact. Opinions would be cheaper. Breaker is off; seat the contact, then prime it. No sparks near the fuel.

*Existing gesture: speak.*

**Mara Finch:** Three sealed cans left. The heater, clinic sterilizer, and aerial generator all drink from that pile. The allocation board shows what each can buys.

*Existing gesture: speak.*

**Tessa Rowan:** And put the heat where people actually sleep. Last night the pipes were warm and my shoes froze.

*Move to local mark [-210, 160, 0] cm; Existing gesture: speak.*

**Mara Finch:** We have known worse mornings. We have also had enough of them. Help me get this room through one more.

*Existing gesture: speak.*

- **I will repair it.** Requires: none. Applies: `met_bellwether`, `journal:Bellwether has three cans of fuel. Heat, sterilization, and communications compete for them.`. Response: `—`; stage: `inherited continuation`.

#### `fuel`

**Ivo Bell:** Two cans keep the heater running through the night. One gives us a warm room now, but cold beds later.

*Existing gesture: speak.*

**Mara Finch:** One for the clinic lets me sterilize instruments. Two also powers the suction unit. Neither makes us a hospital.

*Existing gesture: speak.*

**Ivo Bell:** One for the aerial lets Tessa reach our road watch. No aerial, no early warning. We can survive any split, but call the cost what it is.

*Existing gesture: speak.*

- **Two heat, one clinic. Keep the sleeping room warm.** Requires: none. Applies: `heat=2`, `clinic=1`, `comms=0`, `fuel_assigned`, `trust_ivo+=1`. Response: `fuel_heat`; stage: `inherited continuation`.
- **Two clinic, one communications. Prioritize treatment and warning.** Requires: none. Applies: `heat=0`, `clinic=2`, `comms=1`, `fuel_assigned`, `trust_mara+=1`. Response: `fuel_clinic`; stage: `inherited continuation`.
- **One each. Share the shortage.** Requires: none. Applies: `heat=1`, `clinic=1`, `comms=1`, `fuel_assigned`, `trust_tessa+=1`. Response: `fuel_shared`; stage: `inherited continuation`.

#### `fuel_heat`

**Ivo Bell:** I will bank it for tonight. Tessa will have to watch the road herself.

*Existing gesture: speak.*

**Tessa Rowan:** I heard. Save me a place by the vent when I come back.

*Existing gesture: speak.*


#### `fuel_clinic`

**Mara Finch:** Thank you. I will move the patients together and bring the blankets. I will not pretend the cold is harmless.

*Existing gesture: speak.*

**Ivo Bell:** I can warm the room briefly while we test the contact. After that, we shut it down.

*Existing gesture: speak.*


#### `fuel_shared`

**Mara Finch:** A little margin everywhere. I can work with that.

*Existing gesture: speak.*

**Tessa Rowan:** I will take the aerial shift. Someone tell my father that turning it louder does not make the signal travel further.

*Existing gesture: speak.*


#### `warm_room`

**Ivo Bell:** There. Hear the fan? That is the bearing I could not find in three counties.

*Existing gesture: point; Minimum beat hold: 1.6 s; Existing audio cue: Click.*

**Tessa Rowan:** My gloves are steaming. That cannot be good for them.

*Existing gesture: speak.*

**Mara Finch:** Give them here. Slowly. You put numb hands over that vent and you will burn them before you feel it.

*Move to local mark [-60, 140, 0] cm; Existing gesture: speak; Minimum beat hold: 1 s.*

**Ivo Bell [when `heat>=2`]:** We will have this all night.

*Existing gesture: speak.*

**Mara Finch [when `heat>=2`]:** Then move Mrs. Vale closer before she insists she is not cold.

*Existing gesture: speak.*

**Ivo Bell [when `heat=0`]:** Only the test run. Clinic gets the cans after this.

*Existing gesture: speak.*

**Mara Finch [when `heat=0`]:** I know. Leave the blankets here. Nobody owes us a brave face.

*Existing gesture: speak.*

**Tessa Rowan [when `heat=1`]:** I will sit here until the aerial warms up. Five minutes.

*Existing gesture: speak.*

**Mara Finch:** Five minutes, then I need someone to help with the road gate. ...I used to complain about radiators I could not turn down.

*Existing gesture: speak; Minimum beat hold: 1.4 s.*

**Ivo Bell:** You still can. I will take it as a professional compliment.

*Existing gesture: speak.*


Continuation: `bell_prepare`.

#### `ivo_break`

**Ivo Bell:** My son Samir is at the market clinic. We argue by courier. He says I cannot repair everyone. I say he picked a poor profession to make that argument.

*Existing gesture: speak.*

**Ivo Bell:** If you get north before I do, there is a parcel under my bench. His mother's keys. There is no house to open. That is not the point.

*Existing gesture: speak.*

- **I will carry them.** Requires: none. Applies: `parcel_accepted`, `journal:Ivo asked me to deliver his wife's keys to Samir at the market clinic.`. Response: `ivo_parcel`; stage: `inherited continuation`.
- **I cannot promise that yet.** Requires: none. Applies: none. Response: `—`; stage: `inherited continuation`.

#### `ivo_parcel`

**Ivo Bell:** Thank you. Tell him I put the blue tag back on. He will know.

*Existing gesture: speak.*


#### `tessa_break`

**Tessa Rowan:** The road-watch notebook has two columns: arrivals and departures. They stopped matching last winter.

*Existing gesture: speak.*

**Tessa Rowan:** I still write both. You can decide it is pointless after you have looked for someone who never got written down.

*Existing gesture: speak.*


#### `prepare_go`

**Mara Finch:** Before we settle in: brace the side gate, check the aerial, or put the clinic kit within reach. You choose how much time to spend. Come back when you are ready.

*Existing gesture: speak.*

**Tessa Rowan:** There is someone on the west approach. A cart. They are waving a shirt.

*Existing gesture: speak.*

- **Let them in.** Requires: none. Applies: none. Response: `—`; stage: `bell_arrival`.
- **I need to finish preparing.** Requires: none. Applies: none. Response: `—`; stage: `inherited continuation`.

#### `lena_arrival`

**Tessa Rowan:** Three at the gate. Two walking. One on the cart. Nobody behind them that I can see.

*Existing gesture: point.*

**Mara Finch:** You with the cart, take that corner. Easy. Easy. What is your name?

*Move to local mark [80, -100, 0] cm; Existing gesture: point; Minimum beat hold: 0.7 s.*

**Lena Ortiz:** Lena Ortiz. Do not throw the lining away. My coat. Please.

*Existing gesture: speak.*

**Mara Finch:** I am trying to keep the rest of you. Tessa, scissors.

*Existing gesture: speak.*

**Lena Ortiz:** They looked through the bags. Left the tins. They kept asking who had the transfer sheets. I copied them at work. I did not know what else to do.

*Existing gesture: speak.*

**Mara Finch:** We will talk after I stop the bleeding. You can help at this medical case. Or cover the gate. I cannot do both.

*Existing gesture: speak.*

**Ivo Bell:** Two vehicles stopped short of the forecourt. Their lights are off.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

- **Hold the plaza. I will help where I can.** Requires: none. Applies: `defend_bellwether`, `journal:Armed strangers followed Lena. They searched luggage instead of taking food.`. Response: `—`; stage: `bell_attack`.
- **Prepare an evacuation as well. Nobody is trapped here.** Requires: none. Applies: `evac_plan`, `journal:Mara has an evacuation route behind the plaza if the defense fails.`. Response: `—`; stage: `bell_attack`.

#### `lena_treat`

**Mara Finch [when `clinic>=2`]:** Press here. Keep pressing. Good. The suction unit is doing the rest.

*Existing gesture: speak.*

**Mara Finch [when `clinic=1`]:** Clean instruments. I can close this. Hold the light steady.

*Existing gesture: speak.*

**Mara Finch [when `clinic=0`, `medical_ready`]:** No power for the sterilizer. The kit we set aside will do; do not open anything else.

*Existing gesture: speak.*

**Mara Finch [when `clinic=0`, `!medical_ready`]:** We are short of sterile dressings. A field medkit will cover it.

*Existing gesture: speak.*

**Lena Ortiz:** My coat. Inside seam. Blue stamps. Those are the copies, not the originals.

*Existing gesture: speak; Minimum beat hold: 0.6 s.*


Continuation: `bell_attack`.

#### `bell_after`

**Tessa Rowan [when `!dead_tessa`]:** They went for my intake book. Not the till. Not the pantry. One shouted names I had only just written down.

*Existing gesture: speak.*

**Ivo Bell [when `bell_condition=1`, `!dead_ivo`]:** Gate held. I can mend the rest without losing the room.

*Existing gesture: speak.*

**Ivo Bell [when `bell_condition=2`, `!dead_ivo`]:** Clinic window is gone. I have plywood. We keep one room heated until I can close it.

*Existing gesture: speak.*

**Mara Finch [when `bell_condition=3`, `!dead_mara`]:** We left the plaza. I counted everyone I could reach. I will not call that saving the building.

*Existing gesture: speak.*

**Mara Finch [when `!dead_lena`, `!dead_mara`]:** Lena is alive. She needs a powered suction unit and clean tubing from the market clinic.

*Existing gesture: speak.*

**Mara Finch [when `dead_lena`, `!dead_mara`]:** Lena died. I have written her name down. The clinic still needs equipment; the others are not out of danger.

*Existing gesture: speak; Minimum beat hold: 1.8 s.*

**Tessa Rowan [when `dead_lena`, `!dead_tessa`]:** Lena's coat had transfer copies sewn inside. I kept them dry. If she cannot explain them, the stamps still name a Syracuse office.

*Existing gesture: speak.*

**Lena Ortiz [when `!dead_lena`]:** I audited emergency deliveries. These sheets do not prove what happened to the people. They do prove someone kept moving their supplies.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** Samir works the old shopping complex clinic. Ask him about Northbank, too. I want an actual road to Canada. Not another promise.

*Existing gesture: speak.*

**Scene [when `dead_mara`]:** The survivors have marked the market clinic and the municipal relocation office. The damaged transfer copies remain readable.


Continuation: `market_arrive`.

#### `market_intro`

**Samir Bell:** Bellwether? Father wrote that the heater ate another contact. ...You are bleeding. Sit down.

*Existing gesture: point.*

**Mara Finch [when `!dead_mara`]:** An attack. They were looking for papers. We need powered suction and proper tubing.

*Existing gesture: speak.*

**Samir Bell:** The service annex has a portable unit. Nadia kept it running when the lower corridor flooded. She needs it for her brother, too. Ask her. Do not just carry it off.

*Existing gesture: speak.*

**Lena Ortiz [when `!dead_lena`]:** I can tell you which cabinet the service manual used to live in. That does not mean the cabinet is still there.

*Existing gesture: speak.*

**Samir Bell:** And yes, Northbank is real. Their receiving station is north, around Watertown. They take people, not just people with useful résumés. Getting to them is the hard part.

*Existing gesture: speak.*


Continuation: `market_claim`.

#### `parcel`

**Samir Bell:** The blue tag. He kept it.

*Existing gesture: speak.*

**Samir Bell:** She used to label everything except her own keys. He said the tag was ugly. It was. ...I will write him. Thank you.

*Existing gesture: speak; Minimum beat hold: 1.8 s.*

**Samir Bell:** There are two clean field dressings in that drawer. Take them. That is me thanking you, not the clinic selling you anything.

*Existing gesture: speak.*


#### `nadia_claim`

**Nadia Sayegh:** Stop there. That pump is connected to my brother. We do not have another. If you are looking for a convenient abandoned thing, this is the wrong room.

*Existing gesture: stop.*

**Nadia Sayegh:** There is a second unit under the service platform. Drain the water, isolate the dead feeder, and I can test it. You cannot power the drain and the socket at the same time.

*Existing gesture: speak.*

**Samir Bell:** Or we share this one and make a treatment schedule. It means someone waits, and someone has to carry it back.

*Existing gesture: speak.*

- **I will help recover the second unit. Nobody loses treatment.** Requires: none. Applies: `market_cooperation`, `trust_nadia+=2`. Response: `nadia_work`; stage: `market_drain`.
- **Let us share it. I will return the pump after treatment.** Requires: none. Applies: `pump_loan`, `equipment_ready`, `trust_nadia+=1`. Response: `nadia_share`; stage: `market_return`.
- **Offer 60 credits toward her replacement, with Samir taking over care.** Requires: `credits>=60`. Applies: `spend:60`, `market_paid`, `equipment_ready`. Response: `nadia_buy`; stage: `market_resolve`.
- **Threaten to take it by force. Her patients will lose it.** Requires: none. Applies: none. Response: `nadia_warning`; stage: `inherited continuation`.

#### `nadia_work`

**Nadia Sayegh:** Drain valve first. Disconnect the feeder before you touch the platform. I will meet you at the test socket.

*Existing gesture: speak.*

**Nadia Sayegh:** I am not asking you to be a hero. I am asking you not to elect somebody else expendable.

*Existing gesture: speak.*


#### `nadia_share`

**Nadia Sayegh:** Samir gets the case number. I get your word and a return time. If it does not come back, I will tell every clinic north of here.

*Existing gesture: speak.*

**Samir Bell:** I will bring the patient here for this treatment cycle. Carry the case to the clinic chair, then return it here. Nobody needs to walk to Bellwether with a tube in their chest.

*Existing gesture: speak.*


#### `nadia_buy`

**Nadia Sayegh:** That buys tubing and a battery. Samir has agreed to keep my brother until I have the replacement. You are paying for a workable alternative. Not for him to stop mattering.

*Existing gesture: speak.*


#### `nadia_warning`

**Nadia Sayegh:** If you pull a weapon, the corridor watch will fight you. Say what you mean. Say you are taking it from a person.

*Existing gesture: speak.*

- **Take the pump. I understand this means violence.** Requires: none. Applies: `market_violence`, `trust_nadia=-3`, `civilian_harm+=1`. Response: `—`; stage: `market_force`.
- **Back down. We can work something out.** Requires: none. Applies: none. Response: `nadia_claim`; stage: `inherited continuation`.

#### `pump_test`

**Nadia Sayegh:** Pressure holds. Leave that cracked hose; use mine. Here, put your hand over the outlet. Feel that? Your people get a pump. My brother keeps his.

*Existing gesture: point.*

**Samir Bell:** This is what I mean when I say the market is a town. It is not the shops. It is whether this happens.

*Existing gesture: speak.*


Continuation: `market_resolve`.

#### `market_aftermath`

**Samir Bell:** Treatment is underway. I cannot promise everyone recovers. I can promise we are no longer improvising suction with a kettle.

*Existing gesture: speak.*

**Nadia Sayegh [when `pump_returned`]:** You brought it back. Next time, just say who sent you.

*Existing gesture: speak.*

**Samir Bell [when `market_violence`]:** The corridor watch will bury their own. You got equipment. That does not make what happened in there necessary.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`, `!market_violence`]:** Thank you for staying with the practical part. I needed something I could actually put my hands on.

*Existing gesture: speak.*

**Samir Bell:** Directorate table at the old municipal office has sealed antibiotics and referral forms. Keene is here in person. Their medicine has worked. The questions on their forms are another matter.

*Existing gesture: speak.*


Continuation: `aid_arrive`.

#### `aid_intro`

**Marshal Adrian Keene:** Marshal Adrian Keene. Restoration Directorate. Do not mind the seals: the medicine inside is current, and the clinic can test a dose before signing anything.

*Existing gesture: speak.*

**June Mercer:** The northern referral has a space for every household member, including dependents. Registration is not a guarantee of a seat.

*Existing gesture: speak.*

**Marshal Adrian Keene:** We need an accurate survey around Bellwether: trades, relatives, chronic conditions. We cannot plan with guesses. In return, your clinic gets antibiotics and you get a named contact on the northern road.

*Existing gesture: speak.*

**Lena Ortiz [when `!dead_lena`]:** Who receives the household sheet after you?

*Existing gesture: point.*

**June Mercer:** The transfer office. The copy stays here until the next dispatch. Names and skills can determine placement. Read that part before you agree.

*Existing gesture: speak.*

- **Explain what registration means.** Requires: none. Applies: none. Response: `aid_questions`; stage: `inherited continuation`.
- **Supply accurate household records.** Requires: none. Applies: `registration=1`, `aid_received`, `give:medkit:2`, `trust_keene+=1`, `journal:Keene supplied medicine for Bellwether's household survey. The duplicate remains at the municipal office until dispatch.`. Response: `aid_accept`; stage: `housing_visit`.
- **Submit a falsified survey that omits residents and skills.** Requires: none. Applies: `registration=2`, `aid_received`, `give:medkit:2`, `directorate_suspicion+=1`. Response: `aid_false`; stage: `housing_visit`.
- **Refuse the survey. Keep people off the register.** Requires: none. Applies: `registration=3`. Response: `aid_refuse`; stage: `housing_visit`.
- **Offer to check the duplicate records before deciding.** Requires: none. Applies: `registration=4`. Response: `aid_investigate`; stage: `aid_records`.

#### `aid_questions`

**Marshal Adrian Keene:** It means a location, a head count, and somebody responsible for correcting it. Some trades are urgently needed. Families move together where possible.

*Existing gesture: speak.*

**June Mercer:** Where possible is not a promise. You can inspect your duplicate or withdraw it here before dispatch.

*Existing gesture: speak.*

**Marshal Adrian Keene:** June is right. If you know a cleaner way to deliver medicine across three counties, bring it to me. Until then, we will use lists.

*Existing gesture: speak.*

- **Return to the offer.** Requires: none. Applies: none. Response: `aid_intro`; stage: `inherited continuation`.

#### `aid_accept`

**June Mercer:** Sign below the household count. Keep this receipt. The red duplicate is the one to correct if anything is wrong.

*Existing gesture: speak.*

**Marshal Adrian Keene:** You will find the antibiotics do more good than the argument about who sent them. I hope we can keep it that way.

*Existing gesture: speak.*


#### `aid_false`

**June Mercer:** This is unusually sparse. I will mark it provisional. The transport office may ask to verify it.

*Existing gesture: speak.*

**Marshal Adrian Keene:** Provisional is better than nothing. June, release the clinic pack.

*Existing gesture: speak.*


#### `aid_refuse`

**Marshal Adrian Keene:** That is your decision. The package is tied to the survey; I cannot keep making private exceptions.

*Existing gesture: speak.*

**June Mercer:** The market clinic can still refer patients through Northbank. Ask Samir. Humanitarian intake is not ours to deny.

*Existing gesture: speak.*


#### `aid_investigate`

**June Mercer:** Duplicates are in the side office. Read only what the household authorized. If a name is missing, bring me a case number.

*Existing gesture: speak.*

**Marshal Adrian Keene:** Accuracy is the point. If you find a mistake, I want it on paper.

*Existing gesture: speak.*


#### `duplicates`

**Scene:** DUPLICATE 14 / TOMAS PAREDES / water technician. Dependents: Elena Paredes, spouse; Luis Paredes, child. Destination on applicant copy: FAMILY HOUSING. Destination on dispatch carbon: PROCESSING / SEPARATE TRANSPORT.

**Scene:** The carbon is dated two days before the housing inspection was signed. A marginal note reads: retain trade classification; no forwarding address.

**June Mercer:** That is not a typo I can fix at this desk. Elena still lives at the listed address. She came here yesterday asking where they took her husband.

*Existing gesture: speak.*


Continuation: `housing_visit`.

#### `elena_intro`

**Elena Paredes:** If you are here for the rest of the house, go away. They already measured the kitchen.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** We are not taking anything. Samir sent us.

*Existing gesture: speak.*

**Elena Paredes:** Tomas repaired water pumps. They offered us housing where the pipes worked. The transport came for him first. They said Luis and I would follow.

*Existing gesture: speak.*

**Elena Paredes:** He left his wedding ring because the pump casings catch it. It is not a clue he planted. It is just what he always did.

*Existing gesture: speak; Minimum beat hold: 1.4 s.*

**Elena Paredes:** The driver asked me to pack our documents separately. Tomas made a rubbing of the cargo tag while they waited. It is on the kitchen table. Please find an address, not a rumor.

*Existing gesture: speak.*


Continuation: `housing_search`.

#### `cargo_tag`

**Scene:** A pencil rubbing: ROME FREIGHT / TRANSFER 14 / RETAIN TECHNICIAN. Beneath it, Tomas wrote: ask D. Voss at canal terminal if the housing office loses us.

**Elena Paredes:** Della knew him before any of this. He helped her get the old pumps turning. If she has seen that tag, she will know where it went.

*Existing gesture: speak.*

**Elena Paredes:** Take the family photograph, if you need it. Leave the ring. I need one thing to stay where he left it.

*Existing gesture: speak.*


Continuation: `records_remedy`.

#### `remedy`

**June Mercer:** I checked transfer fourteen. The transport office will not give me a forwarding address. They say the family is not authorized to ask.

*Existing gesture: speak.*

**June Mercer [when `registration=1`]:** Your Bellwether duplicate is still here. I warned you about placement. I did not understand it could mean this.

*Existing gesture: speak.*

**June Mercer [when `registration=2`]:** Your provisional survey has not been verified. I can still withdraw it.

*Existing gesture: speak.*

**June Mercer [when `registration>=3`]:** There is no Bellwether submission from you. There are still other families on these sheets.

*Existing gesture: speak.*

**June Mercer:** If you take the dispatch carbon, people can compare the two destinations. If you burn every copy, fewer names leave this room, but so does part of the trail. Decide while the courier is away.

*Existing gesture: speak.*

- **Take the carbon; withdraw my survey and hide the dependent addresses.** Requires: none. Applies: `records_recovered`, `registration_withdrawn`, `evidence_transfer`, `trust_mercer+=2`, `journal:June withdrew the Bellwether survey. I retained the conflicting transfer carbon without the dependent addresses.`. Response: `remedy_protect`; stage: `inherited continuation`.
- **Destroy the dispatch bundle so these families cannot be traced from it.** Requires: none. Applies: `records_destroyed`, `registration_withdrawn`, `evidence_transfer=0`, `trust_mercer+=1`, `journal:The dispatch bundle was destroyed. Elena's rubbing remains an alternate lead to Rome.`. Response: `remedy_destroy`; stage: `inherited continuation`.
- **Keep the evidence, but leave registration active to maintain access.** Requires: none. Applies: `evidence_transfer`, `registration_active`, `leverage+=1`, `journal:I kept the carbon but left registration active. Listed families remain exposed to placement orders.`. Response: `remedy_keep`; stage: `inherited continuation`.

#### `remedy_protect`

**June Mercer:** I will mark those files withdrawn. Do not tell Elena I solved this. Her husband is still missing.

*Existing gesture: speak.*


Continuation: `syracuse_after`.

#### `remedy_destroy`

**June Mercer:** The stove is hot enough. I will say the pipe burst. Tomas's wife still has the tag; do not let anyone take that from her.

*Existing gesture: speak.*


Continuation: `syracuse_after`.

#### `remedy_keep`

**June Mercer:** Then understand what you are leaving in their hands. An open file is not a disguise for the families written in it.

*Existing gesture: speak.*


Continuation: `syracuse_after`.

#### `syracuse_after`

**Lena Ortiz [when `!dead_lena`]:** Rome is where several emergency consignments vanished from my ledger. I can explain that part to Voss. Nothing I have says who gave the order.

*Existing gesture: speak.*

**Mara Finch [when `dead_lena`, `!dead_mara`]:** Lena would have known the ledger. We have her copies and a family willing to put names to one transfer. That is enough to ask a real question.

*Existing gesture: speak.*

**Elena Paredes:** If you find Tomas, tell him Luis still puts his shoes by the heater. He says we should be ready when his dad comes home.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** I want Canada. I have not changed my mind. But I will not step over these people on the way to it.

*Existing gesture: speak.*

**Samir Bell:** I sent word ahead to the canal crews. Bellwether will hear from us, too. You do not have to disappear to travel north.

*Existing gesture: speak.*


Continuation: `rome_arrive`.

#### `clinic_quiet`

**Samir Bell:** You can sit. Nobody has a form for sitting.

*Existing gesture: speak.*

**Samir Bell [when `parcel_delivered`, `!dead_ivo`]:** Father wrote back. Three pages about the heater bearing. Two lines about himself. I am choosing to call that good news.

*Existing gesture: speak.*

**Samir Bell [when `parcel_delivered`, `dead_ivo`]:** I kept the parcel. I know he meant to bring it himself.

*Existing gesture: speak; Minimum beat hold: 1.2 s.*

**Samir Bell:** The clinic will be here when you need it. Bring clean water if you have some. Bring yourself even if you do not.

*Existing gesture: speak.*


## 02 Canal Guard Freeholds

### Playable mission sequence

#### Lock, Stock · `rome_arrive`

Mission: `lockstock`. Location: `rome`.

Meet Della Voss at the canal freight terminal.

Cast: Della Voss, Hank Corwin, Mara Finch, Lena Ortiz.

Arrival conversation: `rome_intro`.


#### Lock, Stock · `rome_crew`

Mission: `lockstock`. Location: `rome`.

Resolve the cargo lien with Hank and Della.

Cast: Della Voss, Hank Corwin, Mara Finch, Lena Ortiz.

- **Negotiate the stranded farm cargo** (`talk_hank`). Requires: none. Applies: none. Conversation: `hank_terms`; next stage: `—`.

#### Lock, Stock · `rome_repair`

Mission: `lockstock`. Location: `rome`.

Secure the crane, drain the pit, and restore the loading gate.

Cast: Della Voss, Hank Corwin.

- **Engage the crane maintenance lock** (`crane_lock`). Requires: `!crane_locked`. Applies: `crane_locked`. Conversation: `—`; next stage: `—`.
- **Drain the lift pit** (`pit_drain`). Requires: `crane_locked`, `!pit_drained`. Applies: `pit_drained`. Conversation: `—`; next stage: `—`.
- **Test the freight gate under crew supervision** (`freight_gate`). Requires: `pit_drained`. Applies: `terminal_working`. Conversation: `—`; next stage: `—`.

Automatic continuation: `rome_cargo` after `terminal_working`.

#### What the Water Carries · `rome_cargo`

Mission: `watercarries`. Location: `rome`.

Inspect the misdeclared processing shipment.

Cast: Della Voss, Hank Corwin, Lena Ortiz.

- **Open the misdeclared shipment** (`tagged_bags`). Requires: none. Applies: `belongings_catalogued`, `evidence_belongings`, `journal:Tomas Paredes and the Wellers appear on Rome processing tags. Their belongings were removed before transfer.`. Conversation: `cargo_open`; next stage: `—`.

#### What the Water Carries · `rome_control`

Mission: `watercarries`. Location: `rome`.

Resolve control of the reopened terminal.

Cast: Della Voss, Hank Corwin.

- **Decide the dispatch arrangement** (`talk_della`). Requires: none. Applies: none. Conversation: `rome_vote`; next stage: `—`.

#### Escort Duty · `guard_arrive`

Mission: `escort`. Location: `guard`.

Meet Rook and Imani at the functioning corridor hospital.

Cast: Captain Elias Rook, Sergeant Imani Bell, Mara Finch.

Arrival conversation: `guard_intro`.


#### Escort Duty · `guard_convoy`

Mission: `escort`. Location: `guard`.

Inspect the waiting transport and its patient manifest.

Cast: Captain Elias Rook, Sergeant Imani Bell, Tomas Paredes.

- **Check the restraint order** (`convoy_manifest`). Requires: none. Applies: none. Conversation: `convoy_restraints`; next stage: `—`.

#### Missing in Transit · `guard_holding`

Mission: `transit`. Location: `holding`.

Enter the transfer annex and recover its duty book.

Cast: Sergeant Imani Bell, Lena Ortiz.

- **Release the detained travelers** (`annex_release`). Requires: `!annex_released`. Applies: `annex_released`, `civilian_rescues+=2`. Conversation: `—`; next stage: `—`.
- **Copy the signed duty book** (`annex_book`). Requires: none. Applies: `evidence_authentication`. Conversation: `holding_records`; next stage: `—`.

#### Orders from Above · `guard_confront`

Mission: `orders`. Location: `guard`.

Confront Rook with the annex duty book.

Cast: Captain Elias Rook, Sergeant Imani Bell.

- **Confront Rook** (`talk_rook`). Requires: none. Applies: none. Conversation: `rook_confront`; next stage: `—`.

#### Wrong Turn · `hill_arrive`

Mission: `wrongturn`. Location: `tughill`.

Reach the Tug Hill maintenance shelter before following the distress call.

Cast: Hannah Pike, Mara Finch.

Arrival conversation: `hill_weather`.


#### Wrong Turn · `hill_beacon`

Mission: `wrongturn`. Location: `tughill`.

Watch the cut-through before approaching the repeated distress call.

Cast: Hannah Pike.

- **Seal the shelter draft and warm the traveling group** (`dry_shelter`). Requires: `!shelter_prepared`. Applies: `shelter_prepared`. Conversation: `—`; next stage: `—`.
- **Observe the distress-beacon approach** (`observe_beacon`). Requires: none. Applies: none. Conversation: `beacon_observe`; next stage: `—`.

#### Wrong Turn · `hill_fight`

Mission: `wrongturn`. Location: `tughill`.

Clear the ambush approach, then disable its transmitter.

Cast: Hannah Pike.

Authored encounter: 4 base opponents; preparation and saved defeats modify the live count.

- **Disconnect the false distress beacon** (`silence_beacon`). Requires: `enemies_clear`. Applies: `beacon_disabled`. Conversation: `—`; next stage: `freehold_arrive`.

#### A Place at the Table · `freehold_arrive`

Mission: `table`. Location: `freehold`.

Join the winter ration meeting.

Cast: Hannah Pike, Orrin Lake, Mara Finch.

Arrival conversation: `freehold_table`.


#### A Place at the Table · `freehold_refuge`

Mission: `table`. Location: `freehold`.

Make the reserved lodging habitable for the incoming families.

Cast: Hannah Pike, Orrin Lake.

- **Seal the winter-room window** (`refuge_window`). Requires: `!refuge_sealed`. Applies: `refuge_sealed`. Conversation: `—`; next stage: `—`.
- **Prepare the spare bedding** (`refuge_beds`). Requires: `refuge_sealed`. Applies: `refuge_ready`. Conversation: `—`; next stage: `witness_arrive`.

#### The Man Who Heard It · `witness_arrive`

Mission: `heardit`. Location: `witness`.

Speak to Simon and Ruth Weller at the sheltered cabin.

Cast: Simon Weller, Ruth Weller, Hannah Pike, Mara Finch.

Arrival conversation: `weller_intro`.


### Complete conversations

#### `rome_intro`

**Della Voss:** If you are here for a ride, get in line. If you are here for Tomas, come here.

*Existing gesture: speak.*

**Della Voss:** That tag is ours. We loaded the sealed consignments. I did not see the people inside until the third run. Then the Directorate stopped letting my crews near their wagons.

*Existing gesture: speak.*

**Lena Ortiz [when `!dead_lena`]:** The same dispatch number appears on my emergency ledger. You were being paid with diverted public stock.

*Existing gesture: speak.*

**Della Voss:** Paid? They brought antibiotics. My crane operator was dying. I am not proud enough to call that easy.

*Existing gesture: point.*

**Della Voss:** We need the terminal working to reach the northern station. Hank has the switch key. He wants his stranded cargo released before he puts his crew under another suspended load. Talk to him. Then lock the crane, drain the lift pit, and test the gate. In that order.

*Existing gesture: speak.*

**Scene [when `dead_della`]:** Della's maintenance instructions survive at the dispatch desk. The crews will need a new agreement; her death has not made the machinery safer.


Continuation: `rome_crew`.

#### `hank_terms`

**Hank Corwin:** The crate with the green strap belongs to three farms. Della has it under lien. They miss the planting window, we miss meals.

*Existing gesture: speak.*

**Della Voss:** They owe us two engine rebuilds. If debts vanish whenever someone needs the cargo, I cannot keep crews fed.

*Existing gesture: speak.*

- **Release the farm cargo against a written labor schedule.** Requires: none. Applies: `crew_ready`, `farm_schedule`, `trust_della+=1`, `freehold_supply+=1`. Response: `hank_shared`; stage: `inherited continuation`.
- **Pay the 80-credit lien. The cargo goes free.** Requires: `credits>=80`. Applies: `spend:80`, `crew_ready`, `freehold_supply+=1`, `trust_hank+=2`. Response: `hank_paid`; stage: `inherited continuation`.
- **Back Della: cargo remains until the debt is worked.** Requires: `!dead_della`. Applies: `crew_ready`, `toll_debt`, `trust_della+=2`, `trust_hank-=2`. Response: `hank_lien`; stage: `inherited continuation`.

#### `hank_shared`

**Hank Corwin:** I will put my name on the schedule. Put hers beside it. Nobody gets to pretend a signature only binds the poor.

*Existing gesture: speak.*

**Della Voss:** Fine. Two crews, three days. Here is the key.

*Existing gesture: speak.*


Continuation: `rome_repair`.

#### `hank_paid`

**Della Voss:** Eighty clears it. You should have asked which of those farms can pay you back.

*Existing gesture: speak.*

**Hank Corwin:** They can feed people. Some debts ought to stay that simple.

*Existing gesture: speak.*


Continuation: `rome_repair`.

#### `hank_lien`

**Hank Corwin:** Then I work. You can tell those farmers why.

*Existing gesture: speak.*

**Della Voss:** I will tell them myself. We keep a terminal or lose all three farms next winter.

*Existing gesture: speak.*


Continuation: `rome_repair`.

#### `cargo_open`

**Hank Corwin:** The lock is cut. That is clothing, not machine packing.

*Existing gesture: speak.*

**Scene:** Among the bags: a water-board identification card for Tomas Paredes, a blue child's glove labeled LUIS, and a ring engraved R + S. The bags have processing tags. They were removed from travelers.

**Della Voss:** I know two of those names. We are not selling this lot. Photograph the tags and put every bag back in its own sack.

*Existing gesture: speak.*

**Lena Ortiz [when `!dead_lena`]:** The weights in my ledger make sense now. They called the belongings surplus, so nobody had to record a person losing them.

*Existing gesture: speak.*


Continuation: `rome_control`.

#### `rome_vote`

**Della Voss:** Northbound freight can move again. The question is who signs a load out. One desk is quick. Three desks catch a mistake. Three desks can also spend a week arguing while medicine spoils.

*Existing gesture: speak.*

**Hank Corwin:** A crew seat, a farm seat, her seat. Recorded tariffs. That is what I want.

*Existing gesture: speak.*

**Della Voss:** Or hand the keys to Corwin. He has asked often enough. He would open the gates and run out of spares before the thaw.

*Existing gesture: speak.*

- **Della controls dispatch, with a public tariff ledger.** Requires: `!dead_della`. Applies: `canal_control=1`, `logistics=2`, `trust_della+=2`, `canal_prices=2`. Response: `rome_della`; stage: `inherited continuation`.
- **Negotiate shared dispatch: crews, farms, and Della each hold a key.** Requires: none. Applies: `canal_control=2`, `logistics=1`, `coalition+=1`, `canal_prices=1`. Response: `rome_shared`; stage: `inherited continuation`.
- **Give Hank and the cargo owners control. Remove the toll desk.** Requires: none. Applies: `canal_control=3`, `logistics=1`, `trust_della-=3`, `canal_prices=0`, `spares_shortage`. Response: `rome_hank`; stage: `inherited continuation`.

#### `rome_della`

**Della Voss:** You will pay the published price like everyone else. But a medical wagon never waits behind luxury cargo. Write that first.

*Existing gesture: speak.*

**Hank Corwin:** I will keep a second copy of the ledger. You can count on that.

*Existing gesture: speak.*


Continuation: `guard_arrive`.

#### `rome_shared`

**Della Voss:** Hank takes nights. Farms appoint someone who can count. If this deadlocks, all three of us answer for it.

*Existing gesture: speak.*

**Hank Corwin:** Fair. You have a seat at the table when it does, traveler. Not ownership of it.

*Existing gesture: speak.*


Continuation: `guard_arrive`.

#### `rome_hank`

**Hank Corwin:** Gate opens tonight. First run carries food, nobody pays to get their own blankets back.

*Existing gesture: speak.*

**Della Voss:** The spare parts will not arrive out of gratitude. Remember I said that when the winch stops.

*Existing gesture: speak.*


Continuation: `guard_arrive`.

#### `guard_intro`

**Captain Elias Rook:** Captain Elias Rook. The ward is through there. Keep your voice down; they have finally slept.

*Existing gesture: speak.*

**Sergeant Imani Bell:** We hold the supply corridor. It is why the clinic has oxygen. It is also why I need two more drivers than I have.

*Existing gesture: speak.*

**Captain Elias Rook:** Directorate medicines keep this hospital open. They ask us to escort their transfers. You have questions about one. Help Imani check the next convoy. Stay with the patients, and do not let a rifle settle a paperwork dispute.

*Existing gesture: speak.*


Continuation: `guard_convoy`.

#### `convoy_restraints`

**Tomas Paredes:** Do not close that door. My hands are tied to the seat.

*Existing gesture: speak.*

**Sergeant Imani Bell:** Driver, where is the medical restraint order? ...That is a cargo receipt.

*Existing gesture: point.*

**Tomas Paredes:** They said Elena and Luis were following. Nobody will tell me where they are.

*Existing gesture: speak.*

**Captain Elias Rook:** I am halting the movement. Sergeant, check every transport. Nobody leaves until we know who authorized this.

*Existing gesture: speak.*

- **Unfasten the restraint and move Tomas to the ward.** Requires: none. Applies: `tomas_released`, `trust_imani+=2`, `evidence_restraints`. Response: `convoy_release`; stage: `inherited continuation`.
- **Keep the transport intact and copy the holding-facility route.** Requires: none. Applies: `transport_traced`, `evidence_restraints`, `trust_tomas-=1`. Response: `convoy_follow`; stage: `inherited continuation`.

#### `convoy_release`

**Tomas Paredes:** My ring is at home. Tell her that if she does not believe it is me.

*Existing gesture: speak.*

**Sergeant Imani Bell:** I will book him as a patient under my own name. Whatever command thinks of that, it will have to say to me.

*Existing gesture: speak.*


Continuation: `guard_holding`.

#### `convoy_follow`

**Tomas Paredes:** You are letting them take me so you can follow? Then do not lose the road.

*Existing gesture: point.*

**Sergeant Imani Bell:** I copied the axle number. If the convoy turns off, you will know which one it is.

*Existing gesture: speak.*


Continuation: `guard_holding`.

#### `holding_records`

**Scene:** Transfer annex duty book: incoming families separated by trade category. Medical objections returned unsigned. Freight receipts use a presidential authentication series issued after September 2028.

**Lena Ortiz [when `!dead_lena`]:** This series was supposed to be retired. It proves someone can issue orders through that system. It does not prove who is alive.

*Existing gesture: speak.*

**Sergeant Imani Bell:** Rook saw the first restraint complaint six weeks ago. His initials are on the response: maintain escort pending clarification.

*Existing gesture: speak.*

**Scene [when `dead_imani`, `dead_lena`]:** The signed duty book remains evidence even without a living officer to explain it. Its authentication series can be checked against the northern archives.


Continuation: `guard_confront`.

#### `rook_confront`

**Captain Elias Rook:** I saw the complaint. I asked for clarification. They sent oxygen and another order. I kept the first and obeyed the second.

*Existing gesture: speak.*

**Sergeant Imani Bell:** You could have stopped a transport. You stopped this one when there were witnesses.

*Existing gesture: speak.*

**Captain Elias Rook:** Yes. Do not soften that for me. Tell me what you need changed now.

*Existing gesture: speak.*

**Sergeant Imani Bell [when `dead_rook`]:** Rook is dead. Half the escort section wants to go home; the others want to keep the hospital open. I will hold the ward, but I cannot promise his battalion or use his name to get it.

*Existing gesture: speak.*

**Scene [when `dead_rook`, `dead_imani`]:** Both officers are dead. The ward's duty book survives, but nobody can promise an organized escort. The remaining patients need the records made public, not another officer's name on an empty command.

- **Work with him privately: release the annex detainees and cut Directorate escorts.** Requires: `!dead_rook`. Applies: `guard_control=1`, `rook_cooperates`, `guard_support=2`, `coalition+=1`. Response: `rook_private`; stage: `inherited continuation`.
- **Publish the signed complaint. Let the ward and guard elect new command.** Requires: `!dead_rook`. Applies: `guard_control=2`, `imani_leads`, `guard_support=1`, `evidence_public`, `trust_rook-=2`. Response: `rook_public`; stage: `inherited continuation`.
- **Conceal the complaint in exchange for continued access.** Requires: `!dead_rook`. Applies: `guard_control=3`, `directorate_access`, `guard_support=0`, `leverage+=1`. Response: `rook_conceal`; stage: `inherited continuation`.
- **Release the ward records and support Imani's reduced hospital guard.** Requires: `dead_rook`, `!dead_imani`. Applies: `guard_control=4`, `imani_leads`, `guard_support=1`, `guard_fragmented`, `evidence_public`. Response: `—`; stage: `hill_arrive`.
- **Preserve the ward records. Continue north without Guard support.** Requires: `dead_rook`, `dead_imani`. Applies: `guard_control=4`, `guard_support=0`, `guard_fragmented`, `evidence_public`. Response: `—`; stage: `hill_arrive`.

#### `rook_private`

**Captain Elias Rook:** I will sign the release in front of the ward. Private does not mean invisible.

*Existing gesture: speak.*

**Sergeant Imani Bell:** I will verify the cells myself. Cooperation is useful. Forgiveness can wait.

*Existing gesture: speak.*


Continuation: `hill_arrive`.

#### `rook_public`

**Sergeant Imani Bell:** The vote is for me. I wanted a shift change, not a command. First order: no transfer without the traveler's consent recorded by someone outside the escort.

*Existing gesture: speak.*

**Captain Elias Rook:** I will turn over the route codes. You do not have to keep me to use them.

*Existing gesture: speak.*


Continuation: `hill_arrive`.

#### `rook_conceal`

**Captain Elias Rook:** Access is not absolution. I know what you are holding.

*Existing gesture: speak.*

**Sergeant Imani Bell:** The book did not vanish because you closed it. I kept the names of the people still there.

*Existing gesture: speak.*


Continuation: `hill_arrive`.

#### `hill_weather`

**Hannah Pike:** Shut the outer door behind you. Wind is coming straight through.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** We heard a distress call on the approach. Same words, every forty seconds.

*Existing gesture: speak.*

**Hannah Pike:** A recording can still mean trouble. Look before you take the cut-through. Fresh tire ruts go in. None come out. Whoever is calling has not asked once about the weather.

*Existing gesture: speak.*

**Hannah Pike:** Take the maintenance shelter first. Dry socks, a fire, someone checking the roof. Then decide which road deserves your life.

*Existing gesture: speak.*


Continuation: `hill_beacon`.

#### `beacon_observe`

**Scene:** From the maintenance overlook, the antenna cable runs to a vehicle battery behind the cut-through. Two lookouts trade positions each time the recorded call begins.

**Hannah Pike:** There. You can disconnect the aerial from this side without putting anyone in that lane. Or warn the next travelers. A fight is an option, not an obligation.

*Existing gesture: speak.*

- **Disconnect the beacon and mark the safe detour.** Requires: none. Applies: `beacon_disabled`, `trust_hannah+=2`, `wilderness_routes`. Response: `beacon_safe`; stage: `inherited continuation`.
- **Broadcast a warning, leaving the ambushers exposed.** Requires: `comms`. Applies: `beacon_warning`, `wilderness_routes`, `communications_restored`. Response: `beacon_warn`; stage: `inherited continuation`.
- **Drive the ambushers away and open the direct road.** Requires: none. Applies: `beacon_fight`. Response: `—`; stage: `hill_fight`.

#### `beacon_safe`

**Hannah Pike:** A quiet road is a result. You do not have to fill it with bodies to prove you did something.

*Existing gesture: speak.*


Continuation: `freehold_arrive`.

#### `beacon_warn`

**Hannah Pike:** I hear the road watch repeating you. Good. Let the lie travel slower than the warning for once.

*Existing gesture: speak.*


Continuation: `freehold_arrive`.

#### `freehold_table`

**Orrin Lake:** Forty sacks until thaw. Twenty-three mouths already. You bring nine more, you have thirty-two people hungry later.

*Existing gesture: speak.*

**Hannah Pike:** And nine people dead now if I close the door. They have a welder, a midwife, and seven people who should not need a useful job to be let inside.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** We can count both the food and the people. Neither number becomes kinder if we stop saying it.

*Existing gesture: speak.*

**Hannah Pike:** The canal farms can cover some of the difference if their shipment got through. Otherwise, we need a ration agreement or somewhere else warm enough to go.

*Existing gesture: speak.*

- **Bring in all nine; use the released canal farm shipment.** Requires: `freehold_supply`. Applies: `freehold_refuge=2`, `coalition+=1`, `trust_hannah+=2`, `winter_food=2`. Response: `freehold_supply`; stage: `inherited continuation`.
- **Share the winter ration and convert the reserved lodging into a refuge.** Requires: none. Applies: `freehold_refuge=1`, `coalition+=1`, `trust_hannah+=1`, `winter_food=1`. Response: `freehold_ration`; stage: `inherited continuation`.
- **Arrange guarded onward travel instead of permanent admission.** Requires: none. Applies: `freehold_refuge=3`, `civilian_rescues+=1`, `trust_hannah-=1`. Response: `freehold_travel`; stage: `inherited continuation`.

#### `freehold_supply`

**Orrin Lake:** I saw the sacks. That changes the arithmetic. It does not make me glad I said no.

*Existing gesture: speak.*

**Hannah Pike:** You can help me move the spare beds. Let them hear the answer from both of us.

*Existing gesture: speak.*


Continuation: `witness_arrive`.

#### `freehold_ration`

**Orrin Lake:** Everyone cuts back. Including the people making the speech.

*Existing gesture: speak.*

**Hannah Pike:** Including me. The reserved rooms become family rooms. We will need the boarded windows sealed before nightfall.

*Existing gesture: speak.*


Continuation: `freehold_refuge`.

#### `freehold_travel`

**Hannah Pike:** I will take them to the next shelter. I will remember why I had to. Keep a lamp on until we have all come back.

*Existing gesture: speak.*


Continuation: `witness_arrive`.

#### `weller_intro`

**Simon Weller:** I maintained command links. I did not choose targets. I tell myself that a lot.

*Existing gesture: speak.*

**Ruth Weller:** Tell them the other part. The part you say in your sleep.

*Existing gesture: speak.*

**Simon Weller:** An authenticated order after the news said Richardson was dead. An instruction to preserve continuity personnel and deny a surrender channel. I heard it. I kept the acknowledgement tape.

*Existing gesture: speak.*

**Simon Weller:** I am not leaving Ruth and the children. They hid me when an old colleague came asking. Do not offer me safety that makes them pay for it.

*Existing gesture: speak.*

**Scene [when `dead_simon`]:** Simon is dead. His labeled acknowledgement tape survives in the cabin. Without his testimony, its date and provenance will need independent checking.

- **Travel together. I will put the whole family on the evacuation request.** Requires: `!dead_simon`. Applies: `witness_travels`, `witness_family`, `testimony`, `trust_simon+=2`. Response: `weller_travel`; stage: `inherited continuation`.
- **Record the testimony and leave the family at the sheltered Freehold.** Requires: `freehold_refuge>=1`, `!dead_simon`. Applies: `testimony`, `witness_sheltered`, `witness_family`. Response: `weller_tape`; stage: `inherited continuation`.
- **Hand Simon to the Directorate in exchange for access.** Requires: `!dead_simon`. Applies: none. Response: `weller_warning`; stage: `inherited continuation`.
- **Preserve the tape and the family record. Seek independent corroboration.** Requires: `dead_simon`. Applies: `witness_lost`, `tape_unverified`, `journal:Simon died. His recording remains, but cannot answer questions. Northbank can compare it with independent records.`. Response: `—`; stage: `watertown_arrive`.

#### `weller_travel`

**Ruth Weller:** Then help me pack enough food for all of us. We are not a document with feet.

*Existing gesture: speak.*

**Simon Weller:** The tape is in the labeled tin. I will carry it myself until we meet whoever is receiving us.

*Existing gesture: speak.*


Continuation: `watertown_arrive`.

#### `weller_tape`

**Simon Weller:** I will give you the recording and a signed account. If anyone can get a message back, tell me what the outside records say. I need to know which parts I remember correctly.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*


Continuation: `watertown_arrive`.

#### `weller_warning`

**Ruth Weller:** He trusted you because Hannah sent you. If you hand him over, the rest of us cannot stay here. Understand that before you say another word.

*Existing gesture: speak.*

- **Confirm the surrender. This betrays Simon and exposes his family.** Requires: `!dead_simon`. Applies: `witness_surrendered`, `directorate_access`, `trust_hannah-=4`, `civilian_harm+=2`, `testimony=0`. Response: `weller_surrender`; stage: `inherited continuation`.
- **Withdraw the threat.** Requires: `!dead_simon`. Applies: none. Response: `weller_intro`; stage: `inherited continuation`.

#### `weller_surrender`

**Simon Weller:** There is a duplicate acknowledgement at the hospital archive. I am telling you that because someone ought to use it. It is not forgiveness.

*Existing gesture: speak.*

**Ruth Weller:** Look at me. Do not watch the door while I am talking to you. You know what you are doing.

*Existing gesture: stop; Minimum beat hold: 1.5 s.*


Continuation: `watertown_arrive`.

## 03 Crossing Canada

### Playable mission sequence

#### A Place on the Manifest · `watertown_arrive`

Mission: `manifest`. Location: `watertown`.

Discuss a crossing route with Northbank and Ada.

Cast: Dr. Leila Chen, Ada Quinn, Mara Finch, Simon Weller, Lena Ortiz, Milo Sato.

Arrival conversation: `northbank_intake`.


#### The Waiting Room · `route_patients`

Mission: `waitingroom`. Location: `watertown`.

Reconnect dispatch and prepare the patient convoy.

Cast: Dr. Leila Chen, Mara Finch.

- **Reconnect the receiving station line** (`dispatch_line`). Requires: `!dispatch_ready`. Applies: `dispatch_ready`. Conversation: `—`; next stage: `—`.
- **Check every patient against the transport roster** (`patient_roster`). Requires: `dispatch_ready`, `!patients_counted`. Applies: `patients_counted`. Conversation: `—`; next stage: `—`.
- **Secure the oxygen cart and leave with the convoy** (`oxygen_cart`). Requires: `patients_counted`. Applies: `patient_convoy_ready`. Conversation: `—`; next stage: `crossing_arrive`.
- **Return to Northbank and choose a different route** (`reconsider_route_patients`). Requires: none. Applies: `reconsider_route`. Conversation: `—`; next stage: `watertown_arrive`.

#### Material Witness · `route_witness`

Mission: `materialwitness`. Location: `watertown`.

Seal the evidence and protect the transfer identity.

Cast: Dr. Leila Chen, Simon Weller, Ruth Weller, Lena Ortiz.

- **Seal the corroborated evidence packet** (`witness_seal`). Requires: `!sealed_evidence`. Applies: `sealed_evidence`. Conversation: `—`; next stage: `—`.
- **Separate the public manifest from the confidential witness record** (`witness_manifest`). Requires: `sealed_evidence`. Applies: `witness_confidential`. Conversation: `—`; next stage: `crossing_arrive`.
- **Return to Northbank and choose a different route** (`reconsider_route_witness`). Requires: none. Applies: `reconsider_route`. Conversation: `—`; next stage: `watertown_arrive`.

#### Night Crossing · `route_ferry`

Mission: `nightcrossing`. Location: `ferry`.

Check Ada's rescue equipment and passenger arrangements.

Cast: Ada Quinn, Milo Sato, Mara Finch.

- **Decide the ferry load** (`talk_ada`). Requires: none. Applies: none. Conversation: `route_ferry`; next stage: `—`.
- **Check the spare line and fuel** (`ferry_lines`). Requires: `!ferry_repaired`. Applies: `ferry_repaired`. Conversation: `—`; next stage: `—`.
- **Join the ferry staging group** (`ferry_leave`). Requires: `ferry_repaired`, `ferry_load_chosen`. Applies: `ferry_ready`. Conversation: `—`; next stage: `crossing_arrive`.
- **Return to Northbank and choose a different route** (`reconsider_route_ferry`). Requires: none. Applies: `reconsider_route`. Conversation: `—`; next stage: `watertown_arrive`.

#### All Those Lights · `crossing_arrive`

Mission: `lastmile`. Location: `crossing`.

Regroup at the American staging area.

Cast: Dr. Leila Chen, Ada Quinn, Sergeant Imani Bell, Mara Finch, Simon Weller, Milo Sato.

- **Help the stranded convoy** (`route_patients_begin`). Requires: `entry_route=1`. Applies: none. Conversation: `lastmile_patients`; next stage: `—`.
- **Protect the confidential transfer** (`route_witness_begin`). Requires: `entry_route=2`. Applies: none. Conversation: `lastmile_witness`; next stage: `—`.
- **Reach the last landing** (`route_ferry_begin`). Requires: `entry_route=3`. Applies: none. Conversation: `lastmile_ferry`; next stage: `—`.

#### The Last Mile · `crossing_patients`

Mission: `lastmile`. Location: `crossing`.

Rescue the rear patients and clear the blocked approach.

Cast: Dr. Leila Chen, Ada Quinn, Sergeant Imani Bell, Mara Finch, Simon Weller, Milo Sato, Tomas Paredes, Ruth Weller.

Authored encounter: 4 base opponents; preparation and saved defeats modify the live count.

- **Lead the stranded group to Northbank cover** (`rescue_patients`). Requires: `!crossing_rescued`. Applies: `crossing_rescued`, `civilian_rescues+=2`, `evacuate_group`. Conversation: `—`; next stage: `—`.
- **Clear the convoy obstruction** (`clear_patients`). Requires: `!crossing_open`. Applies: `crossing_open`. Conversation: `—`; next stage: `—`.
- **Enter humanitarian processing** (`cross_patients`). Requires: `crossing_open`, `group_at_cover`. Applies: `humanitarian_permit`, `travel_canada`. Conversation: `—`; next stage: `canada_arrival`.
- **Fall back and choose another route** (`retreat_patients`). Requires: none. Applies: `crossing_retreat`. Conversation: `—`; next stage: `watertown_arrive`.

#### The Last Mile · `crossing_witness`

Mission: `lastmile`. Location: `crossing`.

Recover the intercepted evidence and protect the witness group.

Cast: Dr. Leila Chen, Ada Quinn, Sergeant Imani Bell, Mara Finch, Simon Weller, Milo Sato.

Authored encounter: 4 base opponents; preparation and saved defeats modify the live count.

- **Recover the separate evidence case** (`rescue_witness`). Requires: `!crossing_rescued`. Applies: `crossing_rescued`, `civilian_rescues+=2`. Conversation: `—`; next stage: `—`.
- **Secure the witness escape gate** (`clear_witness`). Requires: `!crossing_open`. Applies: `crossing_open`. Conversation: `—`; next stage: `—`.
- **Enter humanitarian processing** (`cross_witness`). Requires: `crossing_open`, `crossing_rescued`. Applies: `humanitarian_permit`, `travel_canada`. Conversation: `—`; next stage: `canada_arrival`.
- **Fall back and choose another route** (`retreat_witness`). Requires: none. Applies: `crossing_retreat`. Conversation: `—`; next stage: `watertown_arrive`.

#### The Last Mile · `crossing_ferry`

Mission: `lastmile`. Location: `crossing`.

Secure the landing and recover stranded passengers.

Cast: Dr. Leila Chen, Ada Quinn, Sergeant Imani Bell, Mara Finch, Simon Weller, Milo Sato, Tomas Paredes, Ruth Weller.

Authored encounter: 3 base opponents; preparation and saved defeats modify the live count.

- **Lead the stranded group to Northbank cover** (`rescue_ferry`). Requires: `!crossing_rescued`. Applies: `crossing_rescued`, `civilian_rescues+=2`, `evacuate_group`. Conversation: `—`; next stage: `—`.
- **Release the last mooring** (`clear_ferry`). Requires: `!crossing_open`. Applies: `crossing_open`. Conversation: `—`; next stage: `—`.
- **Process weapons and board the Night Ferry** (`cross_ferry`). Requires: `crossing_open`, `group_at_cover`. Applies: `humanitarian_permit`, `travel_canada`. Conversation: `—`; next stage: `canada_arrival`.
- **Fall back and choose another route** (`retreat_ferry`). Requires: none. Applies: `crossing_retreat`. Conversation: `—`; next stage: `watertown_arrive`.

#### All Those Lights · `canada_arrival`

Mission: `arrival`. Location: `canada`.

Complete processing and take a clean bed.

Cast: Claire Boudreau, Dr. Leila Chen, Mara Finch, Lena Ortiz.

Arrival conversation: `border_arrival`.


#### A Room With a Door · `canada_settle`

Mission: `arrival`. Location: `canada`.

Wash at the sink. Rest in the right-hand bay, or continue when you are ready.

Cast: Claire Boudreau, Mara Finch.

- **Use the wash station** (`canada_wash`). Requires: `!washed`. Applies: `washed`. Conversation: `—`; next stage: `—`.
- **Sleep for four hours in your clean bed** (`canada_sleep`). Requires: `washed`. Applies: `canada_rested`, `journal:Northbank gave us a safe room. I slept before meeting the community workshop.`. Conversation: `—`; next stage: `canada_tuesday`.
- **Continue to the workshop without sleeping** (`canada_continue`). Requires: `washed`. Applies: `journal:Northbank gave us a safe room. I chose to visit the workshop before sleeping.`. Conversation: `—`; next stage: `canada_tuesday`.

#### A Normal Tuesday · `canada_tuesday`

Mission: `tuesday`. Location: `canada`.

Spend time in the community workshop.

Cast: Jules Mercier, Dr. Leila Chen, Mara Finch.

Arrival conversation: `tuesday`.


#### Things People Still Do · `canada_routine`

Mission: `tuesday`. Location: `canada`.

Help with an ordinary repair, then collect the clinic prescription.

Cast: Jules Mercier, Dr. Leila Chen, Mara Finch, Claire Boudreau.

- **Hold and secure the workshop shelf** (`library_shelf`). Requires: `!ordinary_work`. Applies: `ordinary_work`. Conversation: `—`; next stage: `—`.
- **Collect the prepared prescription** (`prescription`). Requires: `ordinary_work`. Applies: `canada_rest`, `give:medkit:2`. Conversation: `—`; next stage: `canada_tracing`.
- **Check the family letter filed under an old surname** (`misnamed_letter`). Requires: none. Applies: `traced_alias`. Conversation: `old_name`; next stage: `—`.

#### Names, Not Numbers · `canada_tracing`

Mission: `names`. Location: `canada`.

Meet the family-tracing team.

Cast: Dr. Leila Chen, Tomas Paredes, Ruth Weller, Milo Sato, Mara Finch.

Arrival conversation: `tracing`.


#### What the World Saw · `canada_investigate`

Mission: `worldsaw`. Location: `canada`.

Compare the independent records with your evidence.

Cast: Dr. Leila Chen, Simon Weller, Lena Ortiz, Mara Finch.

- **Read the corroborated September timeline** (`archive_timeline`). Requires: none. Applies: `richardson_revealed`, `evidence_secured`. Conversation: `world_records`; next stage: `—`.

#### What We Owe · `canada_decide`

Mission: `choice`. Location: `canada`.

Decide whether to remain safe or return south.

Cast: Dr. Leila Chen, Mara Finch, Jules Mercier.

- **Discuss what comes next** (`talk_chen`). Requires: none. Applies: none. Conversation: `canada_choice`; next stage: `—`.

#### The Other Shore · `ending_other`

Mission: `ending`. Location: `canada`.

You have chosen a life in Canada.

Cast: Jules Mercier, Dr. Leila Chen, Mara Finch.

Arrival conversation: `other_shore`.


### Complete conversations

#### `northbank_intake`

**Dr. Leila Chen:** Leila Chen, Northbank Mission. Sit wherever there is a chair. If there is not a chair, tell me and I will stop using one for boxes.

*Existing gesture: speak.*

**Dr. Leila Chen:** We can receive patients by protected convoy. A witness with verifiable evidence can request a confidential transfer. Neither is the only way to get north.

*Existing gesture: speak.*

**Ada Quinn:** And if the road is watched, I know the islands. There is a boat. There is a limit to what it carries. We will discuss that before anyone brings a wardrobe.

*Existing gesture: speak.*

**Dr. Leila Chen:** An unofficial arrival still gets processed in Canada. A person does not stop needing help because they used the wrong boat. Our problem is the American approach.

*Existing gesture: speak.*

**Dr. Leila Chen:** Choose the route that suits the people with you. If it closes, come back. We do not cross your name out for trying.

*Existing gesture: speak.*

- **Help reconnect the patient station and take the humanitarian convoy.** Requires: none. Applies: `entry_route=1`. Response: `route_patients`; stage: `route_patients`.
- **Request a protected transfer for the witness and evidence.** Requires: `testimony`. Applies: `entry_route=2`. Response: `route_witness`; stage: `route_witness`.
- **Request a protected transfer using corroborating records.** Requires: `evidence_authentication`. Applies: `entry_route=2`. Response: `route_witness`; stage: `route_witness`.
- **Arrange Ada's crossing through the islands.** Requires: none. Applies: `entry_route=3`. Response: `route_ferry`; stage: `route_ferry`.

#### `route_patients`

**Dr. Leila Chen:** The station lost its line to our dispatcher. Reconnect it, check the patient roster, and secure the oxygen cart. Everyone on that roster travels, including the people who cannot walk.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** Give me the triage list. And someone else the heavy end of the cart.

*Existing gesture: speak.*

**Dr. Leila Chen:** When you leave, the rear stretcher goes first at each obstruction. Otherwise it becomes the one everybody forgets.

*Existing gesture: speak.*


#### `route_witness`

**Dr. Leila Chen:** I need a sealed copy, an independently checkable date, and the name of the person who can explain it. Only the receiving officer sees the full witness name.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Simon Weller [when `witness_travels`, `!dead_simon`]:** And the family. My agreement still includes the family.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Dr. Leila Chen:** I have written them on the same manifest. You can watch me seal it.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Lena Ortiz [when `!dead_lena`]:** My dispatch copies can verify the dates even if the original auditor is not welcome at a government desk.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*


#### `route_ferry`

**Ada Quinn:** Six benches. Cargo below the tape line. Anything higher and we are arguing with the water, and the water is a poor negotiator.

*Existing gesture: speak.*

**Ada Quinn:** Before you hear it from somebody angry: I left another boat behind last year. Engine fire. I got my passengers out. I did not go back for theirs.

*Existing gesture: speak.*

**Milo Sato:** My sister was on it. Do not make that a story about how hard Ada felt.

*Existing gesture: speak.*

**Ada Quinn:** It is his story too. Ask. Then decide whether you trust me with this crossing.

*Existing gesture: speak.*

- **Ask Milo what happened.** Requires: none. Applies: none. Response: `ada_history`; stage: `inherited continuation`.
- **Reserve space for stranded passengers; leave surplus cargo.** Requires: none. Applies: `ferry_rescue_space`, `trust_ada+=1`, `ferry_load_chosen`. Response: `ferry_rescue`; stage: `inherited continuation`.
- **Keep the cargo and take only the listed group.** Requires: none. Applies: `ferry_cargo`, `winter_food+=1`, `ferry_load_chosen`. Response: `ferry_cargo`; stage: `inherited continuation`.

#### `ada_history`

**Milo Sato:** She came back the next morning with a tow line. I know. I was still on the bank. My sister was not.

*Existing gesture: speak.*

**Ada Quinn:** I do not get a cleaner version. This time I have a second line, spare fuel, and someone watching the stern. You can check all three.

*Existing gesture: speak.*

- **Check the rescue equipment and leave room for passengers.** Requires: none. Applies: `ferry_rescue_space`, `ada_past_known`, `trust_milo+=2`, `ferry_load_chosen`. Response: `ferry_rescue`; stage: `inherited continuation`.
- **Keep the manifest limited to the agreed group.** Requires: none. Applies: `ferry_cargo`, `ada_past_known`, `ferry_load_chosen`. Response: `ferry_cargo`; stage: `inherited continuation`.

#### `ferry_rescue`

**Ada Quinn:** The crates stay. People get the benches. Milo, you check the spare line. You do not have to trust my hands with it.

*Existing gesture: speak.*

**Scene:** The passenger space is marked on the manifest. American weapons will still be held in secure Canadian storage on arrival.


#### `ferry_cargo`

**Milo Sato:** Then if we find someone out there, you explain the empty space above the crates.

*Existing gesture: speak.*

**Ada Quinn:** We will know the limit before we cast off. That is at least more honest than inventing it in the dark.

*Existing gesture: speak.*

**Scene:** The passenger space is marked on the manifest. American weapons will still be held in secure Canadian storage on arrival.


#### `lastmile_patients`

**Dr. Leila Chen:** The lead truck is blocked. Two patients are still behind it. Move the obstruction and get the rear group under cover; our receiving team can see the signal light.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** I have the front stretcher. Go to the one you cannot see from here.

*Existing gesture: speak.*

**Scene:** Across the crossing, streetlights remain steady. The shooting is behind the American staging sheds.


Continuation: `crossing_patients`.

#### `lastmile_witness`

**Sergeant Imani Bell:** That is not a customs vehicle. They came through the side gate without stopping.

*Existing gesture: speak.*

**Simon Weller [when `witness_travels`, `!dead_simon`]:** They used my old call sign. Someone told them which transfer this was.

*Existing gesture: speak.*

**Dr. Leila Chen:** Keep the sealed copy apart from the witness. Either can make it across and bring attention to the other. Do not let them take both.

*Existing gesture: speak.*


Continuation: `crossing_witness`.

#### `lastmile_ferry`

**Ada Quinn:** Lines off. No—hold. People on the last landing.

*Existing gesture: speak.*

**Milo Sato:** I can reach them with the spare line if someone keeps that searchlight off us.

*Existing gesture: speak.*

**Ada Quinn:** The far shore is ready. We choose whether to make another approach here. Not whether Canada will let them be people.

*Existing gesture: speak.*


Continuation: `crossing_ferry`.

#### `border_arrival`

**Claire Boudreau:** Hello. I am Claire. Before the questions: is anyone having trouble breathing?

*Move to local mark [0, 100, 0] cm; Existing gesture: point; Minimum beat hold: 1 s.*

**Mara Finch [when `!dead_mara`]:** Only when I stop to notice it.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Claire Boudreau:** All right. We will take it slowly. Your weapons are in sealed storage. This is the receipt. Keep it; they are yours to collect if you go back.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Claire Boudreau:** There is hot water through that door. It takes a moment to run warm. You do not have to fill every bottle before you use it.

*Move to local mark [160, 100, 0] cm; Existing gesture: speak; Minimum beat hold: 1.6 s.*

**Mara Finch [when `!dead_mara`]:** You have the lights on in the empty room.

*Existing gesture: speak; Minimum beat hold: 1.2 s.*

**Claire Boudreau:** Someone will use it later. There is a clean bed for each person on your group record. We can talk about tomorrow after you have slept.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Scene:** The processing questions stop. Claire leaves the wash station and rest bays open. There is no alarm and no order to leave.


Continuation: `canada_settle`.

#### `tuesday`

**Jules Mercier:** Could you hold this shelf while I tighten it? The books keep arriving faster than the brackets.

*Move to local mark [-120, 120, 0] cm; Existing gesture: point; Minimum beat hold: 0.65 s.*

**Mara Finch [when `!dead_mara`]:** People still return library books?

*Existing gesture: point.*

**Jules Mercier:** Late, mostly. But yes. We waived the fines for arrivals. It seemed a strange hill to die on.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** I used to think I wanted a whole day without anyone needing me. Now I have one and I keep checking the door.

*Existing gesture: speak; Minimum beat hold: 1 s.*

**Jules Mercier:** You can help for ten minutes and leave. We have enough people for someone to take a break.

*Existing gesture: speak.*

**Dr. Leila Chen [when `!dead_mara`]:** Your prescription is ready, Mara. No emergency queue. Just the pharmacy desk.

*Existing gesture: speak.*


Continuation: `canada_routine`.

#### `tracing`

**Dr. Leila Chen [when `tomas_released`, `!dead_tomas`]:** We matched Tomas's case with Elena's inquiry. They can speak over the mission line.

*Existing gesture: speak.*

**Tomas Paredes [when `tomas_released`, `!dead_tomas`]:** Luis? ...Yes. By the heater is a good place for them. Keep the shoes there until I come.

*Existing gesture: point; Minimum beat hold: 1.7 s.*

**Dr. Leila Chen [when `!tomas_released`]:** Tomas's intake is confirmed, but we cannot locate him after the annex. I will not tell Elena that no news is good news.

*Existing gesture: speak.*

**Ruth Weller [when `witness_family`, `!dead_ruth`]:** The ring in that freight sack is ours. Simon took it off before they searched him. I thought he had sold it to get home.

*Existing gesture: speak.*

**Milo Sato [when `ada_past_known`]:** They found my sister's name in the shoreline recovery ledger. I asked them to check the spelling. It was right.

*Existing gesture: speak; Minimum beat hold: 1.6 s.*

**Dr. Leila Chen:** Some cases are open. Some have an answer nobody wanted. Each keeps a person's name. Nothing gets reduced to the number of passengers a boat carried.

*Existing gesture: speak.*


Continuation: `canada_investigate`.

#### `world_records`

**Dr. Leila Chen:** These are independent broadcast archives, intercepted warning records, and relief reports. None alone tells the whole story. Start with the dates.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Scene:** SEPTEMBER 2028: foreign target reports record attempted American launches that failed to reach their targets. Independently timed imagery records subsequent attacks on American cities. The sequence is corroborated; individual decisions abroad remain incompletely documented.

**Scene:** A dated ceasefire channel required Richardson's surrender. The acknowledgement tape rejects those terms and orders the preservation of continuity personnel. Separate command testimony records officers preventing another outward escalation.

**Simon Weller [when `testimony`, `!dead_simon`, `!witness_surrendered`]:** That is the acknowledgement I heard. Not a man on television claiming it later. The reply on our circuit that night.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Lena Ortiz [when `!dead_lena`]:** The emergency-stock transfers started before the attacks. I can match the requisition signatures to the continuity compounds. They chose who would have supplies.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Dr. Leila Chen:** A recent Meridian medical requisition bears the same live authentication chain. A defector identifies Richardson in a current secure attendance record. With the earlier evidence, we can say he survived. We cannot say he controlled every event that followed.

*Existing gesture: speak; Minimum beat hold: 0.8 s.*

**Mara Finch [when `!dead_mara`]:** He let a country burn rather than surrender himself. And now he has lists of who gets a room in the part he kept.

*Existing gesture: speak; Minimum beat hold: 1.3 s.*


Continuation: `canada_decide`.

#### `canada_choice`

**Mara Finch [when `!dead_mara`]:** I am safe here. I need you to hear that before you ask what comes next. I can go a day without deciding who gets the last sterile needle.

*Existing gesture: speak.*

**Dr. Leila Chen:** You may remain. You may return. Neither decision changes your right to a bed here. We cannot send an army south with you.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** If you go back, give me an honest job and an honest way out. Do not ask me to call every sacrifice worthwhile.

*Existing gesture: speak.*

- **Stay in Canada. Build a life on this shore.** Requires: none. Applies: `remain_canada`, `ending_other`, `journal:I chose to remain in Canada. Safety did not require another war.`. Response: `other_shore`; stage: `ending_other`.
- **Return for the missing people. Ask Mara to stay safe here.** Requires: none. Applies: `return_chosen`, `mara_stayed`, `evidence_secured`, `humanitarian_permit`. Response: `return_mara_stays`; stage: `homecoming`.
- **Return and ask Mara to help with medical evacuation, with a guaranteed way back.** Requires: `!dead_mara`, `trust_mara>=1`. Applies: `return_chosen`, `mara_returns`, `evidence_secured`, `humanitarian_permit`. Response: `return_mara_joins`; stage: `homecoming`.
- **Return to investigate Richardson's offer of power.** Requires: none. Applies: `return_chosen`, `mara_stayed`, `ambition`, `evidence_secured`, `humanitarian_permit`. Response: `return_ambition`; stage: `homecoming`.

#### `return_mara_stays`

**Mara Finch [when `!dead_mara`]:** Thank you for letting that be a complete answer. Write if you can. I will still answer.

*Existing gesture: speak.*

**Dr. Leila Chen:** Your crossing record stays open. Collect your stored equipment at the American checkpoint. The mission will keep a copy of the evidence here.

*Existing gesture: speak.*


#### `return_mara_joins`

**Mara Finch:** Medical evacuation. Not revenge dressed as medicine. If you make me choose between your campaign and patients, you already know my answer.

*Existing gesture: speak.*

**Dr. Leila Chen:** I will keep her return space and the evidence copy. Do not use either as a promise you can casually spend.

*Existing gesture: speak.*


#### `return_ambition`

**Mara Finch [when `!dead_mara`]:** Then name it ambition. Do not borrow the people we lost to make it sound better.

*Existing gesture: speak.*

**Dr. Leila Chen:** The archive keeps its independent copy. Whatever you choose south of here cannot make the record disappear.

*Existing gesture: speak.*


#### `other_shore`

**Jules Mercier:** There is a vacancy at the workshop. Trial shift on Monday. It is all right if you have forgotten what a Monday feels like.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`]:** I put a plant by the window. Something that needs water and is allowed to be a small problem.

*Existing gesture: speak; Minimum beat hold: 1.5 s.*

**Dr. Leila Chen [when `bell_condition=1`]:** News still comes from the south. Bellwether needs roof repairs.

*Existing gesture: speak.*

**Dr. Leila Chen [when `bell_condition=3`]:** The people who left Bellwether have found another room together. They sent a list of names.

*Existing gesture: speak.*

**Scene:** You keep the names. You work. Sometimes you answer a letter. Canada remains safe. The world you left continues beyond your choice to stop fighting.


## 04 Return Meridian

### Playable mission sequence

#### The People You Left Behind · `homecoming`

Mission: `homecoming`. Location: `bellwether`.

Return to Bellwether and learn what changed.

Cast: Ivo Bell, Tessa Rowan, Mara Finch, Lena Ortiz.

Arrival conversation: `homecoming`.


#### Proof of Life · `proof_life`

Mission: `proof`. Location: `labor`.

Identify the workers behind the processing numbers.

Cast: Vera Cho, Tomas Paredes, Lena Ortiz.

- **Compare the worker roster with the family records** (`labor_roster`). Requires: none. Applies: `prisoner_roster`. Conversation: `labor_names`; next stage: `—`.

#### A Voice That Carries · `voice_carries`

Mission: `broadcast`. Location: `rome`.

Choose how to use the corroborated evidence.

Cast: Della Voss, Sergeant Imani Bell, Ada Quinn.

- **Decide how the evidence travels** (`talk_imani`). Requires: none. Applies: none. Conversation: `evidence_decision`; next stage: `—`.

#### The President Will See You Now · `meridian_access`

Mission: `access`. Location: `meridian`.

Choose a credible way into Meridian.

Cast: Vera Cho.

- **Plan the approach** (`talk_vera`). Requires: none. Applies: none. Conversation: `access`; next stage: `—`.

#### The Service Gate · `meridian_service`

Mission: `access`. Location: `meridian`.

Observe a delivery cycle and pass the maintenance gate.

Cast: Vera Cho.

- **Observe and use the delivery access** (`delivery_cycle`). Requires: none. Applies: `service_access`. Conversation: `—`; next stage: `meridian_life`.

#### A Functioning Country · `meridian_life`

Mission: `country`. Location: `meridian`.

Walk the continuity resort and speak with Vera.

Cast: Vera Cho.

Arrival conversation: `functioning_country`.


#### The Unregistered · `meridian_unregistered`

Mission: `unregistered`. Location: `meridian`.

Read the admission terms in the identity office.

Cast: Vera Cho, Marshal Adrian Keene.

- **Read the identity retention and labor terms** (`admission_terms`). Requires: none. Applies: `inheritance_evidence`. Conversation: `unregistered`; next stage: `—`.

#### The Offer · `meridian_offer`

Mission: `offer`. Location: `meridian`.

Answer Richardson's guarded invitation.

Cast: President Ronald Richardson, Vera Cho, Marshal Adrian Keene.

Arrival conversation: `offer_open`.

- **Speak to Richardson** (`talk_richardson`). Requires: none. Applies: none. Conversation: `offer_open`; next stage: `—`.

#### A Table for Enemies · `coalition_table`

Mission: `tableenemies`. Location: `rome`.

Negotiate responsibilities and control of the reserves.

Cast: Della Voss, Captain Elias Rook, Sergeant Imani Bell, Hannah Pike, Ada Quinn, Mara Finch.

Arrival conversation: `coalition`.


#### Before the Gates Close · `prisoner_plan`

Mission: `gatesclose`. Location: `labor`.

Copy the transfer roster and prepare a civilian exit.

Cast: Vera Cho, Hannah Pike.

- **Identify the remaining families** (`last_roster`). Requires: none. Applies: `prisoner_roster`. Conversation: `—`; next stage: `—`.
- **Prepare the service exit** (`escape_route`). Requires: `prisoner_roster`. Applies: `evac_route`. Conversation: `—`; next stage: `—`.

Automatic continuation: `resistance_evacuate` after `evac_route`.

#### Terms of Service · `terms`

Mission: `terms`. Location: `meridian`.

Review the authority and dependencies of the commission.

Cast: President Ronald Richardson, Marshal Adrian Keene, Vera Cho.

Arrival conversation: `terms`.


#### Necessary Measures · `necessary_measures`

Mission: `measures`. Location: `meridian`.

Decide whether to enforce the incorporation order.

Cast: President Ronald Richardson, Vera Cho.

Arrival conversation: `commitment`.


### Complete conversations

#### `homecoming`

**Ivo Bell [when `bell_condition=1`, `!dead_ivo`]:** You came back. Door still catches, but the room is warm.

*Existing gesture: speak.*

**Tessa Rowan [when `bell_condition=2`, `!dead_tessa`]:** We closed the clinic with plywood. I left your name in the departure book. Now I get to cross it out.

*Existing gesture: speak.*

**Tessa Rowan [when `bell_condition=3`, `!dead_tessa`]:** We came back for the things we could carry. Nobody sleeps here now. We use the market rooms.

*Existing gesture: speak.*

**Mara Finch [when `mara_returns`, `!dead_mara`]:** Canada is still there. I need everyone here to know that. We did not dream it.

*Existing gesture: speak.*

**Ivo Bell [when `registration_active`, `!dead_ivo`]:** The Directorate came asking for skilled households. Their list already had ours on it.

*Existing gesture: speak.*

**Tessa Rowan [when `registration_withdrawn`, `!dead_tessa`]:** They brought a list with the names blanked out. Somebody saved us a difficult conversation.

*Existing gesture: speak.*

**Scene:** At the intake desk, the empty places retain the names of the people who died here.

**Lena Ortiz [when `!dead_lena`]:** The latest labor orders point into the Adirondack supply corridor. We can identify the missing workers by the belongings we catalogued.

*Existing gesture: speak.*


Continuation: `proof_life`.

#### `labor_names`

**Tomas Paredes [when `!dead_tomas`, `!tomas_released`]:** Elena kept the ring? ...Then she believed I was coming back.

*Existing gesture: point.*

**Vera Cho:** I teach in the resort school. These are the parents of children upstairs. The children were told their parents volunteered for a separate work district.

*Existing gesture: speak.*

**Vera Cho:** I believed it until one of them asked why volunteers needed a gate code to visit. Her father works this shift.

*Existing gesture: speak.*

**Vera Cho:** The roster has names, sleeping blocks, and the next transfer time. Take a copy. The service gate can be opened from the shift desk. Do not sound the general alarm with families in the yard.

*Existing gesture: speak.*

- **Open the service gate and escort the workers out.** Requires: none. Applies: `labor_rescued`, `tomas_released`, `civilian_rescues+=3`, `trust_vera+=2`. Response: `labor_rescue`; stage: `inherited continuation`.
- **Secure the roster quietly and prepare a later evacuation.** Requires: none. Applies: `prisoner_roster`, `insider_access`, `leverage+=1`. Response: `labor_wait`; stage: `inherited continuation`.

#### `labor_rescue`

**Vera Cho:** I will count them against the classroom register. That tells us which children we still have to find.

*Existing gesture: speak.*

**Tomas Paredes [when `!dead_tomas`]:** Give me the back end of that stretcher. I have been carried far enough.

*Existing gesture: speak.*


Continuation: `voice_carries`.

#### `labor_wait`

**Vera Cho:** I can delay the transfer once. After that, I have to tell them I was lying. Remember there are people inside the time you are buying.

*Existing gesture: speak.*


Continuation: `voice_carries`.

#### `evidence_decision`

**Della Voss [when `!dead_della`]:** A public copy can stop crews obeying the next transfer order. It also tells Meridian exactly what we know.

*Existing gesture: speak.*

**Sergeant Imani Bell:** Private distribution gives the guard something to check before it acts. Silence gives the Directorate another unchallenged briefing.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`]:** You can sell access, too. Say that out loud if that is the plan. The buyers will know who paid for your silence.

*Existing gesture: speak.*

- **Publish the corroborated evidence with protected witness identities.** Requires: none. Applies: `evidence_public`, `defections=2`, `meridian_alert=2`, `coalition+=1`. Response: `evidence_publish`; stage: `inherited continuation`.
- **Share verified copies privately with the surviving allies.** Requires: none. Applies: `evidence_private`, `defections=1`, `meridian_alert=1`. Response: `evidence_private`; stage: `inherited continuation`.
- **Withhold the American copies and preserve surprise.** Requires: none. Applies: `evidence_withheld`, `meridian_alert=0`. Response: `evidence_withhold`; stage: `inherited continuation`.
- **Sell exclusive access to the Directorate. The Canadian archive remains independent.** Requires: none. Applies: `evidence_sold`, `directorate_access`, `trust_mara-=4`, `trust_hannah-=3`, `credits_reward:300`. Response: `evidence_sell`; stage: `inherited continuation`.

#### `evidence_publish`

**Sergeant Imani Bell:** I will include the source dates and what we cannot confirm. People deserve something better than a new certainty shouted from the old loudspeaker.

*Existing gesture: speak.*


Continuation: `meridian_access`.

#### `evidence_private`

**Sergeant Imani Bell:** Separate copies. Separate couriers. If one is caught, the others still arrive.

*Existing gesture: speak.*


Continuation: `meridian_access`.

#### `evidence_withhold`

**Ada Quinn:** Then I will stop promising the people south of here an explanation. Surprise has a price; they are paying part of it.

*Existing gesture: speak.*


Continuation: `meridian_access`.

#### `evidence_sell`

**Ada Quinn:** You got paid. That part worked. Do not expect the families on those sheets to call it a negotiation.

*Existing gesture: speak.*


Continuation: `meridian_access`.

#### `access`

**Vera Cho:** Meridian has guest lists, labor lists, and maintenance rosters. Being on one is not the same as being welcome everywhere.

*Existing gesture: speak.*

**Vera Cho:** If you come as a guest, they will search you and escort you. If you use my maintenance route, the service corridor is your way out as well as in. A prisoner exchange happens at the guarded reception gate.

*Existing gesture: speak.*

- **Use the Directorate invitation and submit to the escorted reception.** Requires: `directorate_access`. Applies: `meridian_entry=1`. Response: `access_guest`; stage: `inherited continuation`.
- **Negotiate an exchange of the recovered transfer records for family access.** Requires: `evidence_transfer`. Applies: `meridian_entry=2`. Response: `access_exchange`; stage: `inherited continuation`.
- **Use Vera's service route under an assumed maintenance assignment.** Requires: `insider_access`. Applies: `meridian_entry=3`, `suspicion=1`. Response: `access_insider`; stage: `inherited continuation`.
- **Observe the deliveries and infiltrate through the maintenance gate.** Requires: none. Applies: `meridian_entry=4`, `suspicion=2`. Response: `access_infiltrate`; stage: `inherited continuation`.

#### `access_guest`

**Vera Cho:** They know you are coming. Expect courtesy, guards, and no room with an unobserved exit.

*Existing gesture: speak.*


Continuation: `meridian_life`.

#### `access_exchange`

**Vera Cho:** I will transmit the case numbers first. They cannot pretend they do not know which families you mean.

*Existing gesture: speak.*


Continuation: `meridian_life`.

#### `access_insider`

**Vera Cho:** Memorize the shift name. Do not memorize a lie about the people you meet there. Most of them do not know what is downstairs.

*Existing gesture: speak.*


Continuation: `meridian_life`.

#### `access_infiltrate`

**Vera Cho:** The kitchen delivery leaves the service gate open for a trolley, not a squad. Watch its next cycle and go through before the latch resets.

*Existing gesture: speak.*


Continuation: `meridian_service`.

#### `functioning_country`

**Vera Cho:** The school has heat. The infirmary has medicine. The vegetables came from covered beds here, not a warehouse nobody else can reach.

*Existing gesture: speak.*

**Vera Cho:** That is why people defend this place. It saved their children. Some cannot afford to ask what paid for it.

*Existing gesture: speak.*

**Scene:** A school notice lists visiting hours for WORK DISTRICT PARENTS. A handwritten request beneath it asks who can authorize a visit outside those hours.

**Vera Cho:** Come to the identity office. It is not on the guest tour.

*Existing gesture: speak.*


Continuation: `meridian_unregistered`.

#### `unregistered`

**Scene:** IDENTITY OFFICE: travel papers surrendered on admission. Reassignment requires command approval. Refusal of labor allocation suspends dependent housing privileges.

**Vera Cho:** There is the choice. Work where you are told, or your family loses the room. They call it voluntary because the gate is not always locked.

*Existing gesture: speak.*

**Marshal Adrian Keene [when `!dead_keene`]:** The president has asked to speak to you. Yes. The president. You will be escorted. Whatever you think you have learned, listen before you decide what it means.

*Existing gesture: speak.*

**Scene [when `dead_keene`]:** An escorted audience order is delivered from the executive office.


Continuation: `meridian_offer`.

#### `offer_open`

**President Ronald Richardson:** Please. The coffee is fresh. You have spent a great deal of time among people who cannot offer that without giving something up.

*Move to local mark [0, 350, 0] cm; Existing gesture: speak; Minimum beat hold: 1 s.*

**President Ronald Richardson [when `meridian_entry=1`]:** Your arrival was expected. Keene's office kept me informed.

*Existing gesture: speak.*

**President Ronald Richardson [when `meridian_entry=2`]:** You negotiated your way to the door. I prefer a person who knows what their information is worth.

*Existing gesture: speak.*

**President Ronald Richardson [when `meridian_entry=3`]:** The maintenance supervisor noticed an extra worker. We will leave the question of whose idea that was for later.

*Existing gesture: speak.*

**President Ronald Richardson [when `meridian_entry=4`]:** You entered through a service gate. You are now in a guarded room because I requested a conversation instead of an arrest.

*Existing gesture: speak.*

**President Ronald Richardson [when `heat>=1`, `registration=1`]:** Bellwether. A shortage of fuel. A heater you chose to keep running. People remember the person who makes a cold room warm.

*Existing gesture: speak.*

**President Ronald Richardson [when `aid_received`]:** Keene sent me the clinic referral. You understand that medicine delivered today persuades more reliably than a promise about next year.

*Existing gesture: speak.*

**President Ronald Richardson:** I do not need you because you are unique. I need you because people let you through their doors. I can offer food, secure transport, a medical allocation, and regional authority. In return, independent armed networks become part of one administration.

*Existing gesture: speak.*

- **Ask about the people forbidden to leave.** Requires: none. Applies: none. Response: `offer_labor`; stage: `inherited continuation`.
- **Ask about the ceasefire he rejected in 2028.** Requires: none. Applies: none. Response: `offer_ceasefire`; stage: `inherited continuation`.
- **Ask what he expects you to do to your allies.** Requires: none. Applies: none. Response: `offer_cost`; stage: `inherited continuation`.
- **Give your answer.** Requires: none. Applies: none. Response: `offer_answer`; stage: `inherited continuation`.

#### `offer_labor`

**President Ronald Richardson:** I cannot operate a water plant with everyone free to leave their shift at once.

*Existing gesture: speak.*

**Vera Cho:** Their children lose housing if they refuse the shift. They surrendered their identities. That is not a scheduling problem.

*Existing gesture: speak.*

**President Ronald Richardson:** It is coercion. A government uses it. The question is whether it builds something that lasts. I will not flatter you by pretending otherwise.

*Existing gesture: speak.*

- **Return to the offer.** Requires: none. Applies: none. Response: `offer_open`; stage: `inherited continuation`.

#### `offer_ceasefire`

**President Ronald Richardson:** They demanded my surrender in exchange for a pause they could end whenever it suited them.

*Existing gesture: speak.*

**President Ronald Richardson:** I chose to preserve a national government. Millions died while I made that choice. You want an admission? There it is. You still have to decide who can keep the next million alive.

*Existing gesture: point.*

**Vera Cho:** The personnel who stopped your next launch kept people alive without preserving your command.

*Existing gesture: speak.*

**President Ronald Richardson:** And now you are standing in the part they did not build.

*Existing gesture: speak.*

- **Return to the offer.** Requires: none. Applies: none. Response: `offer_open`; stage: `inherited continuation`.

#### `offer_cost`

**President Ronald Richardson:** Names, routes, meeting places. Then disarmament, incorporation, and removal of leaders who organize violent refusal. Registration. Consolidation. Removal. Project Inheritance is a sequence, not a slogan.

*Existing gesture: speak.*

**President Ronald Richardson:** You will not be asked merely to smile beside a flag. If you accept command, you will arrest people who trusted you. Some will refuse to submit. I expect you to carry out lawful sentences.

*Existing gesture: speak.*

**President Ronald Richardson:** Consider it carefully. A reluctant signature is of no use to either of us.

*Existing gesture: speak.*

- **Give your answer.** Requires: none. Applies: none. Response: `offer_answer`; stage: `inherited continuation`.

#### `offer_answer`

**President Ronald Richardson:** I have explained the position. Ask for time if you need it. Do not agree to a responsibility you intend to pretend was unclear.

*Existing gesture: speak.*

- **Refuse. People are not his property.** Requires: none. Applies: `resistance`. Response: `offer_refuse`; stage: `coalition_table`.
- **Accept a provisional commission. Review the first order before any commitment to violence.** Requires: none. Applies: `provisional`, `directorate_access`, `leverage+=1`. Response: `offer_provisional`; stage: `terms`.
- **Feign acceptance to expose the transfer system, without promising an execution.** Requires: `insider_access`. Applies: `double_agent`, `directorate_access`, `suspicion+=1`. Response: `offer_deception`; stage: `terms`.

#### `offer_refuse`

**President Ronald Richardson:** Then leave by the guest route while it is still available to you. The escort will make sure you do. I do not intend to keep offering the same terms.

*Existing gesture: speak.*


#### `offer_provisional`

**President Ronald Richardson:** Review it. I prefer useful agreement to theatrical loyalty. Keene's office will show you what regional authority can accomplish.

*Existing gesture: speak.*


#### `offer_deception`

**President Ronald Richardson:** You are careful about promises. I respect that. I will be equally careful about what access you receive.

*Existing gesture: speak.*


#### `coalition`

**Della Voss [when `!dead_della`]:** I can move freight and repair the route. I cannot promise a clean fight. Ask somebody who commands one.

*Existing gesture: speak.*

**Captain Elias Rook [when `rook_cooperates`, `!dead_rook`]:** I can coordinate units and issue a credible stand-down instruction. They will need proof and a place to surrender.

*Existing gesture: speak.*

**Sergeant Imani Bell [when `imani_leads`]:** The guard I command answers to the ward and the corridor communities. We will not trade one private chain of command for another.

*Existing gesture: speak.*

**Hannah Pike [when `!dead_hannah`]:** I can get families through the woods. Put a marching column on that route and you will lose both the cover and the families.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`]:** Extraction, decoys, the last vehicle out. Do not assign me a heroic final stand. I have spent too long learning why it is a bad plan.

*Existing gesture: speak.*

**Mara Finch [when `mara_returns`, `!dead_mara`]:** I take the wounded. I do not choose who deserves treatment by their uniform.

*Existing gesture: speak.*

**Sergeant Imani Bell [when `guard_fragmented`]:** The hospital guard has splintered since Rook died. I can provide medics and a small escort. I will not promise the units that left.

*Existing gesture: speak.*

**Hannah Pike [when `defected`, `!dead_hannah`]:** I will get families out. That does not make us friends again. The people you sent to the Directorate are still missing.

*Existing gesture: speak.*

**Scene:** Absent seats remain empty. The surviving representatives divide the work they actually know; no one inherits a dead leader's experience.

- **Agree to a joint relief council with public accounts and divided keys.** Requires: none. Applies: `postwar_control=0`, `coalition+=1`, `support+=2`. Response: `keys_shared`; stage: `inherited continuation`.
- **Give the Lockkeepers central control of supply distribution.** Requires: `!dead_della`, `trust_della>=0`. Applies: `postwar_control=1`, `logistics+=1`, `support+=1`. Response: `keys_della`; stage: `inherited continuation`.
- **Give the Civic Guard emergency authority over supplies and security.** Requires: `guard_support`. Applies: `postwar_control=2`, `guard_support+=1`, `support+=1`. Response: `keys_guard`; stage: `inherited continuation`.
- **Put the Freeholds in charge of food and local access, without a central council.** Requires: `!dead_hannah`, `trust_hannah>=0`. Applies: `postwar_control=3`, `winter_food+=1`, `support+=1`. Response: `keys_freehold`; stage: `inherited continuation`.

#### `keys_shared`

**Sergeant Imani Bell:** Then put the rules on paper before we have a building worth arguing over. Two signatures to release reserves, no family loses food for refusing a job.

*Existing gesture: speak.*


Continuation: `prisoner_plan`.

#### `keys_della`

**Della Voss:** A ledger, a timetable, and a single dispatch desk. It will work. People will also have to ask us before they move what they need.

*Existing gesture: speak.*


Continuation: `prisoner_plan`.

#### `keys_guard`

**Sergeant Imani Bell:** An emergency mandate needs an end date. If the guard will not sign that date, people will have reason to fear what we built.

*Existing gesture: speak.*


Continuation: `prisoner_plan`.

#### `keys_freehold`

**Hannah Pike:** Food stays with the communities that raise it. I will not pretend that makes every village generous. We will have our own arguments to answer for.

*Existing gesture: speak.*


Continuation: `prisoner_plan`.

#### `terms`

**Marshal Adrian Keene [when `!dead_keene`]:** Your commission clears supply requisitions and commands one regional detachment. It does not give you the president's personal troops.

*Existing gesture: speak.*

**President Ronald Richardson:** The first order is to identify the opposition's gathering place and deliver an incorporation demand. Review the personnel roster and reserve allocation. You are holding authority now, not merely being shown it.

*Existing gesture: speak.*

**Vera Cho:** If you are going to turn on this system, these are the transport codes. You can evacuate the people on the next list instead of delivering them. That will cost your disguise.

*Existing gesture: speak.*

- **Audit the detachment roster and keep an independent reserve allocation.** Requires: none. Applies: `loyal_personnel`, `reserve_control`, `leverage+=2`, `support+=1`. Response: `terms_leverage`; stage: `inherited continuation`.
- **Leave personnel and reserves entirely under Richardson's control.** Requires: none. Applies: `leverage=0`, `support=0`. Response: `terms_dependent`; stage: `inherited continuation`.
- **Use the codes to evacuate the targeted households; expose the deception now.** Requires: none. Applies: `resistance`, `double_exposed`, `civilian_rescues+=2`, `loyalist=0`, `suspicion+=3`. Response: `terms_defect`; stage: `coalition_table`.

#### `terms_leverage`

**President Ronald Richardson:** You want a command that can function without asking permission for every crate. Sensible. I will hold you responsible for it.

*Existing gesture: speak.*


Continuation: `necessary_measures`.

#### `terms_dependent`

**President Ronald Richardson:** Then requests come through this office. Keep the chain intact and it will keep you supplied.

*Existing gesture: speak.*


Continuation: `necessary_measures`.

#### `terms_defect`

**Vera Cho:** The families are moving. They will know who opened the gate, even if nobody at your old meetings trusts you yet.

*Existing gesture: speak.*


#### `commitment`

**President Ronald Richardson:** The community will surrender its weapons, register every household, and accept appointed leadership. You will arrest the people who organize refusal. If you use their trust to bring them here, you will not be permitted to set them free afterward.

*Existing gesture: speak.*

**President Ronald Richardson:** This is the commitment. You may resign the commission before you carry it out. Afterward, do not call yourself an observer.

*Existing gesture: speak.*

- **Carry out incorporation and the arrests. I understand this betrays the community.** Requires: none. Applies: `loyalist`, `betrayal_committed`, `trust_mara-=8`, `trust_hannah-=8`, `civilian_harm+=2`. Response: `commit_loyal`; stage: `loyal_hunt`.
- **Resign and take the order to the resistance.** Requires: none. Applies: `resistance`, `loyalist=0`, `defected`, `evidence_order`. Response: `commit_resist`; stage: `coalition_table`.
- **Warn the community and falsify the departure count; accept that this exposes me.** Requires: `double_agent`. Applies: `resistance`, `loyalist=0`, `double_exposed`, `civilian_rescues+=2`, `evidence_order`. Response: `commit_double`; stage: `coalition_table`.

#### `commit_loyal`

**President Ronald Richardson:** Then proceed. Your detachment will follow a clear order. Make certain you give one.

*Existing gesture: speak.*


#### `commit_resist`

**President Ronald Richardson:** You have seen what we can preserve. Remember that when you choose what to destroy.

*Existing gesture: speak.*


#### `commit_double`

**Vera Cho:** The departure count buys an hour. I have already told the families what that hour is for. Now go before Keene checks the gate.

*Existing gesture: speak.*


## 05 Finale Epilogues

### Playable mission sequence

#### The Long Way Out · `resistance_evacuate`

Mission: `longwayout`. Location: `labor`.

Get the labor families out before the scheduled transfer.

Cast: Vera Cho, Hannah Pike, Mara Finch.

Arrival conversation: `evacuation_orders`.


#### The Long Way Out · `resistance_exit`

Mission: `longwayout`. Location: `labor`.

Unlock the service gate, escort the families through the central passage, and count them at the sheltered exit.

Cast: Vera Cho, Hannah Pike, Mara Finch.

- **Unlock the labor settlement service gate** (`labor_gate`). Requires: none. Applies: `labor_exit_open`. Conversation: `—`; next stage: `—`.
- **Count the first families at the sheltered exit** (`shelter_count`). Requires: `labor_exit_open`, `crowd_clear77`. Applies: `labor_rescued`, `civilian_rescues+=3`. Conversation: `—`; next stage: `standdown`.

#### An Order to Stand Down · `standdown`

Mission: `standdown`. Location: `guard`.

Offer the security unit a credible surrender guarantee.

Cast: Sergeant Imani Bell, Captain Elias Rook.

Arrival conversation: `standdown`.


#### An Order to Stand Down · `resistance_line`

Mission: `standdown`. Location: `guard`.

Defeat the unit holding the approach.

Cast: Sergeant Imani Bell.

Authored encounter: 5 base opponents; preparation and saved defeats modify the live count.

- **Open the secured approach** (`security_clear`). Requires: `enemies_clear`. Applies: `security_cleared`. Conversation: `—`; next stage: `winter_stores`.

#### Winter Stores · `winter_stores`

Mission: `winterstores`. Location: `labor`.

Choose an approach that accounts for the food and medicine.

Cast: Della Voss, Vera Cho.

Arrival conversation: `winter_plan`.


#### Winter Stores · `winter_secure`

Mission: `winterstores`. Location: `labor`.

Separate the defense feed from the cold stores.

Cast: Della Voss, Vera Cho.

- **Isolate the transport-hall defense feed** (`feed_isolate`). Requires: none. Applies: `defense_feed_isolated`. Conversation: `—`; next stage: `—`.
- **Test the independent cold-store supply** (`coldstore_test`). Requires: `defense_feed_isolated`. Applies: `reserves_secured`. Conversation: `—`; next stage: `meridian_assault`.

#### The Last President · `meridian_assault`

Mission: `lastpresident`. Location: `meridian`.

Approach Meridian through the severe weather.

Cast: Sergeant Imani Bell, Hannah Pike, Della Voss.

Arrival conversation: `assault_approach`.


#### The Inhabited Sector · `meridian_inside`

Mission: `lastpresident`. Location: `meridian`.

Separate the defenders from the families seeking shelter.

Cast: Vera Cho, Marshal Adrian Keene, Sergeant Imani Bell.

- **Open the family corridor** (`family_route`). Requires: none. Applies: `family_corridor`. Conversation: `—`; next stage: `—`.
- **Confront Marshal Keene** (`talk_keene`). Requires: `!dead_keene`. Applies: none. Conversation: `keene_decision`; next stage: `—`.
- **Read the dead marshal's last routing order** (`keene_orders`). Requires: `dead_keene`. Applies: `keene_resolved`. Conversation: `—`; next stage: `carrier_hall`.

#### The Marshal's Detachment · `keene_fight`

Mission: `lastpresident`. Location: `meridian`.

Fight the detachment while keeping the family corridor open.

Cast: Vera Cho, Sergeant Imani Bell, Marshal Adrian Keene.

Authored encounter: 5 base opponents; preparation and saved defeats modify the live count.

- **Secure the transport-hall access** (`detachment_clear`). Requires: `enemies_clear`, `dead_keene`. Applies: `keene_resolved`. Conversation: `—`; next stage: `carrier_hall`.

#### The Executive Carrier · `carrier_hall`

Mission: `lastpresident`. Location: `transport`.

Enter the transport-hall approach.

Cast: President Ronald Richardson, Sergeant Imani Bell, Della Voss.

Arrival conversation: `carrier_reveal`.


#### The Executive Carrier · `carrier_battle`

Mission: `lastpresident`. Location: `transport`.

Disable the defense feed and shutters under escort fire.

Cast: Sergeant Imani Bell, Della Voss, President Ronald Richardson.

Authored encounter: 6 base opponents; preparation and saved defeats modify the live count.

- **Disconnect the external carrier defense feed** (`carrier_feed`). Requires: `!carrier_feed_cut`. Applies: `carrier_feed_cut`. Conversation: `—`; next stage: `—`.
- **Drop the transport shutters across the carrier route** (`carrier_shutters`). Requires: `!carrier_trapped`. Applies: `carrier_trapped`. Conversation: `—`; next stage: `—`.
- **Open the disabled command platform** (`carrier_breach`). Requires: `carrier_feed_cut`, `carrier_trapped`, `enemies_clear`. Applies: `carrier_disabled`. Conversation: `broken_office`; next stage: `—`.

#### The Broken Office · `richardson_final`

Mission: `lastpresident`. Location: `transport`.

Defeat the last escort and kill Richardson.

Cast: President Ronald Richardson, Sergeant Imani Bell, Vera Cho, Mara Finch, Ada Quinn, Della Voss.

Authored encounter: 3 base opponents; preparation and saved defeats modify the live count.

- **Return to the civilian evacuation** (`final_clear`). Requires: `dead_richardson`, `enemies_clear`. Applies: `richardson_dead`. Conversation: `everyone_out`; next stage: `—`.

#### Everyone Out · `final_evacuation`

Mission: `everyoneout`. Location: `meridian`.

Keep the school and residential evacuation lane clear. Account for the surviving group before dispatching supplies.

Cast: Vera Cho, Sergeant Imani Bell, Ada Quinn, Mara Finch, Della Voss.

- **Account for the families at the exit** (`evac_count`). Requires: `crowd_clear77`. Applies: `evac_counted`. Conversation: `—`; next stage: `—`.
- **Dispatch the final medical and supply transport** (`final_supplies`). Requires: `evac_counted`. Applies: `evac_complete`. Conversation: `—`; next stage: `ending_resistance`.

#### The Country We Left Behind · `ending_resistance`

Mission: `ending`. Location: `meridian`.

Live with what the victory preserved and what it cost.

Cast: Mara Finch, Vera Cho, Sergeant Imani Bell.

Arrival conversation: `resistance_epilogue`.


#### Old Friends · `loyal_hunt`

Mission: `oldfriends`. Location: `freehold`.

Identify the former safehouse and its meeting routes.

Cast: Marshal Adrian Keene, President Ronald Richardson.

Arrival conversation: `old_friends`.


#### Old Friends · `loyal_safehouse`

Mission: `oldfriends`. Location: `freehold`.

Search the known refuge and send the leadership invitation.

Cast: Marshal Adrian Keene.

- **Identify the occupied refuge rooms** (`safehouse_records`). Requires: none. Applies: `safehouse_identified`. Conversation: `—`; next stage: `—`.
- **Send the trusted invitation** (`meeting_invitation`). Requires: `safehouse_identified`. Applies: `meeting_bait`. Conversation: `—`; next stage: `loyal_meeting`.

#### Come Home · `loyal_meeting`

Mission: `comehome`. Location: `rome`.

Meet the leaders who answered the invitation.

Cast: Della Voss, Hannah Pike, Ada Quinn.

Arrival conversation: `meeting_arrest`.


#### A Necessary Example · `ally_example`

Mission: `example`. Location: `meridian`.

Confront the condemned former ally.

Cast: President Ronald Richardson, Mara Finch, Della Voss, Hannah Pike, Ada Quinn, Captain Elias Rook, Ivo Bell, Tessa Rowan, Vera Cho, Sergeant Imani Bell.

Arrival conversation: `example_order`.


#### Hearth and Home · `loyal_bellwether`

Mission: `hearthhome`. Location: `bellwether`.

Enforce the confiscations and compulsory household register.

Cast: Ivo Bell, Tessa Rowan.

- **Confiscate the community arms** (`confiscate`). Requires: none. Applies: `bell_confiscated`. Conversation: `—`; next stage: `—`.
- **Sign the forced-transfer register** (`register_bell`). Requires: `bell_confiscated`. Applies: `bell_incorporated`, `civilian_harm+=2`. Conversation: `incorporation`; next stage: `—`.

#### The Country Is Yours · `loyal_border`

Mission: `countryyours`. Location: `crossing`.

Command the blockade on the American side.

Cast: President Ronald Richardson, Captain Elias Rook, Della Voss, Hannah Pike, Ada Quinn, Mara Finch.

Arrival conversation: `border_mirror`.


#### The Last American Landing · `loyal_final`

Mission: `countryyours`. Location: `crossing`.

Defeat the resistance defense and seize its evacuation records.

Cast: Captain Elias Rook, Della Voss, Hannah Pike, Ada Quinn, Mara Finch, President Ronald Richardson.

Authored encounter: 7 base opponents; preparation and saved defeats modify the live count.

- **Seize the evacuation register** (`blockade_records`). Requires: `enemies_clear`. Applies: `coalition_defeated`. Conversation: `loyal_outcome`; next stage: `—`.

#### The Inheritance · `loyal_takeover`

Mission: `inheritance`. Location: `meridian`.

Use the prepared takeover and defeat Richardson's guard.

Cast: President Ronald Richardson.

Authored encounter: 4 base opponents; preparation and saved defeats modify the live count.

- **Assume control of the executive office** (`take_control`). Requires: `dead_richardson`, `enemies_clear`. Applies: `richardson_dead`. Conversation: `—`; next stage: `ending_loyal`.

#### The Country Is Yours · `ending_loyal`

Mission: `ending`. Location: `meridian`.

Live with the order you helped impose.

Cast: President Ronald Richardson.

Arrival conversation: `loyal_epilogue`.


### Complete conversations

#### `evacuation_orders`

**Vera Cho:** They move the labor families tonight. The service exit you prepared is still clear. Open it before you touch the security office.

*Existing gesture: speak.*

**Hannah Pike:** I marked a route that keeps them out of the vehicle yard. Walk the first group to the shelter marker. The rest will follow people, not a shouted coordinate.

*Existing gesture: speak.*

**Mara Finch [when `mara_returns`, `!dead_mara`]:** I will take anyone who cannot walk. Do not send them down a stairwell and call them evacuated.

*Existing gesture: speak.*


Continuation: `resistance_exit`.

#### `standdown`

**Sergeant Imani Bell:** We have the signed order and a place to process surrender. Their lieutenant has asked who guarantees his people will not be shot after they put their rifles down.

*Existing gesture: speak.*

**Captain Elias Rook [when `rook_cooperates`, `!dead_rook`]:** I will give that guarantee in my own name, under the ward's authority.

*Existing gesture: speak.*

**Sergeant Imani Bell:** If Rook is not speaking for us, the civilian relief roster can. It has to be more than a threat with a polite ending.

*Existing gesture: speak.*

- **Transmit the verified orders and the signed surrender guarantee.** Requires: `evidence_authentication`, `guard_support`. Applies: `unit_surrender`, `defections+=1`. Response: `standdown_yes`; stage: `inherited continuation`.
- **Use the public evidence and an independently witnessed guarantee.** Requires: `evidence_public`, `evidence_secured`. Applies: `unit_surrender`, `defections+=1`. Response: `standdown_yes`; stage: `inherited continuation`.
- **They will not accept our guarantee. Fight through the security line.** Requires: none. Applies: `security_fight`. Response: `—`; stage: `resistance_line`.

#### `standdown_yes`

**Sergeant Imani Bell:** Rifles down. Keep the lane open. They are walking out. Nobody settles a private score in this line.

*Existing gesture: speak.*


Continuation: `winter_stores`.

#### `winter_plan`

**Della Voss [when `!dead_della`]:** Those tanks feed kitchens, not just generators. That warehouse is seed and medicine. Blow the compound open from this side and there is no clever way to unburn it.

*Existing gesture: speak.*

**Vera Cho:** The isolation valves can cut the hall defense feed without cutting the refrigeration. The manual bypass is slower. Either saves the stores.

*Existing gesture: speak.*

- **Isolate the defense feed and preserve the winter reserves.** Requires: none. Applies: `preserve_stores`. Response: `—`; stage: `winter_secure`.
- **Open a demolition breach through the reserve block. The food and medicine will be lost.** Requires: none. Applies: `reserves_lost`, `civilian_losses+=2`. Response: `winter_loss`; stage: `inherited continuation`.

#### `winter_loss`

**Vera Cho:** Then we evacuate the store crews first. The people who survive the battle will still have to survive the winter you just made harder.

*Existing gesture: speak.*


Continuation: `meridian_assault`.

#### `assault_approach`

**Hannah Pike [when `!dead_hannah`, `wilderness_routes`]:** Storm is covering the service ridge. Small groups on my markers. Keep the wounded route separate.

*Existing gesture: speak.*

**Della Voss [when `!dead_della`, `logistics`]:** Freight diversion is moving. You have the maintenance approach until they notice the second truck is empty.

*Existing gesture: speak.*

**Sergeant Imani Bell:** Use the covered entrance. Civilians to the refuge rooms. Anyone who lays down a weapon goes to the surrender point.

*Existing gesture: speak.*

**Scene:** The approach remains possible without allied guides, but the exposed lane has no diversion.


Continuation: `meridian_inside`.

#### `keene_decision`

**Marshal Adrian Keene:** You have reached the inhabited sector. The families here are not a shield I intend to die behind.

*Existing gesture: speak.*

**Marshal Adrian Keene:** I implemented the lists. I signed transfers. I will not tell you I never knew. I can order my detachment off the family corridor. Tell me what happens after.

*Existing gesture: speak.*

- **Surrender to civilian custody. Give the stand-down order and face an inquiry.** Requires: none. Applies: `keene_captured`, `unit_surrender`, `civilian_rescues+=1`. Response: `keene_custody`; stage: `inherited continuation`.
- **Help evacuate the families, then surrender with your detachment.** Requires: `prisoner_roster`. Applies: `keene_defects`, `civilian_rescues+=2`. Response: `keene_help`; stage: `inherited continuation`.
- **Refuse his terms and fight his detachment.** Requires: none. Applies: `keene_fights`. Response: `—`; stage: `keene_fight`.

#### `keene_custody`

**Marshal Adrian Keene:** I will transmit it on the open channel. You will have my voice on the order. Keep it for the inquiry.

*Existing gesture: speak.*


Continuation: `carrier_hall`.

#### `keene_help`

**Marshal Adrian Keene:** I know which gate leads to the school. Put someone beside me who will stop me if I turn the wrong way.

*Existing gesture: speak.*


Continuation: `carrier_hall`.

#### `carrier_reveal`

**President Ronald Richardson [when `terminal_working`]:** You restored a canal terminal, and now you intend to dismantle the government that can supply it.

*Existing gesture: speak.*

**President Ronald Richardson [when `registration_active`]:** You kept the household files active. You understood their value when they opened a door for you.

*Existing gesture: speak.*

**President Ronald Richardson:** The carrier contains a functioning command office. It can leave this hall while you argue over who gets the warehouses. Stand down.

*Existing gesture: speak.*

**Sergeant Imani Bell:** Its escorts keep the service controls covered. The hall shutters and external feed give it overlapping defenses. Break either system and the other still works.

*Existing gesture: speak.*

**Della Voss [when `!dead_della`, `reserves_secured`]:** The refrigeration stays on the separate feed we preserved. I can keep the winter stores running while you cut the carrier loose.

*Existing gesture: speak.*

**Scene:** Through the armored glass: a clean desk, a national flag, a lit reading lamp. The command office travels inside the machine.

**President Ronald Richardson [when `evidence_public`]:** Your Canadian friends kept copies. They will have an accurate account of what happens to you here.

*Existing gesture: speak.*

**President Ronald Richardson [when `aid_received`]:** The clinic referral was honored. Ask yourself who will replace its next shipment when this hall burns.

*Existing gesture: speak.*

**Sergeant Imani Bell [when `!dead_imani`]:** The rotating gun has a separate power feed. Listen for the acquisition click before the burst. Break that feed or work the isolation panel.

*Existing gesture: speak.*

**Scene:** The external feed can be shot out or disconnected at its marked panel. The loading shutter can trap the moving platform. Both controls are reachable from cover.


Continuation: `carrier_battle`.

#### `broken_office`

**Scene:** The command platform stops. Its protected office opens to the transport hall: the desk lamp still burns beside torn wall panels. A guard drops the evacuation case and pulls Richardson toward the service suite.

*Minimum beat hold: 1.4 s.*

**President Ronald Richardson:** Close that door. Get the door closed.

*Move to local mark [0, 500, 0] cm; Existing gesture: speak; Minimum beat hold: 0.65 s.*

**Sergeant Imani Bell:** The carrier is disabled. Richardson is moving with the last escort. The civilian exit stays open. Do not follow him into the refuge rooms.

*Existing gesture: speak.*


Continuation: `richardson_final`.

#### `everyone_out`

**Vera Cho:** The school group is out. The workers are still coming through the service passage. Count them against the roster.

*Existing gesture: speak.*

**Mara Finch [when `mara_returns`, `!dead_mara`]:** Richardson is dead. That does not make this patient breathe. Bring me the kit and keep the passage clear.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`, `evac_route`]:** Last vehicle is at the extraction marker. I am staying until the count matches.

*Existing gesture: speak.*

**Della Voss [when `!dead_della`, `reserves_secured`]:** Stores are on the isolated feed. We can get the food out after the people.

*Existing gesture: speak.*

**Vera Cho [when `reserves_lost`]:** The reserve block is gone. We are taking every usable dressing and blanket from the ward.

*Existing gesture: speak.*


Continuation: `final_evacuation`.

#### `resistance_epilogue`

**Scene [when `ending_open`]:** Richardson is dead. The joint relief council opens its accounts, the Canadian archive releases its evidence, and the preserved reserves reach the surrounding communities.

**Scene [when `ending_uniforms`, `postwar_control=1`]:** The Lockkeepers control distribution from a single dispatch desk. Trains run and kitchens reopen. Every independent settlement now negotiates with the people holding the freight keys.

**Scene [when `ending_uniforms`, `postwar_control=2`]:** The Civic Guard keeps the roads open under an emergency mandate. The promised expiration date becomes the first real test of the new command.

**Scene [when `ending_uniforms`, `postwar_control=3`]:** The Freeholds keep food and local access under village councils. Some welcome travelers; others close their gates. No president orders them to open.

**Scene [when `ending_uniforms`, `postwar_control=0`]:** Without a durable coalition, control settles around the strongest surviving transport and security crews. Their uniforms change. The arguments about who may pass do not.

**Scene [when `ending_ash`]:** Richardson dies, but destroyed reserves and civilian losses define the winter. Freedom arrives with empty shelves and more names to trace. The survivors refuse to call the cost inevitable.

**Mara Finch [when `mara_returns`, `!dead_mara`]:** We saved people. We also lost people. I will keep both lists.

*Existing gesture: speak.*

**Scene [when `mara_stayed`, `!dead_mara`]:** Mara remains in Canada. Her letters ask after patients and friends. She has not mistaken staying safe for forgetting them.

**Scene [when `!dead_lena`]:** Lena continues the audit under independent supervision. Her account stays limited to what she can prove.

**Scene [when `dead_lena`]:** Lena's copies survive her. The audit names the missing witness instead of pretending the surviving paperwork can replace her.

**Scene [when `keene_captured`]:** Keene faces an inquiry with the transfer orders he signed entered into evidence.

**Scene [when `keene_defects`]:** Keene's evacuation order is entered alongside his earlier transfers. Helping at the end does not erase what he authorized before it.

**Scene [when `dead_keene`]:** Keene died before he could answer for the transfers. The surviving records identify his part without inventing a final confession.

**Scene [when `bell_condition=1`]:** Bellwether remains a warm room with a repaired gate and a departure book people still bother to keep.

**Scene [when `bell_condition=2`]:** Bellwether rebuilds a room at a time. The boarded clinic window is replaced before anyone restores the sign.

**Scene [when `bell_condition=3`]:** The Bellwether families retain their name in the market refuge. They decide together whether the abandoned plaza is worth reclaiming.


#### `old_friends`

**Marshal Adrian Keene [when `!dead_keene`]:** The first safehouse is one you used on the northern journey. Your detachment has the perimeter. You identify the room and the meeting route.

*Existing gesture: speak.*

**Scene:** The familiar departure board has new chalk marks. Someone has reserved the room where travelers used to dry their coats.

**President Ronald Richardson:** Your knowledge makes this operation possible. Do not waste it pretending these people are strangers.

*Existing gesture: speak.*


Continuation: `loyal_safehouse`.

#### `meeting_arrest`

**Hannah Pike [when `!dead_hannah`]:** You asked for a meeting. There are soldiers on both roads.

*Existing gesture: speak.*

**Della Voss [when `!dead_della`]:** I counted the transport vehicles. More seats than your delegation needs.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`]:** If this is an arrest, say arrest. We have all had enough trouble caused by polite words.

*Existing gesture: speak.*

- **Order the arrests. These are people who trusted the invitation.** Requires: none. Applies: `leaders_arrested`, `betrayal_meeting`, `civilian_harm+=2`, `support-=1`. Response: `arrest_confirmed`; stage: `inherited continuation`.
- **Warn them and stand down the detachment. Defect; the earlier victims remain harmed.** Requires: none. Applies: `defected`, `resistance`, `loyalist=0`, `suspicion+=4`, `trust_hannah=-2`. Response: `arrest_defect`; stage: `coalition_table`.

#### `arrest_confirmed`

**Della Voss [when `!dead_della`]:** The terminal ran because people trusted a schedule. You have just taught them what your appointments are worth.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`]:** I will remember the exit you stood nearest. You always leave yourself one.

*Existing gesture: speak.*


Continuation: `ally_example`.

#### `arrest_defect`

**Hannah Pike [when `!dead_hannah`]:** We will use the open road. Do not call that forgiving you. There are people missing because of what you already did.

*Existing gesture: speak.*


#### `example_order`

**President Ronald Richardson:** A leader has refused incorporation in front of the detainees. If that refusal succeeds, every order becomes a negotiation. Carry out the sentence. You know the person. That is why it must be you.

*Existing gesture: speak.*

**Scene:** This is an execution order. Carrying it out kills the selected former ally permanently. Refusing it means open defection.

**President Ronald Richardson [when `condemned=0`]:** There is nobody left here who trusted your invitation. The detention officer has suspended this hearing. Return to the incorporation operation; your earlier decisions have already made their example.

*Existing gesture: speak.*

- **Speak to the condemned person before deciding.** Requires: `condemned>=1`. Applies: none. Response: `example_selected`; stage: `inherited continuation`.
- **Refuse the execution and defect.** Requires: none. Applies: `defected`, `resistance`, `loyalist=0`, `suspicion+=4`. Response: `example_refuse`; stage: `coalition_table`.
- **Proceed with the incorporation operation; no invented prisoner takes an absent ally's place.** Requires: `condemned=0`. Applies: `no_available_ally`. Response: `—`; stage: `loyal_bellwether`.

#### `example_selected`

**Mara Finch [when `condemned=1`, `!dead_mara`]:** I trusted you with the people in the warm room. I watched what you chose when there was not enough for everyone. I am looking at that same person now.

*Existing gesture: speak.*

**Mara Finch [when `condemned=1`, `!dead_mara`]:** I came back to take patients out. You brought them another man who can decide whether they get a room. Do not tell me this is keeping me safe.

*Existing gesture: speak.*

**Della Voss [when `condemned=2`, `!dead_della`, `crane_locked`]:** The crane lock. Remember? Nobody put a hand under the load until you set it. We built a rule because a person was worth the delay.

*Existing gesture: point.*

**Della Voss [when `condemned=2`, `!dead_della`]:** You can run every dispatch desk in this country and still be the person who ignored that rule when the load was mine.

*Existing gesture: speak.*

**Hannah Pike [when `condemned=3`, `!dead_hannah`]:** At the winter table, we counted mouths and sacks. I thought you understood neither number was an excuse to stop seeing a face.

*Existing gesture: speak.*

**Hannah Pike [when `condemned=3`, `!dead_hannah`]:** Look at mine. I will not give you the comfort of shouting a threat you can use afterward.

*Existing gesture: speak; Minimum beat hold: 1.5 s.*

**Ada Quinn [when `condemned=4`, `!dead_ada`]:** You heard what happened to the boat I left. I gave you the ugly version because I wanted one person not to make a legend out of it.

*Existing gesture: speak.*

**Ada Quinn [when `condemned=4`, `!dead_ada`]:** This time there is no fire, no tide, no choice forced by the engine. There is just you with time to stop.

*Existing gesture: speak.*

**Captain Elias Rook [when `condemned=5`, `!dead_rook`]:** I obeyed orders I should have questioned. You know that because you made me read my signature aloud. Do not pretend this one arrives without a name attached.

*Existing gesture: speak.*

**Ivo Bell [when `condemned=6`, `!dead_ivo`, `parcel_delivered`]:** You brought my wife's keys to Samir. I thanked you. I will not take that thanks back to make this simpler for you.

*Existing gesture: speak.*

**Tessa Rowan [when `condemned=7`, `!dead_tessa`]:** I wrote your return in the book. There was room beside your name. There is still room to write what you chose today.

*Existing gesture: speak.*

**Vera Cho [when `condemned=8`, `!dead_vera`]:** I opened the service route because you said people would get out. You knew which classroom their children were in. That is what makes this betrayal, not a misunderstanding.

*Existing gesture: speak.*

**Ivo Bell [when `condemned=6`, `!parcel_delivered`, `!dead_ivo`]:** I saw you come into the warm room. You were another person who needed a place. I never asked you to earn the right to stand beside the heater. Remember that while you decide whether I have earned it.

*Existing gesture: speak.*

- **Carry out the execution. The death is real and permanent.** Requires: `condemned>=1`. Applies: `execute_ally`, `ally_executed`, `civilian_harm+=2`, `loyalist`. Response: `execution_after`; stage: `inherited continuation`.
- **Lower the weapon, release the prisoner, and openly defect.** Requires: none. Applies: `defected`, `resistance`, `loyalist=0`, `suspicion+=5`. Response: `example_refuse`; stage: `coalition_table`.

#### `example_refuse`

**Scene:** The sentence is refused. The living prisoner can leave. Other prisoners, earlier arrests, and deaths remain part of what the player must answer for.


#### `execution_after`

**Scene:** The shot ends the conversation. The detachment records the sentence as carried out. The dead ally does not return when allegiance changes.

*Minimum beat hold: 2 s.*

**President Ronald Richardson:** The incorporation of Bellwether is next. Submit the household count with the confiscation report.

*Existing gesture: speak.*


Continuation: `loyal_bellwether`.

#### `incorporation`

**Tessa Rowan [when `!dead_tessa`]:** That is the same intake desk they tried to burn. Now you want us to write the names ourselves.

*Existing gesture: speak.*

**Ivo Bell [when `!dead_ivo`]:** The heater is not payment for owning the room. You were here when we shared what we had.

*Existing gesture: speak.*

**Scene:** Weapons, work classifications, and family addresses are entered on the incorporation sheet. The transfer list includes people the player met before the first attack.


Continuation: `loyal_border`.

#### `border_mirror`

**President Ronald Richardson:** The remaining opposition is gathering on the American side of the crossing. Prevent the witnesses and records from leaving. Canada is not your objective.

*Existing gesture: speak.*

**Captain Elias Rook [when `!dead_rook`, `rook_cooperates`]:** The transport lane stays open. Defensive positions only. Get civilians behind the sheds.

*Existing gesture: speak.*

**Della Voss [when `!dead_della`]:** Use the loading doors. I have put the freight carts where their vehicles cannot pass.

*Existing gesture: speak.*

**Hannah Pike [when `!dead_hannah`]:** Small groups through the trees. Keep away from the road lights.

*Existing gesture: speak.*

**Ada Quinn [when `!dead_ada`]:** Decoy lamp to the west. Actual passengers to the last landing.

*Existing gesture: speak.*

**Mara Finch [when `!dead_mara`, `!mara_stayed`]:** I am with the stretcher group. We are taking civilians across. If you still know my voice, let them go.

*Existing gesture: speak.*

**Scene:** Across the boundary, the lights remain steady. The player is now blocking the route they once fought to reach.


Continuation: `loyal_final`.

#### `loyal_outcome`

**President Ronald Richardson:** The regional opposition is broken. Your command now controls its former routes. There is a secure residence, a paid staff, and an allocation that will not disappear when winter comes.

*Existing gesture: speak.*

- **Remain Richardson's regional enforcer.** Requires: none. Applies: `enforcer`. Response: `loyal_rewards`; stage: `ending_loyal`.
- **Use the independent detachment and reserve control to move against Richardson.** Requires: `leverage>=3`, `loyal_personnel`, `reserve_control`. Applies: `takeover`. Response: `takeover`; stage: `loyal_takeover`.
- **Stand down the remaining blockade and defect even now.** Requires: none. Applies: `loyalist=0`, `defected`, `resistance`, `support=-2`. Response: `late_defection`; stage: `coalition_table`.

#### `loyal_rewards`

**Scene [when `support`]:** Orders are processed under the regional office's new seal. The player receives real authority, security, and income. Independent communities lose the right to refuse that office.

**Scene [when `!support`, `!leverage`]:** Having surrendered independent personnel and reserves, the player receives a reassignment order with no command attached. The escorts who arrive answer only to the executive office.


#### `takeover`

**Scene:** The detachment accepts the reserve codes the player secured earlier. Meridian's local administration loses access to its own supply authorization. The final move still requires taking Richardson's guarded office by force.


#### `late_defection`

**Scene:** The road opens to survivors. They use it without saluting. No one comes back from the grave, and the people arrested at the meeting do not suddenly trust its host.


#### `loyal_epilogue`

**Scene [when `ending_citizen`]:** The player rules as First Citizen under Richardson's administration. Food reaches registered households. Unregistered families discover how far those same roads can keep them from help.

**Scene [when `ending_discarded`]:** Useful Until Dawn: the commission ends in a locked administrative room. There is no detachment that owes its position to the player, no independent stock to negotiate with, and no ally willing to intervene.

**Scene [when `ending_inheritance`]:** The Inheritance: Richardson is dead, and the player holds his personnel files, reserve keys, and enforcement routes. Removing him has not undone the arrests or the executed ally. The machinery of control has a new signature.

**Scene:** Canada remains safe. Fewer people reach it. The family-tracing office keeps the unanswered inquiries open.

**Scene [when `mara_stayed`, `!dead_mara`]:** Mara remains in Canada and refuses further official contact. Her work turns toward the people displaced by the player's administration.

**Scene:** Bellwether is administered through a household register and compulsory transfers. Its old departure book survives in a private collection of names.


#### `old_name`

**Claire Boudreau:** The inquiry was filed under her married name. She signed the crossing record with the name on her old work badge.

*Existing gesture: speak.*

**Scene:** Two entries become one person: Ana Paredes, recorded as Ana Ruiz. The address is checked before a reunion notice is sent.

**Dr. Leila Chen:** We found the name because someone stayed with the form long enough. That is work too.

*Existing gesture: speak.*

