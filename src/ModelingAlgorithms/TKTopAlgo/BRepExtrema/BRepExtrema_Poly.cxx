// Created on: 1995-09-08
// Created by: Christophe MARION
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

#include <BRepExtrema_Poly.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <Poly_Triangulation.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

bool BRepExtrema_Poly::Distance(const TopoDS_Shape& S1,
                                            const TopoDS_Shape& S2,
                                            gp_Pnt&             P1,
                                            gp_Pnt&             P2,
                                            double&      dist)
{
  dist = Precision::Infinite();

  TopLoc_Location            L;
  occ::handle<Poly_Triangulation> Tr;
  TopExp_Explorer            exFace;

  int nbn1 = 0;
  for (exFace.Init(S1, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr                   = BRep_Tool::Triangulation(F, L);
    if (!Tr.IsNull())
      nbn1 += Tr->NbNodes();
  }
  if (nbn1 == 0)
    return false;

  int nbn2 = 0;
  for (exFace.Init(S2, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr                   = BRep_Tool::Triangulation(F, L);
    if (!Tr.IsNull())
      nbn2 += Tr->NbNodes();
  }
  if (nbn2 == 0)
    return false;

  int i, n;

  NCollection_Array1<gp_Pnt> TP1(1, nbn1);
  nbn1 = 0;

  for (exFace.Init(S1, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr                   = BRep_Tool::Triangulation(F, L);
    if (!Tr.IsNull())
    {
      const gp_Trsf aTrsf = L;
      n                   = Tr->NbNodes();
      for (i = 1; i <= n; i++)
      {
        nbn1++;
        TP1.SetValue(nbn1, Tr->Node(i).Transformed(aTrsf));
      }
    }
  }

  NCollection_Array1<gp_Pnt> TP2(1, nbn2);
  nbn2 = 0;

  for (exFace.Init(S2, TopAbs_FACE); exFace.More(); exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr                   = BRep_Tool::Triangulation(F, L);
    if (!Tr.IsNull())
    {
      const gp_Trsf aTrsf = L;
      n                   = Tr->NbNodes();
      for (i = 1; i <= n; i++)
      {
        nbn2++;
        TP2.SetValue(nbn2, Tr->Node(i).Transformed(aTrsf));
      }
    }
  }

  int i1, i2;
  for (i1 = 1; i1 <= nbn1; i1++)
  {
    const gp_Pnt& PP1 = TP1(i1);
    for (i2 = 1; i2 <= nbn2; i2++)
    {
      const gp_Pnt&       PP2  = TP2(i2);
      const double dCur = PP1.Distance(PP2);
      if (dist > dCur)
      {
        P1   = PP1;
        P2   = PP2;
        dist = dCur;
      }
    }
  }
  return true;
}
