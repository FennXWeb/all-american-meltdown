"""Generate NPC voice arrays through the existing metered ElevenLabs pipeline."""
import argparse,json
import vehicle_audio42 as pipeline

pipeline.OUT=pipeline.ROOT/'ArtSource/AudioV44'
pipeline.OUT.mkdir(parents=True,exist_ok=True)
pipeline.ROWS=[]
types={
 'Zombie':'undead zombie, wet raspy throat, breathy decayed groans',
 'DogGrowl':'rabid feral dog, canine barking and snarling, animal only',
 'MannequinMove':'haunted plastic mannequin, dry joint creaks and unsettling hollow plastic clicks, no human voice',
 'MooseRoar':'mutated bull moose, resonant ungulate bellows and rough snorts',
 'TitanRoar':'ten foot muscular undead brute, deep chest growls and strained guttural roars',
 'DeathclawRoar':'large fictional reptilian predator, layered reptile hiss and animal throat growl',
 'ScorpionHiss':'giant scorpion, chitin mandible rattles and dry arthropod hissing, no mammal voice',
 'KarenShriek':'female undead creature, ragged furious shrieks and throaty zombie groans, no words',
 'BehemothVoice44':'enormous thirty foot undead behemoth, powerful bass bellow with rough breathing',
 'ColossusVoice44':'colossal ninety foot undead giant, subterranean chest resonance and thunderous guttural bellow',
 'WorldEaterVoice44':'massive subterranean worm monster, wet cavernous throat rumble and alien rasping clicks'
}
variations=['short wary warning call','restless low growling phrase','sharp aggressive challenge','ragged strained vocal outburst','longer intimidating rumble ending in an abrupt snarl']
for slot,description in types.items():
 for i,variation in enumerate(variations):
  pipeline.add(f'{slot}_{i+1:02}',f'{description}. {variation}. One isolated creature sound for a horror game. Dry close recording, no reverb, no music, no background scene, no intelligible speech.',2.5)
  pipeline.ROWS[-1]['group']=slot
for gender in ['Male','Female']:
 for i,mood in enumerate(['calm greeting','curious question','friendly explanation','quiet disagreement','excited discovery','worried warning','confident answer','tired complaint','amused remark','angry challenge']):
  slot='Human'+gender+'44'
  pipeline.add(f'{slot}_{i+1:02}',f'One adult {gender.lower()} human voice speaking a brief {mood} in an entirely invented gibberish language. Expressive conversational syllables, natural breath and rhythm. Absolutely no English or real language words, no narration, no singing. Single speaker, dry close clean game dialogue recording, no music or background noise.',3)
  pipeline.ROWS[-1]['group']=slot
(pipeline.OUT/'manifest.json').write_text(json.dumps({'sounds':pipeline.ROWS,'estimated_credits':1975,'budget':2500},indent=2))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--generate',action='store_true');args=p.parse_args()
 print('NPC library: 75 sounds; estimated 1975 credits at last returned rate; cap 2500',flush=True)
 if args.generate:pipeline.generate(2500)
