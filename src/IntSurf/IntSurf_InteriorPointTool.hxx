// Created on: 1992-10-01
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_InteriorPointTool_HeaderFile
#define _IntSurf_InteriorPointTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
class IntSurf_InteriorPoint;


//! This class provides a tool on the "interior point"
//! that can be used to instantiates the Walking
//! algorithmes (see package IntWalk).
class IntSurf_InteriorPointTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the 3d coordinates of the starting point.
    static gp_Pnt Value3d (const IntSurf_InteriorPoint& PStart);
  
  //! Returns the <U,V> parameters which are associated
  //! with <P>
  //! it's the parameters which start the marching algorithm
    static void Value2d (const IntSurf_InteriorPoint& PStart, Standard_Real& U, Standard_Real& V);
  
  //! returns the tangent at the intersectin in 3d space
  //! associated to <P>
    static gp_Vec Direction3d (const IntSurf_InteriorPoint& PStart);
  
  //! returns the tangent at the intersectin in the
  //! parametric space of the parametrized surface.This tangent
  //! is associated to the value2d
    static gp_Dir2d Direction2d (const IntSurf_InteriorPoint& PStart);




protected:





private:





};


#include <IntSurf_InteriorPointTool.lxx>





#endif // _IntSurf_InteriorPointTool_HeaderFile
