// Created on: 1993-09-23
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
