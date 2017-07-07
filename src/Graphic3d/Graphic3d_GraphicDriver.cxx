// Created on: 1997-01-28
// Created by: CAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Graphic3d_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_GraphicDriver,Standard_Transient)

// =======================================================================
// function : Graphic3d_GraphicDriver
// purpose  :
// =======================================================================
Graphic3d_GraphicDriver::Graphic3d_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp)
: myDisplayConnection (theDisp)
{
  //
}

// =======================================================================
// function : GetDisplayConnection
// purpose  :
// =======================================================================
const Handle(Aspect_DisplayConnection)& Graphic3d_GraphicDriver::GetDisplayConnection() const
{
  return myDisplayConnection;
}

// =======================================================================
// function : NewIdentification
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_GraphicDriver::NewIdentification()
{
  return myStructGenId.Next();
}

// =======================================================================
// function : RemoveIdentification
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriver::RemoveIdentification(const Standard_Integer theId)
{
  myStructGenId.Free(theId);
}