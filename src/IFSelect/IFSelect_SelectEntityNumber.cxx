#include <IFSelect_SelectEntityNumber.ixx>
#include <Interface_InterfaceModel.hxx>


    IFSelect_SelectEntityNumber::IFSelect_SelectEntityNumber ()    {  }

    void  IFSelect_SelectEntityNumber::SetNumber
  (const Handle(IFSelect_IntParam)& num)
      {  thenum = num;  }

    Handle(IFSelect_IntParam)  IFSelect_SelectEntityNumber::Number () const 
      {  return thenum;  }

    Interface_EntityIterator  IFSelect_SelectEntityNumber::RootResult
  (const Interface_Graph& G) const 
{
  Interface_EntityIterator iter;
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  if (num < 1) return iter;        // vide si num < 1 ou num > NbEntities
  if (num <= G.Size()) iter.GetOneItem(G.Entity(num));
  return iter;
}

    TCollection_AsciiString  IFSelect_SelectEntityNumber::Label () const 
{
  Standard_Integer num = 0;
  if (!thenum.IsNull()) num = thenum->Value();
  TCollection_AsciiString labl(num);
  labl.InsertBefore (1,"Entity Number ");
  return labl;
}
