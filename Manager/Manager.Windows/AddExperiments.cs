using Manager.Models;

namespace Manager;

public partial class AddExperiments : Form
{
    public Experiment Experiment { get; private set; }

    public AddExperiments() : this(new Experiment()) { }

    public AddExperiments(Experiment experiment)
    {
        InitializeComponent();

        Experiment = experiment;

        ExperminmentStatus.DataSource = Enum.GetValues<ExperimentStatus>();

        if (experiment.Id != 0)
        {
            LoadExperimentData();
        }
    }

    private void LoadExperimentData()
    {
        ExperminmentName.Text = Experiment.Name;
        ExperminmentDescritpion.Text = Experiment.Description;
        ExperminmentLanguage.Text = Experiment.Language;
        ExperminmentFramework.Text = Experiment.Framework;
        ExperminmentStatus.SelectedItem = Experiment.Status;
        textBox1.Text = string.Join(", ", Experiment.Tags);
        NotesTextBox.Text = Experiment.Notes;
        EngineTextBox.Text = Experiment.Engine;
        ProjectPathTextBox.Text = Experiment.ProjectPath;


        Text = "Edit Experiment";
    }

    private void SaveButton_Click(object sender, EventArgs e)
    {
        if (string.IsNullOrWhiteSpace(ExperminmentName.Text))
        {
            MessageBox.Show("Name is required.", "Validation",
                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        Experiment.Name = ExperminmentName.Text.Trim();
        Experiment.Description = ExperminmentDescritpion.Text.Trim();
        Experiment.Language = ExperminmentLanguage.Text.Trim();
        Experiment.Framework = ExperminmentFramework.Text.Trim();
        Experiment.Engine = EngineTextBox.Text.Trim();
        if (ExperminmentStatus.SelectedItem is ExperimentStatus status)
            Experiment.Status = status;
        Experiment.ProjectPath = ProjectPathTextBox.Text.Trim();
        Experiment.Tags = textBox1.Text
            .Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
            .ToList();
        Experiment.Notes = NotesTextBox.Text.Trim();
 

        DialogResult = DialogResult.OK;
        Close();
    }

    private void CancelButton_Click(object sender, EventArgs e)
    {
        DialogResult = DialogResult.Cancel;
        Close();
    }

    private void AddExperiments_Load(object sender, EventArgs e)
    {

    }
}
