// Created on: 1992-11-05
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



#include <Units_ShiftedToken.ixx>
#include <TCollection_AsciiString.hxx>

#define XTRACE 1

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
#ifdef TRACE
  cout << " CREATES ShiftedToken " << endl;
  Dump(0,0);
#endif
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

void Units_ShiftedToken::Destroy () {

#ifdef TRACE
  cout << " DESTROIES ShiftedToken " << endl;
  Dump(0,0);
#endif
}
