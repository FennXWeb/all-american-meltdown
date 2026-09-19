import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
s=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV53').replace('models_v5_manifest','models_v53_manifest')
exec(compile(s,'retail53_import','exec'),{'__name__':'__main__'})
u.log('RETAIL53_IMPORTED');u.SystemLibrary.quit_editor()
