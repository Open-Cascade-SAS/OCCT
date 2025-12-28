// Created on: 1992-11-04
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

#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Units_Quantity.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_ShiftedUnit.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_ShiftedUnit, Units_Unit)

//=================================================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const char*                        aname,
                                     const char*                        asymbol,
                                     const double                       avalue,
                                     const double                       amove,
                                     const occ::handle<Units_Quantity>& aquantity)
    : Units_Unit(aname, asymbol, avalue, aquantity)
{
  themove = amove;
}

//=================================================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const char* aname, const char* asymbol)
    : Units_Unit(aname, asymbol),
      themove(0.0)
{
}

//=================================================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const char* aname)
    : Units_Unit(aname),
      themove(0.0)
{
}

//=================================================================================================

void Units_ShiftedUnit::Move(const double amove)
{
  themove = amove;
}

//=================================================================================================

double Units_ShiftedUnit::Move() const
{
  return themove;
}

//=================================================================================================

occ::handle<Units_Token> Units_ShiftedUnit::Token() const
{
  TCollection_AsciiString string = SymbolsSequence()->Value(1)->String();
  return new Units_ShiftedToken(string.ToCString(),
                                " ",
                                Value(),
                                themove,
                                Quantity()->Dimensions());
}

//=================================================================================================

// void Units_ShiftedUnit::Dump(const int ashift,
//			     const int alevel) const
void Units_ShiftedUnit::Dump(const int, const int) const
{
  int                     index;
  TCollection_AsciiString string;

  //  int i;
  //  for(i=0; i<ashift; i++)std::cout<<"  ";
  for (index = 1; index <= thesymbolssequence->Length(); index++)
  {
    string = thesymbolssequence->Value(index)->String();
    if (index != 1)
      std::cout << " or ";
    std::cout << "\"" << string.ToCString() << "\"";
  }
  std::cout << "		Name:  " << Name().ToCString() << "		(= *" << thevalue << " SI + "
            << themove << ")" << std::endl;
}
