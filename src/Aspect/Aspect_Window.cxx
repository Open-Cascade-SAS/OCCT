// Created by: NW,JPB,CAL,GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Modified:    GG 28/11/00 G002
//              Add BackgroundImage() and BackgroundFillMethod() methods
//-Version
//-Design       Creation d'une fenetre
//-Warning
//-References
//-Language     C++ 2.0
//-Declarations
// for the class

#include <Aspect_Background.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Aspect_WindowError.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_Window,Standard_Transient)

//-Aliases
//-Global data definitions
//-Destructors
//-Constructors
Aspect_Window::Aspect_Window()
: MyBackground(),
  MyGradientBackground(),
  MyBackgroundFillMethod(Aspect_FM_NONE)
{
}

Aspect_Background Aspect_Window::Background() const
{
  return MyBackground;
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

void Aspect_Window::SetBackground (const Aspect_Background& theBackground)
{
  SetBackground (theBackground.Color());
}

void Aspect_Window::SetBackground (const Quantity_Color& theColor)
{
  MyBackground.SetColor (theColor);
}

void Aspect_Window::SetBackground (const Aspect_GradientBackground& theBackground)
{
  Quantity_Color aFirstColor, aSecondColor;
  theBackground.Colors (aFirstColor, aSecondColor);
  SetBackground (aFirstColor, aSecondColor, theBackground.BgGradientFillMethod());
}

void Aspect_Window::SetBackground (const Quantity_Color& theFirstColor,
                                   const Quantity_Color& theSecondColor,
                                   const Aspect_GradientFillMethod theFillMethod)
{
  MyGradientBackground.SetColors (theFirstColor, theSecondColor, theFillMethod);
}
