"""ElevenLabs expansion sounds; resumable metered generation, keys stay local."""
import argparse,json
import vehicle_audio42 as p
p.OUT=p.ROOT/'ArtSource/AudioV57';p.OUT.mkdir(parents=True,exist_ok=True);p.ROWS=[]
engines={'apc':'eight wheel armored personnel carrier, deep heavy turbo diesel and gear whine','armoredtruck':'armored bank security truck, powerful muffled diesel and armored cabin resonance','technical':'battle worn pickup truck V8 with loud throaty exhaust','helicopter':'medium utility helicopter turboshaft and rhythmic rotor blade slap, no flyby'}
for vehicle,description in engines.items():
 for state,load in [('Idle','steady idle'),('Low','steady moderate loaded operation'),('High','steady maximum loaded operation')]:
  p.add('Vehicle42_'+vehicle+'_'+state,description+', '+load+'. Constant close stationary sound for a seamless engine loop, no music, no voices, no start or end transient.',4,True)
for group,desc in {'Hornet57':'giant hornet alien insect, chitin mandible clicks, angry insect hiss and buzzing, not a mammal','Bear57':'large aggressive grizzly bear, realistic deep throaty growls, snorts and explosive roars','Rogue57':'hostile humanoid robot, threatening electronic chirps, broken modem chatter and grinding servos, no human speech'}.items():
 for i,action in enumerate(['wary alert call','aggressive attack outburst','short injured response','restless searching call','long dying mechanical or animal decay']):
  p.add(group+'_'+str(i+1),desc+'. '+action+'. Single isolated dry game creature effect, no background, no music or words.',2.2);p.ROWS[-1]['group']=group
for name,description,duration,loop in [
 ('TurretCannon57','single heavy 25 mm autocannon shot, powerful sharp crack then short mechanical recoil clunk, dry isolated',.7,False),
 ('TurretMG57','single mounted heavy machine gun shot, hard ballistic crack and deep report, no burst',.5,False),
 ('TurretServo57','steady electric turret traverse servo motor whir, subtle gears, seamless',2.5,True),
 ('TurretReload57','heavy mounted machine gun feed cover opens, ammunition belt clatters into receiver, feed cover snaps shut',2.5,False),
 ('TurretEmpty57','mounted gun dry firing metal hammer clicks once',.5,False),
 ('HornetWings57','giant hornet wings fluttering rapidly, angry insect wing buzz, constant seamless hovering loop',3,True),
 ('HornetSting57','giant insect stinger strikes with a sharp wet chitin snap',.6,False),
 ('BearSwipe57','heavy bear paw swishes then strikes with a deep fleshy impact, no voice',.7,False),
 ('RogueDash57','fast powered robotic leg servos accelerating with a short electrical swoosh',.8,False),
 ('RogueShot57','single sharp futuristic electromagnetic rifle shot with a brief electrical tail',.6,False),
 ('RogueStep57','single heavy articulated robot foot contacts concrete, metallic clack with hydraulic hiss',.5,False),
 ('HeliStart57','utility helicopter turboshaft ignition, quick starter whine and rotor catching, isolated',4,False),
 ('HeliStop57','utility helicopter turboshaft winds down with gradually slowing rotor flutter, isolated',3,False),
 ('Vault57','heavy bank vault steel door unlocking with tumblers clanking and a weighty hinge opening',2,False),
 ('Target57','single distant steel shooting target impact with bright metallic ring',.7,False)]:p.add(name,description+'. No music or speech.',duration,loop)
(p.OUT/'manifest.json').write_text(json.dumps({'sounds':p.ROWS,'estimated_credits':sum(r['duration']*40 for r in p.ROWS)},indent=2))
a=argparse.ArgumentParser();a.add_argument('--generate',action='store_true');a.add_argument('--budget',type=int,default=6000);args=a.parse_args();print('SOUNDS',len(p.ROWS),'ESTIMATED_CREDITS',sum(r['duration']*40 for r in p.ROWS),flush=True)
if args.generate:p.generate(args.budget)
