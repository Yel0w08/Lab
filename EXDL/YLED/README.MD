# YLED

**Yel0w's Lab Experiment Donloader** — a desktop experiment manager and launcher
built with **C++ / Qt 6 (Widgets)**.

YLED is a native port of the `Yel0w.Lab` app. It keeps the exact same
functionality, but ships as a single native desktop binary instead of a
MAUI/Blazor app.

## Features

- **Experiments** — fetches the Lab database (`LabManager.db`, SQLite) from
  GitHub, caches it locally for 5 minutes, and lists every experiment with its
  description, language / framework / engine, status badge and ★ favorite badge.
  Each tile has a small hand-drawn **download icon button** (no emoji/assets) that
  downloads the package straight from the list, showing inline progress and
  turning into a **run** button once it is downloaded.
- **Manifest driven** — every experiment ships a `YLEDMETA.yaml` that declares
  its per-platform **run** / **init** commands and where to download it from
  (`link-tgz` tarball or `link-git-dir` GitHub folder). YLED reads it, downloads
  accordingly, runs `init`, then launches `run`.
- **Experiment details** — full description, tags, notes and dates, plus:
  - **Download** the package (tarball / Git folder / legacy zip) with a live
    progress bar,
  - automatic **extraction** into the local experiments folder,
  - the resolved **run / init / download** commands shown read-only,
  - **Run** (`▶`) which launches the package once downloaded.
  - experiments that are not downloadable show *"Not available for download"*.
- **Settings** — **Clear Cache**, **Refresh Database**, **Refresh Manifests** and
  About info.

## Theme

A sharp, square, professional **black / white / yellow** UI:

- pure black background, hairline `#202020` borders, **zero border radius**,
- white text with a restrained yellow accent (`#FFD400`) for actions and active
  navigation,
- a flat, calm layout — no animated background.

## YLEDMETA.yaml

Each experiment root carries a manifest. It is the single source of truth for
YLED and is fetched from the Lab repository
(`https://github.com/Yel0w08/Lab/tree/main/Experiments/<Name>/YLEDMETA.yaml`).
YLED falls back to the `Yel0w08/BINDT` repository, then to a legacy zip.

```yaml
# Platform keys: windows, linux, macos, ALL. Every section is optional.
init:
  windows: ""
  linux: "echo 'bash commands to init'"
  macos: ""

run:
  windows: "./run.bat"
  linux: "./run.sh"
  macos: "./run.sh"

dl:
  Info:
    USETGZ: false      # prefer the platform tarball in link-tgz
    USEGITDIR: true    # prefer the GitHub folder in link-git-dir
  link-tgz:
    windows: ""
    linux: "https://github.com/Yel0w08/BINDT/releases/download/LabexerimentsTGZ/N-Body-Linux.tar.gz"
    macos: ""
  link-git-dir:
    ALL: "https://github.com/Yel0w08/BINDT/tree/main/LAB/EXPERIMENTS/N-Body"
```

Resolution order for the download:

1. `USETGZ` + a non-empty platform `link-tgz` → stream the `.tar.gz` and extract it.
2. `USEGITDIR` + a non-empty `link-git-dir` → `git clone --depth 1 --filter=blob:none
   --sparse` + `sparse-checkout`, with a codeload tarball fallback.
3. The other link if only one is present.
4. Otherwise the legacy `Exp/Bin/win/<ProjectPath>.zip` from the archive repo.

After a successful download YLED writes the manifest into the experiment root
(so every experiment keeps its own `YLEDMETA.yaml`), runs the `init` command,
and drops a `.yled.ok` marker that `isDownloaded()` checks.

## Requirements

- Qt 6 (modules: `Widgets`, `Network`, `Sql`) — the SQLite driver ships with Qt.
- A C++17 compiler.
- For archive extraction: `tar` (or `7z`) on Linux / macOS, PowerShell on Windows.
- For Git-folder downloads: `git` (optional; falls back to a tarball download).
- On Linux / macOS, **Wine** (`wine`) is used to launch Windows-only packages
  (`run.bat`) when no native `run.sh` exists; it is optional but recommended.

## Build

With qmake:

```sh
mkdir build && cd build
qmake6 /path/to/YLED/YLED.pro
make -j4
./YLED
```

Or open `YLED.pro` in Qt Creator.

## Data / cache locations

- Database cache: `QStandardPaths::CacheLocation` (`LabManager.db`).
- Downloaded experiments: `QStandardPaths::AppDataLocation/Experiments`.
- Download archives / git clones: `QStandardPaths::CacheLocation`.

## Layout

```
YLED/
├── YLED.pro                 # qmake project
├── resources/
│   ├── app.qrc
│   └── style.qss            # sharp black / white / yellow theme
└── src/
    ├── main.cpp
    ├── Theme.h              # colour palette
    ├── Experiment.h         # data model
    ├── Manifest.*           # YLEDMETA.yaml parser
    ├── ExperimentService.*  # DB fetch, manifests, download, extract, run
    ├── ExperimentTile.*     # experiment card (download/run icon)
    ├── NavBar.*
    ├── PageExperiments.*
    ├── PageExperimentDetail.*
    ├── PageSettings.*
    └── MainWindow.*
```

## Notes

- The manifest parser is intentionally tolerant of uneven indentation: values are
  located by key name and platform rather than by a strict YAML path.
- Platform keys accept aliases (`win`, `unix`, `mac`, `osx`, `darwin`).
- A successful download writes a `.yled.ok` marker; `isDownloaded()` also accepts
  a pre-existing `run.sh` / `run.bat` for manually installed experiments.
