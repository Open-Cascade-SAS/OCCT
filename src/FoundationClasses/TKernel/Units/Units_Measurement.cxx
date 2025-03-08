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

#include <Units.hxx>
#include <Units_Measurement.hxx>
#include <Units_Operators.hxx>
#include <Units_Token.hxx>
#include <Units_UnitSentence.hxx>

//=================================================================================================

Units_Measurement::Units_Measurement()
{
  themeasurement = 0.;
  myHasToken     = Standard_False;
}

//=================================================================================================

Units_Measurement::Units_Measurement(const Standard_Real avalue, const Handle(Units_Token)& atoken)
{
  themeasurement = avalue;
  thetoken       = atoken;
  myHasToken     = Standard_True;
}

//=================================================================================================

Units_Measurement::Units_Measurement(const Standard_Real avalue, const Standard_CString aunit)
{
  themeasurement = avalue;
  Units_UnitSentence unit(aunit);
  if (!unit.IsDone())
  {
#ifdef OCCT_DEBUG
    std::cout << "can not create Units_Measurement - incorrect unit" << std::endl;
#endif
    myHasToken = Standard_False;
  }
  else
  {
    thetoken = unit.Evaluate();
    thetoken->Word(aunit);
    thetoken->Mean("U");
    myHasToken = Standard_True;
  }
}

//=================================================================================================

void Units_Measurement::Convert(const Standard_CString aunit)
{
  Handle(Units_Token) oldtoken = thetoken;
  Units_UnitSentence  newunit(aunit);
  if (!newunit.IsDone())
  {
    std::cout << "Units_Measurement: can not convert - incorrect unit => result is not correct"
              << std::endl;
    return;
  }
  Handle(Units_Token)      newtoken   = newunit.Evaluate();
  Handle(Units_Token)      token      = oldtoken / newtoken;
  Handle(Units_Dimensions) dimensions = token->Dimensions();

  if (dimensions->IsEqual(Units::NullDimensions()))
  {
    thetoken = new Units_Token(aunit, "U");
    thetoken->Value(((newunit.Sequence())->Value(1))->Value());
    thetoken->Dimensions(((newunit.Sequence())->Value(1))->Dimensions());
    themeasurement = oldtoken->Multiplied(themeasurement);
    themeasurement = newtoken->Divided(themeasurement);
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << " The units don't have the same physical dimensions" << std::endl;
  }
#endif
}

//=================================================================================================

Units_Measurement Units_Measurement::Integer() const
{
  return Units_Measurement((Standard_Integer)themeasurement, thetoken);
}

//=================================================================================================

Units_Measurement Units_Measurement::Fractional() const
{
  return Units_Measurement(themeasurement - (Standard_Integer)themeasurement, thetoken);
}

//=================================================================================================

Standard_Real Units_Measurement::Measurement() const
{
  return themeasurement;
}

//=================================================================================================

Handle(Units_Token) Units_Measurement::Token() const
{
  return thetoken;
}

//=================================================================================================

Units_Measurement Units_Measurement::Add(const Units_Measurement& ameasurement) const
{
  Standard_Real     value;
  Units_Measurement measurement;
  if (thetoken->Dimensions()->IsNotEqual((ameasurement.Token())->Dimensions()))
    return measurement;
  value                     = ameasurement.Token()->Multiplied(ameasurement.Measurement());
  value                     = thetoken->Divided(value);
  value                     = themeasurement + value;
  Handle(Units_Token) token = thetoken->Creates();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Subtract(const Units_Measurement& ameasurement) const
{
  Standard_Real     value;
  Units_Measurement measurement;
  if (thetoken->Dimensions()->IsNotEqual((ameasurement.Token())->Dimensions()))
    return measurement;
  value                     = ameasurement.Token()->Multiplied(ameasurement.Measurement());
  value                     = thetoken->Divided(value);
  value                     = themeasurement - value;
  Handle(Units_Token) token = thetoken->Creates();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Multiply(const Units_Measurement& ameasurement) const
{
  Standard_Real       value = themeasurement * ameasurement.Measurement();
  Handle(Units_Token) token = thetoken * ameasurement.Token();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Multiply(const Standard_Real avalue) const
{
  Standard_Real       value = themeasurement * avalue;
  Handle(Units_Token) token = thetoken->Creates();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Divide(const Units_Measurement& ameasurement) const
{
  Standard_Real       value = themeasurement / ameasurement.Measurement();
  Handle(Units_Token) token = thetoken / ameasurement.Token();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Divide(const Standard_Real avalue) const
{
  Standard_Real       value = themeasurement / avalue;
  Handle(Units_Token) token = thetoken->Creates();
  return Units_Measurement(value, token);
}

//=================================================================================================

Units_Measurement Units_Measurement::Power(const Standard_Real anexponent) const
{
  Standard_Real       value = pow(themeasurement, anexponent);
  Handle(Units_Token) token = pow(thetoken, anexponent);
  return Units_Measurement(value, token);
}

//=================================================================================================

Standard_Boolean Units_Measurement::HasToken() const
{
  return myHasToken;
}

//=================================================================================================

void Units_Measurement::Dump() const
{
  std::cout << " Measurement : " << themeasurement << std::endl;
  thetoken->Dump(1, 1);
}

#ifdef BUG
//=======================================================================
// function : operator*
// purpose  :
//=======================================================================

Units_Measurement operator*(const Standard_Real avalue, const Units_Measurement& ameasurement)
{
  return ameasurement * avalue;
}

//=======================================================================
// function : operator/
// purpose  :
//=======================================================================

Units_Measurement operator/(const Standard_Real avalue, const Units_Measurement& ameasurement)
{
  return ameasurement / avalue;
}
#endif
