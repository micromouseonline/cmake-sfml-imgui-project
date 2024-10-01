

#include <imgui.h>
#include <string>

#include "imgui-knobs.h"
#include "imgui_toggle.h"

/**
 * Right justifies the given string within the specified width.
 *
 * @param s the input string to be right-justified
 * @param width the width within which the string should be right-justified
 *
 * @return void
 *
 * @throws None
 */
void right_justify(std::string s, int width) {
  auto posX = (ImGui::GetCursorPosX() + width - ImGui::CalcTextSize(s.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
  if (posX > ImGui::GetCursorPosX())
    ImGui::SetCursorPosX(posX);
  ImGui::Text("%s", s.c_str());
}

void imgui_toggle_demo() {
  ImGui::Begin("Toggles, radiobuttons and checkboxes");
  /// save some colour shortcuts
  const ImVec4 green(0.16f, 0.66f, 0.45f, 1.0f);
  const ImVec4 red(0.96f, 0.0f, 0.0f, 1.0f);
  const ImVec4 dark_red(0.96f, 0.0f, 0.0f, 0.25f);
  const ImVec4 light_cyan = (ImVec4)ImColor::HSV(0.5, 0.3f, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_Text, light_cyan);

  static bool values[] = {true, true, false, true, true, true, true, true};

  ImGui::Text("Only the Toggle switches change the state");
  //// Some ways to show the 8 states in Values
  right_justify("Radio Buttons:", 120);
  ImGui::SameLine(120);
  //// the leds and checkboxes have invisible but unique labels
  for (int i = 0; i < 8; i++) {
    ImGui::PushID(i);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, dark_red);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, red);
    std::string id = (std::string("##LED") + std::to_string(i));
    ImGui::RadioButton(id.c_str(), values[i]);
    ImGui::PopStyleColor(2);
    ImGui::PopID();
    ImGui::SameLine();
  }
  ImGui::NewLine();

  right_justify("Checkboxes:", 120);
  ImGui::SameLine(120);
  for (int i = 0; i < 8; i++) {
    ImGui::PushID(i);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, light_cyan);
    std::string id = (std::string("##LED") + std::to_string(i));
    ImGui::Checkbox(id.c_str(), &values[i]);
    ImGui::PopStyleColor(1);
    ImGui::PopID();
    ImGui::SameLine();
  }
  ImGui::NewLine();

  //// Buttons do not have an ID and are not in a group
  right_justify("Buttons:", 120);
  ImGui::SameLine(120);
  for (int i = 0; i < 8; i++) {
    std::string id = (std::string("##LED") + std::to_string(i));
    ImGui::PushID(i);
    ImGui::Button(values[i] ? "1" : "0", ImVec2(19, 20));
    ImGui::PopID();
    ImGui::SameLine();
  }

  ImGui::NewLine();

  /// A set of toggle switches for the values[] array state
  size_t value_index = 0;
  ImGui::Toggle("N Wall", &values[value_index++]);
  ImGui::Toggle("E Wall", &values[value_index++]);
  ImGui::Toggle("W Wall", &values[value_index++]);
  ImGui::Toggle("S Wall", &values[value_index++]);

  ImGui::Toggle("N Unseen", &values[value_index++]);
  ImGui::Toggle("E Unseen", &values[value_index++]);
  ImGui::Toggle("W Unseen", &values[value_index++]);
  ImGui::Toggle("S Unseen", &values[value_index++]);

  ImGui::NewLine();
  ImGui::Separator();
  ImGui::NewLine();
  ImGui::Text("Some Types of Toggle Switch");

  // a default and default animated toggle
  static bool state_a = true;
  ImGui::Toggle("Default Toggle", &state_a);

  static bool state_b = true;
  ImGui::Toggle("Animated Toggle", &state_b, ImGuiToggleFlags_Animated);

  // this toggle uses stack-pushed style colors to change the way it displays
  ImGui::PushStyleColor(ImGuiCol_Button, green);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, red);
  static bool state_c = true;
  ImGui::Toggle("Coloured Toggle", &state_c);
  ImGui::PopStyleColor(1);
  ImGui::PopStyleColor(1);

  /// A toggle with an indicative text
  static bool toggle_state = true;
  ImGui::Toggle(toggle_state ? "Toggled Off" : "Toggled On", &toggle_state);

  ImGui::PopStyleColor(1);
  ImGui::End();
}

float imgui_knob_demo() {
  static float value = 3.0;
  ImGui::Begin("Imgui-Knobs Demo");
  /// save some colour shortcuts
  const ImVec4 green(0.16f, 0.66f, 0.45f, 1.0f);
  const ImVec4 red(0.96f, 0.0f, 0.0f, 1.0f);
  const ImVec4 dark_red(0.96f, 0.0f, 0.0f, 0.25f);
  const ImVec4 light_cyan = (ImVec4)ImColor::HSV(0.5, 0.3f, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_Text, light_cyan);
  if (ImGuiKnobs::Knob("Ball Velocity", &value, 0.0f, 60.0f, 1.0f, "%.1f", ImGuiKnobVariant_Stepped, 168)) {
    // value was changed
  }
  ImGui::PopStyleColor(1);
  ImGui::End();
  return value;
}