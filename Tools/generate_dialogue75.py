"""Two-worker, resumable ElevenLabs recording batch. Never retry an ambiguous bill.

The 15,000-credit limit applies to this update, independently of historical usage.
Reserve a full credit per character while requests are in flight (Flash normally
bills less), then account using the provider's character-cost response header.
"""
import concurrent.futures, hashlib, json, math, sys, threading, urllib.request, urllib.error
import dialogue59 as d

def generate():
    sys.path.insert(0,str(d.ROOT/'Saved/Audio42Deps'))
    import numpy as np
    import soundfile as sf
    rows=json.loads((d.OUT/'complete75.json').read_text())['lines']
    profiles={p['id']:p for p in d.registry()['profiles']}
    ledger=d.OUT/'usage.json';usage=json.loads(ledger.read_text())
    capfile=d.OUT/'budget75.json'
    if not capfile.exists():
        d.save(capfile,{'limit':15000,'baseline':sum(v['cost'] for v in usage.values()),'description':'User-authorized maximum for dialogue update 75'})
    cap=json.loads(capfile.read_text());assert cap['limit']==15000
    secret=d.key();lock=threading.Lock();stopped=threading.Event()
    def spent(): return sum(v['cost'] for v in usage.values())-cap['baseline']
    def worker(row):
        if stopped.is_set():return
        p=profiles[row['profile']];k=row['key'];target=d.OUT/row['file'];mp3=d.OUT/(k+'.mp3')
        assert k==d.line_key(p['id'],row['text'])
        payload={'text':d.normalize(row['text']),'model_id':p['model_id'],'voice_settings':p['voice_settings'],'seed':int(hashlib.md5(k.encode()).hexdigest()[:8],16)}
        fingerprint=hashlib.sha256(json.dumps([p['voice_id'],payload],sort_keys=True).encode()).hexdigest()
        with lock:
            prior=usage.get(k)
            if prior:
                if prior['fingerprint']!=fingerprint:raise RuntimeError('Immutable profile changed: '+k)
                if prior['state']=='complete' and target.exists():return
                if prior['state'] not in ('received','complete'):raise RuntimeError('Review ambiguous previous request before resuming: '+k)
            else:
                estimate=len(payload['text'])
                if spent()+estimate>cap['limit']:raise RuntimeError('15,000-credit cap reached; remaining clips left queued')
                usage[k]={'state':'pending','cost':estimate,'fingerprint':fingerprint,'batch':75};d.save(ledger,usage)
        if not prior:
            req=urllib.request.Request('https://api.elevenlabs.io/v1/text-to-speech/'+p['voice_id']+'?output_format=mp3_44100_128',data=json.dumps(payload).encode(),headers={'xi-api-key':secret,'Content-Type':'application/json'})
            try:
                with urllib.request.urlopen(req,timeout=120) as res:
                    raw=res.read();cost=float(res.headers.get('character-cost') or estimate);request_id=res.headers.get('request-id','')
            except urllib.error.HTTPError as e:
                with lock:
                    # A rejection is recorded for explicit review, never automatically retried.
                    usage[k].update(state='http_error',http_status=e.code);d.save(ledger,usage)
                print('API_ERROR',e.code,e.read().decode()[:500],flush=True);raise
            mp3.write_bytes(raw)
            with lock:
                usage[k].update(state='received',cost=cost,request_id=request_id);d.save(ledger,usage)
        data,rate=sf.read(mp3,dtype='float32',always_2d=True)
        mono=data.mean(axis=1);mono-=mono.mean()
        rms=float(np.sqrt(np.mean(mono*mono)));peak=float(np.max(np.abs(mono)))
        if len(mono)<rate*.2 or rms<.001 or not np.isfinite(mono).all():raise RuntimeError('Audio quality check failed: '+k)
        mono*=min(2.5,.10/max(rms,.001),.94/max(peak,.001))
        n=min(int(rate*.008),len(mono)//8);mono[:n]*=np.linspace(0,1,n);mono[-n:]*=np.linspace(1,0,n)
        sf.write(target,mono,rate,subtype='PCM_16')
        with lock:
            usage[k].update(state='complete',seconds=len(data)/rate,rms=rms);d.save(ledger,usage)
            print('GENERATED',k,'seconds',round(len(data)/rate,2),'update_credits',round(spent(),2),flush=True)
    # Generate a sample of each new voice before the rest of the script.
    rows.sort(key=lambda r:(r['text']!='The roads are quiet. Stay close, and watch the rooftops.',r['profile']))
    errors=[]
    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        jobs=[pool.submit(worker,r) for r in rows]
        for job in concurrent.futures.as_completed(jobs):
            try:job.result()
            except Exception as e:stopped.set();errors.append(str(e));print('STOPPED',str(e),flush=True)
    if errors:raise SystemExit(1)
    report={'clips':len(rows),'credits':spent(),'limit':cap['limit'],'seconds':sum(usage[r['key']]['seconds'] for r in rows)}
    d.save(d.ROOT/'Saved/Dialogue75Generated.json',report)
    print('DIALOGUE75_DONE',json.dumps(report),flush=True)
if __name__=='__main__':generate()
