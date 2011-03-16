#include <Standard_Transient.hxx>

//
// The Initialization of the Standard_Transient variables
//
IMPLEMENT_STANDARD_TYPE(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Standard_Transient)

IMPLEMENT_STANDARD_RTTI(Standard_Transient)

//
// The Standard_Transient Methods
//

// The Method This 
//
Handle_Standard_Transient Standard_Transient::This() const
{
  return Handle_Standard_Transient(this);
}

// Empty Destructor
//
Standard_Transient::~Standard_Transient()
{
}

//
//
Standard_Boolean Standard_Transient::IsInstance(const Handle(Standard_Type) &AType) const
{
  return (Standard_Boolean) (AType ==  DynamicType());
}

//
//
Standard_Boolean Standard_Transient::IsInstance(const Standard_CString theTypeName) const
{
  return IsSimilar ( DynamicType()->Name(), theTypeName );
}

//
//
Standard_Boolean Standard_Transient::IsKind (const Handle(Standard_Type)& aType) const
{
  return DynamicType()->SubType ( aType );
}

//
//
Standard_Boolean Standard_Transient::IsKind (const Standard_CString theTypeName) const
{
  return DynamicType()->SubType ( theTypeName );
}

//============================================================================
Standard_Integer Standard_Transient::HashCode(const Standard_Integer Lim) const
{
  return ::HashCode(this, Lim);
}


void Standard_Transient::ShallowDump(Standard_OStream& theStream) const
{
  theStream << "class " << DynamicType()->Name() << " at " << this << endl;
}


void Standard_Transient::Delete() const
{ 
  delete((Standard_Transient *)this); 
}
