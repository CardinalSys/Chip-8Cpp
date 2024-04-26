#include "Renderer.cpp"
#include <fstream>
class CPU {
public:

	CPU(Renderer _renderer) {
		renderer = _renderer;
	}

	void LoadSpritesIntoMemory() {
		uint8_t sprites[80]{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

		for (int i = 0; i < sizeof(sprites); i++)
		{
			memory[i] = sprites[i];
		}
	}

	void LoadRomIntoMemory(char const* filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		if (file.is_open())
		{
			std::streampos size = file.tellg();
			char* buffer = new char[size];

			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();

			for (long i = 0; i < size; ++i){
				memory[0x200 + i] = buffer[i];
			}

			delete[] buffer;
		}

	}

	void Cycle() {
		for (int i = 0; i < speed; i++) {
			if (!isPaused){
				uint16_t opcode = (memory[programCounter] << 8 || memory[programCounter + 1]);
				ExecuteInstruction(opcode);
			}
		}
	}

	void UpdateTimers() {
		if (delayTimer > 0)
			delayTimer -= 1;

		if (soundTimer > 0)
			delayTimer -= 1;
	}

	void ExecuteInstruction(uint16_t opcode) {
		programCounter += 2;

		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;

		//Compare the opcode
	}


private:
	Renderer renderer;
	uint8_t memory[4096];
	uint8_t registers[16];
	uint16_t index = 0;
	uint8_t delayTimer = 0;
	uint8_t soundTimer = 0;
	uint16_t programCounter = 0;
	uint16_t stack[16];
	uint8_t speed = 10;
	bool isPaused = false;
};
