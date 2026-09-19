from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
exec(compile((root/'Tools/import_vehicle42.py').read_text(),'import_vehicle42','exec'))
exec(compile((root/'Tools/finish_vehicle42_lods.py').read_text(),'vehicle42_lods','exec'))
