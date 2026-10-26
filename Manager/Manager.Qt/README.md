# Manager.Qt

A modern **Qt 6 Widgets** replacement for both `.NET` managers
(`Manager.Windows` WinForms and `Manager.Avalonia`). It talks to the **same
`LabManager.db`** at the repository root, so experiments created in any manager
show up everywhere.

```
┌───────────────────────────────────────────────────────────────────────┐
│  Lab Manager                                             ☾  + New      │
├──────────┬──────────────────────────────────────────┬─────────────────┤
│ Library  │  search…                                  │  Details        │
│  All     ├──────────────────────────────────────────┤  …              │
│  Favs    │  ★  Name / description      Status  Tags   │                 │
│ Status   │  ★  N-Body                   Done   cpp    │                 │
│  Idea    │     OpenSolarSystem          InProg …      │                 │
│  …       │     Nuclear Chain Reaction   Paused …      │                 │
├──────────┴──────────────────────────────────────────┴─────────────────┤
│ 3 experiments           Edit  Delete  Favourite  Downloadable  Open    │
└───────────────────────────────────────────────────────────────────────┘
```

## Features

- **One library, every manager** — reads and writes the shared
  `LabManager.db` (SQLite). The schema is kept compatible with EF Core:
  `Status` is stored as its enum name, `Tags` as a comma-joined list,
  `Favorite`/`Downloadable` as `0`/`1`, dates as `yyyy-MM-dd HH:mm:ss.zzz`.
- **Live search + filters** — instant search across name, description, tags,
  language, framework, engine and path; navigation rail filters for
  *All*, *Favourites* and every status.
- **Favourites pinned on top** — starred experiments always float to the top of
  any sort order.
- **Rich rows** — two-line name/description cells, coloured status pills, tag
  chips, a star toggle and a downloadable check, with hover/selection states.
- **Details panel** — a right-hand panel showing the full record with tag chips
  (`Ctrl+I`).
- **Full CRUD** — add and edit via a themed dialog, delete with confirmation,
  toggle favourite/downloadable, and open the project folder in the file manager.
- **Dark & light themes** — a "computation lab" look: deep-space navy with
  electric cyan + violet accents (or a paper-blueprint light mode), monospace
  technical labels, an engineering grid and corner-targeting brackets, all with
  sharp squared corners. Toggle at runtime; the choice is remembered via
  `QSettings("Yel0w", "LabManager")`.
- **Empty state** — a friendly overlay when the library (or a filter) has no
  results.
- **JSON bootstrap** — if the database is empty, experiments are imported from
  `ExperimentsDB.json` (repo root, `Manager.Windows/`, `Manager.Avalonia/`).

## Requirements

- Qt 6 (`Core`, `Gui`, `Widgets`, `Sql`) — e.g. `qt6-base-dev` on Debian/Ubuntu.
- A C++17 compiler (`g++` / `clang++` / MSVC).
- The SQLite driver ships with Qt (`libqt6sql6-sqlite`).

## Build

### Linux / macOS

```bash
cd Manager/Manager.Qt
./build.sh          # release build → bin/ManagerQt
./build.sh debug    # debug build
```

The script uses `qmake6` + `make` and writes a self-contained `bin/ManagerQt`
(the Qt runtime is linked normally, so no extra files are copied).

### Windows

```bat
cd Manager\Manager.Qt
build.bat
```

`build.bat` looks for `qmake6` (or `qmake`) on `PATH` and builds with
`nmake`/`jom`/`mingw32-make`, depending on your Qt kit.

### Manual (any platform)

```bash
mkdir build-release && cd build-release
qmake6 ../Manager.Qt.pro CONFIG+=release CONFIG-=debug
make -j
```

## Run

```bash
Manager/Manager.Qt/bin/ManagerQt
# or, from the repository root:
./managerqt.sh
```

`ManagerQt` locates the repository root by walking up from the current directory,
its own executable directory and the compile-time source path, then opens
`<repo>/LabManager.db`. It can therefore be launched from anywhere.

### Hidden helper flags

Useful for screenshots/CI checks:

```bash
ManagerQt --theme light
ManagerQt --theme dark --screenshot /tmp/manager.png   # save a PNG then exit
```

## Keyboard shortcuts

| Shortcut      | Action                          |
| ------------- | ------------------------------- |
| `Ctrl+N`      | New experiment                  |
| `Ctrl+F`      | Focus the search box            |
| `F5`          | Refresh from the database       |
| `Ctrl+I`      | Toggle the details panel        |
| `Ctrl+E` / `Enter` | Edit the selected experiment |
| `Delete`      | Delete the selected experiment  |
| `Ctrl+D`      | Toggle favourite                |
| `Ctrl+U`      | Toggle downloadable             |
| `Ctrl+O`      | Open the project folder         |

## Project layout

```
Manager.Qt.pro              qmake project (target: ManagerQt)
build.sh / build.bat        build helpers
src/
  main.cpp                  app bootstrap, theme, DB, CLI flags
  core/
    Experiment.h            model, status enum/names + tag helpers
    ExperimentService.*     Qt SQL CRUD + JSON import
    DatabaseConfig.*        repo-root/db resolution, schema + migration
    Theme.*                 palettes, generated QSS, font scaling
  models/
    ExperimentTableModel.*  QAbstractTableModel over experiments
    ExperimentFilterProxy.* live search/status/favourites filtering
  ui/
    MainWindow.*            sidebar, table, footer, shortcuts
    Blueprint.*             blueprint grid backdrop, corner-bracket frames
    DetailsPanel.*          right-hand detail view
    ExperimentDialog.*      add/edit form
    Dialogs.*               themed message/confirm dialogs
    Delegates.*             custom item painting (pills, chips, star…)
    Chips.*                 tag chip widgets + row painting helpers
    NavItem.h               navigation rail item
```

## Notes

- The database is **not** modified beyond normal CRUD: the app deliberately does
  not enable WAL mode, so `LabManager.db` stays byte-compatible with the .NET
  managers (SQLite is still the storage engine).
- `Downloadable` is auto-added to older schemas at startup if missing.
