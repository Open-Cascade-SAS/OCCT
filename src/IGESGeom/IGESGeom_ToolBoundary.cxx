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
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_ToolBoundary.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <stdio.h>
// MGE 30/07/98
//=======================================================================
//function : IGESGeom_ToolBoundary
//purpose  : 
//=======================================================================
IGESGeom_ToolBoundary::IGESGeom_ToolBoundary ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::ReadOwnParams(const Handle(IGESGeom_Boundary)& ent,
                                          const Handle(IGESData_IGESReaderData)& IR,
                                          IGESData_ParamReader& PR) const
{
  // MGE 30/07/98

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num; //szv#4:S4163:12Mar99 j not needed, i moved down in `for`
  Standard_Integer tempType, tempPreference;
  Handle(IGESData_IGESEntity) tempSurface;
  Handle(TColStd_HArray1OfInteger) tempSenses;
  Handle(IGESData_HArray1OfIGESEntity) tempModelCurves;
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) tempParameterCurves;
  IGESData_Status aStatus;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadInteger(PR.Current(), tempType)){
    Message_Msg Msg122("XTSEP_122");
    PR.SendFail(Msg122);
  }
  if (!PR.ReadInteger(PR.Current(), tempPreference)){
    Message_Msg Msg123("XTSEP_123");
    PR.SendFail(Msg123);
  }

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, tempSurface)){
    Message_Msg Msg124("XTSEP_124");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg124.Arg(Msg216.Value());
      PR.SendFail(Msg124);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg124.Arg(Msg217.Value());
      PR.SendFail(Msg124);
      break; }
    default:{
    }
    }
  }
 
  if (PR.ReadInteger(PR.Current(), num) && (num > 0)) {
    tempSenses = new TColStd_HArray1OfInteger(1, num);
    tempModelCurves = new IGESData_HArray1OfIGESEntity(1, num);
    tempParameterCurves = new IGESBasic_HArray1OfHArray1OfIGESEntity(1, num);
  }
  else{
    Message_Msg Msg126("XTSEP_126");
    PR.SendFail(Msg126);
  }

  if (!tempSenses.IsNull() && !tempModelCurves.IsNull() && !tempParameterCurves.IsNull() ) {
    for ( Standard_Integer i = 1;  i <= num;  i++ ) //szv#4:S4163:12Mar99 Standard_Integer moved in `for`
      {
	Handle(IGESData_IGESEntity) tempEnt;
	//st = PR.ReadEntity(IR, PR.Current(), Msg127, tempEnt); //szv#4:S4163:12Mar99 moved in if
	//st = PR.ReadEntity(IR, PR.Current(), "Model Space Curves", tempEnt);
	if (PR.ReadEntity(IR, PR.Current(), aStatus, tempEnt))
	  tempModelCurves->SetValue(i, tempEnt);
	else {
	  Message_Msg Msg127("XTSEP_127");
	  switch(aStatus) {
	  case IGESData_ReferenceError: {  
	    Message_Msg Msg216 ("IGES_216");
	    Msg127.Arg(Msg216.Value());
	    PR.SendFail(Msg127);
	    break; }
	  case IGESData_EntityError: {
	    Message_Msg Msg217 ("IGES_217");
	    Msg127.Arg(Msg217.Value());
	    PR.SendFail(Msg127);
	    break; }
	  default:{
	  }
	  }
	}

	Standard_Integer tempSense;
	//st = PR.ReadInteger(PR.Current(), Msg128, tempSense); //szv#4:S4163:12Mar99 moved in if
	//st = PR.ReadInteger(PR.Current(), "Orientation flags", tempSense);
	if (PR.ReadInteger(PR.Current(), tempSense))
	  tempSenses->SetValue(i, tempSense);
	else{
	  Message_Msg Msg128("XTSEP_128");
	  PR.SendFail(Msg128);
	}

	Standard_Integer tempCount;
	//st = PR.ReadInteger(PR.Current(), Msg129, tempCount); //szv#4:S4163:12Mar99 moved in if
	//st = PR.ReadInteger(PR.Current(), "Count of Parameter Space Curves", tempCount);
	//szv#4:S4163:12Mar99 optimized
/*	
        if (st && tempCount >= 0)
	  {
	    Handle(IGESData_HArray1OfIGESEntity) tempParCurves;
	    if (tempCount > 0)
	      st = PR.ReadEnts (IR,PR.CurrentList(tempCount), Msg130, tempParCurves);
	      //st = PR.ReadEnts (IR,PR.CurrentList(tempCount), "Parameter Space Curves", tempParCurves);
*/
/*
	      {
		tempParCurves = new
		  IGESData_HArray1OfIGESEntity(1, tempCount);
		for ( j = 1; j <= tempCount; j++ ) {
		  Handle(IGESData_IGESEntity) tempEnt;
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
	if (PR.ReadInteger(PR.Current(), tempCount) && (tempCount >= 0)) {
	      Handle(IGESData_HArray1OfIGESEntity) tempParCurves;
	      if (tempCount > 0){
		Message_Msg Msg130("XTSEP_130");
		PR.ReadEnts (IR,PR.CurrentList(tempCount), Msg130, tempParCurves); //szv#4:S4163:12Mar99 `st=` not needed
	      }
	      tempParameterCurves->SetValue(i, tempParCurves);
	}
	else  {
	  Message_Msg Msg129("XTSEP_129");
	  PR.SendFail(Msg129);
	}
      }
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempType, tempPreference, tempSurface, tempModelCurves,
     tempSenses, tempParameterCurves);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::WriteOwnParams(const Handle(IGESGeom_Boundary)& ent,
                                           IGESData_IGESWriter& IW)  const
{
  Standard_Integer i, j, num1;
  IW.Send(ent->BoundaryType());
  IW.Send(ent->PreferenceType());
  IW.Send(ent->Surface());
  IW.Send(ent->NbModelSpaceCurves());
  for ( num1 = ent->NbModelSpaceCurves(), i = 1; i <= num1; i++)
    {
      IW.Send(ent->ModelSpaceCurve(i));
      IW.Send(ent->Sense(i));
      Handle(IGESData_HArray1OfIGESEntity) curves = ent->ParameterCurves(i);
      Standard_Integer nbc = ent->NbParameterCurves(i);
      IW.Send(nbc);
      if (nbc > 0)
	{
          for ( j = 1; j <= nbc; j ++)
	    IW.Send(curves->Value(j));
	}
    }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::OwnShared(const Handle(IGESGeom_Boundary)& ent,
                                      Interface_EntityIterator& iter) const
{
  Standard_Integer i, j, num1;
  iter.GetOneItem(ent->Surface());
  for ( num1 = ent->NbModelSpaceCurves(), i = 1; i <= num1; i++)
    {
      iter.GetOneItem(ent->ModelSpaceCurve(i));
      Handle(IGESData_HArray1OfIGESEntity) curves = ent->ParameterCurves(i);
      if (!curves.IsNull())
	{
	  Standard_Integer nbc = curves->Length();
          for ( j = 1; j <= nbc; j ++)
	    iter.GetOneItem(curves->Value(j));
	}
    }
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::OwnCopy(const Handle(IGESGeom_Boundary)& another,
                                    const Handle(IGESGeom_Boundary)& ent,
                                    Interface_CopyTool& TC) const
{
  Standard_Integer i, j;
  Standard_Integer tempType = another->BoundaryType();
  Standard_Integer tempPreference = another->PreferenceType();
  Standard_Integer num1 = another->NbModelSpaceCurves();

  DeclareAndCast(IGESData_IGESEntity, tempSurface,
		 TC.Transferred(another->Surface()) );

  Handle(TColStd_HArray1OfInteger) tempSenses =
    new TColStd_HArray1OfInteger(1, num1);
  Handle(IGESData_HArray1OfIGESEntity) tempModelCurves =
    new IGESData_HArray1OfIGESEntity(1, num1);
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) tempParameterCurves =
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1, num1);

  for ( i = 1; i <= num1; i++ )
    {
      DeclareAndCast(IGESData_IGESEntity, tempEnt,
		     TC.Transferred(another->ModelSpaceCurve(i)) );
      tempModelCurves->SetValue(i, tempEnt);
      tempSenses->SetValue(i, another->Sense(i));
      Standard_Integer num2 = another->NbParameterCurves(i);
      Handle(IGESData_HArray1OfIGESEntity) ParCurves =
	another->ParameterCurves(i);
      Handle(IGESData_HArray1OfIGESEntity) tempParCurves;
      if (num2 > 0) tempParCurves = new IGESData_HArray1OfIGESEntity(1, num2);
      for ( j = 1; j <= num2; j++ )
	{
          DeclareAndCast(IGESData_IGESEntity, tempEnt1,
			 TC.Transferred(ParCurves->Value(j)) );
          tempParCurves->SetValue(j, tempEnt1);
	}
      tempParameterCurves->SetValue(i, tempParCurves);
    }
  ent->Init(tempType, tempPreference, tempSurface, tempModelCurves,
	    tempSenses, tempParameterCurves);
}


//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESGeom_ToolBoundary::OwnCorrect
  (const Handle(IGESGeom_Boundary)& ent) const
{
//  Standard_Boolean t0 = (ent->BoundaryType() == 0);
  Standard_Boolean res = Standard_False;
  Standard_Boolean r2d = Standard_False;
  Standard_Integer nb = ent->NbModelSpaceCurves();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {
    Standard_Integer nbi = ent->NbParameterCurves(i);
    if (nbi == 0) continue;
    for (Standard_Integer j = 1; j <= nbi; j ++) {
      Handle(IGESData_IGESEntity) c2d = ent->ParameterCurve (i,j);
      if (c2d.IsNull()) continue;
      c2d->InitStatus
	(c2d->BlankStatus(),c2d->SubordinateStatus(),5,c2d->HierarchyStatus());
      res = Standard_True;
    }
    r2d = Standard_True;
  }
  if (!r2d) return res;
  if (ent->BoundaryType() != 0) return res;    // OK

//  Reste Boundary Type : s ilya des ParameterCurves, il doit valoir 1
//  On reconstruit donc la Boundary a l identique, mais avec BoundaryType = 1

// si type = 0, annuller tous les ParameterCurves
//  -> On reconstruit, avec ParameterCurves Nulles
// En plus, les ParameterCurves doivent avoir leur UseFlag a 5

  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) cv2d  =
    new IGESBasic_HArray1OfHArray1OfIGESEntity(1,nb);
  Handle(IGESData_HArray1OfIGESEntity) modcv  =
    new IGESData_HArray1OfIGESEntity(1,nb);
  Handle(TColStd_HArray1OfInteger) senses = new TColStd_HArray1OfInteger(1,nb);
  for (i = 1; i <= nb; i ++) {
    senses->SetValue(i, ent->Sense(i));
    modcv->SetValue (i, ent->ModelSpaceCurve(i));
    cv2d->SetValue  (i, ent->ParameterCurves(i));
  }
  ent->Init (1, ent->PreferenceType(), ent->Surface(),
	     modcv, senses, cv2d);
  return Standard_True;
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolBoundary::DirChecker
  (const Handle(IGESGeom_Boundary)& /* ent */ )  const
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


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::OwnCheck(const Handle(IGESGeom_Boundary)& ent,
                                     const Interface_ShareTool&,
                                     Handle(Interface_Check)& ach)  const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg122("XTSEP_122");
  //Message_Msg Msg123("XTSEP_123");
  //Message_Msg Msg125("XTSEP_125");
  //Message_Msg Msg128("XTSEP_128");
  //========================================

  if ((ent->BoundaryType() != 0) && (ent->BoundaryType() != 1)) {
    Message_Msg Msg122("XTSEP_122");
    ach->SendFail(Msg122);
  }
  if ((ent->PreferenceType() < 0) || (ent->PreferenceType() > 3)) {
    Message_Msg Msg123("XTSEP_123");
    ach->SendFail(Msg123);
  }

// il faudrait aussi tester que, pour BoundaryType = 1, la Surface est bien
//  Parametrique ... (au moins un cas ne passe pas : c est Plane 108)
  if (ent->BoundaryType() == 1) {
    if (ent->Surface()->TypeNumber() == 108) {
      Message_Msg Msg125("XTSEP_125");
      ach->SendFail(Msg125);
    }
  }

  Standard_Integer i, num;
  for ( num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++ )
    if (ent->Sense(i) != 1 && ent->Sense(i) != 2) {
      Message_Msg Msg128("XTSEP_128");
      ach->SendFail(Msg128);
    }
/*
  if (ent->BoundaryType() == 0)
    for ( num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++ )
      if (ent->NbParameterCurves(i) != 0)
	{
          char mess[80];
	  sprintf(mess,"Nb. Parameter Space Curve %d !=0 while Boundary Type=0",i);
	  ach.SendFail(mess);
	}
*/
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolBoundary::OwnDump(const Handle(IGESGeom_Boundary)& ent,
                                    const IGESData_IGESDumper& dumper,
                                    const Handle(Message_Messenger)& S,
                                    const Standard_Integer level)  const
{
  Standard_Integer i, num, sublevel = (level > 4) ? 1 : 0;
  S << "IGESGeom_Boundary" << endl;
  S << "Bounded Surface Representation Type : " << ent->BoundaryType() << endl;
  S << "Trimming Curves Representation : " << ent->PreferenceType() << endl;
  S << "Bounded Surface    : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
  S << "Model Space Curves : " << endl;
  S << "Orientation Flags  : " << endl;
  S << "Parameter Curves Set : ";
  IGESData_DumpEntities
    (S,dumper,-level,1,ent->NbModelSpaceCurves(),ent->ModelSpaceCurve);
  S << endl;
  if (level > 4)
    for ( num = ent->NbModelSpaceCurves(), i = 1; i <= num; i++ )
      {
	S << "[" << i << "]: ";
	S << "Model Space Curve : ";
	dumper.Dump (ent->ModelSpaceCurve(i),S, 1);
	S << "  Orientation Flags : " << ent->Sense(i) << endl;
	S << "  Parameter Curves : ";
	Handle(IGESData_HArray1OfIGESEntity) curves = ent->ParameterCurves(i);
	if (!curves.IsNull()) {
	  IGESData_DumpEntities(S,dumper, level,1,curves->Length(),curves->Value);
	}
	else S << " List Empty";
	S << endl;
      }
  S << endl;
}
