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
        private int _uViewPos;
        private int _uColorSeed;

        // bloom
        private uint _hdrFbo;
        private uint _hdrColorTex;
        private uint _hdrDepthRbo;
        private uint _brightFbo;
        private uint _brightTex;
        private uint _blurPingFbo;
        private uint _blurPingTex;
        private uint _blurPongFbo;
        private uint _blurPongTex;
        private uint _quadVao;
        private uint _quadVbo;
        private uint _bloomProgram;
        private uint _brightProgram;
        private uint _blurProgram;
        private int _bloomULocation;
        private float _bloomIntensity = 0f;
        private float _bloomThreshold = 0f;

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
                _gl.Enable(EnableCap.Multisample);
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
                _uColorSeed = _gl.GetUniformLocation(_program, "uColorSeed");
                _uViewPos = _gl.GetUniformLocation(_program, "uViewPos");
                _uView = _gl.GetUniformLocation(_program, "uView");
                _uProjection = _gl.GetUniformLocation(_program, "uProjection");
                _imguiController = new ImGuiController(_gl, _window, _input);
                NumberOfBodyToAdd = _simulation.Bodies.Count;

                InitBloom();
            };

            _window.Render += (deltaTime) =>
            {

                _calc.CalculateForces(_simulation.Bodies);
                _calc.UpdatePositions(_simulation.Bodies, _dt);

                _gl.BindFramebuffer(FramebufferTarget.Framebuffer, _hdrFbo);
                _gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

                _gl.UseProgram(_program);

                var proj = Matrix4X4.CreatePerspectiveFieldOfView(
                    MathF.PI / 4f,
                    800f / 600f,
                    0.1f,
                    1000f
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

                _gl.BindVertexArray(_vao);
                _gl.Uniform3(_uViewPos, camX, camY, camZ);

                for (int i = 0; i < _simulation.Bodies.Count; i++)
                {
                    float massScale = System.Math.Clamp((float)(System.Math.Log10(_simulation.Bodies[i].mass) - 23) / 10f, 0f, 1f);

                    _gl.Uniform1(_uMass, massScale);
                    _gl.Uniform1(_uColorSeed, _simulation.Bodies[i].colorSeed);
                    float s = System.Math.Clamp(massScale, 0.02f, 1.0f);
                    var model = Matrix4X4.CreateScale(s);
                    _gl.UniformMatrix4(_uModel, 1, false, ref model.Row1.X);

                    float px = (float)(_simulation.Bodies[i].X / _scale);
                    float py = (float)(_simulation.Bodies[i].Y / _scale);
                    float pz = (float)(_simulation.Bodies[i].Z / _scale);
                    _gl.Uniform3(_uPosition, px, py, pz);
                    unsafe { _gl.DrawElements(PrimitiveType.Triangles, (uint)_sphereIndices.Length, DrawElementsType.UnsignedInt, null); }
                }

                RenderBloom();

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
        public void Render()
        {

        }

        private void InitBloom()
        {
            int w = 800;
            int h = 600;

            uint CreateTex(int width, int height)
            {
                uint tex = _gl.GenTexture();
                _gl.BindTexture(TextureTarget.Texture2D, tex);
                unsafe { _gl.TexImage2D((GLEnum)TextureTarget.Texture2D, 0, (int)InternalFormat.Rgba16f, (uint)width, (uint)height, 0, (GLEnum)PixelFormat.Rgba, (GLEnum)PixelType.Float, null); }
                _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
                _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
                _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.ClampToEdge);
                _gl.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.ClampToEdge);
                return tex;
            }

            uint CreateFbo(uint tex)
            {
                uint fbo = _gl.GenFramebuffer();
                _gl.BindFramebuffer(FramebufferTarget.Framebuffer, fbo);
                _gl.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.ColorAttachment0, TextureTarget.Texture2D, tex, 0);
                return fbo;
            }

            // main HDR FBO with depth
            _hdrColorTex = CreateTex(w, h);
            _hdrFbo = _gl.GenFramebuffer();
            _gl.BindFramebuffer(FramebufferTarget.Framebuffer, _hdrFbo);
            _gl.FramebufferTexture2D(FramebufferTarget.Framebuffer, FramebufferAttachment.ColorAttachment0, TextureTarget.Texture2D, _hdrColorTex, 0);
            _hdrDepthRbo = _gl.GenRenderbuffer();
            _gl.BindRenderbuffer(RenderbufferTarget.Renderbuffer, _hdrDepthRbo);
            _gl.RenderbufferStorage((GLEnum)RenderbufferTarget.Renderbuffer, (GLEnum)InternalFormat.DepthComponent24, (uint)w, (uint)h);
            _gl.FramebufferRenderbuffer(FramebufferTarget.Framebuffer, FramebufferAttachment.DepthAttachment, RenderbufferTarget.Renderbuffer, _hdrDepthRbo);

            // bright FBO
            _brightTex = CreateTex(w, h);
            _brightFbo = CreateFbo(_brightTex);

            // blur ping/pong FBOs
            _blurPingTex = CreateTex(w, h);
            _blurPingFbo = CreateFbo(_blurPingTex);
            _blurPongTex = CreateTex(w, h);
            _blurPongFbo = CreateFbo(_blurPongTex);

            _gl.BindFramebuffer(FramebufferTarget.Framebuffer, 0);

            // fullscreen quad
            float[] quadVertices = {
                -1f,  1f,  0f, 1f,
                -1f, -1f,  0f, 0f,
                 1f, -1f,  1f, 0f,
                -1f,  1f,  0f, 1f,
                 1f, -1f,  1f, 0f,
                 1f,  1f,  1f, 1f,
            };

            _quadVao = _gl.GenVertexArray();
            _quadVbo = _gl.GenBuffer();
            _gl.BindVertexArray(_quadVao);
            _gl.BindBuffer(BufferTargetARB.ArrayBuffer, _quadVbo);
            _gl.BufferData(BufferTargetARB.ArrayBuffer, (nuint)(quadVertices.Length * sizeof(float)), quadVertices, BufferUsageARB.StaticDraw);
            _gl.VertexAttribPointer(0, 2, VertexAttribPointerType.Float, false, 4 * sizeof(float), 0);
            _gl.EnableVertexAttribArray(0);
            _gl.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, 4 * sizeof(float), 2 * sizeof(float));
            _gl.EnableVertexAttribArray(1);

            // compile bloom shaders
            string screenVert = File.ReadAllText("Shaders/screen.vert");

            string brightSrc = File.ReadAllText("Shaders/bright.frag");
            _brightProgram = CompileProgram(screenVert, brightSrc);

            string blurSrc = File.ReadAllText("Shaders/blur.frag");
            _blurProgram = CompileProgram(screenVert, blurSrc);

            string bloomSrc = File.ReadAllText("Shaders/bloom.frag");
            _bloomProgram = CompileProgram(screenVert, bloomSrc);

            _bloomULocation = _gl.GetUniformLocation(_bloomProgram, "uBloomIntensity");
        }

        private uint CompileProgram(string vertSrc, string fragSrc)
        {
            uint vert = _gl.CreateShader(ShaderType.VertexShader);
            _gl.ShaderSource(vert, vertSrc);
            _gl.CompileShader(vert);

            uint frag = _gl.CreateShader(ShaderType.FragmentShader);
            _gl.ShaderSource(frag, fragSrc);
            _gl.CompileShader(frag);

            uint program = _gl.CreateProgram();
            _gl.AttachShader(program, vert);
            _gl.AttachShader(program, frag);
            _gl.LinkProgram(program);

            _gl.DeleteShader(vert);
            _gl.DeleteShader(frag);

            return program;
        }

        private void RenderBloom()
        {
            int w = 800;
            int h = 600;
            float bloomIntensity = _bloomIntensity;
            float threshold = _bloomThreshold;

            _gl.BindVertexArray(_quadVao);
            _gl.UseProgram(_brightProgram);

            _gl.Uniform1(_gl.GetUniformLocation(_brightProgram, "uThreshold"), threshold);
            _gl.ActiveTexture(TextureUnit.Texture0);
            _gl.BindTexture(TextureTarget.Texture2D, _hdrColorTex);
            _gl.Uniform1(_gl.GetUniformLocation(_brightProgram, "uScene"), 0);

            _gl.BindFramebuffer(FramebufferTarget.Framebuffer, _brightFbo);
            _gl.Clear(ClearBufferMask.ColorBufferBit);
            unsafe { _gl.DrawArrays(PrimitiveType.Triangles, 0, 6); }

            // ping-pong blur (2 iterations = 4 passes)
            uint srcTex = _brightTex;
            int blurUHorizontal = _gl.GetUniformLocation(_blurProgram, "uHorizontal");
            int blurUTexelSize = _gl.GetUniformLocation(_blurProgram, "uTexelSize");
            int blurUTexture = _gl.GetUniformLocation(_blurProgram, "uTexture");

            _gl.UseProgram(_blurProgram);

            for (int iter = 0; iter < 2; iter++)
            {
                // horizontal
                _gl.BindFramebuffer(FramebufferTarget.Framebuffer, _blurPingFbo);
                _gl.Clear(ClearBufferMask.ColorBufferBit);
                _gl.ActiveTexture(TextureUnit.Texture0);
                _gl.BindTexture(TextureTarget.Texture2D, srcTex);
                _gl.Uniform1(blurUTexture, 0);
                _gl.Uniform1(blurUHorizontal, 1);
                _gl.Uniform2(blurUTexelSize, 1f / w, 1f / h);
                unsafe { _gl.DrawArrays(PrimitiveType.Triangles, 0, 6); }

                // vertical
                _gl.BindFramebuffer(FramebufferTarget.Framebuffer, _blurPongFbo);
                _gl.Clear(ClearBufferMask.ColorBufferBit);
                _gl.ActiveTexture(TextureUnit.Texture0);
                _gl.BindTexture(TextureTarget.Texture2D, _blurPingTex);
                _gl.Uniform1(blurUHorizontal, 0);
                _gl.Uniform2(blurUTexelSize, 1f / w, 1f / h);
                unsafe { _gl.DrawArrays(PrimitiveType.Triangles, 0, 6); }

                srcTex = _blurPongTex;
            }

            // composite to default framebuffer
            _gl.BindFramebuffer(FramebufferTarget.Framebuffer, 0);
            _gl.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
            _gl.UseProgram(_bloomProgram);
            _gl.Uniform1(_bloomULocation, bloomIntensity);

            _gl.ActiveTexture(TextureUnit.Texture0);
            _gl.BindTexture(TextureTarget.Texture2D, _hdrColorTex);
            _gl.Uniform1(_gl.GetUniformLocation(_bloomProgram, "uScene"), 0);

            _gl.ActiveTexture(TextureUnit.Texture1);
            _gl.BindTexture(TextureTarget.Texture2D, srcTex);
            _gl.Uniform1(_gl.GetUniformLocation(_bloomProgram, "uBloom"), 1);

            unsafe { _gl.DrawArrays(PrimitiveType.Triangles, 0, 6); }
        }
    }

}
