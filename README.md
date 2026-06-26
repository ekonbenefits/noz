# noz

`noz` removes Clipper EOF marker bytes (`0x1D`) from files in-place.

## Targets

- `noz.exe`: Windows 32-bit console executable.
- `noz_16.exe`: 16-bit DOS executable built with OpenWatcom.

## Source

- Main program: `src/noz.c`

## Usage

### Win32

```bash
noz.exe file1.dbf file2.dbf
```

### DOS16

Run from DOS command line:

```bash
noz_16.exe file1.dbf file2.dbf
```

## GitHub Actions

Workflow file: `.github/workflows/build-cross.yml`

On each push, pull request, or manual run, Actions produces:

- `dist/noz.exe`
- `dist/noz_16.exe`

Both files are uploaded as the `noz-windows-binaries` artifact.
