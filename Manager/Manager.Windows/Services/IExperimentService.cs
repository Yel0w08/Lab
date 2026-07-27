using Manager.Models;

namespace Manager.Services;

public interface IExperimentService
{
    List<Experiment> GetAll();
    Experiment? GetById(int id);
    void Add(Experiment experiment);
    void Update(Experiment experiment);
    void Delete(int id);
    List<Experiment> Search(string query);
    List<Experiment> GetByStatus(ExperimentStatus status);
}
