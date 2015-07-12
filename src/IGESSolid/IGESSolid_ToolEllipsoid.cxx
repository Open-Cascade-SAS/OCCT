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

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESSolid_Ellipsoid.hxx>
#include <IGESSolid_ToolEllipsoid.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolEllipsoid::IGESSolid_ToolEllipsoid ()    {  }


void  IGESSolid_ToolEllipsoid::ReadOwnParams
  (const Handle(IGESSolid_Ellipsoid)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  gp_XYZ tempSize, tempCenter, tempXAxis, tempZAxis;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Real tempreal;

  PR.ReadXYZ(PR.CurrentList(1, 3), "Size", tempSize); //szv#4:S4163:12Mar99 `st=` not needed

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (X)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (X)", tempreal))
	tempCenter.SetX(tempreal);
    }
  else  tempCenter.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (Y)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (Y)", tempreal))
	tempCenter.SetY(tempreal);
    }
  else  tempCenter.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Center Point (Z)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Center Point (Z)", tempreal))
	tempCenter.SetZ(tempreal);
    }
  else  tempCenter.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local X axis (I)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local X axis (I)", tempreal))
	tempXAxis.SetX(tempreal);
    }
  else  tempXAxis.SetX(1.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local X axis (J)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local X axis (J)", tempreal))
	tempXAxis.SetY(tempreal);
    }
  else  tempXAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local X axis (K)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local X axis (K)", tempreal))
	tempXAxis.SetZ(tempreal);
    }
  else  tempXAxis.SetZ(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local Z axis (I)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local Z axis (I)", tempreal))
	tempZAxis.SetX(tempreal);
    }
  else  tempZAxis.SetX(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local Z axis (J)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local Z axis (J)", tempreal))
	tempZAxis.SetY(tempreal);
    }
  else  tempZAxis.SetY(0.0);

  if (PR.DefinedElseSkip())
    {
      //st = PR.ReadReal(PR.Current(), "Local Z axis (K)", tempreal); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadReal(PR.Current(), "Local Z axis (K)", tempreal))
	tempZAxis.SetZ(tempreal);
    }
  else  tempZAxis.SetZ(1.0);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempSize, tempCenter, tempXAxis, tempZAxis);
  Standard_Real eps = 1.E-05;
  if (!tempXAxis.IsEqual(ent->XAxis().XYZ(),eps)) PR.AddWarning
    ("XAxis poorly unitary, normalized");
  if (!tempZAxis.IsEqual(ent->ZAxis().XYZ(),eps)) PR.AddWarning
    ("ZAxis poorly unitary, normalized");
}

void  IGESSolid_ToolEllipsoid::WriteOwnParams
  (const Handle(IGESSolid_Ellipsoid)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Size().X());
  IW.Send(ent->Size().Y());
  IW.Send(ent->Size().Z());
  IW.Send(ent->Center().X());
  IW.Send(ent->Center().Y());
  IW.Send(ent->Center().Z());
  IW.Send(ent->XAxis().X());
  IW.Send(ent->XAxis().Y());
  IW.Send(ent->XAxis().Z());
  IW.Send(ent->ZAxis().X());
  IW.Send(ent->ZAxis().Y());
  IW.Send(ent->ZAxis().Z());
}

void  IGESSolid_ToolEllipsoid::OwnShared
  (const Handle(IGESSolid_Ellipsoid)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESSolid_ToolEllipsoid::OwnCopy
  (const Handle(IGESSolid_Ellipsoid)& another,
   const Handle(IGESSolid_Ellipsoid)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init(another->Size(), another->Center().XYZ(),
	    another->XAxis().XYZ(), another->ZAxis().XYZ());
}

IGESData_DirChecker  IGESSolid_ToolEllipsoid::DirChecker
  (const Handle(IGESSolid_Ellipsoid)& /* ent */ ) const
{
  IGESData_DirChecker DC(168, 0);
  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.UseFlagRequired (0);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolEllipsoid::OwnCheck
  (const Handle(IGESSolid_Ellipsoid)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  Standard_Real eps = 1.E-04;
  Standard_Real prosca = ent->XAxis().Dot(ent->ZAxis());
  if (prosca < -eps || prosca > eps)
    ach->AddFail("Local Z axis : Not orthogonal to X axis");
  if (! (ent->Size().X() >= ent->Size().Y()
	 && ent->Size().Y() >= ent->Size().Z()
	 && ent->Size().Z() > 0))
    ach->AddFail("Size : The values does not satisfy LX >= LY >= LZ > 0");
}

void  IGESSolid_ToolEllipsoid::OwnDump
  (const Handle(IGESSolid_Ellipsoid)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_Ellipsoid" << endl;

  S << "Size   : ";
  IGESData_DumpXYZ(S, ent->Size());
  S << endl << "Center : ";
  IGESData_DumpXYZL(S,level, ent->Center(), ent->Location());
  S << endl << "XAxis  : ";
  IGESData_DumpXYZL(S,level, ent->XAxis(), ent->VectorLocation());
  S << endl << "ZAxis  : ";
  IGESData_DumpXYZL(S,level, ent->ZAxis(), ent->VectorLocation());
  S << endl;
}
