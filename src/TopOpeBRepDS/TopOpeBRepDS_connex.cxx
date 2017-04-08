// Created on: 1997-11-25
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRepDS_connex.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

static TopTools_DataMapOfShapeListOfShape *GLOBAL_elf1 = NULL; // NYI to CDLize as a tool of DS
static TopTools_DataMapOfShapeListOfShape *GLOBAL_elf2 = NULL; // NYI to CDLize as a tool of DS
static TopTools_DataMapOfShapeListOfShape *GLOBAL_fle = NULL;  // NYI to CDLize as a tool of DS
static TopTools_ListOfShape *GLOBAL_los = NULL;  // NYI to CDLize as a tool of DS
static Standard_Boolean GLOBAL_FDSCNX_prepared = Standard_False;

//modified by NIZNHY-PKV Sun Dec 15 17:41:43 2002 f
//=======================================================================
//function : FDSCNX_Close
//purpose  :
//=======================================================================
void FDSCNX_Close()
{
  if (GLOBAL_elf1) {
    delete GLOBAL_elf1;
    GLOBAL_elf1=NULL;
  }
  //
  if (GLOBAL_elf2) {
    delete GLOBAL_elf2;
    GLOBAL_elf2=NULL;
  }
  //
  if (GLOBAL_fle) {
    delete GLOBAL_fle;
    GLOBAL_fle=NULL;
  }
  //
  if (GLOBAL_los) {
    delete GLOBAL_los;
    GLOBAL_los=NULL;
  }
  //
  GLOBAL_FDSCNX_prepared = Standard_False;
}
//modified by NIZNHY-PKV Sun Dec 15 17:41:40 2002 t

Standard_EXPORT const TopTools_ListOfShape& FDSCNX_EdgeConnexityShapeIndex(const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer SI)
{
  if (HDS.IsNull()) return *GLOBAL_los;
  if (!GLOBAL_FDSCNX_prepared) return *GLOBAL_los;
  if (SI != 1 && SI != 2) return *GLOBAL_los;
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  TopAbs_ShapeEnum t = E.ShapeType(); if (t != TopAbs_EDGE) return *GLOBAL_los;
  Standard_Boolean has = FDSCNX_HasConnexFace(E,HDS); if (!has) return *GLOBAL_los;
  Standard_Integer re = BDS.AncestorRank(E); if (re == 0) return *GLOBAL_los;
  TopTools_DataMapOfShapeListOfShape* pelf = (SI == 1) ? GLOBAL_elf1 : GLOBAL_elf2;
  TopTools_DataMapOfShapeListOfShape& elf = *pelf;
  const TopTools_ListOfShape& lof = elf.Find(E);
  return lof;
}

// S = edge --> liste de faces connexes par S
// S = face --> liste d'edges E de S qui ont au moins une autre face connexe
Standard_EXPORT const TopTools_ListOfShape& FDSCNX_EdgeConnexitySameShape(const TopoDS_Shape& S,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  TopAbs_ShapeEnum t = S.ShapeType();
  if      (t == TopAbs_EDGE) {
    Standard_Integer si = HDS->DS().AncestorRank(S);
    const TopTools_ListOfShape& lf = FDSCNX_EdgeConnexityShapeIndex(S,HDS,si);
    return lf;
  }
  else if (t == TopAbs_FACE) {
    TopTools_DataMapOfShapeListOfShape& fle = *GLOBAL_fle;
    if (fle.IsBound(S)) {
      const TopTools_ListOfShape& le = fle.Find(S);
      return le;
    }
  }
  return *GLOBAL_los;
}

Standard_EXPORT void FDSCNX_Prepare(const TopoDS_Shape& /*S1*/,
                                    const TopoDS_Shape& /*S2*/,
                                    const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (HDS.IsNull()) {
    GLOBAL_FDSCNX_prepared = Standard_False;
    return;
  }
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  if (GLOBAL_elf1 == NULL) GLOBAL_elf1 = (TopTools_DataMapOfShapeListOfShape*) new TopTools_DataMapOfShapeListOfShape();
  if (GLOBAL_elf2 == NULL) GLOBAL_elf2 = (TopTools_DataMapOfShapeListOfShape*) new TopTools_DataMapOfShapeListOfShape();
  if (GLOBAL_fle == NULL) GLOBAL_fle = (TopTools_DataMapOfShapeListOfShape*) new TopTools_DataMapOfShapeListOfShape();
  if (GLOBAL_los == NULL) GLOBAL_los = (TopTools_ListOfShape*) new TopTools_ListOfShape();
  GLOBAL_elf1->Clear();
  GLOBAL_elf2->Clear();
  GLOBAL_fle->Clear();
  GLOBAL_los->Clear();

  Standard_Integer i=0,n=BDS.NbShapes();
  for (i=1;i<=n;i++) {
    const TopoDS_Shape& f = BDS.Shape(i); if (f.ShapeType() != TopAbs_FACE) continue;
    Standard_Integer rf = BDS.AncestorRank(f); if (rf == 0) continue;
//    BDS.Shape(f);
    TopTools_DataMapOfShapeListOfShape& fle = *GLOBAL_fle;
    TopTools_DataMapOfShapeListOfShape& elf = (rf == 1) ? *GLOBAL_elf1 : *GLOBAL_elf2;
    TopExp_Explorer exe;
    for (exe.Init(f,TopAbs_EDGE);exe.More();exe.Next()) {
//    for (TopExp_Explorer exe(f,TopAbs_EDGE);exe.More();exe.Next()) {
      const TopoDS_Shape& e = exe.Current();
//               BDS.Shape(e);
//      Standard_Boolean se = BDS.IsSectionEdge(TopoDS::Edge(e)); if (!se) continue;
      Standard_Boolean hs = BDS.HasShape(TopoDS::Edge(e)); if (!hs) continue;

      TopTools_ListOfShape* aListFle = fle.ChangeSeek (f);
      if (aListFle == NULL)
      {
        aListFle = fle.Bound (f, TopTools_ListOfShape());
      }
      aListFle->Append(e);

      TopTools_ListOfShape* aListElf = elf.ChangeSeek (e);
      if (aListElf == NULL)
      {
        aListElf = elf.Bound (e, TopTools_ListOfShape());
      }
      aListElf->Append(f);
    }
  }
  GLOBAL_FDSCNX_prepared = Standard_True;
}

Standard_EXPORT Standard_Boolean FDSCNX_HasConnexFace(const TopoDS_Shape& S,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (HDS.IsNull())
  {
    return Standard_False;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  const TopAbs_ShapeEnum t = S.ShapeType();
  if (t != TopAbs_FACE
   && t != TopAbs_EDGE)
  {
    return Standard_False;
  }
  const Standard_Integer rs = BDS.AncestorRank(S);
  if (rs == 0)
  {
    return Standard_False;
  }

  TopTools_DataMapOfShapeListOfShape* pelf = (rs == 1) ? GLOBAL_elf1 : GLOBAL_elf2;
  if (pelf == NULL)
  {
    return Standard_False;
  }

  Standard_Boolean has = (t == TopAbs_EDGE ? pelf : GLOBAL_fle)->IsBound(S);
  return has;
}

Standard_EXPORT void FDSCNX_FaceEdgeConnexFaces(const TopoDS_Shape& F,const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS,TopTools_ListOfShape& LF)
{
  LF.Clear();
  // verifier que E est une arete de connexite de F
  Standard_Boolean EofF = Standard_False;
  const TopTools_ListOfShape& loe = FDSCNX_EdgeConnexitySameShape(F,HDS);
  if (loe.IsEmpty())
  {
    return;
  }
  for (TopTools_ListIteratorOfListOfShape i(loe);i.More();i.Next())
  {
    const TopoDS_Shape& e = i.Value();
//             HDS->Shape(e);
    Standard_Boolean b = e.IsSame(E);
    if (b)
    {
      EofF=Standard_True;
      break;
    }
  }
  if (!EofF)
  {
    return;
  }

  const TopTools_ListOfShape& lof = FDSCNX_EdgeConnexitySameShape(E,HDS);
  if (lof.IsEmpty())
  {
    return;
  }
  for (TopTools_ListIteratorOfListOfShape it(lof);it.More();it.Next())
  {
    const TopoDS_Shape& f = it.Value();
    Standard_Boolean b = f.IsSame(F);
    if (!b)
    {
      LF.Append(f);
    }
  }
}

Standard_EXPORT void FDSCNX_DumpIndex(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer I)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  Standard_Integer ns=BDS.NbShapes();
  if (I<1 || I>ns)
  {
    return;
  }

  Standard_Integer i=I;
  const TopoDS_Shape& s=BDS.Shape(i);
  TopAbs_ShapeEnum ts=s.ShapeType();
  const TopTools_ListOfShape& ls=FDSCNX_EdgeConnexitySameShape(s,HDS);
  if (ts == TopAbs_EDGE)
  {
    TopTools_ListIteratorOfListOfShape ils(ls);
    if (!ils.More())
    {
      return;
    }

    for (; ils.More(); ils.Next())
    {
      cout<<BDS.Shape(ils.Value())<<" ";
    }
  }
  else if (ts == TopAbs_FACE)
  {
    TopTools_ListIteratorOfListOfShape ils(ls);
    if (!ils.More())
    {
      return;
    }

    for (; ils.More(); ils.Next())
    {
      const TopoDS_Shape& e=ils.Value();
      TopTools_ListOfShape lf;FDSCNX_FaceEdgeConnexFaces(s,e,HDS,lf);
      TopTools_ListIteratorOfListOfShape ilf(lf);
      if (!ilf.More())
      {
        continue;
      }
      for (; ilf.More(); ilf.Next())
      {
        cout<<BDS.Shape(ilf.Value())<<" ";
      }
    }
  }
}

Standard_EXPORT void FDSCNX_Dump(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer I)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  Standard_Integer ns=BDS.NbShapes();
  if (I<1 || I>ns)
  {
    return;
  }

  Standard_Integer i=I;
  const TopoDS_Shape& s=BDS.Shape(i);Standard_Integer is=BDS.Shape(s);TopAbs_ShapeEnum ts=s.ShapeType();
  const TopTools_ListOfShape& ls=FDSCNX_EdgeConnexitySameShape(s,HDS);
  if      (ts == TopAbs_EDGE)
  {
    TopTools_ListIteratorOfListOfShape ils(ls);
    if(!ils.More())
    {
      return;
    }

    cout<<"clear;";
    for (; ils.More(); ils.Next())
    {
      cout<<"tsee f "<<BDS.Shape(ils.Value())<<";";
    }
    cout<<"tsee e "<<is<<";### edge "<<is<<" connexity"<<endl;
  }
  else if (ts == TopAbs_FACE)
  {
    TopTools_ListIteratorOfListOfShape ils(ls);
    if (!ils.More())
    {
      return;
    }
    for(;ils.More();ils.Next())
    {
      const TopoDS_Shape& e=ils.Value();Standard_Integer ie=BDS.Shape(e);
      TopTools_ListOfShape lf;FDSCNX_FaceEdgeConnexFaces(s,e,HDS,lf);
      TopTools_ListIteratorOfListOfShape ilf(lf);
      if (!ilf.More())
      {
        continue;
      }

      cout<<"clear;";cout<<"tsee f "<<is<<";";
      for (;ilf.More(); ilf.Next())
      {
        cout<<"tsee f "<<BDS.Shape(ilf.Value())<<";";
      }
      cout<<"tsee e "<<ie<<";### face "<<is<<" connexity"<<endl;
    }
  }
}

Standard_EXPORT void FDSCNX_Dump(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (HDS.IsNull())
  {
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  Standard_Integer ns=BDS.NbShapes();
  for (Standard_Integer i = 1;i<=ns;i++)
  {
    FDSCNX_Dump(HDS,i);
  }
}
