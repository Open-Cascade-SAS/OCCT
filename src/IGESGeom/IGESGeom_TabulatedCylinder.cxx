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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_TabulatedCylinder,IGESData_IGESEntity)

IGESGeom_TabulatedCylinder::IGESGeom_TabulatedCylinder ()    {  }


    void IGESGeom_TabulatedCylinder::Init
  (const Handle(IGESData_IGESEntity)& aDirectrix,
   const gp_XYZ&                      anEnd)
{
  theDirectrix = aDirectrix;
  theEnd       = anEnd;
  InitTypeAndForm(122,0);
}

    Handle(IGESData_IGESEntity) IGESGeom_TabulatedCylinder::Directrix () const 
{
  return theDirectrix;
}

    gp_Pnt IGESGeom_TabulatedCylinder::EndPoint () const
{
  return ( gp_Pnt(theEnd) );
}

    gp_Pnt IGESGeom_TabulatedCylinder::TransformedEndPoint () const
{
  gp_XYZ EndPoint = theEnd;
  if (HasTransf()) Location().Transforms(EndPoint);
  return ( gp_Pnt(EndPoint) );
}
