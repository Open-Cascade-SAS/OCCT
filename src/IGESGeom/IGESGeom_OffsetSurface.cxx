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
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_OffsetSurface,IGESData_IGESEntity)

IGESGeom_OffsetSurface::IGESGeom_OffsetSurface ()     {  }


    void IGESGeom_OffsetSurface::Init
  (const gp_XYZ&                      anIndicator,
   const Standard_Real                aDistance,
   const Handle(IGESData_IGESEntity)& aSurface)
{
  theIndicator = anIndicator;
  theDistance  = aDistance;
  theSurface   = aSurface;
  InitTypeAndForm(140,0);
}

    gp_Vec IGESGeom_OffsetSurface::OffsetIndicator () const
{
  return gp_Vec(theIndicator);
}

    gp_Vec IGESGeom_OffsetSurface::TransformedOffsetIndicator () const
{
  if (!HasTransf()) return gp_Vec(theIndicator);
  gp_XYZ temp(theIndicator);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
  loc.Transforms(temp);
  return gp_Vec(temp);
}

    Standard_Real IGESGeom_OffsetSurface::Distance () const
{
  return theDistance;
}

    Handle(IGESData_IGESEntity) IGESGeom_OffsetSurface::Surface () const
{
  return theSurface;
}
