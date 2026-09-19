"""Original deterministic synthesized placeholder SFX, replaceable in Audio Manager."""
from pathlib import Path
import numpy as np
import wave
root=Path(__file__).resolve().parents[1]/'ArtSource/Audio'
rng=np.random.default_rng(3003);sr=44100
for name,duration in [('DoorHinge',1.1),('GlassBreak',1.3),('FuelExplosion',3.2),('DogGrowl',1.4),('RaiderVoice',1.2),('MannequinMove',.85)]:
    t=np.arange(int(sr*duration))/sr;n=rng.normal(0,1,len(t));smooth=np.convolve(n,np.ones(65)/65,'same')
    if name=='FuelExplosion':s=(smooth*3+np.sin(2*np.pi*(60*t-6*t*t))*.3+n*np.exp(-t*18)*.3)*np.exp(-t*1.8)
    elif name=='GlassBreak':s=n*np.exp(-t*7)*.3+sum(np.sin(2*np.pi*f*t)*np.exp(-t*(5+i))*.08 for i,f in enumerate([1800,2701,3907,5700]))
    elif name=='DogGrowl':s=(np.sin(2*np.pi*(95*t+8*np.sin(t*11)))+smooth*4)*(.4+.6*np.sin(t*12)**2)*np.sin(np.pi*t/duration)**2*.4
    elif name=='RaiderVoice':s=(np.sin(2*np.pi*(170*t+3*np.sin(t*19)))+n*.2)*np.sin(t*13)**4*np.sin(np.pi*t/duration)*.4
    else:s=(np.sin(2*np.pi*(320*t+14*np.sin(t*4)))+n*.4)*np.sin(np.pi*t/duration)**2*.16+n*np.exp(-t*75)*.3
    s=np.tanh(s);pcm=(s*24000).astype('<i2')
    with wave.open(str(root/('S_'+name+'.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes(pcm.tobytes())
print('LW_V3_AUDIO_COMPLETE 6')
