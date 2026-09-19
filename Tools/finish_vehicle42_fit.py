import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV42').replace('models_v5_manifest','models_v42_manifest')
source=source.replace("records = manifest['assets']", "records = [r for r in manifest['assets'] if r['name'] in ['Cabin42_rv','Cabin42_bus','Cabin42_boxtruck','Vehicle42_supercar','Windshield42_supercar']]")
exec(compile(source,'vehicle42_fit_import','exec'),{'__name__':'__main__'})
exec(compile((root/'Tools/finish_vehicle42_lods.py').read_text(),'vehicle42_lods','exec'))
