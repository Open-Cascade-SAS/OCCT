// Created on: 1993-07-12
// Created by: Yves FRICAUD
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

#ifndef _MAT2d_SketchExplorer_HeaderFile
#define _MAT2d_SketchExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_Curve;


//! SketchExplorer  is  an iterator on  a  sketch.   A
//! sketch is a set of contours, each contour is a set
//! of curves from Geom2d.
//! It's use by BisectingLocus.
class MAT2d_SketchExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of contours in the figure.
  Standard_EXPORT Standard_Integer NumberOfContours() const;
  
  //! Initializes the curves explorer on the contour
  //! of range <ContourIndex>.
  Standard_EXPORT void Init (const Standard_Integer ContourIndex);
  
  //! Returns False if  there is no  more curves on the
  //! current contour.
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Moves to the next curve of the current contour.
  Standard_EXPORT void Next();
  
  //! Returns the current curve on the current contour.
  Standard_EXPORT Handle(Geom2d_Curve) Value() const;




protected:





private:





};







#endif // _MAT2d_SketchExplorer_HeaderFile
