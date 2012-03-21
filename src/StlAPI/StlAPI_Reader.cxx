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

#include <StlAPI_Reader.ixx>
#include <RWStl.hxx>
#include <StlMesh_Mesh.hxx>
#include <OSD_Path.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <StlMesh_MeshExplorer.hxx>



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

