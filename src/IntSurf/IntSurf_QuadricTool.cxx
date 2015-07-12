// Copyright (c) 1995-1999 Matra Datavision
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


#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_QuadricTool.hxx>

Standard_Real IntSurf_QuadricTool::Tolerance (const IntSurf_Quadric& Q) {
  switch (Q.TypeQuadric()) {
  case GeomAbs_Sphere:
    return 2.e-6*Q.Sphere().Radius();
  case GeomAbs_Cylinder:
    return 2.e-6*Q.Cylinder().Radius();
  default: 
    break;
  }
  return(1e-6);
}

