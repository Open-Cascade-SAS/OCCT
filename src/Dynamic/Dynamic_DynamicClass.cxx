// File:	Dynamic_DynamicClass.cxx
// Created:	Fri Jan 22 11:45:20 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


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
