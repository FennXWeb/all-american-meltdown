"""Focused RV correction using the canonical vehicle mesh generator."""
from pathlib import Path
source=Path(__file__).with_name('make_vehicle42_models.py').read_text()
source=source.replace("m.OUT=m.ROOT/'ArtSource/ModelsV42'", "m.OUT=m.ROOT/'ArtSource/ModelsV56'")
source=source.replace('for car,L,W,H,WB,N in SPECS:', 'for car,L,W,H,WB,N in [s for s in SPECS if s[0]=="rv"]:')
source=source.replace("m.export_all();", "m.OBJECTS={k:v for k,v in m.OBJECTS.items() if k in ('Vehicle42_rv','Cabin42_rv')};m.RECORDS={k:v for k,v in m.RECORDS.items() if k in m.OBJECTS};m.export_all();")
source=source.replace('models_v42_manifest','models_v56_manifest').replace('VehicleOverhaul42.blend','Camper56.blend')
exec(compile(source,str(Path(__file__).with_name('make_vehicle42_models.py')),'exec'))
