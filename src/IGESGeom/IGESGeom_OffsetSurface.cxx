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

#include <IGESGeom_OffsetSurface.ixx>
#include <gp_GTrsf.hxx>


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
