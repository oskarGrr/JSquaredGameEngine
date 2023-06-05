#include <iostream>

#include "glm.hpp"
#include "SDL.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui.h"

//generates a circle texture at startup to use later
void initCircleTexture(int radius, Uint8 RR, Uint8 GG,
    Uint8 BB, Uint8 AA, SDL_Texture** toInit, SDL_Renderer* renderer)
{
//make sure the ordering of the RGBA bytes will be the same
//regarless of endianess of the target platform
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    uint32_t rMask = 0xFF000000;
    uint32_t gMask = 0x00FF0000;
    uint32_t bMask = 0x0000FF00;
    uint32_t aMask = 0x000000FF;
    uint32_t circleColor = (RR << 24) + (GG << 16) + (BB << 8) + AA;
#else
    uint32_t rMask = 0x000000FF;
    uint32_t gMask = 0x0000FF00;
    uint32_t bMask = 0x00FF0000;
    uint32_t aMask = 0xFF000000;
    uint32_t circleColor = (AA << 24) + (BB << 16) + (GG << 8) + RR;
#endif
    int const diameter = radius * 2;
    SDL_Rect const boundingBox{-radius, -radius, diameter, diameter};
    uint32_t const pixelCount = diameter * diameter;
    auto pixels = std::make_unique<uint32_t[]>(pixelCount);

    int radiusSquared = radius * radius;
    int xOffset = -radius, yOffset = -radius;
    for(int x = 0; x < diameter; ++x)
    {
        for(int y = 0; y < diameter; ++y)
        {
            if((x-radius)*(x-radius) + (y-radius)*(y-radius) <= radiusSquared)
            {
                pixels[x + y * diameter] = circleColor;
            }
        }
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom
    (
         pixels.get(), 
         diameter, 
         diameter,
         32, 
         diameter * sizeof(uint32_t), 
         rMask, gMask, bMask, aMask
    );
   
    if(!surface) throw std::exception(SDL_GetError());

    *toInit = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

static void setImGuiSettings()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                  = {1.00f, 1.00f, 1.00f, 1.00f};
    colors[ImGuiCol_TextDisabled]          = {0.50f, 0.50f, 0.50f, 1.00f};
    colors[ImGuiCol_WindowBg]              = {0.06f, 0.06f, 0.06f, 0.94f};
    colors[ImGuiCol_ChildBg]               = {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_PopupBg]               = {0.08f, 0.08f, 0.08f, 0.94f};
    colors[ImGuiCol_Border]                = {0.43f, 0.43f, 0.50f, 0.50f};
    colors[ImGuiCol_BorderShadow]          = {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_FrameBg]               = {0.44f, 0.44f, 0.44f, 0.60f};
    colors[ImGuiCol_FrameBgHovered]        = {0.57f, 0.57f, 0.57f, 0.70f};
    colors[ImGuiCol_FrameBgActive]         = {0.76f, 0.76f, 0.76f, 0.80f};
    colors[ImGuiCol_TitleBg]               = {0.04f, 0.04f, 0.04f, 1.00f};
    colors[ImGuiCol_TitleBgActive]         = {0.16f, 0.16f, 0.16f, 1.00f};
    colors[ImGuiCol_TitleBgCollapsed]      = {0.00f, 0.00f, 0.00f, 0.60f};
    colors[ImGuiCol_MenuBarBg]             = {0.14f, 0.14f, 0.14f, 1.00f};
    colors[ImGuiCol_ScrollbarBg]           = {0.02f, 0.02f, 0.02f, 0.53f};
    colors[ImGuiCol_ScrollbarGrab]         = {0.31f, 0.31f, 0.31f, 1.00f};
    colors[ImGuiCol_ScrollbarGrabHovered]  = {0.41f, 0.41f, 0.41f, 1.00f};
    colors[ImGuiCol_ScrollbarGrabActive]   = {0.51f, 0.51f, 0.51f, 1.00f};
    colors[ImGuiCol_CheckMark]             = {0.13f, 0.75f, 0.55f, 0.80f};
    colors[ImGuiCol_SliderGrab]            = {0.13f, 0.75f, 0.75f, 0.80f};
    colors[ImGuiCol_SliderGrabActive]      = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_Button]                = {0.13f, 0.75f, 0.55f, 0.40f};
    colors[ImGuiCol_ButtonHovered]         = {0.13f, 0.75f, 0.75f, 0.60f};
    colors[ImGuiCol_ButtonActive]          = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_Header]                = {0.13f, 0.75f, 0.55f, 0.40f};
    colors[ImGuiCol_HeaderHovered]         = {0.13f, 0.75f, 0.75f, 0.60f};
    colors[ImGuiCol_HeaderActive]          = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_Separator]             = {0.13f, 0.75f, 0.55f, 0.40f};
    colors[ImGuiCol_SeparatorHovered]      = {0.13f, 0.75f, 0.75f, 0.60f};
    colors[ImGuiCol_SeparatorActive]       = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_ResizeGrip]            = {0.13f, 0.75f, 0.55f, 0.40f};
    colors[ImGuiCol_ResizeGripHovered]     = {0.13f, 0.75f, 0.75f, 0.60f};
    colors[ImGuiCol_ResizeGripActive]      = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_Tab]                   = {0.13f, 0.75f, 0.55f, 0.80f};
    colors[ImGuiCol_TabHovered]            = {0.13f, 0.75f, 0.75f, 0.80f};
    colors[ImGuiCol_TabActive]             = {0.13f, 0.75f, 1.00f, 0.80f};
    colors[ImGuiCol_TabUnfocused]          = {0.18f, 0.18f, 0.18f, 1.00f};
    colors[ImGuiCol_TabUnfocusedActive]    = {0.36f, 0.36f, 0.36f, 0.54f};
    colors[ImGuiCol_DockingPreview]        = {0.13f, 0.75f, 0.55f, 0.80f};
    colors[ImGuiCol_DockingEmptyBg]        = {0.13f, 0.13f, 0.13f, 0.80f};
    colors[ImGuiCol_PlotLines]             = {0.61f, 0.61f, 0.61f, 1.00f};
    colors[ImGuiCol_PlotLinesHovered]      = {1.00f, 0.43f, 0.35f, 1.00f};
    colors[ImGuiCol_PlotHistogram]         = {0.90f, 0.70f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogramHovered]  = {1.00f, 0.60f, 0.00f, 1.00f};
    colors[ImGuiCol_TableHeaderBg]         = {0.19f, 0.19f, 0.20f, 1.00f};
    colors[ImGuiCol_TableBorderStrong]     = {0.31f, 0.31f, 0.35f, 1.00f};
    colors[ImGuiCol_TableBorderLight]      = {0.23f, 0.23f, 0.25f, 1.00f};
    colors[ImGuiCol_TableRowBg]            = {0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_TableRowBgAlt]         = {1.00f, 1.00f, 1.00f, 0.07f};
    colors[ImGuiCol_TextSelectedBg]        = {0.26f, 0.59f, 0.98f, 0.35f};
    colors[ImGuiCol_DragDropTarget]        = {1.00f, 1.00f, 0.00f, 0.90f};
    colors[ImGuiCol_NavHighlight]          = {0.26f, 0.59f, 0.98f, 1.00f};
    colors[ImGuiCol_NavWindowingHighlight] = {1.00f, 1.00f, 1.00f, 0.70f};
    colors[ImGuiCol_NavWindowingDimBg]     = {0.80f, 0.80f, 0.80f, 0.20f};
    colors[ImGuiCol_ModalWindowDimBg]      = {0.80f, 0.80f, 0.80f, 0.35f};

    //add the font from the fonts folder
    auto& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("assets/fonts/ebrima.ttf", 16.0);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

int main(int, char**)
{
    SDL_Window* wnd{nullptr};
    SDL_Renderer* renderer{nullptr};

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(900, 900, 0, &wnd, &renderer);
    SDL_SetWindowTitle(wnd, "j^2");

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(wnd, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    setImGuiSettings();

    SDL_Rect box{450, 450, 40, 40};
    SDL_Texture* circleTexture{nullptr};
    initCircleTexture(100, 0x34, 0xd5, 0xeb, 0xff, &circleTexture, renderer);
    SDL_Rect circleBounds{0,0,0,0};
    SDL_QueryTexture(circleTexture, nullptr, nullptr, &circleBounds.w, &circleBounds.h);
    
    bool isWindowOpen{true};
    while(isWindowOpen)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        SDL_Event evnt;     
        while(SDL_PollEvent(&evnt))
        {
            ImGui_ImplSDL2_ProcessEvent(&evnt);
            switch(evnt.type)
            {
            case SDL_QUIT:
            {
                isWindowOpen = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
                    break;

                if(evnt.button.button == SDL_BUTTON_LEFT)
                {
                    box.x = evnt.button.x - box.w/2;
                    box.y = evnt.button.y - box.h/2;
                }
                else if(evnt.button.button == SDL_BUTTON_RIGHT)
                {
                    circleBounds.x = evnt.button.x - circleBounds.w/2;
                    circleBounds.y = evnt.button.y - circleBounds.h/2;
                }
                break;
            }
            }
        }

        SDL_RenderClear(renderer);

        ImGui::ShowDemoWindow();

        SDL_RenderCopy(renderer, circleTexture, nullptr, &circleBounds);
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderDrawRect(renderer, &box);
        SDL_SetRenderDrawColor(renderer, 0,0,0, 255);

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
        ImGui::EndFrame();
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(circleTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(wnd);
    SDL_Quit();

    return EXIT_SUCCESS;
}