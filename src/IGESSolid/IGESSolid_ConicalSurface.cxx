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

#include <IGESSolid_ConicalSurface.ixx>


IGESSolid_ConicalSurface::IGESSolid_ConicalSurface ()    {  }


    void  IGESSolid_ConicalSurface::Init
  (const Handle(IGESGeom_Point)&     aLocation,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real aRadius,  const Standard_Real anAngle,
   const Handle(IGESGeom_Direction)& aRefdir)
{
  theLocationPoint = aLocation;
  theAxis          = anAxis;
  theRadius        = aRadius;
  theAngle         = anAngle;
  theRefDir        = aRefdir;
  InitTypeAndForm(194, (theRefDir.IsNull() ? 0 : 1));
}

    Handle(IGESGeom_Point) IGESSolid_ConicalSurface::LocationPoint () const
{
  return theLocationPoint;
}

    Handle(IGESGeom_Direction) IGESSolid_ConicalSurface::Axis () const
{
  return theAxis;
}

    Standard_Real IGESSolid_ConicalSurface::Radius () const
{
  return theRadius;
}

    Standard_Real IGESSolid_ConicalSurface::SemiAngle () const
{
  return theAngle;
}

    Handle(IGESGeom_Direction) IGESSolid_ConicalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean IGESSolid_ConicalSurface::IsParametrised () const
{
  return (!theRefDir.IsNull());
}
