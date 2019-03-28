// Created on: 1993-01-14
// Created by: Remi LEQUETTE
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

#ifndef _TopTools_ShapeMapHasher_HeaderFile
#define _TopTools_ShapeMapHasher_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;


//! Hash tool, used for generating maps of shapes in topology.
class TopTools_ShapeMapHasher 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Computes a hash code for the given shape, in the range [1, theUpperBound]
  //! @param theShape the shape which hash code is to be computed
  //! @param theUpperBound the upper bound of the range a computing hash code must be within
  //! @return a computed hash code, in the range [1, theUpperBound]
  static Standard_Integer HashCode (const TopoDS_Shape& theShape, Standard_Integer theUpperBound);

  //! Returns True  when the two  keys are the same. Two
  //! same  keys  must   have  the  same  hashcode,  the
  //! contrary is not necessary.
    static Standard_Boolean IsEqual (const TopoDS_Shape& S1, const TopoDS_Shape& S2);




protected:





private:





};


#include <TopTools_ShapeMapHasher.lxx>





#endif // _TopTools_ShapeMapHasher_HeaderFile
