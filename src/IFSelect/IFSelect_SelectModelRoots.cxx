#include <IFSelect_SelectModelRoots.ixx>


    IFSelect_SelectModelRoots::IFSelect_SelectModelRoots ()    {  }

    Interface_EntityIterator  IFSelect_SelectModelRoots::RootResult
  (const Interface_Graph& G) const
      {  return G.RootEntities();  }

    TCollection_AsciiString  IFSelect_SelectModelRoots::Label () const
{  return TCollection_AsciiString("Root (not shared) Entities from Model");  }
