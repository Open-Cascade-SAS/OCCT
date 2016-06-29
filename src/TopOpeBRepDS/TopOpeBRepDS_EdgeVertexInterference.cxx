// Created on: 1994-10-28
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Standard_Type.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_Transition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_EdgeVertexInterference,TopOpeBRepDS_ShapeShapeInterference)

//=======================================================================
//function : TopOpeBRepDS_EdgeVertexInterference
//purpose  : 
//=======================================================================
TopOpeBRepDS_EdgeVertexInterference::TopOpeBRepDS_EdgeVertexInterference
  (const TopOpeBRepDS_Transition& T, 
   const TopOpeBRepDS_Kind        ST,
   const Standard_Integer         S, 
   const Standard_Integer         G,
   const Standard_Boolean         GIsBound, 
   const TopOpeBRepDS_Config      C,
   const Standard_Real            P) :
  TopOpeBRepDS_ShapeShapeInterference
  (T,ST,S,TopOpeBRepDS_VERTEX,G,GIsBound,C),
  myParam(P)
{
}

//=======================================================================
//function : TopOpeBRepDS_EdgeVertexInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_EdgeVertexInterference::TopOpeBRepDS_EdgeVertexInterference
  (const TopOpeBRepDS_Transition& T, 
   const Standard_Integer         S, 
   const Standard_Integer         G,
   const Standard_Boolean         GIsBound, 
   const TopOpeBRepDS_Config      C,
   const Standard_Real            P) :
  TopOpeBRepDS_ShapeShapeInterference
  (T,TopOpeBRepDS_EDGE,S,TopOpeBRepDS_VERTEX,G,GIsBound,C),
  myParam(P)
{
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_EdgeVertexInterference::Parameter()const 
{
  return myParam;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_EdgeVertexInterference::Parameter(const Standard_Real P)
{
  myParam = P;
}
