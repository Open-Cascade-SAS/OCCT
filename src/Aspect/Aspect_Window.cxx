
// File         Aspect_Window.cxx
// Created      Fevrier 1993
// Author       NW,JPB,CAL,GG
// Modified:    GG 28/11/00 G002
//              Add BackgroundImage() and BackgroundFillMethod() methods

//-Copyright    MatraDatavision 1991,1992,1993

//-Version

//-Design       Creation d'une fenetre

//-Warning

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Aspect_Window.ixx>

//-Aliases

//-Global data definitions

//-Destructors

//-Constructors

Aspect_Window::Aspect_Window (const Handle(Aspect_GraphicDevice)& aGraphicDevice)
: MyGraphicDevice(aGraphicDevice),
  MyBackground(),
  MyBackgroundImage(),
  MyGradientBackground(),
  MyBackgroundFillMethod(Aspect_FM_NONE),
  MyHBackground(0)
{
}

Handle(Aspect_GraphicDevice) Aspect_Window::GraphicDevice() const
{
  return MyGraphicDevice;
}

Aspect_Background Aspect_Window::Background() const
{
  return MyBackground;
}

Standard_CString Aspect_Window::BackgroundImage() const
{
  return MyBackgroundImage.ToCString();
}

Aspect_FillMethod Aspect_Window::BackgroundFillMethod() const
{
  return MyBackgroundFillMethod;
}

Aspect_GradientBackground Aspect_Window::GradientBackground() const
{
  return MyGradientBackground;
}

Standard_Boolean Aspect_Window::IsVirtual() const
{
  return MyIsVirtual;
}

void Aspect_Window::SetVirtual (const Standard_Boolean theVirtual)
{
  MyIsVirtual = theVirtual;
}
