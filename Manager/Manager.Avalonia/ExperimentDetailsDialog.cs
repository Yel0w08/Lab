using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;
using Manager.Models;

namespace Manager;

public class ExperimentDetailsDialog : Window
{
    public ExperimentDetailsDialog(Experiment experiment)
    {
        Title = $"Experiment #{experiment.Id} - {experiment.Name}";
        Width = 420;
        CanResize = false;
        WindowStartupLocation = WindowStartupLocation.CenterOwner;

        var fields = new StackPanel
        {
            Spacing = 4,
            Margin = new Avalonia.Thickness(15),
            Children =
            {
                MakeRow("Name:", experiment.Name, false),
                MakeRow("Status:", FormatStatus(experiment.Status), false),
                MakeRow("Language:", experiment.Language, false),
                MakeRow("Framework:", experiment.Framework, false),
                MakeRow("Engine:", experiment.Engine, false),
                MakeRow("Description:", experiment.Description, true),
                MakeRow("Tags:", experiment.Tags.Count > 0 ? string.Join(", ", experiment.Tags) : "-", false),
                MakeRow("Project Path:", experiment.ProjectPath, false),
                MakeRow("Notes:", experiment.Notes, true),
                MakeRow("Created:", experiment.CreatedAt.ToString("g"), false),
                MakeRow("Modified:", experiment.LastModified.ToString("g"), false),
                MakeRow("Favorite:", experiment.Favorite ? "Yes" : "No", experiment.Favorite),
                MakeRow("Downloadable:", experiment.Downloadable ? "Yes" : "No", experiment.Downloadable),
            }
        };

        var closeButton = new Button
        {
            Content = "Close",
            Width = 80,
            HorizontalAlignment = HorizontalAlignment.Center,
            Margin = new Avalonia.Thickness(0, 10)
        };
        closeButton.Click += (_, _) => Close();

        Content = new StackPanel
        {
            Children = { fields, closeButton }
        };
    }

    private static StackPanel MakeRow(string label, string value, bool bold)
    {
        var valueBlock = new TextBlock
        {
            Text = string.IsNullOrWhiteSpace(value) ? "-" : value,
            VerticalAlignment = VerticalAlignment.Center,
            TextWrapping = TextWrapping.Wrap,
            MaxWidth = 280
        };
        if (bold)
        {
            valueBlock.FontWeight = FontWeight.Bold;
            valueBlock.Foreground = Brushes.DarkOrange;
        }

        return new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 8,
            Children =
            {
                new TextBlock
                {
                    Text = label,
                    Width = 90,
                    VerticalAlignment = VerticalAlignment.Center,
                    FontWeight = FontWeight.SemiBold
                },
                valueBlock
            }
        };
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
