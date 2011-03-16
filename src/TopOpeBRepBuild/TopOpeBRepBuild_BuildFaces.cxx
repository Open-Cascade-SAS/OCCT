// File:	TopOpeBRepBuild_Builder.hxx
// Created:	Mon Jun 14 11:48:36 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

#include <TopOpeBRepBuild_Builder.jxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_define.hxx>

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
  TopoDS_Shape aFace;
  myBuildTool.MakeFace(aFace,HDS->Surface(iS));
  TopOpeBRepBuild_WireEdgeSet WES(aFace,this);
#ifdef DEB
  Standard_Boolean tSE = TopOpeBRepBuild_GettraceSPF();
  Standard_Boolean tCU = TopOpeBRepBuild_GettraceCU();
  Standard_Boolean NtCUV = !TopOpeBRepBuild_GettraceCUV();
#endif
  for (TopOpeBRepDS_CurveIterator SCurves(HDS->SurfaceCurves(iS)); 
       SCurves.More(); SCurves.Next()) {
    Standard_Integer iC = SCurves.Current();
    const TopOpeBRepDS_Curve& CDS = HDS->Curve(iC);
#ifdef DEB
    if (tSE) cout<<endl<<"BuildFaces : C "<<iC<<" on S "<<iS<<endl;
    if (tCU) { CDS.Dump(cout,iC,NtCUV); cout<<endl; }
#endif
    TopoDS_Shape anEdge;
    for (TopTools_ListIteratorOfListOfShape Iti(NewEdges(iC)); 
	 Iti.More(); 
	 Iti.Next()) {
      anEdge = Iti.Value();
      TopAbs_Orientation ori = SCurves.Orientation(TopAbs_IN);
      myBuildTool.Orientation(anEdge,ori);
      const Handle(Geom2d_Curve)& PC = SCurves.PCurve();
      myBuildTool.PCurve(aFace,anEdge,CDS,PC);
      WES.AddStartElement(anEdge);
    }
  }  
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
