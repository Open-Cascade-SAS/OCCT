// Created on: 1992-11-04
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



#include <Units_ShiftedUnit.ixx>
#include <Units_ShiftedToken.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

//=======================================================================
//function : Units_ShiftedUnit
//purpose  : 
//=======================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const Standard_CString aname,
				     const Standard_CString asymbol,
				     const Standard_Real avalue,
				     const Standard_Real amove,
				     const Handle(Units_Quantity)& aquantity)
     : Units_Unit(aname,asymbol,avalue,aquantity)
{
  themove = amove;
}

//=======================================================================
//function : Units_ShiftedUnit
//purpose  : 
//=======================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const Standard_CString aname,
				     const Standard_CString asymbol)
     : Units_Unit(aname,asymbol)
{}

//=======================================================================
//function : Units_ShiftedUnit
//purpose  : 
//=======================================================================

Units_ShiftedUnit::Units_ShiftedUnit(const Standard_CString aname)
     : Units_Unit(aname)
{}

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================

void Units_ShiftedUnit::Move(const Standard_Real amove)
{
  themove = amove;
}

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================

Standard_Real Units_ShiftedUnit::Move() const
{
  return themove;
}

//=======================================================================
//function : Token
//purpose  : 
//=======================================================================

Handle(Units_Token) Units_ShiftedUnit::Token() const
{
  TCollection_AsciiString string = SymbolsSequence()->Value(1)->String();
  return new Units_ShiftedToken(string.ToCString()," ",Value(),themove,Quantity()->Dimensions());
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

//void Units_ShiftedUnit::Dump(const Standard_Integer ashift,
//			     const Standard_Integer alevel) const
void Units_ShiftedUnit::Dump(const Standard_Integer ,
			     const Standard_Integer ) const
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
  cout<< "		Name:  " <<Name().ToCString()<<"		(= *" << thevalue << " SI + " << themove << ")"<<endl;
}

