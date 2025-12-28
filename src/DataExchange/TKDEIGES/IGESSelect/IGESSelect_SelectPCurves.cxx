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
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESSelect_SelectBasicGeom.hxx>
#include <IGESSelect_SelectPCurves.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectPCurves, IFSelect_SelectExplore)

IGESSelect_SelectPCurves::IGESSelect_SelectPCurves(const bool basic)
    : IFSelect_SelectExplore(-1),
      thebasic(basic)
{
}

bool IGESSelect_SelectPCurves::Explore(const int                              level,
                                       const occ::handle<Standard_Transient>& ent,
                                       const Interface_Graph& /*G*/,
                                       Interface_EntityIterator& explored) const
{
  DeclareAndCast(IGESData_IGESEntity, igesent, ent);
  if (igesent.IsNull())
    return false;
  int igt = igesent->TypeNumber();

  //   TrimmedSurface 144
  if (igt == 144)
  {
    DeclareAndCast(IGESGeom_TrimmedSurface, trs, ent);
    explored.AddItem(trs->OuterContour());
    int i, nb = trs->NbInnerContours();
    for (i = 1; i <= nb; i++)
      explored.AddItem(trs->InnerContour(i));
    return true;
  }

  //   CurveOnSurface 142
  if (igt == 142)
  {
    DeclareAndCast(IGESGeom_CurveOnSurface, crf, ent);
    explored.AddItem(crf->CurveUV());
    if (thebasic)
      IGESSelect_SelectBasicGeom::SubCurves(crf->CurveUV(), explored);
    return true;
  }

  //   Boundary 141
  if (igt == 141)
  {
    DeclareAndCast(IGESGeom_Boundary, bnd, ent);
    int i, nb = bnd->NbModelSpaceCurves();
    for (i = 1; i <= nb; i++)
    {
      int j, np = bnd->NbParameterCurves(i);
      for (j = 1; j <= np; j++)
      {
        explored.AddItem(bnd->ParameterCurve(i, j));
      }
    }
    return (nb > 0);
  }

  //   BoundedSurface 143
  if (igt == 143)
  {
    DeclareAndCast(IGESGeom_BoundedSurface, bns, ent);
    int i, nb = bns->NbBoundaries(); // szv#4:S4163:12Mar99 optimized
    for (i = 1; i <= nb; i++)
      explored.AddItem(bns->Boundary(i));
    return (nb != 0);
    // return true; //szv#4:S4163:12Mar99 unreached
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

  //  Face 510 -> Loops
  if (igt == 510)
  {
    DeclareAndCast(IGESSolid_Face, fc, ent);
    int i, nb = fc->NbLoops();
    for (i = 1; i <= nb; i++)
      explored.AddItem(fc->Loop(i));
    return true;
  }

  //  Loop 508 -> PCurves (enfin !)
  if (igt == 508)
  {
    DeclareAndCast(IGESSolid_Loop, lp, ent);
    int i, nb = lp->NbEdges();
    for (i = 1; i <= nb; i++)
    {
      int j, np = lp->NbParameterCurves(i);
      for (j = 1; j <= np; j++) // szv#4:S4163:12Mar99 was bug
        explored.AddItem(lp->ParametricCurve(i, j));
    }
    return true;
  }

  //   LES LIGNES : seult si en tant que pcurve : donc level >= 3
  //   Lignes en general. Attention CopiousData, aux variantes "habillage"
  if (level < 3)
    return false;

  if (igt == 106)
    return (igesent->FormNumber() < 20);
  if ((igt >= 100 && igt <= 106) || igt == 110 || igt == 112 || igt == 116 || igt == 126
      || igt == 130)
    return true;

  //  Pas trouve
  return false;
}

TCollection_AsciiString IGESSelect_SelectPCurves::ExploreLabel() const
{
  if (thebasic)
    return TCollection_AsciiString("Basic PCurves");
  else
    return TCollection_AsciiString("Global PCurves");
}
