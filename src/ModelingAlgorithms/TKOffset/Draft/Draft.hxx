// Created on: 1994-08-31
// Created by: Jacques GOUSSARD
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

#ifndef _Draft_HeaderFile
#define _Draft_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class TopoDS_Face;
class gp_Dir;

class Draft
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the draft angle of the  face <F> using the
  //! direction <Direction>.  The  method is valid for :
  //! - Plane  faces,
  //! - Cylindrical or conical faces, when the direction
  //! of the axis of the surface is colinear with the
  //! direction.
  //! Otherwise, the exception DomainError is raised.
  Standard_EXPORT static Standard_Real Angle(const TopoDS_Face& F, const gp_Dir& Direction);
};

#endif // _Draft_HeaderFile
