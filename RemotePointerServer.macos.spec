# -*- mode: python ; coding: utf-8 -*-

block_cipher = None
version      = '3.0'

# find the SHA-1 hash of you Developer ID Application certificate
# for signing via `security find-identity -v -p codesigning` or use `None`
codesign_identity = '4B7092469383AAFE294DA4B2B0CCB1BB0050DF72'

a = Analysis(
    ['RemotePointerServer.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('res/*.png', 'res/'),
        ('res/pointers/*', 'res/pointers/'),
        ('lang/*.qm', 'lang/'),
    ],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    contents_directory='.',
    exclude_binaries=True,
    name='RemotePointerServer',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=True,
    target_arch=None,
    codesign_identity=codesign_identity,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='RemotePointerServer',
)

app = BUNDLE(coll,
    name='RemotePointer.app',
    icon='res/icon.png',
    bundle_identifier='systems.sieber.remotespotlight',
    version=version,
    info_plist={}
)
