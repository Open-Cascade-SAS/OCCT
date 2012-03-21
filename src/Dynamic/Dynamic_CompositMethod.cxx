// Created on: 1994-08-26
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

// CRD : 15/04/97 : Passage WOK++ : Remplacement de TYPE par STANDARD_TYPE

#include <Dynamic_CompositMethod.ixx>


//=======================================================================
//function : Dynamic_CompositMethod
//purpose  : 
//=======================================================================

Dynamic_CompositMethod::Dynamic_CompositMethod(const Standard_CString aname)
: Dynamic_MethodDefinition(aname)
{
  thesequenceofmethods = new Dynamic_SequenceOfMethods();
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

void Dynamic_CompositMethod::Method(const Handle(Dynamic_Method)& amethod)
{
  if(amethod->IsKind(STANDARD_TYPE(Dynamic_MethodDefinition)))
    cout<<"bad argument type"<<endl;
  else
    thesequenceofmethods->Append(amethod);
}

//=======================================================================
//function : NumberOfMethods
//purpose  : 
//=======================================================================

Standard_Integer Dynamic_CompositMethod::NumberOfMethods() const
{
  return thesequenceofmethods->Length();
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_CompositMethod::Method(const Standard_Integer anindex) const
{
  return thesequenceofmethods->Value(anindex);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_CompositMethod::Dump(Standard_OStream& astream) const
{
  astream << "CompositMethod : " << endl;
  Dynamic_MethodDefinition::Dump(astream);
  astream << "Dump of Methods Instances : " << endl;
  for (Standard_Integer i=1; i<= thesequenceofmethods->Length(); i++) {
    astream << "Method No : " << i << endl;
    thesequenceofmethods->Value(i)->Dump(astream);
    astream << endl;
  }
}
