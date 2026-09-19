"""Original synthesized paper/chip foley, no external recordings."""
from pathlib import Path
import wave,random,math,struct
root=Path(__file__).resolve().parents[1]/'ArtSource/Audio';root.mkdir(exist_ok=True);rng=random.Random(120)
for name,duration in [('CardDeal',.16),('CardShuffle',.65),('CardChips',.22)]:
 rate=22050;values=[];last=0
 for i in range(int(rate*duration)):
  t=i/rate;noise=rng.uniform(-1,1);high=noise-last;last=noise
  if name=='CardDeal':v=high*math.exp(-t*32)*.19+math.sin(t*1700)*math.exp(-t*65)*.12
  elif name=='CardShuffle':v=high*(.25+.75*max(0,math.sin(t*85)))*math.sin(math.pi*t/duration)**.5*.12
  else:v=(math.sin(t*math.tau*2100)+.4*math.sin(t*math.tau*3400))*math.exp(-t*35)*.24
  values.append(int(max(-1,min(1,v))*32767))
 with wave.open(str(root/('S_'+name+'.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes(struct.pack('<'+'h'*len(values),*values))
