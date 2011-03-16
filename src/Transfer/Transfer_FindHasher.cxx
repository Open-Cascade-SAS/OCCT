#include <Transfer_FindHasher.ixx>


    Standard_Integer  Transfer_FindHasher::HashCode
  (const Handle(Transfer_Finder)& K, const Standard_Integer Upper)
{
  return ((K->GetHashCode() - 1) % Upper) + 1;
}

    Standard_Boolean  Transfer_FindHasher::IsEqual
  (const Handle(Transfer_Finder)& K1, const Handle(Transfer_Finder)& K2)
{
  if (K1.IsNull()) return Standard_False;
  return K1->Equates(K2);
}
