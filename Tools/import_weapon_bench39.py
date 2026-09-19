from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
script=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'Workbench39'").replace('models_v5_manifest','bench39_manifest')
exec(compile(script,'bench39_import','exec'),{'__name__':'__main__'})
u.log('WEAPON_BENCH39_IMPORTED')
