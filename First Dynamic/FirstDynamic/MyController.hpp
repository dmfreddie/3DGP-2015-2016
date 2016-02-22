#pragma once
#include <tygra/WindowControlDelegate.hpp>

class MyView;

/*
 * Tutorial: This class exists to integrate with the tygra framework. It
 *           implements the tygra::WindowControlDelegate interface which the
 *           tygra::Window object (which receives input events) uses to respond
 *           to the operating system (include mouse and keyboard actions).
 */

class MyController : public tygra::WindowControlDelegate
{
public:
	
    MyController();

    ~MyController();

private:
    /*
     * Tutorial: These methods are needed to implement the framework's
     *           tygra::WindowControlDelegate interface.
     */

    void
    windowControlWillStart(std::shared_ptr<tygra::Window> window);

    void
    windowControlDidStop(std::shared_ptr<tygra::Window> window);

    void
    windowControlMouseMoved(std::shared_ptr<tygra::Window> window,
                            int x,
                            int y);	

    void
    windowControlMouseButtonChanged(std::shared_ptr<tygra::Window> window,
                                    int button_index,
                                    bool down);


    void
    windowControlMouseWheelMoved(std::shared_ptr<tygra::Window> window,
                                 int position);

    void
    windowControlKeyboardChanged(std::shared_ptr<tygra::Window> window,
                                 int key_index,
                                 bool down);

    void
    windowControlGamepadAxisMoved(std::shared_ptr<tygra::Window> window,
                                  int gamepad_index,
                                  int axis_index,
                                  float pos);

    void
    windowControlGamepadButtonChanged(std::shared_ptr<tygra::Window> window,
                                      int gamepad_index,
                                      int button_index,
                                      bool down);

private:
	std::shared_ptr<MyView> view_;

};
