using ImGuiNET;
using N_Body.Models;
using System;
using System.Collections.Generic;
using System.Text;

namespace N_Body.GUI.GUIs
{
    public class MainPopup
    {
        public void Render(Body body)
        {
            ImGui.BeginPopup("Conttrol Panel and infos");

            ImGui.EndPopup();
        }

    }
}
