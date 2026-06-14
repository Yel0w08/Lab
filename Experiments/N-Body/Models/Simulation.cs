using N_Body.Models;

public class Simulation
{
    public List<Body> Bodies { get; set; }
    Random random = new Random();
    public Simulation()
    {
        Bodies = new List<Body>();
        Bodies.Add(new Body { name = "Star", mass = 1e30, X = 0, Y = 0, Z = 0 });
        Bodies.Add(new Body { name = "Cool Star", mass = 1e28, X = 1e11, Y = 0, Z = 0 });

        for (int i = 0; i < 20; i++)
        {
            Bodies.Add(new Body
            {
                name = $"Body_{i}",
                mass = 1e24 + random.NextDouble() * 1e26,
                X = (random.NextDouble() - 0.5) * 4e11,
                Y = (random.NextDouble() - 0.5) * 4e11,
                Z = 0,
                vX = (random.NextDouble() - 0.5) * 1e3,
                vY = (random.NextDouble() - 0.5) * 1e3,
            });
        }
    }
}