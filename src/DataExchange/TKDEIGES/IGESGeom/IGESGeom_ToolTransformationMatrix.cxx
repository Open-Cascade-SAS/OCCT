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
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_ToolTransformationMatrix.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Msg.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

// MGE 03/08/98
//=================================================================================================

IGESGeom_ToolTransformationMatrix::IGESGeom_ToolTransformationMatrix() {}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::ReadOwnParams(
  const occ::handle<IGESGeom_TransformationMatrix>& ent,
  const occ::handle<IGESData_IGESReaderData>& /*IR*/,
  IGESData_ParamReader& PR) const
{
  // MGE 03/08/98

  // bool st; //szv#4:S4163:12Mar99 not needed
  double                 temp;
  occ::handle<NCollection_HArray2<double>> aMatrix = new NCollection_HArray2<double>(1, 3, 1, 4);

  for (int I = 1; I <= 3; I++)
  {
    for (int J = 1; J <= 4; J++)
    {
      // st = PR.ReadReal(PR.Current(), Msg215, temp); //szv#4:S4163:12Mar99 moved in if
      // st = PR.ReadReal(PR.Current(), "Matrix Elements", temp);
      if (PR.ReadReal(PR.Current(), temp))
        aMatrix->SetValue(I, J, temp);
      else
      {
        Message_Msg Msg215("XSTEP_215");
        PR.SendFail(Msg215);
      }
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(aMatrix);
}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::WriteOwnParams(
  const occ::handle<IGESGeom_TransformationMatrix>& ent,
  IGESData_IGESWriter&                         IW) const
{
  for (int I = 1; I <= 3; I++)
  {
    for (int J = 1; J <= 4; J++)
    {
      IW.Send(ent->Data(I, J));
    }
  }
}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::OwnShared(
  const occ::handle<IGESGeom_TransformationMatrix>& /*ent*/,
  Interface_EntityIterator& /*iter*/) const
{
}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::OwnCopy(
  const occ::handle<IGESGeom_TransformationMatrix>& another,
  const occ::handle<IGESGeom_TransformationMatrix>& ent,
  Interface_CopyTool& /*TC*/) const
{
  occ::handle<NCollection_HArray2<double>> data = new NCollection_HArray2<double>(1, 3, 1, 4);
  for (int I = 1; I <= 3; I++)
  {
    for (int J = 1; J <= 4; J++)
    {
      data->SetValue(I, J, another->Data(I, J));
    }
  }

  ent->Init(data);
  ent->SetFormNumber(another->FormNumber());
}

//=================================================================================================

bool IGESGeom_ToolTransformationMatrix::OwnCorrect(
  const occ::handle<IGESGeom_TransformationMatrix>& ent) const
{
  if (ent->FormNumber() > 1)
    return false;
  int cfn = (ent->Value().IsNegative() ? 1 : 0);
  if (cfn == ent->FormNumber())
    return false;
  ent->SetFormNumber(cfn);
  return true;
}

//=================================================================================================

IGESData_DirChecker IGESGeom_ToolTransformationMatrix::DirChecker(
  const occ::handle<IGESGeom_TransformationMatrix>& /*ent*/) const
{
  IGESData_DirChecker DC(124);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  //  DC.LineWeight(IGESData_DefAny);
  DC.Color(IGESData_DefAny);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();

  return DC;
}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::OwnCheck(const occ::handle<IGESGeom_TransformationMatrix>& ent,
                                                 const Interface_ShareTool&,
                                                 occ::handle<Interface_Check>& ach) const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  // Message_Msg Msg71("XSTEP_71");
  //========================================

  int form = ent->FormNumber();
  if ((form != 0) && (form != 1) && ((form < 10) || (form > 12)))
  {
    Message_Msg Msg71("XSTEP_71");
    ach->SendFail(Msg71);
  }
  // These messages are transferred in the translation procedure
  /*
    if (form > 1) return;    // what follows : orthogonal matrix
    if (form == 0 && ent->Value().IsNegative())
      ach.AddFail("For Form 0, Determinant is not Positive");
    else if (form == 1 && !ent->Value().IsNegative())
      ach.AddFail("For Form 1, Determinant is not Negative");

    double p12 = ent->Data(1,1)*ent->Data(2,1) +
      ent->Data(1,2)*ent->Data(2,2) + ent->Data(1,3)*ent->Data(2,3);
    double p13 = ent->Data(1,1)*ent->Data(3,1) +
      ent->Data(1,2)*ent->Data(3,2) + ent->Data(1,3)*ent->Data(3,3);
    double p23 = ent->Data(2,1)*ent->Data(3,1) +
      ent->Data(2,2)*ent->Data(3,2) + ent->Data(2,3)*ent->Data(3,3);
    double ep = 1.e-05;  // ?? Tolorance des tests ?
    if (p12 < -ep || p12 > ep || p13 < -ep || p13 > ep || p23 < -ep || p23 > ep)
      ach.AddFail("Matrix is not orthogonal");
  */
}

//=================================================================================================

void IGESGeom_ToolTransformationMatrix::OwnDump(const occ::handle<IGESGeom_TransformationMatrix>& ent,
                                                const IGESData_IGESDumper& /*dumper*/,
                                                Standard_OStream& S,
                                                const int /*level*/) const
{
  S << "IGESGeom_TransformationMatrix\n"
    << "| R11, R12, R13, T1 |       " << ent->Data(1, 1) << ", " << ent->Data(1, 2) << ", "
    << ent->Data(1, 3) << ", " << ent->Data(1, 4) << "\n"
    << "| R21, R22, R23, T2 |       " << ent->Data(2, 1) << ", " << ent->Data(2, 2) << ", "
    << ent->Data(2, 3) << ", " << ent->Data(2, 4) << "\n"
    << "| R31, R32, R33, T3 |       " << ent->Data(3, 1) << ", " << ent->Data(3, 2) << ", "
    << ent->Data(3, 3) << ", " << ent->Data(3, 4) << "\n";

  switch (ent->FormNumber())
  {
    case 0:
      S << "-- Direct Orthogonal Matrix" << std::endl;
      break;
    case 1:
      S << "-- Reverse Orthogonal Matrix" << std::endl;
      break;
    case 10:
      S << "-- Cartesien Coordinate System" << std::endl;
      break;
    case 11:
      S << "-- Cylindrical Coordinate System" << std::endl;
      break;
    case 12:
      S << "-- Spherical Coordinate System" << std::endl;
      break;
    default:
      S << "--  (Incorrect Form Number)" << std::endl;
      break;
  }
}
