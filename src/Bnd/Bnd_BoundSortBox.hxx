// Created on: 1992-11-24
// Created by: Didier PIFFAULT
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

#ifndef _Bnd_BoundSortBox_HeaderFile
#define _Bnd_BoundSortBox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
class gp_Pln;

//! A tool to compare a bounding box or a plane with a set of
//! bounding boxes. It sorts the set of bounding boxes to give
//! the list of boxes which intersect the element being compared.
//! The boxes being sorted generally bound a set of shapes,
//! while the box being compared bounds a shape to be
//! compared. The resulting list of intersecting boxes therefore
//! gives the list of items which potentially intersect the shape to be compared.
class Bnd_BoundSortBox
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty comparison algorithm for bounding boxes.
  //! The bounding boxes are then defined using the Initialize function.
  Standard_EXPORT Bnd_BoundSortBox();

  //! Initializes this comparison algorithm with
  //! -   the set of bounding boxes SetOfBox.
  Standard_EXPORT void Initialize(const Bnd_Box&                  CompleteBox,
                                  const Handle(Bnd_HArray1OfBox)& SetOfBox);

  //! Initializes this comparison algorithm with
  //! -   the set of bounding boxes SetOfBox, where
  //! CompleteBox is given as the global bounding box of SetOfBox.
  Standard_EXPORT void Initialize(const Handle(Bnd_HArray1OfBox)& SetOfBox);

  //! Initializes this comparison algorithm, giving it only
  //! -   the maximum number nbComponents
  //! of the bounding boxes to be managed. Use the Add
  //! function to define the array of bounding boxes to be sorted by this algorithm.
  Standard_EXPORT void Initialize(const Bnd_Box& CompleteBox, const Standard_Integer nbComponents);

  //! Adds the bounding box theBox at position boxIndex in
  //! the array of boxes to be sorted by this comparison algorithm.
  //! This function is used only in conjunction with the third
  //! syntax described in the synopsis of Initialize.
  //!
  //! Exceptions:
  //!
  //! - Standard_OutOfRange if boxIndex is not in the
  //! range [ 1,nbComponents ] where
  //! nbComponents is the maximum number of bounding
  //! boxes declared for this comparison algorithm at
  //! initialization.
  //!
  //! - Standard_MultiplyDefined if a box already exists at
  //! position boxIndex in the array of boxes to be sorted by
  //! this comparison algorithm.
  Standard_EXPORT void Add(const Bnd_Box& theBox, const Standard_Integer boxIndex);

  //! Compares the bounding box theBox,
  //! with the set of bounding boxes to be sorted by this
  //! comparison algorithm, and returns the list of intersecting
  //! bounding boxes as a list of indexes on the array of
  //! bounding boxes used by this algorithm.
  Standard_EXPORT const TColStd_ListOfInteger& Compare(const Bnd_Box& theBox);

  //! Compares the plane P
  //! with the set of bounding boxes to be sorted by this
  //! comparison algorithm, and returns the list of intersecting
  //! bounding boxes as a list of indexes on the array of
  //! bounding boxes used by this algorithm.
  Standard_EXPORT const TColStd_ListOfInteger& Compare(const gp_Pln& P);

  Standard_EXPORT void Dump() const;

  Standard_EXPORT void Destroy();

  ~Bnd_BoundSortBox() { Destroy(); }

protected:
private:
  //! Prepares  BoundSortBox and  sorts   the  boxes of
  //! <SetOfBox> .
  Standard_EXPORT void SortBoxes();

  Bnd_Box                         myBox;
  Handle(Bnd_HArray1OfBox)        myBndComponents;
  Standard_Real                   Xmin;
  Standard_Real                   Ymin;
  Standard_Real                   Zmin;
  Standard_Real                   deltaX;
  Standard_Real                   deltaY;
  Standard_Real                   deltaZ;
  Standard_Integer                discrX;
  Standard_Integer                discrY;
  Standard_Integer                discrZ;
  Standard_Integer                theFound;
  TColStd_DataMapOfIntegerInteger Crible;
  TColStd_ListOfInteger           lastResult;
  Standard_Address                TabBits;
};

#endif // _Bnd_BoundSortBox_HeaderFile
