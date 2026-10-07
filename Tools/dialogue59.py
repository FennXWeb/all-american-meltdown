"""Offline, resumable dialogue generation. Secrets never enter assets or manifests.
The immutable profile registry is the source of truth for future dialogue batches.
Generation requires an explicit --generate and a total batch credit cap.
"""
import argparse, hashlib, io, json, os, sys, urllib.request, urllib.error, winreg
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'ArtSource/Dialogue59'
REG=ROOT/'Data/VoiceProfiles59.json'
def key():
    value=os.environ.get('ELEVENLABS_API_KEY','')
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER,'Environment') as r:value=winreg.QueryValueEx(r,'ELEVENLABS_API_KEY')[0]
    except FileNotFoundError:pass
    if not value:raise RuntimeError('ELEVENLABS_API_KEY unavailable')
    return value.strip()
def normalize(text):return ' '.join(text.split())
def line_key(profile,text):return profile+'_'+hashlib.md5(normalize(text).encode('utf-8')).hexdigest()
def save(path,data):
    path.parent.mkdir(parents=True,exist_ok=True)
    tmp=path.with_suffix('.tmp');tmp.write_text(json.dumps(data,indent=2,ensure_ascii=False),encoding='utf-8');tmp.replace(path)
def registry():
    if not REG.exists():
        names=[('M01','Roger','CwhRBWXzGAHq8TQ4Fs17'),('M02','Brian','nPczCjzI2devNBz1zQrb'),('M03','Harry','SOYHLrjzK2X1ezoPC6cr'),('F01','Sarah','EXAVITQu4vr4xnSDxMaL'),('F02','Jessica','cgSgspJ2msm6clMCkdW9'),('F03','Laura','FGY2WhTYpPnrIDTdsKH5')]
        save(REG,{'schema':1,'assignment_version':1,'profiles':[{'id':i,'name':n,'voice_id':v,'female':i[0]=='F','model_id':'eleven_flash_v2_5','voice_settings':{'stability':.65,'similarity_boost':.8,'style':0,'use_speaker_boost':True,'speed':1.0}} for i,n,v in names], 'cast':{'story_mara':'F01','story_inez':'F02','story_elsie':'F03','story_voss':'F03','story_tessa':'F01','story_jonah':'M01','story_tomas':'M02','story_rusk':'M03','story_mercer':'M02','narrator':'M01'}})
    return json.loads(REG.read_text(encoding='utf-8'))
def starter():
    lines=[
      'The roads are quiet. Stay close, and watch the rooftops.',
      'Still with you. Keep your eyes on the street.',
      'Looking for work? We have contracts.',
      'Supplies for sale. Fair prices.',
      'The clinic is open. Stay safe out there.',
      'Rain is coming. I can feel it.',
      'Someone fixed the water pump.',
      'I used to live north of here.',
      'Need another pair of hands?',
      'I am hit. Give me a moment!',
      'Hostile in town! Take cover!',
      'There you are!',
      'You cannot hide forever!',
      'Keep them pinned down!',
      'Moving up! Cover me!',
      'You picked the wrong fight!',
      'Over here. Could use a hand.',
      'Thank you. We will remember this.',
      'Then we will find another way.',
      "We survive by looking after each other. What do you need?",
      'You picked the wrong people.','Stay close. We are not done yet.',
      'Would you sit with me a moment?',"We came back for you. Keep moving!",
      "I'm here. Finish what we started.",'The pharmacy cabinet. Please take what you need.',
      'Over here. I cannot stop the bleeding.',
      'Six active contracts at a time. Deliver supplies here, and return when your work is finished.']
    rows=[]
    for p in registry()['profiles']:
        for t in lines:
            k=line_key(p['id'],t);rows.append({'key':k,'profile':p['id'],'text':t,'file':k+'.wav','sample':t==lines[0]})
    path=OUT/'starter.json'
    if not path.exists():save(path,{'schema':1,'lines':rows})
    return path
def generate(manifest,budget):
    sys.path.insert(0,str(ROOT/'Saved/Audio42Deps'))
    import soundfile as sf
    import numpy as np
    profiles={p['id']:p for p in registry()['profiles']}
    rows=json.loads(manifest.read_text(encoding='utf-8'))['lines']
    ledger=OUT/'usage.json';usage=json.loads(ledger.read_text(encoding='utf-8')) if ledger.exists() else {}
    spent=sum(x['cost'] for x in usage.values());secret=key()
    for row in rows:
        p=profiles[row['profile']];k=row['key'];target=OUT/row['file']
        assert k==line_key(p['id'],row['text'])
        payload={'text':normalize(row['text']),'model_id':p['model_id'],'voice_settings':p['voice_settings'],'seed':int(hashlib.md5(k.encode()).hexdigest()[:8],16)}
        fingerprint=hashlib.sha256(json.dumps([p['voice_id'],payload],sort_keys=True).encode()).hexdigest()
        if k in usage:
            if usage[k]['fingerprint']!=fingerprint:raise RuntimeError('Profile or text changed for existing audio: '+k)
            if usage[k]['state']=='complete' and target.exists():continue
            if usage[k]['state']!='received':raise RuntimeError('Ambiguous prior request; review ledger, do not rebill '+k)
        else:
            # Conservative reservation at one credit per character, including rounding.
            estimate=len(payload['text'])
            if spent+estimate>budget:raise RuntimeError('Credit cap reached before '+k)
            usage[k]={'state':'pending','cost':estimate,'fingerprint':fingerprint};save(ledger,usage)
            req=urllib.request.Request('https://api.elevenlabs.io/v1/text-to-speech/'+p['voice_id']+'?output_format=mp3_44100_128',data=json.dumps(payload).encode(),headers={'xi-api-key':secret,'Content-Type':'application/json'})
            try:
                with urllib.request.urlopen(req,timeout=90) as res:raw=res.read();cost=float(res.headers.get('character-cost') or estimate)
            except urllib.error.HTTPError as e:
                usage[k]['state']='http_error';usage[k]['http_status']=e.code;save(ledger,usage)
                print('API_ERROR',e.code,e.read().decode()[:500],flush=True);raise SystemExit(1)
            (OUT/(k+'.mp3')).write_bytes(raw);usage[k].update(state='received',cost=cost);save(ledger,usage);spent+=cost
        data,rate=sf.read(OUT/(k+'.mp3'),dtype='float32',always_2d=True)
        mono=data.mean(axis=1);mono-=mono.mean();n=min(int(rate*.008),len(mono)//8)
        mono[:n]*=np.linspace(0,1,n);mono[-n:]*=np.linspace(1,0,n)
        sf.write(target,mono,rate,subtype='PCM_16')
        usage[k].update(state='complete',seconds=len(data)/rate);save(ledger,usage)
        print('GENERATED',k,'seconds',round(len(data)/rate,2),'batch_credits',spent,flush=True)
    print('DIALOGUE59_DONE',len(rows),'clips',spent,'credits',flush=True)
if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--generate',action='store_true');ap.add_argument('--budget',type=int,default=3851);ap.add_argument('--manifest',type=Path);a=ap.parse_args()
    path=a.manifest or starter();rows=json.loads(path.read_text(encoding='utf-8'))['lines']
    print('Clips',len(rows),'characters',sum(len(normalize(r['text'])) for r in rows),flush=True)
    if a.generate:generate(path,a.budget)

