using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;

namespace Manager;

public class MessageDialog : Window
{
    public MessageDialog(string title, string message)
    {
        Title = title;
        Width = 300;
        Height = 150;
        CanResize = false;
        WindowStartupLocation = WindowStartupLocation.CenterOwner;

        var okButton = new Button { Content = "OK", Width = 80, HorizontalAlignment = HorizontalAlignment.Center };
        okButton.Click += (_, _) => Close();

        Content = new StackPanel
        {
            Margin = new Avalonia.Thickness(20),
            Spacing = 15,
            VerticalAlignment = VerticalAlignment.Center,
            Children =
            {
                new TextBlock { Text = message, TextWrapping = TextWrapping.Wrap, MaxWidth = 260 },
                okButton
            }
        };
    }
}

public class ConfirmDialog : Window
{
    public bool Confirmed { get; private set; }

    public ConfirmDialog(string title, string message)
    {
        Title = title;
        Width = 320;
        Height = 150;
        CanResize = false;
        WindowStartupLocation = WindowStartupLocation.CenterOwner;

        var yesButton = new Button { Content = "Yes", Width = 80 };
        var noButton = new Button { Content = "No", Width = 80 };
        yesButton.Click += (_, _) => { Confirmed = true; Close(); };
        noButton.Click += (_, _) => Close();

        Content = new StackPanel
        {
            Margin = new Avalonia.Thickness(20),
            Spacing = 15,
            VerticalAlignment = VerticalAlignment.Center,
            Children =
            {
                new TextBlock { Text = message, TextWrapping = TextWrapping.Wrap, MaxWidth = 280 },
                new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    Spacing = 10,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    Children = { yesButton, noButton }
                }
            }
        };
    }
}
