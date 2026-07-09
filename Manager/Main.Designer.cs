namespace Manager
{
    partial class Main
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                components?.Dispose();
                _experimentService?.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            ExperimentText = new Label();
            AddExperienceButton = new Button();
            EditExperimentButton = new Button();
            experimentGrid = new DataGridView();
            LoadExperimentButton = new Button();
            DeleteExperimentButton = new Button();
            ToggleFavoriteButton = new Button();
            SearchTextBox = new TextBox();
            SearchButton = new Button();
            StatusFilterCombo = new ComboBox();
            FavoritesOnlyCheck = new CheckBox();
            ToolStrip = new Panel();
            ((System.ComponentModel.ISupportInitialize)experimentGrid).BeginInit();
            ToolStrip.SuspendLayout();
            SuspendLayout();
            //
            // ToolStrip
            //
            ToolStrip.Controls.Add(SearchTextBox);
            ToolStrip.Controls.Add(SearchButton);
            ToolStrip.Controls.Add(StatusFilterCombo);
            ToolStrip.Controls.Add(FavoritesOnlyCheck);
            ToolStrip.Location = new Point(0, 0);
            ToolStrip.Name = "ToolStrip";
            ToolStrip.Size = new Size(800, 30);
            ToolStrip.TabIndex = 7;
            //
            // SearchTextBox
            //
            SearchTextBox.Location = new Point(12, 5);
            SearchTextBox.Name = "SearchTextBox";
            SearchTextBox.Size = new Size(200, 23);
            SearchTextBox.TabIndex = 0;
            SearchTextBox.Text = "Search experiments...";
            SearchTextBox.Enter += SearchTextBox_Enter;
            SearchTextBox.Leave += SearchTextBox_Leave;
            SearchTextBox.KeyDown += SearchTextBox_KeyDown;
            //
            // SearchButton
            //
            SearchButton.Location = new Point(218, 4);
            SearchButton.Name = "SearchButton";
            SearchButton.Size = new Size(50, 23);
            SearchButton.TabIndex = 1;
            SearchButton.Text = "Search";
            SearchButton.Click += SearchButton_Click;
            //
            // StatusFilterCombo
            //
            StatusFilterCombo.DropDownStyle = ComboBoxStyle.DropDownList;
            StatusFilterCombo.Location = new Point(290, 4);
            StatusFilterCombo.Name = "StatusFilterCombo";
            StatusFilterCombo.Size = new Size(100, 23);
            StatusFilterCombo.TabIndex = 2;
            StatusFilterCombo.SelectedIndexChanged += StatusFilterCombo_SelectedIndexChanged;
            //
            // FavoritesOnlyCheck
            //
            FavoritesOnlyCheck.AutoSize = true;
            FavoritesOnlyCheck.Location = new Point(400, 6);
            FavoritesOnlyCheck.Name = "FavoritesOnlyCheck";
            FavoritesOnlyCheck.Size = new Size(104, 19);
            FavoritesOnlyCheck.TabIndex = 3;
            FavoritesOnlyCheck.Text = "Favorites Only";
            FavoritesOnlyCheck.UseVisualStyleBackColor = true;
            FavoritesOnlyCheck.CheckedChanged += FavoritesOnlyCheck_CheckedChanged;
            //
            // ExperimentText
            //
            ExperimentText.AutoSize = true;
            ExperimentText.Font = new Font("Segoe UI", 9F, FontStyle.Bold);
            ExperimentText.Location = new Point(12, 34);
            ExperimentText.Name = "ExperimentText";
            ExperimentText.Size = new Size(132, 15);
            ExperimentText.TabIndex = 0;
            ExperimentText.Text = "No Loaded Experiments";
            //
            // experimentGrid
            //
            experimentGrid.AllowUserToAddRows = false;
            experimentGrid.AllowUserToDeleteRows = false;
            experimentGrid.AllowUserToResizeRows = false;
            experimentGrid.BackgroundColor = SystemColors.Window;
            experimentGrid.BorderStyle = BorderStyle.Fixed3D;
            experimentGrid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            experimentGrid.Location = new Point(12, 55);
            experimentGrid.MultiSelect = false;
            experimentGrid.Name = "experimentGrid";
            experimentGrid.ReadOnly = false;
            experimentGrid.RowHeadersVisible = false;
            experimentGrid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            experimentGrid.Size = new Size(776, 355);
            experimentGrid.TabIndex = 1;
            experimentGrid.CellValueChanged += ExperimentGrid_CellValueChanged;
            experimentGrid.CellDoubleClick += ExperimentGrid_CellDoubleClick;
            //
            // AddExperienceButton
            //
            AddExperienceButton.Location = new Point(12, 416);
            AddExperienceButton.Name = "AddExperienceButton";
            AddExperienceButton.Size = new Size(43, 23);
            AddExperienceButton.TabIndex = 2;
            AddExperienceButton.Text = "Add";
            AddExperienceButton.Click += AddExperienceButton_Click;
            //
            // ToggleFavoriteButton
            //
            ToggleFavoriteButton.Location = new Point(61, 416);
            ToggleFavoriteButton.Name = "ToggleFavoriteButton";
            ToggleFavoriteButton.Size = new Size(83, 23);
            ToggleFavoriteButton.TabIndex = 3;
            ToggleFavoriteButton.Text = "\u2606 Favorite";
            ToggleFavoriteButton.Click += ToggleFavoriteButton_Click;
            //
            // EditExperimentButton
            //
            EditExperimentButton.Location = new Point(150, 416);
            EditExperimentButton.Name = "EditExperimentButton";
            EditExperimentButton.Size = new Size(42, 23);
            EditExperimentButton.TabIndex = 4;
            EditExperimentButton.Text = "Edit";
            EditExperimentButton.Click += EditExperimentButton_Click;
            //
            // DeleteExperimentButton
            //
            DeleteExperimentButton.Location = new Point(198, 416);
            DeleteExperimentButton.Name = "DeleteExperimentButton";
            DeleteExperimentButton.Size = new Size(42, 23);
            DeleteExperimentButton.TabIndex = 5;
            DeleteExperimentButton.Text = "Del";
            DeleteExperimentButton.Click += DeleteExperimentButton_Click;
            //
            // LoadExperimentButton
            //
            LoadExperimentButton.Location = new Point(246, 416);
            LoadExperimentButton.Name = "LoadExperimentButton";
            LoadExperimentButton.Size = new Size(75, 23);
            LoadExperimentButton.TabIndex = 6;
            LoadExperimentButton.Text = "Refresh";
            LoadExperimentButton.Click += LoadExperimentButton_Click;
            //
            // Main
            //
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            MaximizeBox = false;
            Controls.Add(LoadExperimentButton);
            Controls.Add(DeleteExperimentButton);
            Controls.Add(EditExperimentButton);
            Controls.Add(ToggleFavoriteButton);
            Controls.Add(experimentGrid);
            Controls.Add(AddExperienceButton);
            Controls.Add(ExperimentText);
            Controls.Add(ToolStrip);
            Name = "Main";
            Text = "Lab Manager";
            Load += Main_Load;
            ((System.ComponentModel.ISupportInitialize)experimentGrid).EndInit();
            ToolStrip.ResumeLayout(false);
            ToolStrip.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        private Label ExperimentText;
        private Button AddExperienceButton;
        private Button EditExperimentButton;
        private DataGridView experimentGrid;
        private Button LoadExperimentButton;
        private Button DeleteExperimentButton;
        private Button ToggleFavoriteButton;
        private TextBox SearchTextBox;
        private Button SearchButton;
        private ComboBox StatusFilterCombo;
        private CheckBox FavoritesOnlyCheck;
        private Panel ToolStrip;
    }
}
