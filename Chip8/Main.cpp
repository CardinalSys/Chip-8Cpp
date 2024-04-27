#include <iostream>
#include "CPU.cpp"
#include <chrono>
#include <thread>



const unsigned int fps = 60;
const std::chrono::milliseconds fpsInterval(1000 / fps);

int main(int argc, char* argv[]) {

    Renderer renderer;
    CPU cpu(renderer);

    renderer.SetPixel(1, 4);
    auto startTime = std::chrono::steady_clock::now();
    std::cout << "Test" << std::endl;
    cpu.LoadRomIntoMemory("D:\\projects\\javascript\\chip8\\roms\\tetris");

    while (true) {
        renderer.RenderCycle();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        
        if (elapsed > fpsInterval) {
            startTime = now;
           cpu.Cycle();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}



