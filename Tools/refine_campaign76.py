"""Continuity and performance pass, applied after the five chronological scripts.
Run campaign76.py --author to rebuild the complete authored library.
"""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
files={p:json.loads(p.read_text(encoding='utf-8')) for p in sorted((ROOT/'Data/Campaign76').glob('*.json'))}
st={x['id']:x for d in files.values() for x in d['stages']}
sc={x['id']:x for d in files.values() for x in d['scenes']}
def beat(who,text,needs=None,**kw):return dict(who=who,text=text,needs=needs or [],**kw)
def choice(text,next='',stage='',needs=None,effects=None):return dict(text=text,next=next,stage=stage,needs=needs or [],effects=effects or [])
def action(id,label,at,mesh='Dossier52',scene='',next='',needs=None,effects=None,work=0):return dict(id=id,label=label,at=at,mesh=mesh,scene=scene,next=next,needs=needs or [],effects=effects or [],work=work)
def newscene(id,beats,choices=None,next=''):
    v=dict(id=id,beats=beats,choices=choices or [],next=next)
    list(files.values())[-1]['scenes'].append(v);sc[id]=v
for d in files.values():d['stages'][:]=[s for s in d['stages'] if s['id']!='syracuse_complete']

# Death creates a different source, never a dead person's conversation with a new face.
for id in ('weller_intro','weller_warning'):
    for c in sc[id]['choices']:c['needs']+=['!dead_simon']
sc['weller_intro']['beats'].append(beat('',"Simon is dead. His labeled acknowledgement tape survives in the cabin. Without his testimony, its date and provenance will need independent checking.",['dead_simon']))
sc['weller_intro']['choices'].append(choice('Preserve the tape and the family record. Seek independent corroboration.',stage='watertown_arrive',needs=['dead_simon'],effects=['witness_lost','tape_unverified','journal:Simon died. His recording remains, but cannot answer questions. Northbank can compare it with independent records.']))
for c in sc['rook_confront']['choices']:c['needs']+=['!dead_rook']
sc['rook_confront']['beats'].append(beat('imani',"Rook is dead. Half the escort section wants to go home; the others want to keep the hospital open. I will hold the ward, but I cannot promise his battalion or use his name to get it.",['dead_rook']))
sc['rook_confront']['choices'].append(choice('Release the ward records and support Imani\'s reduced hospital guard.',stage='hill_arrive',needs=['dead_rook','!dead_imani'],effects=['guard_control=4','imani_leads','guard_support=1','guard_fragmented','evidence_public']))
sc['rook_confront']['beats'].append(beat('',"Both officers are dead. The ward's duty book survives, but nobody can promise an organized escort. The remaining patients need the records made public, not another officer's name on an empty command.",['dead_rook','dead_imani']))
sc['rook_confront']['choices'].append(choice('Preserve the ward records. Continue north without Guard support.',stage='hill_arrive',needs=['dead_rook','dead_imani'],effects=['guard_control=4','guard_support=0','guard_fragmented','evidence_public']))
sc['holding_records']['beats'].append(beat('',"The signed duty book remains evidence even without a living officer to explain it. Its authentication series can be checked against the northern archives.",['dead_imani','dead_lena']))
sc['rome_intro']['beats'].append(beat('',"Della's maintenance instructions survive at the dispatch desk. The crews will need a new agreement; her death has not made the machinery safer.",['dead_della']))
for id in ('rome_vote','hank_terms'):
    for c in sc[id]['choices']:
        if any(e.startswith('canal_control=1') or e=='toll_debt' for e in c['effects']):c['needs']+=['!dead_della']
sc['rome_vote']['choices'][1]['needs']=[]  # shared control remains negotiable after a death
for c in sc['coalition']['choices']:
    if 'postwar_control=3' in c['effects']:c['needs']+=['trust_hannah>=0']
    if 'postwar_control=1' in c['effects']:c['needs']+=['trust_della>=0']
sc['coalition']['beats'].extend([
 beat('imani',"The hospital guard has splintered since Rook died. I can provide medics and a small escort. I will not promise the units that left.",['guard_fragmented']),
 beat('hannah',"I will get families out. That does not make us friends again. The people you sent to the Directorate are still missing.",['defected','!dead_hannah']),
 beat('',"Absent seats remain empty. The surviving representatives divide the work they actually know; no one inherits a dead leader's experience.")])

st['bell_attack']['actions'][0]['scene']='lena_treat'

# The crossing routes are alternatives with usable recovery and clear equipment consequences.
st['route_ferry']['actions'][-1]['needs']+=['ferry_load_chosen']
for id in ('ferry_rescue','ferry_cargo'):
    sc[id]['beats'].append(beat('',"The passenger space is marked on the manifest. American weapons will still be held in secure Canadian storage on arrival."))
for id in ('route_ferry','ada_history'):
    for c in sc[id]['choices']:
        if c['next'] in ('ferry_rescue','ferry_cargo'):c['effects']+=['ferry_load_chosen']
for id in ('route_patients','route_witness','route_ferry'):
    st[id]['actions'].append(action('reconsider_'+id,'Return to Northbank and choose a different route',[0,-480,0],next='watertown_arrive',effects=['reconsider_route']))
for id in ('crossing_patients','crossing_ferry'):
    st[id]['actions'][0]['effects']+=['evacuate_group']
    st[id]['cast']+=['tomas','ruth']
    st[id]['actions'][0]['work']=0
    # Reaching cover is observed, rather than a timer standing beside a medical box.
    st[id]['actions'][0]['label']='Lead the stranded group to Northbank cover'
    st[id]['actions'][-2]['needs']+=['group_at_cover']
st['crossing_witness']['actions'][-2]['needs']+=['crossing_rescued']
# Retreat preserves rescued people and opened exits; switching routes never resurrects them.

# Meaningful environmental state and individual fates in the final encounter.
st['keene_fight']['cast']+=['keene']
st['keene_fight']['actions'][0]['needs']+=['dead_keene']
st['keene_fight']['actions'][0]['effects'].remove('dead_keene')
for id in ('carrier_hall','carrier_battle','richardson_final'):st[id]['site']='transport'
st['carrier_battle']['actions'][0]['at']=[-1000,0,0]
st['carrier_battle']['actions'][1]['at']=[1000,0,0]
st['carrier_battle']['actions'][2]['at']=[0,170,0]
sc['carrier_reveal']['beats'].extend([
 beat('richardson',"Your Canadian friends kept copies. They will have an accurate account of what happens to you here.",['evidence_public']),
 beat('richardson',"The clinic referral was honored. Ask yourself who will replace its next shipment when this hall burns.",['aid_received']),
 beat('imani',"The rotating gun has a separate power feed. Listen for the acquisition click before the burst. Break that feed or work the isolation panel.",['!dead_imani']),
 beat('',"The external feed can be shot out or disconnected at its marked panel. The loading shutter can trap the moving platform. Both controls are reachable from cover.")])
st['ally_example']['cast']+=['imani']
sc['example_order']['choices'][0]['needs']+=['condemned>=1']
sc['example_order']['beats'].append(beat('richardson',"There is nobody left here who trusted your invitation. The detention officer has suspended this hearing. Return to the incorporation operation; your earlier decisions have already made their example.",['condemned=0']))
sc['example_order']['choices'].append(choice('Proceed with the incorporation operation; no invented prisoner takes an absent ally\'s place.',stage='loyal_bellwether',needs=['condemned=0'],effects=['no_available_ally']))
sc['example_selected']['choices'][0]['needs']+=['condemned>=1']
# Do not invent shared experiences when the player chose another resolution.
for b in sc['example_selected']['beats']:
    if b['who']=='mara' and b['text'].startswith('You held'):
        b['text']='I trusted you with the people in the warm room. I watched what you chose when there was not enough for everyone. I am looking at that same person now.'
    if b['who']=='della' and b['text'].startswith('The crane'):
        b['needs']+=['crane_locked']
    if b['who']=='ivo':b['needs']+=['parcel_delivered']
sc['example_selected']['beats'].append(beat('ivo',"I saw you come into the warm room. You were another person who needed a place. I never asked you to earn the right to stand beside the heater. Remember that while you decide whether I have earned it.",['condemned=6','!parcel_delivered','!dead_ivo']))

# Quiet optional work uses existing locations and feeds into later support.
st['market_claim']['actions'].append(action('clinic_break','Sit with Samir between patients',[470,-180,0],scene='clinic_quiet'))
st['canada_routine']['actions'].append(action('misnamed_letter','Check the family letter filed under an old surname',[-520,-250,0],scene='old_name',effects=['traced_alias']))
newscene('old_name',[beat('claire',"The inquiry was filed under her married name. She signed the crossing record with the name on her old work badge."),beat('',"Two entries become one person: Ana Paredes, recorded as Ana Ruiz. The address is checked before a reunion notice is sent."),beat('chen',"We found the name because someone stayed with the form long enough. That is work too.")])
st['canada_routine']['cast']+=['claire']

# Introduce pauses and purposeful blocking only for characters present in those scenes.
for id,who,move in [('bell_intro','mara',[-40,70,0]),('warm_room','mara',[-60,140,0]),('border_arrival','claire',[0,100,0]),('tuesday','jules',[-120,120,0]),('offer_open','richardson',[0,350,0]),('broken_office','richardson',[0,500,0])]:
    if id in sc:
        for b in sc[id]['beats']:
            if b['who']==who:b['move']=move;b['pause']=max(b.get('pause',0),.65);break
for d in files.values():
    for s in d['scenes']:
        for b in s['beats']:
            if b.get('who') and not b.get('gesture'):b['gesture']='point' if '?' in b['text'] else 'speak'
    # Every staged speaker must exist in the stage's cast (including chained responses).
    for s in d['stages']:
        visited=set()
        def include(sceneid):
            if not sceneid or sceneid in visited:return
            visited.add(sceneid);scene=sc[sceneid]
            for b in scene['beats']:
                if b.get('who') and b['who'] not in s['cast']:s['cast'].append(b['who'])
            for c in scene['choices']:include(c.get('next',''))
        include(s.get('arrival',''))
        for a in s['actions']:include(a.get('scene',''))
for p,d in files.items():p.write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
