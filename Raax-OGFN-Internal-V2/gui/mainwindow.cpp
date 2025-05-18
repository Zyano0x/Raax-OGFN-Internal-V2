#include "mainwindow.h"

#include <extern/imgui/imgui.h>
#define NOMINMAX
#include <config/config.h>
#include <config/keybind.h>
#include <config/config_reflection.h>
#include <gui/gui.h>

namespace GUI {
namespace MainWindow {

// --- Keybind UI Functions ------------------------------------------

template <typename T>
void DisplayMembersInTree(T* Instance, std::string Prefix, ConfigReflection::ConfigFieldView& OutField) {
    auto Members = ConfigReflection::DescribeMembers<T>();
    std::apply(
        [&](auto&&... Member) {
            (
                [&Member, Instance, &Prefix, &OutField] {
                    using MemberType = std::remove_cvref_t<decltype(Instance->*(Member.Ptr))>;
                    auto&       FieldValue = Instance->*(Member.Ptr);
                    std::string CurrentPath =
                        Prefix.empty() ? std::string(Member.Name) : Prefix + "." + Member.Name.data();

                    if constexpr (requires { ConfigReflection::DescribeMembers<MemberType>(); }) {
                        if (ImGui::TreeNode(Member.Name.data())) {
                            DisplayMembersInTree<MemberType>(&FieldValue, CurrentPath, OutField);
                            ImGui::TreePop();
                        }
                    } else if constexpr (std::is_same_v<MemberType, bool>) {
                        if (ImGui::Selectable(Member.Name.data(), OutField.Ptr == &FieldValue)) {
                            OutField = ConfigReflection::ConfigFieldView{
                                .Ptr = &FieldValue,
                                .Name = Member.Name,
                                .Type = ConfigReflection::TypeDescriptor<MemberType>::Name,
                                .FullPath = CurrentPath};
                        }
                    }
                }(),
                ...);
        },
        Members);
}

void DisplayKeybindPopup(Keybind::KeybindData& CurrentKeybind, const char* PopupName, bool AllowCancelling) {
    ImGui::SetNextWindowSize(ImVec2(350.f, 350.f), ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal(PopupName, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        ImVec2 WindowSize = ImGui::GetWindowSize();
        ImVec2 ScreenSize = ImGui::GetIO().DisplaySize;
        ImVec2 Centered = ImVec2(ScreenSize.x / 2.f - WindowSize.x / 2.f, ScreenSize.y / 2.f - WindowSize.y / 2.f);
        ImGui::SetWindowPos(Centered);

        ImGui::Text("Configure Keybind");
        ImGui::Separator();

        ImGui::Text("Select a target");
        if (ImGui::BeginChild("TargetSelection", ImVec2(0, 200), ImGuiChildFlags_Borders,
                              ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
            DisplayMembersInTree<Config::ConfigData>(&Config::g_Config, "", CurrentKeybind.ReflectedBool);
        }
        ImGui::EndChild();

        ImGui::Spacing();

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeight() * 2 -
                             ImGui::GetStyle().ItemSpacing.y * 2 - 5);
        static bool WaitingForKeybind = false;
        GUI::Keybind("New Keybind", WaitingForKeybind, CurrentKeybind.Keybind);

        bool ReadyToConfirm = CurrentKeybind.Keybind != ImGuiKey_None && CurrentKeybind.ReflectedBool.Ptr;

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - 5);
        ImGui::BeginDisabled(!ReadyToConfirm);
        if (ImGui::Button("Confirm", ImVec2(90, 0))) {
            Config::g_Config.Keybinds.Keybinds.push_back(CurrentKeybind);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::BeginDisabled(!AllowCancelling);
        if (ImGui::Button("Cancel", ImVec2(90, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::EndPopup();
    } else {
        CurrentKeybind = {};
    }
}

void AddKeybindButton() {
    if (ImGui::Button("Add Keybind")) {
        ImGui::OpenPopup("Keybind");
    }

    static Keybind::KeybindData CurrentKeybind = {};
    DisplayKeybindPopup(CurrentKeybind, "Keybind", true);
}

void ListKeybinds() {
    static Keybind::KeybindData CurrentKeybind = {};

    if (ImGui::Button("Delete All Keybinds")) {
        Config::g_Config.Keybinds.Keybinds.clear();
    }

    if (ImGui::BeginChild("KeybindList", ImVec2(0, 200), ImGuiChildFlags_Borders,
                          ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        size_t PendingDeleteIdx = -1;
        size_t PendingEditIdx = -1;

        if (Config::g_Config.Keybinds.Keybinds.empty()) {
            ImGui::Text("No keybinds!");
        } else {
            for (size_t i = 0; i < Config::g_Config.Keybinds.Keybinds.size(); i++) {
                const auto& Keybind = Config::g_Config.Keybinds.Keybinds[i];

                std::string KeybindInfoStr =
                    std::format("{} - {}", ImGui::GetKeyName(Keybind.Keybind), Keybind.ReflectedBool.Name.data());
                ImGui::PushID(&Keybind);
                if (ImGui::Button("Delete")) {
                    PendingDeleteIdx = i;
                }
                ImGui::SameLine();
                if (ImGui::Button("Edit")) {
                    PendingEditIdx = i;
                }
                ImGui::SameLine();
                ImGui::Text(KeybindInfoStr.c_str());
                ImGui::PopID();
            }
        }

        if (PendingDeleteIdx != -1) {
            Config::g_Config.Keybinds.Keybinds.erase(Config::g_Config.Keybinds.Keybinds.begin() + PendingDeleteIdx);
        }
        if (PendingEditIdx != -1) {
            CurrentKeybind = Config::g_Config.Keybinds.Keybinds[PendingEditIdx];
            Config::g_Config.Keybinds.Keybinds.erase(Config::g_Config.Keybinds.Keybinds.begin() + PendingEditIdx);
            ImGui::OpenPopup("Keybind (Edit)");
        }
    }

    DisplayKeybindPopup(CurrentKeybind, "Keybind (Edit)", false);
    ImGui::EndChild();
}

// --- Window Tab Functions & Data -----------------------------------

enum class WindowTab : int { Aimbot = 0, TriggerBot, Visuals, Exploits, Keybinds, Config, Misc, NUM };
WindowTab Tab = WindowTab::Aimbot;

void AimbotTab() {
    enum class WindowSubTab { Shells = 0, Light, Medium, Heavy, Other };
    static WindowSubTab SubTab = WindowSubTab::Shells;

    auto DisplayAimbotConfig = [](Config::ConfigData::AimbotConfig::AimbotAmmoConfig& Config) {
        ImGui::Checkbox("Enabled", &Config.Enabled);
        ImGui::Checkbox("Use In Trigger Bot", &Config.UseInTriggerBot);

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

    auto& AimbotConfig = Config::g_Config.Aimbot;

    ImGui::Checkbox("Split By Ammo Type", &AimbotConfig.SplitAimbotByAmmo);
    ImGui::Checkbox("Bullet Prediction", &AimbotConfig.BulletPrediction);
    static bool WaitingForKeybind = false;
    GUI::Keybind("Aim Keybind", WaitingForKeybind, (ImGuiKey&)AimbotConfig.AimbotKeybind);

    if (AimbotConfig.SplitAimbotByAmmo) {
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

        if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
            switch (SubTab) {
            case WindowSubTab::Shells:
                DisplayAimbotConfig(AimbotConfig.ShellsAimbot);
                break;
            case WindowSubTab::Light:
                DisplayAimbotConfig(AimbotConfig.LightAimbot);
                break;
            case WindowSubTab::Medium:
                DisplayAimbotConfig(AimbotConfig.MediumAimbot);
                break;
            case WindowSubTab::Heavy:
                DisplayAimbotConfig(AimbotConfig.HeavyAimbot);
                break;
            case WindowSubTab::Other:
                DisplayAimbotConfig(AimbotConfig.OtherAimbot);
                break;
            }
        }
        ImGui::EndChild();
    } else {
        if (ImGui::BeginChild("ConfigRegion")) {
            DisplayAimbotConfig(AimbotConfig.AllAimbot);
        }
        ImGui::EndChild();
    }
}

void TriggerBotTab() {
    auto& Config = Config::g_Config.TriggerBot;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::Checkbox("Enabled", &Config.Enabled);
        ImGui::Checkbox("Use Keybind", &Config.UseKeybind);
        static bool WaitingForKeybind = false;
        GUI::Keybind("Keybind", WaitingForKeybind, (ImGuiKey&)Config.Keybind);
        ImGui::Checkbox("Show FOV", &Config.ShowFOV);
        ImGui::SliderFloat("FOV", &Config.FOV, 0.25f, 180.f);
        ImGui::SliderFloat("Max Distance", &Config.MaxDistance, 0.f, 500.f);
        ImGui::SliderFloat("Fire Delay (s)", &Config.FireDelayS, 0.f, 2.f);
    }
    ImGui::EndChild();
}

void VisualsTab() {
    enum class WindowSubTab { Player = 0, Radar, Loot };
    static WindowSubTab SubTab = WindowSubTab::Player;

    ImGui::BeginDisabled(SubTab == WindowSubTab::Player);
    if (ImGui::Button("Player"))
        SubTab = WindowSubTab::Player;
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(SubTab == WindowSubTab::Radar);
    if (ImGui::Button("Radar"))
        SubTab = WindowSubTab::Radar;
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(SubTab == WindowSubTab::Loot);
    if (ImGui::Button("Loot"))
        SubTab = WindowSubTab::Loot;
    ImGui::EndDisabled();

    auto& Config = Config::g_Config.Visuals;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        switch (SubTab) {
        case WindowSubTab::Player:
            ImGui::SliderFloat("Max Distance", &Config.Player.MaxDistance, 0.f, 500.f);

            ImGui::Checkbox("Box", &Config.Player.Box);
            ImGui::Combo("Box Type", (int*)&Config.Player.BoxType, "Full\000Cornered\000Full 3D");
            ImGui::SliderFloat("Box Thickness", &Config.Player.BoxThickness, 1.f, 20.f);

            ImGui::Checkbox("Filled Box", &Config.Player.FilledBox);
            ImGui::ColorEdit4("Filled Box Color", (float*)&Config.Player.FilledBoxColor);

            ImGui::Checkbox("Skeleton", &Config.Player.Skeleton);
            ImGui::SliderFloat("Skeleton Thickness", &Config.Player.SkeletonThickness, 1.f, 20.f);

            ImGui::Checkbox("Tracer", &Config.Player.Tracer);
            ImGui::SliderFloat("Tracer Thickness", &Config.Player.TracerThickness, 1.f, 20.f);
            ImGui::Combo("Tracer Start", (int*)&Config.Player.TracerStart, "Top\000Middle\000Bottom");
            ImGui::Combo("Tracer End", (int*)&Config.Player.TracerEnd, "Top\000Middle\000Bottom");

            ImGui::Checkbox("Platform", &Config.Player.Platform);
            ImGui::Checkbox("Name", &Config.Player.Name);
            ImGui::Checkbox("Current Weapon", &Config.Player.CurrentWeapon);
            ImGui::Checkbox("Distance", &Config.Player.Distance);

            ImGui::Checkbox("OSI Enabled", &Config.Player.OSI);
            ImGui::Checkbox("OSI Match FOV", &Config.Player.OSIMatchFOV);
            ImGui::SliderFloat("OSI FOV", &Config.Player.OSIFOV, 0.25f, 180.f);
            ImGui::SliderFloat("OSI Size", &Config.Player.OSISize, 1.f, 20.f);
            break;
        case WindowSubTab::Radar: {
            float MaxPosition = 100.f - Config.Radar.Size;
            Config.Radar.PosX = std::min(Config.Radar.PosX, MaxPosition);
            Config.Radar.PosY = std::min(Config.Radar.PosY, MaxPosition);

            ImGui::Checkbox("Enable Radar", &Config.Radar.Radar);
            ImGui::Checkbox("Rotate With Camera", &Config.Radar.RotateWithCamera);
            ImGui::Checkbox("Show Camera FOV", &Config.Radar.ShowCameraFOV);
            ImGui::SliderFloat("Max Distance", &Config.Radar.MaxDistance, 1.f, 500.f);
            ImGui::SliderFloat("Position X", &Config.Radar.PosX, 0.f, MaxPosition);
            ImGui::SliderFloat("Position Y", &Config.Radar.PosY, 0.f, MaxPosition);
            ImGui::SliderFloat("Size", &Config.Radar.Size, 3.f, 100.f);
            ImGui::ColorEdit4("Color Visible", (float*)&Config.Radar.ColorVisible);
            ImGui::ColorEdit4("Color Hidden", (float*)&Config.Radar.ColorHidden);
            break;
        }
        case WindowSubTab::Loot:
            ImGui::Checkbox("Loot Text", &Config.Loot.LootText);
            ImGui::SliderFloat("Loot Max Distance", &Config.Loot.LootMaxDistance, 1.f, 500.f);
            ImGui::Combo("Minimum Loot Tier", (int*)&Config.Loot.MinLootTier,
                         "Common\000Uncommon\000Rare\000Epic\000Legendary\000Mythic");

            ImGui::Checkbox("Chest Text", &Config.Loot.ChestText);
            ImGui::SliderFloat("Chest Max Distance", &Config.Loot.ChestMaxDistance, 1.f, 500.f);

            ImGui::Checkbox("Ammo Box Text", &Config.Loot.AmmoBoxText);
            ImGui::SliderFloat("Ammo Box Max Distance", &Config.Loot.AmmoBoxMaxDistance, 1.f, 500.f);

            ImGui::Checkbox("Supply Drop Text", &Config.Loot.SupplyDropText);
            ImGui::SliderFloat("Supply Drop Max Distance", &Config.Loot.SupplyDropMaxDistance, 1.f, 500.f);
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

void KeybindsTab() {
    if (ImGui::BeginChild("ConfigRegion")) {
        AddKeybindButton();
        ListKeybinds();
    }
    ImGui::EndChild();
}

void ConfigTab() {
    static char InputConfigData[8196];
    static bool InputConfigIsValid = false;
    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        if (ImGui::InputText("Config Data", InputConfigData, sizeof(InputConfigData))) {
            Config::ConfigData TestConfig = {};
            InputConfigIsValid = TestConfig.DeserializeConfig(InputConfigData);
        }

        if (ImGui::Button("Copy Config")) {
            ImGui::SetClipboardText(Config::g_Config.SerializeConfig(false).c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy Full Config")) {
            ImGui::SetClipboardText(Config::g_Config.SerializeConfig(true).c_str());
        }

        ImGui::BeginDisabled(!InputConfigIsValid);
        if (ImGui::Button("Load Config")) {
            Config::g_Config.DeserializeConfig(InputConfigData);
            InputConfigIsValid = false;
            InputConfigData[0] = '\00';
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Load Default Config")) {
            Config::g_Config = Config::ConfigData();
        }
    }
    ImGui::EndChild();
}

void MiscTab() {
    auto& ColorConfig = Config::g_Config.Color;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::ColorEdit4("Primary Color Visible", (float*)&ColorConfig.PrimaryColorVisible);
        ImGui::ColorEdit4("Primary Color Hidden", (float*)&ColorConfig.PrimaryColorHidden);
        ImGui::ColorEdit4("Secondary Color Visible", (float*)&ColorConfig.SecondaryColorVisible);
        ImGui::ColorEdit4("Secondary Color Hidden", (float*)&ColorConfig.SecondaryColorHidden);

        static bool WaitingForKeybind = false;
        GUI::Keybind("Menu Keybind", WaitingForKeybind, (ImGuiKey&)Config::g_Config.MenuKeybind);

        ImGui::Text("Thank you for using my cheat! Join my Discord and star the repository!");
        ImGui::Text("NotTacs was here.");
        ImGui::Text("discord.gg/Sde5mtbQe6 - github.com/raax7");
    }
    ImGui::EndChild();
}

// --- Public Tick Functions -----------------------------------------

void Tick() {
    if (ImGui::IsKeyReleased((ImGuiKey)Config::g_Config.MenuKeybind))
        GUI::MainWindow::g_WindowOpen = !GUI::MainWindow::g_WindowOpen;

    if (GUI::MainWindow::g_WindowOpen) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 5.f, ImColor(0.f, 0.f, 0.f));
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 4.f, ImColor(1.f, 1.f, 1.f));

        constexpr float TabButtonWidth = 90.f;
        ImGui::SetNextWindowSize(ImVec2((ImGui::GetStyle().WindowPadding.x * ((int)WindowTab::NUM + 1)) +
                                            (TabButtonWidth * (int)WindowTab::NUM),
                                        400.f));
        if (ImGui::Begin("Raax-OGFN-Internal V2", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::BeginDisabled(Tab == WindowTab::Aimbot);
            if (ImGui::Button("Aimbot", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Aimbot;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::TriggerBot);
            if (ImGui::Button("Trigger Bot", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::TriggerBot;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Visuals);
            if (ImGui::Button("Visuals", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Visuals;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Exploits);
            if (ImGui::Button("Exploits", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Exploits;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Keybinds);
            if (ImGui::Button("Keybinds", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Keybinds;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Config);
            if (ImGui::Button("Config", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Config;
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(Tab == WindowTab::Misc);
            if (ImGui::Button("Misc", ImVec2(TabButtonWidth, 30.f)))
                Tab = WindowTab::Misc;
            ImGui::EndDisabled();

            switch (Tab) {
            case WindowTab::Aimbot:
                AimbotTab();
                break;
            case WindowTab::TriggerBot:
                TriggerBotTab();
                break;
            case WindowTab::Visuals:
                VisualsTab();
                break;
            case WindowTab::Exploits:
                ExploitsTab();
                break;
            case WindowTab::Keybinds:
                KeybindsTab();
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
