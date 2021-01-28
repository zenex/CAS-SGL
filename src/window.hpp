// ╔═╗╦╔═╔═╗╦  ╔═╗╔╦╗╔═╗╔╗╔╔═╗╦
// ╚═╗╠╩╗║╣ ║  ║╣  ║ ║ ║║║║║ ╦║
// ╚═╝╩ ╩╚═╝╩═╝╚═╝ ╩ ╚═╝╝╚╝╚═╝╩═╝
// ─┐ ┬┌─┐┌┐┌┌─┐┌┐ ┬ ┬┌┬┐┌─┐ ─┐ ┬┬ ┬┌─┐
// ┌┴┬┘├┤ ││││ │├┴┐└┬┘ │ ├┤  ┌┴┬┘└┬┘┌─┘
// ┴ └─└─┘┘└┘└─┘└─┘ ┴  ┴ └─┘o┴ └─ ┴ └─┘
// Author:  SENEX @ XENOBYTE.XYZ
// License: MIT License
// Website: https://xenobyte.xyz/projects/?nav=cas-sgl

#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <iostream>
#include <array>
#include <ctime>
#include <vector>
#include <memory>
#include <stdio.h>
#include <thread>
#include <fstream>
#include <cstdint>
#include <random>
#include <stdexcept>
#include "skeletonGL/skeletonGL.hpp"

#include "cell_auto_simulation.hpp"



class Window
{
private:
    // Disable all copying and moving (singelton pattern)
    Window(const Window&) = delete;
    Window &operator = (const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator = (Window &&) = delete;

    // SkeletonGL
    SGL_Window *pWindowManager;
    // Cap delta time to a max value to stabilize physics in slower machines
    SGL_InputFrame pDeltaInput;

    //required UI sprites
    std::shared_ptr<SGL_Sprite>pixel;
    std::shared_ptr<SGL_Bitmap_Text>text;

    // FIXED TIME STEP
    float pDeltaTimeMS, pFixedTimeStepMS, pDTAccumulator;
    std::uint16_t pUpdateCycles, pMaxUpdatesPerFrame;
    std::vector<SGL_FrameData> pRenderedFrames;
    std::chrono::steady_clock::time_point pChronoStart, pChronoEnd; ///< C++11 chrono based counter
    // FPS META DATA
    float pTimeElapsed, pDeltaTimeElapsed; // Used to time when to collect fps meta data
    std::uint16_t pFrameCounter; // Counts frames, reset every second
    std::uint16_t pFPS;         // Current FPS, updated every second
    float pSimulationTime, pMsgTime;

    bool pMainLoopActive;
    float pCameraZoom;

    bool pPaused, pGenerating;
    std::shared_ptr<CellAutoSimulation>pCAS;
    std::vector<glm::mat4> pSpriteBatchData;
    std::array<std::uint16_t, SIMULATION_WIDTH*SIMULATION_HEIGHT> pSeed;
    SGL_Color pMonoColor;
    std::string pSaveFile, pDisplayMsg;
public:

    Window();
    ~Window();

    int createSGLWindow();
    void mainLoop();

    void render();
    void update();
    void input();

    bool saveSeed(const std::string &fn);
    bool loadSeed(const std::string &fn);
    void randomizeSeed(std::uint8_t freq);

    // Repopulates the simulation with the data in the pSeed array
    void reset();
};

#endif
