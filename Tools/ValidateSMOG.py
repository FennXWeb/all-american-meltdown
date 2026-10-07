"""Validate the launcher contract and optionally create/test its portable ZIP."""
from pathlib import Path, PurePosixPath
import argparse, hashlib, json, re, stat, struct, zipfile
import xml.etree.ElementTree as ET
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
FILES = ('smog_icon.ico', 'smog_logo.png', 'smog_header.png', 'smog_meta.xml', 'smog_launch.bat')
EXE = 'LethalWorld/Binaries/Win64/LethalWorld-Win64-Shipping.exe'

def contract(folder):
    for name in FILES:
        assert (folder/name).is_file(), f'Missing {name}'
    data = (folder/'smog_meta.xml').read_bytes()
    assert b'<!DOCTYPE' not in data.upper() and b'<!ENTITY' not in data.upper()
    xml = ET.fromstring(data)
    assert xml.tag == 'smog'
    for key in ('title','description','developer','genre','version','accent','release/asset'):
        assert xml.findtext(key), f'Missing metadata field: {key}'
    assert re.fullmatch(r'#?[0-9a-fA-F]{6}', xml.findtext('accent'))
    for name, size in [('smog_header.png',(2400,1000)),('smog_logo.png',(1200,400))]:
        path=folder/name
        assert path.stat().st_size < 8*1024*1024
        assert not path.read_bytes().startswith(b'version https://git-lfs')
        with Image.open(path) as im:
            assert im.size==size, (name,im.size)
            if 'logo' in name:
                assert im.mode=='RGBA' and im.getchannel('A').getextrema()==(0,255), 'Logo must retain transparency'
    with Image.open(folder/'smog_icon.ico') as im:
        assert {(16,16),(32,32),(48,48),(256,256)} <= im.ico.sizes()
    launcher=(folder/'smog_launch.bat').read_text()
    assert '-UserDir="%LOCALAPPDATA%\\AllAmericanMeltdown"' in launcher
    assert not re.search(r'^\s*start\s',launcher,re.I|re.M)
    return xml.findtext('version')

def package(stage, output):
    version=contract(stage)
    assert (stage/EXE).is_file(), 'Shipping executable missing'
    files=sorted(p for p in stage.rglob('*') if p.is_file())
    assert len(files)<=100000
    total=sum(p.stat().st_size for p in files)
    assert total<30*1024**3, 'SMOG extracted-size limit exceeded'
    forbidden=re.compile(r'^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\.|$)',re.I)
    for p in files:
        rel=p.relative_to(stage)
        assert not p.is_symlink()
        assert all(not forbidden.match(s) and ':' not in s and s not in ('.','..') for s in rel.parts)
        assert p.suffix.lower() not in ('.sav','.pdb','.key','.pem'), f'Unexpected packaged file: {rel}'
        assert 'SaveGames' not in rel.parts, 'Do not distribute local saves'
    output.parent.mkdir(parents=True,exist_ok=True)
    assert not output.exists(), f'Refusing to overwrite {output}'
    with zipfile.ZipFile(output,'w',zipfile.ZIP_DEFLATED,compresslevel=9,allowZip64=True) as z:
        for p in files: z.write(p,p.relative_to(stage).as_posix())
    with zipfile.ZipFile(output) as z:
        assert z.testzip() is None, 'ZIP CRC validation failed'
        assert all(n in z.namelist() for n in FILES)
        assert EXE in z.namelist()
    assert output.stat().st_size<2*1024**3, 'GitHub release asset exceeds 2 GiB'
    with output.open('rb') as artifact:
        digest=hashlib.file_digest(artifact,'sha256').hexdigest()
    output.with_suffix('.zip.sha256').write_text(f'{digest}  {output.name}\n',encoding='utf-8')
    info={'version':version,'asset':output.name,'bytes':output.stat().st_size,'extracted_bytes':total,'files':len(files),'sha256':digest}
    build_info=stage/'build-info.json'
    if build_info.is_file():
        info['build']=json.loads(build_info.read_text(encoding='utf-8-sig'))
    output.with_suffix('.manifest.json').write_text(json.dumps(info,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(info,indent=2))

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--stage',type=Path);ap.add_argument('--zip',type=Path);args=ap.parse_args()
    print('SMOG_ROOT_PASS',contract(ROOT))
    if args.stage:
        assert args.zip
        package(args.stage,args.zip)
