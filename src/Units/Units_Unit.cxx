// Created on: 1992-06-24
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Units_Unit.ixx>
#include <Units.hxx>
#include <Units_UnitsDictionary.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

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
