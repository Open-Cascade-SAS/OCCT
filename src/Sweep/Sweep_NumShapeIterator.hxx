// Created on: 1993-06-02
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

#ifndef _Sweep_NumShapeIterator_HeaderFile
#define _Sweep_NumShapeIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Sweep_NumShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class Sweep_NumShape;


//! This class provides iteration services required by
//! the   Swept Primitives  for   a Directing NumShape
//! Line.
class Sweep_NumShapeIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Sweep_NumShapeIterator();
  
  //! Reset the NumShapeIterator on sub-shapes of <aShape>.
  Standard_EXPORT void Init (const Sweep_NumShape& aShape);
  
  //! Returns True if there is a current sub-shape.
    Standard_Boolean More() const;
  
  //! Moves to the next sub-shape.
  Standard_EXPORT void Next();
  
  //! Returns the current sub-shape.
    const Sweep_NumShape& Value() const;
  
  //! Returns the orientation of the current sub-shape.
    TopAbs_Orientation Orientation() const;




protected:





private:



  Sweep_NumShape myNumShape;
  Sweep_NumShape myCurrentNumShape;
  Standard_Integer myCurrentRange;
  Standard_Boolean myMore;
  TopAbs_Orientation myCurrentOrientation;


};


#include <Sweep_NumShapeIterator.lxx>





#endif // _Sweep_NumShapeIterator_HeaderFile
