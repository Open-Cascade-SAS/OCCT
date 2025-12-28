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
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolLeaderArrow.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IGESDimen_ToolLeaderArrow::IGESDimen_ToolLeaderArrow() = default;

void IGESDimen_ToolLeaderArrow::ReadOwnParams(const occ::handle<IGESDimen_LeaderArrow>& ent,
                                              const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                              IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down

  double                                  arrowHeadHeight;
  double                                  arrowHeadWidth;
  double                                  zDepth;
  gp_XY                                   arrowHead;
  occ::handle<NCollection_HArray1<gp_XY>> segmentTails;
  int                                     nbval;

  bool st = PR.ReadInteger(PR.Current(), "Count of Segments", nbval);
  if (st && nbval > 0)
    segmentTails = new NCollection_HArray1<gp_XY>(1, nbval);
  else
    PR.AddFail("Count of Segments: Not Positive");

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), "Arrow Head Height", arrowHeadHeight);
  PR.ReadReal(PR.Current(), "Arrow Head Width", arrowHeadWidth);
  PR.ReadReal(PR.Current(), "Z Depth", zDepth);
  PR.ReadXY(PR.CurrentList(1, 2), "Arrow Head Position", arrowHead);

  if (!segmentTails.IsNull())
  {
    for (int i = 1; i <= nbval; i++)
    {
      gp_XY tempXY;
      // st = PR.ReadXY(PR.CurrentList(1, 2), "Segment Co-ords.", tempXY); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadXY(PR.CurrentList(1, 2), "Segment Co-ords.", tempXY))
        segmentTails->SetValue(i, tempXY);
    }
    DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
    ent->Init(arrowHeadHeight, arrowHeadWidth, zDepth, arrowHead, segmentTails);
  } // segmentTails.IsNull() crash in ent->Init( ...
}

void IGESDimen_ToolLeaderArrow::WriteOwnParams(const occ::handle<IGESDimen_LeaderArrow>& ent,
                                               IGESData_IGESWriter&                      IW) const
{
  int upper = ent->NbSegments();
  IW.Send(upper);
  IW.Send(ent->ArrowHeadHeight());
  IW.Send(ent->ArrowHeadWidth());
  IW.Send(ent->ZDepth());
  IW.Send(ent->ArrowHead().X());
  IW.Send(ent->ArrowHead().Y());
  for (int i = 1; i <= upper; i++)
  {
    IW.Send((ent->SegmentTail(i)).X());
    IW.Send((ent->SegmentTail(i)).Y());
  }
}

void IGESDimen_ToolLeaderArrow::OwnShared(const occ::handle<IGESDimen_LeaderArrow>& /* ent */,
                                          Interface_EntityIterator& /* iter */) const
{
}

void IGESDimen_ToolLeaderArrow::OwnCopy(const occ::handle<IGESDimen_LeaderArrow>& another,
                                        const occ::handle<IGESDimen_LeaderArrow>& ent,
                                        Interface_CopyTool& /* TC */) const
{
  int    nbval           = another->NbSegments();
  double arrowHeadHeight = another->ArrowHeadHeight();
  double arrowHeadWidth  = another->ArrowHeadWidth();
  double zDepth          = another->ZDepth();
  gp_XY  arrowHead       = another->ArrowHead().XY();

  occ::handle<NCollection_HArray1<gp_XY>> segmentTails = new NCollection_HArray1<gp_XY>(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    segmentTails->SetValue(i, (another->SegmentTail(i)).XY());
  }

  ent->Init(arrowHeadHeight, arrowHeadWidth, zDepth, arrowHead, segmentTails);
  ent->SetFormNumber(another->FormNumber());
}

IGESData_DirChecker IGESDimen_ToolLeaderArrow::DirChecker(
  const occ::handle<IGESDimen_LeaderArrow>& /* ent */) const
{
  IGESData_DirChecker DC(214, 1, 12);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolLeaderArrow::OwnCheck(const occ::handle<IGESDimen_LeaderArrow>& /* ent */,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDimen_ToolLeaderArrow::OwnDump(const occ::handle<IGESDimen_LeaderArrow>& ent,
                                        const IGESData_IGESDumper& /* dumper */,
                                        Standard_OStream& S,
                                        const int         level) const
{
  S << "IGESDimen_LeaderArrow\n"
    << "Number of Segments : " << ent->NbSegments() << "\n"
    << "Arrowhead Height   : " << ent->ArrowHeadHeight() << "\n"
    << "Arrowhead Width    : " << ent->ArrowHeadWidth() << "\n"
    << "Z depth            : " << ent->ZDepth() << "\n"
    << "Arrowhead co-ords  : ";
  IGESData_DumpXYLZ(S, level, ent->ArrowHead(), ent->Location(), ent->ZDepth());
  S << "\nSegment Tails : ";
  IGESData_DumpListXYLZ(S,
                        level,
                        1,
                        ent->NbSegments(),
                        ent->SegmentTail,
                        ent->Location(),
                        ent->ZDepth());
  S << std::endl;
}
