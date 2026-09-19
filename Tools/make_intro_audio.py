"""Original synthesized sound design for the 18-second prologue shots. No speech samples."""
import math,random,wave,struct
from pathlib import Path
out=Path(__file__).resolve().parents[1]/'ArtSource/AudioV15';out.mkdir(exist_ok=True)
for index,name in enumerate(['IntroArchive','IntroUnrest','IntroAlarm','IntroBlast','IntroAftermath','IntroShelter']):
 rng=random.Random(2030+index);data=bytearray();low=0;phase=0
 for i in range(22050*18):
  t=i/22050;noise=rng.uniform(-1,1);low=low*.96+noise*.04;env=min(1,t/1.5,(18-t)/2);v=.12*math.sin(t*math.tau*55)+.07*math.sin(t*math.tau*82.41)+low*.4
  if index==0:v+=.035*math.sin(t*math.tau*(330 if int(t)%4<2 else 247))*max(0,1-(t%2))+.012*noise
  if index==1:v=low*1.4+.08*noise*(rng.random()<.04)+.05*math.sin(t*math.tau*38)
  if index==2:
   phase+=math.tau*(420+160*math.sin(t*.9))/22050;v+=.15*math.sin(phase)
  if index==3:
   age=max(0,t-1.5);v=.04*noise if t<1.5 else (.8*low+.22*noise)*math.exp(-age*.23)+.3*math.sin(t*math.tau*32)*math.exp(-age*.25)
  if index==4:v=.16*low+.06*math.sin(t*math.tau*65.4)+.02*noise*math.sin(t*.5)**12
  if index==5:v=.11*math.sin(t*math.tau*60)+low*.3+.045*math.sin(t*math.tau*98)
  data.extend(struct.pack('<h',int(max(-.95,min(.95,v*env))*32767)))
 with wave.open(str(out/('S_'+name+'.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(22050);w.writeframes(data)
print('AAM_INTRO_AUDIO_COMPLETE 6')
