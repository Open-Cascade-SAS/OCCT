// File:	Units_ShiftedToken.cxx
// Created:	Thu Nov  5 15:23:49 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


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
