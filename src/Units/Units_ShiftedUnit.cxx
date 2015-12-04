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
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Units_Quantity.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_ShiftedUnit.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_ShiftedUnit,Units_Unit)

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

