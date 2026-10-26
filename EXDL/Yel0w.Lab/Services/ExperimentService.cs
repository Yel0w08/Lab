using System.Diagnostics;
using System.IO.Compression;
using Yel0w.lab.Models;
using Microsoft.Data.Sqlite;

namespace Yel0w.lab.Services;

public class ExperimentService
{
    private const string DbUrl =
        "https://raw.githubusercontent.com/Yel0w08/Lab/main/LabManager.db";

    private const string StorageBaseUrl =
        "https://github.com/Yel0w08/storage.archive.data/raw/refs/heads/main/Yel0w.Lab/";

    private static string ExperimentsRoot =>
        Path.Combine(FileSystem.AppDataDirectory, "Experiments");

    private string? _cachedDbPath;
    private List<Experiment>? _cachedExperiments;
    private DateTime _lastRefresh = DateTime.MinValue;

    private static readonly TimeSpan CacheDuration = TimeSpan.FromMinutes(5);

    private async Task<string?> EnsureDatabaseAsync()
    {
        if (_cachedDbPath is not null && File.Exists(_cachedDbPath))
            return _cachedDbPath;

        var dbPath = Path.Combine(FileSystem.CacheDirectory, "LabManager.db");

        try
        {
            using var http = new HttpClient();
            var bytes = await http.GetByteArrayAsync(DbUrl);
            await File.WriteAllBytesAsync(dbPath, bytes);
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Failed to download database: {ex.Message}");

            if (File.Exists(dbPath))
                return dbPath;

            return null;
        }

        _cachedDbPath = dbPath;
        return dbPath;
    }

    public async Task<List<Experiment>> GetExperimentsAsync(bool forceRefresh = false)
    {
        if (!forceRefresh && _cachedExperiments is not null && DateTime.UtcNow - _lastRefresh < CacheDuration)
            return _cachedExperiments;

        var dbPath = await EnsureDatabaseAsync();

        if (dbPath is null)
            return [];

        var experiments = new List<Experiment>();

        try
        {
            using var connection = new SqliteConnection($"Data Source={dbPath}");
            connection.Open();

            var command = connection.CreateCommand();
            command.CommandText = """
                SELECT
                    Id, Name, Description, Language, Framework, Engine,
                    Status, ProjectPath, Tags, Favorite, Downloadable,
                    Notes, CreatedAt, LastModified
                FROM Experiments
                ORDER BY Favorite DESC, LastModified DESC;
                """;

            using var reader = command.ExecuteReader();

            while (reader.Read())
            {
                experiments.Add(new Experiment
                {
                    Id = reader.GetInt32(0),
                    Name = reader.GetString(1),
                    Description = reader.IsDBNull(2) ? "" : reader.GetString(2),
                    Language = reader.IsDBNull(3) ? "" : reader.GetString(3),
                    Framework = reader.IsDBNull(4) ? "" : reader.GetString(4),
                    Engine = reader.IsDBNull(5) ? "" : reader.GetString(5),
                    Status = reader.IsDBNull(6) ? "" : reader.GetString(6),
                    ProjectPath = reader.IsDBNull(7) ? "" : reader.GetString(7),
                    Tags = reader.IsDBNull(8) ? "" : reader.GetString(8),
                    Favorite = !reader.IsDBNull(9) && reader.GetBoolean(9),
                    Downloadable = !reader.IsDBNull(10) && reader.GetBoolean(10),
                    Notes = reader.IsDBNull(11) ? "" : reader.GetString(11),
                    CreatedAt = reader.IsDBNull(12) ? DateTime.MinValue : reader.GetDateTime(12),
                    LastModified = reader.IsDBNull(13) ? DateTime.MinValue : reader.GetDateTime(13)
                });
            }
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Failed to read experiments: {ex.Message}");
            return _cachedExperiments ?? [];
        }

        _cachedExperiments = experiments;
        _lastRefresh = DateTime.UtcNow;

        return experiments;
    }

    public async Task<Experiment?> GetExperimentByIdAsync(int id)
    {
        var cached = _cachedExperiments?.FirstOrDefault(e => e.Id == id);

        if (cached is not null)
            return cached;

        var list = await GetExperimentsAsync();
        return list.FirstOrDefault(e => e.Id == id);
    }

    public bool IsDownloaded(string projectPath)
    {
        if (string.IsNullOrWhiteSpace(projectPath))
            return false;

        var dir = Path.Combine(ExperimentsRoot, projectPath);
        var batPath = Path.Combine(dir, "run.bat");

        return File.Exists(batPath);
    }

    public string GetExperimentDir(string projectPath)
    {
        return Path.Combine(ExperimentsRoot, projectPath);
    }

    public async Task DownloadExperimentAsync(string projectPath, Action<double>? onProgress = null)
    {
        if (string.IsNullOrWhiteSpace(projectPath))
            throw new ArgumentException("Project path is empty.");

        var zipUrl = $"{StorageBaseUrl}Exp/Bin/win/{projectPath}.zip";
        var tempZip = Path.Combine(FileSystem.CacheDirectory, $"{projectPath}.zip");
        var targetDir = Path.Combine(ExperimentsRoot, projectPath);

        using var http = new HttpClient();

        using var response = await http.GetAsync(zipUrl, HttpCompletionOption.ResponseHeadersRead);
        response.EnsureSuccessStatusCode();

        var totalBytes = response.Content.Headers.ContentLength ?? -1L;
        var totalBytesRead = 0L;

        await using var stream = await response.Content.ReadAsStreamAsync();
        await using var fileStream = File.Create(tempZip);

        var buffer = new byte[8192];
        int bytesRead;

        while ((bytesRead = await stream.ReadAsync(buffer)) > 0)
        {
            await fileStream.WriteAsync(buffer.AsMemory(0, bytesRead));
            totalBytesRead += bytesRead;

            if (totalBytes > 0)
                onProgress?.Invoke((double)totalBytesRead / totalBytes * 100);
        }

        await fileStream.DisposeAsync();

        if (Directory.Exists(targetDir))
            Directory.Delete(targetDir, true);

        ZipFile.ExtractToDirectory(tempZip, targetDir);
        File.Delete(tempZip);

        var nestedDir = Path.Combine(targetDir, projectPath);
        if (Directory.Exists(nestedDir))
        {
            foreach (var file in Directory.GetFiles(nestedDir))
                File.Move(file, Path.Combine(targetDir, Path.GetFileName(file)));

            foreach (var dir in Directory.GetDirectories(nestedDir))
            {
                var destDir = Path.Combine(targetDir, Path.GetFileName(dir));
                Directory.Move(dir, destDir);
            }

            Directory.Delete(nestedDir, true);
        }

        onProgress?.Invoke(100);
    }

    public static void RunExperiment(string projectPath)
    {
        var dir = Path.Combine(ExperimentsRoot, projectPath);
        var batPath = Path.Combine(dir, "run.bat");

        if (!File.Exists(batPath))
            throw new FileNotFoundException($"run.bat not found in {dir}");

        var psi = new ProcessStartInfo
        {
            FileName = batPath,
            WorkingDirectory = dir,
            UseShellExecute = true
        };

        Process.Start(psi);
    }
}
