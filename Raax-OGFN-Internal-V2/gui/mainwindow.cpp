#include "mainwindow.h"

#include <extern/imgui/imgui.h>
#define NOMINMAX
#include <config/config.h>
#include <config/keybind.h>
#include <config/config_reflection.h>
#include <gui/gui.h>
#include <cheat/sdk/Engine.h>

#ifdef _ENGINE
#include <extern/raaxgui/raaxgui.h>
#endif

namespace GUI {
namespace MainWindow {

#ifndef _ENGINE
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
        if (ImGui::BeginChild("TargetSelection", ImVec2(0, 200), ImGuiChildFlags_Borders)) {
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

    static size_t EditingIdx = -1;
    static bool   bEditing = false;

    if (ImGui::BeginChild("KeybindList", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        size_t PendingDeleteIdx = -1;

        if (Config::g_Config.Keybinds.Keybinds.empty()) {
            ImGui::Text("No keybinds!");
        } else {
            for (size_t i = 0; i < Config::g_Config.Keybinds.Keybinds.size(); i++) {
                auto& Keybind = Config::g_Config.Keybinds.Keybinds[i];

                std::string KeybindInfoStr =
                    std::format("{} - {}", ImGui::GetKeyName(Keybind.Keybind), Keybind.ReflectedBool.Name.data());
                ImGui::PushID(&Keybind);
                ImGui::BeginDisabled(bEditing && i != EditingIdx);

                if (ImGui::Button("Delete")) {
                    PendingDeleteIdx = i;
                }
                ImGui::SameLine();
                GUI::Keybind("Key", bEditing, Keybind.Keybind);
                if (bEditing && EditingIdx == -1) {
                    EditingIdx = i;
                }
                if (!bEditing && EditingIdx != -1) {
                    EditingIdx = -1;
                }
                ImGui::SameLine();
                ImGui::Text(Keybind.ReflectedBool.Name.data());

                ImGui::EndDisabled();
                ImGui::PopID();
            }
        }

        if (PendingDeleteIdx != -1) {
            Config::g_Config.Keybinds.Keybinds.erase(Config::g_Config.Keybinds.Keybinds.begin() + PendingDeleteIdx);
        }
    }

    DisplayKeybindPopup(CurrentKeybind, "Keybind (Edit)", false);
    ImGui::EndChild();
}

// --- Window Tab Functions & Data -----------------------------------

void AimbotTab() {
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

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::Checkbox("Split By Ammo Type", &AimbotConfig.SplitAimbotByAmmo);
        ImGui::SameLine();
        ImGui::Checkbox("Bullet Prediction", &AimbotConfig.BulletPrediction);
        ImGui::SameLine();
        ImGui::Checkbox("Show Target Line", &AimbotConfig.ShowTargetLine);
        static bool WaitingForKeybind = false;
        GUI::Keybind("Aim Keybind", WaitingForKeybind, (ImGuiKey&)AimbotConfig.AimbotKeybind);

        if (ImGui::BeginChild("AimbotConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
            ImGui::BeginTabBar("Aimbot Tab");

            static bool AllWeaponsSelected = false;

            ImGui::BeginDisabled(!AllWeaponsSelected && AimbotConfig.SplitAimbotByAmmo);
            if (ImGui::BeginTabItem("All Weapons", nullptr,
                                    AimbotConfig.SplitAimbotByAmmo ? ImGuiTabItemFlags_None
                                                                   : ImGuiTabItemFlags_SetSelected)) {
                AllWeaponsSelected = true;
                DisplayAimbotConfig(AimbotConfig.AllAimbot);
                ImGui::EndTabItem();
            } else {
                AllWeaponsSelected = false;
            }
            ImGui::EndDisabled();

            ImGui::BeginDisabled(!AimbotConfig.SplitAimbotByAmmo);
            if (ImGui::BeginTabItem("Shells", nullptr,
                                    (AllWeaponsSelected && AimbotConfig.SplitAimbotByAmmo)
                                        ? ImGuiTabItemFlags_SetSelected
                                        : ImGuiTabItemFlags_None)) {
                DisplayAimbotConfig(AimbotConfig.ShellsAimbot);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Light")) {
                DisplayAimbotConfig(AimbotConfig.LightAimbot);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Medium")) {
                DisplayAimbotConfig(AimbotConfig.MediumAimbot);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Heavy")) {
                DisplayAimbotConfig(AimbotConfig.HeavyAimbot);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Other")) {
                DisplayAimbotConfig(AimbotConfig.OtherAimbot);
                ImGui::EndTabItem();
            }
            ImGui::EndDisabled();

            ImGui::EndTabBar();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void TriggerBotTab() {
    auto& Config = Config::g_Config.TriggerBot;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::Checkbox("Enabled", &Config.Enabled);

        ImGui::Checkbox("Use Keybind", &Config.UseKeybind);
        ImGui::SameLine();
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
    auto& Config = Config::g_Config.Visuals;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::BeginTabBar("Visuals Tab");
        if (ImGui::BeginTabItem("Player")) {
            ImGui::SliderFloat("Max Distance", &Config.Player.MaxDistance, 0.f, 500.f);

            ImGui::Checkbox("Box", &Config.Player.Box);
            if (Config.Player.Box) {
                ImGui::Combo("Box Type", (int*)&Config.Player.BoxType, "Full\000Cornered\000Full 3D");
                ImGui::SliderFloat("Box Thickness", &Config.Player.BoxThickness, 1.f, 20.f);
                ImGui::Checkbox("Filled Box", &Config.Player.FilledBox);
                if (Config.Player.FilledBox)
                    ImGui::ColorEdit4("Filled Box Color", (float*)&Config.Player.FilledBoxColor);
            }

            ImGui::Checkbox("Skeleton", &Config.Player.Skeleton);
            if (Config.Player.Skeleton)
                ImGui::SliderFloat("Skeleton Thickness", &Config.Player.SkeletonThickness, 1.f, 20.f);

            ImGui::Checkbox("Tracer", &Config.Player.Tracer);
            if (Config.Player.Tracer) {
                ImGui::SliderFloat("Tracer Thickness", &Config.Player.TracerThickness, 1.f, 20.f);
                ImGui::Combo("Tracer Start", (int*)&Config.Player.TracerStart, "Top\000Middle\000Bottom");
                ImGui::Combo("Tracer End", (int*)&Config.Player.TracerEnd, "Top\000Middle\000Bottom");
            }

            ImGui::Checkbox("Platform", &Config.Player.Platform);
            ImGui::Checkbox("Name", &Config.Player.Name);
            ImGui::Checkbox("Current Weapon", &Config.Player.CurrentWeapon);
            ImGui::Checkbox("Distance", &Config.Player.Distance);

            ImGui::Checkbox("OSI", &Config.Player.OSI);
            if (Config.Player.OSI) {
                ImGui::Checkbox("OSI Match FOV", &Config.Player.OSIMatchFOV);
                ImGui::SliderFloat("OSI FOV", &Config.Player.OSIFOV, 0.25f, 180.f);
                ImGui::SliderFloat("OSI Size", &Config.Player.OSISize, 1.f, 20.f);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Radar")) {
            float MaxPosition = 100.f - Config.Radar.Size;
            Config.Radar.PosX = std::min(Config.Radar.PosX, MaxPosition);
            Config.Radar.PosY = std::min(Config.Radar.PosY, MaxPosition);

            ImGui::Checkbox("Enable Radar", &Config.Radar.Radar);
            ImGui::Checkbox("Rotate With Camera", &Config.Radar.RotateWithCamera);
            ImGui::Checkbox("Show Camera FOV", &Config.Radar.ShowCameraFOV);
            ImGui::Checkbox("Show Guidelines", &Config.Radar.ShowGuidelines);
            ImGui::SliderFloat("Max Distance", &Config.Radar.MaxDistance, 1.f, 500.f);
            ImGui::SliderFloat("Position X", &Config.Radar.PosX, 0.f, MaxPosition);
            ImGui::SliderFloat("Position Y", &Config.Radar.PosY, 0.f, MaxPosition);
            ImGui::SliderFloat("Size", &Config.Radar.Size, 3.f, 100.f);
            ImGui::ColorEdit4("Background Color", (float*)&Config.Radar.BackgroundColor);
            ImGui::ColorEdit4("Color Visible", (float*)&Config.Radar.ColorVisible);
            ImGui::ColorEdit4("Color Hidden", (float*)&Config.Radar.ColorHidden);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Loot")) {
            ImGui::Checkbox("Loot Text", &Config.Loot.LootText);
            if (Config.Loot.LootText) {
                ImGui::Combo("Loot Minimum Tier", (int*)&Config.Loot.LootMinTier,
                             "Common\000Uncommon\000Rare\000Epic\000Legendary\000Mythic");
                ImGui::Checkbox("Loot Fade Off", &Config.Loot.LootFadeOff);
                ImGui::SameLine();
                ImGui::SliderFloat("Loot Max Distance", &Config.Loot.LootMaxDistance, 1.f, 1000.f);
            }

            ImGui::Checkbox("Chest Text", &Config.Loot.ChestText);
            if (Config.Loot.ChestText) {
                ImGui::Checkbox("Chest Fade Off", &Config.Loot.ChestFadeOff);
                ImGui::SameLine();
                ImGui::SliderFloat("Chest Max Distance", &Config.Loot.ChestMaxDistance, 1.f, 1000.f);
            }

            ImGui::Checkbox("Ammo Box Text", &Config.Loot.AmmoBoxText);
            if (Config.Loot.AmmoBoxText) {
                ImGui::Checkbox("Ammo Box Fade Off", &Config.Loot.AmmoBoxFadeOff);
                ImGui::SameLine();
                ImGui::SliderFloat("Ammo Box Max Distance", &Config.Loot.AmmoBoxMaxDistance, 1.f, 1000.f);
            }

            ImGui::Checkbox("Supply Drop Text", &Config.Loot.SupplyDropText);
            if (Config.Loot.SupplyDropText) {
                ImGui::Checkbox("Supply Drop Fade Off", &Config.Loot.SupplyDropFadeOff);
                ImGui::SameLine();
                ImGui::SliderFloat("Supply Drop Max Distance", &Config.Loot.SupplyDropMaxDistance, 1.f, 2500.f);
            }

            ImGui::Checkbox("Llama Text", &Config.Loot.LlamaText);
            if (Config.Loot.LlamaText) {
                ImGui::Checkbox("Llama Fade Off", &Config.Loot.LlamaFadeOff);
                ImGui::SameLine();
                ImGui::SliderFloat("Llama Max Distance", &Config.Loot.LlamaMaxDistance, 1.f, 2500.f);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colors")) {
            auto& ColorConfig = Config::g_Config.Color;
            ImGui::ColorEdit4("Primary Color Visible", (float*)&ColorConfig.PrimaryColorVisible);
            ImGui::ColorEdit4("Primary Color Hidden", (float*)&ColorConfig.PrimaryColorHidden);
            ImGui::ColorEdit4("Secondary Color Visible", (float*)&ColorConfig.SecondaryColorVisible);
            ImGui::ColorEdit4("Secondary Color Hidden", (float*)&ColorConfig.SecondaryColorHidden);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void ExploitsTab() {
    auto& Config = Config::g_Config.Exploit;

    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        ImGui::BeginTabBar("Exploits Tab");
        if (ImGui::BeginTabItem("Weapon")) {
            ImGui::Checkbox("No Spread", &Config.NoSpread);
            ImGui::SameLine();
            ImGui::SliderFloat("Multiplier##0", &Config.NoSpreadAmount, 0.f, 1.f);
            ImGui::Checkbox("No Recoil", &Config.NoRecoil);
            ImGui::SameLine();
            ImGui::SliderFloat("Multiplier##1", &Config.NoRecoilAmount, 0.f, 1.f);
            ImGui::Checkbox("No Reload", &Config.NoReload);
            ImGui::SameLine();
            ImGui::SliderFloat("Time##2", &Config.NoReloadAmount, 0.f, 1.f);

            ImGui::Checkbox("Rapid Fire", &Config.RapidFire);
            Config.RapidFireAmount *= 50.f; // scuffed fix for now
            ImGui::SameLine();
            ImGui::SliderFloat("Speed##3", &Config.RapidFireAmount, 1.f, 50.f);
            Config.RapidFireAmount /= 50.f;

            ImGui::Checkbox("Damage Multiplier", &Config.DamageMultiplier);
            Config.DamageMultiplierAmount *= 50.f; // scuffed fix for now
            ImGui::SameLine();
            ImGui::SliderFloat("Multiplier##4", &Config.DamageMultiplierAmount, 1.f, 50.f);
            Config.DamageMultiplierAmount /= 50.f;

            ImGui::Checkbox("Fast Pickaxe", &Config.FastPickaxe);
            if (Config.FastPickaxe) {
                Config.FastPickaxeSpeed *= 50.f; // scuffed fix for now
                ImGui::SameLine();
                ImGui::SliderFloat("Speed##5", &Config.FastPickaxeSpeed, 1.f, 50.f);
                Config.FastPickaxeSpeed /= 50.f;
            }

            ImGui::Checkbox("Automatic Weapons", &Config.AutomaticWeapons);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Player")) {
            ImGui::Checkbox("Zipline Fly", &Config.ZiplineFly);
            ImGui::Checkbox("Instant Revive", &Config.InstantRevive);

            static char InputName[1024];
            ImGui::InputText("New Name", InputName, sizeof(InputName));
            if (ImGui::Button("Server Change Name")) {
                std::string             InputNameStr = InputName;
                std::wstring            InputNameWStr = std::wstring(InputNameStr.begin(), InputNameStr.end());
                SDK::APlayerController* Controller = SDK::GetLocalController();
                if (Controller)
                    Controller->ServerChangeName(SDK::FString(InputNameWStr.c_str()));
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void KeybindsTab() {
    if (ImGui::BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
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
        static bool WaitingForKeybind = false;
        GUI::Keybind("Menu Keybind", WaitingForKeybind, (ImGuiKey&)Config::g_Config.MenuKeybind);

        ImGui::Text("Thank you for using my cheat! Join my Discord and star the repository!");
        ImGui::Text("Made by Raax with help from Toxy, Parkie & NotTacs");
        ImGui::Text("2ly.link/26uAo - github.com/raax7");
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

        ImGui::SetNextWindowSize(ImVec2(700.f, 375.f), ImGuiCond_Once);
        if (ImGui::Begin("Raax-OGFN-Internal V2", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::BeginTabBar("Menu Tab");
            if (ImGui::BeginTabItem("Aimbot")) {
                AimbotTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Trigger Bot")) {
                TriggerBotTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Visuals")) {
                VisualsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Exploits")) {
                ExploitsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Keybinds")) {
                KeybindsTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Config")) {
                ConfigTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Misc")) {
                MiscTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}
#else
void Tick() {
    if (GetAsyncKeyState(VK_INSERT) & 1)
        GUI::MainWindow::g_WindowOpen = !GUI::MainWindow::g_WindowOpen;

    static bool test = false;

    RaaxGUI::NewFrame();
    RaaxGUI::SetNextWindowSize(SDK::FVector2D(400.f, 600.f));
    if (RaaxGUI::Begin("test", &GUI::MainWindow::g_WindowOpen)) {
        RaaxGUI::Checkbox("Rapid Fire", &test);
    }
    RaaxGUI::End();
    RaaxGUI::EndFrame();
}
#endif

// --- Global Variables ----------------------------------------------

bool g_WindowOpen = true;

} // namespace MainWindow
} // namespace GUI
