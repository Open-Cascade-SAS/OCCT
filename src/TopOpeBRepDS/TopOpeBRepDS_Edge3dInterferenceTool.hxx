// Created on: 1997-12-24
// Created by: Xuan PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Edge3dInterferenceTool_HeaderFile
#define _TopOpeBRepDS_Edge3dInterferenceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTrans_SurfaceTransition.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
class TopOpeBRepDS_Interference;


//! a tool computing edge / face complex transition,
//! Interferences of edge reference are given by
//! I = (T on face, G = point or vertex, S = edge)
class TopOpeBRepDS_Edge3dInterferenceTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_Edge3dInterferenceTool();
  
  Standard_EXPORT void InitPointVertex (const Standard_Integer IsVertex, const TopoDS_Shape& VonOO);
  
  Standard_EXPORT void Init (const TopoDS_Shape& Eref, const TopoDS_Shape& E, const TopoDS_Shape& F, const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT void Add (const TopoDS_Shape& Eref, const TopoDS_Shape& E, const TopoDS_Shape& F, const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT void Transition (const Handle(TopOpeBRepDS_Interference)& I) const;




protected:





private:



  Standard_Integer myFaceOriented;
  TopTrans_SurfaceTransition myTool;
  Standard_Real myTole;
  Standard_Boolean myrefdef;
  Standard_Integer myIsVertex;
  TopoDS_Shape myVonOO;
  gp_Pnt myP3d;
  gp_Dir myTgtref;


};







#endif // _TopOpeBRepDS_Edge3dInterferenceTool_HeaderFile
