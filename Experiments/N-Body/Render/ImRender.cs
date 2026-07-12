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
using System.Numerics;
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
        private float _camAngleX = 0.3f;
        private float _camAngleY = 0.0f;
        private float _scale = 1e11f;
        private int _uView;
        private int _uProjection;
        private uint _ebo;
        private float[] _sphereVertices;
        private uint[] _sphereIndices;
        private int _uViewPos;
        private int NumberOfBodyToAdd = 1;
        private uint _instanceVbo;

        private bool _mouseDragging = false;
        private Vector2 _lastMousePos;

        public void Initalize()
        {



            var options = WindowOptions.Default;
            options.Size = new Silk.NET.Maths.Vector2D<int>(800, 600);
            options.Title = "N-Body";
            options.PreferredDepthBufferBits = 24;


            _window = Window.Create(options);

            _window.Load += () =>
            {
                _gl = GL.GetApi(_window);

                _gl.Enable(EnableCap.DepthTest);
                _gl.Enable(EnableCap.ProgramPointSize);
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

                _instanceVbo = _gl.GenBuffer();
                _gl.BindBuffer(BufferTargetARB.ArrayBuffer, _instanceVbo);
                unsafe { _gl.BufferData(BufferTargetARB.ArrayBuffer, 0, null, BufferUsageARB.DynamicDraw); }

                _gl.VertexAttribPointer(1, 3, VertexAttribPointerType.Float, false, 6 * sizeof(float), 0);
                _gl.EnableVertexAttribArray(1);
                _gl.VertexAttribDivisor(1, 1);

                _gl.VertexAttribPointer(2, 1, VertexAttribPointerType.Float, false, 6 * sizeof(float), 3 * sizeof(float));
                _gl.EnableVertexAttribArray(2);
                _gl.VertexAttribDivisor(2, 1);

                _gl.VertexAttribPointer(3, 1, VertexAttribPointerType.Float, false, 6 * sizeof(float), 4 * sizeof(float));
                _gl.EnableVertexAttribArray(3);
                _gl.VertexAttribDivisor(3, 1);

                _gl.VertexAttribPointer(4, 1, VertexAttribPointerType.Float, false, 6 * sizeof(float), 5 * sizeof(float));
                _gl.EnableVertexAttribArray(4);
                _gl.VertexAttribDivisor(4, 1);

                _input = _window.CreateInput();

                _input.Mice[0].Scroll += (mouse, wheel) =>
                {
                    _camDistance *= wheel.Y > 0 ? 0.8f : 1.2f;
                };

                _input.Mice[0].MouseDown += (mouse, button) =>
                {
                    if (button == MouseButton.Left)
                    {
                        _mouseDragging = true;
                        _lastMousePos = mouse.Position;
                    }
                };

                _input.Mice[0].MouseUp += (mouse, button) =>
                {
                    if (button == MouseButton.Left)
                        _mouseDragging = false;
                };

                _input.Mice[0].MouseMove += (mouse, position) =>
                {
                    if (_mouseDragging)
                    {
                        float dx = position.X - _lastMousePos.X;
                        float dy = position.Y - _lastMousePos.Y;
                        _camAngleY -= dx * 0.005f;
                        _camAngleX -= dy * 0.005f;
                        if (_camAngleX < -MathF.PI / 2f + 0.01f) _camAngleX = -MathF.PI / 2f + 0.01f;
                        if (_camAngleX > MathF.PI / 2f - 0.01f) _camAngleX = MathF.PI / 2f - 0.01f;
                        _lastMousePos = position;
                    }
                };

                _uViewPos = _gl.GetUniformLocation(_program, "uViewPos");
                _uView = _gl.GetUniformLocation(_program, "uView");
                _uProjection = _gl.GetUniformLocation(_program, "uProjection");
                _imguiController = new ImGuiController(_gl, _window, _input);
                NumberOfBodyToAdd = _simulation.Bodies.Count;
            };

            _window.Render += (deltaTime) =>
            {

                _calc.CalculateForces(_simulation.Bodies);
                _calc.UpdatePositions(_simulation.Bodies, _dt);

                _gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

                _gl.UseProgram(_program);

                var proj = Matrix4X4.CreatePerspectiveFieldOfView(
                    MathF.PI / 4f,
                    800f / 600f,
                    0.01f,
                    2000f
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

                _gl.BindVertexArray(_vao);
                _gl.Uniform3(_uViewPos, camX, camY, camZ);

                int count = _simulation.Bodies.Count;
                float[] instanceData = new float[count * 6];
                for (int i = 0; i < count; i++)
                {
                    float massScale = System.Math.Clamp((float)(System.Math.Log10(_simulation.Bodies[i].mass) - 23) / 10f, 0f, 1f);
                    float s = System.Math.Clamp(massScale, 0.02f, 1.0f);
                    instanceData[i * 6 + 0] = (float)(_simulation.Bodies[i].X / _scale);
                    instanceData[i * 6 + 1] = (float)(_simulation.Bodies[i].Y / _scale);
                    instanceData[i * 6 + 2] = (float)(_simulation.Bodies[i].Z / _scale);
                    instanceData[i * 6 + 3] = s;
                    instanceData[i * 6 + 4] = massScale;
                    instanceData[i * 6 + 5] = _simulation.Bodies[i].colorSeed;
                }

                unsafe
                {
                    fixed (float* data = instanceData)
                    {
                        _gl.BufferData(BufferTargetARB.ArrayBuffer, (nuint)(instanceData.Length * sizeof(float)), data, BufferUsageARB.DynamicDraw);
                    }
                }

                unsafe { _gl.DrawElementsInstanced(PrimitiveType.Triangles, (uint)_sphereIndices.Length, DrawElementsType.UnsignedInt, null, (uint)count); }

                _imguiController.Update((float)deltaTime);

                ImGui.Begin("Controls");
                ImGui.SliderFloat("Speed", ref _dt, 0.001f, 10000.0f);
                int prev = NumberOfBodyToAdd;
                ImGui.InputInt("Number of Body", ref NumberOfBodyToAdd);
                if (NumberOfBodyToAdd > prev)
                {
                    for (int i = 0; i < NumberOfBodyToAdd - prev; i++)
                        _simulation.AddRandomBody();

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

    }

}
