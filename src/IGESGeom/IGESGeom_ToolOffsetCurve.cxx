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
//:l9 abv 15.01.99: CTS22023 and TEC0278: issue data fail on offset tapered flag 
// only if type is not constant

#include <gp_Vec.hxx>
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
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_ToolOffsetCurve.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 30/07/98
//=======================================================================
//function : IGESGeom_ToolOffsetCurve
//purpose  : 
//=======================================================================
IGESGeom_ToolOffsetCurve::IGESGeom_ToolOffsetCurve ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::ReadOwnParams(const Handle(IGESGeom_OffsetCurve)& ent,
                                             const Handle(IGESData_IGESReaderData)& IR,
                                             IGESData_ParamReader& PR) const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  Message_Msg Msg121("XSTEP_121");
  //========================================

  Standard_Integer anOffsetType, aFunctionCoord, aTaperedOffsetType; 
  Standard_Real offDistance1, offDistance2;
  Standard_Real arcLength1, arcLength2, anOffsetParam, anotherOffsetParam;
  gp_XYZ aNormalVec;
  Handle(IGESData_IGESEntity) aBaseCurve;
  Handle(IGESData_IGESEntity) aFunction;
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  // Reading the curve entity to be offset
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aBaseCurve)){
    Message_Msg Msg110("XSTEP_110");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg110.Arg(Msg216.Value());
      PR.SendFail(Msg110);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg110.Arg(Msg217.Value());
      PR.SendFail(Msg110);
      break; }
    default:{
    }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadEntity(IR, PR.Current(), "Curve to be offset", aBaseCurve);

  // Reading the offset distance flag
  if (!PR.ReadInteger(PR.Current(), anOffsetType)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg111("XSTEP_111");
    PR.SendFail(Msg111);
  }
  //st = PR.ReadInteger(PR.Current(), "Offset Distance Flag", anOffsetType);

  // Reading the curve entity describing the offset as a function, can be Null
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aFunction, Standard_True)){
    Message_Msg Msg112("XSTEP_112");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg112.Arg(Msg216.Value());
      PR.SendFail(Msg112);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg112.Arg(Msg217.Value());
      PR.SendFail(Msg112);
      break; }
    default:{
    }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed
/*
  st = PR.ReadEntity(IR, PR.Current(), "Curve whose coordinate describes the offset", aFunction, Standard_True);
*/

  // Reading the coordinate describing the offset as a function
  if (!PR.ReadInteger(PR.Current(), aFunctionCoord)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg113("XSTEP_113");
    PR.SendFail(Msg113);
  }
  //st = PR.ReadInteger(PR.Current(), "Coordinate of the curve", aFunctionCoord);

  // Reading the tapered offset type flag
  if (!PR.ReadInteger(PR.Current(), aTaperedOffsetType)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg114("XSTEP_114");
    PR.SendFail(Msg114);
  }
  //st = PR.ReadInteger(PR.Current(), "Tapered offset type flag", aTaperedOffsetType);

  // Reading the first offset distance
  if (!PR.ReadReal(PR.Current(), offDistance1)){
    Message_Msg Msg115("XSTEP_115");
    PR.SendFail(Msg115);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "First Offset distance", offDistance1);

  // Reading the arc length or parameter value of the first offset distance
  if (!PR.ReadReal(PR.Current(), arcLength1)){
    Message_Msg Msg116("XSTEP_116");
    PR.SendFail(Msg116);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "Arc length of first offset distance", arcLength1);

  // Reading the second offset distance
  if (!PR.ReadReal(PR.Current(),offDistance2)){
    Message_Msg Msg117("XSTEP_117");
    PR.SendFail(Msg117);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "Second Offset distance", offDistance2);

  // Reading the arc length or parameter value of the second offset distance
  if (!PR.ReadReal(PR.Current(), arcLength2)){
    Message_Msg Msg118("XSTEP_118");
    PR.SendFail(Msg118);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "Arc length of Second offset distance", arcLength2);

  // Reading the Unit vector normal to plane
  PR.ReadXYZ (PR.CurrentList(1, 3), Msg121, aNormalVec); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadXYZ (PR.CurrentList(1, 3), "Unit vector normal to plane", aNormalVec);

  // Reading the offset curve starting parameter value
  if (!PR.ReadReal(PR.Current(), anOffsetParam)){
    Message_Msg Msg119("XSTEP_119");
    PR.SendFail(Msg119);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "Starting parameter value of Offset curve", anOffsetParam);

  // Reading the offset curve ending parameter value
  if (!PR.ReadReal(PR.Current(), anotherOffsetParam)){
    Message_Msg Msg120("XSTEP_120");
    PR.SendFail(Msg120);
  } //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadReal(PR.Current(), "Ending parameter value of Offset curve", anotherOffsetParam);

  // Reading the Unit vector normal to plane
  PR.ReadXYZ (PR.CurrentList(1, 3), Msg121, aNormalVec); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadXYZ (PR.CurrentList(1, 3), "Unit vector normal to plane", aNormalVec);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (aBaseCurve, anOffsetType, aFunction, aFunctionCoord,
     aTaperedOffsetType, offDistance1, arcLength1, offDistance2,
     arcLength2, aNormalVec, anOffsetParam, anotherOffsetParam);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::WriteOwnParams(const Handle(IGESGeom_OffsetCurve)& ent,
                                              IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->BaseCurve());
  IW.Send(ent->OffsetType());

  IW.Send(ent->Function());

  IW.Send(ent->FunctionParameter());
  IW.Send(ent->TaperedOffsetType());
  IW.Send(ent->FirstOffsetDistance());
  IW.Send(ent->ArcLength1());
  IW.Send(ent->SecondOffsetDistance());
  IW.Send(ent->ArcLength2());
  IW.Send(ent->NormalVector().X());
  IW.Send(ent->NormalVector().Y());
  IW.Send(ent->NormalVector().Z());
  IW.Send(ent->StartParameter());
  IW.Send(ent->EndParameter());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::OwnShared(const Handle(IGESGeom_OffsetCurve)& ent,
                                         Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->BaseCurve());
  iter.GetOneItem(ent->Function());
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::OwnCopy(const Handle(IGESGeom_OffsetCurve)& another,
                                       const Handle(IGESGeom_OffsetCurve)& ent,
                                       Interface_CopyTool& TC) const
{
  Standard_Integer anOffsetType, aFunctionCoord, aTaperedOffsetType; 
  Standard_Real offDistance1, offDistance2;
  Standard_Real arcLength1, arcLength2, anOffsetParam1, anOffsetParam2;

  DeclareAndCast(IGESData_IGESEntity, aBaseCurve,
		 TC.Transferred(another->BaseCurve()));
  anOffsetType   = another->OffsetType();
  DeclareAndCast(IGESData_IGESEntity, aFunction,
		 TC.Transferred(another->Function()));
  aFunctionCoord = another->FunctionParameter();
  aTaperedOffsetType = another->TaperedOffsetType();
  offDistance1   = another->FirstOffsetDistance();
  arcLength1     = another->ArcLength1();
  offDistance2   = another->SecondOffsetDistance();
  arcLength2     = another->ArcLength2();
  gp_XYZ aNormalVec = (another->NormalVector()).XYZ();
  anOffsetParam1 = another->StartParameter();
  anOffsetParam2 = another->EndParameter();

  ent->Init(aBaseCurve, anOffsetType, aFunction, aFunctionCoord,
	    aTaperedOffsetType, offDistance1, arcLength1, offDistance2,
	    arcLength2, aNormalVec, anOffsetParam1, anOffsetParam2);
}


//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESGeom_ToolOffsetCurve::OwnCorrect
  (const Handle(IGESGeom_OffsetCurve)& ent) const
{
  if (ent->OffsetType() == 3) return Standard_False;
  Handle(IGESData_IGESEntity) func = ent->Function();
  if (func.IsNull()) return Standard_False;
//  OffsetType != 3 : reconstruire avec Offset Function Nulle
  func.Nullify();
  ent->Init (ent->BaseCurve(), ent->OffsetType(), func,0,  // func+coord Nuls
	     ent->TaperedOffsetType(),
	     ent->FirstOffsetDistance(),  ent->ArcLength1(),
	     ent->SecondOffsetDistance(), ent->ArcLength2(),
	     ent->NormalVector().XYZ(),
	     ent->StartParameter(), ent->EndParameter() );
  return Standard_True;
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolOffsetCurve::DirChecker
  (const Handle(IGESGeom_OffsetCurve)& /* ent */ )   const
{
  IGESData_DirChecker DC(130, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::OwnCheck(const Handle(IGESGeom_OffsetCurve)& ent,
                                        const Interface_ShareTool&,
                                        Handle(Interface_Check)& ach)  const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg111("XSTEP_111");
  //Message_Msg Msg114("XSTEP_114");
  //========================================

  Standard_Integer ot = ent->OffsetType();
  if (ot < 1 || ot > 3) {
    Message_Msg Msg111("XSTEP_111");
    ach->SendFail(Msg111);
  }
/*  if (ot == 3)  if (ent->Function().IsNull())
    ach.SendFail("Offset Function Not Defined while Offset Type = 3");
  if (ot == 3 && (ent->FunctionParameter() < 1 || ent->FunctionParameter() > 3))
    ach.SendFail("Offset Function Parameter != 1-2 or 3 (rq : for X-Y or Z)");
*/
  if (ot !=1 && //:l9 abv 15.01.99: CTS22023 and TEC0278: only if ot is function
      ((ent->TaperedOffsetType() < 1) || (ent->TaperedOffsetType() > 2))) {
    Message_Msg Msg114("XSTEP_114");
    ach->SendFail(Msg114);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolOffsetCurve::OwnDump(const Handle(IGESGeom_OffsetCurve)& ent,
                                       const IGESData_IGESDumper& dumper,
                                       const Handle(Message_Messenger)& S,
                                       const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESGeom_OffsetCurve" << endl;

  S << "The curve to be offset     : " << endl;
  dumper.Dump(ent->BaseCurve(),S, sublevel);
  S << "Offset Distance Flag       : " << ent->OffsetType() << endl;
  S << "Curve entity whose coordinate defines the offset : ";
  dumper.Dump(ent->Function(),S, sublevel);
  S << endl;
  S << "In which Coordinate to use : " << ent->FunctionParameter()    << endl;
  S << "Tapered Offset Type Flag   : " << ent->TaperedOffsetType()    << endl;
  S << "First Offset Distance      : " << ent->FirstOffsetDistance()  << "  ";
  S << "Arc Length : " << ent->ArcLength1() << endl;
  S << "Second Offset Distance     : " << ent->SecondOffsetDistance() << "  ";
  S << "Arc Length : " << ent->ArcLength2() << endl;
  S << "Normal Vector : ";
  IGESData_DumpXYZL(S,level, ent->NormalVector(), ent->VectorLocation());  S<<endl;
  S << "Offset curve Parameters. Starting : " << ent->StartParameter() << "  ";
  S << "Ending : " << ent->EndParameter()   << endl;
}
