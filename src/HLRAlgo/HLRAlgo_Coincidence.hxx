// Created on: 1992-08-20
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRAlgo_Coincidence_HeaderFile
#define _HLRAlgo_Coincidence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_State.hxx>

//! The Coincidence class is used in an Inteference to
//! store informations on the "hiding" edge.
//!
//! 2D  Data : The  tangent  and the  curvature of the
//! projection of the edge  at the intersection point.
//! This is necesserary  when the intersection  is  at
//! the extremity of the edge.
//!
//! 3D   Data  :  The   state of  the   edge  near the
//! intersection   with  the face (before  and after).
//! This is necessary  when the  intersection is  "ON"
//! the face.
class HLRAlgo_Coincidence
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRAlgo_Coincidence() :
    myFE(0),
    myParam(0.),
    myStBef(TopAbs_IN),
    myStAft(TopAbs_IN)
  {
  }

  void Set2D (const Standard_Integer FE, const Standard_Real Param)
  {
    myFE    = FE;
    myParam = Param;
  }

  void SetState3D (const TopAbs_State stbef, const TopAbs_State staft)
  {
    myStBef = stbef;
    myStAft = staft;
  }

  void Value2D (Standard_Integer& FE, Standard_Real& Param) const
  {
    FE    = myFE;
    Param = myParam;
  }

  void State3D (TopAbs_State& stbef, TopAbs_State& staft) const
  {
    stbef = myStBef;
    staft = myStAft;
  }

private:
  Standard_Integer myFE;
  Standard_Real myParam;
  TopAbs_State myStBef;
  TopAbs_State myStAft;
};

#endif // _HLRAlgo_Coincidence_HeaderFile
