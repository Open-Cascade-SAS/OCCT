// Created on: 2000-02-14
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

static void LastModif(TNaming_NewShapeIterator&                                      it,
                      const TopoDS_Shape&                                            S,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS,
                      const NCollection_Map<TDF_Label>&                              Updated,
                      const NCollection_Map<TDF_Label>&                              Forbiden)
{
  bool YaModif = false;
  for (; it.More(); it.Next())
  {
    const TDF_Label& Lab = it.Label();
    if (!Updated.IsEmpty() && !Updated.Contains(Lab))
      continue;
    if (TNaming::IsForbidden(Forbiden, Lab))
      continue;
    if (it.IsModification())
    {
      YaModif = true;
      TNaming_NewShapeIterator it2(it);
      if (!it2.More())
      {
        const TopoDS_Shape& aS = it.Shape();
        MS.Add(aS); // Modified
      }
      else
        LastModif(it2, it.Shape(), MS, Updated, Forbiden);
    }
  }
  if (!YaModif)
    MS.Add(S);
}

//=================================================================================================

void TNaming_NamingTool::CurrentShape(
  const NCollection_Map<TDF_Label>&                              Valid,
  const NCollection_Map<TDF_Label>&                              Forbiden,
  const occ::handle<TNaming_NamedShape>&                         Att,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS)
{
  TDF_Label Lab = Att->Label();
  if (!Valid.IsEmpty() && !Valid.Contains(Lab))
  {
    return;
  }

  TNaming_Iterator itL(Att);
  for (; itL.More(); itL.Next())
  {
    const TopoDS_Shape& S = itL.NewShape();
    if (S.IsNull())
      continue;
    bool               YaOrientationToApply(false);
    TopAbs_Orientation OrientationToApply(TopAbs_FORWARD);
    if (Att->Evolution() == TNaming_SELECTED)
    {
      if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX)
      { // OR-N
        occ::handle<TNaming_Naming> aNaming;
        Lab.FindAttribute(TNaming_Naming::GetID(), aNaming);
        if (!aNaming.IsNull())
        {
          if (aNaming->GetName().Type() == TNaming_ORIENTATION)
          {
            OrientationToApply = aNaming->GetName().Orientation();
          }
          else
          {
            occ::handle<TNaming_Naming> aNaming2;
            TDF_ChildIterator           it(aNaming->Label());
            for (; it.More(); it.Next())
            {
              const TDF_Label& aLabel = it.Value();
              aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming2);
              if (!aNaming2.IsNull())
              {
                if (aNaming2->GetName().Type() == TNaming_ORIENTATION)
                {
                  OrientationToApply = aNaming2->GetName().Orientation();
                  break;
                }
              }
            }
          }
          if (OrientationToApply == TopAbs_FORWARD || OrientationToApply == TopAbs_REVERSED)
            YaOrientationToApply = true;
        }
      } //
    }
    TNaming_NewShapeIterator it(itL);
    if (!it.More())
    {
      if (YaOrientationToApply)
        MS.Add(S.Oriented(OrientationToApply));
      else
        MS.Add(S);
    }
    else
    {
      //     LastModif(it, S, MS, Valid, Forbiden);
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS2;
      LastModif(it, S, MS2, Valid, Forbiden);
      if (YaOrientationToApply)
        TNaming::ApplyOrientation(MS2, OrientationToApply);
      for (int anItMS2 = 1; anItMS2 <= MS2.Extent(); ++anItMS2)
        MS.Add(MS2(anItMS2));
    }
  }
}

//=================================================================================================

void TNaming_NamingTool::CurrentShapeFromShape(
  const NCollection_Map<TDF_Label>&                              Valid,
  const NCollection_Map<TDF_Label>&                              Forbiden,
  const TDF_Label&                                               Acces,
  const TopoDS_Shape&                                            S,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS)
{
  TNaming_NewShapeIterator it(S, Acces);

  occ::handle<TNaming_NamedShape> NS = it.NamedShape();
  if (!NS.IsNull() && NS->Evolution() == TNaming_SELECTED)
    MS.Add(TNaming_Tool::GetShape(NS));
  else
  {
    if (!it.More())
      MS.Add(S);
    else
      LastModif(it, S, MS, Valid, Forbiden);
  }
}

//=================================================================================================

static void MakeDescendants(TNaming_NewShapeIterator& it, NCollection_Map<TDF_Label>& Descendants)
{
  for (; it.More(); it.Next())
  {
    Descendants.Add(it.Label());
    if (!it.Shape().IsNull())
    {
      TNaming_NewShapeIterator it2(it);
      MakeDescendants(it2, Descendants);
    }
  }
}

//=======================================================================
void BuildDescendants2(const occ::handle<TNaming_NamedShape>& NS,
                       const TDF_Label&                       ForbLab,
                       NCollection_Map<TDF_Label>&            Descendants)
{
  if (NS.IsNull())
    return;
  TNaming_NewShapeIterator it(NS);
  for (; it.More(); it.Next())
  {
    if (!it.NamedShape().IsNull())
    {
      if (ForbLab == it.Label())
        continue;
      Descendants.Add(it.Label());
      TNaming_NewShapeIterator it2(it);
      MakeDescendants(it2, Descendants);
    }
  }
}

//=================================================================================================

void TNaming_NamingTool::BuildDescendants(const occ::handle<TNaming_NamedShape>& NS,
                                          NCollection_Map<TDF_Label>&            Descendants)
{
  if (NS.IsNull())
    return;
  Descendants.Add(NS->Label());
  TNaming_NewShapeIterator it(NS);
  MakeDescendants(it, Descendants);
  TNaming_OldShapeIterator it2(NS);
  for (; it2.More(); it2.Next())
  {
    if (!it2.Shape().IsNull())
    {
      occ::handle<TNaming_NamedShape> ONS = TNaming_Tool::NamedShape(it2.Shape(), NS->Label());
      if (!ONS.IsNull())
      {
        BuildDescendants2(ONS, NS->Label(), Descendants);
      }
    }
  }
}
