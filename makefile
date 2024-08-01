CFLAGS = -Wno-deprecated-declarations -Wc++11-extensions
CLINKS = -L/System/Library/Frameworks -framework GLUT -framework OpenGL

chip8: main.cpp inputs.cpp chip8.cpp
	@echo "Compiling CHIP-8-EMULATOR"
	@g++ main.cpp  $(CLINKS) $(CFLAGS)  -o chip8

clean:
	@rm chip8