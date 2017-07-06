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
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units.hxx>
#include <Units_Quantity.hxx>
#include <Units_Token.hxx>
#include <Units_Unit.hxx>
#include <Units_UnitsDictionary.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_Unit,Standard_Transient)

//=======================================================================
//function : Units_Unit
//purpose  : 
//=======================================================================
Units_Unit::Units_Unit(const Standard_CString aname,
		       const Standard_CString asymbol,
		       const Standard_Real avalue,
		       const Handle(Units_Quantity)& aquantity)
{
  thename       = new TCollection_HAsciiString(aname);
  thevalue      = avalue;
  thequantity = aquantity;
  Handle(TCollection_HAsciiString) symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence = new TColStd_HSequenceOfHAsciiString();
  thesymbolssequence->Prepend(symbol);
}

//=======================================================================
//function : Units_Unit
//purpose  : 
//=======================================================================

Units_Unit::Units_Unit(const Standard_CString aname,
		       const Standard_CString asymbol)
{
  thename       = new TCollection_HAsciiString(aname);
  thevalue      = 0.;
  Handle(TCollection_HAsciiString) symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence = new TColStd_HSequenceOfHAsciiString();
  thesymbolssequence->Prepend(symbol);
}

//=======================================================================
//function : Units_Unit
//purpose  : 
//=======================================================================

Units_Unit::Units_Unit(const Standard_CString aname)
{
  thename  = new TCollection_HAsciiString(aname);
  thevalue = 0.;
  thesymbolssequence = new TColStd_HSequenceOfHAsciiString();
}

//=======================================================================
//function : Symbol
//purpose  : 
//=======================================================================

void Units_Unit::Symbol(const Standard_CString asymbol)
{
  Handle(TCollection_HAsciiString) symbol = new TCollection_HAsciiString(asymbol);
  thesymbolssequence->Append(symbol);
}

//=======================================================================
//function : Token
//purpose  : 
//=======================================================================

Handle(Units_Token) Units_Unit::Token() const
{
  TCollection_AsciiString string = thesymbolssequence->Value(1)->String();
  return new Units_Token(string.ToCString()," ",thevalue,thequantity->Dimensions());
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean Units_Unit::IsEqual(const Standard_CString astring) const
{
  Standard_Integer index;
  TCollection_AsciiString symbol;

  for(index=1;index<=thesymbolssequence->Length();index++)
    {
      symbol = thesymbolssequence->Value(index)->String();
      if(symbol == astring) return Standard_True;
    }

  return Standard_False;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Units_Unit::Dump(const Standard_Integer /*ashift*/,
		      const Standard_Integer) const
{
  Standard_Integer index;
  TCollection_AsciiString string;

//  int i;
//  for(i=0; i<ashift; i++)cout<<"  ";
  for(index=1;index<=thesymbolssequence->Length();index++)
    {
      string = thesymbolssequence->Value(index)->String();
      if(index != 1) cout << " or " ;
      cout<<"\""<<string.ToCString()<<"\"";
    }
  cout<< "		Name:  " <<Name().ToCString()<<"		(= " << thevalue << " SI)" <<endl;
}

//=======================================================================
//function : operator ==
//purpose  : 
//=======================================================================

Standard_Boolean operator ==(const Handle(Units_Unit)& aunit,const Standard_CString astring)
{
  return aunit->IsEqual(astring);
}
