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

#ifndef _BRepExtrema_SolutionElem_HeaderFile
#define _BRepExtrema_SolutionElem_HeaderFile

#include <gp_Pnt.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! This class is used to store information relative to the minimum distance between two shapes.
class BRepExtrema_SolutionElem
{
 public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  BRepExtrema_SolutionElem()
  : myDist    (0.0),
    myPoint   (0.0, 0.0, 0.0),
    mySupType (BRepExtrema_IsVertex),
    myPar1    (0.0),
    myPar2    (0.0)
  {
  }

  //! This constructor is used when the solution of a distance is a Vertex.
  //! The different initialized fields are:
  //! @param theDist    the distance
  //! @param thePoint   the solution point
  //! @param theSolType the type of solution
  //! @param theVertex  and the Vertex
  BRepExtrema_SolutionElem (const Standard_Real           theDist,
                            const gp_Pnt&                 thePoint,
                            const BRepExtrema_SupportType theSolType,
                            const TopoDS_Vertex&          theVertex)
  : myDist    (theDist),
    myPoint   (thePoint),
    mySupType (theSolType),
    myVertex  (theVertex),
    myPar1    (0.0),
    myPar2    (0.0) {}

  //! This constructor is used when the  solution of distance is on an Edge.
  //! The different initialized fields are:
  //! @param theDist    the distance
  //! @param thePoint   the solution point
  //! @param theSolType the type of solution
  //! @param theEdge    the Edge
  //! @param theParam   the parameter to locate the solution
  BRepExtrema_SolutionElem (const Standard_Real           theDist,
                            const gp_Pnt&                 thePoint,
                            const BRepExtrema_SupportType theSolType,
                            const TopoDS_Edge&            theEdge,
                            const Standard_Real           theParam)
  : myDist    (theDist),
    myPoint   (thePoint),
    mySupType (theSolType),
    myEdge    (theEdge),
    myPar1    (theParam),
    myPar2    (0.0) {}

  //! This constructor is used when the  solution of distance is in a Face.
  //! The different initialized fields are:
  //! @param theDist    the distance
  //! @param thePoint   the solution point
  //! @param theSolType the type of solution
  //! @param theFace    the Face
  //! @param theU       U parameter to locate the solution
  //! @param theV       V parameter to locate the solution
  BRepExtrema_SolutionElem (const Standard_Real           theDist,
                            const gp_Pnt&                 thePoint,
                            const BRepExtrema_SupportType theSolType,
                            const TopoDS_Face&            theFace,
                            const Standard_Real           theU,
                            const Standard_Real           theV)
  : myDist    (theDist),
    myPoint   (thePoint),
    mySupType (theSolType),
    myFace    (theFace),
    myPar1    (theU),
    myPar2    (theV) {}

  //! Returns the value of the minimum distance.
  Standard_Real Dist() const
  {
    return myDist;
  }

  //! Returns the solution point.
  const gp_Pnt& Point() const
  {
    return myPoint;
  }

  //! Returns the Support type:
  //!   IsVertex => The solution is a vertex.
  //!   IsOnEdge => The solution belongs to an Edge.
  //!   IsInFace => The solution is inside a Face.
  BRepExtrema_SupportType SupportKind() const
  {
    return mySupType;
  }

  //! Returns the vertex if the solution is a Vertex.
  const TopoDS_Vertex& Vertex() const
  {
    return myVertex;
  }

  //! Returns the vertex if the solution is an Edge.
  const TopoDS_Edge& Edge() const
  {
    return myEdge;
  }

  //! Returns the vertex if the solution is an Face.
  const TopoDS_Face& Face() const
  {
    return myFace;
  }

  //! Returns the parameter value if the solution is on Edge.
  void EdgeParameter (Standard_Real& theParam) const
  {
    theParam = myPar1;
  }

  //! Returns the parameters U and V if the solution is in a Face.
  void FaceParameter (Standard_Real& theU,
                      Standard_Real& theV) const
  {
    theU = myPar1;
    theV = myPar2;
  }

 private:

  Standard_Real myDist;
  gp_Pnt myPoint;
  BRepExtrema_SupportType mySupType;
  TopoDS_Vertex myVertex;
  TopoDS_Edge myEdge;
  TopoDS_Face myFace;
  Standard_Real myPar1;
  Standard_Real myPar2;

};

#endif
