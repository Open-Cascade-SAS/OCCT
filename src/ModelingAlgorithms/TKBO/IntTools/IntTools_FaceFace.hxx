// Created on: 2000-11-23
// Created by: Michael KLOKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_FaceFace_HeaderFile
#define _IntTools_FaceFace_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <IntPatch_Intersection.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <TopoDS_Face.hxx>

class IntTools_Context;
class Adaptor3d_TopolTool;

//! This class provides the intersection of
//! face's underlying surfaces.
class IntTools_FaceFace
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT IntTools_FaceFace();

  //! Modifier
  Standard_EXPORT void SetParameters(const Standard_Boolean ApproxCurves,
                                     const Standard_Boolean ComputeCurveOnS1,
                                     const Standard_Boolean ComputeCurveOnS2,
                                     const Standard_Real    ApproximationTolerance);

  //! Intersects underliing surfaces of F1 and F2
  //! Use sum of tolerance of F1 and F2 as intersection
  //! criteria
  Standard_EXPORT void Perform(const TopoDS_Face&     F1,
                               const TopoDS_Face&     F2,
                               const Standard_Boolean theToRunParallel = Standard_False);

  //! Returns True if the intersection was successful
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! Returns sequence of 3d curves as result of intersection
  Standard_EXPORT const IntTools_SequenceOfCurves& Lines() const;

  //! Returns sequence of 3d curves as result of intersection
  Standard_EXPORT const IntTools_SequenceOfPntOn2Faces& Points() const;

  //! Returns first of processed faces
  Standard_EXPORT const TopoDS_Face& Face1() const;

  //! Returns second of processed faces
  Standard_EXPORT const TopoDS_Face& Face2() const;

  //! Returns True if faces are tangent
  Standard_EXPORT Standard_Boolean TangentFaces() const;

  //! Provides post-processing the result lines.
  //! @param[in] bToSplit  split the closed 3D-curves on parts when TRUE,
  //!                      remain untouched otherwise
  Standard_EXPORT void PrepareLines3D(const Standard_Boolean bToSplit = Standard_True);

  Standard_EXPORT void SetList(IntSurf_ListOfPntOn2S& ListOfPnts);

  //! Sets the intersection context
  Standard_EXPORT void SetContext(const Handle(IntTools_Context)& aContext);

  //! Sets the Fuzzy value
  Standard_EXPORT void SetFuzzyValue(const Standard_Real theFuzz);

  //! Returns Fuzzy value
  Standard_EXPORT Standard_Real FuzzyValue() const;

  //! Gets the intersection context
  Standard_EXPORT const Handle(IntTools_Context)& Context() const;

protected:
  //! Creates curves from the IntPatch_Line.
  Standard_EXPORT void MakeCurve(const Standard_Integer             Index,
                                 const Handle(Adaptor3d_TopolTool)& D1,
                                 const Handle(Adaptor3d_TopolTool)& D2,
                                 const Standard_Real                theToler);

  //! Computes the valid tolerance for the intersection curves
  //! as a maximal deviation between 3D curve and 2D curves on faces.<br>
  //! If there are no 2D curves the maximal deviation between 3D curves
  //! and surfaces is computed.
  Standard_EXPORT void ComputeTolReached3d(const Standard_Boolean theToRunParallel);

protected:
  Standard_Boolean               myIsDone;
  IntPatch_Intersection          myIntersector;
  GeomInt_LineConstructor        myLConstruct;
  Handle(GeomAdaptor_Surface)    myHS1;
  Handle(GeomAdaptor_Surface)    myHS2;
  Standard_Integer               myNbrestr;
  Standard_Boolean               myApprox;
  Standard_Boolean               myApprox1;
  Standard_Boolean               myApprox2;
  Standard_Real                  myTolApprox;
  Standard_Real                  myTolF1;
  Standard_Real                  myTolF2;
  Standard_Real                  myTol;
  Standard_Real                  myFuzzyValue;
  IntTools_SequenceOfCurves      mySeqOfCurve;
  Standard_Boolean               myTangentFaces;
  TopoDS_Face                    myFace1;
  TopoDS_Face                    myFace2;
  IntTools_SequenceOfPntOn2Faces myPnts;
  IntSurf_ListOfPntOn2S          myListOfPnts;
  Handle(IntTools_Context)       myContext;
};

#endif // _IntTools_FaceFace_HeaderFile
