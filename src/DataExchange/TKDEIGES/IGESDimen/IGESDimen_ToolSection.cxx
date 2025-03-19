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
#include <IGESDimen_Section.hxx>
#include <IGESDimen_ToolSection.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXY.hxx>

IGESDimen_ToolSection::IGESDimen_ToolSection() {}

void IGESDimen_ToolSection::ReadOwnParams(const Handle(IGESDimen_Section)& ent,
                                          const Handle(IGESData_IGESReaderData)& /* IR */,
                                          IGESData_ParamReader& PR) const
{
  // Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  Standard_Integer           datatype;
  Standard_Real              zDisplacement;
  Standard_Integer           nbval;
  Handle(TColgp_HArray1OfXY) dataPoints;

  // clang-format off
  PR.ReadInteger(PR.Current(), "Interpretation Flag", datatype); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of data points", nbval);
  if (st && nbval > 0)
    dataPoints = new TColgp_HArray1OfXY(1, nbval);
  else  PR.AddFail("Number of data points: Not Positive");

  PR.ReadReal(PR.Current(), "Common Z Displacement", zDisplacement); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  if (!dataPoints.IsNull())
    for (Standard_Integer i = 1; i <= nbval; i++)
    {
      gp_XY tempXY;
      PR.ReadXY(PR.CurrentList(1, 2), "Data Points", tempXY); // szv#4:S4163:12Mar99 `st=` not
                                                              // needed
      dataPoints->SetValue(i, tempXY);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(datatype, zDisplacement, dataPoints);
}

void IGESDimen_ToolSection::WriteOwnParams(const Handle(IGESDimen_Section)& ent,
                                           IGESData_IGESWriter&             IW) const
{
  Standard_Integer upper = ent->NbPoints();
  IW.Send(ent->Datatype());
  IW.Send(upper);
  IW.Send(ent->ZDisplacement());
  for (Standard_Integer i = 1; i <= upper; i++)
  {
    IW.Send((ent->Point(i)).X());
    IW.Send((ent->Point(i)).Y());
  }
}

void IGESDimen_ToolSection::OwnShared(const Handle(IGESDimen_Section)& /* ent */,
                                      Interface_EntityIterator& /* iter */) const
{
}

void IGESDimen_ToolSection::OwnCopy(const Handle(IGESDimen_Section)& another,
                                    const Handle(IGESDimen_Section)& ent,
                                    Interface_CopyTool& /* TC */) const
{
  Standard_Integer datatype      = another->Datatype();
  Standard_Integer nbval         = another->NbPoints();
  Standard_Real    zDisplacement = another->ZDisplacement();

  Handle(TColgp_HArray1OfXY) dataPoints = new TColgp_HArray1OfXY(1, nbval);

  for (Standard_Integer i = 1; i <= nbval; i++)
  {
    gp_Pnt tempPnt = (another->Point(i));
    gp_XY  tempPnt2d(tempPnt.X(), tempPnt.Y());
    dataPoints->SetValue(i, tempPnt2d);
  }
  ent->Init(datatype, zDisplacement, dataPoints);
  ent->SetFormNumber(another->FormNumber());
}

Standard_Boolean IGESDimen_ToolSection::OwnCorrect(const Handle(IGESDimen_Section)& ent) const
{
  Standard_Boolean res = (ent->RankLineFont() != 1);
  if (res)
  {
    Handle(IGESData_LineFontEntity) nulfont;
    ent->InitLineFont(nulfont, 1);
  }
  if (ent->Datatype() == 1)
    return res;
  //  Forcer DataType = 1 -> reconstruire
  Standard_Integer nb = ent->NbPoints();
  if (nb == 0)
    return Standard_False; // rien pu faire (est-ce possible ?)
  Handle(TColgp_HArray1OfXY) pts = new TColgp_HArray1OfXY(1, nb);
  for (Standard_Integer i = 1; i <= nb; i++)
    pts->SetValue(i, gp_XY(ent->Point(i).X(), ent->Point(i).Y()));
  ent->Init(1, ent->ZDisplacement(), pts);
  return Standard_True;
}

IGESData_DirChecker IGESDimen_ToolSection::DirChecker(
  const Handle(IGESDimen_Section)& /* ent */) const
{
  IGESData_DirChecker DC(106, 31, 38);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefValue);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolSection::OwnCheck(const Handle(IGESDimen_Section)& ent,
                                     const Interface_ShareTool&,
                                     Handle(Interface_Check)& ach) const
{
  if (ent->RankLineFont() != 1)
    ach->AddFail("Line Font Pattern != 1");
  if (ent->Datatype() != 1)
    ach->AddFail("Interpretation Flag != 1");
  if (ent->NbPoints() % 2 != 0)
    ach->AddFail("Number of data points is not even");
}

void IGESDimen_ToolSection::OwnDump(const Handle(IGESDimen_Section)& ent,
                                    const IGESData_IGESDumper& /* dumper */,
                                    Standard_OStream&      S,
                                    const Standard_Integer level) const
{
  S << "IGESDimen_Section\n"
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
