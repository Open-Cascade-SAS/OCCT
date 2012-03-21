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

#include <IGESGeom_Direction.ixx>
#include <gp_GTrsf.hxx>


IGESGeom_Direction::IGESGeom_Direction ()    {  }


    void IGESGeom_Direction::Init
  (const gp_XYZ& aDirection)
{
  theDirection = aDirection;
  InitTypeAndForm(123,0);
}

    gp_Vec IGESGeom_Direction::Value () const
{
  gp_Vec direction(theDirection);
  return direction;
}

    gp_Vec IGESGeom_Direction::TransformedValue () const
{
  if (!HasTransf()) return gp_Vec(theDirection);
  gp_XYZ xyz (theDirection);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
  loc.Transforms(xyz);
  return gp_Vec(xyz);
}
