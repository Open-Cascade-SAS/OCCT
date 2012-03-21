// Created on: 1993-01-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <Dynamic_DynamicClass.ixx>
#include <Dynamic_DynamicInstance.hxx>
#include <Dynamic_Parameter.hxx>
#include <Dynamic_CompiledMethod.hxx>
#include <Dynamic_InterpretedMethod.hxx>
#include <Dynamic_SequenceOfMethods.hxx>
#include <TCollection_AsciiString.hxx>


//=======================================================================
//function : Dynamic_DynamicClass
//purpose  : 
//=======================================================================

Dynamic_DynamicClass::Dynamic_DynamicClass(const Standard_CString aname)
{
  thename = new TCollection_HAsciiString(aname);
  thesequenceofmethods = new Dynamic_SequenceOfMethods();
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicClass::Parameter(const Handle(Dynamic_Parameter)& aparameter)
{
  Handle(Dynamic_ParameterNode) parameternode = new Dynamic_ParameterNode(aparameter);
  parameternode->Next(thefirstparameternode);
  thefirstparameternode = parameternode;
}

//=======================================================================
//function : CompiledMethod
//purpose  : 
//=======================================================================

void Dynamic_DynamicClass::CompiledMethod(const Standard_CString amethod,
					  const Standard_CString anaddress)
{
  TCollection_AsciiString string = thename->String();
  string = string + "_" + amethod;
  Handle(Dynamic_CompiledMethod) method = new Dynamic_CompiledMethod(string.ToCString(),anaddress);
  thesequenceofmethods->Append(method);
}

//=======================================================================
//function : InterpretedMethod
//purpose  : 
//=======================================================================

void Dynamic_DynamicClass::InterpretedMethod(const Standard_CString amethod,
					     const Standard_CString afile)
{
  TCollection_AsciiString string = thename->String();
  string = string + "_" + amethod;
  Handle(Dynamic_InterpretedMethod) method =
    new Dynamic_InterpretedMethod(string.ToCString(),afile); 
  thesequenceofmethods->Append(method);
}

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_DynamicClass::Method(const Standard_CString amethod) const
{
  Standard_Integer index;
  TCollection_AsciiString methodname(amethod);
  Handle(Dynamic_Method) method;
  Handle(Dynamic_Method) nullmethod;
  Handle(Dynamic_SequenceOfMethods) sequenceofmethods = thesequenceofmethods;

  if(methodname.Search("_") == -1)
    {
      methodname = thename->String();
      methodname = methodname + "_" + amethod;
    }

  for(index=1;index<=thesequenceofmethods->Length();index++)
    {
      method = thesequenceofmethods->Value(index);
      if(method->Type() == methodname) return method;
    }
  return nullmethod;
}

//=======================================================================
//function : Instance
//purpose  : 
//=======================================================================

Handle(Dynamic_DynamicInstance) Dynamic_DynamicClass::Instance() const
{
  Handle(Dynamic_DynamicInstance) instance = new Dynamic_DynamicInstance();
  Handle(Dynamic_DynamicClass) me(this);

  Handle(Dynamic_ParameterNode) parameternode;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      instance->Parameter(parameternode->Object());
      parameternode = parameternode->Next();
    }

  instance->Class(me);

  return instance;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

//void Dynamic_DynamicClass::Dump(Standard_OStream& astream) const
void Dynamic_DynamicClass::Dump(Standard_OStream& ) const
{}
