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



#include <Dynamic_DynamicInstance.ixx>
#include <Dynamic_Parameter.hxx>
#include <Dynamic_IntegerParameter.hxx>
#include <Dynamic_RealParameter.hxx>
#include <Dynamic_StringParameter.hxx>
#include <Dynamic_InstanceParameter.hxx>
#include <Dynamic_Method.hxx>
#include <Dynamic_CompiledMethod.hxx>
#include <Dynamic_InterpretedMethod.hxx>
#include <Dynamic_SequenceOfMethods.hxx>
#include <Standard_CString.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Dynamic_DynamicInstance
//purpose  : 
//=======================================================================

Dynamic_DynamicInstance::Dynamic_DynamicInstance()
{
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter
  (const Handle(Dynamic_DynamicInstance)& aninstance) const
{
  Handle(Dynamic_ParameterNode) parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      aninstance->Parameter(parameternode->Object());
      parameternode = parameternode->Next();
    }
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter(const Handle(Dynamic_Parameter)& aparameter)
{
  Handle(Dynamic_ParameterNode) parameternode = new Dynamic_ParameterNode(aparameter);
  parameternode->Next(thefirstparameternode);
  thefirstparameternode = parameternode;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter(const Standard_CString aparameter,
					const Standard_Integer avalue) const
{
  Handle(Dynamic_IntegerParameter) parameter =
    Handle(Dynamic_IntegerParameter)::DownCast(Parameter(aparameter));
  parameter->Value(avalue);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter(const Standard_CString aparameter,
					const Standard_Real avalue) const
{
  Handle(Dynamic_RealParameter) parameter =
    Handle(Dynamic_RealParameter)::DownCast(Parameter(aparameter));
  parameter->Value(avalue);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter(const Standard_CString aparameter,
					const Standard_CString avalue) const
{
  Handle(Dynamic_StringParameter) parameter =
    Handle(Dynamic_StringParameter)::DownCast(Parameter(aparameter));
  parameter->Value(avalue);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Parameter
  (const Standard_CString aparameter,
   const Handle(Dynamic_DynamicInstance)& avalue) const
{
  Handle(Dynamic_InstanceParameter) parameter =
    Handle(Dynamic_InstanceParameter)::DownCast(Parameter(aparameter));
  parameter->Value(avalue);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Handle(Dynamic_Parameter) Dynamic_DynamicInstance::Parameter
       (const Standard_CString aparameter) const
{
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_ParameterNode) parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      TCollection_AsciiString aStr(aparameter);
      parameter = parameternode->Object();
      if(parameter->Name() == aStr) return parameter;
      parameternode = parameternode->Next();
    }
  return parameter;
}

//=======================================================================
//function : Class
//purpose  : 
//=======================================================================

void Dynamic_DynamicInstance::Class(const Handle(Dynamic_DynamicClass)& aclass)
{
  thedynamicclass = aclass;
}

//=======================================================================
//function : Execute
//purpose  : 
//=======================================================================

//extern source(Standard_Integer argc, char** argv);

void Dynamic_DynamicInstance::Execute(const Standard_CString amethod) const
{
  Handle(Dynamic_Method) method = thedynamicclass->Method(amethod);

  if(method->IsKind(STANDARD_TYPE(Dynamic_CompiledMethod)))
    {
      Handle(Dynamic_DynamicInstance) me(this) ;
      Handle(Dynamic_CompiledMethod) method_1 = Handle(Dynamic_CompiledMethod)::DownCast(method);
      typedef void (*function)(const Handle(Dynamic_DynamicInstance)&);
      
//      ((function)((*(Handle_Dynamic_CompiledMethod*)&method)->Function().ToCString()))(me);
      ((function)(method_1->Function().ToCString()))(me);
    }
  else
    {
/*      Standard_Integer argc = 2;
      char* argv[2];
      argv[0] = "source";
      argv[1] = (*(Handle_Dynamic_InterpretedMethod*)&method)->Function();
      source(argc,argv);*/
    }
}

