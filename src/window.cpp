// ╔═╗╦╔═╔═╗╦  ╔═╗╔╦╗╔═╗╔╗╔╔═╗╦
// ╚═╗╠╩╗║╣ ║  ║╣  ║ ║ ║║║║║ ╦║
// ╚═╝╩ ╩╚═╝╩═╝╚═╝ ╩ ╚═╝╝╚╝╚═╝╩═╝
// ─┐ ┬┌─┐┌┐┌┌─┐┌┐ ┬ ┬┌┬┐┌─┐ ─┐ ┬┬ ┬┌─┐
// ┌┴┬┘├┤ ││││ │├┴┐└┬┘ │ ├┤  ┌┴┬┘└┬┘┌─┘
// ┴ └─└─┘┘└┘└─┘└─┘ ┴  ┴ └─┘o┴ └─ ┴ └─┘
// Author:  SENEX @ XENOBYTE.XYZ
// License: MIT License
// Website: https://xenobyte.xyz/projects/?nav=cas-sgl

#include "window.hpp"

Window::Window()
{
    this->pWindowManager = new SGL_Window();
    //seed RNG
    srand(time(NULL));
}

Window::~Window()
{
    delete pWindowManager;
}

int Window::createSGLWindow()
{
    SGL_Log("Initializing...");
    SGL_Log("Creating window...");
    try
    {
        //this->pWindowManager->initializeWindow(0, 0, 1024, 576, 480, 270, std::string("With_his_jimmy "), false, std::string("skeletongl.ini"));
        this->pWindowManager->initializeWindow(0, 0, 640, 360, 640/4, 360/3, std::string("RISK VECTOR"), false, std::string("skeletongl.ini"));
    }
    catch (SGL_Exception &e)
    {
        SGL_Log("Aborting window creating procedure.");
        SGL_Log(e.what());
        assert(false);
        return -1;
    }
    SGL_Log("Window ready.");
    return 0;
}


void Window::mainLoop()
{
    pWindowManager->setLogLevel(LOG_LEVEL::SGL_DEBUG);
    SGL_Log("Loading the game...", LOG_LEVEL::DEBUG);
    pWindowManager->checkForErrors();

    // FIXED TIME STEP
    pFixedTimeStepMS = 1.0f / 100.0f; // Refresh the simulation at 100hz
    pMaxUpdatesPerFrame = 5;
    pUpdateCycles = 0;
    pChronoStart = std::chrono::steady_clock::now();
    pChronoEnd = pChronoStart;
    pSimulationTime = 0.0;
    pMsgTime = 2.0;
    pDisplayMsg = "Initialized";

    pWindowManager->assetManager->loadTexture("assets/textures/avatar.png", true, "debug1");

    pPaused = false;
    pSeed = {0};
    pMonoColor = SGL_Color(0.30f, 1.0f, 0.10f, 1.0f);
    pMonoColor = SGL_Color(0.99f, 0.99f, 0.99f, 1.0f);
    pSaveFile = "seed.txt";
    pCameraZoom = 1.0f;
    pMainLoopActive = true;

    // Render related

    // Cell pixel
    pixel = std::make_shared<SGL_Sprite>();
    pixel->texture = pWindowManager->assetManager->getTexture(SGL::SQUARE_TEXTURE);
    //pixel->shader = pWindowManager->assetManager->getShader("customSprite");
    pixel->resetUVCoords();
    pixel->color = pMonoColor;
    pixel->size.x = 1;
    pixel->size.y = 1;

    // BMP font
    text = std::make_shared<SGL_Bitmap_Text>();
    text->texture = pWindowManager->assetManager->getTexture(SGL::DEFAULT_BMP_FONT_TEXTURE);
    text->color = {1.0, 1.0, 1.0, 1.0};
    text->position.x = 10;
    text->position.y = 0;
    text->scale = 8;
    text->text = "cell-auto";

    // Instanciate the sim nerd pointer
    pCAS = std::make_shared<CellAutoSimulation>();


    // Set the initial seed
    // Bill Gosper's glider gun
    pCAS->setCell(25, 45, "........................#............");
    pCAS->setCell(25, 46, "......................#.#............");
    pCAS->setCell(25, 47, "............##......##............##.");
    pCAS->setCell(25, 48, "...........#...#....##............##.");
    pCAS->setCell(25, 49, "##........#.....#...##...............");
    pCAS->setCell(25, 50, "##........#...#.##....#.#............");
    pCAS->setCell(25, 51, "..........#.....#.......#............");
    pCAS->setCell(25, 52, "...........#...#.....................");
    pCAS->setCell(25, 53, "............##.......................");

    // Must be called before entering the game loop to properly initialize
    // the post processor's FBO
    this->pWindowManager->setClearColor(SGL_Color(0.10f, 0.10f, 0.10f, 1.0f));
    while (pMainLoopActive)
    {
        // Time since epoch
        pChronoStart = std::chrono::steady_clock::now();
        auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(pChronoStart - pChronoEnd);
        pTimeElapsed += timeElapsed.count();
        if (!pPaused)
            pSimulationTime += std::chrono::duration_cast<std::chrono::milliseconds>(pChronoStart - pChronoEnd).count();

        // Delta time
        auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(pChronoStart - pChronoEnd);
        pDeltaTimeMS = timeDiff.count();
        pDeltaTimeMS /= 1000000000;

        // Collect frame meta data?
        pFrameCounter++;
        if ( ( (pTimeElapsed / 1000) - pDeltaTimeElapsed) >= 1.0) // A second has passed, collect meta data
        {
            pFPS = pFrameCounter;
            pFrameCounter = 0;
            pDeltaTimeElapsed = pTimeElapsed / 1000;
        }

        // Update the FPS string in the window title
        std::string str = "CELLULAR AUTOMATA - SGL -FPS: " + std::to_string(pFPS);
        pWindowManager->setWindowTitle(str);
        // START COUNTING THE FRAME TIME
        pChronoEnd = pChronoStart;

        // FIXED TIME STEP
        pDTAccumulator += pDeltaTimeMS;
        // Total number of updates to perform this frame
        pUpdateCycles = static_cast<int>(std::floor(pDTAccumulator / pFixedTimeStepMS));

        // Consume the cycles from the time accumulator
        if (pUpdateCycles > 0)
            pDTAccumulator -= pUpdateCycles * pFixedTimeStepMS;

        // Just crash it at this point
        assert("Accumulator must have a value lower than the fixes time step" && pDTAccumulator < pFixedTimeStepMS + FLT_EPSILON);

        // Should the program lag behind its predefined time step, force it to recover a max amount
        // of times each frame to prevent a sprial-of-death scenario
        pUpdateCycles = std::min(pUpdateCycles, pMaxUpdatesPerFrame);
        this->input();
        while ( pUpdateCycles > 0)
        {
            this->update();
            pUpdateCycles--;
        }
        // Must be called as soon as the render frame starts to properly calculate delta time values and initiate the postprocessor FBO
        //this->update();
        this->pWindowManager->startFrame();
        this->render();
        this->pWindowManager->endFrame();

        this->pWindowManager->checkForErrors();
    }
    SGL_Log("Exiting main loop");
}

void Window::update()
{
    if (!pPaused)
        pCAS->update();

    if (pMsgTime > 0.0)
        pMsgTime -= pDeltaTimeMS;
    // ------------- //
    // UPDATE CAMERA //
    // ------------- //
    //position the center of the camera on the center of the FBO texture (which has a size of internalW x interalH)
    this->pWindowManager->setCameraPosition(glm::vec2(pWindowManager->getWindowCreationSpecs().internalW / 2, pWindowManager->getWindowCreationSpecs().internalH / 2));
    // this->pWindowManager->cameraPosition(pPlayerOne->center());
    //zoom
    this->pWindowManager->setCameraScale(pCameraZoom);
    //update the internal offset matrix for rendering
    this->pWindowManager->updateCamera();
}

void Window::input()
{
    pGenerating = false;
    SGL_InputFrame desiredKeys = this->pWindowManager->getFrameInput();

    if (desiredKeys.num1.pressed && !pDeltaInput.num1.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Bill Gosper glider gun";
        pCAS->resetSimulation();

        // Bill Gosper's glider gun
        pCAS->setCell(25, 45, "........................#............");
        pCAS->setCell(25, 46, "......................#.#............");
        pCAS->setCell(25, 47, "............##......##............##.");
        pCAS->setCell(25, 48, "...........#...#....##............##.");
        pCAS->setCell(25, 49, "##........#.....#...##...............");
        pCAS->setCell(25, 50, "##........#...#.##....#.#............");
        pCAS->setCell(25, 51, "..........#.....#.......#............");
        pCAS->setCell(25, 52, "...........#...#.....................");
        pCAS->setCell(25, 53, "............##.......................");

        pSeed = {0};
        pSimulationTime = 0.0;
        pGenerating = true;

        // randomizeSeed(1);
    }
    if (desiredKeys.num2.pressed && !pDeltaInput.num2.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 2";
        randomizeSeed(2);
    }
    if (desiredKeys.num3.pressed && !pDeltaInput.num3.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 3";

        randomizeSeed(3);
    }
    if (desiredKeys.num4.pressed && !pDeltaInput.num4.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 4";
        randomizeSeed(4);
    }
    if (desiredKeys.num5.pressed && !pDeltaInput.num5.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 5";
        randomizeSeed(5);
    }
    if (desiredKeys.num6.pressed && !pDeltaInput.num6.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 6";
        randomizeSeed(6);
    }
    if (desiredKeys.num7.pressed && !pDeltaInput.num7.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 7";
        randomizeSeed(7);
    }
    if (desiredKeys.num8.pressed && !pDeltaInput.num8.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 8";
        randomizeSeed(8);
    }
    if (desiredKeys.num9.pressed && !pDeltaInput.num9.pressed)
    {
        pMsgTime = 4.0;
        pDisplayMsg = "Randomizing seed with 1 in 9";
        randomizeSeed(9);
    }

    if (desiredKeys.esc.pressed && !pDeltaInput.esc.pressed)
        pMainLoopActive = false;

    if (desiredKeys.z.pressed && !pDeltaInput.z.pressed)
        pCameraZoom -= 0.02;

    if (desiredKeys.x.pressed && !pDeltaInput.x.pressed)
        pCameraZoom += 0.02;

    if (desiredKeys.c.pressed && !pDeltaInput.c.pressed)
        pCameraZoom = 1.0;

    if (desiredKeys.space.pressed && !pDeltaInput.space.pressed)
        pPaused = !pPaused;

    // Reset the simulation with randomized values
    if (desiredKeys.r.pressed && !pDeltaInput.r.pressed)
        randomizeSeed(9);
    // Save the generated seed to a file & make short pause
    if (desiredKeys.s.pressed && !pDeltaInput.s.pressed)
    {
        if (!pSeed.empty())
        {
            if (saveSeed(pSaveFile))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                pMsgTime = 4.0;
                pDisplayMsg = "Seed saved to ./seed.txt";
                SGL_Log("Seed saved to ./seed.txt");
            }
            else
            {
                throw std::runtime_error("Error saving to ./seed.txt");
                SGL_Log("Error saving to ./seed.txt");
            }
        }
    }

    // Load the contents of the save file
    if (desiredKeys.l.pressed && !pDeltaInput.l.pressed)
    {
        if (loadSeed(pSaveFile))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                pMsgTime = 4.0;
                pDisplayMsg = "Seed loaded from ./seed.txt";
                SGL_Log("Seed loaded from ./seed.txt");
            }
        else
        {
                SGL_Log("Error loading ./seed.txt");
                throw std::runtime_error("Error loading ./seed.txt");
        }
    }
}


void Window::randomizeSeed(std::uint8_t freq)
{
    if (freq > 9)
        freq = 9;
    if (freq <= 0)
        freq = 2;

    pSeed = {0};

    for (std::uint16_t i = 0; i < (SIMULATION_WIDTH * SIMULATION_HEIGHT); ++i)
    {
        if ((rand() % freq) == 1)
            pSeed[i] = 1;
        else
            pSeed[i] = 0;
    }
    reset();
}

void Window::reset()
{
    pCAS->resetSimulation();
    for (std::uint16_t i = 0; i < (SIMULATION_WIDTH * SIMULATION_HEIGHT); ++i)
    {
        if (pSeed[i] == 1)
            pCAS->setCell(i,0," # ");
    }
    pSimulationTime = 0.0;
    pGenerating = true;
}


bool Window::loadSeed(const std::string &fn)
{
    std::ifstream file(fn);
    if (file.is_open())
    {
        SGL_Log("Loading save file");
        // Load file header (first 17 bytes)
        // std::string str;
        // for(std::uint16_t i = 0; i < 17; ++i)
        //     str += file.get();
        // SGL_Log(fn + " contents: ");
        // SGL_Log(str);

        // Read the relevant data into a char buffer
        char *buffer = new char[SIMULATION_WIDTH*SIMULATION_HEIGHT];
        memset(buffer, 0, SIMULATION_WIDTH*SIMULATION_HEIGHT);
        file.seekg(17);
        file.read(buffer, SIMULATION_WIDTH*SIMULATION_HEIGHT);

        // Populate the seed array
        for (std::uint16_t i = 0; i < SIMULATION_WIDTH*SIMULATION_HEIGHT; ++i)
        {
            if (buffer[i] == '1')
                pSeed[i] = 1;
            else
                pSeed[i] = 0;
        }

        delete [] buffer;
        file.close();
        reset();

        return true;
    }
    return false;
}


bool Window::saveSeed(const std::string &fn)
{
    std::ofstream file(fn);
    if (file.is_open() && !pSeed.empty())
    {
        SGL_Log("saving file");
        //printSeed();
        file << "-- SEED START --\n";
        //file.write((char*)&pSeed, SIMULATION_WIDTH*SIMULATION_HEIGHT);

        // std::string test = "";
        // for (std::uint16_t i = 0; i < SIMULATION_WIDTH*SIMULATION_HEIGHT; ++i)
        // {
        //     file << static_cast<int>(pSeed[i]);
        // }

        for (const std::uint8_t &i : pSeed)
        {
            //test += i;
            // file << static_cast<char>(i);
            if (i == 1)
                file << 1;
            else
                file << 0;
        }
        //file.write(test.c_str());
        //file << test.c_str();
        file << "\n-- SEED END --";
        file << "\n";
        SGL_Log("done, string saved: ");
        //SGL_Log(test);
        file.close();

        return true;
    }
    return false;
}

void Window::render()
{
    this->pWindowManager->setCameraMode(CAMERA_MODE::DEFAULT);

    // Render simulation bounds
    std::uint16_t x, y, w, h;
    // x = pWindowManager->getWindowCreationSpecs().internalW / 2;
    // y = pWindowManager->getWindowCreationSpecs().internalH / 2;
    x = 10;
    y = 10;
    w = SIMULATION_WIDTH * pixel->size.x;
    h = SIMULATION_HEIGHT * pixel->size.y;;

    // Top boundary
    pWindowManager->renderer->renderLine(x, y, x+w, y, 1.0, SGL_Color(1.0f, 1.0f, 0.0f, 1.0f));
    // Bottom boundary
    pWindowManager->renderer->renderLine(x, y+h, x+w, y+h, 1.0, SGL_Color(1.0f, 1.0f, 0.0f, 1.0f));
    // Left boundary
    pWindowManager->renderer->renderLine(x, y, x, y+h, 1.0, SGL_Color(1.0f, 1.0f, 0.0f, 1.0f));
    // Right boundary
    pWindowManager->renderer->renderLine(x+w, y, x+w, y+h, 1.0, SGL_Color(1.0f, 1.0f, 0.0f, 1.0f));

    // Only clear the vector to render if the simulation is NOT paused (to render the last state of the simulation when paused)
    if (!pPaused)
        pSpriteBatchData.clear();

    for (std::uint16_t i = 0; i < pCAS->getTotalActiveCells(); ++i)
    {
        std::pair<std::uint16_t, std::uint16_t> activeCell = pCAS->getActiveCell(i);

        glm::mat4 model{1.0f};
        model = glm::translate(model, glm::vec3(activeCell.first + x, activeCell.second + y, 0.0f)); //move

        pSpriteBatchData.emplace_back(model);
    }
    pWindowManager->renderer->renderSpriteBatch(*pixel, &pSpriteBatchData);
    pCAS->clearActiveCells();

    // Render text
    text->text = "CAS-SGL: Game of Life Simulator";
    text->position.x = x;
    text->position.y = y - 8;
    text->scale = 4;
    text->color = SGL_Color(0.80f, 0.20f, 0.20f, 1.0f);
    pWindowManager->renderer->renderBitmapText(*text);

    if (pGenerating)
        text->text = "-- generating -- ";
    else
        text->text = "-- running -- ";

    if (pPaused)
        text->text = "-- paused -- ";

    text->position.x = x;
    text->position.y = y - 4;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);

    if (pGenerating)
    {
        text->text = "SEEDING...";

        text->position.x = 10 + 20;
        text->position.y = 10 + 38;
        text->scale = 6;
        text->color = SGL_Color(0.9, 0.2, 0.2, 1.0f);
        pWindowManager->renderer->renderBitmapText(*text);

    }


    // Sim data
    text->text = "- Sim data - " + std::to_string(pSpriteBatchData.size());
    text->position.x = x+w;
    text->position.y = y;
    text->scale = 4;
    text->color = SGL_Color(0.10, 0.8, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = "CELLS: " + std::to_string(pSpriteBatchData.size());
    text->position.x = x+w;
    text->position.y = y+4;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = "FPS: " + std::to_string(pFPS);
    text->position.x = x+w;
    text->position.y = y+8;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);

    // Trim all but the first decimal
    std::string timeElapsedStr = std::to_string(pSimulationTime / 1000);
    timeElapsedStr.erase(timeElapsedStr.find('.') + 2, timeElapsedStr.back());
    text->text = "Time: " + timeElapsedStr;
    text->position.x = x+w;
    text->position.y = y+12;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = "HZ: " + std::to_string(pCAS->getIterations());
    text->position.x = x+w;
    text->position.y = y+16;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);

    std::string seedStr = std::to_string(pSeed.size());
    text->text = seedStr;
    text->position.x = x + w;
    text->position.y = y-8;
    text->scale = 4;
    text->color = pMonoColor;
    //pWindowManager->renderer->renderBitmapText(*text);


    // Instructions

    y += 30;
    text->text = "- CONTROLS -";
    text->position.x = x + w;
    text->position.y = y -4;
    text->scale = 4;
    text->color = SGL_Color(0.10, 0.8, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = "spc - pause";
    text->position.x = x + w;
    text->position.y = y;
    text->scale = 4;
    text->color = SGL_Color(0.40, 0.4, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = " s  - save";
    text->position.x = x + w;
    text->position.y = y + 4;
    text->scale = 4;
    text->color = SGL_Color(0.40, 0.4, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = " l  - load";
    text->position.x = x + w;
    text->position.y = y + 8;
    text->scale = 4;
    text->color = SGL_Color(0.40, 0.4, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    text->text = "1:9 - reset";
    text->position.x = x + w;
    text->position.y = y + 12;
    text->scale = 4;
    text->color = SGL_Color(0.40, 0.4, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);

    // NEOHEX logo
    y = SIMULATION_HEIGHT - 20;
    text->text = "VISIT";
    text->position.x = x + w + 4;
    text->position.y = y;
    text->scale = 4;
    text->color = SGL_Color(0.90, 0.2, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);
    text->text = "XENOBYTE.XYZ";
    text->position.x = x + w;
    text->position.y = y + 4;
    text->scale = 4;
    text->color = pMonoColor;
    pWindowManager->renderer->renderBitmapText(*text);
    text->text = "4 MORE";
    text->position.x = x + w + 16;
    text->position.y = y + 8;
    text->scale = 4;
    text->color = SGL_Color(0.90, 0.2, 0.90, 1.0);
    pWindowManager->renderer->renderBitmapText(*text);



    // Render system messages

    if ( pMsgTime > 0.0)
    {
        text->text = pDisplayMsg;
        text->position.x = x;
        text->position.y = SIMULATION_HEIGHT + 11;
        text->scale = 4;
        text->color = SGL_Color(0.90, 0.9, 0.90, pMsgTime);
        pWindowManager->renderer->renderBitmapText(*text);

    }
}
