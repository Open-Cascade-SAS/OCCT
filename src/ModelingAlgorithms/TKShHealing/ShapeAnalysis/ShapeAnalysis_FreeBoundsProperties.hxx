// Created on: 1998-07-30
// Created by: Pavel DURANDIN <pdn@nnov.matra-dtv.fr>
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

#ifndef _ShapeAnalysis_FreeBoundsProperties_HeaderFile
#define _ShapeAnalysis_FreeBoundsProperties_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <ShapeAnalysis_FreeBoundData.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
class ShapeAnalysis_FreeBoundData;
class TopoDS_Wire;

//! This class is intended to calculate shape free bounds
//! properties.
//! This class provides the following functionalities:
//! - calculates area of the contour,
//! - calculates perimeter of the contour,
//! - calculates ratio of average length to average width of the
//! contour,
//! - estimates average width of contour,
//! - finds the notches (narrow 'V'-like sub-contour) on the
//! contour.
//!
//! For getting free bounds this class uses
//! ShapeAnalysis_FreeBounds class.
//!
//! For description of parameters used for initializing this
//! class refer to CDL of ShapeAnalysis_FreeBounds.
//!
//! Properties of each contour are stored in the data structure
//! ShapeAnalysis_FreeBoundData.
class ShapeAnalysis_FreeBoundsProperties
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_FreeBoundsProperties();

  //! Creates the object and calls corresponding Init.
  //! <shape> should be a compound of faces.
  Standard_EXPORT ShapeAnalysis_FreeBoundsProperties(const TopoDS_Shape& shape,
                                                     const double        tolerance,
                                                     const bool          splitclosed = false,
                                                     const bool          splitopen   = false);

  //! Creates the object and calls corresponding Init.
  //! <shape> should be a compound of shells.
  Standard_EXPORT ShapeAnalysis_FreeBoundsProperties(const TopoDS_Shape& shape,
                                                     const bool          splitclosed = false,
                                                     const bool          splitopen   = false);

  //! Initializes the object with given parameters.
  //! <shape> should be a compound of faces.
  Standard_EXPORT void Init(const TopoDS_Shape& shape,
                            const double        tolerance,
                            const bool          splitclosed = false,
                            const bool          splitopen   = false);

  //! Initializes the object with given parameters.
  //! <shape> should be a compound of shells.
  Standard_EXPORT void Init(const TopoDS_Shape& shape,
                            const bool          splitclosed = false,
                            const bool          splitopen   = false);

  //! Builds and analyzes free bounds of the shape.
  //! First calls ShapeAnalysis_FreeBounds for building free
  //! bounds.
  //! Then on each free bound computes its properties:
  //! - area of the contour,
  //! - perimeter of the contour,
  //! - ratio of average length to average width of the contour,
  //! - average width of contour,
  //! - notches on the contour and for each notch
  //! - maximum width of the notch.
  Standard_EXPORT bool Perform();

  //! Returns True if shape is loaded
  bool IsLoaded() const;

  //! Returns shape
  TopoDS_Shape Shape() const;

  //! Returns tolerance
  double Tolerance() const;

  //! Returns number of free bounds
  int NbFreeBounds() const;

  //! Returns number of closed free bounds
  int NbClosedFreeBounds() const;

  //! Returns number of open free bounds
  int NbOpenFreeBounds() const;

  //! Returns all closed free bounds
  occ::handle<NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>> ClosedFreeBounds()
    const;

  //! Returns all open free bounds
  occ::handle<NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>> OpenFreeBounds()
    const;

  //! Returns properties of closed free bound specified by its rank
  //! number
  occ::handle<ShapeAnalysis_FreeBoundData> ClosedFreeBound(const int index) const;

  //! Returns properties of open free bound specified by its rank
  //! number
  occ::handle<ShapeAnalysis_FreeBoundData> OpenFreeBound(const int index) const;

  Standard_EXPORT bool DispatchBounds();

  Standard_EXPORT bool CheckContours(const double prec = 0.0);

  Standard_EXPORT bool CheckNotches(const double prec = 0.0);

  Standard_EXPORT bool CheckNotches(occ::handle<ShapeAnalysis_FreeBoundData>& fbData,
                                    const double                              prec = 0.0);

  Standard_EXPORT bool CheckNotches(const TopoDS_Wire& freebound,
                                    const int          num,
                                    TopoDS_Wire&       notch,
                                    double&            distMax,
                                    const double       prec = 0.0);

  Standard_EXPORT bool FillProperties(occ::handle<ShapeAnalysis_FreeBoundData>& fbData,
                                      const double                              prec = 0.0);

private:
  TopoDS_Shape                                                                 myShape;
  double                                                                       myTolerance;
  bool                                                                         mySplitClosed;
  bool                                                                         mySplitOpen;
  occ::handle<NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>> myClosedFreeBounds;
  occ::handle<NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>> myOpenFreeBounds;
};

#include <ShapeAnalysis_FreeBoundsProperties.lxx>

#endif // _ShapeAnalysis_FreeBoundsProperties_HeaderFile
