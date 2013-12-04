// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GraphicDriver.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <OpenGl_CView.hxx>
#include <OpenGl_Display.hxx>

//=======================================================================
//function : Begin
//purpose  :
//=======================================================================

Standard_Boolean OpenGl_GraphicDriver::Begin (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
  myDisplayConnection = theDisplayConnection;
  try
  {
    myGlDisplay = new OpenGl_Display (myDisplayConnection);
    return Standard_True;
  }
  catch (Standard_Failure)
  {
  }
  return Standard_False;
}

//=======================================================================
//function : End
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::End ()
{
  // This is unsafe to realease global object here
  // because noone guaranteed that only one instance of OpenGl_GraphicDriver is used!
  // So we disable this destructor here until openglDisplay not moved to  OpenGl_GraphicDriver class definition.
  ///openglDisplay.Nullify();
}
