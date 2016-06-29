// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceCU();
extern Standard_Boolean TopOpeBRepBuild_GettraceCUV();
extern Standard_Boolean TopOpeBRepBuild_GettraceSPF();
extern Standard_Boolean TopOpeBRepBuild_GettraceSPS();
extern Standard_Boolean TopOpeBRepBuild_GetcontextSF2();
extern Standard_Boolean TopOpeBRepBuild_GettraceSHEX();
#endif

//=======================================================================
//function : BuildFaces
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::BuildFaces(const Standard_Integer iS,
					 const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Standard_Real aTBSTol, aTBCTol;
  BRep_Builder aBB;
  TopoDS_Shape aFace;
  //
  //modified by NIZNHY-PKV Mon Dec 13 10:00:23 2010f
  const TopOpeBRepDS_Surface& aTBS=HDS->Surface(iS);
  aTBSTol=aTBS.Tolerance();
  //
  myBuildTool.MakeFace(aFace, aTBS);
  //
  //myBuildTool.MakeFace(aFace,HDS->Surface(iS));
  //modified by NIZNHY-PKV Mon Dec 13 10:01:03 2010t
  //
  TopOpeBRepBuild_WireEdgeSet WES(aFace, this);
  //
#ifdef OCCT_DEBUG
  Standard_Boolean tSE = TopOpeBRepBuild_GettraceSPF();
#endif
  //
  TopOpeBRepDS_CurveIterator SCurves(HDS->SurfaceCurves(iS)); 
  for (;  SCurves.More(); SCurves.Next()) {
    Standard_Integer iC = SCurves.Current();
    const TopOpeBRepDS_Curve& CDS = HDS->Curve(iC);
#ifdef OCCT_DEBUG
    if (tSE) cout<<endl<<"BuildFaces : C "<<iC<<" on S "<<iS<<endl;
#endif
    TopoDS_Shape anEdge;
    TopTools_ListIteratorOfListOfShape Iti(NewEdges(iC)); 
    for (;  Iti.More();  Iti.Next()) {
      anEdge = Iti.Value();
      //modified by NIZNHY-PKV Mon Dec 13 10:09:38 2010f
      TopoDS_Edge& aE=*((TopoDS_Edge*)&anEdge);
      aTBCTol=BRep_Tool::Tolerance(aE);
      if (aTBCTol < aTBSTol) {
	aBB.UpdateEdge(aE, aTBSTol);
      }
      //modified by NIZNHY-PKV Mon Dec 13 10:09:43 2010f
      TopAbs_Orientation ori = SCurves.Orientation(TopAbs_IN);
      myBuildTool.Orientation(anEdge,ori);
      const Handle(Geom2d_Curve)& PC = SCurves.PCurve();
      myBuildTool.PCurve(aFace,anEdge,CDS,PC);
      WES.AddStartElement(anEdge);
    }
  }  
  //
  TopOpeBRepBuild_FaceBuilder FABU(WES,aFace);  
  TopTools_ListOfShape& FaceList = ChangeNewFaces(iS);
  MakeFaces(aFace,FABU,FaceList);
}

//=======================================================================
//function : BuildFaces
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::BuildFaces(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Standard_Integer iS, n = HDS->NbSurfaces();
  myNewFaces = new TopTools_HArray1OfListOfShape(0,n);
  for (iS = 1; iS <= n; iS++) BuildFaces(iS,HDS);
}
