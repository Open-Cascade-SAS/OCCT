// Created on: 1996-10-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _LocOpe_SplitDrafts_HeaderFile
#define _LocOpe_SplitDrafts_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

class TopoDS_Face;
class TopoDS_Wire;
class gp_Dir;
class gp_Pln;

//! This  class  provides  a    tool to   realize  the
//! following operations on a shape :
//! - split a face of the shape with a wire,
//! - put draft angle on both side of the wire.
//! For each side, the draft angle may be different.
class LocOpe_SplitDrafts
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  LocOpe_SplitDrafts() {}

  //! Creates the algorithm on the shape <S>.
  LocOpe_SplitDrafts(const TopoDS_Shape& S)
      : myShape(S)
  {
  }

  //! Initializes the algorithm with the shape <S>.
  Standard_EXPORT void Init(const TopoDS_Shape& S);

  //! Splits the face <F> of the former given shape with
  //! the wire  <W>.  The wire is  assumed to lie on the
  //! face.    Puts a draft  angle on  both parts of the
  //! wire.    <Extractg>,  <Nplg>, <Angleg> define  the
  //! arguments  for   the   left  part   of the   wire.
  //! <Extractd>,  <Npld>, <Angled> define the arguments
  //! for the right part of the wire. The draft angle is
  //! measured    with the  direction  <Extract>.  <Npl>
  //! defines the neutral plane (points belonging to the
  //! neutral plane are not  modified).  <Angle> is  the
  //! value of the draft  angle.  If <ModifyLeft> is set
  //! to <Standard_False>, no draft  angle is applied to
  //! the left part of the wire. If <ModifyRight> is set
  //! to <Standard_False>,no draft  angle  is applied to
  //! the right part of the wire.
  Standard_EXPORT void Perform(const TopoDS_Face&     F,
                               const TopoDS_Wire&     W,
                               const gp_Dir&          Extractg,
                               const gp_Pln&          NPlg,
                               const Standard_Real    Angleg,
                               const gp_Dir&          Extractd,
                               const gp_Pln&          NPld,
                               const Standard_Real    Angled,
                               const Standard_Boolean ModifyLeft  = Standard_True,
                               const Standard_Boolean ModifyRight = Standard_True);

  //! Splits the face <F> of the former given shape with
  //! the  wire <W>.  The wire is  assumed to lie on the
  //! face.  Puts a draft angle  on the left part of the
  //! wire.   The draft    angle is   measured  with the
  //! direction  <Extract>.   <Npl> defines the  neutral
  //! plane (points belonging  to the neutral plane  are
  //! not modified). <Angle> is  the value of  the draft
  //! angle.
  Standard_EXPORT void Perform(const TopoDS_Face&  F,
                               const TopoDS_Wire&  W,
                               const gp_Dir&       Extract,
                               const gp_Pln&       NPl,
                               const Standard_Real Angle);

  //! Returns <Standard_True> if the modification has been successfully performed.
  Standard_Boolean IsDone() const { return !myResult.IsNull(); }

  const TopoDS_Shape& OriginalShape() const { return myShape; }

  //! Returns the modified shape.
  Standard_EXPORT const TopoDS_Shape& Shape() const;

  //! Manages the descendant shapes.
  Standard_EXPORT const TopTools_ListOfShape& ShapesFromShape(const TopoDS_Shape& S) const;

private:
  TopoDS_Shape                       myShape;
  TopoDS_Shape                       myResult;
  TopTools_DataMapOfShapeListOfShape myMap;
};

#endif // _LocOpe_SplitDrafts_HeaderFile
