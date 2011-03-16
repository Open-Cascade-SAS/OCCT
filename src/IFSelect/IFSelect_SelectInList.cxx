#include <IFSelect_SelectInList.ixx>

// ....    Specialisation de SelectAnyList dans laquelle on traite une liste
//         dont chaque item est une Entite


    void IFSelect_SelectInList::FillResult
  (const Standard_Integer n1, const Standard_Integer n2,
   const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& result) const
{
  for (Standard_Integer i = n1; i <= n2; i ++)
    result.GetOneItem (ListedEntity(i,ent));
}
