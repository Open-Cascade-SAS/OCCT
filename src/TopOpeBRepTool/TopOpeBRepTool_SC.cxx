// Created on: 1998-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>

// ----------------------------------------------------------------------
static TopOpeBRepTool_PShapeClassifier TopOpeBRepTool_PSC = NULL;
Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(void)
{
  if (TopOpeBRepTool_PSC == NULL) TopOpeBRepTool_PSC = new TopOpeBRepTool_ShapeClassifier();
  return *TopOpeBRepTool_PSC;
}
// ----------------------------------------------------------------------
Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(const TopoDS_Shape& S)
{ 
  if (TopOpeBRepTool_PSC == NULL) TopOpeBRepTool_PSC = new TopOpeBRepTool_ShapeClassifier();
  TopOpeBRepTool_PSC->SetReference(S);
  return *TopOpeBRepTool_PSC;
}

// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FSC_StatePonFace(const gp_Pnt& P,const TopoDS_Shape& F,
				      TopOpeBRepTool_ShapeClassifier& PSC)
{
  // Projects <P> on the surface and classifies it in the face <F>
  Handle(Geom_Surface) S = BRep_Tool::Surface(TopoDS::Face(F));

  gp_Pnt2d UV; Standard_Real dist; Standard_Boolean ok = FUN_tool_projPonS(P,S,UV,dist);
  if (!ok) return TopAbs_UNKNOWN;

  PSC.SetReference(TopoDS::Face(F));
  PSC.StateP2DReference(UV);
  TopAbs_State state = PSC.State();
  
  return state;
}

// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FSC_StateEonFace(const TopoDS_Shape& E,const Standard_Real t,const TopoDS_Shape& F,
				      TopOpeBRepTool_ShapeClassifier& PSC)
{
  BRepAdaptor_Curve BAC(TopoDS::Edge(E));
  Standard_Real f,l; FUN_tool_bounds(TopoDS::Edge(E),f,l);
  Standard_Real par = (1-t)*f + t*l;
  gp_Pnt P;BAC.D0(par,P);
  TopAbs_State state = FSC_StatePonFace(P,F,PSC);  
  return state;
}
