// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _MeshTest_DrawableMesh_HeaderFile
#define _MeshTest_DrawableMesh_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>

class TopoDS_Shape;
class Draw_Display;

//! A drawable mesh. 
//! Provides a mesh object inherited from Drawable3d to draw a triangulation.
//! It contains a sequence of highlighted edges and highlighted vertices. <br>
class MeshTest_DrawableMesh : public Draw_Drawable3D
{
public:
  
  Standard_EXPORT MeshTest_DrawableMesh();
  
  Standard_EXPORT MeshTest_DrawableMesh(const TopoDS_Shape& theShape,
                                        const Standard_Real theDeflection);
  
  Standard_EXPORT MeshTest_DrawableMesh(const Handle(BRepMesh_IncrementalMesh)& theMesher);
  
  Standard_EXPORT void Add(const TopoDS_Shape& theShape);
  
  Standard_EXPORT TColStd_SequenceOfInteger& Edges();
  
  Standard_EXPORT TColStd_SequenceOfInteger& Vertices();
  
  Standard_EXPORT TColStd_SequenceOfInteger& Triangles();
  
  Standard_EXPORT void DrawOn(Draw_Display& theDisplay) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Dump(Standard_OStream& theStream) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& theDi) const Standard_OVERRIDE;
  
  Standard_EXPORT const Handle(BRepMesh_IncrementalMesh)& Mesher() const;

  DEFINE_STANDARD_RTTIEXT(MeshTest_DrawableMesh,Draw_Drawable3D)

private: 

  Handle(BRepMesh_IncrementalMesh) myMesher;
  Standard_Real                    myDeflection;
  TColStd_SequenceOfInteger        myEdges;
  TColStd_SequenceOfInteger        myVertices;
  TColStd_SequenceOfInteger        myTriangles;
};

DEFINE_STANDARD_HANDLE(MeshTest_DrawableMesh, Draw_Drawable3D)

#endif
