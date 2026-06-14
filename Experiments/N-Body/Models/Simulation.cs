using N_Body.Models;

public class Simulation
{
    public List<Body> Bodies { get; set; }
    Random random = new Random();
    public Simulation()
    {
        Bodies = new List<Body>();
        Bodies.Add(new Body { name = "Star", mass = 1e32, X = 0, Y = 0, Z = 0, colorSeed = 0.5f });

        for (int i = 0; i < 20; i++)
        {
            Bodies.Add(new Body
            {
                name = $"Body_{i}",
                mass = 1e20 + random.NextDouble() * 1e30,
                X = (random.NextDouble() - 0.5) * 4e11,
                Y = (random.NextDouble() - 0.5) * 4e11,
                Z = 0,
                vX = (random.NextDouble() - 0.5) * 1e3,
                vY = (random.NextDouble() - 0.5) * 1e3,
                vZ = (random.NextDouble() - 0.5) * 1e3,
                colorSeed = (float)random.NextDouble(),
            });
        }
    }
          public void AddRandomBody()
    {
        Bodies.Add(new Body
        {

            mass = 1e20 + random.NextDouble() * 1e30,
            X = (random.NextDouble() - 0.5) * 4e11,
            Y = (random.NextDouble() - 0.5) * 4e11,
            Z = 0,
            vX = (random.NextDouble() - 0.5) * 1e3,
            vY = (random.NextDouble() - 0.5) * 1e3,
            vZ = (random.NextDouble() - 0.5) * 1e3,
            colorSeed = (float)random.NextDouble(),
        });

    }


}
