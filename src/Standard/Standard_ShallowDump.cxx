#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_Persistent.hxx>

// now with these two proto, we don't need the include of Standard_Persistent
//
class Handle(Standard_Persistent);
Handle_Standard_Type &Standard_Persistent_Type_();

//============================================================================
void ShallowDump(const Handle(Standard_Persistent)& me, Standard_OStream &aOut)
{
  Handle(Standard_Type) aType = me->DynamicType();

  aOut << "class " << aType->Name() << endl;
}

//============================================================================
void ShallowDump(const Handle(Standard_Transient)& me, Standard_OStream &aOut)
{
  Handle(Standard_Type) aType = me->DynamicType();

  aOut << "class " << aType->Name() << endl;
}

