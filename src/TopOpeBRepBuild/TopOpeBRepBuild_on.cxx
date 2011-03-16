// file:	TopOpeBRepBuild_on.cxx
// Created:	Thu Mar  7 10:49:33 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepBuild_Builder.ixx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_BuilderON.hxx>

//Standard_IMPORT extern Standard_Boolean GLOBAL_faces2d;
Standard_EXPORTEXTERN Standard_Boolean GLOBAL_faces2d;

//=======================================================================
//function : GFillONPartsWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillONPartsWES(const TopoDS_Shape& FOR,const TopOpeBRepBuild_GTopo& G,const TopTools_ListOfShape& LSclass,TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopOpeBRepBuild_BuilderON BON;
  if (GLOBAL_faces2d)
  BON.Perform2d(this,FOR,(TopOpeBRepBuild_PGTopo)&G,(TopOpeBRepTool_Plos)&LSclass,(TopOpeBRepBuild_PWireEdgeSet)&WES);
  else 
  BON.Perform(this,FOR,(TopOpeBRepBuild_PGTopo)&G,(TopOpeBRepTool_Plos)&LSclass,(TopOpeBRepBuild_PWireEdgeSet)&WES);
}
