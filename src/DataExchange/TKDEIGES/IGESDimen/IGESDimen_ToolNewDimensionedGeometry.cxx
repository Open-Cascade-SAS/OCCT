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
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESDimen_NewDimensionedGeometry.hxx>
#include <IGESDimen_ToolNewDimensionedGeometry.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

IGESDimen_ToolNewDimensionedGeometry::IGESDimen_ToolNewDimensionedGeometry() = default;

void IGESDimen_ToolNewDimensionedGeometry::ReadOwnParams(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  const occ::handle<IGESData_IGESReaderData>&          IR,
  IGESData_ParamReader&                                PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int                                                                i, num;
  int                                                                tempNbDimens;
  int                                                                tempDimOrientFlag;
  double                                                             tempAngle;
  occ::handle<IGESData_IGESEntity>                                   tempDimen;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempGeomEnts;
  occ::handle<NCollection_HArray1<int>>                              tempDimLocFlags;
  occ::handle<NCollection_HArray1<gp_XYZ>>                           tempPoints;

  if (PR.DefinedElseSkip())
    // clang-format off
    PR.ReadInteger(PR.Current(), "Number of Dimensions", tempNbDimens); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on
  else
    tempNbDimens = 1;

  bool st = PR.ReadInteger(PR.Current(), "Number of Geometries", num);
  if (st && num > 0)
  {
    tempGeomEnts    = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num);
    tempDimLocFlags = new NCollection_HArray1<int>(1, num);
    tempPoints      = new NCollection_HArray1<gp_XYZ>(1, num);
  }
  else
    PR.AddFail("Number of Geometries: Not Positive");

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR, PR.Current(), "Dimension Entity", tempDimen);
  PR.ReadInteger(PR.Current(), "Dimension Orientation Flag", tempDimOrientFlag);
  PR.ReadReal(PR.Current(), "Angle Value", tempAngle);

  if (!tempGeomEnts.IsNull())
    for (i = 1; i <= num; i++)
    {
      occ::handle<IGESData_IGESEntity> tempEnt;
      // szv#4:S4163:12Mar99 `st=` not needed
      // clang-format off
	PR.ReadEntity(IR, PR.Current(), "Geometry Entity", tempEnt, (i == num)); // The last one may be Null
	tempGeomEnts->SetValue(i, tempEnt);

	int tempInt;
	PR.ReadInteger(PR.Current(), "Dimension Location Flag", tempInt); //szv#4:S4163:12Mar99 `st=` not needed
      // clang-format on
      tempDimLocFlags->SetValue(i, tempInt);

      gp_XYZ tempPnt;
      PR.ReadXYZ(PR.CurrentList(1, 3), "Point", tempPnt); // szv#4:S4163:12Mar99 `st=` not needed
      tempPoints->SetValue(i, tempPnt);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNbDimens,
            tempDimen,
            tempDimOrientFlag,
            tempAngle,
            tempGeomEnts,
            tempDimLocFlags,
            tempPoints);
}

void IGESDimen_ToolNewDimensionedGeometry::WriteOwnParams(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  IGESData_IGESWriter&                                 IW) const
{
  int i, num;
  IW.Send(ent->NbDimensions());
  IW.Send(ent->NbGeometries());
  IW.Send(ent->DimensionEntity());
  IW.Send(ent->DimensionOrientationFlag());
  IW.Send(ent->AngleValue());
  for (num = ent->NbGeometries(), i = 1; i <= num; i++)
  {
    IW.Send(ent->GeometryEntity(i));
    IW.Send(ent->DimensionLocationFlag(i));
    IW.Send(ent->Point(i).X());
    IW.Send(ent->Point(i).Y());
    IW.Send(ent->Point(i).Z());
  }
}

void IGESDimen_ToolNewDimensionedGeometry::OwnShared(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  Interface_EntityIterator&                            iter) const
{
  int i, num;
  iter.GetOneItem(ent->DimensionEntity());
  for (num = ent->NbGeometries(), i = 1; i <= num; i++)
    iter.GetOneItem(ent->GeometryEntity(i));
}

void IGESDimen_ToolNewDimensionedGeometry::OwnCopy(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& another,
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  Interface_CopyTool&                                  TC) const
{
  int    num               = another->NbGeometries();
  int    tempNbDimens      = another->NbDimensions();
  int    tempDimOrientFlag = another->DimensionOrientationFlag();
  double tempAngle         = another->AngleValue();
  DeclareAndCast(IGESData_IGESEntity, tempDimen, TC.Transferred(another->DimensionEntity()));

  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempGeomEnts =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num);
  occ::handle<NCollection_HArray1<int>>    tempDimLocFlags = new NCollection_HArray1<int>(1, num);
  occ::handle<NCollection_HArray1<gp_XYZ>> tempPoints = new NCollection_HArray1<gp_XYZ>(1, num);

  for (int i = 1; i <= num; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, tempEnt, TC.Transferred(another->GeometryEntity(i)));
    tempGeomEnts->SetValue(i, tempEnt);
    tempDimLocFlags->SetValue(i, another->DimensionLocationFlag(i));
    tempPoints->SetValue(i, another->Point(i).XYZ());
  }
  ent->Init(tempNbDimens,
            tempDimen,
            tempDimOrientFlag,
            tempAngle,
            tempGeomEnts,
            tempDimLocFlags,
            tempPoints);
}

bool IGESDimen_ToolNewDimensionedGeometry::OwnCorrect(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent) const
{
  bool res = ent->HasTransf();
  if (res)
  {
    occ::handle<IGESData_TransfEntity> nultransf;
    ent->InitTransf(nultransf);
  }
  if (ent->NbDimensions() == 1)
    return res;
  //   Force NbDimensions = 1 -> reconstruct
  int                                                                nb = ent->NbGeometries();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempGeomEnts =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb);
  occ::handle<NCollection_HArray1<int>>    tempDimLocFlags = new NCollection_HArray1<int>(1, nb);
  occ::handle<NCollection_HArray1<gp_XYZ>> tempPoints      = new NCollection_HArray1<gp_XYZ>(1, nb);

  for (int i = 1; i <= nb; i++)
  {
    tempGeomEnts->SetValue(i, ent->GeometryEntity(i));
    tempDimLocFlags->SetValue(i, ent->DimensionLocationFlag(i));
    tempPoints->SetValue(i, ent->Point(i).XYZ());
  }
  ent->Init(1,
            ent->DimensionEntity(),
            ent->DimensionOrientationFlag(),
            ent->AngleValue(),
            tempGeomEnts,
            tempDimLocFlags,
            tempPoints);
  return true;
}

IGESData_DirChecker IGESDimen_ToolNewDimensionedGeometry::DirChecker(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& /* ent */) const
{
  IGESData_DirChecker DC(402, 21);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(1);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDimen_ToolNewDimensionedGeometry::OwnCheck(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& ach) const
{
  if (ent->NbDimensions() != 1)
    ach->AddFail("Number of Dimensions != 1");
  if (ent->HasTransf())
    ach->AddWarning("Transformation Matrix exists, ignored");
}

void IGESDimen_ToolNewDimensionedGeometry::OwnDump(
  const occ::handle<IGESDimen_NewDimensionedGeometry>& ent,
  const IGESData_IGESDumper&                           dumper,
  Standard_OStream&                                    S,
  const int                                            level) const
{
  int i, num, sublevel = (level > 4) ? 1 : 0;
  S << "IGESDimen_NewDimensionedGeometry\n"
    << "Number of Dimensions : " << ent->NbDimensions() << "\n"
    << "Dimension Entity : ";
  dumper.Dump(ent->DimensionEntity(), S, sublevel);
  S << "\n"
    << "Dimension Orientation Flag : " << ent->DimensionOrientationFlag() << "\n"
    << "Angle Value Flag : " << ent->AngleValue() << "\n"
    << "Geometry Entities :\n"
    << "Dimension Location Flags :\n"
    << "Points : ";
  IGESData_DumpEntities(S, dumper, -level, 1, ent->NbGeometries(), ent->GeometryEntity);
  S << "\n";
  if (level > 4)
    for (num = ent->NbGeometries(), i = 1; i <= num; i++)
    {
      S << "[" << i << "]:\n"
        << "Geometry Entity : ";
      dumper.Dump(ent->GeometryEntity(i), S, 1);
      S << "\n"
        << "Dimension Location Flag : " << ent->DimensionLocationFlag(i) << "\n"
        << "Point : ";
      IGESData_DumpXYZL(S, level, ent->Point(i), ent->Location());
    }
  S << std::endl;
}
