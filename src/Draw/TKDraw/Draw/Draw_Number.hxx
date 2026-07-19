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

//! To store numbers in variables.
class Draw_Number : public Draw_Drawable3D
{
  DEFINE_STANDARD_RTTIEXT(Draw_Number, Draw_Drawable3D)
  Draw_Drawable3D_FACTORY
public:
  Standard_EXPORT Draw_Number(const double theV);

  double Value() const { return myValue; }

  void Value(const double theV) { myValue = theV; }

  //! Does nothing,
  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  //! Returns TRUE if object can be displayed.
  bool IsDisplayable() const override { return false; }

  //! For variable copy.
  Standard_EXPORT occ::handle<Draw_Drawable3D> Copy() const override;

  //! For variable dump.
  Standard_EXPORT void Dump(Standard_OStream& S) const override;

  //! Save drawable into stream.
  Standard_EXPORT void Save(Standard_OStream& theStream) const override;

  //! For variable whatis command. Set as a result the type of the variable.
  Standard_EXPORT void Whatis(Draw_Interpretor& I) const override;

private:
  double myValue;
};

#endif // _Draw_Number_HeaderFile
