// Created on: 1993-10-22
// Created by: Gilles DEBARBOUILLE
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

//              Convertir correctement les unites translatees

#include <Resource_Manager.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Units.hxx>
#include <Units_NoSuchType.hxx>
#include <Units_NoSuchUnit.hxx>
#include <Units_Operators.hxx>
#include <Units_Quantity.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_ShiftedUnit.hxx>
#include <Units_Token.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitSentence.hxx>
#include <Units_UnitsSequence.hxx>
#include <Units_UnitsSystem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_UnitsSystem, Standard_Transient)

//=================================================================================================

Units_UnitsSystem::Units_UnitsSystem()
{
  thequantitiessequence  = new Units_QuantitiesSequence();
  theactiveunitssequence = new TColStd_HSequenceOfInteger;
}

//=================================================================================================

Units_UnitsSystem::Units_UnitsSystem(const Standard_CString aName, const Standard_Boolean Verbose)
{
  Handle(Resource_Manager) themanager = new Resource_Manager(aName, Verbose);

  thequantitiessequence  = new Units_QuantitiesSequence();
  theactiveunitssequence = new TColStd_HSequenceOfInteger;
}

//=================================================================================================

Handle(Units_QuantitiesSequence) Units_UnitsSystem::QuantitiesSequence() const
{
  return thequantitiessequence;
}

//=================================================================================================

Handle(TColStd_HSequenceOfInteger) Units_UnitsSystem::ActiveUnitsSequence() const
{
  return theactiveunitssequence;
}

//=================================================================================================

void Units_UnitsSystem::Specify(const Standard_CString aquantity, const Standard_CString aunit)
{
  Standard_Integer                 index;
  Handle(Units_Unit)               unit;
  Handle(Units_UnitsSequence)      unitssequence;
  Handle(Units_Quantity)           quantity;
  Handle(Units_Quantity)           thequantity;
  Handle(Units_QuantitiesSequence) quantitiessequence;
  TCollection_AsciiString          quantityname;

  Units_UnitSentence unitsentence(aunit);
  if (!unitsentence.IsDone())
  {
    std::cout << "Units_UnitsSystem::Specify : incorrect unit" << std::endl;
    return;
  }
  Handle(Units_Token) token = unitsentence.Evaluate();

  if (token->IsKind(STANDARD_TYPE(Units_ShiftedToken)))
  {
    Handle(Units_ShiftedToken) stoken = Handle(Units_ShiftedToken)::DownCast(token);
    Handle(Units_ShiftedUnit)  sunit;
    unit = sunit = new Units_ShiftedUnit(aunit, aunit);
    sunit->Value(stoken->Value());
    sunit->Move(stoken->Move());
  }
  else
  {
    unit = new Units_Unit(aunit, aunit);
    unit->Value(token->Value());
  }

  for (index = 1; index <= thequantitiessequence->Length(); index++)
  {
    quantity = thequantitiessequence->Value(index);
    if (quantity == aquantity)
    {
      unit->Quantity(quantity);
      quantity->Sequence()->Append(unit);
      return;
    }
  }

  quantity = Units::Quantity(aquantity);

  //  Units_NoSuchType_Raise_if(quantity.IsNull(),aquantity);
  if (quantity.IsNull())
  {
    std::cout << "Warning: in Units_UnitsSystem : Units_NoSuchType '" << aquantity << "'"
              << std::endl;
    return;
  }

  unitssequence = new Units_UnitsSequence();
  quantityname  = quantity->Name();
  thequantity = new Units_Quantity(quantityname.ToCString(), quantity->Dimensions(), unitssequence);
  unit->Quantity(thequantity);
  thequantitiessequence->Append(thequantity);
  theactiveunitssequence->Append(0);
  thequantity->Sequence()->Append(unit);
}

//=================================================================================================

void Units_UnitsSystem::Remove(const Standard_CString aquantity, const Standard_CString aunit)
{
  Standard_Integer            index1, index2;
  Handle(Units_Unit)          unit;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Quantity)      quantity;

  for (index1 = 1; index1 <= thequantitiessequence->Length(); index1++)
  {

    quantity = thequantitiessequence->Value(index1);
    if (quantity == aquantity)
    {

      unitssequence = quantity->Sequence();
      for (index2 = 1; index2 <= unitssequence->Length(); index2++)
      {

        unit = unitssequence->Value(index2);
        if (unit == aunit)
        {
          unitssequence->Remove(index2);

          if (unitssequence->Length() == 0)
          {
            thequantitiessequence->Remove(index1);
            theactiveunitssequence->Remove(index1);
          }
          else
          {
            if (theactiveunitssequence->Value(index1) == index2)
              theactiveunitssequence->SetValue(index1, 0);
            else if (theactiveunitssequence->Value(index1) > index2)
              theactiveunitssequence->SetValue(index1, theactiveunitssequence->Value(index1) - 1);
            return;
          }
        }
      }

      throw Units_NoSuchUnit(aunit);
    }
  }

  throw Units_NoSuchType(aquantity);
}

//=================================================================================================

void Units_UnitsSystem::Activate(const Standard_CString aquantity, const Standard_CString aunit)
{
  Standard_Integer            index1, index2;
  Handle(Units_Unit)          unit;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Quantity)      quantity;

  for (index1 = 1; index1 <= thequantitiessequence->Length(); index1++)
  {
    quantity = thequantitiessequence->Value(index1);
    if (quantity == aquantity)
    {
      unitssequence = quantity->Sequence();
      for (index2 = 1; index2 <= thequantitiessequence->Length(); index2++)
      {
        unit = unitssequence->Value(index2);
        if (unit == aunit)
        {
          theactiveunitssequence->SetValue(index1, index2);
          return;
        }
      }
      throw Units_NoSuchUnit(aunit);
    }
  }

  throw Units_NoSuchType(aquantity);
}

//=================================================================================================

void Units_UnitsSystem::Activates()
{
  Standard_Integer            index;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Quantity)      quantity;

  for (index = 1; index <= thequantitiessequence->Length(); index++)
  {
    quantity      = thequantitiessequence->Value(index);
    unitssequence = quantity->Sequence();
    if (unitssequence->Length() > 0)
    {
      theactiveunitssequence->SetValue(index, 1);
    }
  }
}

//=================================================================================================

TCollection_AsciiString Units_UnitsSystem::ActiveUnit(const Standard_CString aquantity) const
{
  Standard_Integer            index1, index2;
  Handle(Units_Unit)          unit;
  Handle(Units_UnitsSequence) unitssequence;
  Handle(Units_Quantity)      quantity;

  for (index1 = 1; index1 <= thequantitiessequence->Length(); index1++)
  {
    quantity = thequantitiessequence->Value(index1);
    if (quantity == aquantity)
    {
      unitssequence = quantity->Sequence();
      index2        = theactiveunitssequence->Value(index1);
      if (index2)
        return unitssequence->Value(index2)->SymbolsSequence()->Value(1)->String();
      else
      {
#ifdef OCCT_DEBUG
        std::cout << " Pas d'unite active pour " << aquantity << std::endl;
#endif
        return TCollection_AsciiString();
      }
    }
  }

  throw Units_NoSuchType(aquantity);
}

//=================================================================================================

Standard_Real Units_UnitsSystem::ConvertValueToUserSystem(const Standard_CString aquantity,
                                                          const Standard_Real    avalue,
                                                          const Standard_CString aunit) const
{
  Units_UnitSentence unitsentence(aunit);
  if (!unitsentence.IsDone())
  {
    std::cout << "Units_UnitsSystem::ConvertValueToUserSystem : incorrect unit => return 0"
              << std::endl;
    return 0.;
  }
  return ConvertSIValueToUserSystem(aquantity, avalue * (unitsentence.Evaluate())->Value());
}

//=================================================================================================

Standard_Real Units_UnitsSystem::ConvertSIValueToUserSystem(const Standard_CString aquantity,
                                                            const Standard_Real    avalue) const
{
  Standard_Integer                 index, activeunit;
  Handle(Units_UnitsSequence)      unitssequence;
  Handle(Units_Quantity)           quantity;
  Handle(Units_QuantitiesSequence) quantitiessequence;
  Handle(Units_Unit)               unit;
  Handle(Units_ShiftedUnit)        sunit;
  Standard_Real                    uvalue, umove;

  for (index = 1; index <= thequantitiessequence->Length(); index++)
  {
    quantity = thequantitiessequence->Value(index);
    if (quantity == aquantity)
    {
      activeunit = theactiveunitssequence->Value(index);
      if (activeunit)
      {
        unitssequence = quantity->Sequence();
        unit          = unitssequence->Value(activeunit);
        if (unit->IsKind(STANDARD_TYPE(Units_ShiftedUnit)))
        {
          sunit  = Handle(Units_ShiftedUnit)::DownCast(unit);
          uvalue = sunit->Value();
          umove  = sunit->Move();
          return avalue / uvalue - umove;
        }
        else
        {
          uvalue = unit->Value();
          return avalue / uvalue;
        }
      }
      else
      {
        return avalue;
      }
    }
  }

  quantity = Units::Quantity(aquantity);

  Units_NoSuchType_Raise_if(quantity.IsNull(), aquantity);

  return avalue;
}

//=================================================================================================

Standard_Real Units_UnitsSystem::ConvertUserSystemValueToSI(const Standard_CString aquantity,
                                                            const Standard_Real    avalue) const
{
  Standard_Integer                 index, activeunit;
  Handle(Units_UnitsSequence)      unitssequence;
  Handle(Units_Quantity)           quantity;
  Handle(Units_QuantitiesSequence) quantitiessequence;
  Handle(Units_Unit)               unit;
  Handle(Units_ShiftedUnit)        sunit;
  Standard_Real                    uvalue, umove;

  for (index = 1; index <= thequantitiessequence->Length(); index++)
  {
    quantity = thequantitiessequence->Value(index);
    if (quantity == aquantity)
    {
      activeunit = theactiveunitssequence->Value(index);
      if (activeunit)
      {
        unitssequence = quantity->Sequence();
        unit          = unitssequence->Value(activeunit);
        if (unit->IsKind(STANDARD_TYPE(Units_ShiftedUnit)))
        {
          sunit  = Handle(Units_ShiftedUnit)::DownCast(unit);
          uvalue = sunit->Value();
          umove  = sunit->Move();
          return avalue * (uvalue + umove);
        }
        else
        {
          uvalue = unit->Value();
          return avalue * uvalue;
        }
      }
      else
      {
        return avalue;
      }
    }
  }

  quantity = Units::Quantity(aquantity);

  Units_NoSuchType_Raise_if(quantity.IsNull(), aquantity);

  return avalue;
}

//=================================================================================================

void Units_UnitsSystem::Dump() const
{
  Handle(Standard_Transient) transient   = This();
  Handle(Units_UnitsSystem)  unitssystem = Handle(Units_UnitsSystem)::DownCast(transient);
  Units_Explorer             explorer(unitssystem);
  std::cout << " UNITSSYSTEM : " << std::endl;
  for (; explorer.MoreQuantity(); explorer.NextQuantity())
  {
    std::cout << explorer.Quantity() << std::endl;
    for (; explorer.MoreUnit(); explorer.NextUnit())
      std::cout << "  " << explorer.Unit() << std::endl;
  }
}

//=================================================================================================

Standard_Boolean Units_UnitsSystem::IsEmpty() const
{
  return (thequantitiessequence->Length() > 0) ? Standard_False : Standard_True;
}
