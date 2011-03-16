#include <Interface_Protocol.ixx>

//  Gestion du Protocol actif : tres simple, une variable statique


static Handle(Interface_Protocol)& theactive()
{
  static Handle(Interface_Protocol) theact;
  return theact;
}


//=======================================================================
//function : Active
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) Interface_Protocol::Active ()
{
  return theactive();
}


//=======================================================================
//function : SetActive
//purpose  : 
//=======================================================================

void Interface_Protocol::SetActive(const Handle(Interface_Protocol)& aprotocol)
{
  theactive() = aprotocol;
}


//=======================================================================
//function : ClearActive
//purpose  : 
//=======================================================================

void Interface_Protocol::ClearActive ()
{
  theactive().Nullify();
}


//  ===   Typage (formules fournies par defaut)


//=======================================================================
//function : CaseNumber
//purpose  : 
//=======================================================================

Standard_Integer Interface_Protocol::CaseNumber
  (const Handle(Standard_Transient)& obj) const
{
  if (obj.IsNull()) return 0;
  return TypeNumber(Type(obj));
}


//=======================================================================
//function : IsDynamicType
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Protocol::IsDynamicType
  (const Handle(Standard_Transient)& /*obj*/) const
{
  return Standard_True;
}


//=======================================================================
//function : NbTypes
//purpose  : 
//=======================================================================

Standard_Integer Interface_Protocol::NbTypes
  (const Handle(Standard_Transient)& /*obj*/) const
{
  return 1;
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

Handle(Standard_Type) Interface_Protocol::Type
       (const Handle(Standard_Transient)& obj,
        const Standard_Integer /*nt*/) const
{
  if (obj.IsNull()) return STANDARD_TYPE(Standard_Transient);
  return obj->DynamicType();
}


//=======================================================================
//function : GlobalCheck
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Protocol::GlobalCheck(const Interface_Graph& /*graph*/,
                                                 Handle(Interface_Check)& /*ach*/) const
{
  return Standard_True;
}
