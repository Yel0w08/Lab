namespace Manager
{
    partial class ExperimentDetails
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components is not null))
                components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            NameLabel = new Label();
            NameValue = new Label();
            StatusLabel = new Label();
            StatusValue = new Label();
            LanguageLabel = new Label();
            LanguageValue = new Label();
            FrameworkLabel = new Label();
            FrameworkValue = new Label();
            EngineLabel = new Label();
            EngineValue = new Label();
            DescriptionLabel = new Label();
            DescriptionValue = new Label();
            TagsLabel = new Label();
            TagsValue = new Label();
            NotesLabel = new Label();
            NotesValue = new Label();
            ProjectPathLabel = new Label();
            ProjectPathValue = new Label();
            CreatedLabel = new Label();
            CreatedValue = new Label();
            ModifiedLabel = new Label();
            ModifiedValue = new Label();
            FavoriteLabel = new Label();
            FavoriteValue = new Label();
            CloseBtn = new Button();
            SuspendLayout();

            var labelWidth = 90;
            var valueX = 110;
            var formWidth = 420;
            var rowHeight = 28;
            var startY = 15;

            // Name
            NameLabel.Location = new Point(12, startY);
            NameLabel.Size = new Size(labelWidth, rowHeight);
            NameLabel.Text = "Name:";
            NameLabel.TextAlign = ContentAlignment.MiddleRight;
            NameLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            NameValue.Location = new Point(valueX, startY);
            NameValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            NameValue.TextAlign = ContentAlignment.MiddleLeft;
            NameValue.AutoEllipsis = true;

            // Status
            StatusLabel.Location = new Point(12, startY += rowHeight);
            StatusLabel.Size = new Size(labelWidth, rowHeight);
            StatusLabel.Text = "Status:";
            StatusLabel.TextAlign = ContentAlignment.MiddleRight;
            StatusLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            StatusValue.Location = new Point(valueX, startY);
            StatusValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            StatusValue.TextAlign = ContentAlignment.MiddleLeft;

            // Language
            LanguageLabel.Location = new Point(12, startY += rowHeight);
            LanguageLabel.Size = new Size(labelWidth, rowHeight);
            LanguageLabel.Text = "Language:";
            LanguageLabel.TextAlign = ContentAlignment.MiddleRight;
            LanguageLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            LanguageValue.Location = new Point(valueX, startY);
            LanguageValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            LanguageValue.TextAlign = ContentAlignment.MiddleLeft;

            // Framework
            FrameworkLabel.Location = new Point(12, startY += rowHeight);
            FrameworkLabel.Size = new Size(labelWidth, rowHeight);
            FrameworkLabel.Text = "Framework:";
            FrameworkLabel.TextAlign = ContentAlignment.MiddleRight;
            FrameworkLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            FrameworkValue.Location = new Point(valueX, startY);
            FrameworkValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            FrameworkValue.TextAlign = ContentAlignment.MiddleLeft;

            // Engine
            EngineLabel.Location = new Point(12, startY += rowHeight);
            EngineLabel.Size = new Size(labelWidth, rowHeight);
            EngineLabel.Text = "Engine:";
            EngineLabel.TextAlign = ContentAlignment.MiddleRight;
            EngineLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            EngineValue.Location = new Point(valueX, startY);
            EngineValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            EngineValue.TextAlign = ContentAlignment.MiddleLeft;

            // Description
            DescriptionLabel.Location = new Point(12, startY += rowHeight);
            DescriptionLabel.Size = new Size(labelWidth, rowHeight);
            DescriptionLabel.Text = "Description:";
            DescriptionLabel.TextAlign = ContentAlignment.MiddleRight;
            DescriptionLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            DescriptionValue.Location = new Point(valueX, startY);
            DescriptionValue.Size = new Size(formWidth - valueX - 20, 40);
            DescriptionValue.TextAlign = ContentAlignment.TopLeft;

            // Tags
            TagsLabel.Location = new Point(12, startY += 48);
            TagsLabel.Size = new Size(labelWidth, rowHeight);
            TagsLabel.Text = "Tags:";
            TagsLabel.TextAlign = ContentAlignment.MiddleRight;
            TagsLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            TagsValue.Location = new Point(valueX, startY);
            TagsValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            TagsValue.TextAlign = ContentAlignment.MiddleLeft;
            TagsValue.AutoEllipsis = true;

            // Project Path
            ProjectPathLabel.Location = new Point(12, startY += rowHeight);
            ProjectPathLabel.Size = new Size(labelWidth, rowHeight);
            ProjectPathLabel.Text = "Project Path:";
            ProjectPathLabel.TextAlign = ContentAlignment.MiddleRight;
            ProjectPathLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            ProjectPathValue.Location = new Point(valueX, startY);
            ProjectPathValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            ProjectPathValue.TextAlign = ContentAlignment.MiddleLeft;
            ProjectPathValue.AutoEllipsis = true;

            // Notes
            NotesLabel.Location = new Point(12, startY += rowHeight + 5);
            NotesLabel.Size = new Size(labelWidth, rowHeight);
            NotesLabel.Text = "Notes:";
            NotesLabel.TextAlign = ContentAlignment.TopRight;
            NotesLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            NotesValue.Location = new Point(valueX, startY);
            NotesValue.Size = new Size(formWidth - valueX - 20, 60);
            NotesValue.TextAlign = ContentAlignment.TopLeft;

            // Created
            CreatedLabel.Location = new Point(12, startY += 68);
            CreatedLabel.Size = new Size(labelWidth, rowHeight);
            CreatedLabel.Text = "Created:";
            CreatedLabel.TextAlign = ContentAlignment.MiddleRight;
            CreatedLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            CreatedValue.Location = new Point(valueX, startY);
            CreatedValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            CreatedValue.TextAlign = ContentAlignment.MiddleLeft;

            // Last Modified
            ModifiedLabel.Location = new Point(12, startY += rowHeight);
            ModifiedLabel.Size = new Size(labelWidth, rowHeight);
            ModifiedLabel.Text = "Modified:";
            ModifiedLabel.TextAlign = ContentAlignment.MiddleRight;
            ModifiedLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            ModifiedValue.Location = new Point(valueX, startY);
            ModifiedValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            ModifiedValue.TextAlign = ContentAlignment.MiddleLeft;

            // Favorite
            FavoriteLabel.Location = new Point(12, startY += rowHeight);
            FavoriteLabel.Size = new Size(labelWidth, rowHeight);
            FavoriteLabel.Text = "Favorite:";
            FavoriteLabel.TextAlign = ContentAlignment.MiddleRight;
            FavoriteLabel.Font = new Font("Segoe UI", 9F, FontStyle.Bold);

            FavoriteValue.Location = new Point(valueX, startY);
            FavoriteValue.Size = new Size(formWidth - valueX - 20, rowHeight);
            FavoriteValue.TextAlign = ContentAlignment.MiddleLeft;

            // Close button
            CloseBtn.Location = new Point((formWidth - 75) / 2, startY += 40);
            CloseBtn.Size = new Size(75, 23);
            CloseBtn.Text = "Close";
            CloseBtn.Click += CloseButton_Click;

            // Form
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(formWidth, startY + 50);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "ExperimentDetails";
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;
            Text = "Experiment Details";

            Controls.Add(CloseBtn);
            Controls.Add(FavoriteLabel); Controls.Add(FavoriteValue);
            Controls.Add(ModifiedLabel); Controls.Add(ModifiedValue);
            Controls.Add(CreatedLabel); Controls.Add(CreatedValue);
            Controls.Add(NotesLabel); Controls.Add(NotesValue);
            Controls.Add(ProjectPathLabel); Controls.Add(ProjectPathValue);
            Controls.Add(TagsLabel); Controls.Add(TagsValue);
            Controls.Add(DescriptionLabel); Controls.Add(DescriptionValue);
            Controls.Add(EngineLabel); Controls.Add(EngineValue);
            Controls.Add(FrameworkLabel); Controls.Add(FrameworkValue);
            Controls.Add(LanguageLabel); Controls.Add(LanguageValue);
            Controls.Add(StatusLabel); Controls.Add(StatusValue);
            Controls.Add(NameLabel); Controls.Add(NameValue);

            ResumeLayout(false);
            PerformLayout();
        }

        private Label NameLabel; private Label NameValue;
        private Label StatusLabel; private Label StatusValue;
        private Label LanguageLabel; private Label LanguageValue;
        private Label FrameworkLabel; private Label FrameworkValue;
        private Label EngineLabel; private Label EngineValue;
        private Label DescriptionLabel; private Label DescriptionValue;
        private Label TagsLabel; private Label TagsValue;
        private Label NotesLabel; private Label NotesValue;
        private Label ProjectPathLabel; private Label ProjectPathValue;
        private Label CreatedLabel; private Label CreatedValue;
        private Label ModifiedLabel; private Label ModifiedValue;
        private Label FavoriteLabel; private Label FavoriteValue;
        private Button CloseBtn;
    }
}
