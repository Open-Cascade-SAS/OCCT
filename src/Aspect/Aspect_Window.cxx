// Created by: NW,JPB,CAL,GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// Modified:    GG 28/11/00 G002
//              Add BackgroundImage() and BackgroundFillMethod() methods


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
