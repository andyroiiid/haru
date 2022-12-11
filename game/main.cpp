//
// Created by andyroiiid on 11/21/2022.
//

#include <haru/system/Window.h>
#include <haru/system/FileSystem.h>
#include <haru/system/JsonFile.h>

#include "Game.h"

// Use discrete GPU by default.
extern "C" {
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}

struct GameConfig {
    explicit GameConfig(const std::string &filename) {
        JsonFile gameJson(filename);
        name = gameJson.GetField<std::string>("name");
        size = {gameJson.GetField<double>("width"), gameJson.GetField<double>("height")};
        maximized = gameJson.GetField<bool>("maximized");
        startMap = gameJson.GetField<std::string>("startMap");
    }

    std::string name;
    glm::ivec2 size{};
    bool maximized;
    std::string startMap;
};

int main() {
    FileSystem fileSystem;
    fileSystem.Mount("data", "/");

    GameConfig config("game.json");

    Window window{config.name, config.size, config.maximized};

    Game game{config.startMap};

    window.MainLoop(game);
}
