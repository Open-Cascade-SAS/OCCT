// Created on: 2014-05-28
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

#ifndef _BRepMesh_FaceChecker_HeaderFile
#define _BRepMesh_FaceChecker_HeaderFile

#include <Standard.hxx>
#include <Standard_Mutex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopLoc_Location.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepMesh_EdgeChecker.hxx>

#include <vector>

#ifdef HAVE_TBB
  // paralleling using Intel TBB
  #include <tbb/parallel_for_each.h>
#endif

//! Auxilary class implementing functionality for 
//! checking consistency of triangulation on the given face.
class BRepMesh_FaceChecker
{
public:

  //! Constructor
  //! @param isInParallel Flag indicates that face edges should be checked in parallel.
  BRepMesh_FaceChecker(const Standard_Boolean isInParallel)
    : myIsFailed(Standard_False),
      myIsInParallel(isInParallel)
  {
  }

  //! Checker's body.
  //! @param theFace Face to be checked.
  void operator ()(const TopoDS_Face& theFace) const
  {
    if (theFace.IsNull() || myIsFailed)
      return;

    TopLoc_Location aFaceLoc;
    Handle(Poly_Triangulation) aFaceT =
      BRep_Tool::Triangulation(theFace, aFaceLoc);

    if (aFaceT.IsNull())
      return;

    std::vector<TopoDS_Edge> aEdges;
    TopExp_Explorer aEdgeIt(theFace, TopAbs_EDGE);
    for ( ; aEdgeIt.More(); aEdgeIt.Next())
      aEdges.push_back(TopoDS::Edge(aEdgeIt.Current()));

    BRepMesh_EdgeChecker aEdgeChecker(aFaceT, aFaceLoc, myMutex, myIsFailed);
#ifdef HAVE_TBB
    if (myIsInParallel)
    {
      // check faces in parallel threads using TBB
      tbb::parallel_for_each(aEdges.begin(), aEdges.end(), aEdgeChecker);
    }
    else
    {
#endif
      for (std::vector<TopoDS_Edge>::iterator it(aEdges.begin()); it != aEdges.end(); it++)
        aEdgeChecker(*it);
#ifdef HAVE_TBB
    }
#endif
  }

  //! Returns status of the check.
  Standard_Boolean IsValid() const 
  {
    return !myIsFailed;
  }

private:
  mutable Standard_Mutex   myMutex;
  mutable Standard_Boolean myIsFailed;
  Standard_Boolean         myIsInParallel;
};

#endif
