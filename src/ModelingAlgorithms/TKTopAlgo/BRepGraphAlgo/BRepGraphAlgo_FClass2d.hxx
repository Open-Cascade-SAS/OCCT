// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraphAlgo_FClass2d_HeaderFile
#define _BRepGraphAlgo_FClass2d_HeaderFile

#include <CSLib_Class2d.hxx>
#include <NCollection_Vector.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopAbs_State.hxx>

class BRepGraph;
class gp_Pnt2d;

//! Graph-based 2D face point classifier.
//!
//! Classifies UV points relative to a face boundary (IN/OUT/ON) using polygon
//! approximation of wire pcurves, similar to BRepTopAdaptor_FClass2d but operating
//! on pre-built BRepGraph data instead of traversing TopoDS topology.
//!
//! The constructor discretizes wire pcurves into polygons and builds CSLib_Class2d
//! classifiers. Subsequent Perform() calls classify points cheaply.
class BRepGraphAlgo_FClass2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Build wire polygon classifiers from graph data for a face.
  //! @param[in] theGraph      pre-built BRepGraph (IsDone() == true)
  //! @param[in] theFaceDefIdx zero-based face definition index
  //! @param[in] theTolUV      classification tolerance in UV space
  Standard_EXPORT BRepGraphAlgo_FClass2d(const BRepGraph& theGraph,
                                         int              theFaceDefIdx,
                                         double           theTolUV);

  //! Classify a UV point relative to the face boundary.
  //! @param[in] thePntUV              point in UV parameter space
  //! @param[in] theRecadreOnPeriodic  wrap coordinates for periodic surfaces
  //! @return classification state (TopAbs_IN, TopAbs_OUT, TopAbs_ON)
  Standard_EXPORT TopAbs_State Perform(const gp_Pnt2d& thePntUV,
                                       bool            theRecadreOnPeriodic = true) const;

  //! Classify a point at infinity (far outside face bounds).
  //! @return TopAbs_OUT for typical faces
  Standard_EXPORT TopAbs_State PerformInfinitePoint() const;

  //! Classify with explicit ON tolerance.
  //! @param[in] thePntUV              point in UV parameter space
  //! @param[in] theTol                tolerance for ON detection
  //! @param[in] theRecadreOnPeriodic  wrap coordinates for periodic surfaces
  //! @return classification state
  Standard_EXPORT TopAbs_State TestOnRestriction(const gp_Pnt2d& thePntUV,
                                                 double          theTol,
                                                 bool            theRecadreOnPeriodic = true) const;

  BRepGraphAlgo_FClass2d(const BRepGraphAlgo_FClass2d&)                = delete;
  BRepGraphAlgo_FClass2d& operator=(const BRepGraphAlgo_FClass2d&)     = delete;
  BRepGraphAlgo_FClass2d(BRepGraphAlgo_FClass2d&&) noexcept            = default;
  BRepGraphAlgo_FClass2d& operator=(BRepGraphAlgo_FClass2d&&) noexcept = default;

private:
  //! Wire orientation in the face.
  enum class WireOrient
  {
    Outer   = 1, //!< Outer boundary wire.
    Inner   = 0, //!< Inner (hole) wire.
    Invalid = -1 //!< Bad/degenerate wire.
  };

  //! Internal classification with periodic retry logic.
  //! @param[in] thePntUV              point in UV parameter space
  //! @param[in] theTol                tolerance for fallback/ON detection
  //! @param[in] theRecadreOnPeriodic  wrap coordinates for periodic surfaces
  //! @param[in] theUseOnMode          true to use SiDans_OnMode (TestOnRestriction),
  //!                                  false to use SiDans (Perform)
  //! @return classification state
  TopAbs_State classifyInternal(const gp_Pnt2d& thePntUV,
                                double          theTol,
                                bool            theRecadreOnPeriodic,
                                bool            theUseOnMode) const;

  NCollection_Vector<CSLib_Class2d> myWireClassifiers; //!< Per-wire polygon classifiers.
  NCollection_Vector<WireOrient>    myWireOrients;     //!< Per-wire orientation.
  double                            myTolUV      = 0.0;
  const BRepGraph*                  myGraph      = nullptr; //!< For fallback face reconstruction.
  int                               myFaceDefIdx = 0;       //!< For fallback face reconstruction.
  double                            myUmin       = 0.0;
  double                            myUmax       = 0.0;
  double                            myVmin       = 0.0;
  double                            myVmax       = 0.0;
  bool                              myIsUPer     = false;
  bool                              myIsVPer     = false;
  double                            myUPeriod    = 0.0;
  double                            myVPeriod    = 0.0;
};

#endif // _BRepGraphAlgo_FClass2d_HeaderFile
