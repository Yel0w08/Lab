using Microsoft.EntityFrameworkCore;
using Manager.Models;

namespace Manager.Data;

public class AppDbContext : DbContext
{
    public DbSet<Experiment> Experiments => Set<Experiment>();

    public AppDbContext() { }

    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

    protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
    {
        if (!optionsBuilder.IsConfigured)
        {
            optionsBuilder.UseSqlite(DatabaseConfig.ConnectionString);
        }
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Experiment>(entity =>
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id).ValueGeneratedOnAdd();

            entity.Property(e => e.Name)
                  .IsRequired()
                  .HasMaxLength(200);

            entity.Property(e => e.Description)
                  .HasMaxLength(2000);

            entity.Property(e => e.Language)
                  .HasMaxLength(100);

            entity.Property(e => e.Framework)
                  .HasMaxLength(100);

            entity.Property(e => e.Engine)
                  .HasMaxLength(100);

            entity.Property(e => e.Status)
                  .HasConversion<string>()
                  .HasMaxLength(50);

            entity.Property(e => e.ProjectPath)
                  .HasMaxLength(500);

            entity.Property(e => e.Tags)
                  .HasMaxLength(1000)
                  .HasConversion(
                      v => string.Join(',', v),
                      v => v.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                             .ToList()
                  );

            entity.Property(e => e.Notes)
                  .HasMaxLength(4000);
        });
    }
}
