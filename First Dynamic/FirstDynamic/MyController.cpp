#include "MyController.hpp"
#include "MyView.hpp"
#include <tygra/Window.hpp>
#include <iostream>

MyController::
MyController()
{
    /*
     * Tutorial: Create an instance of MyView (our class to draw with OpenGL).
     */

	view_ = std::make_shared<MyView>();
}

MyController::
~MyController()
{
}

void MyController::
windowControlWillStart(std::shared_ptr<tygra::Window> window)
{
    /*
     * Tutorial: Assign our view object to be used by the window and perform
     *           other initialisation work now that the window exists.
     */

    window->setView(view_);
    window->setTitle("3D Graphics Programming :: FirstDynamic");
}

void MyController::
windowControlDidStop(std::shared_ptr<tygra::Window> window)
{
    window->setView(nullptr);
}

void MyController::
windowControlMouseMoved(std::shared_ptr<tygra::Window> window,
                        int x,
                        int y)
{
}

void MyController::
windowControlMouseButtonChanged(std::shared_ptr<tygra::Window> window,
                                int button_index,
                                bool down)
{
}


void MyController::
windowControlMouseWheelMoved(std::shared_ptr<tygra::Window> window,
                             int position)
{
}

void MyController::
windowControlKeyboardChanged(std::shared_ptr<tygra::Window> window,
                             int key_index,
                             bool down)
{
    /*
     * Tutorial: We can use this method to respond to keyboard presses.
     */

    if (down == false) {
        return;
    }

    switch (key_index) {
    case tygra::kWindowKeyF5:
        std::cout << "Reloading shaders not implemented yet ..." << std::endl;
        break;
    }
}

void MyController::
windowControlGamepadAxisMoved(std::shared_ptr<tygra::Window> window,
                              int gamepad_index,
                              int axis_index,
                              float pos)
{
}

void MyController::
windowControlGamepadButtonChanged(std::shared_ptr<tygra::Window> window,
                                  int gamepad_index,
                                  int button_index,
                                  bool down)
{
}
