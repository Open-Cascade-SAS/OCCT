// Created on: 2012-02-10
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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


#include <Bnd_Box2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Intf_Polygon2d.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================
Standard_Boolean Intf_Polygon2d::Closed () const
{
  return Standard_False;
}
