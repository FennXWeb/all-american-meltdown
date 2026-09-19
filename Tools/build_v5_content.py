import runpy
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
for script in ['import_models_v5.py','build_audio_v2.py']:runpy.run_path(str(root/'Tools'/script),run_name='__main__')
u.log('LW_V5_CONTENT_COMPLETE')
