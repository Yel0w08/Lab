namespace Yel0w.lab.Models;

public class Experiment
{
    public int Id { get; set; }

    public string Name { get; set; } = "";

    public string Description { get; set; } = "";

    public string Language { get; set; } = "";

    public string Framework { get; set; } = "";

    public string Engine { get; set; } = "";

    public string Status { get; set; } = "";

    public string ProjectPath { get; set; } = "";

    public string Tags { get; set; } = "";

    public bool Favorite { get; set; }

    public bool Downloadable { get; set; }

    public string Notes { get; set; } = "";

    public DateTime CreatedAt { get; set; }

    public DateTime LastModified { get; set; }
}