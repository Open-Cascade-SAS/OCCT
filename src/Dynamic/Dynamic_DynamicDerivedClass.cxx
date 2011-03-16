// File:	Dynamic_DynamicDerivedClass.cxx
// Created:	Fri Feb  5 10:14:00 1993
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


#include <Dynamic_DynamicDerivedClass.ixx>

//=======================================================================
//function : Dynamic_DynamicDerivedClass
//purpose  : 
//=======================================================================

Dynamic_DynamicDerivedClass::Dynamic_DynamicDerivedClass(const Standard_CString aname)
     : Dynamic_DynamicClass(aname)
{
  thesequenceofclasses = new Dynamic_SequenceOfClasses();
}

//=======================================================================
//function : AddClass
//purpose  : 
//=======================================================================

void Dynamic_DynamicDerivedClass::AddClass(const Handle(Dynamic_DynamicClass)& aclass)
{
  thesequenceofclasses->Append(aclass);
}  

//=======================================================================
//function : Method
//purpose  : 
//=======================================================================

Handle(Dynamic_Method) Dynamic_DynamicDerivedClass::Method
       (const Standard_CString amethod) const
{
  Standard_Integer index;
  Handle(Dynamic_Method) method = Dynamic_DynamicClass::Method(amethod);

  if(method.IsNull())
    {
      for(index=1;index<=thesequenceofclasses->Length();index++)
	{
	  method = (thesequenceofclasses->Value(index))->Method(amethod);
	  if(!method.IsNull()) break;
	}
    }
  return method;
}

//=======================================================================
//function : Instance
//purpose  : 
//=======================================================================

Handle(Dynamic_DynamicInstance) Dynamic_DynamicDerivedClass::Instance() const
{
  Standard_Integer index;
  Handle(Dynamic_DynamicInstance) instance = Dynamic_DynamicClass::Instance();

  for(index=1;index<=thesequenceofclasses->Length();index++)
    {
      ((thesequenceofclasses->Value(index))->Instance())->Parameter(instance);
    }
  return instance;
}
