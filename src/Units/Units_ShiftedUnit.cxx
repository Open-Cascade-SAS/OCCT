// File:	Units_ShiftedUnit.cxx
// Created:	Wed Nov  4 17:46:04 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


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

