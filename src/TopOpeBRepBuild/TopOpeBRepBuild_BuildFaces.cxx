// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepBuild_Builder.jxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_define.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#ifdef DEB
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceCU();
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceCUV();
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceSPF();
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceSPS();
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GetcontextSF2();
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceSHEX();
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
#ifdef DEB
  Standard_Boolean tSE = TopOpeBRepBuild_GettraceSPF();
  Standard_Boolean tCU = TopOpeBRepBuild_GettraceCU();
  Standard_Boolean NtCUV = !TopOpeBRepBuild_GettraceCUV();
#endif
  //
  TopOpeBRepDS_CurveIterator SCurves(HDS->SurfaceCurves(iS)); 
  for (;  SCurves.More(); SCurves.Next()) {
    Standard_Integer iC = SCurves.Current();
    const TopOpeBRepDS_Curve& CDS = HDS->Curve(iC);
#ifdef DEB
    if (tSE) cout<<endl<<"BuildFaces : C "<<iC<<" on S "<<iS<<endl;
    if (tCU) { CDS.Dump(cout,iC,NtCUV); cout<<endl; }
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
