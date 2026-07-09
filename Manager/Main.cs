using Manager.Services;

namespace Manager
{


    public partial class Main : Form
    {
        private readonly SaveManager _saveManager;
        public string ExperimentDBFileName = "ExperimentsDB.json";
        public string ExperimentDBFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ExperimentsDB.json");
        public Main()
        {

            InitializeComponent();

            if (!File.Exists(ExperimentDBFileName))
            {
                DialogResult result = MessageBox.Show(
                    $"{ExperimentDBFileName} file does not already exist. Do you want to create it?",
                    "Popup Example",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question,
                    MessageBoxDefaultButton.Button1);

                if (result == DialogResult.Yes)
                {

                    File.WriteAllText(ExperimentDBFileName, "{ \"experiments\": [] }");
                }
                else
                {
                    return;
                }
            }

            if (File.Exists(ExperimentDBFilePath))
            {

                _saveManager = new SaveManager(ExperimentDBFilePath);

                _saveManager.Load();
            }

        }

        private void AddExperienceButton_Click(object sender, EventArgs e)
        {
            _saveManager.AddExperiment(new Models.Experiment
            {
                Name = "New Experiment",
                Description = "Description of the new experiment",
                Language = "C#",
                Framework = ".NET",
                Engine = "Custom Engine",
                Status = Models.ExperimentStatus.Planned,
                ProjectPath = "",
                Tags = new List<string>(),
                Favorite = false,
                Notes = ""
            });

        }

        private void ExperimentText_Click(object sender, EventArgs e)
        {

        }

        private void Main_Load(object sender, EventArgs e)
        {

        }
    }
}
