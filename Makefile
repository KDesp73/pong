pong: pong.c
	cc -Wall -Iinclude -Iraylib/raylib-5.0_linux_amd64/include -o pong pong.c -L./raylib/raylib-5.0_linux_amd64/lib -lm -lpthread -ldl -l:libraylib.a
	

