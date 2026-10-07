"""Repair the border palette without reimporting meshes or changing menu art."""
from pathlib import Path
import unreal as u
script=Path(u.Paths.project_dir())/'Tools/import_update51.py'
materials=script.read_text().split("source=(root/")[0]
exec(compile(materials,str(script),'exec'))
u.log('BORDER68_MATERIALS_PASS')
u.SystemLibrary.quit_editor()
