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
