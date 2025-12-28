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

#include <IGESBasic_Group.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESSelect_SelectFaces.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectFaces, IFSelect_SelectExplore)

IGESSelect_SelectFaces::IGESSelect_SelectFaces()
    : IFSelect_SelectExplore(-1)
{
}

bool IGESSelect_SelectFaces::Explore(const int /*level*/,
                                                 const occ::handle<Standard_Transient>& ent,
                                                 const Interface_Graph& /*G*/,
                                                 Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESData_IGESEntity, igesent, ent);
  if (igesent.IsNull())
    return false;
  int igt = igesent->TypeNumber();

  //   Cas clairs et nets : Faces typees comme telles

  if (igt == 510 || igt == 144 || igt == 143)
    return true;

  //   Surfaces Libres
  if (igt == 114 || igt == 118 || igt == 120 || igt == 122 || igt == 128 || igt == 140)
    return true;

  //   Cas du Plane
  if (igt == 108)
  {
    DeclareAndCast(IGESGeom_Plane, pln, ent);
    return pln->HasBoundingCurve();
  }

  //   A present, contenants possibles

  //  SingleParent
  if (igt == 402 && igesent->FormNumber() == 9)
  {
    DeclareAndCast(IGESBasic_SingleParent, sp, ent);
    if (sp.IsNull())
      return false;
    explored.AddItem(sp->SingleParent());
    int i, nb = sp->NbChildren();
    for (i = 1; i <= nb; i++)
      explored.AddItem(sp->Child(i));
    return true;
  }

  //  Groups ... en dernier de la serie 402
  if (igt == 402)
  {
    DeclareAndCast(IGESBasic_Group, gr, ent);
    if (gr.IsNull())
      return false;
    int i, nb = gr->NbEntities();
    for (i = 1; i <= nb; i++)
      explored.AddItem(gr->Entity(i));
    return true;
  }

  //  ManifoldSolid 186  -> Shells
  if (igt == 186)
  {
    DeclareAndCast(IGESSolid_ManifoldSolid, msb, ent);
    explored.AddItem(msb->Shell());
    int i, nb = msb->NbVoidShells();
    for (i = 1; i <= nb; i++)
      explored.AddItem(msb->VoidShell(i));
    return true;
  }

  //  Shell 514 -> Faces
  if (igt == 514)
  {
    DeclareAndCast(IGESSolid_Shell, sh, ent);
    int i, nb = sh->NbFaces();
    for (i = 1; i <= nb; i++)
      explored.AddItem(sh->Face(i));
    return true;
  }

  //  Pas trouve
  return false;
}

TCollection_AsciiString IGESSelect_SelectFaces::ExploreLabel() const
{
  return TCollection_AsciiString("Faces");
}
