// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_PaveClassifier.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>

//=================================================================================================

TopOpeBRepBuild_EdgeBuilder::TopOpeBRepBuild_EdgeBuilder() {}

//=================================================================================================

TopOpeBRepBuild_EdgeBuilder::TopOpeBRepBuild_EdgeBuilder(TopOpeBRepBuild_PaveSet&        LS,
                                                         TopOpeBRepBuild_PaveClassifier& LC,
                                                         const Standard_Boolean          ForceClass)
{
  InitEdgeBuilder(LS, LC, ForceClass);
}

//=================================================================================================

void TopOpeBRepBuild_EdgeBuilder::InitEdgeBuilder(TopOpeBRepBuild_LoopSet&        LS,
                                                  TopOpeBRepBuild_LoopClassifier& LC,
                                                  const Standard_Boolean          ForceClass)
{
  InitAreaBuilder(LS, LC, ForceClass);
}

//=================================================================================================

void TopOpeBRepBuild_EdgeBuilder::InitEdge()
{
  InitArea();
}

//=================================================================================================

Standard_Boolean TopOpeBRepBuild_EdgeBuilder::MoreEdge() const
{
  Standard_Boolean b = MoreArea();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_EdgeBuilder::NextEdge()
{
  NextArea();
}

//=================================================================================================

void TopOpeBRepBuild_EdgeBuilder::InitVertex()
{
  InitLoop();
}

//=================================================================================================

Standard_Boolean TopOpeBRepBuild_EdgeBuilder::MoreVertex() const
{
  Standard_Boolean b = MoreLoop();
  return b;
}

//=================================================================================================

void TopOpeBRepBuild_EdgeBuilder::NextVertex()
{
  NextLoop();
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_EdgeBuilder::Vertex() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = Loop();
  Handle(TopOpeBRepBuild_Pave)        PV(Handle(TopOpeBRepBuild_Pave)::DownCast(L));
  const TopoDS_Shape&                 V = PV->Vertex();
  return V;
}

//=================================================================================================

Standard_Real TopOpeBRepBuild_EdgeBuilder::Parameter() const
{
  const Handle(TopOpeBRepBuild_Loop)& L = Loop();
  Handle(TopOpeBRepBuild_Pave)        PV(Handle(TopOpeBRepBuild_Pave)::DownCast(L));
  Standard_Real                       parV = PV->Parameter();
  return parV;
}
