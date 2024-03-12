#include <imgui-SFML.h>
#include <math.h>
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS  // Access to math operators
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "imgui-knobs.h"

#include "demo-code.h"

//-----------------------------------------------------------------------------
#define IM_TRACE_LOCATION()                                        \
  if (ImGui::Begin("Function Trace")) {                            \
    ImGui::Text("%s(), %s::%d", __FUNCTION__, __FILE__, __LINE__); \
  }                                                                \
  ImGui::End();

int main() {
  auto window = sf::RenderWindow{{1800u, 1024u}, "CMake SFML Project"};
  window.setFramerateLimit(144);
  int x = ImGui::SFML::Init(window);
  sf::Clock clock;
  while (window.isOpen()) {
    for (auto event = sf::Event{}; window.pollEvent(event);) {
      ImGui::SFML::ProcessEvent(window, event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    ImGui::SFML::Update(window, clock.restart());

    IM_TRACE_LOCATION();

    ImGui::Begin("Hello");
    ImGui::Button("Look at this pretty button");
    static float value = 3.0;

    if (ImGuiKnobs::Knob("Volume", &value, -6.0f, 6.0f, 0.1f, "%.1fdB", ImGuiKnobVariant_Stepped, 168)) {
      // value was changed
    }
    IM_TRACE_LOCATION();

    ImGui::End();
    imgui_toggle_demo();
    ImGui::ShowDemoWindow();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
