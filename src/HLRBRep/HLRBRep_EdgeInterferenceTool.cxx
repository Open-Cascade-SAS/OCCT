// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef No_Exception
#define No_Exception
#endif


#include <gp_Dir.hxx>
#include <HLRAlgo_Interference.hxx>
#include <HLRAlgo_Intersection.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_EdgeInterferenceTool.hxx>

//=======================================================================
//function : HLRBRep_EdgeInterferenceTool
//purpose  : 
//=======================================================================
HLRBRep_EdgeInterferenceTool::HLRBRep_EdgeInterferenceTool
  (const Handle(HLRBRep_Data)& DS) : myDS(DS)
{
}

//=======================================================================
//function : LoadEdge
//purpose  : 
//=======================================================================

void HLRBRep_EdgeInterferenceTool::LoadEdge()
{
  Standard_Real p1,p2;
  Standard_ShortReal t1,t2;
  HLRBRep_Array1OfEData& ED = myDS->EDataArray();
  HLRBRep_EdgeData& ed = ED(myDS->Edge());
  ed.Status().Bounds(p1,t1,p2,t2);
  inter[0].Parameter(p1);
  inter[0].Tolerance(t1);
  inter[0].Index(ed.VSta());
  inter[1].Parameter(p2);
  inter[1].Tolerance(t2);
  inter[1].Index(ed.VEnd());
}

//=======================================================================
//function : EdgeGeometry
//purpose  : 
//=======================================================================

void HLRBRep_EdgeInterferenceTool::EdgeGeometry
  (const Standard_Real Param,
   gp_Dir& Tgt,
   gp_Dir& Nrm,
   Standard_Real& CrLE) const
{
  gp_Dir2d TgLE,NmLE;
  myDS->LocalLEGeometry2D(Param,TgLE,NmLE,CrLE);
  Tgt.SetCoord(TgLE.X(),TgLE.Y(),0);
  Nrm.SetCoord(NmLE.X(),NmLE.Y(),0);
}

//=======================================================================
//function : SameInterferences
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_EdgeInterferenceTool::SameInterferences
  (const HLRAlgo_Interference& I1,
   const HLRAlgo_Interference& I2) const
{
  Standard_Integer ind1 = I1.Intersection().Index();
  Standard_Integer ind2 = I2.Intersection().Index();
  if ( ind1 != 0 && ind2 != 0 ) return ind1 == ind2;
  return Standard_False;
}

//=======================================================================
//function : SameVertexAndInterference
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_EdgeInterferenceTool::SameVertexAndInterference
  (const HLRAlgo_Interference& I) const
{
  if (I.Intersection().Index() == inter[cur].Index())
    return Standard_True;
  return I.Intersection().Orientation() == 
    ((cur == 0) ? TopAbs_FORWARD : TopAbs_REVERSED);
}

//=======================================================================
//function : InterferenceBoundaryGeometry
//purpose  : 
//=======================================================================

void HLRBRep_EdgeInterferenceTool::InterferenceBoundaryGeometry
  (const HLRAlgo_Interference& I,
   gp_Dir& Tang,
   gp_Dir& Norm,
   Standard_Real& CrFE) const
{
  Standard_Integer FE;
  Standard_Real Param;
  gp_Dir2d TgFE,NmFE;
  I.Boundary().Value2D(FE,Param);
  myDS->LocalFEGeometry2D(FE,Param,TgFE,NmFE,CrFE);
  Tang.SetCoord(TgFE.X(),TgFE.Y(),0);
  Norm.SetCoord(NmFE.X(),NmFE.Y(),0);
}
