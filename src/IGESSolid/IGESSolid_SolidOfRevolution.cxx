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

#include <IGESSolid_SolidOfRevolution.ixx>
#include <gp_GTrsf.hxx>


IGESSolid_SolidOfRevolution::IGESSolid_SolidOfRevolution ()    {  }


    void  IGESSolid_SolidOfRevolution::Init
  (const Handle(IGESData_IGESEntity)& aCurve, const Standard_Real Fract,
   const gp_XYZ& AxisPnt, const gp_XYZ& Direction)
{
  theCurve     = aCurve;
  theFraction  = Fract;           // default 1.0
  theAxisPoint = AxisPnt;         // default (0,0,0)
  theAxis      = Direction;       // default (0,0,1)
  InitTypeAndForm(162,FormNumber());
// Form 0 : Curve closed to Axis;   Form 1 : Curve closed to itself
}

    void  IGESSolid_SolidOfRevolution::SetClosedToAxis (const Standard_Boolean F)
{
  InitTypeAndForm(162, (F ? 0 : 1));
}

    Standard_Boolean IGESSolid_SolidOfRevolution::IsClosedToAxis () const
{
  return (FormNumber() == 0);
}


    Handle(IGESData_IGESEntity)  IGESSolid_SolidOfRevolution::Curve () const
{
  return theCurve;
}

    Standard_Real  IGESSolid_SolidOfRevolution::Fraction () const
{
  return theFraction;
}

    gp_Pnt  IGESSolid_SolidOfRevolution::AxisPoint () const
{
  return gp_Pnt(theAxisPoint);
}

    gp_Pnt  IGESSolid_SolidOfRevolution::TransformedAxisPoint () const
{
  if (!HasTransf()) return gp_Pnt(theAxisPoint);
  else
    {
      gp_XYZ tmp = theAxisPoint;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_SolidOfRevolution::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_SolidOfRevolution::TransformedAxis () const
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
