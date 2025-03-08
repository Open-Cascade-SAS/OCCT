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

#ifndef _Draw_Number_HeaderFile
#define _Draw_Number_HeaderFile

#include <Draw_Drawable3D.hxx>

DEFINE_STANDARD_HANDLE(Draw_Number, Draw_Drawable3D)

//! To store numbers in variables.
class Draw_Number : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(Draw_Number, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY public :

      Standard_EXPORT
      Draw_Number(const Standard_Real theV);

  Standard_Real Value() const { return myValue; }

  void Value(const Standard_Real theV) { myValue = theV; }

  //! Does nothing,
  Standard_EXPORT virtual void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  //! Returns TRUE if object can be displayed.
  virtual bool IsDisplayable() const Standard_OVERRIDE { return false; }

  //! For variable copy.
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump(Standard_OStream& S) const Standard_OVERRIDE;

  //! Save drawable into stream.
  Standard_EXPORT virtual void Save(Standard_OStream& theStream) const Standard_OVERRIDE;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const Standard_OVERRIDE;

private:
  Standard_Real myValue;
};

#endif // _Draw_Number_HeaderFile
