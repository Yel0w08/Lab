using System;
using System.Collections.Generic;
using System.Text;
using N_Body.Models;


namespace N_Body.Math
{
    public class Math
    {



        public void CalculateForces(List<Body> bodies)
        {

            Body b1 = bodies[1];
            Body b2 = bodies[2];
            double masse = b1.mass;
            double posX = b1.X;
            double Gravity = 6.674e-11;

            double dx = b2.X - b1.X;
            double dy = b2.Y - b1.Y;
            double dz = b2.Z - b1.Z;
            double r = System.Math.Sqrt(dx * dx + dy * dy + dz * dz);

            double Force = Gravity * b1.mass * b2.mass / b1.r * b1.r ;


        }

    }
}
