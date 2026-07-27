using Yel0w.lab.Models;
using Microsoft.Data.Sqlite;

namespace Yel0w.lab.Services;

public class ExperimentService
{
    private const string DbUrl =
        "https://raw.githubusercontent.com/Yel0w08/Lab/main/LabManager.db";

    public async Task<List<Experiment>> GetExperimentsAsync()
    {
        var dbPath = Path.Combine(FileSystem.CacheDirectory, "LabManager.db");

        try
        {
            using var http = new HttpClient();

            var bytes = await http.GetByteArrayAsync(DbUrl);
            await File.WriteAllBytesAsync(dbPath, bytes);
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Failed to download database: {ex.Message}");
            return [];
        }

        if (!File.Exists(dbPath))
        {
            return [];
        }

        var experiments = new List<Experiment>();

        try
        {
            using var connection = new SqliteConnection($"Data Source={dbPath}");
            connection.Open();

            var command = connection.CreateCommand();

            command.CommandText = """
                SELECT
                    Id,
                    Name,
                    Description,
                    Language,
                    Framework,
                    Engine,
                    Status,
                    ProjectPath,
                    Tags,
                    Favorite,
                    Notes,
                    CreatedAt,
                    LastModified
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
                    Notes = reader.IsDBNull(10) ? "" : reader.GetString(10),
                    CreatedAt = reader.IsDBNull(11) ? DateTime.MinValue : reader.GetDateTime(11),
                    LastModified = reader.IsDBNull(12) ? DateTime.MinValue : reader.GetDateTime(12)
                });
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Failed to read experiments: {ex.Message}");
            return [];
        }

        return experiments;
    }
}