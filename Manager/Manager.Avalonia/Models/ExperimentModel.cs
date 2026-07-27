namespace Manager.Models;

public class Experiment
{
    public int Id { get; set; }

    public string Name { get; set; } = "";
    public string Description { get; set; } = "";

    public string Language { get; set; } = "";
    public string Framework { get; set; } = "";
    public string Engine { get; set; } = "";

    public ExperimentStatus Status { get; set; } = ExperimentStatus.Planned;

    public string ProjectPath { get; set; } = "";

    public List<string> Tags { get; set; } = [];
    public bool Favorite { get; set; } = false;

    public string Notes { get; set; } = "";

    public DateTime CreatedAt { get; set; } = DateTime.Now;
    public DateTime LastModified { get; set; } = DateTime.Now;
}

public enum ExperimentStatus
{
    Planned,
    InProgress,
    Finished,
    Archived,
    Abandoned
}
