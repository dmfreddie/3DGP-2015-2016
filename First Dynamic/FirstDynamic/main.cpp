#include <crtdbg.h>
#include <cstdlib>

#include "MyController.hpp"
#include <tygra/Window.hpp>

int main(int argc, char *argv[])
{
    // enable debug memory checks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /*
     * Tutorial: This is the tygra framework initialisation and runloop. You
     *           shouldn't need to change much here.
     */

	std::shared_ptr<MyController> controller = std::make_shared<MyController>();
    std::shared_ptr<tygra::Window> window = tygra::Window::mainWindow();
    window->setController(controller);

    const int window_width = 600;
    const int window_height = 600;
    const int number_of_samples = 4;

    if (window->open(window_width, window_height, number_of_samples, true)) {
        while (window->isVisible()) {
            window->update();
        }
        window->close();
    }

    // pause to display any console debug messages
    system("PAUSE");
    return 0;
}
