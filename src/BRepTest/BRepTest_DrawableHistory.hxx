// Created on: 2018/03/21
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BRepTest_DrawableHistory_HeaderFile
#define _BRepTest_DrawableHistory_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepTools_History.hxx>

#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>

#include <Standard_OStream.hxx>

//! Drawable History object.
//! Allows keeping histories of the algorithms in Draw.
class BRepTest_DrawableHistory : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(BRepTest_DrawableHistory, Draw_Drawable3D)

public:

  //! Creation of the Drawable history.
  BRepTest_DrawableHistory(const Handle(BRepTools_History)& theHistory)
  {
    myHistory = theHistory;
  }

  //! Returns the history.
  const Handle(BRepTools_History)& History() const
  {
    return myHistory;
  }

  //! Drawing is not available.
  Standard_EXPORT virtual void DrawOn(Draw_Display&)const Standard_OVERRIDE;

  //! Dumps the history.
  Standard_EXPORT virtual void Dump(Standard_OStream& theS) const Standard_OVERRIDE;

  //! Prints the type of the history object.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& theDI) const Standard_OVERRIDE;

private:

  Handle(BRepTools_History) myHistory; //!< Tool for tracking History of shape's modification
};

DEFINE_STANDARD_HANDLE(BRepTest_DrawableHistory, Draw_Drawable3D)

#endif
