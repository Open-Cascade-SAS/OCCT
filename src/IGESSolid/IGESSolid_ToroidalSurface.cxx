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
#include <gp_Pnt.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_ToroidalSurface,IGESData_IGESEntity)

IGESSolid_ToroidalSurface::IGESSolid_ToroidalSurface ()    {  }


    void  IGESSolid_ToroidalSurface::Init
  (const Handle(IGESGeom_Point)& aCenter,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real majRadius, const Standard_Real minRadius,
   const Handle(IGESGeom_Direction)& Refdir)
{
  theCenter      = aCenter;
  theAxis        = anAxis;
  theMajorRadius = majRadius;
  theMinorRadius = minRadius;
  theRefDir      = Refdir;
  InitTypeAndForm(198, (theRefDir.IsNull() ? 0 : 1) );
}

    Handle(IGESGeom_Point)  IGESSolid_ToroidalSurface::Center () const
{
  return theCenter;
}

    gp_Pnt  IGESSolid_ToroidalSurface::TransformedCenter () const
{
  if (!HasTransf()) return theCenter->Value();
  else
    {
      gp_XYZ tmp = theCenter->Value().XYZ();
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    Handle(IGESGeom_Direction)  IGESSolid_ToroidalSurface::Axis () const
{
  return theAxis;
}

    Standard_Real  IGESSolid_ToroidalSurface::MajorRadius () const
{
  return theMajorRadius;
}

    Standard_Real  IGESSolid_ToroidalSurface::MinorRadius () const
{
  return theMinorRadius;
}

    Handle(IGESGeom_Direction)  IGESSolid_ToroidalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_ToroidalSurface::IsParametrised () const
{
  return !(theRefDir.IsNull());
}
