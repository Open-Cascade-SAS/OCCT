// Created on: 1996-04-09
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepBuilderAPI_Collect.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

#ifdef OCCT_DEBUG
  #include <stdio.h>
bool Affich;
#endif

//=================================================================================================

static void BuildBack(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                            M1,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& BM1)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator it(M1);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape&                      KS = it.Key();
    NCollection_List<TopoDS_Shape>::Iterator itl(it.Value());
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Shape& VS = itl.Value();
      BM1.Bind(VS, KS);
    }
  }
}

//=================================================================================================

static void Replace(NCollection_List<TopoDS_Shape>&       L,
                    const TopoDS_Shape&                   Old,
                    const NCollection_List<TopoDS_Shape>& New)
{
  //-----------------------------------
  // Suppression de Old dans la liste.
  //-----------------------------------
  NCollection_List<TopoDS_Shape>::Iterator it(L);
  while (it.More())
  {
    if (it.Value().IsSame(Old))
    {
      L.Remove(it);
      break;
    }
    if (it.More())
      it.Next();
  }
  //---------------------------
  // Ajout de New a L.
  //---------------------------
  NCollection_List<TopoDS_Shape> copNew;
  copNew = New;
  L.Append(copNew);
}

//=================================================================================================

static void StoreImage(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& MG,
  const TopoDS_Shape&                                                                         S,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MGBack,
  const NCollection_List<TopoDS_Shape>&                                           LI)
{
  if (!LI.IsEmpty())
  {
    if (MGBack.IsBound(S))
    {

      Replace(MG.ChangeFind(MGBack(S)), S, LI);
    }
    else
    {
      if (!MG.IsBound(S))
      {
        NCollection_List<TopoDS_Shape> empty;
        MG.Bind(S, empty);
      }
      // Dans tous les cas on copie la liste pour eviter les pb de
      // const& dans BRepBuilderAPI.
      NCollection_List<TopoDS_Shape>::Iterator it;
      for (it.Initialize(LI); it.More(); it.Next())
      {
        const TopoDS_Shape& SS = it.Value();
        MG(S).Append(SS);
      }
    }
  }
}

//=================================================================================================

static void Update(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Mod,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Gen,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& ModBack,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& GenBack,
  const TopoDS_Shape&                                                             SI,
  BRepBuilderAPI_MakeShape&                                                       MKS,
  const TopAbs_ShapeEnum                                                          ShapeType)
{

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> DejaVu;
  TopExp_Explorer                                        exp;

  for (exp.Init(SI, ShapeType); exp.More(); exp.Next())
  {
    const TopoDS_Shape& S = exp.Current();
    if (!DejaVu.Add(S))
      continue;

    //---------------------------------------
    // Recuperation de l image de S par MKS.
    //---------------------------------------
    const NCollection_List<TopoDS_Shape>& LIM = MKS.Modified(S);
    if (!LIM.IsEmpty())
    {
      if (GenBack.IsBound(S))
      {
        // Modif de generation => generation du shape initial
        StoreImage(Gen, S, GenBack, LIM);
      }
      else
      {
        StoreImage(Mod, S, ModBack, LIM);
      }
    }
    const NCollection_List<TopoDS_Shape>& LIG = MKS.Generated(S);
    if (!LIG.IsEmpty())
    {
      if (ModBack.IsBound(S))
      {
        // Generation de modif  => generation du shape initial
        const TopoDS_Shape& IS = ModBack(S);
        StoreImage(Gen, IS, GenBack, LIG);
      }
      else
      {
        StoreImage(Gen, S, GenBack, LIG);
      }
    }
  }
}
#ifdef OCCT_DEBUG
//=================================================================================================

static void DEBControl(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    MG)
{
  char name[100];
  int  IK = 0;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator it(MG);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& OS = it.Key();
    Sprintf(name, "SK_%d", ++IK);
    NCollection_List<TopoDS_Shape>::Iterator itl(MG(OS));
    int                                      IV = 1;
    for (; itl.More(); itl.Next())
    {
      Sprintf(name, "SV_%d_%d", IK, IV++);
    }
  }
}
#endif
//=================================================================================================

BRepBuilderAPI_Collect::BRepBuilderAPI_Collect() = default;

//=================================================================================================

void BRepBuilderAPI_Collect::Add(const TopoDS_Shape& SI, BRepBuilderAPI_MakeShape& MKS)

{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> GenBack;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> ModBack;
  BuildBack(myGen, GenBack); // Vraiment pas optimum a Revoir
  BuildBack(myMod, ModBack);

  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_COMPOUND);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_COMPSOLID);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_SOLID);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_SHELL);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_FACE);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_WIRE);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_EDGE);
  Update(myMod, myGen, ModBack, GenBack, SI, MKS, TopAbs_VERTEX);

#ifdef OCCT_DEBUG
  if (Affich)
  {
    DEBControl(myGen);
    DEBControl(myMod);
  }
#endif
}

//=================================================================================================

void BRepBuilderAPI_Collect::AddGenerated(const TopoDS_Shape& S, const TopoDS_Shape& NS)
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> GenBack;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> ModBack;
  BuildBack(myGen, GenBack);
  BuildBack(myMod, ModBack);

  NCollection_List<TopoDS_Shape> LIG;
  LIG.Append(NS);
  if (ModBack.IsBound(S))
  {
    // Generation de modif  => generation du shape initial
    TopoDS_Shape IS = ModBack(S);
    StoreImage(myGen, IS, GenBack, LIG);
  }
  else
  {
    StoreImage(myGen, S, GenBack, LIG);
  }
}

//=================================================================================================

void BRepBuilderAPI_Collect::AddModif(const TopoDS_Shape& S, const TopoDS_Shape& NS)

{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> GenBack;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> ModBack;
  BuildBack(myGen, GenBack);
  BuildBack(myMod, ModBack);

  NCollection_List<TopoDS_Shape> LIG;
  LIG.Append(NS);
  if (GenBack.IsBound(S))
  {
    // Modif de generation => generation du shape initial
    StoreImage(myGen, S, GenBack, LIG);
  }
  else
  {
    StoreImage(myMod, S, ModBack, LIG);
  }
}

//=================================================================================================

static void FilterByShape(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& MG,
  const TopoDS_Shape&                                                                         SF)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MSF;
  TopExp_Explorer                                        exp;
  bool                                                   YaEdge   = false;
  bool                                                   YaVertex = false;
  for (exp.Init(SF, TopAbs_FACE); exp.More(); exp.Next())
    MSF.Add(exp.Current());

  //-------------------------------------------------------------
  // Suppression de toutes les images qui ne sont pas dans MSF.
  //-------------------------------------------------------------
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator it(MG);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape&                      OS  = it.Key();
    NCollection_List<TopoDS_Shape>&          LNS = MG.ChangeFind(OS);
    NCollection_List<TopoDS_Shape>::Iterator itl(LNS);
    while (itl.More())
    {
      const TopoDS_Shape& NS = itl.Value();
      //-------------------------------------------------------------------
      // Images contiennet des edges => ajout des edges resultat dans MSF.
      //-------------------------------------------------------------------
      if (!YaEdge && NS.ShapeType() == TopAbs_EDGE)
      {
        for (exp.Init(SF, TopAbs_EDGE); exp.More(); exp.Next())
        {
          MSF.Add(exp.Current());
        }
        YaEdge = true;
      }
      //-------------------------------------------------------------------
      // Images contiennet des vertex => ajout des vertex resultat dans MSF.
      //-------------------------------------------------------------------
      if (!YaVertex && NS.ShapeType() == TopAbs_VERTEX)
      {
        for (exp.Init(SF, TopAbs_VERTEX); exp.More(); exp.Next())
        {
          MSF.Add(exp.Current());
        }
        YaVertex = true;
      }
      //---------------------------------------
      // Si pas dans MSF suprresion de l image.
      //---------------------------------------
      if (!MSF.Contains(NS))
      {
        LNS.Remove(itl);
      }
      else if (itl.More())
        itl.Next();
    }
  }
#ifdef OCCT_DEBUG
  if (Affich)
  {
    DEBControl(MG);
  }
#endif
}

//=================================================================================================

const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
  BRepBuilderAPI_Collect::Modification() const
{
  return myMod;
}

//=================================================================================================

const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
  BRepBuilderAPI_Collect::Generated() const
{
  return myGen;
}

//=================================================================================================

void BRepBuilderAPI_Collect::Filter(const TopoDS_Shape& SF)
{
  FilterByShape(myGen, SF);
  FilterByShape(myMod, SF);
}
