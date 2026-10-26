#pragma once

#include <QString>
#include <QStringList>

// Parsed YLEDMETA.yaml for one experiment.
//
// The metadata is a small per-experiment manifest that lives in the experiment
// root (both in the source repo and locally), e.g.:
//
//   init:
//     Linux: "echo 'bash commands to init'"
//   run:
//     windows: "run.bat"
//     linux: "./run.sh"
//   dl:
//     Info:
//       USETGZ: false
//       USEGITDIR: true
//     link-tgz:
//       windows: ""
//       linux: "https://.../N-Body-Linux.tar.gz"
//     link-git-dir:
//       ALL: "https://github.com/Yel0w08/BINDT/tree/main/LAB/EXPERIMENTS/N-Body"
//
// The parser is intentionally tolerant of shallow/uneven indentation: values are
// located by key name (and platform) rather than by a strict path.
struct Manifest
{
    bool    valid = false;
    QString raw;              // original YAML text (kept so it can be written locally)

    // Commands, per platform.
    QString initWindows, initLinux, initMac;
    QString runWindows,  runLinux,  runMac;

    // Download plan.
    bool    useTgz = false;
    bool    useGitDir = false;
    QString tgzWindows, tgzLinux, tgzMac;
    QString gitDirWindows, gitDirLinux, gitDirMac, gitDirAll;

    static Manifest parse(const QString &yamlText);

    // Current platform: "windows" | "linux" | "macos".
    static QString platform();

    bool         hasDownload() const;
    QString      runCommand() const;   // for the current platform
    QString      initCommand() const;  // for the current platform
    QString      tgzUrl() const;       // for the current platform
    QString      gitDirUrl() const;    // for the current platform
};