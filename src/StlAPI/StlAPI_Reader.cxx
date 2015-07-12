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


#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <StlAPI_Reader.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshExplorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

StlAPI_Reader::StlAPI_Reader() {}

void StlAPI_Reader::Read(TopoDS_Shape& aShape, const Standard_CString aFileName) 
{
  OSD_Path aFile(aFileName);
  
  Handle(StlMesh_Mesh) aSTLMesh = RWStl::ReadFile(aFile);
  Standard_Integer NumberDomains = aSTLMesh->NbDomains();
  Standard_Integer iND;
  gp_XYZ p1, p2, p3;
  TopoDS_Vertex Vertex1, Vertex2, Vertex3;
  TopoDS_Face AktFace;
  TopoDS_Wire AktWire;
  BRepBuilderAPI_Sewing aSewingTool;
  Standard_Real x1, y1, z1;
  Standard_Real x2, y2, z2;
  Standard_Real x3, y3, z3;
  
  aSewingTool.Init(1.0e-06,Standard_True);
  
  TopoDS_Compound aComp;
  BRep_Builder BuildTool;
  BuildTool.MakeCompound( aComp );

  StlMesh_MeshExplorer aMExp (aSTLMesh);
  
  for (iND=1;iND<=NumberDomains;iND++) 
  {
    for (aMExp.InitTriangle (iND); aMExp.MoreTriangle (); aMExp.NextTriangle ()) 
    {
      aMExp.TriangleVertices (x1,y1,z1,x2,y2,z2,x3,y3,z3);
      p1.SetCoord(x1,y1,z1);
      p2.SetCoord(x2,y2,z2);
      p3.SetCoord(x3,y3,z3);
      
      if ((!(p1.IsEqual(p2,0.0))) && (!(p1.IsEqual(p3,0.0))))
      {
        Vertex1 = BRepBuilderAPI_MakeVertex(p1);
        Vertex2 = BRepBuilderAPI_MakeVertex(p2);
        Vertex3 = BRepBuilderAPI_MakeVertex(p3);
        
        AktWire = BRepBuilderAPI_MakePolygon( Vertex1, Vertex2, Vertex3, Standard_True);
        
        if( !AktWire.IsNull())
        {
          AktFace = BRepBuilderAPI_MakeFace( AktWire);
          if(!AktFace.IsNull())
            BuildTool.Add( aComp, AktFace );
        }
      }
    }
  }
  aSTLMesh->Clear();

  aSewingTool.Load( aComp );
  aSewingTool.Perform();
  aShape = aSewingTool.SewedShape();
  if ( aShape.IsNull() )
    aShape = aComp;
}

