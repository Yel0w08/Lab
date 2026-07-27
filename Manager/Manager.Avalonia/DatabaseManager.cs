using System.Text.Json;
using Manager.Data;
using Manager.Models;
using Manager.Services;

namespace Manager.Services;

public static class DatabaseManager
{
    public static ExperimentService CreateService()
    {
        return new ExperimentService();
    }

    public static void ImportFromJson(string jsonFilePath)
    {
        if (!File.Exists(jsonFilePath))
            return;

        var json = File.ReadAllText(jsonFilePath);
        var oldDb = JsonSerializer.Deserialize<ExperimentDatabase>(json);

        if (oldDb?.Experiments is null || oldDb.Experiments.Count == 0)
            return;

        using var service = new ExperimentService();

        foreach (var experiment in oldDb.Experiments)
        {
            experiment.Id = 0;
            service.Add(experiment);
        }
    }

    private class ExperimentDatabase
    {
        public int Version { get; set; } = 1;
        public List<Experiment> Experiments { get; set; } = [];
    }
}
