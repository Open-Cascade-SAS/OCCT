// Created on: 1994-10-04
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepMAT2d_Explorer_HeaderFile
#define _BRepMAT2d_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
class TopoDS_Face;
class TopoDS_Wire;
class Geom2d_Curve;

//! Construct an explorer from wires, face, set of curves
//! from Geom2d to compute the bisecting Locus.
class BRepMAT2d_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepMAT2d_Explorer();

  Standard_EXPORT BRepMAT2d_Explorer(const TopoDS_Face& aFace);

  //! Clear the contents of <me>.
  Standard_EXPORT void Clear();

  Standard_EXPORT void Perform(const TopoDS_Face& aFace);

  //! Returns the Number of contours.
  Standard_EXPORT int NumberOfContours() const;

  //! Returns the Number of Curves in the Contour number
  //! <IndexContour>.
  Standard_EXPORT int NumberOfCurves(const int IndexContour) const;

  //! Initialisation of an Iterator on the curves of
  //! the Contour number <IndexContour>.
  Standard_EXPORT void Init(const int IndexContour);

  //! Return False if there is no more curves on the Contour
  //! initialised by the method Init.
  Standard_EXPORT bool More() const;

  //! Move to the next curve of the current Contour.
  Standard_EXPORT void Next();

  //! Returns the current curve on the current Contour.
  Standard_EXPORT occ::handle<Geom2d_Curve> Value() const;

  Standard_EXPORT TopoDS_Shape Shape() const;

  Standard_EXPORT const NCollection_Sequence<occ::handle<Geom2d_Curve>>& Contour(
    const int IndexContour) const;

  Standard_EXPORT bool IsModified(const TopoDS_Shape& aShape) const;

  //! If the shape is not modified, returns the shape itself.
  Standard_EXPORT TopoDS_Shape ModifiedShape(const TopoDS_Shape& aShape) const;

  Standard_EXPORT const NCollection_Sequence<bool>& GetIsClosed() const;

private:
  //! Construction from a set of curves from Geom2d.
  //! Assume the orientation of the closed lines are
  //! compatible. (ie if A is in B, the orientation of A and B
  //! has to be different.
  //!
  //! Assume the explo contains only lines located in the
  //! area where the bisecting locus will be computed.
  //!
  //! Assume a line don't cross itself or an other line.
  //!
  //! A contour has to be construct in adding each curve in
  //! respect to the sense of the contour.
  //!
  //! afirst point of a curve in a contour is equal to the last
  //! point of the precedent curve.
  //!
  //! No control of this rules is done in the construction
  //! of the explorer
  Standard_EXPORT void Add(const TopoDS_Wire& Spine,
                           const TopoDS_Face& aFace,
                           TopoDS_Face&       aNewFace);

  Standard_EXPORT void NewContour();

  //! Add the curve <aCurve> at me.
  Standard_EXPORT void Add(const occ::handle<Geom2d_Curve>& aCurve);

  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Curve>>>           theCurves;
  int                                                                             current;
  int                                                                             currentContour;
  TopoDS_Shape                                                                    myShape;
  NCollection_Sequence<bool>                                                      myIsClosed;
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myModifShapes;
};

#endif // _BRepMAT2d_Explorer_HeaderFile
