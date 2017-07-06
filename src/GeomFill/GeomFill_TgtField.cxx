// Created on: 1995-12-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <GeomFill_TgtField.hxx>
#include <gp_Vec.hxx>
#include <Law_BSpline.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_TgtField,Standard_Transient)

//=======================================================================
//function : IsScalable
//purpose  : 
//=======================================================================
Standard_Boolean GeomFill_TgtField::IsScalable() const 
{
  return 0;
}

//=======================================================================
//function : Scale
//purpose  : 
//=======================================================================

void GeomFill_TgtField::Scale(const Handle(Law_BSpline)& )
{
}


