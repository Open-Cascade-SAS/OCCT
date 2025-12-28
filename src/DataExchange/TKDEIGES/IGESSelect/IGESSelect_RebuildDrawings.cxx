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

#include <gp_Pnt2d.hxx>
#include <IFSelect_ContextModif.hxx>
#include <IFSelect_PacketList.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESSelect_RebuildDrawings.hxx>
#include <IGESSelect_ViewSorter.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Type.hxx>
#include <gp_XY.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_RebuildDrawings, IGESSelect_ModelModifier)

#define PourDrawing 404

IGESSelect_RebuildDrawings::IGESSelect_RebuildDrawings()
    : IGESSelect_ModelModifier(true)
{
}

void IGESSelect_RebuildDrawings::Performing(IFSelect_ContextModif&                 ctx,
                                            const occ::handle<IGESData_IGESModel>& target,
                                            Interface_CopyTool&                    TC) const
{
  //  On reconstruit les drawings qui peuvent l etre
  //  Pour chaque drawing de l original, on regarde les composants transferes
  //   (evt filtres par <ctx>). Pour cela, on s aide d un ViewSorter
  //  Pour chaque drawing dont au moins un element a ete transfere :
  //  - On passe le Drawing lui-meme, avec ses views, mais pas ses annotations
  //    (c-a-d on le reconstruit)
  //  - On reconnecte les views aux entites (cf Directory Part)

  DeclareAndCast(IGESData_IGESModel, original, ctx.OriginalModel());
  int                     nbo = original->NbEntities();
  NCollection_Array1<int> pris(0, nbo);
  pris.Init(0);

  //  Entites a prendre en compte pour la reconstruction
  //  NB : Les drawings deja transferes ne sont bien sur pas reconstruits !
  occ::handle<IGESSelect_ViewSorter> sorter = new IGESSelect_ViewSorter;
  sorter->SetModel(original);
  sorter->Add(original);
  for (ctx.Start(); ctx.More(); ctx.Next())
  {
    pris.SetValue(original->Number(ctx.ValueOriginal()), 1);
  }
  sorter->SortDrawings(ctx.OriginalGraph());
  occ::handle<IFSelect_PacketList> sets = sorter->Sets(true);
  //  Regarder, pour chaque paquet, si au moins un element a ete copie
  int nbs = sets->NbPackets();
  for (int is = 1; is <= nbs; is++)
  {
    Interface_EntityIterator setl = sets->Entities(is);
    Interface_EntityIterator newset;
    for (setl.Start(); setl.More(); setl.Next())
    {
      occ::handle<Standard_Transient> newent;
      if (TC.Search(setl.Value(), newent))
        newset.AddItem(newent);
    }
    if (newset.NbEntities() == 0)
      continue;
    //    On en tient un : le transferer (le reconstruire)
    occ::handle<IGESData_IGESEntity> item = sorter->SetItem(is, true);
    if (item->TypeNumber() != PourDrawing)
      continue;
    if (item->IsKind(STANDARD_TYPE(IGESDraw_Drawing)))
    {
      DeclareAndCast(IGESDraw_Drawing, draw, item);
      int                                                                    nbv = draw->NbViews();
      occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
      if (nbv > 0)
        views = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbv);
      //  Passer ses vues : toutes
      //  Aussi les positions des vues
      occ::handle<NCollection_HArray1<gp_XY>> origs;
      if (nbv > 0)
        origs = new NCollection_HArray1<gp_XY>(1, nbv);
      for (int iv = 1; iv <= nbv; iv++)
      {
        DeclareAndCast(IGESData_ViewKindEntity, aview, TC.Transferred(draw->ViewItem(iv)));
        views->SetValue(iv, aview);
        target->AddEntity(aview);
        origs->SetValue(iv, draw->ViewOrigin(iv).XY());
      }
      //  Frame : passer ce qui a ete transfere
      occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> frame;
      int                      nba = draw->NbAnnotations();
      Interface_EntityIterator framelist;
      int                      ia; // svv Jan11 2000 : porting on DEC
      for (ia = 1; ia <= nba; ia++)
      {
        occ::handle<Standard_Transient> annot;
        if (TC.Search(draw->Annotation(ia), annot))
          framelist.GetOneItem(annot);
      }
      nba = framelist.NbEntities();
      ia  = 0;
      if (nba > 0)
        frame = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nba);
      for (framelist.Start(); framelist.More(); framelist.Next())
      {
        ia++;
        frame->SetValue(ia, GetCasted(IGESData_IGESEntity, framelist.Value()));
      }
      //  Cayest, fabriquer le nouveau Drawing et l ajouter
      occ::handle<IGESDraw_Drawing> newdraw = new IGESDraw_Drawing;
      newdraw->Init(views, origs, frame);
      //  Reste le nom, + autres ? drawing unit, ...

      //    Drawing With Rotation : quasiment identique
    }
    else if (item->IsKind(STANDARD_TYPE(IGESDraw_DrawingWithRotation)))
    {
      DeclareAndCast(IGESDraw_DrawingWithRotation, draw, item);
      int                                                                    nbv = draw->NbViews();
      occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
      if (nbv > 0)
        views = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbv);
      //  Passer ses vues : toutes
      //  Aussi les positions des vues .. et les rotations
      occ::handle<NCollection_HArray1<gp_XY>> origs;
      if (nbv > 0)
        origs = new NCollection_HArray1<gp_XY>(1, nbv);
      occ::handle<NCollection_HArray1<double>> rots;
      if (nbv > 0)
      {
        rots = new NCollection_HArray1<double>(1, nbv);
        rots->Init(0.0);
      }

      for (int iv = 1; iv <= nbv; iv++)
      {
        DeclareAndCast(IGESData_ViewKindEntity, aview, TC.Transferred(draw->ViewItem(iv)));
        views->SetValue(iv, aview);
        target->AddEntity(aview);
        rots->SetValue(iv, draw->OrientationAngle(iv));
        origs->SetValue(iv, draw->ViewOrigin(iv).XY());
      }
      //  Frame : passer ce qui a ete transfere
      occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> frame;
      int                      nba = draw->NbAnnotations();
      Interface_EntityIterator framelist;
      int                      ia; // svv Jan11 2000 : porting on DEC
      for (ia = 1; ia <= nba; ia++)
      {
        occ::handle<Standard_Transient> annot;
        if (TC.Search(draw->Annotation(ia), annot))
          framelist.GetOneItem(annot);
      }
      nba = framelist.NbEntities();
      ia  = 0;
      if (nba > 0)
        frame = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nba);
      for (framelist.Start(); framelist.More(); framelist.Next())
      {
        ia++;
        frame->SetValue(ia, GetCasted(IGESData_IGESEntity, framelist.Value()));
      }
      //  Cayest, fabriquer le nouveau DrawingWithRotation et l ajouter
      occ::handle<IGESDraw_DrawingWithRotation> newdraw = new IGESDraw_DrawingWithRotation;
      newdraw->Init(views, origs, rots, frame);
      //  Reste le nom, + autres ? drawing unit, ...
    }

    //  Il faut encore mettre a jour les Views notees en Directory Part
    //  Pour cela, considerer <setl>, pour chaque terme, regarder View()
    //  si View() transfere, mettre a jour ...
    for (setl.Start(); setl.More(); setl.Next())
    {
      DeclareAndCast(IGESData_IGESEntity, ent, setl.Value());
      occ::handle<IGESData_ViewKindEntity> vieworig = ent->View();
      if (vieworig.IsNull())
        continue;
      occ::handle<Standard_Transient> aView;
      if (!TC.Search(vieworig, aView))
        continue;
      occ::handle<IGESData_ViewKindEntity> viewnew = occ::down_cast<IGESData_ViewKindEntity>(aView);
      if (!viewnew.IsNull())
        ent->InitView(viewnew);
    }
  }
}

TCollection_AsciiString IGESSelect_RebuildDrawings::Label() const
{
  return TCollection_AsciiString("Rebuild Drawings (with empty views)");
}
