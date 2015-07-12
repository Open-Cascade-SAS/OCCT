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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_ToolPlane.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

#include <stdio.h>
// MGE 30/07/98
//=======================================================================
//function : IGESGeom_ToolPlane
//purpose  : 
//=======================================================================
IGESGeom_ToolPlane::IGESGeom_ToolPlane ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::ReadOwnParams(const Handle(IGESGeom_Plane)& ent,
                                       const Handle(IGESData_IGESReaderData)& IR,
                                       IGESData_ParamReader& PR) const
{
  // MGE 30/07/98

  Standard_Real A, B = 0., C = 0., D = 0., aSize = 0.;
  Handle(IGESData_IGESEntity) aCurve;
  gp_XYZ attach (0.,0.,0.);
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

/*  PR.ReadReal(PR.Current(), Msg135, A); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg135, B); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg135, C); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg135, D); //szv#4:S4163:12Mar99 `st=` not needed
*/
  if ((!PR.ReadReal(PR.Current(),A)) || (!PR.ReadReal(PR.Current(),B)) ||
      (!PR.ReadReal(PR.Current(),C)) || (!PR.ReadReal(PR.Current(),D))){
    Message_Msg Msg135("XSTEP_135");
    PR.SendFail(Msg135);
  }
/*
  st = PR.ReadReal(PR.Current(), "Coefficient Of Plane", A);
  st = PR.ReadReal(PR.Current(), "Coefficient Of Plane", B);
  st = PR.ReadReal(PR.Current(), "Coefficient Of Plane", C);
  st = PR.ReadReal(PR.Current(), "Coefficient Of Plane", D);
*/
  if (PR.IsParamDefined(PR.CurrentNumber())) {
    if (!PR.ReadEntity(IR, PR.Current(), aStatus, aCurve,Standard_True)){
      Message_Msg Msg136("XSTEP_136");
      switch(aStatus) {
      case IGESData_ReferenceError: {  
	Message_Msg Msg216 ("IGES_216");
	Msg136.Arg(Msg216.Value());
	PR.SendFail(Msg136);
	break; }
      case IGESData_EntityError: {
	Message_Msg Msg217 ("IGES_217");
	Msg136.Arg(Msg217.Value());
	PR.SendFail(Msg136);
	break; }
      default:{
      }
      }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadEntity(IR, PR.Current(), "Bounding Curve", aCurve,Standard_True);
//  en principe exige si FormNumber != 0 ... cf OwnCheck (Load accepte)

  if (PR.IsParamDefined(PR.CurrentNumber())) {
    Message_Msg Msg139("XSTEP_139");
   
    PR.ReadXYZ(PR.CurrentList(1, 3), Msg139, attach); //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadXYZ(PR.CurrentList(1, 3), "Coord of DisplaySymbol", attach);

    if (!PR.ReadReal(PR.Current(), aSize)){
      Message_Msg Msg138("XSTEP_138");
      PR.SendFail(Msg138);
    } //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadReal(PR.Current(), "DisplaySymbol Size", aSize);
  }// else {
   /* for (int i = 1; i <= 4; i ++) st = PR.DefinedElseSkip();
    PR.AddWarning("Display Symbol not defined at all");
   */ 
 // }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(A, B, C, D, aCurve, attach, aSize);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::WriteOwnParams(const Handle(IGESGeom_Plane)& ent,
                                        IGESData_IGESWriter& IW)  const
{
  Standard_Real A,B,C,D;
  ent->Equation(A,B,C,D);
  IW.Send(A);
  IW.Send(B);
  IW.Send(C);
  IW.Send(D);

  IW.Send(ent->BoundingCurve());

  IW.Send(ent->SymbolAttach().X());
  IW.Send(ent->SymbolAttach().Y());
  IW.Send(ent->SymbolAttach().Z());
  IW.Send(ent->SymbolSize());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::OwnShared(const Handle(IGESGeom_Plane)& ent,
                                   Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->BoundingCurve());
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::OwnCopy(const Handle(IGESGeom_Plane)& another,
                                 const Handle(IGESGeom_Plane)& ent,
                                 Interface_CopyTool& TC) const
{
  Standard_Real A, B, C, D;
  another->Equation(A, B, C, D);
  gp_XYZ attach = (another->SymbolAttach()).XYZ();
  Standard_Real aSize = another->SymbolSize();
  DeclareAndCast(IGESData_IGESEntity,aCurve,
		 TC.Transferred(another->BoundingCurve()));
  ent->Init(A, B, C, D, aCurve, attach, aSize);
  ent->SetFormNumber(another->FormNumber());
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolPlane::DirChecker
  (const Handle(IGESGeom_Plane)& ent )  const
{
  IGESData_DirChecker DC(108,-1,1);
  DC.Structure(IGESData_DefVoid);
  if (ent->FormNumber() != 0)
    {
      DC.LineFont(IGESData_DefAny);
//      DC.LineWeight(IGESData_DefValue);
    }
  else
    {
      DC.LineFont(IGESData_DefVoid);
      DC.LineWeight(IGESData_DefVoid);
      DC.HierarchyStatusIgnored();
    }
  DC.Color(IGESData_DefAny);
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::OwnCheck(const Handle(IGESGeom_Plane)& ent,
                                  const Interface_ShareTool&,
                                  Handle(Interface_Check)& ach)  const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg71("XSTEP_71");
  //Message_Msg Msg137("XSTEP_137");
  //========================================

  //szv#4:S4163:12Mar99 not needed
  //Standard_Real eps = 1.E-06;  // ?? Precision
  //Standard_Real A,B,C,D;
  //ent->Equation(A,B,C,D);
  if (ent->FormNumber() < -1 || ent->FormNumber() > 1) {
    Message_Msg Msg71("XSTEP_71");
    ach->SendFail(Msg71);
  }
  //szv#4:S4163:12Mar99 `!=` wrong operation on Standard_Boolean
  Standard_Boolean unbounded1 = ent->BoundingCurve().IsNull();
  Standard_Boolean unbounded2 = (ent->FormNumber() == 0);
  if ( (unbounded1 && !unbounded2) || (!unbounded1 && unbounded2) ) {
    Message_Msg Msg137("XSTEP_137");
    ach->SendFail(Msg137);
  }
// These messages are transfered in the translation procedure
//  if ( (A*A + B*B + C*C) < eps)    //  pas nul !
//    ach.SendFail("Incorrect Coefficients for the Plane");
  if ( !ent->HasBoundingCurve()) return;
//  Symbol : verifie si Size defini > 0 (sinon, n a pas de signification)
/*  Standard_Real ec = 0.;
  if (ent->SymbolSize() > 0.) ec = A*ent->SymbolAttach().X() + B*ent->SymbolAttach().Y() +
      C * ent->SymbolAttach().Z() - D;
  if ( ec > eps || ec < -eps) {
    char mess[80];
    Sprintf(mess,"Symbol Attach not in the Plane, gap/equation over %f",
	    Interface_MSG::Intervalled(ec));
    ach.SendWarning(mess,"Symbol Attach not in the Plane");

  }
*/
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolPlane::OwnDump(const Handle(IGESGeom_Plane)& ent,
                                 const IGESData_IGESDumper& dumper,
                                 const Handle(Message_Messenger)& S,
                                 const Standard_Integer level)  const
{
  S << "IGESGeom_Plane" << endl;
  Standard_Real A,B,C,D;
  ent->Equation(A,B,C,D);

  S << "Plane Coefficient A : " << A << endl;
  S << "Plane Coefficient B : " << B << endl;
  S << "Plane Coefficient C : " << C << endl;
  S << "Plane Coefficient D : " << D << endl;
  S << "The Bounding Curve  : " ;
  dumper.Dump(ent->BoundingCurve(),S, (level <= 4) ? 0 : 1);
  S << endl;
  S << "Display Symbol Location : ";
  IGESData_DumpXYZL(S,level, ent->SymbolAttach(), ent->Location());
  S << "  Size  : " << ent->SymbolSize() << endl;
}
