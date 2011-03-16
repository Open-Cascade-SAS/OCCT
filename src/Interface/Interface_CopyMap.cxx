#include <Interface_CopyMap.ixx>
#include <Interface_InterfaceError.hxx>

  //  CopyMap : rien de plus qu une Map passive


    Interface_CopyMap::Interface_CopyMap
  (const Handle(Interface_InterfaceModel)& amodel)
    : theres (0,amodel->NbEntities())
      {  themod = amodel;  }


    void Interface_CopyMap::Clear ()
{
  Standard_Integer nb = theres.Upper();
  Handle(Standard_Transient) bid;  // Null
  for (Standard_Integer i = 1; i <= nb; i ++) theres.SetValue(i,bid);
}

    Handle(Interface_InterfaceModel) Interface_CopyMap::Model () const
      {  return themod;  }

    void Interface_CopyMap::Bind
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& res)
{
  Standard_Integer num = themod->Number(ent);
  if (num == 0 || num > theres.Upper()) Interface_InterfaceError::Raise
    ("CopyMap : Bind, Starting Entity not issued from Starting Model");
  if (!theres.Value(num).IsNull()) Interface_InterfaceError::Raise
    ("CopyMap : Bind, Starting Entity already bound");
  theres.SetValue(num,res);
}


    Standard_Boolean Interface_CopyMap::Search
  (const Handle(Standard_Transient)& ent,
   Handle(Standard_Transient)& res) const
{
  Standard_Integer num = themod->Number(ent);
  if (num == 0) return Standard_False;
  res = theres.Value(num);
  return (!res.IsNull());
}
