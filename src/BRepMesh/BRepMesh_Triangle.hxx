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

class BRepMesh_Triangle  {
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepMesh_Triangle();
  
  Standard_EXPORT BRepMesh_Triangle(const Standard_Integer          theEdge1,
                                    const Standard_Integer          theEdge2,
                                    const Standard_Integer          theEdge3,
                                    const Standard_Boolean          theOrientation1,
                                    const Standard_Boolean          theOrientation2,
                                    const Standard_Boolean          theOrientation3,
                                    const BRepMesh_DegreeOfFreedom  isCanMove);
  
  Standard_EXPORT void Initialize(const Standard_Integer          theEdge1,
                                  const Standard_Integer          theEdge2,
                                  const Standard_Integer          theEdge3,
                                  const Standard_Boolean          theOrientation1,
                                  const Standard_Boolean          theOrientation2,
                                  const Standard_Boolean          theOrientation3,
                                  const BRepMesh_DegreeOfFreedom  isCanMove) ;
  
  Standard_EXPORT void Edges(Standard_Integer& theEdge1,
                             Standard_Integer& theEdge2,
                             Standard_Integer& theEdge3,
                             Standard_Boolean& theOrientation1,
                             Standard_Boolean& theOrientation2,
                             Standard_Boolean& theOrientation3) const;
  
  inline BRepMesh_DegreeOfFreedom Movability() const 
  {
    return myMovability;
  }
  
  Standard_EXPORT void SetMovability(const BRepMesh_DegreeOfFreedom theMovability) ;
  
  Standard_EXPORT Standard_Integer HashCode(const Standard_Integer theUpper) const;
  
  Standard_EXPORT Standard_Boolean IsEqual(const BRepMesh_Triangle& theOther) const;
  
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

// Inline functions
inline Standard_Integer HashCode(const BRepMesh_Triangle& theTriangle,
                                 const Standard_Integer theUpper)
{
 return theTriangle.HashCode(theUpper);
}

#endif
