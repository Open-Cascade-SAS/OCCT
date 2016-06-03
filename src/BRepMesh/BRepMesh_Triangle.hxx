// Created on: 1993-09-23
// Created by: Didier PIFFAULT
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

#ifndef _BRepMesh_Triangle_HeaderFile
#define _BRepMesh_Triangle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <BRepMesh_DegreeOfFreedom.hxx>


//! Light weighted structure representing triangle 
//! of mesh consisting of oriented links.
class BRepMesh_Triangle
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  BRepMesh_Triangle()
  : myEdge1(0),
    myEdge2(0),
    myEdge3(0),
    myOrientation1(Standard_False),
    myOrientation2(Standard_False),
    myOrientation3(Standard_False),
    myMovability  (BRepMesh_Free)
  {
  }

  //! Constructor.
  //! @param theEdges array of edges of triangle.
  //! @param theOrientations array of edge's orientations.
  //! @param theMovability movability of triangle.
  BRepMesh_Triangle(
    const Standard_Integer          (&theEdges)[3],
    const Standard_Boolean          (&theOrientations)[3],
    const BRepMesh_DegreeOfFreedom  theMovability)
  {
    Initialize(theEdges, theOrientations, theMovability);
  }
  
  //! Initializes the triangle by the given parameters.
  //! @param theEdges array of edges of triangle.
  //! @param theOrientations array of edge's orientations.
  //! @param theMovability movability of triangle.
  inline void Initialize(
    const Standard_Integer          (&theEdges)[3],
    const Standard_Boolean          (&theOrientations)[3],
    const BRepMesh_DegreeOfFreedom  theMovability)
  {
    myEdge1        = theEdges[0];
    myEdge2        = theEdges[1];
    myEdge3        = theEdges[2];
    myOrientation1 = theOrientations[0];
    myOrientation2 = theOrientations[1];
    myOrientation3 = theOrientations[2];
    myMovability   = theMovability;
  }
  
  //! Gets edges with orientations composing the triangle.
  //! @param[out] theEdges array edges are stored to.
  //! @param[out] theOrientations array orientations are stored to.
  inline void Edges(Standard_Integer (&theEdges)[3],
                    Standard_Boolean (&theOrientations)[3]) const
  {
    theEdges[0]        = myEdge1;
    theEdges[1]        = myEdge2;
    theEdges[2]        = myEdge3;
    theOrientations[0] = myOrientation1;
    theOrientations[1] = myOrientation2;
    theOrientations[2] = myOrientation3;
  }
  
  //! Returns movability of the triangle.
  inline BRepMesh_DegreeOfFreedom Movability() const 
  {
    return myMovability;
  }
  
  //! Sets movability of the triangle.
  inline void SetMovability(const BRepMesh_DegreeOfFreedom theMovability)
  {
    myMovability = theMovability;
  }
  
  //! Returns hash code for this triangle.
  //! @param theUpper upper index in the container.
  //! @return hash code.
  Standard_Integer HashCode(const Standard_Integer theUpper) const
  {
    return ::HashCode(myEdge1 + myEdge2 + myEdge3, theUpper);
  }
  
  //! Checks for equality with another triangle.
  //! @param theOther triangle to be checked against this one.
  //! @return TRUE if equal, FALSE if not.
  Standard_Boolean IsEqual(const BRepMesh_Triangle& theOther) const
  {
    if (myMovability == BRepMesh_Deleted || theOther.myMovability == BRepMesh_Deleted)
      return Standard_False;

    if (myEdge1 == theOther.myEdge1 && 
        myEdge2 == theOther.myEdge2 && 
        myEdge3 == theOther.myEdge3)
    {
      return Standard_True;
    }

    if (myEdge1 == theOther.myEdge2 && 
        myEdge2 == theOther.myEdge3 && 
        myEdge3 == theOther.myEdge1)
    {
      return Standard_True;
    }

    if (myEdge1 == theOther.myEdge3 && 
        myEdge2 == theOther.myEdge1 && 
        myEdge3 == theOther.myEdge2)
    {
      return Standard_True;
    }

    return Standard_False;
  }
  
  //! Alias for IsEqual.
  Standard_Boolean operator ==(const BRepMesh_Triangle& theOther) const
  {
    return IsEqual(theOther);
  }

private:

  Standard_Integer          myEdge1;
  Standard_Integer          myEdge2;
  Standard_Integer          myEdge3;
  Standard_Boolean          myOrientation1;
  Standard_Boolean          myOrientation2;
  Standard_Boolean          myOrientation3;
  BRepMesh_DegreeOfFreedom  myMovability;
};

inline Standard_Integer HashCode(const BRepMesh_Triangle& theTriangle,
                                 const Standard_Integer   theUpper)
{
 return theTriangle.HashCode(theUpper);
}

#endif
