using ImGuiNET;
using N_Body.Math;
using Silk.NET;
using Silk.NET.Input;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.OpenGL.Extensions.ImGui;
using Silk.NET.Windowing;
using System;
using System.Collections.Generic;
using System.Runtime;
using System.Text;
using static Silk.NET.Core.Native.WinString;


namespace N_Body.Render
{
    public class ImRender
    {
        public ImRender(Simulation simulation, BodyPotitionCalculators calc)
        {
            _simulation = simulation;
            _calc = calc;
        }

        private float _dt = 1000.0f;

        private GL _gl;
        private uint _vao;
        private uint _vbo;
        private uint _program;
        private IWindow _window;
        private Simulation _simulation;
        private BodyPotitionCalculators _calc;
        private ImGuiController _imguiController;
        private IInputContext _input;
        private float _camDistance = 10.0f;
        private float _camAngleX = 0.0f;
        private float _camAngleY = 0.0f;
        private float _scale = 1e11f;
        private int _uView;
        private int _uProjection;
        private uint _ebo;
        private float[] _sphereVertices;
        private uint[] _sphereIndices;
        private int _uPosition;
        private int NumberOfBodyToAdd = 1;
        private int _uMass;
        private int _uModel;

        public void Initalize()
        {



            var options = WindowOptions.Default;
            options.Size = new Silk.NET.Maths.Vector2D<int>(800, 600);
            options.Title = "N-Body";


            _window = Window.Create(options);

            _window.Load += () =>
            {
                _gl = GL.GetApi(_window);

                _gl.Enable(EnableCap.DepthTest);
                _gl.ClearColor(0, 0, 0, 1);

                string vertSrc = File.ReadAllText("Shaders/body.vert");
                string fragSrc = File.ReadAllText("Shaders/body.frag");

                uint vert = _gl.CreateShader(ShaderType.VertexShader);
                _gl.ShaderSource(vert, vertSrc);
                _gl.CompileShader(vert);

                uint frag = _gl.CreateShader(ShaderType.FragmentShader);
                _gl.ShaderSource(frag, fragSrc);
                _gl.CompileShader(frag);
                _gl.GetShader(vert, ShaderParameterName.CompileStatus, out int vertOk);
                _gl.GetShader(frag, ShaderParameterName.CompileStatus, out int fragOk);
                Console.WriteLine($"Vert: {vertOk} Frag: {fragOk}");

                _program = _gl.CreateProgram();

                _gl.AttachShader(_program, vert);
                _gl.AttachShader(_program, frag);
                _gl.LinkProgram(_program);

                _gl.GetProgram(_program, ProgramPropertyARB.LinkStatus, out int linkOk);
                Console.WriteLine($"Link: {linkOk}");
                string log = _gl.GetProgramInfoLog(_program);
                Console.WriteLine($"Log: '{log}'");
                Console.WriteLine($"Program ID: {_program}");

                _vao = _gl.GenVertexArray();
                _vbo = _gl.GenBuffer();

                _sphereVertices = SphereGnenerator.Generate(0.15f, 16);
                _sphereIndices = SphereGnenerator.GenerateIndices(16);

                _ebo = _gl.GenBuffer();

                _gl.BindVertexArray(_vao);
                _gl.BindBuffer(BufferTargetARB.ElementArrayBuffer, _ebo);
                _gl.BufferData(BufferTargetARB.ElementArrayBuffer, (nuint)(_sphereIndices.Length * sizeof(uint)), _sphereIndices, BufferUsageARB.StaticDraw);

                _gl.BindBuffer(BufferTargetARB.ArrayBuffer, _vbo);
                _gl.BufferData(BufferTargetARB.ArrayBuffer, (nuint)(_sphereVertices.Length * sizeof(float)), _sphereVertices, BufferUsageARB.StaticDraw);
                _gl.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 3 * sizeof(float), 0);
                _gl.EnableVertexAttribArray(0);

                _input = _window.CreateInput();

                _uPosition = _gl.GetUniformLocation(_program, "uPosition");



                _input.Mice[0].Scroll += (mouse, wheel) =>
                {
                    _camDistance *= wheel.Y > 0 ? 0.8f : 1.2f;
                };

                //_input.Mice[0].MouseMove += (mouse, delta) =>
                //{
                //    if (mouse.IsButtonPressed(MouseButton.Right))
                //    {
                //        _camAngleY += delta.X * 0.01f;
                //        _camAngleX += delta.Y * 0.01f;
                //    }
                //};
                _uModel = _gl.GetUniformLocation(_program, "uModel");
                _uMass = _gl.GetUniformLocation(_program, "uMass");
                _uView = _gl.GetUniformLocation(_program, "uView");
                _uProjection = _gl.GetUniformLocation(_program, "uProjection");
                _imguiController = new ImGuiController(_gl, _window, _input);
                NumberOfBodyToAdd = _simulation.Bodies.Count;

            };

            _window.Render += (deltaTime) =>
            {


                _gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

                _calc.CalculateForces(_simulation.Bodies);
                _calc.UpdatePositions(_simulation.Bodies, _dt);



                //float[] positions = new float[_simulation.Bodies.Count * 3];
                //for (int i = 0; i < _simulation.Bodies.Count; i++)
                //{
                //    positions[i * 3] = (float)(_simulation.Bodies[i].X / _scale);
                //    positions[i * 3 + 1] = (float)(_simulation.Bodies[i].Y / _scale);
                //    positions[i * 3 + 2] = (float)(_simulation.Bodies[i].Z / _scale);
                //}

                //_gl.BindVertexArray(_vao);

                //_gl.BindBuffer(BufferTargetARB.ArrayBuffer, _vbo);
                //_gl.BufferData(BufferTargetARB.ArrayBuffer, (nuint)(positions.Length * sizeof(float)), positions, BufferUsageARB.DynamicDraw);


                //_gl.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 3 * sizeof(float), 0);

                //_gl.EnableVertexAttribArray(0);

                _gl.UseProgram(_program);

          var proj = Matrix4X4.CreatePerspectiveFieldOfView(
                    MathF.PI / 4f, // 45°
                    800f / 600f,   // aspect ratio
                    0.1f,          // near
                    1000f          // far
                );

                var camX = _camDistance * MathF.Sin(_camAngleY) * MathF.Cos(_camAngleX);
                var camY = _camDistance * MathF.Sin(_camAngleX);
                var camZ = _camDistance * MathF.Cos(_camAngleY) * MathF.Cos(_camAngleX);

                var view = Matrix4X4.CreateLookAt(
                    new Vector3D<float>(camX, camY, camZ),
                    new Vector3D<float>(0, 0, 0),
                    new Vector3D<float>(0, 1, 0)
                );


                _gl.UniformMatrix4(_uProjection, 1, false, ref proj.Row1.X);
                _gl.UniformMatrix4(_uView, 1, false, ref view.Row1.X);


                _gl.Enable(EnableCap.ProgramPointSize);

                _imguiController.Update((float)deltaTime);


                //_gl.DrawArrays(PrimitiveType.Points, 0, (uint)_simulation.Bodies.Count);

                //_gl.BindVertexArray(_vao);
                //_gl.BindBuffer(BufferTargetARB.ArrayBuffer, _vbo);
                //_gl.BufferData(BufferTargetARB.ArrayBuffer, (nuint)(_sphereVertices.Length * sizeof(float)), _sphereVertices, BufferUsageARB.DynamicDraw);

                _gl.BindVertexArray(_vao);

                for (int i = 0; i < _simulation.Bodies.Count; i++)
                {
                    float massScale = System.Math.Clamp((float)(System.Math.Log10(_simulation.Bodies[i].mass) - 23) / 10f,0f,1f);

                    _gl.Uniform1(_uMass, massScale);

                    _gl.Uniform1(_uMass, massScale);
                    var model = Matrix4X4.CreateScale(massScale);
                    _gl.UniformMatrix4(_uModel, 1, false, ref model.Row1.X);

                    float px = (float)(_simulation.Bodies[i].X / _scale);
                    float py = (float)(_simulation.Bodies[i].Y / _scale);
                    float pz = (float)(_simulation.Bodies[i].Z / _scale);
                    _gl.Uniform3(_uPosition, px, py, pz);
                    unsafe { _gl.DrawElements(PrimitiveType.Triangles, (uint)_sphereIndices.Length, DrawElementsType.UnsignedInt, null); }
                }

                ImGui.Begin("Controls");
                ImGui.SliderFloat("Speed", ref _dt, 0.001f, 10000.0f);
                int prev = NumberOfBodyToAdd;
                ImGui.InputInt("Number of Body", ref NumberOfBodyToAdd);
                if (NumberOfBodyToAdd > prev)
                {
                    for (int i = 0; i < NumberOfBodyToAdd - prev; i++)
                        _simulation.AddRandomBody();
                    Console.WriteLine(
        $"mass={_simulation.Bodies[i].mass:E2} scale={massScale}"
    );

                }
                else if (NumberOfBodyToAdd < prev)
                {
                    for (int i = 0; i < prev - NumberOfBodyToAdd; i++)
                        _simulation.Bodies.RemoveAt(_simulation.Bodies.Count - 1);

                }



                ImGui.End();



                _imguiController.Render();


            };





            _window.Run();



        }
        public void Render()
        {

        }
    }

}
