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



#include <Dynamic_FuzzyClass.ixx>
#include <Dynamic_Parameter.hxx>
#include <Dynamic_BooleanParameter.hxx>
#include <Dynamic_IntegerParameter.hxx>
#include <Dynamic_RealParameter.hxx>
#include <Dynamic_StringParameter.hxx>
#include <Dynamic_ObjectParameter.hxx>


//=======================================================================
//function : Dynamic_FuzzyClass
//purpose  : 
//=======================================================================

Dynamic_FuzzyClass::Dynamic_FuzzyClass()
{}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Handle(Dynamic_ParameterNode) Dynamic_FuzzyClass::FirstParameter() const
{
  return thefirstparameternode;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter) const
{
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_ParameterNode) definition = thefirstparameternode;

  while(!definition.IsNull())
    {
      parameter = definition->Object();
      if(parameter->Name() == aparameter) return Standard_True;
      definition = definition->Next();
    }
  return Standard_False;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Handle(Dynamic_Parameter)& aparameter)
{
  Handle(Dynamic_ParameterNode) parameternode = new Dynamic_ParameterNode(aparameter);
  parameternode->Next(thefirstparameternode);
  thefirstparameternode = parameternode;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter,
				   const Standard_Boolean avalue)
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_BooleanParameter) booleanparameter;

  parameternode = thefirstparameternode;
  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  Handle(Dynamic_BooleanParameter)::DownCast(parameter)->Value(avalue);
	  return;
	}
      parameternode = parameternode->Next();
    }
  booleanparameter = new Dynamic_BooleanParameter(aparameter,avalue);
  Parameter(booleanparameter);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter,
				   const Standard_Integer avalue)
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_IntegerParameter) integerparameter;

  parameternode = thefirstparameternode;
  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  Handle(Dynamic_IntegerParameter)::DownCast(parameter)->Value(avalue);
	  return;
	}
      parameternode = parameternode->Next();
    }
  integerparameter = new Dynamic_IntegerParameter(aparameter,avalue);
  Parameter(integerparameter);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter,
				   const Standard_Real avalue)
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_RealParameter) realparameter;

  parameternode = thefirstparameternode;
  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  Handle(Dynamic_RealParameter)::DownCast(parameter)->Value(avalue);
	  return;
	}
      parameternode = parameternode->Next();
    }
  realparameter = new Dynamic_RealParameter(aparameter,avalue);
  Parameter(realparameter);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter,
				   const Standard_CString astring)
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_StringParameter) stringparameter;

  parameternode = thefirstparameternode;
  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  Handle(Dynamic_StringParameter)::DownCast(parameter)->Value(astring);
	  return;
	}
      parameternode = parameternode->Next();
    }
  stringparameter = new Dynamic_StringParameter(aparameter,astring);
  Parameter(stringparameter);
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

void Dynamic_FuzzyClass::Parameter(const Standard_CString aparameter,
				   const Handle(Standard_Transient)& anobject)
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;
  Handle(Dynamic_ObjectParameter) objectparameter;

  parameternode = thefirstparameternode;
  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  Handle(Dynamic_ObjectParameter)::DownCast(parameter)->Value(anobject);
	  return;
	}
      parameternode = parameternode->Next();
    }
  objectparameter = new Dynamic_ObjectParameter(aparameter,anobject);
  Parameter(objectparameter);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Value(const Standard_CString aparameter,
					   Standard_Boolean& avalue) const
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  avalue = (*(Handle_Dynamic_BooleanParameter*)&parameter)->Value();
	  return Standard_True;
	}
      parameternode = parameternode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Value(const Standard_CString aparameter,
					   Standard_Integer& avalue) const
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  avalue = (*(Handle_Dynamic_IntegerParameter*)&parameter)->Value();
	  return Standard_True;
	}
      parameternode = parameternode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Value(const Standard_CString aparameter,
					   Standard_Real& avalue) const
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  avalue = (*(Handle_Dynamic_RealParameter*)&parameter)->Value();
	  return Standard_True;
	}
      parameternode = parameternode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Value(const Standard_CString aparameter,
					   TCollection_AsciiString& avalue) const
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  avalue = (*(Handle_Dynamic_StringParameter*)&parameter)->Value();
	  return Standard_True;
	}
      parameternode = parameternode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_FuzzyClass::Value(const Standard_CString aparameter,
					   Handle(Standard_Transient)& anobject) const
{
  Handle(Dynamic_ParameterNode) parameternode;
  Handle(Dynamic_Parameter) parameter;

  parameternode = thefirstparameternode;

  while(!parameternode.IsNull())
    {
      parameter = parameternode->Object();
      if(parameter->Name() == aparameter)
	{
	  anobject = (*(Handle_Dynamic_ObjectParameter*)&parameter)->Value();
	  return Standard_True;
	}
      parameternode = parameternode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

//void Dynamic_FuzzyClass::Dump(Standard_OStream& astream) const
void Dynamic_FuzzyClass::Dump(Standard_OStream& ) const
{}







