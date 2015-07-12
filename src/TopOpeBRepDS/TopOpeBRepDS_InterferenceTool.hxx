// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_InterferenceTool_HeaderFile
#define _TopOpeBRepDS_InterferenceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Config.hxx>
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_Transition;
class Geom2d_Curve;



class TopOpeBRepDS_InterferenceTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeEdgeInterference (const TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Kind SK, const Standard_Integer SI, const TopOpeBRepDS_Kind GK, const Standard_Integer GI, const Standard_Real P);
  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeCurveInterference (const TopOpeBRepDS_Transition& T, const TopOpeBRepDS_Kind SK, const Standard_Integer SI, const TopOpeBRepDS_Kind GK, const Standard_Integer GI, const Standard_Real P);
  
  //! duplicate I in a new interference with Complement() transition.
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) DuplicateCurvePointInterference (const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeFaceCurveInterference (const TopOpeBRepDS_Transition& Transition, const Standard_Integer FaceI, const Standard_Integer CurveI, const Handle(Geom2d_Curve)& PC);
  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeSolidSurfaceInterference (const TopOpeBRepDS_Transition& Transition, const Standard_Integer SolidI, const Standard_Integer SurfaceI);
  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeEdgeVertexInterference (const TopOpeBRepDS_Transition& Transition, const Standard_Integer EdgeI, const Standard_Integer VertexI, const Standard_Boolean VertexIsBound, const TopOpeBRepDS_Config Config, const Standard_Real param);
  
  Standard_EXPORT static Handle(TopOpeBRepDS_Interference) MakeFaceEdgeInterference (const TopOpeBRepDS_Transition& Transition, const Standard_Integer FaceI, const Standard_Integer EdgeI, const Standard_Boolean EdgeIsBound, const TopOpeBRepDS_Config Config);
  
  Standard_EXPORT static Standard_Real Parameter (const Handle(TopOpeBRepDS_Interference)& CPI);
  
  Standard_EXPORT static void Parameter (const Handle(TopOpeBRepDS_Interference)& CPI, const Standard_Real Par);




protected:





private:





};







#endif // _TopOpeBRepDS_InterferenceTool_HeaderFile
