// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <TopOpeBRepBuild_BuilderON.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>

//Standard_IMPORT extern Standard_Boolean GLOBAL_faces2d;
extern Standard_Boolean GLOBAL_faces2d;

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
