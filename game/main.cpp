//
// Created by andyroiiid on 11/21/2022.
//

#include <haru/system/Window.h>
#include <haru/system/FileSystem.h>

#include "Game.h"

// Use discrete GPU by default.
extern "C" {
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}

int main() {
    FileSystem fileSystem;
    fileSystem.Mount("data", "/");

    Window window;
    Game game;
    window.MainLoop(game);
}
