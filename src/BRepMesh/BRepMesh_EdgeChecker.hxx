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

#ifndef _BRepMesh_EdgeChecker_HeaderFile
#define _BRepMesh_EdgeChecker_HeaderFile

#include <Standard.hxx>
#include <Standard_Mutex.hxx>
#include <Poly_Triangulation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <BRep_Tool.hxx>

//! Auxilary class implementing functionality for checking consistency 
//! of polygon on triangulation of the given edge.
class BRepMesh_EdgeChecker
{
public:

  //! Constructor
  //! @param theFaceTri Poly triangulation of face the edges relie to.
  //! @param theFaceLoc Face location to be used to extract polygon on triangulation.
  //! @param theMutex Upper level shared mutex to protect isFailed flag from concurrent write access.
  //! @param isFailed Upper level shared flag indicating that polygon on triangulation of checked 
  //! edge is not consistent. If this flag is set to TRUE, other tasks will not check details of their data.
  BRepMesh_EdgeChecker( Handle(Poly_Triangulation)& theFaceTri,
                        TopLoc_Location&            theFaceLoc,
                        Standard_Mutex&             theMutex,
                        Standard_Boolean&           isFailed)
    : myMutex(theMutex),
      myIsFailed(isFailed),
      myFaceLoc(theFaceLoc),
      myFaceTri(theFaceTri)
  {
  }

  //! Checker's body.
  //! @param theEdge edge to be checked.
  void operator ()(const TopoDS_Edge& theEdge) const
  {
    if (theEdge.IsNull() || myIsFailed)
      return;

    const Handle(Poly_PolygonOnTriangulation)& aPoly =
      BRep_Tool::PolygonOnTriangulation(theEdge, myFaceTri, myFaceLoc);

    if (!aPoly.IsNull())
      return;

    // Trianglulation stored inside a face is different 
    // than the one an edge data connected to.
    Standard_Mutex::Sentry aSentry(myMutex);
    myIsFailed = Standard_True;
  }

private:

  void operator =(const BRepMesh_EdgeChecker& /*theOther*/)
  {
  }

private:
  Standard_Mutex&             myMutex;
  Standard_Boolean&           myIsFailed;
  TopLoc_Location&            myFaceLoc;
  Handle(Poly_Triangulation)& myFaceTri;
};

#endif
