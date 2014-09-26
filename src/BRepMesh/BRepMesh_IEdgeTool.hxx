// Created on: 2014-08-13
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_IEdgeTool_HeaderFile
#define _BRepMesh_IEdgeTool_HeaderFile

#include <Standard.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>

//! Interface class providing API for edge tessellation tools.
class BRepMesh_IEdgeTool : public Standard_Transient
{
public:
  //! Returns number of tessellation points.
  virtual Standard_Integer NbPoints() const = 0;

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @param thePoint tessellation point.
  //! @param theUV coordinates of tessellation point in parametric space of face.
  virtual void Value(const Standard_Integer theIndex,
                     Standard_Real&         theParameter,
                     gp_Pnt&                thePoint,
                     gp_Pnt2d&              theUV) = 0;

  DEFINE_STANDARD_RTTI(BRepMesh_IEdgeTool)
};

DEFINE_STANDARD_HANDLE(BRepMesh_IEdgeTool, Standard_Transient)

#endif
