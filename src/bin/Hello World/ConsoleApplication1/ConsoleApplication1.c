#include <Windows.h>

int mainCRTStartup(void) {
	char text[14] = "Hello World!\n";
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), text, 13, NULL, NULL);
	ExitProcess(0);
}
