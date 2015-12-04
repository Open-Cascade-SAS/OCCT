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
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESSelect_SelectBasicGeom.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_Shell.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SelectBasicGeom,IFSelect_SelectExplore)

IGESSelect_SelectBasicGeom::IGESSelect_SelectBasicGeom
  (const Standard_Integer mode)
    : IFSelect_SelectExplore (-1)    {  thegeom = mode;  }

    Standard_Boolean  IGESSelect_SelectBasicGeom::Explore
  (const Standard_Integer /*level*/, const Handle(Standard_Transient)& ent,
   const Interface_Graph& /*G*/, Interface_EntityIterator& explored) const
{
//  thegeom > 0 : curves3d   < 0 : surfaces   == 0 : curves3d + surfaces libres

  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer igt = igesent->TypeNumber();

//   CompositeCurve : a decomposer ?
  if (igt == 102 && thegeom == 2) {
    DeclareAndCast(IGESGeom_CompositeCurve,cmc,ent);
    Standard_Integer i, nb = cmc->NbCurves();
    for (i = 1; i <= nb; i ++) explored.AddItem (cmc->Curve(i));
    return Standard_True;
  }

//   Lignes en general. Attention CopiousData, aux variantes "habillage"
  if (igt == 106) return (igesent->FormNumber() < 20);
  if ( (igt >= 100 && igt <= 106) || igt == 110 || igt == 112 || igt == 116 ||
      igt == 126 || igt == 130) return (thegeom >= 0);

//   Surfaces LIBRES, car il n y a pas d autre moyen de les reperer
//   (l ideal serait de prendre les bords naturels)
//   Ou surfaces debarassees de leurs contours
  if (igt == 114 || igt == 118 || igt == 120 || igt == 122 || igt == 128 || igt == 140 || igt == 190)
    return (thegeom <= 0);

//   Plan 108
//   mode surface : on retourne tout le Plane sinon c est inexploitable
  if (igt == 108) {
    DeclareAndCast(IGESGeom_Plane,pln,ent);
    if (thegeom >= 0) explored.AddItem(pln->BoundingCurve());
    return pln->HasBoundingCurve();
  }

  if (igt == 116) return (thegeom >= 0);  // on point, ca va bien ...

//   TrimmedSurface 144
  if (igt == 144) {
    DeclareAndCast(IGESGeom_TrimmedSurface,trs,ent);
    if (thegeom >= 0) {
      explored.AddItem(trs->OuterContour());
      Standard_Integer i, nb = trs->NbInnerContours();
      for (i = 1; i <= nb; i ++) explored.AddItem (trs->InnerContour(i));
    }
    else explored.AddItem (trs->Surface());
    return Standard_True;
  }

//   CurveOnSurface 142
  if (igt == 142 && thegeom >= 0) {
    DeclareAndCast(IGESGeom_CurveOnSurface,crf,ent);
    explored.AddItem(crf->Curve3D());
    return Standard_True;
  }

//   Boundary 141
  if (igt == 141 && thegeom >= 0) {
    DeclareAndCast(IGESGeom_Boundary,bnd,ent);
    Standard_Integer i, nb = bnd->NbModelSpaceCurves();
    for (i = 1; i <= nb; i ++) explored.AddItem (bnd->ModelSpaceCurve(i));
    return (nb > 0);
  }

//   BoundedSurface 143
  if (igt == 143) {
    DeclareAndCast(IGESGeom_BoundedSurface,bns,ent);
    Standard_Integer i, nb = 0;
    if (thegeom >= 0) {
      nb = bns->NbBoundaries();
      for (i = 1; i <= nb; i ++) explored.AddItem (bns->Boundary(i));
      return (nb != 0);
    }
    else explored.AddItem (bns->Surface());
    return Standard_True;
  }

//  SingleParent
  if (igt == 402 && igesent->FormNumber() == 9) {
    DeclareAndCast(IGESBasic_SingleParent,sp,ent);
    if (sp.IsNull()) return Standard_False;
    explored.AddItem (sp->SingleParent());
    Standard_Integer i,nb = sp->NbChildren();
    for (i = 1; i <= nb; i ++) explored.AddItem (sp->Child(i));
    return Standard_True;
  }

//  Groups ... en dernier de la serie 402
  if (igt == 402) {
    DeclareAndCast(IGESBasic_Group,gr,ent);
    if (gr.IsNull()) return Standard_False;
    Standard_Integer i, nb = gr->NbEntities();
    for (i = 1; i <= nb; i ++)  explored.AddItem (gr->Entity(i));
    return Standard_True;
  }

//  ManifoldSolid 186  -> Shells
  if (igt == 186) {
    DeclareAndCast(IGESSolid_ManifoldSolid,msb,ent);
    explored.AddItem (msb->Shell());
    Standard_Integer i, nb = msb->NbVoidShells();
    for (i = 1; i <= nb; i ++)  explored.AddItem (msb->VoidShell(i));
    return Standard_True;
  }

//  Shell 514 -> Faces
  if (igt == 514) {
    DeclareAndCast(IGESSolid_Shell,sh,ent);
    Standard_Integer i, nb = sh->NbFaces();
    for (i = 1; i <= nb; i ++)  explored.AddItem (sh->Face(i));
    return Standard_True;
  }

//  Face 510 -> Loops
  if (igt == 510) {
    DeclareAndCast(IGESSolid_Face,fc,ent);
    if (thegeom >= 0) {
      Standard_Integer i, nb = fc->NbLoops();
      for (i = 1; i <= nb; i ++)  explored.AddItem (fc->Loop(i));
    }
    else explored.AddItem (fc->Surface());
    return Standard_True;
  }

//  Loop 508 -> Curves 3D (enfin !)  mais via EdgeList ...
  if (igt == 508 && thegeom >= 0) {
    DeclareAndCast(IGESSolid_Loop,lp,ent);
    Standard_Integer i, nb = lp->NbEdges();
    for (i = 1; i <= nb; i ++)  {
      DeclareAndCast(IGESSolid_EdgeList,edl,lp->Edge(i));
      Standard_Integer ind = lp->ListIndex(i);
      if (edl.IsNull()) continue;
      explored.AddItem(edl->Curve(ind));
    }
    return Standard_True;
  }

//  Pas trouve
  return Standard_False;
}


    TCollection_AsciiString IGESSelect_SelectBasicGeom::ExploreLabel () const
{
  if (thegeom == 2) return TCollection_AsciiString ("Basic Curves 3d");
  else if (thegeom > 0) return TCollection_AsciiString ("Curves 3d");
  else if (thegeom < 0) return TCollection_AsciiString ("Surfaces");
  else  return TCollection_AsciiString ("Basic Geometry");
}


    Standard_Boolean  IGESSelect_SelectBasicGeom::SubCurves
  (const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& explored)
{
  if (ent.IsNull()) return Standard_False;
  Standard_Integer igt = ent->TypeNumber();

//   CompositeCurve : a decomposer ?
  if (igt == 102) {
    DeclareAndCast(IGESGeom_CompositeCurve,cmc,ent);
    Standard_Integer i, nb = cmc->NbCurves();
    for (i = 1; i <= nb; i ++) explored.AddItem (cmc->Curve(i));
    return Standard_True;
  }

//   Lignes en general. Attention CopiousData, aux variantes "habillage"
  if (igt == 106) return (ent->FormNumber() < 20);
  if ( (igt >= 100 && igt <= 106) || igt == 110 || igt == 112 || igt == 116 ||
      igt == 126 || igt == 130) return Standard_True;

//  Sinon
  return Standard_False;
}
