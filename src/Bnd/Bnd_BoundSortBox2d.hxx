// Created on: 1993-03-05
// Created by: Didier PIFFAULT
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

#ifndef _Bnd_BoundSortBox2d_HeaderFile
#define _Bnd_BoundSortBox2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box2d.hxx>
#include <Bnd_HArray1OfBox2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
class Standard_NullValue;
class Standard_MultiplyDefined;
class Bnd_Box2d;


//! A tool to compare a 2D bounding box with a set of 2D
//! bounding boxes. It sorts the set of bounding boxes to give
//! the list of boxes which intersect the element being compared.
//! The boxes being sorted generally bound a set of shapes,
//! while the box being compared bounds a shape to be
//! compared. The resulting list of intersecting boxes therefore
//! gives the list of items which potentially intersect the shape to be compared.
class Bnd_BoundSortBox2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty comparison algorithm for 2D bounding boxes.
  //! The bounding boxes are then defined using the Initialize function.
  Standard_EXPORT Bnd_BoundSortBox2d();
  
  //! Initializes this comparison algorithm with
  //! -   the set of 2D bounding boxes SetOfBox
  Standard_EXPORT void Initialize (const Bnd_Box2d& CompleteBox, const Handle(Bnd_HArray1OfBox2d)& SetOfBox);
  
  //! Initializes this comparison algorithm with
  //! -   the set of 2D bounding boxes SetOfBox, where
  //! CompleteBox is given as the global bounding box of   SetOfBox.
  Standard_EXPORT void Initialize (const Handle(Bnd_HArray1OfBox2d)& SetOfBox);
  
  //! Initializes this comparison algorithm, giving it only
  //! -   the maximum number nbComponents, and
  //! -   the global bounding box CompleteBox,
  //! of the 2D bounding boxes to be managed. Use the Add
  //! function to define the array of bounding boxes to be sorted by this algorithm.
  Standard_EXPORT void Initialize (const Bnd_Box2d& CompleteBox, const Standard_Integer nbComponents);
  
  //! Adds the 2D bounding box theBox at position boxIndex in
  //! the array of boxes to be sorted by this comparison algorithm.
  //! This function is used only in conjunction with the third
  //! syntax described in the synopsis of Initialize.
  //! Exceptions
  //! -   Standard_OutOfRange if boxIndex is not in the
  //! range [ 1,nbComponents ] where
  //! nbComponents is the maximum number of bounding
  //! boxes declared for this comparison algorithm at
  //! initialization.
  //! -   Standard_MultiplyDefined if a box still exists at
  //! position boxIndex in the array of boxes to be sorted by
  //! this comparison algorithm.
  Standard_EXPORT void Add (const Bnd_Box2d& theBox, const Standard_Integer boxIndex);
  

  //! Compares the 2D bounding box theBox with the set of
  //! bounding boxes to be sorted by this comparison algorithm,
  //! and returns the list of intersecting bounding boxes as a list
  //! of indexes on the array of bounding boxes used by this algorithm.
  Standard_EXPORT const TColStd_ListOfInteger& Compare (const Bnd_Box2d& theBox);
  
  Standard_EXPORT void Dump() const;




protected:





private:

  
  //! Prepares BoundSortBox2d and sorts the rectangles of
  //! <SetOfBox> .
  Standard_EXPORT void SortBoxes();


  Bnd_Box2d myBox;
  Handle(Bnd_HArray1OfBox2d) myBndComponents;
  Standard_Real Xmin;
  Standard_Real Ymin;
  Standard_Real deltaX;
  Standard_Real deltaY;
  Standard_Integer discrX;
  Standard_Integer discrY;
  Handle(TColStd_HArray1OfListOfInteger) axisX;
  Handle(TColStd_HArray1OfListOfInteger) axisY;
  Standard_Integer theFound;
  TColStd_DataMapOfIntegerInteger Crible;
  TColStd_ListOfInteger lastResult;


};







#endif // _Bnd_BoundSortBox2d_HeaderFile
