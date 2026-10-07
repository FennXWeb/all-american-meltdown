"""Rebuild aviation VFX shaders without reimporting the mesh or audio library."""
from pathlib import Path
import unreal as u
source=(Path(u.Paths.project_dir())/'Tools/import_aviation84.py').read_text()
prefix=source[:source.index('for key,color')]
prefix='\n'.join(line for line in prefix.splitlines() if not line.startswith('t=u.AssetImportTask'))
exec(compile(prefix+'\n'+source[source.index('# Fixed-topology'):],'aviation84-vfx','exec'))
