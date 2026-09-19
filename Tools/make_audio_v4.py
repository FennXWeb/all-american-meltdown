from pathlib import Path
import numpy as np,wave
root=Path(__file__).resolve().parents[1]/'ArtSource/Audio';sr=44100
for voice in range(3):
 rng=np.random.default_rng(4040+voice);sound=np.zeros(int(sr*2.4))
 for i in range(9):
  length=int(sr*rng.uniform(.10,.19));t=np.arange(length)/sr;base=100+voice*55+rng.uniform(-15,35)
  f0=base*(1+.06*np.sin(t*18));phase=2*np.pi*np.cumsum(f0)/sr
  signal=sum(np.sin(phase*k)*np.exp(-((base*k-(600+voice*170+i%3*120))/400)**2)/k for k in range(1,18))
  signal+=rng.normal(0,.035,length);signal*=np.sin(np.pi*np.arange(length)/length)**1.6
  start=int(sr*(.1+i*.24));sound[start:start+length]+=signal
 sound/=max(.1,np.max(np.abs(sound)));pcm=(sound*21000).astype('<i2')
 with wave.open(str(root/f'S_Speech{voice}.wav'),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes(pcm.tobytes())
print('Three original gibberish voices generated')
