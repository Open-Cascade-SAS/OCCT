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

#include <IGESSolid_Cylinder.ixx>
#include <gp_GTrsf.hxx>


IGESSolid_Cylinder::IGESSolid_Cylinder ()    {  }


    void  IGESSolid_Cylinder::Init
  (const Standard_Real aHeight, const Standard_Real aRadius,
   const gp_XYZ& aCenter, const gp_XYZ& anAxis)
{
  theHeight     = aHeight;
  theRadius     = aRadius;
  theFaceCenter = aCenter;
  theAxis       = anAxis;
  InitTypeAndForm(154,0);
}

    Standard_Real  IGESSolid_Cylinder::Height () const
{
  return theHeight;
}

    Standard_Real  IGESSolid_Cylinder::Radius () const
{
  return theRadius;
}

    gp_Pnt  IGESSolid_Cylinder::FaceCenter () const
{
  return gp_Pnt(theFaceCenter);
}

    gp_Pnt  IGESSolid_Cylinder::TransformedFaceCenter () const
{
  if (!HasTransf()) return gp_Pnt(theFaceCenter);
  else
    {
      gp_XYZ tmp = theFaceCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Cylinder::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_Cylinder::TransformedAxis () const
{
  if (!HasTransf()) return gp_Dir(theAxis);
  else
    {
      gp_XYZ tmp = theAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
