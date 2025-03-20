// Created on: 1997-05-27
// Created by: Jacques GOUSSARD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _LocOpe_BuildWires_HeaderFile
#define _LocOpe_BuildWires_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
class LocOpe_WiresOnShape;

class LocOpe_BuildWires
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT LocOpe_BuildWires();

  Standard_EXPORT LocOpe_BuildWires(const TopTools_ListOfShape&        Ledges,
                                    const Handle(LocOpe_WiresOnShape)& PW);

  Standard_EXPORT void Perform(const TopTools_ListOfShape&        Ledges,
                               const Handle(LocOpe_WiresOnShape)& PW);

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT const TopTools_ListOfShape& Result() const;

protected:
private:
  Standard_Boolean     myDone;
  TopTools_ListOfShape myRes;
};

#endif // _LocOpe_BuildWires_HeaderFile
