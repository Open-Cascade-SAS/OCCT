// Created on: 1993-01-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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







