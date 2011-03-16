#include <Aspect_WindowDriver.ixx>

Aspect_WindowDriver::Aspect_WindowDriver(const Handle(Aspect_Window)& aWindow) : MyWindow(aWindow) {}

Handle(Aspect_Window) Aspect_WindowDriver::Window() const { 

  return MyWindow;

}
