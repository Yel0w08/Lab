namespace Manager
{
    partial class AddExperiments
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            ExperminmentName = new TextBox();
            label1 = new Label();
            ExperminmentDescritpion = new TextBox();
            label2 = new Label();
            label3 = new Label();
            ExperminmentLanguage = new TextBox();
            framework = new Label();
            ExperminmentFramework = new TextBox();
            ExperminmentStatus = new ComboBox();
            label4 = new Label();
            label5 = new Label();
            textBox1 = new TextBox();
            SaveButton = new Button();
            CancelBtn = new Button();
            NotesLabel = new Label();
            NotesTextBox = new TextBox();
            EngineLabel = new Label();
            EngineTextBox = new TextBox();
            ProjectPathLabel = new Label();
            ProjectPathTextBox = new TextBox();
            SuspendLayout();
            // 
            // ExperminmentName
            // 
            ExperminmentName.Location = new Point(4, 6);
            ExperminmentName.Name = "ExperminmentName";
            ExperminmentName.Size = new Size(186, 23);
            ExperminmentName.TabIndex = 0;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(196, 9);
            label1.Name = "label1";
            label1.Size = new Size(39, 15);
            label1.TabIndex = 1;
            label1.Text = "Name";
            // 
            // ExperminmentDescritpion
            // 
            ExperminmentDescritpion.Location = new Point(4, 35);
            ExperminmentDescritpion.Multiline = true;
            ExperminmentDescritpion.Name = "ExperminmentDescritpion";
            ExperminmentDescritpion.Size = new Size(186, 23);
            ExperminmentDescritpion.TabIndex = 2;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(196, 38);
            label2.Name = "label2";
            label2.Size = new Size(67, 15);
            label2.TabIndex = 3;
            label2.Text = "Description";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(196, 64);
            label3.Name = "label3";
            label3.Size = new Size(59, 15);
            label3.TabIndex = 4;
            label3.Text = "Language";
            // 
            // ExperminmentLanguage
            // 
            ExperminmentLanguage.Location = new Point(4, 64);
            ExperminmentLanguage.Multiline = true;
            ExperminmentLanguage.Name = "ExperminmentLanguage";
            ExperminmentLanguage.Size = new Size(186, 23);
            ExperminmentLanguage.TabIndex = 5;
            // 
            // framework
            // 
            framework.AutoSize = true;
            framework.Location = new Point(196, 96);
            framework.Name = "framework";
            framework.Size = new Size(66, 15);
            framework.TabIndex = 6;
            framework.Text = "Framework";
            // 
            // ExperminmentFramework
            // 
            ExperminmentFramework.Location = new Point(4, 93);
            ExperminmentFramework.Multiline = true;
            ExperminmentFramework.Name = "ExperminmentFramework";
            ExperminmentFramework.Size = new Size(186, 23);
            ExperminmentFramework.TabIndex = 7;
            // 
            // ExperminmentStatus
            // 
            ExperminmentStatus.DropDownStyle = ComboBoxStyle.DropDownList;
            ExperminmentStatus.FormattingEnabled = true;
            ExperminmentStatus.Location = new Point(4, 122);
            ExperminmentStatus.Name = "ExperminmentStatus";
            ExperminmentStatus.Size = new Size(186, 23);
            ExperminmentStatus.TabIndex = 8;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(196, 125);
            label4.Name = "label4";
            label4.Size = new Size(39, 15);
            label4.TabIndex = 9;
            label4.Text = "Status";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(196, 154);
            label5.Name = "label5";
            label5.Size = new Size(31, 15);
            label5.TabIndex = 10;
            label5.Text = "Tags";
            // 
            // textBox1
            // 
            textBox1.Location = new Point(4, 151);
            textBox1.Multiline = true;
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(186, 23);
            textBox1.TabIndex = 11;
            // 
            // SaveButton
            // 
            SaveButton.Location = new Point(69, 414);
            SaveButton.Name = "SaveButton";
            SaveButton.Size = new Size(75, 23);
            SaveButton.TabIndex = 13;
            SaveButton.Text = "Save";
            SaveButton.UseVisualStyleBackColor = true;
            SaveButton.Click += SaveButton_Click;
            // 
            // CancelBtn
            // 
            CancelBtn.Location = new Point(150, 414);
            CancelBtn.Name = "CancelBtn";
            CancelBtn.Size = new Size(75, 23);
            CancelBtn.TabIndex = 14;
            CancelBtn.Text = "Cancel";
            CancelBtn.UseVisualStyleBackColor = true;
            CancelBtn.Click += CancelButton_Click;
            // 
            // NotesLabel
            // 
            NotesLabel.AutoSize = true;
            NotesLabel.Location = new Point(196, 184);
            NotesLabel.Name = "NotesLabel";
            NotesLabel.Size = new Size(38, 15);
            NotesLabel.TabIndex = 16;
            NotesLabel.Text = "Notes";
            // 
            // NotesTextBox
            // 
            NotesTextBox.Location = new Point(4, 180);
            NotesTextBox.Multiline = true;
            NotesTextBox.Name = "NotesTextBox";
            NotesTextBox.Size = new Size(186, 60);
            NotesTextBox.TabIndex = 15;
            // 
            // EngineLabel
            // 
            EngineLabel.AutoSize = true;
            EngineLabel.Location = new Point(196, 250);
            EngineLabel.Name = "EngineLabel";
            EngineLabel.Size = new Size(43, 15);
            EngineLabel.TabIndex = 18;
            EngineLabel.Text = "Engine";
            // 
            // EngineTextBox
            // 
            EngineTextBox.Location = new Point(4, 246);
            EngineTextBox.Name = "EngineTextBox";
            EngineTextBox.Size = new Size(186, 23);
            EngineTextBox.TabIndex = 17;
            // 
            // ProjectPathLabel
            // 
            ProjectPathLabel.AutoSize = true;
            ProjectPathLabel.Location = new Point(196, 279);
            ProjectPathLabel.Name = "ProjectPathLabel";
            ProjectPathLabel.Size = new Size(71, 15);
            ProjectPathLabel.TabIndex = 20;
            ProjectPathLabel.Text = "Project Path";
            // 
            // ProjectPathTextBox
            // 
            ProjectPathTextBox.Location = new Point(4, 275);
            ProjectPathTextBox.Name = "ProjectPathTextBox";
            ProjectPathTextBox.Size = new Size(186, 23);
            ProjectPathTextBox.TabIndex = 19;
            // 
            // AddExperiments
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(300, 450);
            Controls.Add(ProjectPathLabel);
            Controls.Add(ProjectPathTextBox);
            Controls.Add(EngineLabel);
            Controls.Add(EngineTextBox);
            Controls.Add(NotesLabel);
            Controls.Add(NotesTextBox);
            Controls.Add(CancelBtn);
            Controls.Add(SaveButton);
            Controls.Add(textBox1);
            Controls.Add(label5);
            Controls.Add(label4);
            Controls.Add(ExperminmentStatus);
            Controls.Add(ExperminmentFramework);
            Controls.Add(framework);
            Controls.Add(ExperminmentLanguage);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(ExperminmentDescritpion);
            Controls.Add(label1);
            Controls.Add(ExperminmentName);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "AddExperiments";
            StartPosition = FormStartPosition.CenterParent;
            Text = "Add Experiment";
            Load += AddExperiments_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        private TextBox ExperminmentName;
        private Label label1;
        private TextBox ExperminmentDescritpion;
        private Label label2;
        private Label label3;
        private TextBox ExperminmentLanguage;
        private Label framework;
        private TextBox ExperminmentFramework;
        private ComboBox ExperminmentStatus;
        private Label label4;
        private Label label5;
        private TextBox textBox1;
        private Button SaveButton;
        private Button CancelBtn;
        private Label NotesLabel;
        private TextBox NotesTextBox;
        private Label EngineLabel;
        private TextBox EngineTextBox;
        private Label ProjectPathLabel;
        private TextBox ProjectPathTextBox;
    }
}
