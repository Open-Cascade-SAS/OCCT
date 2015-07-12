// Created on: 1998-11-23
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomLib_IsPlanarSurface_HeaderFile
#define _GeomLib_IsPlanarSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pln.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class Geom_Surface;
class gp_Pln;


//! Find if a surface is a planar  surface.
class GeomLib_IsPlanarSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_IsPlanarSurface(const Handle(Geom_Surface)& S, const Standard_Real Tol = 1.0e-7);
  
  //! Return if the Surface is a plan
  Standard_EXPORT Standard_Boolean IsPlanar() const;
  
  //! Return the plan definition
  Standard_EXPORT const gp_Pln& Plan() const;




protected:





private:



  gp_Pln myPlan;
  Standard_Boolean IsPlan;


};







#endif // _GeomLib_IsPlanarSurface_HeaderFile
