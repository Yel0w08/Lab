using Manager.Models;

namespace Manager;

public partial class ExperimentDetails : Form
{
    public ExperimentDetails(Experiment experiment)
    {
        InitializeComponent();
        LoadExperiment(experiment);
    }

    private void LoadExperiment(Experiment exp)
    {
        NameValue.Text = exp.Name;
        StatusValue.Text = FormatStatus(exp.Status);
        LanguageValue.Text = exp.Language;
        FrameworkValue.Text = exp.Framework;
        EngineValue.Text = exp.Engine;
        DescriptionValue.Text = string.IsNullOrWhiteSpace(exp.Description) ? "-" : exp.Description;
        TagsValue.Text = exp.Tags.Count > 0 ? string.Join(", ", exp.Tags) : "-";
        NotesValue.Text = string.IsNullOrWhiteSpace(exp.Notes) ? "-" : exp.Notes;
        ProjectPathValue.Text = string.IsNullOrWhiteSpace(exp.ProjectPath) ? "-" : exp.ProjectPath;
        CreatedValue.Text = exp.CreatedAt.ToString("g");
        ModifiedValue.Text = exp.LastModified.ToString("g");
        FavoriteValue.Text = exp.Favorite ? "Yes" : "No";

        Text = $"Experiment #{exp.Id} - {exp.Name}";

        if (exp.Favorite)
        {
            FavoriteValue.ForeColor = Color.DarkOrange;
            FavoriteValue.Font = new Font(FavoriteValue.Font, FontStyle.Bold);
        }
    }

    private void CloseButton_Click(object sender, EventArgs e)
    {
        Close();
    }

    private static string FormatStatus(ExperimentStatus status) => status switch
    {
        ExperimentStatus.Planned => "Planned",
        ExperimentStatus.InProgress => "In Progress",
        ExperimentStatus.Finished => "Finished",
        ExperimentStatus.Archived => "Archived",
        ExperimentStatus.Abandoned => "Abandoned",
        _ => "Unknown"
    };
}
