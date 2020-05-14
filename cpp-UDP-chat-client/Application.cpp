#include "Application.h"

void Application::run() {
	while (!this->stop) {
        this->tick();
    }
}

void Application::stopApplication() {
    this->stop = true;
}

bool Application::isStopped() {
    return this->stop;
}
