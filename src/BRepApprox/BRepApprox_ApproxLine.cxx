// Created on: 1995-07-20
// Created by: Modelistation
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


#include <BRepApprox_ApproxLine.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepApprox_ApproxLine,Standard_Transient)

//=======================================================================
//function : BRepApprox_ApproxLine
//purpose  : 
//=======================================================================
BRepApprox_ApproxLine::BRepApprox_ApproxLine
   (const Handle(Geom_BSplineCurve)&    CurveXYZ,
    const Handle(Geom2d_BSplineCurve)&  CurveUV1,
    const Handle(Geom2d_BSplineCurve)&  CurveUV2) 
{ 
  myCurveXYZ = CurveXYZ;
  myCurveUV1 = CurveUV1;
  myCurveUV2 = CurveUV2;
}


//=======================================================================
//function : BRepApprox_ApproxLine
//purpose  : 
//=======================================================================

BRepApprox_ApproxLine::BRepApprox_ApproxLine
    (const Handle(IntSurf_LineOn2S)& lin,
     const Standard_Boolean ) 
     :myLineOn2S(lin) 
{ 
}

//=======================================================================
//function : NbPnts
//purpose  : 
//=======================================================================

Standard_Integer BRepApprox_ApproxLine::NbPnts() const 
{
  if(!myCurveXYZ.IsNull())
    return(myCurveXYZ->NbPoles());
  if(!myCurveUV1.IsNull())
    return(myCurveUV1->NbPoles());
  if(!myCurveUV2.IsNull())
    return(myCurveUV2->NbPoles());
  return(myLineOn2S->NbPoints());
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

IntSurf_PntOn2S BRepApprox_ApproxLine::Point(const Standard_Integer Index)
{
  if(!myLineOn2S.IsNull()) { 
    if(myLineOn2S->NbPoints()) { 
      return(myLineOn2S->Value(Index));
    }
  }
  gp_Pnt2d P1,P2;
  gp_Pnt   P;
  if(!myCurveXYZ.IsNull()) 
    P = myCurveXYZ->Pole(Index);
  if(!myCurveUV1.IsNull())
    P1 = myCurveUV1->Pole(Index);
  if(!myCurveUV2.IsNull())
    P2 = myCurveUV2->Pole(Index);

  IntSurf_PntOn2S aPntOn2S;
  aPntOn2S.SetValue(P, P1.X(), P1.Y(), P2.X(), P2.Y());

  return aPntOn2S;
}
