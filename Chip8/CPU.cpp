#include "Renderer.cpp"
#include <fstream>
#include <iostream>
#include <random>
class CPU {
public:

	CPU(Renderer& _renderer) : renderer(_renderer), programCounter(200), speed(10) {
		std::cout << "CPU initialized with PC starting at 0x200 and speed set to " << int(speed) << std::endl;
		std::cout.flush();
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
		for (unsigned int i = 0; i < 80; ++i)
		{
			memory[0x50 + i] = sprites[i];
		}
		std::cout << "Sprites loaded into memory." << std::endl;
	}

	void LoadRomIntoMemory(const char* filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (file.is_open()) {
			std::streampos size = file.tellg();
			char* buffer = new char[size];
			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();

			// Load the ROM contents into the Chip8's memory, starting at 0x200
			for (long i = 0; i < size; ++i)
			{
				memory[0x200 + i] = buffer[i];
			}

			// Free the buffer
			delete[] buffer;

			std::cout << "ROM loaded into memory. Size: " << size << " bytes." << std::endl;
		}
		else {
			std::cerr << "Failed to open ROM file: " << filename << std::endl;
		}
	}

	void Cycle() {
		std::cout << "Starting CPU cycle." << std::endl;
		for (int i = 0; i < speed; i++) {
			if (!isPaused) {
				uint16_t opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
				std::cout << "Executing opcode: " << std::hex << opcode << std::endl;
				ExecuteInstruction(opcode);
			}
		}
	}

	void UpdateTimers() {
		if (delayTimer > 0)
			delayTimer -= 1;

		if (soundTimer > 0)
			soundTimer -= 1;
	}

	void ExecuteInstruction(uint16_t opcode) {
		programCounter += 2;

		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;

		switch (opcode & 0xF000)
		{
		case 0x0000:
			switch (opcode)
			{
			case 0x00E0:
				renderer.Clear();
				break;
			case 0x00EE:
				--stackPointer;
				programCounter = stack[stackPointer];
				break;
			}
			break;
		case 0x1000:
			programCounter = (opcode & 0x0FFF);
			break;
		case 0x2000:
			stack[stackPointer] = programCounter;
			++stackPointer;
			programCounter = (opcode & 0x0FFF);
			break;
		case 0x3000:
			if (registers[x] == (opcode & 0xFF))
				programCounter += 2;
			break;
		case 0x4000:
			if (registers[x] != (opcode & 0xFF))
				programCounter += 2;
			break;
		case 0x5000:
			if (registers[x] == registers[y])
				programCounter += 2;
			break;
		case 0x6000:
			registers[x] = (opcode & 0xFF);
			break;
		case 0x7000:
			registers[x] += (opcode & 0xFF);
			break;
		case 0x8000:
			switch (opcode & 0xFF)
			{
			case 0x0:
				registers[x] = registers[y];
				break;
			case 0x1:
				registers[x] |= registers[y];
				break;
			case 0x2:
				registers[x] &= registers[y];
				break;
			case 0x3:
				registers[x] ^= registers[y];
				break;
			case 0x4: {
				uint8_t sum = registers[x] += registers[y];
				registers[0xF] = 0;
				if (sum > 0xFF) {
					registers[0xF] = 1;
				}
				registers[x] = sum;
				break;
			}

			case 0x5:
				registers[0xF] = 0;
				if (registers[x] > registers[y]) {
					registers[0xF] = 1;
				}

				registers[x] -= registers[y];
				break;
			case 0x6:
				registers[0xF] = (registers[x] * 0x1);
				registers[x] >>= 1;
				break;
			case 0x7:
				registers[0xF] = 0;
				if (registers[y] > registers[x]) {
					registers[0xF] = 1;
				}
				registers[x] = registers[y] - registers[x];
				break;
			case 0xE:
				registers[0xF] = (registers[x] & 0x80);
				registers[x] <<= 1;
				break;
			default:
				break;
			}
		case 0x9000:
			if (registers[x] != registers[y]){
				programCounter += 2;
			}
			break;
		case 0xA000:
			index = (opcode & 0xFFF);
			break;
		case 0xB000:
			programCounter = (opcode & 0xFFF) + registers[0];
			break;
		case 0xC000:
		{
			registers[x] = 255 & (opcode & 0xFF);
			break;
		}
		case 0xD000: {
			uint8_t width = 8;
			uint8_t height = opcode & 0xF;

			registers[0xF] = 0;

			for (uint8_t row = 0; row < height; row++) {
				uint8_t sprite = memory[index + row];

				for (uint8_t col = 0; col < width; col++) {
					if ((sprite & 0x80) > 0) {
						renderer.SetPixel(registers[x] + col, registers[y] + row);
						registers[0xF] = 1;

					}
					sprite <<= 1;
				}
			}
			break;
		}
		case 0xE000:
			switch (opcode & 0xFF)
			{
			case 0x9E:
				//Keyboard
				break;
			case 0xA1:
				//Keyboard
				break;
			default:
				break;
			}
		case 0xF000:
			switch (opcode & 0xFF) {
			case 0x07:
				registers[x] = delayTimer;
				break;
			case 0x0A:
				isPaused = false;
				//Bind keyboard for unpause
				break;
			case 0x15:
				delayTimer = registers[x];
				break;
			case 0x18:
				soundTimer = registers[x];
				break;
			case 0x1E:
				index += registers[x];
				break;
			case 0x29:
				index = registers[x] * 5;
				break;
			case 0x33:
				memory[index] = (int)(registers[x] / 100);
				memory[index + 1] = (int)((registers[x] % 100)/10);
				memory[index + 2] = (int)(registers[x] % 10);
				break;
			case 0x55:
				for (uint8_t registerIndex = 0; registerIndex <= x; registerIndex++) {
					memory[index + registerIndex] = registers[registerIndex];
				}
				break;
			case 0x65:
				for (uint8_t registerIndex = 0; registerIndex <= x; registerIndex++) {
					registers[registerIndex] = memory[index + registerIndex];
				}
				break;
			}
		default:
			std::cerr << "Unknown opcode 0x" << std::hex << opcode
				<< " at PC 0x" << std::hex << programCounter << std::endl;
			break;
		}
	}


private:
	Renderer& renderer;
	uint8_t memory[4096];
	uint8_t registers[16];
	uint16_t index = 0;
	uint8_t delayTimer = 0;
	uint8_t soundTimer = 0;
	uint16_t programCounter = 0;
	uint16_t stack[16];
	uint8_t speed = 10;
	uint8_t stackPointer = 0;
	bool isPaused = false;


};
