// Created on: 1993-10-18
// Created by: Christophe MARION
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <HLRBRep_Curve.hxx>
#include <HLRBRep_EdgeFaceTool.hxx>
#include <HLRBRep_Surface.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : CurvatureDirection
//purpose  : 
//=======================================================================
Standard_Real HLRBRep_EdgeFaceTool::CurvatureValue
 (const Standard_Address F,
  const Standard_Real U,
  const Standard_Real V,
  const gp_Dir& Tg)
{
  gp_Pnt P;
  gp_Vec D1U,D1V,D2U,D2V,D2UV;
  ((HLRBRep_Surface*)F)->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
  Standard_Real d1ut   = D1U*Tg;
  Standard_Real d1vt   = D1V*Tg;
  Standard_Real d1ud1v = D1U*D1V;
  Standard_Real nmu2   = D1U*D1U;
  Standard_Real nmv2   = D1V*D1V;
  Standard_Real det = nmu2 * nmv2 - d1ud1v * d1ud1v;
  Standard_Real alfa = ( d1ut * nmv2 - d1vt * d1ud1v ) / det;
  Standard_Real beta = ( d1vt * nmu2 - d1ut * d1ud1v ) / det;
  gp_Vec Nm = D1U ^ D1V;
  if (Nm.Magnitude() > gp::Resolution()) {
    Nm.Normalize();
    Standard_Real alfa2 = alfa*alfa;
    Standard_Real beta2 = beta*beta;
    Standard_Real alfabeta = alfa*beta;
    Standard_Real N = (Nm*D2U)*alfa2  + 2*(Nm*D2UV)*alfabeta + (Nm*D2V)*beta2;
    Standard_Real D = nmu2    *alfa2  + 2*d1ud1v   *alfabeta + nmv2    *beta2;
    return N/D;
  }
  return 0.;
}

//=======================================================================
//function : UVPoint
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_EdgeFaceTool::UVPoint(const Standard_Real Par,
                                               const Standard_Address E,
                                               const Standard_Address F,
                                               Standard_Real& U,
                                               Standard_Real& V)
{
  Standard_Real pfbid,plbid;
  if (BRep_Tool::CurveOnSurface
      (((HLRBRep_Curve  *)E)->Curve().Edge(),
       ((HLRBRep_Surface*)F)->Surface().Face(),pfbid,plbid).IsNull())
  {
    BRepExtrema_ExtPF proj
      (BRepLib_MakeVertex(((HLRBRep_Curve*)E)->Value3D(Par)),
       ((HLRBRep_Surface*)F)->Surface().Face());
    Standard_Integer i, index = 0;
    Standard_Real dist2 = RealLast();
    const Standard_Integer n = proj.NbExt();
    for (i = 1; i <= n; i++) {
      const Standard_Real newdist2 = proj.SquareDistance(i);
      if (newdist2 < dist2) {
        dist2 = newdist2;
        index = i;
      }
    }
    if (index == 0)
      return Standard_False;

    proj.Parameter(index,U,V);
  }
  else {
    BRepAdaptor_Curve2d PC
      (((HLRBRep_Curve  *)E)->Curve().Edge(),
       ((HLRBRep_Surface*)F)->Surface().Face());
    gp_Pnt2d P2d;
    PC.D0(Par,P2d);
    U = P2d.X();
    V = P2d.Y();
  }
  return Standard_True;
}
