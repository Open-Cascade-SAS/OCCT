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


#include <Vrml_ShapeHints.hxx>

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
  anOStream  << "ShapeHints {\n";

  switch ( myVertexOrdering )
    {
     case Vrml_UNKNOWN_ORDERING: break; // anOStream  << "    vertexOrdering\tUNKNOWN_ORDERING";
     case Vrml_CLOCKWISE:        anOStream  << "    vertexOrdering\tCLOCKWISE\n"; break;
     case Vrml_COUNTERCLOCKWISE: anOStream  << "    vertexOrdering\tCOUNTERCLOCKWISE\n"; break; 
    }

  switch ( myShapeType )
    {
     case Vrml_UNKNOWN_SHAPE_TYPE: break; //anOStream  << "    shapeType\t\tUNKNOWN_SHAPE_TYPE";
     case Vrml_SOLID:              anOStream  << "    shapeType\t\tSOLID\n"; break;
    }

  switch ( myFaceType )
    {
     case Vrml_UNKNOWN_FACE_TYPE: anOStream  << "    faceType\t\tUNKNOWN_FACE_TYPE\n"; break;
     case Vrml_CONVEX:            break; //anOStream  << "    faceType\t\tCONVEX";
    }

  if ( Abs(myAngle - 0.5) > 0.0001 )
    { 
      anOStream  << "    creaseAngle\t\t" << myAngle << "\n";
    } 
  anOStream  << "}\n";
  return anOStream;
}

