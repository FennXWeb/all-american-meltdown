from pathlib import Path
for script in ['import_models63.py','finish_models63.py']:
 exec(compile(Path('X:/LethalWorld/Tools',script).read_text(),script,'exec'))

import unreal
unreal.SystemLibrary.quit_editor()
