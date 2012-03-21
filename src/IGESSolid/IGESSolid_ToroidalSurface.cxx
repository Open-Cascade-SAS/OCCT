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

#include <IGESSolid_ToroidalSurface.ixx>
#include <gp_GTrsf.hxx>


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
