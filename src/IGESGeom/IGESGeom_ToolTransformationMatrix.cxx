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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_ToolTransformationMatrix.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray2OfReal.hxx>

// MGE 03/08/98
//=======================================================================
//function : IGESGeom_ToolTransformationMatrix
//purpose  : 
//=======================================================================
IGESGeom_ToolTransformationMatrix::IGESGeom_ToolTransformationMatrix ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolTransformationMatrix::ReadOwnParams
  (const Handle(IGESGeom_TransformationMatrix)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  // MGE 03/08/98

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Real    temp;
  Handle(TColStd_HArray2OfReal) aMatrix = new TColStd_HArray2OfReal(1,3,1,4);

  for (Standard_Integer I = 1; I <= 3; I++) {
    for (Standard_Integer J = 1; J <= 4; J++) {
      //st = PR.ReadReal(PR.Current(), Msg215, temp); //szv#4:S4163:12Mar99 moved in if
      //st = PR.ReadReal(PR.Current(), "Matrix Elements", temp);
      if (PR.ReadReal(PR.Current(), temp))
	aMatrix->SetValue(I, J, temp);
      else{
	Message_Msg Msg215("XSTEP_215");
	PR.SendFail(Msg215);
      }
    }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aMatrix);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolTransformationMatrix::WriteOwnParams
  (const Handle(IGESGeom_TransformationMatrix)& ent, IGESData_IGESWriter& IW) const
{
  for (Standard_Integer I = 1; I <= 3; I++) {
    for (Standard_Integer J = 1; J <= 4; J++) {
      IW.Send(ent->Data(I, J));
    }
  }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void  IGESGeom_ToolTransformationMatrix::OwnShared
  (const Handle(IGESGeom_TransformationMatrix)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolTransformationMatrix::OwnCopy
  (const Handle(IGESGeom_TransformationMatrix)& another,
   const Handle(IGESGeom_TransformationMatrix)& ent, Interface_CopyTool& /*TC*/) const
{
  Handle(TColStd_HArray2OfReal) data = new TColStd_HArray2OfReal(1, 3, 1, 4);
  for (Standard_Integer I = 1; I <= 3;I++) {
    for (Standard_Integer J = 1; J <= 4; J++) {
      data->SetValue(I, J, another->Data(I, J));
    }
  }

  ent->Init(data);
  ent->SetFormNumber(another->FormNumber());
}


//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESGeom_ToolTransformationMatrix::OwnCorrect
  (const Handle(IGESGeom_TransformationMatrix)& ent) const
{
  if (ent->FormNumber() > 1) return Standard_False;
  Standard_Integer cfn = (ent->Value().IsNegative() ? 1 : 0);
  if (cfn == ent->FormNumber()) return Standard_False;
  ent->SetFormNumber(cfn);
  return Standard_True;
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolTransformationMatrix::DirChecker
  (const Handle(IGESGeom_TransformationMatrix)& /*ent*/)   const
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


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolTransformationMatrix::OwnCheck
  (const Handle(IGESGeom_TransformationMatrix)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  // MGE 03/08/98
  // Building of messages
  //========================================
  //Message_Msg Msg71("XSTEP_71");
  //========================================

  Standard_Integer form = ent->FormNumber();
  if ((form != 0) && (form != 1) && ((form < 10) || (form > 12))) {
    Message_Msg Msg71("XSTEP_71");
    ach->SendFail(Msg71);
  }
// Theses messages are transfered in the translation procedure
/*
  if (form > 1) return;    // ce qui suit : matrice orthogonale
  if (form == 0 && ent->Value().IsNegative())
    ach.AddFail("For Form 0, Determinant is not Positive");
  else if (form == 1 && !ent->Value().IsNegative())
    ach.AddFail("For Form 1, Determinant is not Negative");

  Standard_Real p12 = ent->Data(1,1)*ent->Data(2,1) +
    ent->Data(1,2)*ent->Data(2,2) + ent->Data(1,3)*ent->Data(2,3);
  Standard_Real p13 = ent->Data(1,1)*ent->Data(3,1) +
    ent->Data(1,2)*ent->Data(3,2) + ent->Data(1,3)*ent->Data(3,3);
  Standard_Real p23 = ent->Data(2,1)*ent->Data(3,1) +
    ent->Data(2,2)*ent->Data(3,2) + ent->Data(2,3)*ent->Data(3,3);
  Standard_Real ep = 1.e-05;  // ?? Tolorance des tests ?
  if (p12 < -ep || p12 > ep || p13 < -ep || p13 > ep || p23 < -ep || p23 > ep)
    ach.AddFail("Matrix is not orthogonal");
*/
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolTransformationMatrix::OwnDump
  (const Handle(IGESGeom_TransformationMatrix)& ent, const IGESData_IGESDumper& /*dumper*/,
  const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGeom_TransformationMatrix" << endl;

  S << "| R11, R12, R13, T1 |       "
    << ent->Data(1, 1) << ", " << ent->Data(1, 2) << ", "
    << ent->Data(1, 3) << ", " << ent->Data(1, 4) << endl;

  S << "| R21, R22, R23, T2 |       "
    << ent->Data(2, 1) << ", " << ent->Data(2, 2) << ", "
    << ent->Data(2, 3) << ", " << ent->Data(2, 4) << endl;

  S << "| R31, R32, R33, T3 |       "
    << ent->Data(3, 1) << ", " << ent->Data(3, 2) << ", "
    << ent->Data(3, 3) << ", " << ent->Data(3, 4) << endl;

  switch (ent->FormNumber()) {
    case  0 : S << "-- Direct Orthogonal Matrix"      << endl;  break;
    case  1 : S << "-- Reverse Orthogonal Matrix"     << endl;  break;
    case 10 : S << "-- Cartesien Coordinate System"   << endl;  break;
    case 11 : S << "-- Cylindrical Coordinate System" << endl;  break;
    case 12 : S << "-- Spherical Coordinate System"   << endl;  break;
    default : S << "--  (Incorrect Form Number)"      << endl;  break;
  }
}
