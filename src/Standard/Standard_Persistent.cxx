
#include <Handle_Standard_Persistent.hxx>
#include <Standard_Persistent.hxx>

#include <Standard_Failure.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ImmutableObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

//
// The Initialization of the Standard_Persistent variables
//
Handle_Standard_Type& Standard_Persistent_Type_() 
{
  static const Handle_Standard_Type _Ancestors[]={NULL};
  static Handle_Standard_Type _aType 
    = new Standard_Type("Standard_Persistent",
			sizeof(Standard_Persistent),
			0,
     (Standard_Address) _Ancestors,
			NULL);

  return _aType;
}

//
// The Standard_Persistent Methods
//

// The Method This 
//
Handle_Standard_Persistent Standard_Persistent::This() const
{
  Handle_Standard_Persistent aHand(this);

  return aHand;
}

// The Method ShallowCopy 
//
Handle_Standard_Persistent Standard_Persistent::ShallowCopy() const
{
  //the method must be redefined in the every subclass that is expected to use it
  Standard_NotImplemented::Raise ("The ShallowCopy() method must be redefined in the subclass");
  return This(); // for compilation only
}

// Empty Constructor
//
// Standard_Persistent::Standard_Persistent()
// {
// }

// Empty Destructor
//
Standard_Persistent::~Standard_Persistent()
{
}

// Constructor from a Standard_Persistent
//
// Standard_Persistent::Standard_Persistent(const Standard_Persistent& )
// { 
// }

// Operator= with a Standard_Persistent
//
Standard_Persistent& Standard_Persistent::operator=(const Standard_Persistent& )
{ 
  return *this;
}

//
//
Standard_Boolean Standard_Persistent::IsInstance(const Handle(Standard_Type)
						&AType) const
{
  return (Standard_Boolean) (AType ==  DynamicType());
}

//
//
const Handle(Standard_Type)& Standard_Persistent::DynamicType () const
{  
  return  STANDARD_TYPE(Standard_Persistent);
}

//
//
Standard_Boolean Standard_Persistent::IsKind (const Handle(Standard_Type)& aType) const
{
  return DynamicType()->SubType ( aType );
//  return  (aType == STANDARD_TYPE(Standard_Persistent));
}

//============================================================================
Standard_Integer Standard_Persistent::HashCode(const Standard_Integer Lim)const
{
  Handle(Standard_Persistent) me = this;
  return ::HashCode(me, Lim);
}


void Standard_Persistent::ShallowDump(Standard_OStream& AStream) const
{
  Handle(Standard_Persistent) me = this;
  ::ShallowDump(me, AStream);
}


void Standard_Persistent::Delete() const
 { 
   delete((Standard_Persistent *)this); 
 }

const Handle(Standard_Persistent) Handle(Standard_Persistent)::DownCast(const Handle(Standard_Persistent)& AnObject) 
{
  Handle(Standard_Persistent) _anOtherObject;
  
  if (!AnObject.IsNull()) 
    if (AnObject->IsKind(STANDARD_TYPE(Standard_Persistent))) 
      {
	_anOtherObject = Handle(Standard_Persistent)((Handle(Standard_Persistent)&)AnObject);
      }
  
  return _anOtherObject ;
}

Handle(Standard_Persistent)::~Handle(Standard_Persistent)()
{
 EndScope();
}

void Handle_Standard_Persistent::RaiseNullObject(const Standard_CString S) const
{ 
  Standard_NullObject::Raise(S);
}

void Handle(Standard_Persistent)::Dump(Standard_OStream& out) const
{ 
  out << ControlAccess();
}


