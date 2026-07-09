namespace Manager
{
    partial class Main
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            ExperimentText = new Label();
            AddExperienceButton = new Button();
            EditExperimentButton = new Button();
            treeView1 = new TreeView();
            LoadExperimentButton = new Button();
            InfoDialog = new FontDialog();
            SuspendLayout();
            // 
            // ExperimentText
            // 
            ExperimentText.AutoSize = true;
            ExperimentText.Location = new Point(12, 9);
            ExperimentText.Name = "ExperimentText";
            ExperimentText.Size = new Size(132, 15);
            ExperimentText.TabIndex = 0;
            ExperimentText.Text = "No Loaded Experiments";
            ExperimentText.Click += ExperimentText_Click;
            // 
            // AddExperienceButton
            // 
            AddExperienceButton.Location = new Point(607, 415);
            AddExperienceButton.Name = "AddExperienceButton";
            AddExperienceButton.Size = new Size(43, 23);
            AddExperienceButton.TabIndex = 0;
            AddExperienceButton.Tag = "";
            AddExperienceButton.Text = "Add";
            AddExperienceButton.Click += AddExperienceButton_Click;
            // 
            // EditExperimentButton
            // 
            EditExperimentButton.Location = new Point(559, 415);
            EditExperimentButton.Name = "EditExperimentButton";
            EditExperimentButton.Size = new Size(42, 23);
            EditExperimentButton.TabIndex = 2;
            EditExperimentButton.Tag = "";
            EditExperimentButton.Text = "Edit";
            // 
            // treeView1
            // 
            treeView1.Location = new Point(559, 9);
            treeView1.Name = "treeView1";
            treeView1.Size = new Size(229, 400);
            treeView1.TabIndex = 1;
            // 
            // LoadExperimentButton
            // 
            LoadExperimentButton.Location = new Point(656, 415);
            LoadExperimentButton.Name = "LoadExperimentButton";
            LoadExperimentButton.Size = new Size(132, 23);
            LoadExperimentButton.TabIndex = 3;
            LoadExperimentButton.Tag = "";
            LoadExperimentButton.Text = "Load";
            // 
            // Main
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(LoadExperimentButton);
            Controls.Add(EditExperimentButton);
            Controls.Add(treeView1);
            Controls.Add(AddExperienceButton);
            Controls.Add(ExperimentText);
            Name = "Main";
            Text = "Lab Manager";
            Load += Main_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label ExperimentText;
        private Button AddExperienceButton;
        private Button EditExperimentButton;
        private TreeView treeView1;
        private Button LoadExperimentButton;
        private FontDialog InfoDialog;
    }
}
