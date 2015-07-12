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


#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_BuilderON.hxx>
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
