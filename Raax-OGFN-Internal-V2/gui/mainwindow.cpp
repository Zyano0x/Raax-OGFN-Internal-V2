#include <gui/gui.h>
#include <extern/imgui/imgui.h>
#include <config/config.h>

enum class WindowTab {
	Aimbot = 0,
	Visuals,
	Exploits,
	Config,
	Misc,
};
static WindowTab g_Tab = WindowTab::Aimbot;


void AimbotTab() {
	if (ImGui::BeginChild("ConfigRegion")) {
		ImGui::Checkbox("Aimbot Enabled", &Config::g_Config.Aimbot.Enabled);
		ImGui::SliderFloat("Smoothness", &Config::g_Config.Aimbot.Smoothness, 1.f, 20.f);
	}
	ImGui::EndChild();
}

void VisualsTab() {
	enum class WindowSubTab {
		Player = 0,
		Loot
	};
	static WindowSubTab SubTab = WindowSubTab::Player;

	if (ImGui::Button("Player"))
		SubTab = WindowSubTab::Player;
	ImGui::SameLine();
	if (ImGui::Button("Loot"))
		SubTab = WindowSubTab::Loot;

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
			ImGui::Combo("Minimum Loot Tier", (int*)&Config::g_Config.Visuals.Loot.MinLootTier, "Common\000Uncommon\000Rare\000Epic\000Legendary\000Mythic");
			ImGui::Checkbox("Chest Text", &Config::g_Config.Visuals.Loot.ChestText);
			ImGui::Checkbox("Ammo Box Text", &Config::g_Config.Visuals.Loot.AmmoBoxText);
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
	static char InputConfigData[4096];
	static bool InputConfigIsValid = false;
	if (ImGui::BeginChild("ConfigRegion")) {
		if (ImGui::InputText("Config Data", InputConfigData, sizeof(InputConfigData))) {
			Config::ConfigData TestConfig = {};
			InputConfigIsValid = TestConfig.DeserializeConfig(InputConfigData, TestConfig);
		}

		if (ImGui::Button("Copy Config")) {
			ImGui::SetClipboardText(Config::g_Config.SerializeConfig().c_str());
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
		ImGui::Text("Thank you for using my cheat! I hope you find it fun and useful.");
		ImGui::Text("discord.gg/Sde5mtbQe6 - github.com/raax7");

		ImGui::ColorEdit3("Primary Color Visible", (float*)&Config::g_Config.Color.PrimaryColorVisible);
		ImGui::ColorEdit3("Primary Color Hidden", (float*)&Config::g_Config.Color.PrimaryColorHidden);
		ImGui::ColorEdit3("Secondary Color Visible", (float*)&Config::g_Config.Color.SecondaryColorVisible);
		ImGui::ColorEdit3("Secondary Color Hidden", (float*)&Config::g_Config.Color.SecondaryColorHidden);
	}
	ImGui::EndChild();
}


void GUI::TickMainWindow() {
	if (ImGui::IsKeyReleased(ImGuiKey_Insert)) {
		GUI::WindowOpen = !GUI::WindowOpen;
	}

	if (GUI::WindowOpen) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 5.f, ImColor(0.f, 0.f, 0.f));
		ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 4.f, ImColor(1.f, 1.f, 1.f));

		ImGui::SetNextWindowSize(ImVec2((ImGui::GetStyle().WindowPadding.x * 6.f) + (120.f * 5), 400.f));
		if (ImGui::Begin("Raax-OGFN-Internal V2", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
			if (ImGui::Button("Aimbot", ImVec2(120.f, 30.f)))
				g_Tab = WindowTab::Aimbot;
			ImGui::SameLine();
			if (ImGui::Button("Visuals", ImVec2(120.f, 30.f)))
				g_Tab = WindowTab::Visuals;
			ImGui::SameLine();
			if (ImGui::Button("Exploits", ImVec2(120.f, 30.f)))
				g_Tab = WindowTab::Exploits;
			ImGui::SameLine();
			if (ImGui::Button("Config", ImVec2(120.f, 30.f)))
				g_Tab = WindowTab::Config;
			ImGui::SameLine();
			if (ImGui::Button("Misc", ImVec2(120.f, 30.f)))
				g_Tab = WindowTab::Misc;

			switch (g_Tab) {
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
