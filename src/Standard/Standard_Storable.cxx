#include <Standard_Storable.ixx>

void Standard_Storable::Delete()
{}

//========================================================================
Standard_Integer Standard_Storable::HashCode(const Standard_Integer Upper)const
{
  return ::HashCode((Standard_Address) this, Upper, STANDARD_TYPE(Standard_Storable));
}

//========================================================================
Standard_Boolean Standard_Storable::IsEqual(const Standard_Storable& ) const
{
  return Standard_True;
}

//========================================================================
Standard_Boolean Standard_Storable::IsSimilar(const Standard_Storable& ) const
{
  return Standard_True;
}

//========================================================================
void Standard_Storable::ShallowDump(Standard_OStream& ) const
{
//  ::ShallowDump((Standard_Address) this, STANDARD_TYPE(Standard_Storable), S);
}

