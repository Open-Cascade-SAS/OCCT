// Created on: 1995-09-08
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepExtrema_Poly.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>

//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_Poly::Distance (const TopoDS_Shape& S1, const TopoDS_Shape& S2,
                                             gp_Pnt& P1, gp_Pnt& P2, Standard_Real& dist)
{
  dist = Precision::Infinite();

  TopLoc_Location L;
  Handle(Poly_Triangulation) Tr;
  TopExp_Explorer exFace;

  Standard_Integer nbn1 = 0;
  for (exFace.Init(S1, TopAbs_FACE);
       exFace.More(); 
       exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr = BRep_Tool::Triangulation(F,L);
    if (!Tr.IsNull())
      nbn1 += Tr->NbNodes();
  }
  if (nbn1 == 0) return Standard_False;

  Standard_Integer nbn2 = 0;
  for (exFace.Init(S2, TopAbs_FACE);
       exFace.More(); 
       exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr = BRep_Tool::Triangulation(F,L);
    if (!Tr.IsNull())
      nbn2 += Tr->NbNodes();
  }
  if (nbn2 == 0) return Standard_False;

  Standard_Integer i,n;

  TColgp_Array1OfPnt TP1(1,nbn1);
  nbn1 = 0;
  
  for (exFace.Init(S1, TopAbs_FACE);
       exFace.More(); 
       exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr = BRep_Tool::Triangulation(F,L);
    if (!Tr.IsNull())
    {
      const TColgp_Array1OfPnt& Nod = Tr->Nodes();
      n = Tr->NbNodes();
      for (i = 1; i <= n; i++)
      {
        nbn1++; 
        TP1.SetValue(nbn1,Nod(i).Transformed(L));
      }
    }
  }
  
  TColgp_Array1OfPnt TP2(1,nbn2);
  nbn2 = 0;
  
  for (exFace.Init(S2, TopAbs_FACE);
       exFace.More(); 
       exFace.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exFace.Current());
    Tr = BRep_Tool::Triangulation(F,L);
    if (!Tr.IsNull())
    {
      const TColgp_Array1OfPnt& Nod = Tr->Nodes();
      n = Tr->NbNodes();
      for (i = 1; i <= n; i++)
      {
        nbn2++; 
        TP2.SetValue(nbn2,Nod(i).Transformed(L));
      }
    }
  }

  Standard_Integer i1,i2;
  for (i1 = 1; i1 <= nbn1; i1++)
  {
    const gp_Pnt& PP1 = TP1(i1);
    for (i2 = 1; i2 <= nbn2; i2++)
    {
      const gp_Pnt& PP2 = TP2(i2);
      const Standard_Real dCur = PP1.Distance(PP2);
      if (dist > dCur)
      {
        P1 = PP1;
        P2 = PP2;
        dist = dCur;
      }
    }
  }
  return Standard_True;
}
