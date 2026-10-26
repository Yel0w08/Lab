#pragma once

#include <QString>

/**
 * Locates and opens the shared LabManager.db SQLite database.
 * The path resolution mirrors the C# managers: walk up from the current
 * directory / executable / source tree until a folder containing `.git`
 * is found and use `<repo>/LabManager.db`.
 */
namespace DatabaseConfig {

/** Repository root (the folder holding `.git`), or an empty string if not found. */
QString repositoryRoot();

/** Absolute path of LabManager.db. */
QString databasePath();

/** Opens (and creates/upgrades if needed) the connection. Returns false on error. */
bool openConnection(QString *error = nullptr);

/** True when the connection is open and usable. */
bool isConnected();

QString lastError();

} // namespace DatabaseConfig
