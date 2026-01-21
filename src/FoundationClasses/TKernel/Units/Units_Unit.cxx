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

#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Units_Token.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsDictionary.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Unit, Standard_Transient)

//=================================================================================================

Units_Unit::Units_Unit(const char* const                  aname,
                       const char* const                  asymbol,
                       const double                       avalue,
                       const occ::handle<Units_Quantity>& aquantity)
{
  thename                                      = new TCollection_HAsciiString(aname);
  thevalue                                     = avalue;
  thequantity                                  = aquantity;
  occ::handle<TCollection_HAsciiString> symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thesymbolssequence->Prepend(symbol);
}

//=================================================================================================

Units_Unit::Units_Unit(const char* const aname, const char* const asymbol)
{
  thename                                      = new TCollection_HAsciiString(aname);
  thevalue                                     = 0.;
  occ::handle<TCollection_HAsciiString> symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  thesymbolssequence->Prepend(symbol);
}

//=================================================================================================

Units_Unit::Units_Unit(const char* const aname)
{
  thename            = new TCollection_HAsciiString(aname);
  thevalue           = 0.;
  thesymbolssequence = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
}

//=================================================================================================

void Units_Unit::Symbol(const char* const asymbol)
{
  occ::handle<TCollection_HAsciiString> symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence->Append(symbol);
}

//=================================================================================================

occ::handle<Units_Token> Units_Unit::Token() const
{
  TCollection_AsciiString string = thesymbolssequence->Value(1)->String();
  return new Units_Token(string.ToCString(), " ", thevalue, thequantity->Dimensions());
}

//=================================================================================================

bool Units_Unit::IsEqual(const char* const astring) const
{
  int                     index;
  TCollection_AsciiString symbol;

  for (index = 1; index <= thesymbolssequence->Length(); index++)
  {
    symbol = thesymbolssequence->Value(index)->String();
    if (symbol == astring)
      return true;
  }

  return false;
}

//=================================================================================================

void Units_Unit::Dump(const int /*ashift*/, const int) const
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
  std::cout << "		Name:  " << Name().ToCString() << "		(= " << thevalue << " SI)"
            << std::endl;
}

//=======================================================================
// function : operator ==
// purpose  :
//=======================================================================

bool operator==(const occ::handle<Units_Unit>& aunit, const char* const astring)
{
  return aunit->IsEqual(astring);
}
