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
  for (auto& ball : balls) {
    ball.direction = sf::Vector2f(random(), random());
    ball.ball.setRadius(ball_radius);
    ball.ball.setFillColor(sf::Color::Yellow);
    ball.ball.setOrigin(ball_radius, ball_radius);
    ball.ball.setPosition(window_width / 2, window_height / 2);
    ball.ball.setPosition(ball_radius + rand() % (window_width - 2 * ball_radius), ball_radius + rand() % (window_height - 2 * ball_radius));
    ball.ball.move(random(), random());
  }
}

void update_balls(float velocity, float delta_time) {
  for (auto& ball : balls) {
    const auto pos = ball.ball.getPosition();
    float delta = delta_time * velocity;

    sf::Vector2f new_pos(pos.x + ball.direction.x * delta, pos.y + ball.direction.y * delta);

    if (new_pos.x - ball_radius < 0) {  // left window edge
      ball.direction.x *= -1;
      new_pos.x = 0 + ball_radius;
    } else if (new_pos.x + ball_radius >= window_width) {  // right window edge
      ball.direction.x *= -1;
      new_pos.x = window_width - ball_radius;
    } else if (new_pos.y - ball_radius < 0) {  // top of window
      ball.direction.y *= -1;
      new_pos.y = 0 + ball_radius;
    } else if (new_pos.y + ball_radius >= window_height) {  // bottom of window
      ball.direction.y *= -1;
      new_pos.y = window_height - ball_radius;
    }
    ball.ball.setPosition(new_pos);
  }
}

int main() {
  /// not used - just an aide-memoir
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::string currentPathString = currentPath.string();

  /// the clock is essential for updating displays - possibly only ImGui though
  sf::Clock clock;
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine

  auto window = sf::RenderWindow{{window_width, window_height}, "CMake SFML Project", sf::Style::Default, settings};
  if (!ImGui::SFML::Init(window)) {
    std::cerr << "Failed to initialize ImGui with SFML. Please check your setup." << std::endl;
    window.close();
    return EXIT_FAILURE;
  }

  ImGuiIO& io = ImGui::GetIO();
  // TODO: find out more about these options
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport /
                                                         // Platform Windows

  //// We will use this font in some ImGui stuff
  io.Fonts->Clear();
  io.Fonts->AddFontDefault();
  auto dot_matrix_font = io.Fonts->AddFontFromFileTTF("assets/dotmatrix.ttf", 16);
  auto font1 = io.Fonts->AddFontFromFileTTF("assets/Roboto-Regular.ttf", 14.0f);
  auto font2 = io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", 14.0f);
  auto font3 = io.Fonts->AddFontFromFileTTF("assets/Roboto-Bold.ttf", 14.0f);
  auto font4 = io.Fonts->AddFontFromFileTTF("assets/ubuntu-r.ttf", 14.0f);
  io.Fonts->Build();
  if (!ImGui::SFML::UpdateFontTexture()) {
    std::cerr << "ERROR: Cannot load font file\n";
  }

  //// And this font for SFML
  //// The font file path is relative to the binary directory
  //// Or you must be able to determine an absolute path at runtime
  //// This SFML font setting is entirely independent of ImGui
  sf::Font font;
  font.loadFromFile("assets/consolas.ttf");

  sf::Text text;
  text.setFont(font);
  text.setString("SFML font - consolas");
  text.setCharacterSize(20);
  //// or do it all in one:
  //   sf::Text text("SFML font - consolas", font, 24);
  text.setFillColor(sf::Color(192, 192, 255, 255));
  text.setPosition(10, 10);  // Set position on the window

  ImGui::CreateContext();   // This function initializes the ImGui context, which is essential for managing the state and resources used by ImGui.
  ImPlot::CreateContext();  // initializes the ImPlot context, which is necessary for managing the state and resources specific to ImPlot. It must come after
                            // ImGui::CreateContext() because ImPlot relies on the ImGui context being already set up.

  /// we can set the frame rate in a number of ways
  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(60);

  init_balls();
  sf::Clock frame_time;
  sf::Time elapsed = frame_time.restart();
  while (window.isOpen()) {
    frame_time.restart();
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

    /*********************/
    //// Let's plot some data - because we can
    ImGui::Begin("data plot", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    static int alpha_index = 1;
    static float alpha_list[] = {0.01f, 0.025f, 0.05f, 0.1f, 0.2f};
    const char* alpha_items[] = {"0.01f", "0.025f", "0.05f", "0.1f", "0.2f"};

    static float adc1arr[100];
    float alpha = alpha_list[alpha_index];
    for (int i = 0; i < 100; i++) {
      float d = float(rand() % 200) - 100.0f;
      adc1arr[i] = alpha * d + (1 - alpha) * adc1arr[i];
    }
    ImGui::PlotLines("##Noise", adc1arr, IM_ARRAYSIZE(adc1arr), 0, "EXPONENTIALLY FILTERED NOISE ", -100, 100, ImVec2(00, 100), 4);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::Combo("Alpha", &alpha_index, alpha_items, IM_ARRAYSIZE(alpha_items));
    ImGui::End();

    /************************************************************************/
    /// and plot the frame times
    static std::vector<float> perf(100, 0);
    perf.erase(perf.begin());
    perf.push_back(1e6 / elapsed.asMicroseconds());

    ImVec2 plot_size(400, 200);
    ImGui::SetNextWindowSize(plot_size);
    ImGui::Begin("Frame Rate", nullptr, ImGuiWindowFlags_NoResize);
    int padx = ImGui::GetStyle().WindowPadding.x * 2;
    int pady = ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetFrameHeight();
    ImVec2 plot_actual_size = ImVec2(plot_size.x - padx, plot_size.y - pady);
    if (ImPlot::BeginPlot("FPS", plot_actual_size, ImPlotFlags_NoTitle | ImPlotFlags_NoLegend)) {
      ImPlot::SetupAxes("nullptr", "FPS", ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels, 0);  // ImPlotAxisFlags_NoLabel);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 2000);
      ImPlot::PlotLine("FPS", perf.data(), perf.size());
      //  ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      //  ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
      ImPlot::EndPlot();
    }
    ImGui::End();

    ////  RENDER OBJECTS  ////////////////////////////////////////////////////
    window.clear();
    for (auto& ball : balls) {
      window.draw(ball.ball);
    }
    int us = elapsed.asMicroseconds();
    int fps = 1e6 / us;
    text.setString(std::to_string(us) + "  =>  " + std::to_string(fps) + "fps");
    window.draw(text);
    /// put this last for everything to be on top of window.
    ImGui::SFML::Render(window);
    float a = 0.95;
    auto t = frame_time.getElapsedTime();
    elapsed = a * elapsed + (1 - a) * t;
    window.display();
  }

  ImGui::SFML::Shutdown();
}
