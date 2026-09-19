"""Import seating and weather assets only; never package."""
from pathlib import Path
import runpy, math, random, wave, array
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
runpy.run_path(str(root/'Tools/import_models_v5.py'),run_name='__main__')
audio=root/'ArtSource/Audio';rng=random.Random(808);rate=22050
for name,length in [('Rain',4),('Thunder',3)]:
 data=array.array('h');low=0
 for i in range(rate*length):
  t=i/rate;n=rng.uniform(-1,1);low=low*.97+n*.03
  v=(n*.13+low*.7) if name=='Rain' else (low*3+n*.06)*math.exp(-t*1.3)*min(t*15,1)
  # Rain uses a short edge fade to avoid a loop click.
  v*=min(i/220,1,(rate*length-i)/220)
  data.append(int(max(-.9,min(.9,v))*32767))
 with wave.open(str(audio/f'S_{name}.wav'),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes(data.tobytes())
runpy.run_path(str(root/'Tools/build_audio_v2.py'),run_name='__main__')
lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
m=lib.load_asset('/Game/Materials/M_Sky');ml.delete_all_material_expressions(m)
p=ml.create_material_expression(m,u.MaterialExpressionVectorParameter);p.set_editor_property('parameter_name','SkyColor');p.set_editor_property('default_value',u.LinearColor(.13,.18,.145,1))
ml.connect_material_property(p,'',u.MaterialProperty.MP_EMISSIVE_COLOR);ml.recompile_material(m);lib.save_loaded_asset(m)
u.log('LW_V8_CONTENT_COMPLETE')
