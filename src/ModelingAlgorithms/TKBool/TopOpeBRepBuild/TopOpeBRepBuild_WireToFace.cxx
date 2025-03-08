// Created on: 1996-01-29
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

#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_WireToFace.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>

//=================================================================================================

TopOpeBRepBuild_WireToFace::TopOpeBRepBuild_WireToFace() {}

//=================================================================================================

void TopOpeBRepBuild_WireToFace::Init()
{
  myLW.Clear();
}

//=================================================================================================

void TopOpeBRepBuild_WireToFace::AddWire(const TopoDS_Wire& W)
{
  myLW.Append(W);
}

//=================================================================================================

void TopOpeBRepBuild_WireToFace::MakeFaces(const TopoDS_Face& F, TopTools_ListOfShape& LF)
{
  LF.Clear();

  TopOpeBRepBuild_WireEdgeSet wes(F);
  for (TopTools_ListIteratorOfListOfShape it(myLW); it.More(); it.Next())
    wes.AddShape(it.Value());

  Standard_Boolean            ForceClass = Standard_True;
  TopOpeBRepBuild_FaceBuilder FB;
  FB.InitFaceBuilder(wes, F, ForceClass);

  TopOpeBRepDS_BuildTool  BT(TopOpeBRepTool_APPROX);
  TopOpeBRepBuild_Builder B(BT);
  B.MakeFaces(F, FB, LF);
}
