// Created on: 1992-05-15
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

#ifndef _IntSurf_InteriorPoint_HeaderFile
#define _IntSurf_InteriorPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>


//! Definition of a point solution of the
//! intersection between an implicit an a
//! parametrised surface. These points are
//! passing points on the intersection lines,
//! or starting points for the closed lines
//! on the parametrised surface.
class IntSurf_InteriorPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntSurf_InteriorPoint();
  
  Standard_EXPORT IntSurf_InteriorPoint(const gp_Pnt& P, const Standard_Real U, const Standard_Real V, const gp_Vec& Direc, const gp_Vec2d& Direc2d);
  
  Standard_EXPORT void SetValue (const gp_Pnt& P, const Standard_Real U, const Standard_Real V, const gp_Vec& Direc, const gp_Vec2d& Direc2d);
  
  //! Returns the 3d coordinates of the interior point.
    const gp_Pnt& Value() const;
  
  //! Returns the parameters of the interior point on the
  //! parametric surface.
    void Parameters (Standard_Real& U, Standard_Real& V) const;
  
  //! Returns the first parameter of the interior point on the
  //! parametric surface.
    Standard_Real UParameter() const;
  
  //! Returns the second parameter of the interior point on the
  //! parametric surface.
    Standard_Real VParameter() const;
  
  //! Returns the tangent at the intersection in 3d space
  //! associated to the interior point.
    const gp_Vec& Direction() const;
  
  //! Returns the tangent at the intersection in the parametric
  //! space of the parametric surface.
    const gp_Vec2d& Direction2d() const;




protected:





private:



  gp_Pnt point;
  Standard_Real paramu;
  Standard_Real paramv;
  gp_Vec direc;
  gp_Vec2d direc2d;


};


#include <IntSurf_InteriorPoint.lxx>





#endif // _IntSurf_InteriorPoint_HeaderFile
