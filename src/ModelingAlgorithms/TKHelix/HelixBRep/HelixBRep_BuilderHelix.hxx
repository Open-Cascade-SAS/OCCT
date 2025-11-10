// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HelixBRep_BuilderHelix_HeaderFile
#define _HelixBRep_BuilderHelix_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax3.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TopTools_ListOfShape.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
class gp_Ax1;
class gp_Pnt;
class TopoDS_Wire;
class TopoDS_Edge;

//! Implementation of building helix wire
//! Values of Error Status returned by algo:
//! 0 - OK
//! 1 - object is just initialized, it means that no input parameters were set
//! 2 - approximation fails
//!
//! 10 - R < tolerance - starting point is too close to axis
//! 11 - step (Pitch) < tolerance
//! 12 - Height < tolerance
//! 13 - TaperAngle < 0 or TaperAngle > Pi/2 - TolAng
//! Warning Status:
//! 0 - OK
//! 1 - tolerance reached by approximation > requested tolerance.
class HelixBRep_BuilderHelix
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT HelixBRep_BuilderHelix();
  Standard_EXPORT virtual ~HelixBRep_BuilderHelix();

  //! Sets parameters of general composite helix
  Standard_EXPORT void SetParameters(const gp_Ax3&                  theAxis,
                                     const TColStd_Array1OfReal&    theDiams,
                                     const TColStd_Array1OfReal&    theHeights,
                                     const TColStd_Array1OfReal&    thePitches,
                                     const TColStd_Array1OfBoolean& theIsPitches);

  //! Sets parameters of pure helix
  Standard_EXPORT void SetParameters(const gp_Ax3&                  theAxis,
                                     const Standard_Real            theDiam,
                                     const TColStd_Array1OfReal&    theHeights,
                                     const TColStd_Array1OfReal&    thePitches,
                                     const TColStd_Array1OfBoolean& theIsPitches);

  //! Sets parameters of pure spiral
  Standard_EXPORT void SetParameters(const gp_Ax3&                  theAxis,
                                     const Standard_Real            theDiam1,
                                     const Standard_Real            theDiam2,
                                     const TColStd_Array1OfReal&    theHeights,
                                     const TColStd_Array1OfReal&    thePitches,
                                     const TColStd_Array1OfBoolean& theIsPitches);

  //! Sets parameters of general composite helix
  Standard_EXPORT void SetParameters(const gp_Ax3&               theAxis,
                                     const TColStd_Array1OfReal& theDiams,
                                     const TColStd_Array1OfReal& thePitches,
                                     const TColStd_Array1OfReal& theNbTurns);

  //! Sets parameters of pure helix
  Standard_EXPORT void SetParameters(const gp_Ax3&               theAxis,
                                     const Standard_Real         theDiam,
                                     const TColStd_Array1OfReal& thePitches,
                                     const TColStd_Array1OfReal& theNbTurns);

  //! Sets parameters of pure spiral
  Standard_EXPORT void SetParameters(const gp_Ax3&               theAxis,
                                     const Standard_Real         theDiam1,
                                     const Standard_Real         theDiam2,
                                     const TColStd_Array1OfReal& thePitches,
                                     const TColStd_Array1OfReal& theNbTurns);

  //! Sets parameters for approximation
  Standard_EXPORT void SetApproxParameters(const Standard_Real    theTolerance,
                                           const Standard_Integer theMaxDegree,
                                           const GeomAbs_Shape    theContinuity);

  //! Performs calculations
  Standard_EXPORT void Perform();

  //! Gets tolerance reached by approximation
  Standard_EXPORT Standard_Real ToleranceReached() const;

  //! Returns error status of algorithm
  Standard_EXPORT Standard_Integer ErrorStatus() const;

  //! Returns warning status of algorithm
  Standard_EXPORT Standard_Integer WarningStatus() const;

  //! Gets result of algorithm
  Standard_EXPORT const TopoDS_Shape& Shape() const;

protected:
  gp_Ax3                           myAxis3;
  Handle(TColStd_HArray1OfReal)    myDiams;
  Handle(TColStd_HArray1OfReal)    myHeights;
  Handle(TColStd_HArray1OfReal)    myPitches;
  Handle(TColStd_HArray1OfBoolean) myIsPitches;
  Standard_Real                    myTolerance;
  Standard_Real                    myTolReached;
  GeomAbs_Shape                    myContinuity;
  Standard_Integer                 myMaxDegree;
  Standard_Integer                 myMaxSegments;
  Standard_Integer                 myErrorStatus;
  Standard_Integer                 myWarningStatus;
  TopoDS_Shape                     myShape;

private:
  Standard_EXPORT void BuildPart(const gp_Ax1&          theAxis,
                                 const gp_Pnt&          thePStart,
                                 const Standard_Real    theHeight,
                                 const Standard_Real    thePitch,
                                 const Standard_Real    theTaperAngle,
                                 const Standard_Boolean theIsClockwise,
                                 TopoDS_Wire&           thePart);

  Standard_EXPORT void Smoothing(TopTools_ListOfShape& theParts);

  Standard_EXPORT void SmoothingEdges(TopoDS_Edge& thePrev, TopoDS_Edge& theNext);

  Standard_Integer myNParts;
};

#endif // _HelixBRep_BuilderHelix_HeaderFile
