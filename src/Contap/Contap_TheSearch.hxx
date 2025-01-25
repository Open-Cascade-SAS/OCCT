// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
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

#ifndef _Contap_TheSearch_HeaderFile
#define _Contap_TheSearch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Contap_SequenceOfSegmentOfTheSearch.hxx>
#include <Contap_SequenceOfPathPointOfTheSearch.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_ConstructionError;
class Adaptor3d_HVertex;
class Contap_HCurve2dTool;
class Contap_HContTool;
class Adaptor3d_TopolTool;
class Contap_ArcFunction;
class Contap_ThePathPointOfTheSearch;
class Contap_TheSegmentOfTheSearch;

class Contap_TheSearch
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Contap_TheSearch();

  //! Algorithm to find the points and parts of curves of Domain
  //! (domain of of restriction of a surface) which verify
  //! F = 0.
  //! TolBoundary defines if a curve is on Q.
  //! TolTangency defines if a point is on Q.
  Standard_EXPORT void Perform(Contap_ArcFunction&                F,
                               const Handle(Adaptor3d_TopolTool)& Domain,
                               const Standard_Real                TolBoundary,
                               const Standard_Real                TolTangency,
                               const Standard_Boolean RecheckOnRegularity = Standard_False);

  //! Returns True if the calculus was successful.
  Standard_Boolean IsDone() const;

  //! Returns true if all arc of the Arcs are solution (inside
  //! the surface).
  //! An exception is raised if IsDone returns False.
  Standard_Boolean AllArcSolution() const;

  //! Returns the number of resulting points.
  //! An exception is raised if IsDone returns False (NotDone).
  Standard_Integer NbPoints() const;

  //! Returns the resulting point of range Index.
  //! The exception NotDone is raised if IsDone() returns
  //! False.
  //! The exception OutOfRange is raised if
  //! Index <= 0 or Index > NbPoints.
  const Contap_ThePathPointOfTheSearch& Point(const Standard_Integer Index) const;

  //! Returns the number of the resulting segments.
  //! An exception is raised if IsDone returns False (NotDone).
  Standard_Integer NbSegments() const;

  //! Returns the resulting segment of range Index.
  //! The exception NotDone is raised if IsDone() returns
  //! False.
  //! The exception OutOfRange is raised if
  //! Index <= 0 or Index > NbPoints.
  const Contap_TheSegmentOfTheSearch& Segment(const Standard_Integer Index) const;

protected:
private:
  Standard_Boolean                      done;
  Standard_Boolean                      all;
  Contap_SequenceOfSegmentOfTheSearch   sseg;
  Contap_SequenceOfPathPointOfTheSearch spnt;
};

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_Curve2d)
#define TheArc_hxx <Adaptor2d_Curve2d.hxx>
#define TheArcTool Contap_HCurve2dTool
#define TheArcTool_hxx <Contap_HCurve2dTool.hxx>
#define TheSOBTool Contap_HContTool
#define TheSOBTool_hxx <Contap_HContTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheFunction Contap_ArcFunction
#define TheFunction_hxx <Contap_ArcFunction.hxx>
#define IntStart_ThePathPoint Contap_ThePathPointOfTheSearch
#define IntStart_ThePathPoint_hxx <Contap_ThePathPointOfTheSearch.hxx>
#define IntStart_SequenceOfPathPoint Contap_SequenceOfPathPointOfTheSearch
#define IntStart_SequenceOfPathPoint_hxx <Contap_SequenceOfPathPointOfTheSearch.hxx>
#define IntStart_TheSegment Contap_TheSegmentOfTheSearch
#define IntStart_TheSegment_hxx <Contap_TheSegmentOfTheSearch.hxx>
#define IntStart_SequenceOfSegment Contap_SequenceOfSegmentOfTheSearch
#define IntStart_SequenceOfSegment_hxx <Contap_SequenceOfSegmentOfTheSearch.hxx>
#define IntStart_SearchOnBoundaries Contap_TheSearch
#define IntStart_SearchOnBoundaries_hxx <Contap_TheSearch.hxx>

#include <IntStart_SearchOnBoundaries.lxx>

#undef TheVertex
#undef TheVertex_hxx
#undef TheArc
#undef TheArc_hxx
#undef TheArcTool
#undef TheArcTool_hxx
#undef TheSOBTool
#undef TheSOBTool_hxx
#undef Handle_TheTopolTool
#undef TheTopolTool
#undef TheTopolTool_hxx
#undef TheFunction
#undef TheFunction_hxx
#undef IntStart_ThePathPoint
#undef IntStart_ThePathPoint_hxx
#undef IntStart_SequenceOfPathPoint
#undef IntStart_SequenceOfPathPoint_hxx
#undef IntStart_TheSegment
#undef IntStart_TheSegment_hxx
#undef IntStart_SequenceOfSegment
#undef IntStart_SequenceOfSegment_hxx
#undef IntStart_SearchOnBoundaries
#undef IntStart_SearchOnBoundaries_hxx

#endif // _Contap_TheSearch_HeaderFile
