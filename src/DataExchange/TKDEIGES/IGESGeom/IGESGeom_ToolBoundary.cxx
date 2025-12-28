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

#include <IGESBasic_HArray1OfHArray1OfIGESEntity.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_ToolBoundary.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <stdio.h>

// MGE 30/07/98
//=================================================================================================

IGESGeom_ToolBoundary::IGESGeom_ToolBoundary() {}

//=================================================================================================

void IGESGeom_ToolBoundary::ReadOwnParams(const occ::handle<IGESGeom_Boundary>&       ent,
                                          const occ::handle<IGESData_IGESReaderData>& IR,
                                          IGESData_ParamReader&                  PR) const
{
  // MGE 30/07/98

  // bool st; //szv#4:S4163:12Mar99 not needed
  int                 num; // szv#4:S4163:12Mar99 j not needed, i moved down in `for`
  int                 tempType, tempPreference;
  occ::handle<IGESData_IGESEntity>      tempSurface;
  occ::handle<NCollection_HArray1<int>> tempSenses;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>           tempModelCurves;
  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity> tempParameterCurves;
  IGESData_Status                                aStatus;

  // szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadInteger(PR.Current(), tempType))
  {
    Message_Msg Msg122("XTSEP_122");
    PR.SendFail(Msg122);
  }
  if (!PR.ReadInteger(PR.Current(), tempPreference))
  {
    Message_Msg Msg123("XTSEP_123");
    PR.SendFail(Msg123);
  }

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, tempSurface))
  {
    Message_Msg Msg124("XTSEP_124");
    switch (aStatus)
    {
      case IGESData_ReferenceError: {
        Message_Msg Msg216("IGES_216");
        Msg124.Arg(Msg216.Value());
        PR.SendFail(Msg124);
        break;
      }
      case IGESData_EntityError: {
        Message_Msg Msg217("IGES_217");
        Msg124.Arg(Msg217.Value());
        PR.SendFail(Msg124);
        break;
      }
      default: {
      }
    }
  }

  if (PR.ReadInteger(PR.Current(), num) && (num > 0))
  {
    tempSenses          = new NCollection_HArray1<int>(1, num);
    tempModelCurves     = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num);
    tempParameterCurves = new IGESBasic_HArray1OfHArray1OfIGESEntity(1, num);
  }
  else
  {
    Message_Msg Msg126("XTSEP_126");
    PR.SendFail(Msg126);
  }

  if (!tempSenses.IsNull() && !tempModelCurves.IsNull() && !tempParameterCurves.IsNull())
  {
    // clang-format off
    for ( int i = 1;  i <= num;  i++ ) //szv#4:S4163:12Mar99 int moved in `for`
    // clang-format on
    {
      occ::handle<IGESData_IGESEntity> tempEnt;
      // st = PR.ReadEntity(IR, PR.Current(), Msg127, tempEnt); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadEntity(IR, PR.Current(), "Model Space Curves", tempEnt);
      if (PR.ReadEntity(IR, PR.Current(), aStatus, tempEnt))
        tempModelCurves->SetValue(i, tempEnt);
      else
      {
        Message_Msg Msg127("XTSEP_127");
        switch (aStatus)
        {
          case IGESData_ReferenceError: {
            Message_Msg Msg216("IGES_216");
            Msg127.Arg(Msg216.Value());
            PR.SendFail(Msg127);
            break;
          }
          case IGESData_EntityError: {
            Message_Msg Msg217("IGES_217");
            Msg127.Arg(Msg217.Value());
            PR.SendFail(Msg127);
            break;
          }
          default: {
          }
        }
      }

      int tempSense;
      // st = PR.ReadInteger(PR.Current(), Msg128, tempSense); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadInteger(PR.Current(), "Orientation flags", tempSense);
      if (PR.ReadInteger(PR.Current(), tempSense))
        tempSenses->SetValue(i, tempSense);
      else
      {
        Message_Msg Msg128("XTSEP_128");
        PR.SendFail(Msg128);
      }

      int tempCount;
      // st = PR.ReadInteger(PR.Current(), Msg129, tempCount); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadInteger(PR.Current(), "Count of Parameter Space Curves", tempCount);
      // szv#4:S4163:12Mar99 optimized
      /*
              if (st && tempCount >= 0)
            {
              occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempParCurves;
              if (tempCount > 0)
                st = PR.ReadEnts (IR,PR.CurrentList(tempCount), Msg130, tempParCurves);
                //st = PR.ReadEnts (IR,PR.CurrentList(tempCount), "Parameter Space Curves",
         tempParCurves);
      */
      /*
                {
              tempParCurves = new
                NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, tempCount);
              for ( j = 1; j <= tempCount; j++ ) {
                occ::handle<IGESData_IGESEntity> tempEnt;
                st = PR.ReadEntity(IR, PR.Current(),
                           "Parameter Space Curves", tempEnt);
                if (st) tempParCurves->SetValue(j, tempEnt);
              }
                }
      */
      /*
              tempParameterCurves->SetValue(i, tempParCurves);
            }
          if (st && tempCount < 0)
            PR.SendFail(Msg129);
      */
      if (PR.ReadInteger(PR.Current(), tempCount) && (tempCount >= 0))
      {
        occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempParCurves;
        if (tempCount > 0)
        {
          Message_Msg Msg130("XTSEP_130");
          // clang-format off
		PR.ReadEnts (IR,PR.CurrentList(tempCount), Msg130, tempParCurves); //szv#4:S4163:12Mar99 `st=` not needed
                                      // clang-format on
        }
        tempParameterCurves->SetValue(i, tempParCurves);
      }
      else
      {
        Message_Msg Msg129("XTSEP_129");
        PR.SendFail(Msg129);
      }
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent
    ->Init(tempType, tempPreference, tempSurface, tempModelCurves, tempSenses, tempParameterCurves);
}

//=================================================================================================

void IGESGeom_ToolBoundary::WriteOwnParams(const occ::handle<IGESGeom_Boundary>& ent,
                                           IGESData_IGESWriter&             IW) const
{
  int i, j, num1;
  IW.Send(ent->BoundaryType());
  IW.Send(ent->PreferenceType());
  IW.Send(ent->Surface());
  IW.Send(ent->NbModelSpaceCurves());
  for (num1 = ent->NbModelSpaceCurves(), i = 1; i <= num1; i++)
  {
    IW.Send(ent->ModelSpaceCurve(i));
    IW.Send(ent->Sense(i));
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> curves = ent->ParameterCurves(i);
    int                     nbc    = ent->NbParameterCurves(i);
    IW.Send(nbc);
    if (nbc > 0)
    {
      for (j = 1; j <= nbc; j++)
        IW.Send(curves->Value(j));
    }
  }
}

//=================================================================================================

void IGESGeom_ToolBoundary::OwnShared(const occ::handle<IGESGeom_Boundary>& ent,
                                      Interface_EntityIterator&        iter) const
{
  int i, j, num1;
  iter.GetOneItem(ent->Surface());
  for (num1 = ent->NbModelSpaceCurves(), i = 1; i <= num1; i++)
  {
    iter.GetOneItem(ent->ModelSpaceCurve(i));
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> curves = ent->ParameterCurves(i);
    if (!curves.IsNull())
    {
      int nbc = curves->Length();
      for (j = 1; j <= nbc; j++)
        iter.GetOneItem(curves->Value(j));
    }
  }
}

//=================================================================================================

void IGESGeom_ToolBoundary::OwnCopy(const occ::handle<IGESGeom_Boundary>& another,
                                    const occ::handle<IGESGeom_Boundary>& ent,
                                    Interface_CopyTool&              TC) const
{
  int i, j;
  int tempType       = another->BoundaryType();
  int tempPreference = another->PreferenceType();
  int num1           = another->NbModelSpaceCurves();

  DeclareAndCast(IGESData_IGESEntity, tempSurface, TC.Transferred(another->Surface()));

  occ::handle<NCollection_HArray1<int>>     tempSenses      = new NCollection_HArray1<int>(1, num1);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempModelCurves = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num1);
  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity> tempParameterCurves =
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1, num1);

  for (i = 1; i <= num1; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, tempEnt, TC.Transferred(another->ModelSpaceCurve(i)));
    tempModelCurves->SetValue(i, tempEnt);
    tempSenses->SetValue(i, another->Sense(i));
    int                     num2      = another->NbParameterCurves(i);
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> ParCurves = another->ParameterCurves(i);
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempParCurves;
    if (num2 > 0)
      tempParCurves = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num2);
    for (j = 1; j <= num2; j++)
    {
      DeclareAndCast(IGESData_IGESEntity, tempEnt1, TC.Transferred(ParCurves->Value(j)));
      tempParCurves->SetValue(j, tempEnt1);
    }
    tempParameterCurves->SetValue(i, tempParCurves);
  }
  ent
    ->Init(tempType, tempPreference, tempSurface, tempModelCurves, tempSenses, tempParameterCurves);
}

//=================================================================================================

bool IGESGeom_ToolBoundary::OwnCorrect(const occ::handle<IGESGeom_Boundary>& ent) const
{
  //  bool t0 = (ent->BoundaryType() == 0);
  bool res = false;
  bool r2d = false;
  int nb  = ent->NbModelSpaceCurves();
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i++)
  {
    int nbi = ent->NbParameterCurves(i);
    if (nbi == 0)
      continue;
    for (int j = 1; j <= nbi; j++)
    {
      occ::handle<IGESData_IGESEntity> c2d = ent->ParameterCurve(i, j);
      if (c2d.IsNull())
        continue;
      c2d->InitStatus(c2d->BlankStatus(), c2d->SubordinateStatus(), 5, c2d->HierarchyStatus());
      res = true;
    }
    r2d = true;
  }
  if (!r2d)
    return res;
  if (ent->BoundaryType() != 0)
    return res; // OK

  //  Remaining Boundary Type : if there are ParameterCurves, it must be valued 1
  //  We therefore reconstruct the Boundary identically, but with BoundaryType = 1

  // if type = 0, nullify all ParameterCurves
  //  -> We reconstruct, with Null ParameterCurves
  // In addition, the ParameterCurves must have their UseFlag at 5

  occ::handle<IGESBasic_HArray1OfHArray1OfIGESEntity> cv2d =
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1, nb);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> modcv  = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nb);
  occ::handle<NCollection_HArray1<int>>     senses = new NCollection_HArray1<int>(1, nb);
  for (i = 1; i <= nb; i++)
  {
    senses->SetValue(i, ent->Sense(i));
    modcv->SetValue(i, ent->ModelSpaceCurve(i));
    cv2d->SetValue(i, ent->ParameterCurves(i));
  }
  ent->Init(1, ent->PreferenceType(), ent->Surface(), modcv, senses, cv2d);
  return true;
}

//=================================================================================================

IGESData_DirChecker IGESGeom_ToolBoundary::DirChecker(
  const occ::handle<IGESGeom_Boundary>& /* ent */) const
{
  IGESData_DirChecker DC(141, 0);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefAny);
  //  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.SubordinateStatusRequired(0);
  DC.HierarchyStatusIgnored();
  return DC;
}

//=================================================================================================

void IGESGeom_ToolBoundary::OwnCheck(const occ::handle<IGESGeom_Boundary>& ent,
                                     const Interface_ShareTool&,
                                     occ::handle<Interface_Check>& ach) const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  // Message_Msg Msg122("XTSEP_122");
  // Message_Msg Msg123("XTSEP_123");
  // Message_Msg Msg125("XTSEP_125");
  // Message_Msg Msg128("XTSEP_128");
  //========================================

  if ((ent->BoundaryType() != 0) && (ent->BoundaryType() != 1))
  {
    Message_Msg Msg122("XTSEP_122");
    ach->SendFail(Msg122);
  }
  if ((ent->PreferenceType() < 0) || (ent->PreferenceType() > 3))
  {
    Message_Msg Msg123("XTSEP_123");
    ach->SendFail(Msg123);
  }

  // we should also test that, for BoundaryType = 1, the Surface is indeed
  //  Parametric ... (at least one case fails : it is Plane 108)
  if (ent->BoundaryType() == 1)
  {
    if (ent->Surface()->TypeNumber() == 108)
    {
      Message_Msg Msg125("XTSEP_125");
      ach->SendFail(Msg125);
    }
  }

  int i, num;
  for (num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++)
    if (ent->Sense(i) != 1 && ent->Sense(i) != 2)
    {
      Message_Msg Msg128("XTSEP_128");
      ach->SendFail(Msg128);
    }
  /*
    if (ent->BoundaryType() == 0)
      for ( num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++ )
        if (ent->NbParameterCurves(i) != 0)
      {
            char mess[80];
        Sprintf(mess,"Nb. Parameter Space Curve %d !=0 while Boundary Type=0",i);
        ach.SendFail(mess);
      }
  */
}

//=================================================================================================

void IGESGeom_ToolBoundary::OwnDump(const occ::handle<IGESGeom_Boundary>& ent,
                                    const IGESData_IGESDumper&       dumper,
                                    Standard_OStream&                S,
                                    const int           level) const
{
  int i, num, sublevel = (level > 4) ? 1 : 0;
  S << "IGESGeom_Boundary\n"
    << "Bounded Surface Representation Type : " << ent->BoundaryType() << "\n"
    << "Trimming Curves Representation : " << ent->PreferenceType() << "\n"
    << "Bounded Surface    : ";
  dumper.Dump(ent->Surface(), S, sublevel);
  S << "\n"
    << "Model Space Curves :\n"
    << "Orientation Flags  :\n"
    << "Parameter Curves Set : ";
  IGESData_DumpEntities(S, dumper, -level, 1, ent->NbModelSpaceCurves(), ent->ModelSpaceCurve);
  S << "\n";
  if (level > 4)
    for (num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++)
    {
      S << "[" << i << "]: "
        << "Model Space Curve : ";
      dumper.Dump(ent->ModelSpaceCurve(i), S, 1);
      S << "  Orientation Flags : " << ent->Sense(i) << "\n"
        << "  Parameter Curves : ";
      occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> curves = ent->ParameterCurves(i);
      if (!curves.IsNull())
      {
        IGESData_DumpEntities(S, dumper, level, 1, curves->Length(), curves->Value);
      }
      else
      {
        S << " List Empty";
      }
      S << "\n";
    }
  S << std::endl;
}
