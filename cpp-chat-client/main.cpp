#include <iostream>
#include "Client.h"
#include <Windows.h>

int main(int nargs, char **argv) {
    std::cout << "Computer Networks Chat Client Starting..." << std::endl;
    Application* app = new Client();
    std::cout << "Setting up" << std::endl;
    app->setup();
    std::cout << "Running application" << std::endl;
	MessageBox(NULL, "Goodbye cruel world", "Note", MB_OK);
	app->run();
    delete app;
    return 0;
}
