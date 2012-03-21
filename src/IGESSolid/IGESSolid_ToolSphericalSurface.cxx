// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_ToolSphericalSurface.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_Direction.hxx>
#include <Interface_Macros.hxx>


IGESSolid_ToolSphericalSurface::IGESSolid_ToolSphericalSurface ()    {  }


void  IGESSolid_ToolSphericalSurface::ReadOwnParams
  (const Handle(IGESSolid_SphericalSurface)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  Handle(IGESGeom_Point) tempCenter;
  Standard_Real tempRadius;
  Handle(IGESGeom_Direction) tempAxis;        // default Unparameterised
  Handle(IGESGeom_Direction) tempRefdir;      // default Unparameterised
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadEntity(IR, PR.Current(), "Center point",
		STANDARD_TYPE(IGESGeom_Point), tempCenter); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Radius", tempRadius); //szv#4:S4163:12Mar99 `st=` not needed

  if (ent->FormNumber() == 1)      // Parametrised surface
    {
      PR.ReadEntity(IR, PR.Current(), "Axis direction",
		    STANDARD_TYPE(IGESGeom_Direction), tempAxis); //szv#4:S4163:12Mar99 `st=` not needed

      PR.ReadEntity(IR, PR.Current(), "Reference direction",
		    STANDARD_TYPE(IGESGeom_Direction), tempRefdir); //szv#4:S4163:12Mar99 `st=` not needed
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempCenter, tempRadius, tempAxis, tempRefdir);

}

void  IGESSolid_ToolSphericalSurface::WriteOwnParams
  (const Handle(IGESSolid_SphericalSurface)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Center());
  IW.Send(ent->Radius());
  if (ent->IsParametrised())
    {
      IW.Send(ent->Axis());
      IW.Send(ent->ReferenceDir());
    }
}

void  IGESSolid_ToolSphericalSurface::OwnShared
  (const Handle(IGESSolid_SphericalSurface)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Center());
  iter.GetOneItem(ent->Axis());
  iter.GetOneItem(ent->ReferenceDir());
}

void  IGESSolid_ToolSphericalSurface::OwnCopy
  (const Handle(IGESSolid_SphericalSurface)& another,
   const Handle(IGESSolid_SphericalSurface)& ent, Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESGeom_Point, tempCenter,
		 TC.Transferred(another->Center()));
  Standard_Real tempRadius = another->Radius();
  if (another->IsParametrised())
    {
      DeclareAndCast(IGESGeom_Direction, tempAxis,
		     TC.Transferred(another->Axis()));
      DeclareAndCast(IGESGeom_Direction, tempRefdir,
		     TC.Transferred(another->ReferenceDir()));
      ent->Init (tempCenter, tempRadius, tempAxis, tempRefdir);
    }
  else
    {
      Handle(IGESGeom_Direction) tempAxis;
      Handle(IGESGeom_Direction) tempRefdir;
      ent->Init (tempCenter, tempRadius, tempAxis, tempRefdir);
    }

}

IGESData_DirChecker  IGESSolid_ToolSphericalSurface::DirChecker
  (const Handle(IGESSolid_SphericalSurface)& /*ent*/) const
{
  IGESData_DirChecker DC(196, 0, 1);

  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefAny);
  DC.Color      (IGESData_DefAny);

  DC.BlankStatusIgnored ();
  DC.SubordinateStatusRequired (1);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void  IGESSolid_ToolSphericalSurface::OwnCheck
  (const Handle(IGESSolid_SphericalSurface)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->Radius() <= 0.0)
    ach->AddFail("Radius : Not Positive");
  Standard_Integer fn = 0;
  if (ent->IsParametrised()) fn = 1;
  if (fn != ent->FormNumber()) ach->AddFail
    ("Parametrised Status Mismatches with Form Number");
  if (ent->Axis().IsNull())  if (ent->IsParametrised()) ach->AddFail
    ("Parametrised Spherical Surface : no Axis is defined");
}

void  IGESSolid_ToolSphericalSurface::OwnDump
  (const Handle(IGESSolid_SphericalSurface)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESSolid_SphericalSurface" << endl;
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "Center : ";
  dumper.Dump(ent->Center(),S, sublevel);
  S << endl;
  S << "Radius : " << ent->Radius() << endl;
  if (ent->IsParametrised())
    {
      S << "Surface is Parametrised" << endl;
      S << "Axis direction      : ";
      dumper.Dump(ent->Axis(),S, sublevel);
      S << endl;
      S << "Reference direction : ";
      dumper.Dump(ent->ReferenceDir(),S, sublevel);
      S << endl;
    }
  else S << "Surface is UnParametrised" << endl;
}
