namespace Manager
{
    partial class AddExperiments
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
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
            label6 = new Label();
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
            label2.Size = new Size(70, 15);
            label2.TabIndex = 3;
            label2.Text = "Description ";
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
            framework.Size = new Size(64, 15);
            framework.TabIndex = 6;
            framework.Text = "framework";
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
            label4.Size = new Size(38, 15);
            label4.TabIndex = 9;
            label4.Text = "status";
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
            // label6
            // 
            label6.AutoSize = true;
            label6.Font = new Font("Segoe UI", 5F);
            label6.Location = new Point(192, 144);
            label6.Name = "label6";
            label6.Size = new Size(74, 10);
            label6.TabIndex = 12;
            label6.Text = "(separated by a coma)";
            // 
            // AddExperiments
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(300, 450);
            Controls.Add(label6);
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
            FormBorderStyle = FormBorderStyle.SizableToolWindow;
            Name = "AddExperiments";
            Text = "Add Experiment";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

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
        private Label label6;
    }
}