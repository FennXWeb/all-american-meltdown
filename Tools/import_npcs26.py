from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV26').replace('models_v5_manifest','models_v26_import_manifest'),'npc26_import','exec'),{'__name__':'__main__'})
u.log('AAM_NPCS26_IMPORTED')
