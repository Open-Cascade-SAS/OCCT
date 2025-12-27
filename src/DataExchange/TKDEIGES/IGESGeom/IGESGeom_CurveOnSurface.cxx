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

#include <IGESGeom_CurveOnSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_CurveOnSurface, IGESData_IGESEntity)

IGESGeom_CurveOnSurface::IGESGeom_CurveOnSurface() {}

void IGESGeom_CurveOnSurface::Init(const int             aMode,
                                   const occ::handle<IGESData_IGESEntity>& aSurface,
                                   const occ::handle<IGESData_IGESEntity>& aCurveUV,
                                   const occ::handle<IGESData_IGESEntity>& aCurve3D,
                                   const int             aPreference)
{
  theCreationMode   = aMode;
  theSurface        = aSurface;
  theCurveUV        = aCurveUV;
  theCurve3D        = aCurve3D;
  thePreferenceMode = aPreference;
  InitTypeAndForm(142, 0);
}

int IGESGeom_CurveOnSurface::CreationMode() const
{
  return theCreationMode;
}

occ::handle<IGESData_IGESEntity> IGESGeom_CurveOnSurface::Surface() const
{
  return theSurface;
}

occ::handle<IGESData_IGESEntity> IGESGeom_CurveOnSurface::CurveUV() const
{
  return theCurveUV;
}

occ::handle<IGESData_IGESEntity> IGESGeom_CurveOnSurface::Curve3D() const
{
  return theCurve3D;
}

int IGESGeom_CurveOnSurface::PreferenceMode() const
{
  return thePreferenceMode;
}
