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

#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_PlaneSurface,IGESData_IGESEntity)

IGESSolid_PlaneSurface::IGESSolid_PlaneSurface ()    {  }


    void  IGESSolid_PlaneSurface::Init
  (const Handle(IGESGeom_Point)& aLocation,
   const Handle(IGESGeom_Direction)& aNormal,
   const Handle(IGESGeom_Direction)& Refdir)
{
  theLocationPoint = aLocation;
  theNormal        = aNormal;
  theRefDir        = Refdir;
  InitTypeAndForm(190, (theRefDir.IsNull() ? 0 : 1));
}

    Handle(IGESGeom_Point)  IGESSolid_PlaneSurface::LocationPoint () const
{
  return theLocationPoint;
}

    Handle(IGESGeom_Direction)  IGESSolid_PlaneSurface::Normal () const
{
  return theNormal;
}

    Handle(IGESGeom_Direction)  IGESSolid_PlaneSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_PlaneSurface::IsParametrised () const
{
  return (!theRefDir.IsNull());
}
