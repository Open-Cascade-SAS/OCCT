// Created on: 1993-08-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Draw_Chronometer_HeaderFile
#define _Draw_Chronometer_HeaderFile

#include <Draw_Drawable3D.hxx>
#include <OSD_Timer.hxx>

DEFINE_STANDARD_HANDLE(Draw_Chronometer, Draw_Drawable3D)

//! Class to store chronometer variables.
class Draw_Chronometer : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(Draw_Chronometer, Draw_Drawable3D)
public:

  Standard_EXPORT Draw_Chronometer();

  //! Return timer.
  OSD_Timer& Timer() { return myTimer; }

  //! Does nothing,
  Standard_EXPORT void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;
  
  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;

private:

  OSD_Timer myTimer;

};

#endif // _Draw_Chronometer_HeaderFile
