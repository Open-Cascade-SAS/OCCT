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

  //! Constructor
  //! @param theWires wires that should be checked.
  //! @param theStatus shared flag to set status of the check.
  //! @param theMutex shared mutex for parallel processing.
  BRepMesh_WireInterferenceChecker(
    const BRepMesh::Array1OfSegmentsTree& theWires,
    BRepMesh_Status*                      theStatus,
    Standard_Mutex*                       theMutex = NULL);

  //! Checker's body.
  //! @param theWireId Id of discretized wire to be checked.
  void operator ()(const Standard_Integer& theWireId) const;

private:

  //! Assignment operator.
  void operator =(const BRepMesh_WireInterferenceChecker& /*theOther*/)
  {
  }

private:
  const BRepMesh::Array1OfSegmentsTree& myWires;
  BRepMesh_Status*                      myStatus;
  Standard_Mutex*                       myMutex;
};

#endif
