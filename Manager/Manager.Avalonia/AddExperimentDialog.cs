using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;
using Manager.Models;

namespace Manager;

public class AddExperimentDialog : Window
{
    public Experiment Experiment { get; private set; }

    private readonly TextBox _nameTextBox;
    private readonly TextBox _descriptionTextBox;
    private readonly TextBox _languageTextBox;
    private readonly TextBox _frameworkTextBox;
    private readonly TextBox _engineTextBox;
    private readonly ComboBox _statusCombo;
    private readonly TextBox _tagsTextBox;
    private readonly TextBox _notesTextBox;
    private readonly TextBox _projectPathTextBox;

    public AddExperimentDialog()
    {
        Experiment = new Experiment();
        Title = "Add Experiment";
        (_nameTextBox, _descriptionTextBox, _languageTextBox, _frameworkTextBox,
         _engineTextBox, _statusCombo, _tagsTextBox, _notesTextBox, _projectPathTextBox) = BuildUI();
    }

    public AddExperimentDialog(Experiment experiment)
    {
        Experiment = experiment;
        Title = "Edit Experiment";
        (_nameTextBox, _descriptionTextBox, _languageTextBox, _frameworkTextBox,
         _engineTextBox, _statusCombo, _tagsTextBox, _notesTextBox, _projectPathTextBox) = BuildUI();
        LoadExperimentData();
    }

    private (TextBox, TextBox, TextBox, TextBox, TextBox, ComboBox, TextBox, TextBox, TextBox) BuildUI()
    {
        Width = 320;
        Height = 480;
        CanResize = false;
        WindowStartupLocation = WindowStartupLocation.CenterOwner;

        var nameBox = new TextBox();
        var descBox = new TextBox();
        var langBox = new TextBox();
        var fwBox = new TextBox();
        var engineBox = new TextBox();
        var statusCombo = new ComboBox { ItemsSource = Enum.GetValues<ExperimentStatus>(), SelectedIndex = 0 };
        var tagsBox = new TextBox();
        var notesBox = new TextBox();
        var pathBox = new TextBox();

        var saveButton = new Button { Content = "Save", Width = 80, HorizontalAlignment = HorizontalAlignment.Center };
        var cancelButton = new Button { Content = "Cancel", Width = 80, HorizontalAlignment = HorizontalAlignment.Center };
        saveButton.Click += (_, _) => Save();
        cancelButton.Click += (_, _) => Close();

        var fields = new StackPanel
        {
            Spacing = 8,
            Margin = new Avalonia.Thickness(15),
            Children =
            {
                MakeRow("Name", nameBox),
                MakeRow("Description", descBox),
                MakeRow("Language", langBox),
                MakeRow("Framework", fwBox),
                MakeRow("Engine", engineBox),
                MakeRow("Status", statusCombo),
                MakeRow("Tags", tagsBox),
                MakeRow("Notes", notesBox),
                MakeRow("Project Path", pathBox),
            }
        };

        var buttonBar = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 10,
            HorizontalAlignment = HorizontalAlignment.Center,
            Margin = new Avalonia.Thickness(0, 10),
            Children = { saveButton, cancelButton }
        };

        Content = new StackPanel
        {
            Children = { fields, buttonBar }
        };

        return (nameBox, descBox, langBox, fwBox, engineBox, statusCombo, tagsBox, notesBox, pathBox);
    }

    private static StackPanel MakeRow(string label, Control control)
    {
        return new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 8,
            Children =
            {
                new TextBlock { Text = label, Width = 90, VerticalAlignment = VerticalAlignment.Center, FontWeight = FontWeight.SemiBold },
                control
            }
        };
    }

    private void LoadExperimentData()
    {
        _nameTextBox.Text = Experiment.Name;
        _descriptionTextBox.Text = Experiment.Description;
        _languageTextBox.Text = Experiment.Language;
        _frameworkTextBox.Text = Experiment.Framework;
        _engineTextBox.Text = Experiment.Engine;
        _statusCombo.SelectedItem = Experiment.Status;
        _tagsTextBox.Text = string.Join(", ", Experiment.Tags);
        _notesTextBox.Text = Experiment.Notes;
        _projectPathTextBox.Text = Experiment.ProjectPath;
    }

    private async void Save()
    {
        if (string.IsNullOrWhiteSpace(_nameTextBox.Text))
        {
            var dialog = new MessageDialog("Validation", "Name is required.");
            await dialog.ShowDialog(this);
            return;
        }

        Experiment.Name = _nameTextBox.Text.Trim();
        Experiment.Description = _descriptionTextBox.Text?.Trim() ?? "";
        Experiment.Language = _languageTextBox.Text?.Trim() ?? "";
        Experiment.Framework = _frameworkTextBox.Text?.Trim() ?? "";
        Experiment.Engine = _engineTextBox.Text?.Trim() ?? "";
        if (_statusCombo.SelectedItem is ExperimentStatus status)
            Experiment.Status = status;
        Experiment.Tags = (_tagsTextBox.Text ?? "")
            .Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
            .ToList();
        Experiment.Notes = _notesTextBox.Text?.Trim() ?? "";
        Experiment.ProjectPath = _projectPathTextBox.Text?.Trim() ?? "";

        Close(true);
    }
}
