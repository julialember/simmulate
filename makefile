prog:
	@clang main.c -o prog -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OPENGL
	@leaks --atExit -- ./prog
	@rm prog
	
