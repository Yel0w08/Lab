using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Layout;
using Avalonia.Media;
using Manager.Models;
using Manager.Services;

namespace Manager;

public class MainForm : Window
{
    private readonly ExperimentService _experimentService;
    private List<Experiment> _allExperiments = [];
    private bool _isUpdatingGrid;

    private readonly DataGrid _grid;
    private readonly TextBox _searchTextBox;
    private readonly ComboBox _statusFilterCombo;
    private readonly CheckBox _favoritesOnlyCheck;
    private readonly TextBlock _experimentLabel;

    public MainForm()
    {
        Title = "Lab Manager";
        Width = 800;
        Height = 450;
        MinWidth = 600;
        MinHeight = 350;
        WindowStartupLocation = WindowStartupLocation.CenterScreen;

        _experimentService = DatabaseManager.CreateService();
        DatabaseManager.ImportFromJson("ExperimentsDB.json");

        _searchTextBox = new TextBox { Text = "Search experiments...", Width = 200 };
        var searchButton = new Button { Content = "Search", Width = 60 };
        _favoritesOnlyCheck = new CheckBox { Content = "Favorites Only" };
        _experimentLabel = new TextBlock { Text = "No Loaded Experiments", Foreground = Brushes.Gray, FontWeight = FontWeight.Bold, Margin = new Avalonia.Thickness(5, 0) };

        _statusFilterCombo = new ComboBox { Width = 120 };
        var statusItems = new List<string> { "All Statuses" };
        statusItems.AddRange(Enum.GetValues<ExperimentStatus>().Select(FormatStatus));
        _statusFilterCombo.ItemsSource = statusItems;
        _statusFilterCombo.SelectedIndex = 0;

        _grid = new DataGrid
        {
            AutoGenerateColumns = false,
            IsReadOnly = false,
            CanUserResizeColumns = true,
            CanUserSortColumns = true,
            GridLinesVisibility = DataGridGridLinesVisibility.Horizontal,
            SelectionMode = DataGridSelectionMode.Single
        };

        _grid.Columns.Add(new DataGridCheckBoxColumn { Header = "\u2606", Binding = new Avalonia.Data.Binding("Favorite"), Width = new DataGridLength(40) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Name", Binding = new Avalonia.Data.Binding("Name"), Width = new DataGridLength(200) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Status", Binding = new Avalonia.Data.Binding("Status"), Width = new DataGridLength(90) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Language", Binding = new Avalonia.Data.Binding("Language"), Width = new DataGridLength(80) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Framework", Binding = new Avalonia.Data.Binding("Framework"), Width = new DataGridLength(80) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Engine", Binding = new Avalonia.Data.Binding("Engine"), Width = new DataGridLength(80) });
        _grid.Columns.Add(new DataGridCheckBoxColumn { Header = "\u2B07", Binding = new Avalonia.Data.Binding("Downloadable"), Width = new DataGridLength(40) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Tags", Binding = new Avalonia.Data.Binding("TagsDisplay"), Width = new DataGridLength(100) });
        _grid.Columns.Add(new DataGridTextColumn { Header = "Modified", Binding = new Avalonia.Data.Binding("LastModifiedDisplay"), Width = new DataGridLength(120) });

        _grid.DoubleTapped += Grid_DoubleTapped;
        _grid.CellEditEnding += Grid_CellEditEnding;

        _searchTextBox.KeyDown += (_, e) =>
        {
            if (e.Key == Key.Enter) { ApplyFilters(); e.Handled = true; }
        };
        searchButton.Click += (_, _) => ApplyFilters();
        _statusFilterCombo.SelectionChanged += (_, _) => ApplyFilters();
        _favoritesOnlyCheck.IsCheckedChanged += (_, _) => ApplyFilters();

        var addButton = new Button { Content = "Add", Width = 60 };
        var favoriteButton = new Button { Content = "\u2606 Favorite", Width = 90 };
        var editButton = new Button { Content = "Edit", Width = 60 };
        var deleteButton = new Button { Content = "Del", Width = 60 };
        var refreshButton = new Button { Content = "Refresh", Width = 75 };

        addButton.Click += async (_, _) =>
        {
            var dialog = new AddExperimentDialog();
            var result = await dialog.ShowDialog<bool?>(this);
            if (result == true)
            {
                _experimentService.Add(dialog.Experiment);
                LoadExperiments();
            }
        };

        favoriteButton.Click += async (_, _) =>
        {
            var id = SelectedExperimentId;
            if (id is null)
            {
                await ShowMessage("Select an experiment to toggle favorite.", "No Selection");
                return;
            }
            var experiment = _experimentService.GetById(id.Value);
            if (experiment is null) return;
            experiment.Favorite = !experiment.Favorite;
            _experimentService.Update(experiment);
            LoadExperiments();
        };

        editButton.Click += async (_, _) =>
        {
            var id = SelectedExperimentId;
            if (id is null)
            {
                await ShowMessage("Select an experiment to edit.", "No Selection");
                return;
            }
            var experiment = _experimentService.GetById(id.Value);
            if (experiment is null)
            {
                await ShowMessage("Experiment not found.", "Error");
                return;
            }
            var dialog = new AddExperimentDialog(experiment);
            var result = await dialog.ShowDialog<bool?>(this);
            if (result == true)
            {
                _experimentService.Update(dialog.Experiment);
                LoadExperiments();
            }
        };

        deleteButton.Click += async (_, _) =>
        {
            var id = SelectedExperimentId;
            if (id is null)
            {
                await ShowMessage("Select an experiment to delete.", "No Selection");
                return;
            }
            var confirm = new ConfirmDialog("Confirm", "Delete this experiment?");
            await confirm.ShowDialog(this);
            if (confirm.Confirmed)
            {
                _experimentService.Delete(id.Value);
                LoadExperiments();
            }
        };

        refreshButton.Click += (_, _) => LoadExperiments();

        var toolbar = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 5,
            Margin = new Avalonia.Thickness(5),
            Children = { _searchTextBox, searchButton, _statusFilterCombo, _favoritesOnlyCheck }
        };

        var bottomBar = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 5,
            Margin = new Avalonia.Thickness(5),
            Children = { addButton, favoriteButton, editButton, deleteButton, refreshButton }
        };

        var mainPanel = new DockPanel();
        DockPanel.SetDock(toolbar, Dock.Top);
        DockPanel.SetDock(_experimentLabel, Dock.Top);
        DockPanel.SetDock(bottomBar, Dock.Bottom);
        mainPanel.Children.Add(toolbar);
        mainPanel.Children.Add(_experimentLabel);
        mainPanel.Children.Add(bottomBar);
        mainPanel.Children.Add(_grid);

        Content = mainPanel;

        LoadExperiments();
    }

    private void LoadExperiments()
    {
        _allExperiments = _experimentService.GetAll();
        ApplyFilters();
    }

    private void ApplyFilters()
    {
        _isUpdatingGrid = true;

        var filtered = _allExperiments.AsEnumerable();

        var searchText = _searchTextBox.Text?.Trim() ?? "";
        if (!string.IsNullOrEmpty(searchText) && searchText != "Search experiments...")
        {
            var lower = searchText.ToLower();
            filtered = filtered.Where(e =>
                e.Name.Contains(lower, StringComparison.OrdinalIgnoreCase) ||
                e.Description.Contains(lower, StringComparison.OrdinalIgnoreCase) ||
                e.Language.Contains(lower, StringComparison.OrdinalIgnoreCase) ||
                e.Framework.Contains(lower, StringComparison.OrdinalIgnoreCase) ||
                e.Notes.Contains(lower, StringComparison.OrdinalIgnoreCase) ||
                e.Tags.Any(t => t.Contains(lower, StringComparison.OrdinalIgnoreCase))
            );
        }

        if (_statusFilterCombo.SelectedIndex > 0)
        {
            var selectedStatus = (ExperimentStatus)(_statusFilterCombo.SelectedIndex - 1);
            filtered = filtered.Where(e => e.Status == selectedStatus);
        }

        if (_favoritesOnlyCheck.IsChecked == true)
        {
            filtered = filtered.Where(e => e.Favorite);
        }

        var results = filtered
            .OrderByDescending(e => e.Favorite)
            .ThenByDescending(e => e.LastModified)
            .Select(e => new ExperimentDisplay(e))
            .ToList();

        _grid.ItemsSource = results;

        _experimentLabel.Text = results.Count == 0
            ? "No Experiments Found"
            : $"{results.Count} Experiment(s)";

        _isUpdatingGrid = false;
    }

    private int? SelectedExperimentId
    {
        get
        {
            if (_grid.SelectedItem is ExperimentDisplay display)
                return display.Id;
            return null;
        }
    }

    private async void Grid_DoubleTapped(object? sender, TappedEventArgs e)
    {
        var id = SelectedExperimentId;
        if (id is null) return;
        var experiment = _experimentService.GetById(id.Value);
        if (experiment is null) return;
        var details = new ExperimentDetailsDialog(experiment);
        await details.ShowDialog(this);
    }

    private void Grid_CellEditEnding(object? sender, DataGridCellEditEndingEventArgs e)
    {
        if (_isUpdatingGrid) return;
        if (e.Column is not DataGridCheckBoxColumn) return;
        if (e.EditAction != DataGridEditAction.Commit) return;

        if (_grid.SelectedItem is not ExperimentDisplay display) return;

        var experiment = _experimentService.GetById(display.Id);
        if (experiment is null) return;

        if (e.EditingElement is CheckBox checkBox)
        {
            experiment.Favorite = checkBox.IsChecked == true;
            _experimentService.Update(experiment);
            LoadExperiments();
        }
    }

    private async Task ShowMessage(string text, string title)
    {
        var dialog = new MessageDialog(title, text);
        await dialog.ShowDialog(this);
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

public class ExperimentDisplay
{
    public int Id { get; set; }
    public bool Favorite { get; set; }
    public string Name { get; set; } = "";
    public string Status { get; set; } = "";
    public string Language { get; set; } = "";
    public string Framework { get; set; } = "";
    public string Engine { get; set; } = "";
    public string TagsDisplay { get; set; } = "";
    public bool Downloadable { get; set; }
    public string LastModifiedDisplay { get; set; } = "";

    public ExperimentDisplay(Experiment e)
    {
        Id = e.Id;
        Favorite = e.Favorite;
        Name = e.Name;
        Status = e.Status switch
        {
            ExperimentStatus.Planned => "Planned",
            ExperimentStatus.InProgress => "In Progress",
            ExperimentStatus.Finished => "Finished",
            ExperimentStatus.Archived => "Archived",
            ExperimentStatus.Abandoned => "Abandoned",
            _ => "Unknown"
        };
        Language = e.Language;
        Framework = e.Framework;
        Engine = e.Engine;
        TagsDisplay = string.Join(", ", e.Tags);
        Downloadable = e.Downloadable;
        LastModifiedDisplay = e.LastModified.ToString("g");
    }
}
