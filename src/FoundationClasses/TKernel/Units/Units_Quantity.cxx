// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Units_Operators.hxx>
#include <Units_Quantity.hxx>
#include <Units_Unit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Quantity, Standard_Transient)

//=================================================================================================

Standard_Boolean Units_Quantity::IsEqual(const Standard_CString astring) const
{
  return (Name() == astring);
}

//=================================================================================================

void Units_Quantity::Dump(const Standard_Integer ashift, const Standard_Integer alevel) const
{
  Standard_Integer index;
  std::cout << std::endl;
  for (int i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << Name() << std::endl;
  //  thedimensions->Dump(ashift+1);
  if (alevel > 0)
  {
    for (index = 1; index <= theunitssequence->Length(); index++)
      theunitssequence->Value(index)->Dump(ashift + 1, 0);
  }
}

//=======================================================================
// function : operator ==
// purpose  :
//=======================================================================

Standard_Boolean operator==(const Handle(Units_Quantity)& aquantity, const Standard_CString astring)
{
  return aquantity->IsEqual(astring);
}
