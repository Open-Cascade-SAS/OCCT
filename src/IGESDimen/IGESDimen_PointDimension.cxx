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

#include <IGESDimen_PointDimension.ixx>
#include <Interface_Macros.hxx>

IGESDimen_PointDimension::IGESDimen_PointDimension ()    {  }


    void IGESDimen_PointDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& anArrow,
   const Handle(IGESData_IGESEntity)&   aGeom)
{
  theNote   = aNote;
  theLeader = anArrow;
  theGeom   = aGeom;
  InitTypeAndForm(220,0);
}


    Handle(IGESDimen_GeneralNote) IGESDimen_PointDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_PointDimension::LeaderArrow () const
{
  return theLeader;
}

    Handle(IGESGeom_CircularArc) IGESDimen_PointDimension::CircularArc () const
{
  return GetCasted(IGESGeom_CircularArc, theGeom);
}

    Handle(IGESGeom_CompositeCurve) IGESDimen_PointDimension::CompositeCurve () const
{
  return GetCasted(IGESGeom_CompositeCurve, theGeom);
}

    Handle(IGESData_IGESEntity) IGESDimen_PointDimension::Geom () const
{
  return theGeom;
}

    Standard_Integer IGESDimen_PointDimension::GeomCase () const
{
  if (theGeom.IsNull())                     return 0;
  else if (theGeom->TypeNumber() == 100)    return 1;
  else if (theGeom->TypeNumber() == 102)    return 2;
  else                                      return 3;
}
