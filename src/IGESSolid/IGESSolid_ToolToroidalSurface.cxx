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
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_ToolToroidalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESSolid_ToolToroidalSurface::IGESSolid_ToolToroidalSurface ()    {  }


void  IGESSolid_ToolToroidalSurface::ReadOwnParams
  (const Handle(IGESSolid_ToroidalSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESGeom_Point) tempCenter;
  Standard_Real majRad, minRad;
  Handle(IGESGeom_Direction) tempAxis;            // default Unparametrised
  Handle(IGESData_IGESEntity) tempRefdir;          // default Unparametrised
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Center point",
		STANDARD_TYPE(IGESGeom_Point), tempCenter); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR, PR.Current(), "Axis direction",
		STANDARD_TYPE(IGESGeom_Direction), tempAxis); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Major Radius", majRad); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Minor Radius", minRad); //szv#4:S4163:12Mar99 `st=` not needed

  if (ent->FormNumber() == 1)      // Parametrised surface
    PR.ReadEntity(IR, PR.Current(), "Reference direction", tempRefdir); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempCenter, tempAxis, majRad, minRad, Handle(IGESGeom_Direction)::DownCast (tempRefdir));
}

void  IGESSolid_ToolToroidalSurface::WriteOwnParams
  (const Handle(IGESSolid_ToroidalSurface)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Center());
  IW.Send(ent->Axis());
  IW.Send(ent->MajorRadius());
  IW.Send(ent->MinorRadius());
  if (ent->IsParametrised())    IW.Send(ent->ReferenceDir());
}

void  IGESSolid_ToolToroidalSurface::OwnShared
  (const Handle(IGESSolid_ToroidalSurface)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Center());
  iter.GetOneItem(ent->Axis());
  iter.GetOneItem(ent->ReferenceDir());
}

void  IGESSolid_ToolToroidalSurface::OwnCopy
  (const Handle(IGESSolid_ToroidalSurface)& another,
   const Handle(IGESSolid_ToroidalSurface)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESGeom_Point, tempCenter,
		 TC.Transferred(another->Center()));
  DeclareAndCast(IGESGeom_Direction, tempAxis,
		 TC.Transferred(another->Axis()));
  Standard_Real majRad = another->MajorRadius();
  Standard_Real minRad = another->MinorRadius();
  if (another->IsParametrised())
    {
      DeclareAndCast(IGESGeom_Direction, tempRefdir,
		     TC.Transferred(another->ReferenceDir()));
      ent->Init (tempCenter, tempAxis, majRad, minRad, tempRefdir);
    }
  else
    {
      Handle(IGESGeom_Direction) tempRefdir;
      ent->Init (tempCenter, tempAxis, majRad, minRad, tempRefdir);
    }
}

IGESData_DirChecker  IGESSolid_ToolToroidalSurface::DirChecker
  (const Handle(IGESSolid_ToroidalSurface)& /*ent*/) const
{
  IGESData_DirChecker DC(198, 0, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.BlankStatusIgnored ();
  DC.SubordinateStatusRequired (1);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolToroidalSurface::OwnCheck
  (const Handle(IGESSolid_ToroidalSurface)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->MajorRadius() <= 0.0)
    ach->AddFail("Major Radius : Not Positive");
  if (ent->MinorRadius() <= 0.0)
    ach->AddFail("Minor Radius : Not Positive");
  if (ent->MinorRadius() >= ent->MajorRadius())
    ach->AddFail("Minor Radius : Value not < Major radius");
  Standard_Integer fn = 0;
  if (ent->IsParametrised()) fn = 1;
  if (fn != ent->FormNumber()) ach->AddFail
    ("Parametrised Status Mismatches with Form Number");
}

void  IGESSolid_ToolToroidalSurface::OwnDump
  (const Handle(IGESSolid_ToroidalSurface)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_ToroidalSurface" << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Center : ";
  dumper.Dump(ent->Center(),S, sublevel);
  S << endl;
  S << "Axis direction : ";
  dumper.Dump(ent->Axis(),S, sublevel);
  S << endl;
  S << "Major Radius : " << ent->MajorRadius() << "  ";
  S << "Minor Radius : " << ent->MinorRadius() << endl;
  if (ent->IsParametrised())
    {
      S << "Surface is Parametrised  -  Reference direction : ";
      dumper.Dump(ent->ReferenceDir(),S, sublevel);
      S << endl;
    }
  else S << "Surface is UnParametrised" << endl;
}
