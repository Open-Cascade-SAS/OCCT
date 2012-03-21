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

#include <IGESGeom_RuledSurface.ixx>


IGESGeom_RuledSurface::IGESGeom_RuledSurface ()    {  }


    void IGESGeom_RuledSurface::Init
  (const Handle(IGESData_IGESEntity)& aCurve,
   const Handle(IGESData_IGESEntity)& anotherCurve,
   const Standard_Integer             aDirFlag,
   const Standard_Integer             aDevFlag)
{
  theCurve1  = aCurve;
  theCurve2  = anotherCurve;
  theDirFlag = aDirFlag;
  theDevFlag = aDevFlag;
  InitTypeAndForm(118,FormNumber());
//    FormNumber 0-1 : Ruling spec.  0/Arc Length  1/Parameter
}

    void  IGESGeom_RuledSurface::SetRuledByParameter (const Standard_Boolean F)
{
  InitTypeAndForm(118, (F ? 1 : 0));
}

    Handle(IGESData_IGESEntity) IGESGeom_RuledSurface::FirstCurve () const
{
  return theCurve1;
}

    Handle(IGESData_IGESEntity) IGESGeom_RuledSurface::SecondCurve () const
{
  return theCurve2;
}

    Standard_Integer IGESGeom_RuledSurface::DirectionFlag () const
{
  return theDirFlag;
}

    Standard_Boolean IGESGeom_RuledSurface::IsDevelopable () const
{
  return (theDevFlag == 1);
}

    Standard_Boolean  IGESGeom_RuledSurface::IsRuledByParameter () const
{
  return (FormNumber() != 0);
}
