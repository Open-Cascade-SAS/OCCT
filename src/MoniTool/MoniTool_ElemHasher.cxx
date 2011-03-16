#include <MoniTool_ElemHasher.ixx>


    Standard_Integer  MoniTool_ElemHasher::HashCode
  (const Handle(MoniTool_Element)& K, const Standard_Integer Upper)
{
  return ((K->GetHashCode() - 1) % Upper) + 1;
}

    Standard_Boolean  MoniTool_ElemHasher::IsEqual
  (const Handle(MoniTool_Element)& K1, const Handle(MoniTool_Element)& K2)
{
  if (K1.IsNull()) return Standard_False;
  return K1->Equates(K2);
}
