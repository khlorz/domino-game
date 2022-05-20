#include <iostream>
#include "MainWindow.h"
#include "ImGuiFunctions.h"


//void* operator new(size_t size)
//{
//	std::cout << "Allocating " << size << " bytes\n";
//	return malloc(size);
//}

int main()
{
    // Initialize GLFW
    OGLSet TheWindow;
    if (!TheWindow.InitGLFW("Test", 650, 1200)) {
        return -1;
    }
    TheWindow.SetWindowCentered();
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    ImGuide TheUI;
    TheUI.InitImGui();
    if (!TheUI.InitBackends(TheWindow.GetWindow())) {
        TheUI.Shutdown();
        TheWindow.Shutdown();
        return -1;
    }
    TheUI.EnableConfigFlags(ImGuiConfigFlags_NavEnableKeyboard);
    TheUI.SetTheme(ImGuiThemes_Dark);

    // Our state
    bool show_demo_window = true;

    MainWindow WindowRender;

    Domino2D::InitiateDominoStatics(42.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    PlayerDomino2D::InitializeDominoParameters();

    // Main loop
    while (!TheWindow.IsWindowClosed())
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        TheUI.ImGuiNewFrame();

        // Main Program Function
        //ImGui::ShowDemoWindow(&show_demo_window);

        WindowRender.RenderWindow();
        if(WindowRender.CloseWindow()){
            glfwSetWindowShouldClose(TheWindow.GetWindow(), 1);
        }

        ImGuiOGL_Render(TheWindow.GetWindow());
    }

    TheUI.Shutdown();
    TheWindow.Shutdown();

	return 0;
}