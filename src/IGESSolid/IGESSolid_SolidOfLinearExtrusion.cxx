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

#include <IGESSolid_SolidOfLinearExtrusion.ixx>
#include <gp_GTrsf.hxx>


IGESSolid_SolidOfLinearExtrusion::IGESSolid_SolidOfLinearExtrusion ()   { }


    void  IGESSolid_SolidOfLinearExtrusion::Init
  (const Handle(IGESData_IGESEntity)& aCurve,
   const Standard_Real Length, const gp_XYZ& Direction)
{
  theCurve     = aCurve;
  theLength    = Length;
  theDirection = Direction;           // default (0,0,1)
  InitTypeAndForm(164,0);
}

    Handle(IGESData_IGESEntity)  IGESSolid_SolidOfLinearExtrusion::Curve () const
{
  return theCurve;
}

    Standard_Real  IGESSolid_SolidOfLinearExtrusion::ExtrusionLength () const
{
  return theLength;
}

    gp_Dir  IGESSolid_SolidOfLinearExtrusion::ExtrusionDirection () const
{
  return gp_Dir(theDirection);
}

    gp_Dir  IGESSolid_SolidOfLinearExtrusion::TransformedExtrusionDirection () const
{
  if (!HasTransf()) return gp_Dir(theDirection);
  else
    {
      gp_XYZ tmp = theDirection;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
