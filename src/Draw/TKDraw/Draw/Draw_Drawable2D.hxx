// Created on: 1994-04-18
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draw_Drawable2D_HeaderFile
#define _Draw_Drawable2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Drawable3D.hxx>

class Draw_Drawable2D;
DEFINE_STANDARD_HANDLE(Draw_Drawable2D, Draw_Drawable3D)

class Draw_Drawable2D : public Draw_Drawable3D
{

public:
  //! Returns False.
  Standard_EXPORT virtual Standard_Boolean Is3D() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Draw_Drawable2D, Draw_Drawable3D)

protected:
private:
};

#endif // _Draw_Drawable2D_HeaderFile
