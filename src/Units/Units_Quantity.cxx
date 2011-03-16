// File:	Units_Quantity.cxx
// Created:	Wed Jun 24 12:48:27 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@phobox>


#include <Units_Quantity.ixx>
#include <Units_Unit.hxx>
#include <Units_Operators.hxx>


//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean Units_Quantity::IsEqual(const Standard_CString astring) const
{
  return (Name() == astring);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Units_Quantity::Dump(const Standard_Integer ashift,
			  const Standard_Integer alevel) const
{
  Standard_Integer index;
  cout<<endl;
  for(int i=0; i<ashift; i++)cout<<"  ";
  cout<<Name()<<endl;
//  thedimensions->Dump(ashift+1);
  if(alevel > 0)
    {
      for(index=1;index<=theunitssequence->Length();index++)
	theunitssequence->Value(index)->Dump(ashift+1,0);
    }
}

//=======================================================================
//function : operator ==
//purpose  : 
//=======================================================================

Standard_Boolean operator ==(const Handle(Units_Quantity)& aquantity,const Standard_CString astring)
{
  return aquantity->IsEqual(astring);
}
