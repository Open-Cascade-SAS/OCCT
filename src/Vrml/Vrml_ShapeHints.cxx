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

#include <Vrml_ShapeHints.ixx>

Vrml_ShapeHints::Vrml_ShapeHints(const Vrml_VertexOrdering aVertexOrdering, 
				  const Vrml_ShapeType aShapeType, 
				  const Vrml_FaceType aFaceType, 
				  const Standard_Real aAngle)
{
 myVertexOrdering = aVertexOrdering;
 myShapeType = aShapeType;
 myFaceType = aFaceType;
 myAngle = aAngle;
}

void Vrml_ShapeHints::SetVertexOrdering(const Vrml_VertexOrdering aVertexOrdering)
{
 myVertexOrdering = aVertexOrdering;
}

Vrml_VertexOrdering Vrml_ShapeHints::VertexOrdering() const 
{
 return myVertexOrdering;
}

void Vrml_ShapeHints::SetShapeType(const Vrml_ShapeType aShapeType)
{
 myShapeType = aShapeType;
}

Vrml_ShapeType Vrml_ShapeHints::ShapeType() const 
{
 return myShapeType;
}

void Vrml_ShapeHints::SetFaceType(const Vrml_FaceType aFaceType)
{
 myFaceType = aFaceType;
}

Vrml_FaceType Vrml_ShapeHints::FaceType() const 
{
 return myFaceType;
}

void Vrml_ShapeHints::SetAngle(const Standard_Real aAngle)
{
 myAngle = aAngle;
}

Standard_Real Vrml_ShapeHints::Angle() const 
{
 return myAngle;
}

Standard_OStream& Vrml_ShapeHints::Print(Standard_OStream& anOStream) const 
{
  anOStream  << "ShapeHints {" << endl;

  switch ( myVertexOrdering )
    {
     case Vrml_UNKNOWN_ORDERING: break; // anOStream  << "    vertexOrdering" << "\tUNKNOWN_ORDERING";
     case Vrml_CLOCKWISE:        anOStream  << "    vertexOrdering" << "\tCLOCKWISE" << endl; break;
     case Vrml_COUNTERCLOCKWISE: anOStream  << "    vertexOrdering" << "\tCOUNTERCLOCKWISE" << endl; break; 
    }

  switch ( myShapeType )
    {
     case Vrml_UNKNOWN_SHAPE_TYPE: break; //anOStream  << "    shapeType" << "\t\tUNKNOWN_SHAPE_TYPE";
     case Vrml_SOLID:              anOStream  << "    shapeType" << "\t\tSOLID" << endl; break;
    }

  switch ( myFaceType )
    {
     case Vrml_UNKNOWN_FACE_TYPE: anOStream  << "    faceType" << "\t\tUNKNOWN_FACE_TYPE" << endl; break;
     case Vrml_CONVEX:            break; //anOStream  << "    faceType" << "\t\tCONVEX";
    }

  if ( Abs(myAngle - 0.5) > 0.0001 )
    { 
      anOStream  << "    creaseAngle\t" << '\t' << myAngle << endl;
    } 
  anOStream  << '}' << endl;
  return anOStream;
}

