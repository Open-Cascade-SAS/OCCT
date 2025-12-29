// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_ToolDrawing.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDraw_ToolDrawing::IGESDraw_ToolDrawing() = default;

void IGESDraw_ToolDrawing::ReadOwnParams(const occ::handle<IGESDraw_Drawing>&        ent,
                                         const occ::handle<IGESData_IGESReaderData>& IR,
                                         IGESData_ParamReader&                       PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int nbval;

  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
  occ::handle<NCollection_HArray1<gp_XY>>                                viewOrigins;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     annotations;

  // Reading nbval(No. of View pointers)
  bool st = PR.ReadInteger(PR.Current(), "Count of array of view entities", nbval);
  if (st && nbval > 0)
  {
    views       = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbval);
    viewOrigins = new NCollection_HArray1<gp_XY>(1, nbval);

    occ::handle<IGESData_ViewKindEntity> tempView;
    gp_XY                                tempXY;

    for (int i = 1; i <= nbval; i++)
    {
      // Reading views(HArray1OfView)
      // st = PR.ReadEntity (IR, PR.Current(), "View Entity",
      // STANDARD_TYPE(IGESData_ViewKindEntity), tempView,true); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "View Entity",
                        STANDARD_TYPE(IGESData_ViewKindEntity),
                        tempView,
                        true))
        views->SetValue(i, tempView);

      // Reading viewOrigins(HArray1OfXY)
      // st = PR.ReadXY(PR.CurrentList(1, 2), "array viewOrigins", tempXY); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadXY(PR.CurrentList(1, 2), "array viewOrigins", tempXY))
        viewOrigins->SetValue(i, tempXY);
    }
  }
  else if (nbval < 0)
    PR.AddFail("Count of view entities : Less than zero");

  // Reading nbval(No. of Annotation Entities)
  // st = PR.ReadInteger(PR.Current(),"Count of array of Annotation entities", nbval);
  // //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "Count of array of Annotation entities", nbval))
  {
    if (nbval > 0)
      // clang-format off
      PR.ReadEnts (IR,PR.CurrentList(nbval), "Annotation Entities", annotations); //szv#4:S4163:12Mar99 `st=` not needed
    // clang-format on
    /*
          {
        // Reading annotations(HArray1OfIGESEntity)
        annotations = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbval);
        occ::handle<IGESData_IGESEntity> tempAnnotation;
        for (int i = 1; i <= nbval; i++)
              {
            st = PR.ReadEntity
              (IR, PR.Current(), "annotation entity", tempAnnotation,true);
            if (st) annotations->SetValue(i, tempAnnotation);
              }
          }
    */
    else if (nbval < 0)
      PR.AddFail("Count of Annotation entities : Less than zero");
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(views, viewOrigins, annotations);
}

void IGESDraw_ToolDrawing::WriteOwnParams(const occ::handle<IGESDraw_Drawing>& ent,
                                          IGESData_IGESWriter&                 IW) const
{
  int Up = ent->NbViews();
  IW.Send(Up);
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= Up; i++)
  {
    IW.Send(ent->ViewItem(i));
    IW.Send((ent->ViewOrigin(i)).X());
    IW.Send((ent->ViewOrigin(i)).Y());
  }

  Up = ent->NbAnnotations();
  IW.Send(Up);
  for (i = 1; i <= Up; i++)
    IW.Send(ent->Annotation(i));
}

void IGESDraw_ToolDrawing::OwnShared(const occ::handle<IGESDraw_Drawing>& ent,
                                     Interface_EntityIterator&            iter) const
{
  int Up = ent->NbViews();
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= Up; i++)
    iter.GetOneItem(ent->ViewItem(i));

  Up = ent->NbAnnotations();
  for (i = 1; i <= Up; i++)
    iter.GetOneItem(ent->Annotation(i));
}

void IGESDraw_ToolDrawing::OwnCopy(const occ::handle<IGESDraw_Drawing>& another,
                                   const occ::handle<IGESDraw_Drawing>& ent,
                                   Interface_CopyTool&                  TC) const
{
  int                                                                    nbanot;
  int                                                                    nbval;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
  occ::handle<NCollection_HArray1<gp_XY>>                                viewOrigins;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     annotations;

  nbanot = another->NbAnnotations();
  nbval  = another->NbViews();
  if (nbval > 0)
  {
    views       = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbval);
    viewOrigins = new NCollection_HArray1<gp_XY>(1, nbval);
  }
  if (nbanot > 0)
  {
    annotations = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbanot);
    for (int i = 1; i <= nbanot; i++)
    {
      DeclareAndCast(IGESData_IGESEntity, tempAnnotation, TC.Transferred(another->Annotation(i)));
      annotations->SetValue(i, tempAnnotation);
    }
  }

  for (int i = 1; i <= nbval; i++)
  {
    DeclareAndCast(IGESData_ViewKindEntity, tempView, TC.Transferred(another->ViewItem(i)));
    views->SetValue(i, tempView);

    viewOrigins->SetValue(i, (another->ViewOrigin(i)).XY());
  }

  ent->Init(views, viewOrigins, annotations);
}

bool IGESDraw_ToolDrawing::OwnCorrect(const occ::handle<IGESDraw_Drawing>& ent) const
{
  //  Empty views: remove them
  int i, nb = ent->NbViews();
  int nbtrue = nb;
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_ViewKindEntity> val = ent->ViewItem(i);
    if (val.IsNull())
      nbtrue--;
    else if (val->TypeNumber() == 0)
      nbtrue--;
  }
  if (nbtrue == nb)
    return false;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
  occ::handle<NCollection_HArray1<gp_XY>>                                viewOrigins;
  if (nbtrue > 0)
  {
    views       = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbtrue);
    viewOrigins = new NCollection_HArray1<gp_XY>(1, nbtrue);
  }
  nbtrue = 0;
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_ViewKindEntity> val = ent->ViewItem(i);
    if (val.IsNull())
      continue;
    else if (val->TypeNumber() == 0)
      continue;
    nbtrue++;
    views->SetValue(nbtrue, val);
    viewOrigins->SetValue(nbtrue, ent->ViewOrigin(i).XY());
  }

  //  Don't forget the annotations ...
  int                                                                nbanot = ent->NbAnnotations();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> annotations =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbanot);
  for (i = 1; i <= nbanot; i++)
    annotations->SetValue(i, ent->Annotation(i));

  ent->Init(views, viewOrigins, annotations);
  return true;
}

IGESData_DirChecker IGESDraw_ToolDrawing::DirChecker(
  const occ::handle<IGESDraw_Drawing>& /*ent*/) const
{
  IGESData_DirChecker DC(404, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_ToolDrawing::OwnCheck(const occ::handle<IGESDraw_Drawing>& ent,
                                    const Interface_ShareTool&,
                                    occ::handle<Interface_Check>& ach) const
{
  bool ianul = false;
  int  i, nb = ent->NbViews();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_ViewKindEntity> tempView = ent->ViewItem(i);
    if (tempView.IsNull())
      ianul = true;
    else if (tempView->TypeNumber() == 0)
      ianul = true;
    if (ianul)
    {
      ach->AddWarning("At least one View is Null");
      break;
    }
  }
  nb = ent->NbAnnotations();
  for (i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> ann = ent->Annotation(i);
    if (ann.IsNull())
      ianul = true;
    else if (ann->TypeNumber() == 0)
      ianul = true;
    if (ianul)
    {
      ach->AddWarning("At least one Annotation is Null");
      break;
    }
  }
}

void IGESDraw_ToolDrawing::OwnDump(const occ::handle<IGESDraw_Drawing>& ent,
                                   const IGESData_IGESDumper&           dumper,
                                   Standard_OStream&                    S,
                                   const int                            level) const
{
  int sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_Drawing\n"
    << "View Entities            :\n"
    << "Transformed View Origins : "
    << "Count = " << ent->NbViews();
  switch (level)
  {
    case 4:
      S << " [ ask level > 4 for content ]\n";
      break; // Nothing to be dumped here
    case 5:  // Presently level 5 and 6 have the same Dump
      S << "\n";
      [[fallthrough]];
    case 6: {
      int I;
      int up = ent->NbViews();
      for (I = 1; I <= up; I++)
      {
        S << "\n[" << I << "] "
          << "View Entity : ";
        dumper.Dump(ent->ViewItem(I), S, sublevel);
        S << "\n"
          << "Transformed View Origin : ";
        IGESData_DumpXY(S, ent->ViewOrigin(I));
      }
    }
    break;
  }
  S << "\nAnnotation Entities : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbAnnotations(), ent->Annotation);
  S << std::endl;
}
