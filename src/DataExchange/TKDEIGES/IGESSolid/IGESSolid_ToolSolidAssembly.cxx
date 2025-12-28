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

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_SolidAssembly.hxx>
#include <IGESSolid_ToolSolidAssembly.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>

IGESSolid_ToolSolidAssembly::IGESSolid_ToolSolidAssembly() {}

void IGESSolid_ToolSolidAssembly::ReadOwnParams(const occ::handle<IGESSolid_SolidAssembly>& ent,
                                                const occ::handle<IGESData_IGESReaderData>& IR,
                                                IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int nbitems; // szv#4:S4163:12Mar99 `i` moved in for
  // occ::handle<IGESData_IGESEntity> anent; //szv#4:S4163:12Mar99 moved down
  // occ::handle<IGESGeom_TransformationMatrix> amatr; //szv#4:S4163:12Mar99 moved down
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>           tempItems;
  occ::handle<NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>> tempMatrices;

  bool st = PR.ReadInteger(PR.Current(), "Number of Items", nbitems);
  if (st && nbitems > 0)
  {
    tempItems    = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbitems);
    tempMatrices = new NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>(1, nbitems);

    occ::handle<IGESData_IGESEntity> anent;
    int                              i; // svv Jan 10 2000 : porting on DEC
    for (i = 1; i <= nbitems; i++)
    {
      // st = PR.ReadEntity(IR,PR.Current(), "Solid assembly items", anent); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadEntity(IR, PR.Current(), "Solid assembly items", anent))
        tempItems->SetValue(i, anent);
    }

    occ::handle<IGESGeom_TransformationMatrix> amatr;
    for (i = 1; i <= nbitems; i++)
    {
      // st = PR.ReadEntity(IR,PR.Current(), "Matrices",
      // STANDARD_TYPE(IGESGeom_TransformationMatrix),
      // amatr, true); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Matrices",
                        STANDARD_TYPE(IGESGeom_TransformationMatrix),
                        amatr,
                        true))
        tempMatrices->SetValue(i, amatr);
    }
  }
  else
    PR.AddFail("Number of Items : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempItems, tempMatrices);
}

void IGESSolid_ToolSolidAssembly::WriteOwnParams(const occ::handle<IGESSolid_SolidAssembly>& ent,
                                                 IGESData_IGESWriter& IW) const
{
  int nbitems = ent->NbItems();
  int i;

  IW.Send(nbitems);
  for (i = 1; i <= nbitems; i++)
    IW.Send(ent->Item(i));
  for (i = 1; i <= nbitems; i++)
    IW.Send(ent->TransfMatrix(i));
}

void IGESSolid_ToolSolidAssembly::OwnShared(const occ::handle<IGESSolid_SolidAssembly>& ent,
                                            Interface_EntityIterator&                   iter) const
{
  int nbitems = ent->NbItems();
  int i;
  for (i = 1; i <= nbitems; i++)
    iter.GetOneItem(ent->Item(i));
  for (i = 1; i <= nbitems; i++)
    iter.GetOneItem(ent->TransfMatrix(i));
}

void IGESSolid_ToolSolidAssembly::OwnCopy(const occ::handle<IGESSolid_SolidAssembly>& another,
                                          const occ::handle<IGESSolid_SolidAssembly>& ent,
                                          Interface_CopyTool&                         TC) const
{
  int                              nbitems, i;
  occ::handle<IGESData_IGESEntity> anent;

  nbitems = another->NbItems();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempItems =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbitems);
  occ::handle<NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>> tempMatrices =
    new NCollection_HArray1<occ::handle<IGESGeom_TransformationMatrix>>(1, nbitems);

  for (i = 1; i <= nbitems; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, localent, TC.Transferred(another->Item(i)));
    tempItems->SetValue(i, localent);
  }
  for (i = 1; i <= nbitems; i++)
  {
    DeclareAndCast(IGESGeom_TransformationMatrix,
                   newlocalent,
                   TC.Transferred(another->TransfMatrix(i)));
    tempMatrices->SetValue(i, newlocalent);
  }

  ent->Init(tempItems, tempMatrices);
}

IGESData_DirChecker IGESSolid_ToolSolidAssembly::DirChecker(
  const occ::handle<IGESSolid_SolidAssembly>& /* ent */) const
{
  IGESData_DirChecker DC(184, 0, 1);

  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired(2);
  DC.GraphicsIgnored(1);
  return DC;
}

void IGESSolid_ToolSolidAssembly::OwnCheck(const occ::handle<IGESSolid_SolidAssembly>& /* ent */,
                                           const Interface_ShareTool&,
                                           occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESSolid_ToolSolidAssembly::OwnDump(const occ::handle<IGESSolid_SolidAssembly>& ent,
                                          const IGESData_IGESDumper&                  dumper,
                                          Standard_OStream&                           S,
                                          const int                                   level) const
{
  //  int upper = ent->NbItems();
  S << "IGESSolid_SolidAssembly\n"
    << "Items : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbItems(), ent->Item);
  S << "\n"
    << "Matrices : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbItems(), ent->TransfMatrix);
  S << std::endl;
}
