#include <imgui-SFML.h>
#include <math.h>
#include <functional>
#include <random>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS  // Access to math operators
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <iostream>
#include "demo-code.h"
#include "imgui-knobs.h"
#include "implot.h"

//-----------------------------------------------------------------------------
/// Handy debug macro
#define IM_TRACE_LOCATION()                                        \
  if (ImGui::Begin("Function Trace")) {                            \
    ImGui::Text("%s(), %s::%d", __FUNCTION__, __FILE__, __LINE__); \
  }                                                                \
  ImGui::End();
//-----------------------------------------------------------------------------

const int window_width = 1600;
const int window_height = 1000;
const int ball_radius = 16;
const int bpp = 32;
const int NUM_BALLS = 30;

struct Ball {
  sf::CircleShape ball;
  sf::Vector2f direction;
};

Ball balls[NUM_BALLS];

void init_balls() {
  std::random_device seed_device;
  std::default_random_engine engine(seed_device());
  std::uniform_int_distribution<int> distribution(-16, 16);

  auto random = std::bind(distribution, std::ref(engine));
  sf::Vector2f direction(random(), random());
  float velocity = std::sqrt(direction.x * direction.x + direction.y * direction.y);
  for (int i = 0; i < NUM_BALLS; i++) {
    balls[i].direction = sf::Vector2f(random(), random());
    balls[i].ball.setRadius(ball_radius);
    balls[i].ball.setFillColor(sf::Color::Yellow);
    balls[i].ball.setOrigin(ball_radius, ball_radius);
    balls[i].ball.setPosition(window_width / 2, window_height / 2);
    balls[i].ball.setPosition(ball_radius + rand() % (window_width - 2 * ball_radius), ball_radius + rand() % (window_height - 2 * ball_radius));
    balls[i].ball.move(random(), random());
  }
}

void update_balls(float velocity, float delta_time) {
  for (int i = 0; i < NUM_BALLS; i++) {
    const auto pos = balls[i].ball.getPosition();
    float delta = delta_time * velocity;

    sf::Vector2f new_pos(pos.x + balls[i].direction.x * delta, pos.y + balls[i].direction.y * delta);

    if (new_pos.x - ball_radius < 0) {  // left window edge
      balls[i].direction.x *= -1;
      new_pos.x = 0 + ball_radius;
    } else if (new_pos.x + ball_radius >= window_width) {  // right window edge
      balls[i].direction.x *= -1;
      new_pos.x = window_width - ball_radius;
    } else if (new_pos.y - ball_radius < 0) {  // top of window
      balls[i].direction.y *= -1;
      new_pos.y = 0 + ball_radius;
    } else if (new_pos.y + ball_radius >= window_height) {  // bottom of window
      balls[i].direction.y *= -1;
      new_pos.y = window_height - ball_radius;
    }
    balls[i].ball.setPosition(new_pos);
  }
}

int main() {
  /// not used - just an aide-memoir
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::string currentPathString = currentPath.string();

  /// the clock is essential for updating displays - possibly only ImGui though
  sf::Clock clock;

  auto window = sf::RenderWindow{{window_width, window_height}, "CMake SFML Project"};
  int x = ImGui::SFML::Init(window);  // compiler warning if we discard the result
  ImGuiIO &io = ImGui::GetIO();
  // TODO: find out more about these options
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport /
                                                         // Platform Windows

  //// We will use this font in some ImGui stuff
  auto dot_matrix_font = io.Fonts->AddFontFromFileTTF("assets/dotmatrix.ttf", 16);
  if (!ImGui::SFML::UpdateFontTexture()) {
    std::cerr << "ERROR: Cannot load font file\n";
  }

  //// And this font for SFML
  //// The font file path is relative to the binary directory
  //// Or you must be able to determine an absolute path at runtime
  //// This SFML font setting is entirely independant of ImGui
  sf::Font font;
  font.loadFromFile("assets/consolas.ttf");

  sf::Text text;
  text.setFont(font);
  text.setString("SFML font - consolas");
  text.setCharacterSize(20);
  //// or do it all in one:
  // sf::Text text("SFML font - consolas", font, 24);
  text.setFillColor(sf::Color(192, 192, 255, 255));
  text.setPosition(10, 10);  // Set position on the window

  ImGui::CreateContext();   // not sure this is needed
  ImPlot::CreateContext();  // but this must come afterwards TODO:: Find out why this is needed

  /// we can set the frame rate in a number of ways
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(60);

  init_balls();
  while (window.isOpen()) {
    ////  USER IO  ////////////////////////////////////////////////////////////
    for (auto event = sf::Event{}; window.pollEvent(event);) {
      ImGui::SFML::ProcessEvent(window, event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }

    ////  UPDATE OBJECTS  /////////////////////////////////////////////////////
    auto delta_time = clock.restart();

    /// Call this BEFORE any ImGui functions in the frame
    ImGui::SFML::Update(window, delta_time);

    /************************************************************************/
    /// the ImGui windows do not need to be invoked inside the render section
    /// They actually get drawn by ImGui::SFML::Render()
    /// All the operations just add to a draw list
    float velocity = imgui_knob_demo();
    imgui_toggle_demo();

    update_balls(velocity, delta_time.asSeconds());
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();

    /************************************************************************/
    //// A text label that follows one of the balls
    std::string name_string = "JUPITER";
    /// Create a window that only displays a string
    ImGui::PushFont(dot_matrix_font);
    ImGui::Begin("name", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(" %s", name_string.c_str());
    /// tie it to one of the balls
    ImGui::SetWindowPos(balls[0].ball.getPosition() - ImVec2(0, ball_radius + ImGui::GetWindowHeight()));
    ImGui::End();
    ImGui::PopFont();

    /************************************************************************/

    ////  RENDER OBJECTS  ////////////////////////////////////////////////////
    window.clear();
    for (int i = 0; i < NUM_BALLS; i++) {
      window.draw(balls[i].ball);
    }

    window.draw(text);
    /// put this last for everything to be on top of window.
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
