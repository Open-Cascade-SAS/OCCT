// Created on: 1993-06-08
// Created by: Laurent BOURESCHE
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

#ifndef _BRepSweep_Iterator_HeaderFile
#define _BRepSweep_Iterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Iterator.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
class TopoDS_Shape;


//! This class provides iteration services required by
//! the Generating Line (TopoDS Shape) of a BRepSweep.
//! This   tool is  used  to   iterate  on the  direct
//! sub-shapes of a Shape.
class BRepSweep_Iterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepSweep_Iterator();
  
  //! Reset the Iterator on sub-shapes of <aShape>.
  Standard_EXPORT void Init (const TopoDS_Shape& aShape);
  
  //! Returns True if there is a current sub-shape.
    Standard_Boolean More() const;
  
  //! Moves to the next sub-shape.
  Standard_EXPORT void Next();
  
  //! Returns the current sub-shape.
    const TopoDS_Shape& Value() const;
  
  //! Returns the orientation of the current sub-shape.
    TopAbs_Orientation Orientation() const;




protected:





private:



  TopoDS_Iterator myIterator;


};


#include <BRepSweep_Iterator.lxx>





#endif // _BRepSweep_Iterator_HeaderFile
