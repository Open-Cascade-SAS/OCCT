// Created on: 1999-06-17
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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

#include <PFunction_Function.ixx>
#include <Standard_GUID.hxx>

PFunction_Function::PFunction_Function()
{}

void PFunction_Function::SetDriverGUID(const Standard_GUID& driverGUID)
{
  myDriverGUID = driverGUID;
}

Standard_GUID PFunction_Function::GetDriverGUID() const
{
  return myDriverGUID;
}

Standard_Integer PFunction_Function::GetFailure() const
{
  return myFailure;
}

void PFunction_Function::SetFailure(const Standard_Integer mode)
{
  myFailure = mode;
}

