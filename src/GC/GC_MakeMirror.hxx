// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GC_MakeMirror_HeaderFile
#define _GC_MakeMirror_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom_Transformation;
class gp_Pnt;
class gp_Ax1;
class gp_Lin;
class gp_Dir;
class gp_Pln;
class gp_Ax2;


//! This class implements elementary construction algorithms for a
//! symmetrical transformation in 3D space about a point,
//! axis or plane. The result is a Geom_Transformation transformation.
//! A MakeMirror object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GC_MakeMirror 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GC_MakeMirror(const gp_Pnt& Point);
  
  Standard_EXPORT GC_MakeMirror(const gp_Ax1& Axis);
  
  Standard_EXPORT GC_MakeMirror(const gp_Lin& Line);
  
  //! Make a symmetry transformation af axis defined by
  //! <Point> and <Direc>.
  Standard_EXPORT GC_MakeMirror(const gp_Pnt& Point, const gp_Dir& Direc);
  
  //! Make a symmetry transformation of plane <Plane>.
  Standard_EXPORT GC_MakeMirror(const gp_Pln& Plane);
  
  //! Make a symmetry transformation of plane <Plane>.
  Standard_EXPORT GC_MakeMirror(const gp_Ax2& Plane);
  
  //! Returns the constructed transformation.
  Standard_EXPORT const Handle(Geom_Transformation)& Value() const;
operator const Handle(Geom_Transformation)& () const { return Value(); }




protected:





private:



  Handle(Geom_Transformation) TheMirror;


};







#endif // _GC_MakeMirror_HeaderFile
