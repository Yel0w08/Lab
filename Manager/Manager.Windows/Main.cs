using Manager.Models;
using Manager.Services;

namespace Manager;

public partial class Main : Form
{
    private readonly ExperimentService _experimentService;
    private List<Experiment> _allExperiments = [];
    private bool _isUpdatingGrid;

    public Main()
    {
        InitializeComponent();

        _experimentService = DatabaseManager.CreateService();
        DatabaseManager.ImportFromJson("ExperimentsDB.json");

        SetupGridColumns();
        SetupFilters();
        LoadExperiments();
    }

    private void SetupGridColumns()
    {
        experimentGrid.Columns.Add(new DataGridViewCheckBoxColumn
        {
            Name = "Favorite",
            HeaderText = "\u2606",
            Width = 40,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Name",
            HeaderText = "Name",
            AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Status",
            HeaderText = "Status",
            Width = 90,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Language",
            HeaderText = "Language",
            Width = 80,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Framework",
            HeaderText = "Framework",
            Width = 80,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Engine",
            HeaderText = "Engine",
            Width = 80,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "Tags",
            HeaderText = "Tags",
            Width = 100,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });

        experimentGrid.Columns.Add(new DataGridViewTextBoxColumn
        {
            Name = "LastModified",
            HeaderText = "Modified",
            Width = 120,
            AutoSizeMode = DataGridViewAutoSizeColumnMode.None
        });
    }

    private void SetupFilters()
    {
        StatusFilterCombo.Items.Clear();
        StatusFilterCombo.Items.Add("All Statuses");
        foreach (var status in Enum.GetValues<ExperimentStatus>())
        {
            StatusFilterCombo.Items.Add(FormatStatus(status));
        }
        StatusFilterCombo.SelectedIndex = 0;
    }

    private void LoadExperiments()
    {
        _allExperiments = _experimentService.GetAll();
        ApplyFilters();
    }

    private void ApplyFilters()
    {
        _isUpdatingGrid = true;
        experimentGrid.Rows.Clear();

        var filtered = _allExperiments.AsEnumerable();

        var searchText = SearchTextBox.Text.Trim();
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

        if (StatusFilterCombo.SelectedIndex > 0)
        {
            var selectedStatus = (ExperimentStatus)(StatusFilterCombo.SelectedIndex - 1);
            filtered = filtered.Where(e => e.Status == selectedStatus);
        }

        if (FavoritesOnlyCheck.Checked)
        {
            filtered = filtered.Where(e => e.Favorite);
        }

        var results = filtered
            .OrderByDescending(e => e.Favorite)
            .ThenByDescending(e => e.LastModified)
            .ToList();

        foreach (var exp in results)
        {
            var row = experimentGrid.Rows.Add(
                exp.Favorite,
                exp.Name,
                FormatStatus(exp.Status),
                exp.Language,
                exp.Framework,
                exp.Engine,
                string.Join(", ", exp.Tags),
                exp.LastModified.ToString("g")
            );

            experimentGrid.Rows[row].Tag = exp.Id;

            if (exp.Favorite)
            {
                experimentGrid.Rows[row].DefaultCellStyle.BackColor = Color.LightYellow;
            }
        }

        ExperimentText.Text = results.Count == 0
            ? "No Experiments Found"
            : $"{results.Count} Experiment(s)";
        _isUpdatingGrid = false;
    }

    private int? SelectedExperimentId =>
        experimentGrid.SelectedRows.Count > 0
            ? experimentGrid.SelectedRows[0].Tag as int?
            : null;

    private void ExperimentGrid_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
    {
        if (e.RowIndex < 0) return;
        var id = SelectedExperimentId;
        if (id is null) return;

        var experiment = _experimentService.GetById(id.Value);
        if (experiment is null) return;

        using var details = new ExperimentDetails(experiment);
        details.ShowDialog();
    }

    private void ExperimentGrid_CellValueChanged(object sender, DataGridViewCellEventArgs e)
    {
        if (_isUpdatingGrid || e.RowIndex < 0) return;
        if (experimentGrid.Columns[e.ColumnIndex].Name != "Favorite") return;

        var id = experimentGrid.Rows[e.RowIndex].Tag as int?;
        if (id is null) return;

        var experiment = _experimentService.GetById(id.Value);
        if (experiment is null) return;

        if (experimentGrid.Rows[e.RowIndex].Cells["Favorite"].Value is bool fav)
            experiment.Favorite = fav;

        _experimentService.Update(experiment);
        LoadExperiments();
    }

    private void AddExperienceButton_Click(object sender, EventArgs e)
    {
        using var dialog = new AddExperiments();
        if (dialog.ShowDialog() == DialogResult.OK)
        {
            _experimentService.Add(dialog.Experiment);
            LoadExperiments();
        }
    }

    private void EditExperimentButton_Click(object sender, EventArgs e)
    {
        EditExperimentById(SelectedExperimentId);
    }

    private void EditExperimentById(int? id)
    {
        if (id is null)
        {
            MessageBox.Show("Select an experiment to edit.", "No Selection",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }

        var experiment = _experimentService.GetById(id.Value);
        if (experiment is null)
        {
            MessageBox.Show("Experiment not found.", "Error",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
            return;
        }

        using var dialog = new AddExperiments(experiment);
        if (dialog.ShowDialog() == DialogResult.OK)
        {
            _experimentService.Update(dialog.Experiment);
            LoadExperiments();
        }
    }

    private void ToggleFavoriteButton_Click(object sender, EventArgs e)
    {
        var id = SelectedExperimentId;
        if (id is null)
        {
            MessageBox.Show("Select an experiment to toggle favorite.", "No Selection",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }

        var experiment = _experimentService.GetById(id.Value);
        if (experiment is null) return;

        experiment.Favorite = !experiment.Favorite;
        _experimentService.Update(experiment);
        LoadExperiments();
    }

    private void DeleteExperimentButton_Click(object sender, EventArgs e)
    {
        var id = SelectedExperimentId;
        if (id is null)
        {
            MessageBox.Show("Select an experiment to delete.", "No Selection",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }

        var result = MessageBox.Show("Delete this experiment?", "Confirm",
            MessageBoxButtons.YesNo, MessageBoxIcon.Question);

        if (result == DialogResult.Yes)
        {
            _experimentService.Delete(id.Value);
            LoadExperiments();
        }
    }

    private void LoadExperimentButton_Click(object sender, EventArgs e)
    {
        LoadExperiments();
    }

    private void SearchButton_Click(object sender, EventArgs e)
    {
        ApplyFilters();
    }

    private void SearchTextBox_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Enter)
        {
            ApplyFilters();
            e.SuppressKeyPress = true;
        }
    }

    private void SearchTextBox_Enter(object sender, EventArgs e)
    {
        if (SearchTextBox.Text == "Search experiments...")
        {
            SearchTextBox.Text = "";
            SearchTextBox.ForeColor = SystemColors.WindowText;
        }
    }

    private void SearchTextBox_Leave(object sender, EventArgs e)
    {
        if (string.IsNullOrWhiteSpace(SearchTextBox.Text))
        {
            SearchTextBox.Text = "Search experiments...";
            SearchTextBox.ForeColor = SystemColors.GrayText;
        }
    }

    private void StatusFilterCombo_SelectedIndexChanged(object sender, EventArgs e)
    {
        ApplyFilters();
    }

    private void FavoritesOnlyCheck_CheckedChanged(object sender, EventArgs e)
    {
        ApplyFilters();
    }

    private void ExperimentText_Click(object sender, EventArgs e) { }

    private void Main_Load(object sender, EventArgs e) { }

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
