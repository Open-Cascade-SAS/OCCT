// Created on: 1992-11-05
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
#include <TCollection_AsciiString.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_ShiftedToken, Units_Token)

//=================================================================================================

Units_ShiftedToken::Units_ShiftedToken(const char*                          aword,
                                       const char*                          amean,
                                       const double                         avalue,
                                       const double                         amove,
                                       const occ::handle<Units_Dimensions>& adimensions)
    : Units_Token(aword, amean, avalue, adimensions)
{
  themove = amove;
}

//=================================================================================================

occ::handle<Units_Token> Units_ShiftedToken::Creates() const
{
  TCollection_AsciiString word = Word();
  TCollection_AsciiString mean = Mean();
  return new Units_ShiftedToken(word.ToCString(), mean.ToCString(), Value(), Move(), Dimensions());
}

//=================================================================================================

double Units_ShiftedToken::Move() const
{
  return themove;
}

//=================================================================================================

double Units_ShiftedToken::Multiplied(const double avalue) const
{
  return (avalue + themove) * Value();
}

//=================================================================================================

double Units_ShiftedToken::Divided(const double avalue) const
{
  return (avalue / Value()) - themove;
}

//=================================================================================================

void Units_ShiftedToken::Dump(const int ashift, const int alevel) const
{
  Units_Token::Dump(ashift, alevel);
  for (int i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "  move  : " << themove << std::endl;
}
