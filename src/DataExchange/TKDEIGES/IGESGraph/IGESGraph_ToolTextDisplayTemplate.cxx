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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESGraph_ToolTextDisplayTemplate.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>

IGESGraph_ToolTextDisplayTemplate::IGESGraph_ToolTextDisplayTemplate() = default;

void IGESGraph_ToolTextDisplayTemplate::ReadOwnParams(
  const occ::handle<IGESGraph_TextDisplayTemplate>& ent,
  const occ::handle<IGESData_IGESReaderData>&       IR,
  IGESData_ParamReader&                             PR) const
{
  // bool              st; //szv#4:S4163:12Mar99 not needed

  double                             boxWidth;
  double                             boxHeight;
  int                                fontCode;
  occ::handle<IGESGraph_TextFontDef> fontEnt;
  double                             slantAngle;
  double                             rotationAngle;
  int                                mirrorFlag;
  int                                rotateFlag;
  gp_XYZ                             corner;

  // Reading boxWidth(Real)
  // clang-format off
  PR.ReadReal(PR.Current(), "Character box width", boxWidth); //szv#4:S4163:12Mar99 `st=` not needed

  // Reading boxHeight(Real)
  PR.ReadReal(PR.Current(), "Character box height", boxHeight); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  int curnum = PR.CurrentNumber();
  if (PR.DefinedElseSkip())
  {
    // Reading fontCode(Integer, must be positive)
    PR.ReadInteger(PR.Current(), "Font Code", fontCode); // szv#4:S4163:12Mar99 `st=` not needed
    // Reading fontEnt(TextFontDef) ?
    if (fontCode < 0)
    {
      fontEnt = GetCasted(IGESGraph_TextFontDef, PR.ParamEntity(IR, curnum));
      if (fontEnt.IsNull())
        PR.AddFail("Font Entity : incorrect reference");
    }
  }
  else
    fontCode = 1; // Default Value

  if (PR.DefinedElseSkip())
    // Reading slantAngle(Real)
    PR.ReadReal(PR.Current(), "Slant Angle", slantAngle); // szv#4:S4163:12Mar99 `st=` not needed
  else
    slantAngle = M_PI / 2.0; // Default Value

  // Reading rotationAngle(Real)
  // clang-format off
  PR.ReadReal (PR.Current(), "Rotation Angle", rotationAngle); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  // Reading mirrorFlag(Integer)
  PR.ReadInteger(PR.Current(), "Mirror Flag", mirrorFlag); // szv#4:S4163:12Mar99 `st=` not needed

  // Reading rotateFlag(Integer)
  PR.ReadInteger(PR.Current(), "Rotate Flag", rotateFlag); // szv#4:S4163:12Mar99 `st=` not needed

  // Reading corner(XYZ)
  PR.ReadXYZ(PR.CurrentList(1, 3),
             "Lower left coordinates/Increments",
             corner); // szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(boxWidth,
            boxHeight,
            fontCode,
            fontEnt,
            slantAngle,
            rotationAngle,
            mirrorFlag,
            rotateFlag,
            corner);
}

void IGESGraph_ToolTextDisplayTemplate::WriteOwnParams(
  const occ::handle<IGESGraph_TextDisplayTemplate>& ent,
  IGESData_IGESWriter&                              IW) const
{
  IW.Send(ent->BoxWidth());
  IW.Send(ent->BoxHeight());

  if (ent->IsFontEntity())
    IW.Send(ent->FontEntity(), true); // negative
  else
    IW.Send(ent->FontCode());

  IW.Send(ent->SlantAngle());
  IW.Send(ent->RotationAngle());
  IW.Send(ent->MirrorFlag());
  IW.Send(ent->RotateFlag());
  IW.Send(ent->StartingCorner().X());
  IW.Send(ent->StartingCorner().Y());
  IW.Send(ent->StartingCorner().Z());
}

void IGESGraph_ToolTextDisplayTemplate::OwnShared(
  const occ::handle<IGESGraph_TextDisplayTemplate>& ent,
  Interface_EntityIterator&                         iter) const
{
  if (ent->IsFontEntity())
    iter.GetOneItem(ent->FontEntity());
}

void IGESGraph_ToolTextDisplayTemplate::OwnCopy(
  const occ::handle<IGESGraph_TextDisplayTemplate>& another,
  const occ::handle<IGESGraph_TextDisplayTemplate>& ent,
  Interface_CopyTool&                               TC) const
{
  double                             boxWidth;
  double                             boxHeight;
  int                                fontCode = 0;
  occ::handle<IGESGraph_TextFontDef> fontEntity;
  double                             slantAngle;
  double                             rotationAngle;
  int                                mirrorFlag;
  int                                rotateFlag;
  gp_XYZ                             corner;

  boxWidth  = another->BoxWidth();
  boxHeight = another->BoxHeight();

  if (another->IsFontEntity())
    fontEntity = occ::down_cast<IGESGraph_TextFontDef>(TC.Transferred(another->FontEntity()));
  else
    fontCode = another->FontCode();

  slantAngle    = another->SlantAngle();
  rotationAngle = another->RotationAngle();
  mirrorFlag    = another->MirrorFlag();
  rotateFlag    = another->RotateFlag();
  corner        = another->StartingCorner().XYZ();

  ent->Init(boxWidth,
            boxHeight,
            fontCode,
            fontEntity,
            slantAngle,
            rotationAngle,
            mirrorFlag,
            rotateFlag,
            corner);
}

IGESData_DirChecker IGESGraph_ToolTextDisplayTemplate::DirChecker(
  const occ::handle<IGESGraph_TextDisplayTemplate>& /*ent*/) const
{
  IGESData_DirChecker DC(312, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefAny);
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusRequired(0);
  return DC;
}

void IGESGraph_ToolTextDisplayTemplate::OwnCheck(
  const occ::handle<IGESGraph_TextDisplayTemplate>& /*ent*/,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /*ach*/) const
{
}

void IGESGraph_ToolTextDisplayTemplate::OwnDump(
  const occ::handle<IGESGraph_TextDisplayTemplate>& ent,
  const IGESData_IGESDumper&                        dumper,
  Standard_OStream&                                 S,
  const int                                         level) const
{
  int sublevel = (level <= 4) ? 0 : 1;

  S << "IGESGraph_TextDisplayTemplate\n"
    << "Character box width  : " << ent->BoxWidth() << "  "
    << "Character box height : " << ent->BoxHeight() << "\n";
  if (ent->IsFontEntity())
  {
    S << "Font Entity : ";
    dumper.Dump(ent->FontEntity(), S, sublevel);
  }
  else
    S << "Font code : " << ent->FontCode();

  S << "\n"
    << "Slant angle    : " << ent->SlantAngle() << "  "
    << "Rotation angle : " << ent->RotationAngle() << "\n"
    << "Mirror flag    : " << ent->MirrorFlag() << "  "
    << "Rotate flag    : " << ent->RotateFlag() << "\n";
  if (ent->FormNumber() == 0)
    S << "Lower Left Corner coordinates : ";
  else
    S << "Increments from coordinates : ";
  IGESData_DumpXYZL(S, level, ent->StartingCorner(), ent->Location());
  S << std::endl;
}
