// Created on: 1998-04-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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
