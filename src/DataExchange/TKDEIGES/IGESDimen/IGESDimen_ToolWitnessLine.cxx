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

#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_ToolWitnessLine.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IGESDimen_ToolWitnessLine::IGESDimen_ToolWitnessLine() = default;

void IGESDimen_ToolWitnessLine::ReadOwnParams(const occ::handle<IGESDimen_WitnessLine>& ent,
                                              const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                              IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down

  int                                     datatype;
  double                                  zDisplacement;
  int                                     nbval;
  occ::handle<NCollection_HArray1<gp_XY>> dataPoints;

  // clang-format off
  PR.ReadInteger(PR.Current(), "Interpretation Flag", datatype); //szv#4:S4163:12Mar99 `st=` not needed

  bool st = PR.ReadInteger(PR.Current(), "Number of data points", nbval);
  if (st && nbval > 0)
    dataPoints = new NCollection_HArray1<gp_XY>(1, nbval);
  else  PR.AddFail("Number of data points: Not Positive");

  PR.ReadReal(PR.Current(), "Common Z Displacement", zDisplacement); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  if (!dataPoints.IsNull())
    for (int i = 1; i <= nbval; i++)
    {
      gp_XY tempXY;
      PR.ReadXY(PR.CurrentList(1, 2), "Data Points", tempXY); // szv#4:S4163:12Mar99 `st=` not
                                                              // needed
      dataPoints->SetValue(i, tempXY);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(datatype, zDisplacement, dataPoints);
}

void IGESDimen_ToolWitnessLine::WriteOwnParams(const occ::handle<IGESDimen_WitnessLine>& ent,
                                               IGESData_IGESWriter&                      IW) const
{
  int upper = ent->NbPoints();
  IW.Send(ent->Datatype());
  IW.Send(upper);
  IW.Send(ent->ZDisplacement());
  for (int i = 1; i <= upper; i++)
  {
    IW.Send((ent->Point(i)).X());
    IW.Send((ent->Point(i)).Y());
  }
}

void IGESDimen_ToolWitnessLine::OwnShared(const occ::handle<IGESDimen_WitnessLine>& /* ent */,
                                          Interface_EntityIterator& /* iter */) const
{
}

void IGESDimen_ToolWitnessLine::OwnCopy(const occ::handle<IGESDimen_WitnessLine>& another,
                                        const occ::handle<IGESDimen_WitnessLine>& ent,
                                        Interface_CopyTool& /* TC */) const
{
  int    datatype      = another->Datatype();
  int    nbval         = another->NbPoints();
  double zDisplacement = another->ZDisplacement();

  occ::handle<NCollection_HArray1<gp_XY>> dataPoints = new NCollection_HArray1<gp_XY>(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    gp_Pnt tempPnt = (another->Point(i));
    gp_XY  tempPnt2d(tempPnt.X(), tempPnt.Y());
    dataPoints->SetValue(i, tempPnt2d);
  }
  ent->Init(datatype, zDisplacement, dataPoints);
}

bool IGESDimen_ToolWitnessLine::OwnCorrect(const occ::handle<IGESDimen_WitnessLine>& ent) const
{
  bool res = (ent->RankLineFont() != 1);
  if (res)
  {
    occ::handle<IGESData_LineFontEntity> nulfont;
    ent->InitLineFont(nulfont, 1);
  }
  if (ent->Datatype() == 1)
    return res;
  //  Force DataType = 1 -> reconstruct
  int nb = ent->NbPoints();
  if (nb == 0)
    return false; // nothing could be done (is this possible?)
  occ::handle<NCollection_HArray1<gp_XY>> pts = new NCollection_HArray1<gp_XY>(1, nb);
  for (int i = 1; i <= nb; i++)
    pts->SetValue(i, gp_XY(ent->Point(i).X(), ent->Point(i).Y()));
  ent->Init(1, ent->ZDisplacement(), pts);
  return true;
}

IGESData_DirChecker IGESDimen_ToolWitnessLine::DirChecker(
  const occ::handle<IGESDimen_WitnessLine>& /* ent */) const
{
  IGESData_DirChecker DC(106, 40);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolWitnessLine::OwnCheck(const occ::handle<IGESDimen_WitnessLine>& ent,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& ach) const
{
  if (ent->RankLineFont() != 1)
    ach->AddFail("Line Font Pattern != 1");
  if (ent->Datatype() != 1)
    ach->AddFail("Interpretation Flag != 1");
  if (ent->NbPoints() < 3)
    ach->AddFail("Number of data points < 3");
  if (ent->NbPoints() % 2 == 0)
    ach->AddFail("Number of data points is not odd");
}

void IGESDimen_ToolWitnessLine::OwnDump(const occ::handle<IGESDimen_WitnessLine>& ent,
                                        const IGESData_IGESDumper& /* dumper */,
                                        Standard_OStream& S,
                                        const int         level) const
{
  S << "IGESDimen_WitnessLine\n"
    << "Data Type   : " << ent->Datatype() << "  "
    << "Number of Data Points : " << ent->NbPoints() << "  "
    << "Common Z displacement : " << ent->ZDisplacement() << "\n"
    << "Data Points : ";
  IGESData_DumpListXYLZ(S,
                        level,
                        1,
                        ent->NbPoints(),
                        ent->Point,
                        ent->Location(),
                        ent->ZDisplacement());
  S << std::endl;
}
