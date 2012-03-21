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


#ifndef _TopOpeBRepTool_SC_HeaderFile
#define _TopOpeBRepTool_SC_HeaderFile

#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>

#define MTLsc  TopOpeBRepTool_ShapeClassifier
#define MTLpsc TopOpeBRepTool_PShapeClassifier

Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(void);
Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(const TopoDS_Shape& S);
// ----------------------------------------------------------------------
//  state point <P> on/in shapes (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FSC_StatePonFace(const gp_Pnt& P,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);
Standard_EXPORT TopAbs_State FSC_StateEonFace(const TopoDS_Shape& E,const Standard_Real t,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);

#endif
