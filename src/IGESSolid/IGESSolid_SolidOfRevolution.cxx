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

#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_SolidOfRevolution.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_SolidOfRevolution,IGESData_IGESEntity)

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
