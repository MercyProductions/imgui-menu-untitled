#define IMGUI_DEFINE_MATH_OPERATORS
#include "stdafx.h"
#include "Menu.h"
#include "imgui/imgui.h"
#include "globals.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "obfuscator.hpp"
#include "xor.hpp"
#include "memory.h"
#include "mem.h"
#include "xorstr.hpp"
#include "style.h"
#include "custom.h"
#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

bool dark = true;

char field[45] = { "" };

int page = 0;

static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

int key, m;

bool checkbox_on = true;
bool checkbox_off = false;

static int slider_int = 50;
static float slider_float = 0.5f;

static int select1 = 0;
const char* items[3]{ "Selected", "Nope", "what :/ a y?" };

static int bullet_select = 0;
const char* bullet[2]{ "Disabled", "Enabled" };

static int sound_select = 0;
const char* sound[2]{ "Disabled", "Enabled" };

static int style_select = 0;
const char* stylee[2]{ "Flat", "Back" };

static int style2_select = 0;
const char* stylee2[2]{ "Textured", "3D Mode" };

static bool multi_num[5] = { false, true, true, true, false };
const char* multi_items[5] = { "One", "Two", "Three", "Four", "Five" };

float knob = 1.f;

float col[4] = { 118 / 255.f, 187 / 255.f, 117 / 255.f, 0.5f };

int rotation_start_index;
void ImRotateStart()
{
	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter()
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
	float s = sin(rad), c = cos(rad);
	center = ImRotate(center, s, c) - center;

	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

bool b_menu_open = true;
bool b_debug_open = false;
bool boxcheck;
int Selected_Camo_MW = 0;
int Selected_Camo_CW = 0;
int Selected_Camo_VG = 0;
int gameMode2 = 0;
int i_MenuTab = 0;
uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	if (ptr != 0)
	{
		uintptr_t addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		return addr;
	}
	else
		return 0;
}


//uint64_t BASEIMAGE2 = reinterpret_cast<uint64_t>(GetModuleHandleA(NULL));

bool b_fov = false;
float f_fov = 1.20f;
float f_map = 1.0f;
bool b_map = false;
bool b_brightmax = false;
bool b_thirdperson = false;
bool b_heartcheat = false;
bool b_norecoil = false;
bool b_no_flashbang = false;

struct unnamed_type_integer
{
	int min;
	int max;
};
struct unnamed_type_integer64
{
	__int64 min;
	__int64 max;
};
struct unnamed_type_enumeration
{
	int stringCount;
	const char* strings;
};
/* 433 */
struct unnamed_type_unsignedInt64
{
	unsigned __int64 min;
	unsigned __int64 max;
};

/* 434 */
struct unnamed_type_value
{
	float min;
	float max;
	float devguiStep;
};

/* 435 */
struct unnamed_type_vector
{
	float min;
	float max;
	float devguiStep;
};




uintptr_t cbuff1;
uintptr_t cbuff2;
char inputtext[50];
int C_TagMOde = 0;


__int64 find_pattern(__int64 range_start, __int64 range_end, const char* pattern) {
	const char* pat = pattern;
	__int64 firstMatch = NULL;
	__int64 pCur = range_start;
	__int64 region_end;
	MEMORY_BASIC_INFORMATION mbi{};
	while (sizeof(mbi) == VirtualQuery((LPCVOID)pCur, &mbi, sizeof(mbi))) {
		if (pCur >= range_end - strlen(pattern))
			break;
		if (!(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
			pCur += mbi.RegionSize;
			continue;
		}
		region_end = pCur + mbi.RegionSize;
		while (pCur < region_end)
		{
			if (!*pat)
				return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch)
					firstMatch = pCur;
				if (!pat[1] || !pat[2])
					return firstMatch;

				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;
				else
					pat += 2;
			}
			else {
				if (firstMatch)
					pCur = firstMatch;
				pat = pattern;
				firstMatch = 0;
			}
			pCur++;
		}
	}
	return NULL;
}


bool init_once = true;
//char input[30];
bool Unlock_once = true;

void Visual()
{
	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	ImGui::Spacing();
	ImGui::Checkbox(xorstr_("Check Visibility"), &globals::b_visible);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip(xorstr_("Can only be turned on INGAME!"));
	}
	ImGui::Checkbox(xorstr_("Show Box"), &globals::b_box);
	/*ImGui::SameLine();
	ImGui::Combo("##", &globals::box_index, globals::box_types, 2);*/

	//ImGui::Checkbox(xorstr_("Show HealthBar"), &globals::b_health);
	ImGui::Checkbox(xorstr_("Show Line"), &globals::b_line);
	ImGui::Checkbox(xorstr_("Show Bones "), &globals::b_skeleton);
	ImGui::Checkbox(xorstr_("Show Names"), &globals::b_names);
	ImGui::Checkbox(xorstr_("Show Distance"), &globals::b_distance);
	ImGui::Checkbox(xorstr_("Show Team"), &globals::b_friendly);
	ImGui::SliderInt(xorstr_("##MAXDISTANCE"), &globals::max_distance, 0, 1000, xorstr_("ESP Distance: %d"));
}
void KeyBindButton(int& key, int width, int height)
{
	static auto b_get = false;
	static std::string sz_text = xorstr_("Click to bind.");

	if (ImGui::Button(sz_text.c_str(), ImVec2(static_cast<float>(width), static_cast<float>(height))))
		b_get = true;

	if (b_get)
	{
		for (auto i = 1; i < 256; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				if (i != 12)
				{
					key = i == VK_ESCAPE ? -1 : i;
					b_get = false;
				}
			}
		}
		sz_text = xorstr_("Press a Key.");
	}
	else if (!b_get && key == -1)
		sz_text = xorstr_("Click to bind.");
	else if (!b_get && key != -1)
	{
		sz_text = xorstr_("Key ~ ") + std::to_string(key);
	}
}
void Aimbot()
{

	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	ImGui::Spacing();
	ImGui::Checkbox(xorstr_("Enable"), &globals::b_lock);
	if (globals::b_lock)
	{
		ImGui::SliderInt(xorstr_("##LOCKSMOOTH"), &globals::aim_smooth, 1, 30, xorstr_("Lock Smooth: %d"));
	}
	ImGui::Checkbox(xorstr_("Crosshair"), &globals::b_crosshair);
	ImGui::Checkbox(xorstr_("Show FOV"), &globals::b_fov);
	if (globals::b_fov)
	{
		ImGui::SliderFloat(xorstr_("##LOCKFOV"), &globals::f_fov_size, 10.f, 800.f, xorstr_("FOV Size: %0.0f"));
	}
	ImGui::Checkbox(xorstr_("Skip Knocked"), &globals::b_skip_knocked);

	
	
	/*ImGui::Checkbox(xorstr_("Prediction"), &globals::b_prediction);*/

	
	

	/*ImGui::Checkbox(xorstr_("Use Bones"), &globals::target_bone);
	if (globals::target_bone)
	{
		ImGui::Combo(xorstr_("Lock Bone"), &globals::bone_index, globals::aim_lock_point, 4);
	}*/

	KeyBindButton(globals::aim_key, 100, 30);
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::Spacing();

	

}
void ColorPicker()
{
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::Text("Fov");
	ImGui::ColorEdit4("##Fov Color7", (float*)&color::bfov);
	ImGui::Text("Crosshair");
	ImGui::ColorEdit4("##cross hair Color9", (float*)&color::draw_crosshair);
	ImGui::Text("Visible Team");
	ImGui::ColorEdit4("##esp Color1", (float*)&color::VisibleColorTeam);
	ImGui::Spacing();
	ImGui::Text("Not Visible Team");
	ImGui::ColorEdit4("##esp Color2", (float*)&color::NotVisibleColorTeam);
	ImGui::Spacing();
	ImGui::Text("Visible Enemy");
	ImGui::ColorEdit4("##esp Color3", (float*)&color::VisibleColorEnemy);
	ImGui::Spacing();
	ImGui::Text("Not Visible Enemy");
	ImGui::ColorEdit4("##esp Color4", (float*)&color::NotVisibleColorEnemy);
	/*ImGui::Spacing();
	ImGui::Text("Names");
	ImGui::ColorEdit4("##esp Color5", (float*)&color::nameColor);
	ImGui::Text("Distance");
	ImGui::ColorEdit4("##esp Color6", (float*)&color::dis_Color);*/
	//ImGui::Text("loot Color");
	//ImGui::ColorEdit4("##esp lootcolor", (float*)&loot::lootcolor);
	//ImGui::EndTabItem();

}
void CL_PlayerData_SetCustomClanTag(int controllerIndex, const char* clanTag) {

}
void ShowToastNotificationAfterUserJoinedParty(const char* message)
{

}
void SetCamo(int Class, int Weapon, int Camo)
{
	

}
void Misc()
{
	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	ImGui::Checkbox(xorstr_("UAV"), &globals::b_UAV);
	/*if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip(xorstr_("Use at your own Risk"));
	}*/
	ImGui::Spacing();

	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	ImGui::Spacing();
	/*if (ImGui::SliderFloat("FOV SLIDER", &globals::f_fov, 0.1f, 4.f, "%.2f"))
	{
		dvar_set2("NSSLSNKPN", globals::f_fov);
	}
	ImGui::Spacing();*/
	if (ImGui::Button(xorstr_("Unlock All"), ImVec2(150, 30)))
	{

		sdk::unlockall();
		//ShowToastNotificationAfterUserJoinedParty("^4Sim^2ple^5Too^2lZ ^3Unlock^1ed ^5Everything! ^1<3");
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip(xorstr_("Use at your own Risk! Use AC Blocker!"));
	}
	ImGui::Dummy(ImVec2(0.0f, 3.0f));
	//ImGui::Checkbox(xorstr_("No Recoil"), &globals::b_recoil);
	
	ImGui::Spacing();
	
	
	static char customtag[20];
	ImGui::Spacing();
	ImGui::InputTextWithHint(xorstr_("##KeyInput"), xorstr_("Custom Clan Tag"), customtag, 20);
	ImGui::SameLine();
	if (ImGui::Button(xorstr_("Set"), ImVec2(50, 20)))
	{
		CL_PlayerData_SetCustomClanTag(0, customtag);
	}




}


namespace g_menu
{
	void menu()
	{
		static float color[4] = { 72 / 255.f, 255 / 255.f, 84 / 255.f, 1.f };

		accent_color = ImLerp(accent_color, ImColor(color[0], color[1], color[2]), ImGui::GetIO().DeltaTime * 8.f);
		accent_alpha = ImLerp(accent_alpha, ImColor(color[0], color[1], color[2], 0.f), ImGui::GetIO().DeltaTime * 8.f);
		accent_color_low = ImLerp(accent_color_low, ImColor(color[0] / 2, color[1] / 2, color[2] / 2), ImGui::GetIO().DeltaTime * 8.f);

		ImGui::GetStyle().WindowPadding = ImVec2(0, 0), ImGui::GetStyle().WindowBorderSize = 0;
		ImGui::GetStyle().ItemSpacing = ImVec2(20, 20);
		ImGui::GetStyle().ScrollbarSize = 8.f;

		ImGui::SetNextWindowSizeConstraints(ImVec2(bg::size.x, bg::size.y), ImGui::GetIO().DisplaySize);

		ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
		{
			const auto& p = ImGui::GetWindowPos();

			ImGui::GetBackgroundDrawList()->AddRectFilled(p + ImVec2(0, 0), p + ImVec2(ImGui::GetContentRegionMax()), ImGui::GetColorU32(bg::bg), bg::rounding, ImDrawListFlags_None);

			static int tabs = 0;
			const char* ico_array[6] = { "A", "B", "C", "D", "E", "F" };
			const char* name_array[6] = { "RageBot", "AntiAim", "Visuals", "Changer", "Misc", "Settings" };

			for (int i = 0; i < 6; i++)
				ImGui::SetCursorPos(ImVec2((ImGui::GetContentRegionMax().x / 2) - (77 / 2) * 6, 0));

			ImGui::BeginGroup();
			{

				for (int i = 0; i < 6; i++)
					if (Custom::Tabs(i == tabs, ico_array[i], name_array[i], ImVec2(77, 82), ImGuiButtonFlags_None)) tabs = i;

			}
			ImGui::EndGroup();

			ImGui::GetBackgroundDrawList()->AddLine(p + ImVec2(0, 82), p + ImVec2(ImGui::GetContentRegionMax().x, 82), ImGui::GetColorU32(child::border), 1.f);

			static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

			tab_alpha = ImClamp(tab_alpha + (4.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
			tab_add = ImClamp(tab_add + (std::round(350.f) * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

			if (tab_alpha == 0.f && tab_add == 0.f)
				active_tab = tabs;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * ImGui::GetStyle().Alpha);

			ImGui::SetCursorPos(ImVec2(20, 102));

			if (active_tab == 0) {

				ImGui::BeginGroup();
				{
					Custom::BeginChild("Main", ImVec2((ImGui::GetContentRegionMax().x / 2) - (ImGui::GetStyle().ItemSpacing.x / 2) - 20, ImGui::GetContentRegionMax().y - 122), false, ImGuiWindowFlags_None);
					{
						static bool checkbox = false;
						Custom::Checkbox("Checkbox", &checkbox);

						Custom::Separator();

						static bool switchbox = false;
						Custom::Switchbox("Switchbox", &switchbox);

						Custom::Separator();

						static int slider = 0;
						Custom::SliderInt("Slider Integer", &slider, 0, 1000, nullptr, ImGuiSliderFlags_None);

						Custom::Separator();

						static float slider_float = 0;
						Custom::SliderFloat("Slider Float", &slider_float, 0.f, 10.f, "%.1f", ImGuiSliderFlags_None);

						Custom::Separator();

						static int select = 0;
						const char* items[3]{ "One", "Two", "Three" };
						Custom::Combo("Combo", &select, items, IM_ARRAYSIZE(items), 3);

						Custom::Separator();

						static bool multi_num[5] = { false, true, true, true, false };
						const char* multi_items[5] = { "One", "Two", "Three", "Four", "Five" };
						Custom::MultiCombo("MultiCombo", multi_num, multi_items, 5);

						Custom::Separator();

						static char buf[65] = { "" };
						Custom::InputTextWithHint("TextField", "your text..", buf, 65, ImGuiInputTextFlags_None, NULL, NULL);

						Custom::Separator();

						Custom::ColorEdit4("ColorPicker", color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);

						Custom::Separator();

						static int key = 0, m = 0;
						Custom::Keybind("Keybind", &key, &m);

						Custom::Separator();

						ImGui::SetCursorPosY(2000);

					}
					Custom::EndChild();
				}
				ImGui::EndGroup();

				ImGui::SameLine();

				ImGui::BeginGroup();
				{
					Custom::BeginChild("Settings", ImVec2((ImGui::GetContentRegionMax().x / 2) - (ImGui::GetStyle().ItemSpacing.x / 2) - 20, (ImGui::GetContentRegionMax().y / 2) - (122 / 2) - ImGui::GetStyle().ItemSpacing.y / 2), false, ImGuiWindowFlags_None);
					{

						if (Custom::Button("Button##0", ImVec2(ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x, 25), ImGuiButtonFlags_None));

						if (Custom::Button("Button##1", ImVec2(ImGui::GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x, 25), ImGuiButtonFlags_None));

						Custom::Separator();

					}
					Custom::EndChild();

					Custom::BeginChild("Test", ImVec2((ImGui::GetContentRegionMax().x / 2) - (ImGui::GetStyle().ItemSpacing.x / 2) - 20, (ImGui::GetContentRegionMax().y / 2) - (122 / 2) - ImGui::GetStyle().ItemSpacing.y / 2), false, ImGuiWindowFlags_None);
					{

						static float color1[4] = { 157 / 255.f, 141 / 255.f, 255 / 255.f, 1.f };
						Custom::ColorEdit4("ColorPicker", color1, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);

						static float color2[4] = { 58 / 255.f, 131 / 255.f, 255 / 255.f, 1.f };
						Custom::ColorEdit4("ColorPicker ", color2, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);

						static float color3[4] = { 255 / 255.f, 110 / 255.f, 110 / 255.f, 1.f };
						Custom::ColorEdit4("ColorPicker  ", color3, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);

						Custom::Separator();

					}
					Custom::EndChild();
				}
				ImGui::EndGroup();

			}

			ImGui::PopStyleVar();

		} ImGui::End();
	}
}