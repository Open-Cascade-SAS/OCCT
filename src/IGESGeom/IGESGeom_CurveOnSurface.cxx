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

#include <IGESGeom_CurveOnSurface.ixx>


IGESGeom_CurveOnSurface::IGESGeom_CurveOnSurface ()    {  }


    void IGESGeom_CurveOnSurface::Init
  (const Standard_Integer aMode,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESData_IGESEntity)& aCurveUV,
   const Handle(IGESData_IGESEntity)& aCurve3D,
   const Standard_Integer aPreference)
{
  theCreationMode   = aMode;
  theSurface        = aSurface;
  theCurveUV        = aCurveUV;
  theCurve3D        = aCurve3D;
  thePreferenceMode = aPreference;
  InitTypeAndForm(142,0);
}

    Standard_Integer IGESGeom_CurveOnSurface::CreationMode () const
{
  return theCreationMode;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::Surface () const
{
  return theSurface;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::CurveUV () const
{
  return theCurveUV;
}

    Handle(IGESData_IGESEntity) IGESGeom_CurveOnSurface::Curve3D () const
{
  return theCurve3D;
}

    Standard_Integer IGESGeom_CurveOnSurface::PreferenceMode () const
{
  return thePreferenceMode;
}
