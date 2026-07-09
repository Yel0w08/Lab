using Microsoft.EntityFrameworkCore;
using Manager.Data;
using Manager.Models;

namespace Manager.Services;

public class ExperimentService : IExperimentService, IDisposable
{
    private readonly AppDbContext _context;

    public ExperimentService()
    {
        _context = new AppDbContext();
        _context.Database.EnsureCreated();
    }

    public List<Experiment> GetAll()
    {
        return _context.Experiments
            .OrderByDescending(e => e.LastModified)
            .ToList();
    }

    public Experiment? GetById(int id)
    {
        return _context.Experiments.Find(id);
    }

    public void Add(Experiment experiment)
    {
        experiment.CreatedAt = DateTime.Now;
        experiment.LastModified = DateTime.Now;
        _context.Experiments.Add(experiment);
        _context.SaveChanges();
    }

    public void Update(Experiment experiment)
    {
        experiment.LastModified = DateTime.Now;
        _context.Experiments.Update(experiment);
        _context.SaveChanges();
    }

    public void Delete(int id)
    {
        var experiment = _context.Experiments.Find(id);
        if (experiment is not null)
        {
            _context.Experiments.Remove(experiment);
            _context.SaveChanges();
        }
    }

    public List<Experiment> Search(string query)
    {
        return _context.Experiments
            .Where(e =>
                e.Name.Contains(query) ||
                e.Description.Contains(query) ||
                e.Language.Contains(query) ||
                e.Framework.Contains(query) ||
                e.Notes.Contains(query))
            .OrderByDescending(e => e.LastModified)
            .ToList();
    }

    public List<Experiment> GetByStatus(ExperimentStatus status)
    {
        return _context.Experiments
            .Where(e => e.Status == status)
            .OrderByDescending(e => e.LastModified)
            .ToList();
    }

    public void Dispose()
    {
        _context.Dispose();
    }
}
