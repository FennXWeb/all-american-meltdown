"""Import the 0.2 assets without reimporting an artist's original audio library."""
from pathlib import Path
import runpy
import unreal

root = Path(unreal.Paths.project_dir()).resolve()
for filename in ("import_models_v2.py", "build_audio_v2.py", "build_loot_v2.py"):
    unreal.log("LW_V2_IMPORT: " + filename)
    runpy.run_path(str(root / "Tools" / filename), run_name="__main__")
unreal.log("LW_V2_IMPORT_DONE")
