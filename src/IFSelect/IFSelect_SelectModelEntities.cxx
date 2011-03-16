#include <IFSelect_SelectModelEntities.ixx>
#include <Interface_InterfaceModel.hxx>


    IFSelect_SelectModelEntities::IFSelect_SelectModelEntities ()    {  }

    Interface_EntityIterator  IFSelect_SelectModelEntities::RootResult
  (const Interface_Graph& G) const
      {  return G.Model()->Entities();  }

    Interface_EntityIterator  IFSelect_SelectModelEntities::CompleteResult
  (const Interface_Graph& G) const 
      {  return G.Model()->Entities();  }

    TCollection_AsciiString  IFSelect_SelectModelEntities::Label () const 
{  return TCollection_AsciiString("All Entities from Model");  }
