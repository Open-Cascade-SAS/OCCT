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
#include <Units_Dimensions.hxx>
#include <Units_ShiftedToken.hxx>
#include <Units_Token.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Units_ShiftedToken,Units_Token)

//=======================================================================
//function : Units_ShiftedToken
//purpose  : 
//=======================================================================
Units_ShiftedToken::Units_ShiftedToken(const Standard_CString aword,
				       const Standard_CString amean,
				       const Standard_Real avalue,
				       const Standard_Real amove,
				       const Handle(Units_Dimensions)& adimensions)
     : Units_Token(aword,amean,avalue,adimensions)
{
  themove = amove;
}

//=======================================================================
//function : Creates
//purpose  : 
//=======================================================================

Handle(Units_Token) Units_ShiftedToken::Creates() const
{
  TCollection_AsciiString word = Word();
  TCollection_AsciiString mean = Mean();
  return new Units_ShiftedToken(word.ToCString(),mean.ToCString(),Value(),Move(),Dimensions());
}

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================

Standard_Real Units_ShiftedToken::Move() const
{
  return themove;
}

//=======================================================================
//function : Multiplied
//purpose  : 
//=======================================================================

Standard_Real Units_ShiftedToken::Multiplied (const Standard_Real avalue) const
{
  return (avalue + themove) * Value();
}

//=======================================================================
//function : Divided
//purpose  : 
//=======================================================================

Standard_Real Units_ShiftedToken::Divided (const Standard_Real avalue) const
{
  return (avalue / Value()) - themove;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Units_ShiftedToken::Dump(const Standard_Integer ashift,
			      const Standard_Integer alevel) const
{
  Units_Token::Dump(ashift,alevel);
  for(int i=0; i<ashift; i++)cout<<"  ";
  cout<<"  move  : "<<themove<<endl;
}
