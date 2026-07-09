using System.Text.Json;
using Manager.Models;

namespace Manager.Services;

public class SaveManager
{
    private readonly string _filePath;

    public ExperimentDatabase Database { get; private set; } = new();

    public SaveManager(string filePath)
    {
        _filePath = filePath;
    }


    public void Load()
    {
        if (!File.Exists(_filePath))
        {
            Database = new ExperimentDatabase();
            Save();
            return;
        }

        string json = File.ReadAllText(_filePath);

        Database = JsonSerializer.Deserialize<ExperimentDatabase>(json)
                   ?? new ExperimentDatabase();
    }


    public void Save()
    {
        string json = JsonSerializer.Serialize(
            Database,
            new JsonSerializerOptions
            {
                WriteIndented = true
            });

        File.WriteAllText(_filePath, json);
    }


    public void AddExperiment(Experiment experiment)
    {
        experiment.Id = GetNextId();
        experiment.CreatedAt = DateTime.Now;
        experiment.LastModified = DateTime.Now;

        Database.Experiments.Add(experiment);

        Save();
    }


    public void RemoveExperiment(int id)
    {
        var experiment = Database.Experiments
            .FirstOrDefault(x => x.Id == id);

        if (experiment != null)
        {
            Database.Experiments.Remove(experiment);
            Save();
        }
    }


    private int GetNextId()
    {
        if (Database.Experiments.Count == 0)
            return 1;

        return Database.Experiments.Max(x => x.Id) + 1;
    }
}

