// File:	Dynamic_DynamicInstance.cxx
// Created:	Fri Jan 22 11:45:20 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


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

