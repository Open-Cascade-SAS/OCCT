// Created on: 1994-05-09
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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


#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units_Explorer.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitsSystem.hxx>

//=======================================================================
//function : Units_Explorer
//purpose  : 
//=======================================================================
Units_Explorer::Units_Explorer()
{
  thecurrentquantity = 1;
  thecurrentunit = 1;
}

//=======================================================================
//function : Units_Explorer
//purpose  : 
//=======================================================================

Units_Explorer::Units_Explorer(const Handle(Units_UnitsSystem)& aunitssystem)
{
  Init(aunitssystem);
}

//=======================================================================
//function : Units_Explorer
//purpose  : 
//=======================================================================

Units_Explorer::Units_Explorer(const Handle(Units_UnitsDictionary)& aunitsdictionary)
{
  Init(aunitsdictionary);
}

//=======================================================================
//function : Units_Explorer
//purpose  : 
//=======================================================================

Units_Explorer::Units_Explorer(const Handle(Units_UnitsSystem)& aunitssystem,
			       const Standard_CString aquantity)
{
  Init(aunitssystem,aquantity);
}

//=======================================================================
//function : Units_Explorer
//purpose  : 
//=======================================================================

Units_Explorer::Units_Explorer(const Handle(Units_UnitsDictionary)& aunitsdictionary,
			       const Standard_CString aquantity)
{
  Init(aunitsdictionary,aquantity);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Units_Explorer::Init(const Handle(Units_UnitsSystem)& aunitssystem)
{
  thecurrentquantity = 1;
  thequantitiessequence = aunitssystem->QuantitiesSequence();
  theactiveunitssequence = aunitssystem->ActiveUnitsSequence();
  if(MoreQuantity())theunitssequence = thequantitiessequence->Value(thecurrentquantity)->Sequence();
  thecurrentunit = 1;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Units_Explorer::Init(const Handle(Units_UnitsDictionary)& aunitsdictionary)
{
  Standard_Integer index;
  thecurrentquantity = 1;
  thequantitiessequence = aunitsdictionary->Sequence();
  theactiveunitssequence = new TColStd_HSequenceOfInteger();
  for(index=1; index<=thequantitiessequence->Length(); index++)
    {
      theactiveunitssequence->Append(1);
    }

  if(MoreQuantity())theunitssequence = thequantitiessequence->Value(thecurrentquantity)->Sequence();
  thecurrentunit = 1;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Units_Explorer::Init(const Handle(Units_UnitsSystem)& aunitssystem,
			  const Standard_CString aquantity)
{
  Standard_Integer index;
  thecurrentquantity = 0;
  thequantitiessequence = aunitssystem->QuantitiesSequence();
  theactiveunitssequence = aunitssystem->ActiveUnitsSequence();
  for(index=1; index<=thequantitiessequence->Length(); index++)
    {
      if(thequantitiessequence->Value(index)->Name() == aquantity)
	{
	  thecurrentquantity = index;
	  thecurrentunit = 1;
	  theunitssequence = thequantitiessequence->Value(index)->Sequence();
	  return;
	}
    }

#ifdef OCCT_DEBUG
  cout<<" La grandeur physique "<<aquantity<<" n'existe pas."<<endl;
#endif
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Units_Explorer::Init(const Handle(Units_UnitsDictionary)& aunitsdictionary,
			  const Standard_CString aquantity)
{
  Handle(Units_Quantity) quantity;
  Standard_Integer index;
  thecurrentquantity = 0;
  thequantitiessequence = aunitsdictionary->Sequence();
  theactiveunitssequence = new TColStd_HSequenceOfInteger();
  for(index=1; index<=thequantitiessequence->Length(); index++)
    {
      theactiveunitssequence->Append(1);
    }

  for(index=1; index<=thequantitiessequence->Length(); index++)
    {
      quantity = thequantitiessequence->Value(index);
      if(quantity->Name() == aquantity)
	{
	  thecurrentquantity = index;
	  thecurrentunit = 1;
	  theunitssequence = thequantitiessequence->Value(index)->Sequence();
	  return;
	}
    }

#ifdef OCCT_DEBUG
  cout<<" La grandeur physique "<<aquantity<<" n'existe pas."<<endl;
#endif
}

//=======================================================================
//function : MoreQuantity
//purpose  : 
//=======================================================================

Standard_Boolean Units_Explorer::MoreQuantity() const
{
  return thecurrentquantity <= thequantitiessequence->Length() ? Standard_True : Standard_False;
}

//=======================================================================
//function : NextQuantity
//purpose  : 
//=======================================================================

void Units_Explorer::NextQuantity()
{
  thecurrentquantity++;
  thecurrentunit = 1;
  if(MoreQuantity())theunitssequence = thequantitiessequence->Value(thecurrentquantity)->Sequence();
}

//=======================================================================
//function : Quantity
//purpose  : 
//=======================================================================

TCollection_AsciiString Units_Explorer::Quantity() const
{
  return thequantitiessequence->Value(thecurrentquantity)->Name();
}

//=======================================================================
//function : MoreUnit
//purpose  : 
//=======================================================================

Standard_Boolean Units_Explorer::MoreUnit() const
{
  return thecurrentunit <= theunitssequence->Length() ? Standard_True : Standard_False;
}

//=======================================================================
//function : NextUnit
//purpose  : 
//=======================================================================

void Units_Explorer::NextUnit()
{
  thecurrentunit++;
}

//=======================================================================
//function : Unit
//purpose  : 
//=======================================================================

TCollection_AsciiString Units_Explorer::Unit() const
{
  return theunitssequence->Value(thecurrentunit)->SymbolsSequence()->Value(1)->String();
}

//=======================================================================
//function : IsActive
//purpose  : 
//=======================================================================

Standard_Boolean Units_Explorer::IsActive() const
{
  return theactiveunitssequence->Value(thecurrentquantity) == thecurrentunit ?
    Standard_True : Standard_False;
}
