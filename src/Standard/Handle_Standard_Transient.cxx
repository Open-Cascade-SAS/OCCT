#include <Handle_Standard_Transient.hxx>
#include <Standard_Atomic.hxx>

//============================================================================

void Handle(Standard_Transient)::Dump(Standard_OStream& out) const
{ 
  out << Access();
}

//============================================================================

void Handle(Standard_Transient)::Assign (const Standard_Transient *anItem)
{
  Standard_Transient *anIt = ( anItem ? (Standard_Transient*)anItem : UndefinedHandleAddress );
  if ( anIt == entity ) return;
  EndScope();
  entity = anIt;
  BeginScope();
}

//============================================================================

void Handle(Standard_Transient)::BeginScope()
{
  if (entity != UndefinedHandleAddress)
  {
    Standard_Atomic_Increment (&entity->count);
  }
}

//============================================================================

void Handle(Standard_Transient)::EndScope()
{
  if (entity == UndefinedHandleAddress)
    return;
  if (Standard_Atomic_Decrement (&entity->count) == 0)
    entity->Delete();
  entity = UndefinedHandleAddress;
}
