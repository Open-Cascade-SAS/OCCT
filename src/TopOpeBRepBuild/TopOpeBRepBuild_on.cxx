// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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
