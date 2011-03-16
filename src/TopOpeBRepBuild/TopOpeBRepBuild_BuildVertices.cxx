// File:	TopOpeBRepBuild_BuildVertices.hxx
// Created:	Mon Jun 14 11:48:36 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

#include <TopOpeBRepBuild_Builder.jxx>
#include <TopOpeBRepDS_Point.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : BuildVertices
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::BuildVertices(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Standard_Integer iP, n = HDS->NbPoints();
  
  myNewVertices = new TopTools_HArray1OfShape(0, n);

  for (iP = 1; iP <= n; iP++) {
    const TopOpeBRepDS_Point& aTBSPoint=HDS->Point(iP);
    myBuildTool.MakeVertex(ChangeNewVertex(iP), aTBSPoint);
  }
}
