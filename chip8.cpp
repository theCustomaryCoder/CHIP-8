#ifndef CHIP8_CPP
#define CHIP8_CPP

#include <stdint.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <fstream>
#include <sys/stat.h>

#include "inputs.cpp"

#define pixelSize 10

using namespace std;

long getFileSize(string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void drawPixel(int x, int y, int isOn){
    if(isOn > 0){
        glColor3f(1, 1, 1);
    }
    else {
        glColor3f(0, 0, 0);
    }
    glBegin(GL_QUADS);
    glVertex2i(pixelSize * x, pixelSize * y);
    glVertex2i(pixelSize * x, pixelSize * (y + 1));
    glVertex2i(pixelSize * (x + 1), pixelSize * (y + 1));
    glVertex2i(pixelSize * (x + 1), pixelSize * y);
    glEnd();
}

class Chip8 {
    public:

    ButtonKeys* inputKeys;
    uint8_t ram[40696]; // 0x000 to 0x1FF: Default interpreter space (not usable) -> Start Programs at 0x200 (512 Bytes)
    uint8_t v[16]; // V0 to VF are 8-bit general purpose registers. !!! VF must not be used by programs, because it is used for flags !!!
    uint16_t i; // I is a 16-bit register used for memory addresses. Most often just the twelve lowest bits are used.
    uint8_t st; // ST is a sound register -> A sound is played, when the register is not zero. In this case it also continiously decremented at a frequency of 60Hz
    uint8_t dt; // DT is a delay register -> MORE INFORMATION ON DA WAY
    uint16_t pc; // PC is the 16-bit program counter. it stores the currently executing address.
    uint8_t sp; // SP is the 8-bit stack pointer and points to the topmost level of the stack.
    uint16_t stack[16]; // The stack stores the 16-bit addresses the interpreter should return to when finishing subroutines. CHIP-8 allows for 16 levels of nested subroutines.
    uint16_t inputMatrix; // this 16-bit Value shows which keys are active and which not.
    bool screen[64 * 32]; // This represents a 64x32 monochrome screen
    int decrementer;

    Chip8(ButtonKeys* keys){
        
        inputKeys = keys;
        decrementer = 16;
        // Initialize registers
        i = 0x0000;
        st = 0x00;
        dt = 0x00;
        pc = 0x000;
        sp = 0x00;
        inputMatrix = 0x0000;

        ram[0x000] = 0x12; // Skip save memory space
        ram[0x001] = 0x00;
        // Load hex-sprites into interperter-space of RAM
        // 0: 0x000 to 0x004
        ram[0x002] = 0xF0;
        ram[0x003] = 0x90;
        ram[0x004] = 0x90;
        ram[0x005] = 0x90;
        ram[0x006] = 0xF0;

        // 1: 0x005 to 0x009
        ram[0x007] = 0x20;
        ram[0x008] = 0x60;
        ram[0x009] = 0x20;
        ram[0x00A] = 0x20;
        ram[0x00B] = 0x70;

        // 2: 0x00A to 0x00E
        ram[0x00C] = 0xF0;
        ram[0x00D] = 0x10;
        ram[0x00E] = 0xF0;
        ram[0x00F] = 0x80;
        ram[0x010] = 0xF0;

        // 3: 0x00F to 0x013
        ram[0x011] = 0xF0;
        ram[0x012] = 0x10;
        ram[0x013] = 0xF0;
        ram[0x014] = 0x10;
        ram[0x015] = 0xF0;

        // 4: 0x014 to 0x018
        ram[0x016] = 0x90;
        ram[0x017] = 0x90;
        ram[0x018] = 0xF0;
        ram[0x019] = 0x10;
        ram[0x01A] = 0x10;

        // 5 0x019 to 0x01D
        ram[0x01B] = 0xF0;
        ram[0x01C] = 0x80;
        ram[0x01D] = 0xF0;
        ram[0x01E] = 0x10;
        ram[0x01F] = 0xF0;

        // 6: 0x01E to 0x022
        ram[0x020] = 0xF0;
        ram[0x021] = 0x80;
        ram[0x022] = 0xF0;
        ram[0x023] = 0x90;
        ram[0x024] = 0xF0;

        // 7: 0x023 to 0x027
        ram[0x025] = 0xF0;
        ram[0x026] = 0x10;
        ram[0x027] = 0x20;
        ram[0x028] = 0x40;
        ram[0x029] = 0x40;

        // 8: 0x028 to 0x02C
        ram[0x02A] = 0xF0;
        ram[0x02B] = 0x90;
        ram[0x02C] = 0xF0;
        ram[0x02D] = 0x90;
        ram[0x02E] = 0xF0;

        // 9: 0x02D to 0x031
        ram[0x02F] = 0xF0;
        ram[0x030] = 0x90;
        ram[0x031] = 0xF0;
        ram[0x032] = 0x10;
        ram[0x033] = 0xF0;

        // A: 0x032 to 0x036
        ram[0x034] = 0xF0;
        ram[0x035] = 0x90;
        ram[0x036] = 0xF0;
        ram[0x037] = 0x90;
        ram[0x038] = 0x90;

        // B: 0x037 to 0x03B
        ram[0x039] = 0xE0;
        ram[0x03A] = 0x90;
        ram[0x03B] = 0xE0;
        ram[0x03C] = 0x90;
        ram[0x03D] = 0xE0;

        // C: 0x03C to 0x040
        ram[0x03E] = 0xF0;
        ram[0x03F] = 0x80;
        ram[0x040] = 0x80;
        ram[0x041] = 0x80;
        ram[0x042] = 0xF0;

        // D: 0x041 to 0x045
        ram[0x043] = 0xE0;
        ram[0x044] = 0x90;
        ram[0x045] = 0x90;
        ram[0x046] = 0x90;
        ram[0x047] = 0xE0;

        // E 0x046 to 0x04A
        ram[0x048] = 0xF0;
        ram[0x049] = 0x80;
        ram[0x04A] = 0xF0;
        ram[0x04B] = 0x80;
        ram[0x04C] = 0xF0;

        // F 0x04B to 0x04F
        ram[0x04D] = 0xF0;
        ram[0x04E] = 0x80;
        ram[0x04F] = 0xF0;
        ram[0x050] = 0x80;
        ram[0x051] = 0x80;

        // H 0x052 to 0x056
        ram[0x052] = 0x90;
        ram[0x053] = 0x90;
        ram[0x054] = 0xF0;
        ram[0x055] = 0x90;
        ram[0x056] = 0x90;

        // L 0x057 to 0x05B
        ram[0x057] = 0x80;
        ram[0x058] = 0x80;
        ram[0x059] = 0x80;
        ram[0x05A] = 0x80;
        ram[0x05B] = 0xF0;

        // Load in program for test

        ram[0x200] = 0x00;
        ram[0x201] = 0xE0;

        ram[0x202] = 0x61;
        ram[0x203] = 0x00;

        ram[0x204] = 0x62;
        ram[0x205] = 0x01;

        ram[0x206] = 0x63;
        ram[0x207] = 0x00;

        ram[0x208] = 0x64;
        ram[0x209] = 0x3C;

        ram[0x20A] = 0x43;
        ram[0x20B] = 0x0A;

        ram[0x20C] = 0x12;  //TBD
        ram[0x20D] = 0xFF;  //TBD

        ram[0x20E] = 0xF3;
        ram[0x20F] = 0x29;

        ram[0x210] = 0xD1;
        ram[0x211] = 0x24;

        ram[0x212] = 0x71;
        ram[0x213] = 0x00;

        ram[0x214] = 0x73;
        ram[0x215] = 0x01;

        ram[0x216] = 0xF4;
        ram[0x217] = 0x15;

        ram[0x218] = 0xF5; //
        ram[0x219] = 0x07;

        ram[0x21A] = 0x35;
        ram[0x21B] = 0x00;

        ram[0x21C] = 0x12;
        ram[0x21D] = 0x18;

        ram[0x21E] = 0x12;
        ram[0x21F] = 0x0A;
    }

    void loadProgram(uint8_t code[]){
        uint8_t saveSpace[0x200];
        for (int ctr = 0; ctr < 0x200; ctr++){
            saveSpace[ctr] = ram[ctr];
        }
        for (int ctr = 0x200; ctr < 0xFFF; ctr++){
            ram[ctr] = code[ctr];
        }  
    }

    void updateKeyPresses(){
        inputMatrix = 0x0000;
        if ((*inputKeys).a){
            inputMatrix |= 0b0000000010000000;
        }
        if ((*inputKeys).q){
            inputMatrix |= 0b0000000000010000;
        }
        if ((*inputKeys).one){
            inputMatrix |= 0b0000000000000010;
        }
        if ((*inputKeys).y){
            inputMatrix |= 0b0000010000000000;
        }
        if ((*inputKeys).two){
            inputMatrix |= 0b0000000000000100;
        }
        if ((*inputKeys).w){
            inputMatrix |= 0b0000000000100000;
        }
        if ((*inputKeys).s){
            inputMatrix |= 0b0000000100000000;
        }
        if ((*inputKeys).x){
            inputMatrix |= 0b0000000000000001;
        }
        if ((*inputKeys).three){
            inputMatrix |= 0b0000000000001000;
        }
        if ((*inputKeys).e){
            inputMatrix |= 0b0000000001000000;
        }
        if ((*inputKeys).d){
            inputMatrix |= 0b0000001000000000;
        }
        if ((*inputKeys).c){
            inputMatrix |= 0b0000100000000000;
        }
        if ((*inputKeys).four){
            inputMatrix |= 0b0001000000000000;
        }
        if ((*inputKeys).r){
            inputMatrix |= 0b0010000000000000;
        }
        if ((*inputKeys).f){
            inputMatrix |= 0b0100000000000000;
        }
        if ((*inputKeys).v){
            inputMatrix |= 0b1000000000000000;
        }
    }

    void runInstruction(){
        uint16_t instruction = ((uint16_t)ram[pc] << 8) | ram[pc + 1];

        switch(instruction){
            case 0x00E0:
                for(int x = 0; x < 64; x++){
                    for(int y = 0; y < 32; y++){
                        screen[x + 64 * y] = false;
                    }
                }
                break;
            case 0x00EE:
                pc = stack[sp];
                sp--;
                break;
            default:
                evaluateAndRun(instruction);
                break;
        }
        pc++;
        pc++;
    }

    void evaluateAndRun(uint16_t instruction){
        uint8_t firstNibble = (uint8_t)((instruction & 0xF000) >> 12);
        uint8_t secondNibble = (uint8_t)((instruction & 0x0F00) >> 8);
        uint8_t thirdNibble = (uint8_t)((instruction & 0x00F0) >> 4);
        uint8_t lastNibble = (uint8_t)(instruction & 0x000F);
        uint8_t lastByte = (uint8_t)(instruction & 0x00FF);
        uint16_t temp = v[secondNibble] + v[thirdNibble];
        uint8_t pressed = v[secondNibble];
        int number = (int)v[secondNibble];
                        int hundreds = 0;
                        int tens = 0;
                        int ones = 0;

        switch(firstNibble){
            case 0x1:
                pc = instruction & 0x0FFF;      // JP addr / 0nnn
                pc--;
                pc--;
                break;
            case 0x2:
                sp++;
                stack[sp] = pc;
                pc = instruction & 0x0FFF;
                pc--;
                pc--;
                break;
            case 0x3:
                if (v[secondNibble] == lastByte){
                    pc++;
                    pc++;
                }
                break;
            case 0x4:
                if (v[secondNibble] != lastByte){
                    pc++;
                    pc++;
                }
                break;
            case 0x5:
                if (v[secondNibble] == v[thirdNibble]){
                    pc++;
                    pc++;
                }
                break;
            case 0x6:
                v[secondNibble] = lastByte;
                break;;
            case 0x7:
                v[secondNibble] = v[secondNibble] + lastByte;
                break;
            case 0x8:
                switch (lastNibble) {
                    case 0x0:
                        v[secondNibble] = v[thirdNibble];
                        break;
                    case 0x1:
                        v[secondNibble] = v[secondNibble] | v[thirdNibble];
                        break;
                    case 0x2:
                        v[secondNibble] = v[secondNibble] & v[thirdNibble];
                        break;
                    case 0x3:
                        v[secondNibble] = v[secondNibble] xor v[thirdNibble];
                        break;
                    case 0x4:
                        if(temp > 0x00FF){
                            v[0xF] = 0x01;
                        }
                        else{
                            v[0xF] = 0x00;
                        }
                        v[secondNibble] = temp & 0x00FF;
                        break;
                    case 0x5:
                        if (v[thirdNibble] < v[secondNibble]){
                            v[0xF] = 1;
                        }
                        else{
                            v[0xF] = 0;
                        }
                        v[secondNibble] = v[secondNibble] - v[thirdNibble];
                        break;
                    case 0x6:
                        if ((instruction & 0x0001) == 0x0001){
                            v[0xF] = 1;
                        }
                        else{
                            v[0xF] = 0;
                        }
                        v[secondNibble] = (uint8_t)(v[secondNibble] / 2);
                        break;
                    case 0x7:
                        if (v[thirdNibble] > v[secondNibble]){
                            v[0xF] = 1;
                        }
                        else{
                            v[0xF] = 0;
                        }
                        v[secondNibble] = v[thirdNibble] - v[secondNibble];
                        break;
                    case 0xE:
                        if(((instruction & 0x1000) >> 12) == 0x0001){
                            v[0xF] = 1;
                        }
                        else{
                            v[0xF] = 0;
                        }
                        v[secondNibble] = v[secondNibble] * 2;
                        break;
                }
                break;
            case 0x9:
                if (v[secondNibble] != v[thirdNibble]){
                    pc++;
                    pc++;
                }
                break;
            case 0xA:
                i = instruction & 0x0FFF;
                break;
            case 0xB:
                pc = (instruction & 0x0FFF) + v[0x0] - 0x0002;
                break;
            case 0xC:
                v[secondNibble] = (uint8_t)(rand() % 256) & lastByte;
                break;
            case 0xD:
                drawSprite(v[secondNibble], v[thirdNibble], lastNibble);
                break;
            case 0xE:
                switch (lastByte){
                    case 0x9E:
                        if (inputMatrix == 0b0000000000000000){
                            break;
                        }
                        else if ((inputMatrix & 0b1000000000000000) != 0b0000000000000000 && pressed == 0x0F){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0100000000000000) != 0b0000000000000000 && pressed == 0x0E){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0010000000000000) != 0b0000000000000000 && pressed == 0x0D){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0001000000000000)!= 0b0000000000000000 && pressed == 0x0C){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000100000000000) != 0b0000000000000000 && pressed == 0x0B){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000010000000000) != 0b0000000000000000 && pressed == 0x0A){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000001000000000) != 0b0000000000000000 && pressed == 0x09){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000100000000) != 0b0000000000000000 && pressed == 0x08){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000010000000) != 0b0000000000000000 && pressed == 0x07){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000001000000) != 0b0000000000000000 && pressed == 0x06){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000100000) != 0b0000000000000000 && pressed == 0x05){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000010000) != 0b0000000000000000 && pressed == 0x04){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000001000) != 0b0000000000000000 && pressed == 0x03){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000100) != 0b0000000000000000 && pressed == 0x02){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000010) != 0b0000000000000000 && pressed == 0x01){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000001) != 0b0000000000000000 && pressed == 0x00){
                            pc++;
                            pc++;
                        }
                        break;
                    case 0xA1:
                        if (inputMatrix == 0b0000000000000000){
                            break;
                        }
                        else if ((inputMatrix & 0b1000000000000000) == 0b0000000000000000 && pressed == 0x0F){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0100000000000000) == 0b0000000000000000 && pressed == 0x0E){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0010000000000000) == 0b0000000000000000 && pressed == 0x0D){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0001000000000000) == 0b0000000000000000 && pressed == 0x0C){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000100000000000) == 0b0000000000000000 && pressed == 0x0B){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000010000000000) == 0b0000000000000000 && pressed == 0x0A){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000001000000000) == 0b0000000000000000 && pressed == 0x09){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000100000000) == 0b0000000000000000 && pressed == 0x08){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000010000000) == 0b0000000000000000 && pressed == 0x07){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000001000000) == 0b0000000000000000 && pressed == 0x06){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000100000) == 0b0000000000000000 && pressed == 0x05){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000010000) == 0b0000000000000000 && pressed == 0x04){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000001000) == 0b0000000000000000 && pressed == 0x03){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000100) == 0b0000000000000000 && pressed == 0x02){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000010) == 0b0000000000000000 && pressed == 0x01){
                            pc++;
                            pc++;
                        }
                        else if ((inputMatrix & 0b0000000000000001) == 0b0000000000000000 && pressed == 0x00){
                            pc++;
                            pc++;
                        }
                        break;
                }
                break;
            case 0xF:
                switch (lastByte){
                    case 0x07:
                        v[secondNibble] = dt;
                        break;
                    case 0x0A:
                        while(inputMatrix == 0x0000){
                            updateKeyPresses();
                        }
                        if ((inputMatrix & 0b1000000000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0F;
                        }
                        else if ((inputMatrix & 0b0100000000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0E;
                        }
                        else if ((inputMatrix & 0b0010000000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0D;
                        }
                        else if ((inputMatrix & 0b0001000000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0C;
                        }
                        else if ((inputMatrix & 0b0000100000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0B;
                        }
                        else if ((inputMatrix & 0b0000010000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x0A;
                        }
                        else if ((inputMatrix & 0b0000001000000000) != 0b0000000000000000){
                            v[secondNibble] = 0x09;
                        }
                        else if ((inputMatrix & 0b0000000100000000) != 0b0000000000000000){
                            v[secondNibble] = 0x08;
                        }
                        else if ((inputMatrix & 0b0000000010000000) != 0b0000000000000000){
                            v[secondNibble] = 0x07;
                        }
                        else if ((inputMatrix & 0b0000000001000000) != 0b0000000000000000){
                            v[secondNibble] = 0x06;
                        }
                        else if ((inputMatrix & 0b0000000000100000) != 0b0000000000000000){
                            v[secondNibble] = 0x05;
                        }
                        else if ((inputMatrix & 0b0000000000010000) != 0b0000000000000000){
                            v[secondNibble] = 0x04;
                        }
                        else if ((inputMatrix & 0b0000000000001000) != 0b0000000000000000){
                            v[secondNibble] = 0x03;
                        }
                        else if ((inputMatrix & 0b0000000000000100) != 0b0000000000000000){
                            v[secondNibble] = 0x02;
                        }
                        else if ((inputMatrix & 0b0000000000000010) != 0b0000000000000000){
                            v[secondNibble] = 0x01;
                        }
                        else if ((inputMatrix & 0b0000000000000001) != 0b0000000000000000){
                            v[secondNibble] = 0x00;
                        }
                        break;

                        break;
                    case 0x15:
                        dt = v[secondNibble];
                        break;
                    case 0x18:
                        st = v[secondNibble];
                        break;
                    case 0x1E:
                        i = i + v[secondNibble];
                        break;
                    case 0x29:
                        i = v[secondNibble] * 0x005 + 0x002;
                        break;
                    case 0x33:
                        while (number > 100){
                            number = number - 100;
                            hundreds++;
                        }
                        while (number > 10){
                            number = number - 10;
                            tens++;
                        }

                        ram[i] = (uint8_t)hundreds;
                        ram[i + 0x0001] = (uint8_t)tens;
                        ram[i + 0x0002] = (uint8_t)ones;
                        break;
                    case 0x55:
                        for(int in = 0; in < 16; in++){
                            ram[i + in] = v[in];
                        }
                        break;
                    case 0x65:
                        for(int in = 0; in < 16; in++){
                            v[in] = ram[i + in];
                        }
                        break;
                }   
        }
    }

    void drawSprite(uint8_t x, uint8_t y, uint8_t lines){
        for(int line = 0; line <= lines; line++){
            uint8_t byte = ram[i + line];
            for(int i = 0; i < 8; i++){
                if((x + i < 64) && (y +line < 32)){
                    uint8_t bit;
                    switch (i){
                        case 0:
                            bit = byte & 0b10000000;
                            break;
                        case 1:
                            bit = byte & 0b01000000;
                            break;
                        case 2:
                            bit = byte & 0b00100000;
                            break;
                        case 3:
                            bit = byte & 0b00010000;
                            break;
                        case 4:
                            bit = byte & 0b00001000;
                            break;
                        case 5:
                            bit = byte & 0b00000100;
                            break;
                        case 6:
                            bit = byte & 0b00000010;
                            break;
                        case 7:
                            bit = byte & 0b00000001;
                            break;
                    }
                    screen[x + i + (y + line)*64] = (bit > 0) ? true : false;
                }
            }
        }
    }

    void drawBuffer(){
        for(int x = 0; x < 64; x++){
            for(int y = 0; y < 32; y++){
                if(screen[x + 64 * y]){
                    drawPixel(x, y, 1);
                }
            }
        }
    }

    void run(){
        
        if(decrementer <= 0){
            if(dt > 0) {dt--;}
            if(st > 0) {st--;}
            decrementer = 16;
        }
        updateKeyPresses();
        runInstruction();
        drawBuffer();
        if (pc > 4095) pc = 4095;
        decrementer = decrementer - 2;
    }

    void loadBinary(string filename, bool isSaveMode){
        fstream fin;
        fin.open(filename, ios::in | ios::binary);
        if(fin){
            uint8_t code[4096] = {0x00};
            if (isSaveMode){
                fin.read(reinterpret_cast<char*>(code + 0x200),(unsigned long)(getFileSize(filename)* sizeof(uint8_t)));
            }
            else {
                fin.read(reinterpret_cast<char*>(code), (unsigned long)(getFileSize(filename) * sizeof(uint8_t)));
            }
            fin.close();
            loadProgram(code);
        }
        else {
            exit(1);
        }
    }
};

#endif