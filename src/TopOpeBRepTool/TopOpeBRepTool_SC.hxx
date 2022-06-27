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

#ifndef _TopOpeBRepTool_SC_HeaderFile
#define _TopOpeBRepTool_SC_HeaderFile

#include <TopOpeBRepTool_ShapeClassifier.hxx>

Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(void);
Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(const TopoDS_Shape& S);
// ----------------------------------------------------------------------
//  state point <P> on/in shapes (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FSC_StatePonFace(const gp_Pnt& P,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);
Standard_EXPORT TopAbs_State FSC_StateEonFace(const TopoDS_Shape& E,const Standard_Real t,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);

#endif
