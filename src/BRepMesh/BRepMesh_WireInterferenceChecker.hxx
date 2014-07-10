// Created on: 2014-06-18
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_WireInterferenceChecker_HeaderFile
#define _BRepMesh_WireInterferenceChecker_HeaderFile

#include <Standard.hxx>
#include <Standard_Mutex.hxx>
#include <BRepMesh_WireChecker.hxx>
#include <BRepMesh_Status.hxx>

#ifdef HAVE_TBB
  // paralleling using Intel TBB
  #include <tbb/blocked_range.h>
#endif

//! Auxilary class implementing functionality for 
//! checking interference between two discretized wires.
class BRepMesh_WireInterferenceChecker
{
public:

  //! Enumerates states of segments intersection check.
  enum IntFlag
  {
    NoIntersection,
    Cross,
    EndPointTouch,
    PointOnSegment,
    Glued,
    Same
  };

#ifdef HAVE_TBB
  //! Constructor
  //! \param theWires wires that should be checked.
  //! \param theStatus shared flag to set status of the check.
  //! \param theMutex shared mutex for parallel processing.
  BRepMesh_WireInterferenceChecker(
    const std::vector<BRepMesh_WireChecker::SegmentsTree>& theWires,
    BRepMesh_Status*                                       theStatus,
    Standard_Mutex*                                        theMutex);

  //! Checker's body.
  //! \param theWireRange range of wires to be checked.
  void operator ()(const tbb::blocked_range<Standard_Integer>& theWireRange) const;
#else
  //! Constructor
  //! \param theWires wires that should be checked.
  //! \param theStatus shared flag to set status of the check.
  BRepMesh_WireInterferenceChecker(
    const std::vector<BRepMesh_WireChecker::SegmentsTree>& theWires,
    BRepMesh_Status*                                       theStatus);
#endif

  //! Checker's body.
  //! \param theWireId Id of discretized wire to be checked.
  void operator ()(const Standard_Integer& theWireId) const;

  //! Checks intersection between the two segments.
  //! \param theStartPnt1 start point of first segment.
  //! \param theEndPnt1 end point of first segment.
  //! \param theStartPnt2 start point of second segment.
  //! \param theEndPnt2 end point of second segment.
  //! \param isConsiderEndPointTouch if TRUE EndPointTouch status will be
  //! returned in case if segments are touching by end points, if FALSE
  //! returns NoIntersection flag.
  //! \param isConsiderPointOnSegment if TRUE PointOnSegment status will be
  //! returned in case if end point of one segment lies onto another one, 
  //! if FALSE returns NoIntersection flag.
  //! \param[out] theIntPnt point of intersection.
  //! \return status of intersection check.
  static IntFlag Intersect(const gp_XY&           theStartPnt1,
                           const gp_XY&           theEndPnt1,
                           const gp_XY&           theStartPnt2,
                           const gp_XY&           theEndPnt2,
                           const Standard_Boolean isConsiderEndPointTouch,
                           const Standard_Boolean isConsiderPointOnSegment,
                           gp_Pnt2d&              theIntPnt);

private:

  //! Classifies the point in case of coincidence of two vectors.
  //! \param thePoint1 the start point of a segment (base point).
  //! \param thePoint2 the end point of a segment.
  //! \param thePointToCheck the point to classify.
  //! \return zero value if point is out of segment and non zero value 
  //! if point is between the first and the second point of segment.
  static Standard_Integer classifyPoint (const gp_XY& thePoint1,
                                         const gp_XY& thePoint2,
                                         const gp_XY& thePointToCheck);
private:
  const BRepMesh_WireChecker::SegmentsTree* myWires;
  Standard_Integer                          myWiresNb;
  BRepMesh_Status*                          myStatus;

#ifdef HAVE_TBB
  Standard_Mutex*                           myMutex;
#endif
};

#endif
