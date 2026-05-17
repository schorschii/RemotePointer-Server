# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['RemotePointerServer.py'],
    pathex=[],
    binaries=[],
    datas=[
        ('res/*.png', 'res/'),
        ('res/pointers/*.png', 'res/pointers/'),
        ('lang/*.qm', 'lang/'),
    ],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
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
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
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
