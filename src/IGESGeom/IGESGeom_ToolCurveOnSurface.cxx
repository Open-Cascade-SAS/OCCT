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
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_ToolCurveOnSurface.hxx>
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
//function : IGESGeom_ToolCurveOnSurface
//purpose  : 
//=======================================================================
IGESGeom_ToolCurveOnSurface::IGESGeom_ToolCurveOnSurface ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::ReadOwnParams
  (const Handle(IGESGeom_CurveOnSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{

  // MGE 30/07/98

  Standard_Integer aMode, aPreference;
  Handle(IGESData_IGESEntity) aSurface;
  Handle(IGESData_IGESEntity) aCurveUV;
  Handle(IGESData_IGESEntity) aCurve3D;
  IGESData_Status aStatus;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  if (!PR.ReadInteger(PR.Current(), aMode)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg276("XSTEP_276");
    PR.SendFail(Msg276);
  }
  if (!PR.ReadEntity(IR, PR.Current(),aStatus, aSurface)){
    Message_Msg Msg131("XSTEP_131");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg131.Arg(Msg216.Value());
      PR.SendFail(Msg131);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg131.Arg(Msg217.Value());
      PR.SendFail(Msg131);
      break; }
    default:{
    }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed

  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aCurveUV, Standard_True)){
    Message_Msg Msg132("XSTEP_132");
    switch(aStatus) {
    case IGESData_ReferenceError: {  
      Message_Msg Msg216 ("IGES_216");
      Msg132.Arg(Msg216.Value());
      PR.SendFail(Msg132);
      break; }
    case IGESData_EntityError: {
      Message_Msg Msg217 ("IGES_217");
      Msg132.Arg(Msg217.Value());
      PR.SendFail(Msg132);
      break; }
    default:{
    }
    }
  } //szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadEntity(IR, PR.Current(), aStatus, aCurve3D, Standard_True)){; //szv#4:S4163:12Mar99 `st=` not needed
      Message_Msg Msg133("XSTEP_133");
      switch(aStatus) {
      case IGESData_ReferenceError: {  
	Message_Msg Msg216 ("IGES_216");
	Msg133.Arg(Msg216.Value());
	PR.SendFail(Msg133);
	break; }
      case IGESData_EntityError: {
	Message_Msg Msg217 ("IGES_217");
	Msg133.Arg(Msg217.Value());
	PR.SendFail(Msg133);
	break; }
      default:{
      }
      }
  }
  if (!PR.ReadInteger(PR.Current(), aPreference)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg277("XSTEP_277");  
    PR.SendFail(Msg277);
  }
/*
  st = PR.ReadInteger(PR.Current(), "Creation mode of curve", aMode);
  st = PR.ReadEntity(IR, PR.Current(), "Surface (on which curve lies)", aSurface);
  st = PR.ReadEntity(IR, PR.Current(), "Curve UV", aCurveUV, Standard_True);
  st = PR.ReadEntity(IR, PR.Current(), "Curve 3D", aCurve3D, Standard_True);
  st = PR.ReadInteger(PR.Current(), "Preferred representation", aPreference);
*/
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (aMode, aSurface, aCurveUV, aCurve3D, aPreference);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::WriteOwnParams
  (const Handle(IGESGeom_CurveOnSurface)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->CreationMode());
  IW.Send(ent->Surface());
  IW.Send(ent->CurveUV());
  IW.Send(ent->Curve3D());
  IW.Send(ent->PreferenceMode());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::OwnShared(const Handle(IGESGeom_CurveOnSurface)& ent,
                                            Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Surface());
  iter.GetOneItem(ent->CurveUV());
  iter.GetOneItem(ent->Curve3D());
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::OwnCopy(const Handle(IGESGeom_CurveOnSurface)& another,
                                          const Handle(IGESGeom_CurveOnSurface)& ent,
                                          Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESData_IGESEntity, aSurface, TC.Transferred(another->Surface()));
  DeclareAndCast(IGESData_IGESEntity, aCurveUV, TC.Transferred(another->CurveUV()));
  DeclareAndCast(IGESData_IGESEntity, aCurve3D, TC.Transferred(another->Curve3D()));

  Standard_Integer aMode = another->CreationMode();
  Standard_Integer aPreference = another->PreferenceMode();

  ent->Init(aMode, aSurface, aCurveUV, aCurve3D, aPreference);
}


//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESGeom_ToolCurveOnSurface::OwnCorrect
  (const Handle(IGESGeom_CurveOnSurface)& ent) const
{
  Handle(IGESData_IGESEntity) c2d = ent->CurveUV();
  if (c2d.IsNull()) return Standard_False;
  Standard_Integer uf = c2d->UseFlag();
  if (uf == 5) return Standard_False;
  c2d->InitStatus
    (c2d->BlankStatus(),c2d->SubordinateStatus(),5,c2d->HierarchyStatus());
  return Standard_True;
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolCurveOnSurface::DirChecker
  (const Handle(IGESGeom_CurveOnSurface)& /* ent */ )  const
{
  IGESData_DirChecker DC(142, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired (5);
  DC.HierarchyStatusIgnored ();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::OwnCheck(const Handle(IGESGeom_CurveOnSurface)& ent,
                                           const Interface_ShareTool&,
                                           Handle(Interface_Check)& ach)  const
{
  // MGE 30/07/98
  // Building of messages
  //========================================
  //Message_Msg Msg134 = Message_Msg("XSTEP_134");
  //========================================

//  if (ent->CreationMode() < 0 || ent->CreationMode() > 3)
//    ach.SendFail("Incorrect value for the Creation Mode");
//  Standard_Integer pref = ent->PreferenceMode();
//  if (pref < 0 || pref > 3)
//    ach.SendFail("Incorrect value for the Preference Mode");
//  tolerance a la norme
//  if (pref == 1 && ent->CurveUV().IsNull())
//    ach.AddWarning("Preference Mode asks UV and Curve UV not defined");
//  if (pref == 2 && ent->Curve3D().IsNull())
//    ach.AddWarning("Preference Mode asks UV and Curve UV not defined");
  if (ent->CurveUV().IsNull() && ent->Curve3D().IsNull()) {
    Message_Msg Msg134 = Message_Msg("XSTEP_134");
    ach->SendFail(Msg134);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolCurveOnSurface::OwnDump(const Handle(IGESGeom_CurveOnSurface)& ent,
                                          const IGESData_IGESDumper& dumper,
                                          const Handle(Message_Messenger)& S,
                                          const Standard_Integer level)  const
{
  S << "IGESGeom_CurveOnSurface" << endl << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  Standard_Integer crem = ent->CreationMode();
  S << "Creation Mode : " << crem << "  i.e. ";
  if      (crem == 0) S << " <Unspecified>" << endl;
  else if (crem == 1) S << " Projection of a Curve on a Surface" << endl;
  else if (crem == 2) S << " Intersection of two Surfaces" << endl;
  else if (crem == 3) S << " Isoparametric Curve (either U or V)" << endl;
  else S << " <Incorrect Value>" << endl;
  S << "The Surface on which the curve lies : ";
  dumper.Dump(ent->Surface(),S, sublevel);
  S << endl;
  S << "The curve B (in the parametric space (u, v))  : ";
  dumper.Dump(ent->CurveUV(),S, sublevel);
  S << endl;
  S << "The curve C (in the 3D Space) : ";
  dumper.Dump(ent->Curve3D(),S, sublevel);
  S << endl;
  Standard_Integer pref = ent->PreferenceMode();
  S << "Preferred representation mode : " << pref << "  i.e. ";
  if      (pref == 0) S << " <Unspecified>" << endl;
  else if (pref == 1) S << " Curve B on Surface" << endl;
  else if (pref == 2) S << " Curve C in 3D Space" << endl;
  else if (pref == 3) S << " Curves B & C equally preferred" << endl;
  else S << " <Incorrect Value>" << endl;
}
