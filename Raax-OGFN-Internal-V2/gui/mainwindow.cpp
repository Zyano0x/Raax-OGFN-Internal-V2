#include "mainwindow.h"

#include <extern/imgui/imgui.h>
#include <config/config.h>
#include <gui/gui.h>

namespace GUI {
namespace MainWindow {

// --- Window Tab Functions & Data -----------------------------------

enum class WindowTab {
    Aimbot = 0,
    Visuals,
    Exploits,
    Config,
    Misc,
};
WindowTab Tab = WindowTab::Aimbot;

void AimbotTab() {
    enum class WindowSubTab { Shells = 0, Light, Medium, Heavy, Other };
    static WindowSubTab SubTab = WindowSubTab::Shells;

    auto DisplayAimbotConfig = [](Config::ConfigData::AimbotConfig& Config) {
        ImGui::Checkbox("Aimbot Enabled", &Config.Enabled);
        ImGui::SliderFloat("Smoothness", &Config.Smoothness, 1.f, 20.f);
        ImGui::SliderFloat("Max Distance", &Config.MaxDistance, 1.f, 500.f);

        ImGui::Checkbox("Visible Check", &Config.VisibleCheck);
        ImGui::Checkbox("Sticky Target", &Config.StickyTarget);

        ImGui::Checkbox("Show FOV", &Config.ShowFOV);
        ImGui::SliderFloat("FOV", &Config.FOV, 0.25f, 180.f);

        if (Config.DeadzoneFOV > Config.FOV)
            Config.DeadzoneFOV = Config.FOV;

        ImGui::Checkbox("Use Deadzone", &Config.UseDeadzone);
        ImGui::Checkbox("Show Deadzone FOV", &Config.ShowDeadzoneFOV);
        ImGui::SliderFloat("Deadzone FOV", &Config.DeadzoneFOV, 0.25f, Config.FOV);

        ImGui::Combo("Target Selection", (int*)&Config.Selection, "Distance\000Degrees\000Combined");
        ImGui::Combo("Target Bone", (int*)&Config.Bone, "Head\000Neck\000Chest\000Pelvis\000Random");

        ImGui::SliderFloat("Random Bone Refresh Rate", &Config.RandomBoneRefreshRate, 0.1f, 3.f);
    };

    ImGui::Checkbox("Split By Ammo Type", &Config::g_Config.SplitAimbotByAmmo);
    ImGui::Checkbox("Bullet Prediction", &Config::g_Config.BulletPrediction);
    static bool WaitingForKeybind = false;
    GUI::Keybind("Aim Keybind", WaitingForKeybind, (ImGuiKey&)Config::g_Config.AimbotKeybind);

    if (Config::g_Config.SplitAimbotByAmmo) {
        ImGui::BeginDisabled(SubTab == WindowSubTab::Shells);
        if (ImGui::Button("Shells"))
            SubTab = WindowSubTab::Shells;
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(SubTab == WindowSubTab::Light);
        if (ImGui::Button("Light"))
            SubTab = WindowSubTab::Light;
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(SubTab == WindowSubTab::Medium);
        if (ImGui::Button("Medium"))
            SubTab = WindowSubTab::Medium;
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(SubTab == WindowSubTab::Heavy);
        if (ImGui::Button("Heavy"))
            SubTab = WindowSubTab::Heavy;
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(SubTab == WindowSubTab::Other);
        if (ImGui::Button("Other"))
            SubTab = WindowSubTab::Other;
        ImGui::EndDisabled();

        if (ImGui::BeginChild("ConfigRegion")) {
            switch (SubTab) {
            case WindowSubTab::Shells:
                DisplayAimbotConfig(Config::g_Config.ShellsAimbot);
                break;
            case WindowSubTab::Light:
                DisplayAimbotConfig(Config::g_Config.LightAimbot);
                break;
            case WindowSubTab::Medium:
                DisplayAimbotConfig(Config::g_Config.MediumAimbot);
                break;
            case WindowSubTab::Heavy:
                DisplayAimbotConfig(Config::g_Config.HeavyAimbot);
                break;
            case WindowSubTab::Other:
                DisplayAimbotConfig(Config::g_Config.OtherAimbot);
                break;
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("ConfigRegion")) {
            DisplayAimbotConfig(Config::g_Config.AllAimbot);
        }
        ImGui::EndChild();
    }
}

void VisualsTab() {
    enum class WindowSubTab { Player = 0, Loot };
    static WindowSubTab SubTab = WindowSubTab::Player;

    ImGui::BeginDisabled(SubTab == WindowSubTab::Player);
    if (ImGui::Button("Player"))
        SubTab = WindowSubTab::Player;
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(SubTab == WindowSubTab::Loot);
    if (ImGui::Button("Loot"))
        SubTab = WindowSubTab::Loot;
    ImGui::EndDisabled();

    if (ImGui::BeginChild("ConfigRegion")) {
        switch (SubTab) {
        case WindowSubTab::Player:
            ImGui::Checkbox("Box", &Config::g_Config.Visuals.Player.Box);
            ImGui::Combo("Box Type", (int*)&Config::g_Config.Visuals.Player.BoxType, "Full\000Cornered\000Full 3D");
            ImGui::SliderFloat("Box Thickness", &Config::g_Config.Visuals.Player.BoxThickness, 1.f, 20.f);

            ImGui::Checkbox("Skeleton", &Config::g_Config.Visuals.Player.Skeleton);
            ImGui::SliderFloat("Skeleton Thickness", &Config::g_Config.Visuals.Player.SkeletonThickness, 1.f, 20.f);

            ImGui::Checkbox("Tracer", &Config::g_Config.Visuals.Player.Tracer);
            ImGui::SliderFloat("Tracer Thickness", &Config::g_Config.Visuals.Player.TracerThickness, 1.f, 20.f);
            ImGui::Combo("Tracer Start", (int*)&Config::g_Config.Visuals.Player.TracerStart, "Top\000Middle\000Bottom");
            ImGui::Combo("Tracer End", (int*)&Config::g_Config.Visuals.Player.TracerEnd, "Top\000Middle\000Bottom");

            ImGui::Checkbox("Name", &Config::g_Config.Visuals.Player.Name);
            break;
        case WindowSubTab::Loot:
            ImGui::Checkbox("Loot Text", &Config::g_Config.Visuals.Loot.LootText);
            ImGui::SliderFloat("Loot Max Distance", &Config::g_Config.Visuals.Loot.LootMaxDistance, 1.f, 500.f);
            ImGui::Combo("Minimum Loot Tier", (int*)&Config::g_Config.Visuals.Loot.MinLootTier,
                         "Common\000Uncommon\000Rare\000Epic\000Legendary\000Mythic");

            ImGui::Checkbox("Chest Text", &Config::g_Config.Visuals.Loot.ChestText);
            ImGui::SliderFloat("Chest Max Distance", &Config::g_Config.Visuals.Loot.ChestMaxDistance, 1.f, 500.f);

            ImGui::Checkbox("Ammo Box Text", &Config::g_Config.Visuals.Loot.AmmoBoxText);
            ImGui::SliderFloat("Ammo Box Max Distance", &Config::g_Config.Visuals.Loot.AmmoBoxMaxDistance, 1.f, 500.f);
            break;
        }
    }
    ImGui::EndChild();
}

void ExploitsTab() {
    if (ImGui::BeginChild("ConfigRegion")) {
        ImGui::Text("Nothing here yet...");
    }
    ImGui::EndChild();
}

void ConfigTab() {
    static char InputConfigData[8196];
    static bool InputConfigIsValid = false;
    if (ImGui::BeginChild("ConfigRegion")) {
        if (ImGui::InputText("Config Data", InputConfigData, sizeof(InputConfigData))) {
            Config::ConfigData TestConfig = {};
            InputConfigIsValid = TestConfig.DeserializeConfig(InputConfigData, TestConfig);
        }

        if (ImGui::Button("Copy Config")) {
            ImGui::SetClipboardText(Config::g_Config.SerializeConfig(false).c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy Full Config")) {
            ImGui::SetClipboardText(Config::g_Config.SerializeConfig(true).c_str());
        }
        ImGui::SameLine();
        bool ShouldDisable = InputConfigIsValid;
        if (!ShouldDisable)
            ImGui::BeginDisabled();
        if (ImGui::Button("Load Config")) {
            Config::g_Config.DeserializeConfig(InputConfigData, Config::g_Config);
            InputConfigIsValid = false;
            InputConfigData[0] = '\00';
        }
        if (!ShouldDisable)
            ImGui::EndDisabled();

        if (ImGui::Button("Load Default Config"))
            Config::g_Config = Config::ConfigData();
    }
    ImGui::EndChild();
}

void MiscTab() {
    if (ImGui::BeginChild("ConfigRegion")) {
        ImGui::ColorEdit3("Primary Color Visible", (float*)&Config::g_Config.Color.PrimaryColorVisible);
        ImGui::ColorEdit3("Primary Color Hidden", (float*)&Config::g_Config.Color.PrimaryColorHidden);
        ImGui::ColorEdit3("Secondary Color Visible", (float*)&Config::g_Config.Color.SecondaryColorVisible);
        ImGui::ColorEdit3("Secondary Color Hidden", (float*)&Config::g_Config.Color.SecondaryColorHidden);

        ImGui::Text("Thank you for using my cheat! Join my Discord and star the repository!");
        ImGui::Text("discord.gg/Sde5mtbQe6 - github.com/raax7");
    }
    ImGui::EndChild();
}

// --- Public Tick Functions -----------------------------------------

void Tick() {
    if (ImGui::IsKeyReleased(ImGuiKey_Insert))
        GUI::MainWindow::g_WindowOpen = !GUI::MainWindow::g_WindowOpen;

    if (GUI::MainWindow::g_WindowOpen) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 5.f, ImColor(0.f, 0.f, 0.f));
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 4.f, ImColor(1.f, 1.f, 1.f));

        ImGui::SetNextWindowSize(ImVec2((ImGui::GetStyle().WindowPadding.x * 6.f) + (120.f * 5), 400.f));
        if (ImGui::Begin("Raax-OGFN-Internal V2", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::BeginDisabled(Tab == WindowTab::Aimbot);
            if (ImGui::Button("Aimbot", ImVec2(120.f, 30.f)))
                Tab = WindowTab::Aimbot;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Visuals);
            if (ImGui::Button("Visuals", ImVec2(120.f, 30.f)))
                Tab = WindowTab::Visuals;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Exploits);
            if (ImGui::Button("Exploits", ImVec2(120.f, 30.f)))
                Tab = WindowTab::Exploits;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Config);
            if (ImGui::Button("Config", ImVec2(120.f, 30.f)))
                Tab = WindowTab::Config;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Misc);
            if (ImGui::Button("Misc", ImVec2(120.f, 30.f)))
                Tab = WindowTab::Misc;
            ImGui::EndDisabled();

            switch (Tab) {
            case WindowTab::Aimbot:
                AimbotTab();
                break;
            case WindowTab::Visuals:
                VisualsTab();
                break;
            case WindowTab::Exploits:
                ExploitsTab();
                break;
            case WindowTab::Config:
                ConfigTab();
                break;
            case WindowTab::Misc:
                MiscTab();
                break;
            }
        }
        ImGui::End();
    }
}

// --- Global Variables ----------------------------------------------

bool g_WindowOpen = true;

} // namespace MainWindow
} // namespace GUI
