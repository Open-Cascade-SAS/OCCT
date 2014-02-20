// Created on: 1994-08-26
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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
