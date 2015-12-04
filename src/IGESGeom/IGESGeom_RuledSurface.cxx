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

#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_RuledSurface,IGESData_IGESEntity)

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
