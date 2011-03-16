#include <IFSelect_SelectFlag.ixx>


    IFSelect_SelectFlag::IFSelect_SelectFlag (const Standard_CString flagname)
    : thename (flagname)    {  }

    Standard_CString  IFSelect_SelectFlag::FlagName () const
      {  return thename.ToCString();  }


    Interface_EntityIterator  IFSelect_SelectFlag::RootResult
  (const Interface_Graph& G) const
{
  Standard_Boolean direct = IsDirect();
  Interface_EntityIterator res;
  const Interface_BitMap& bm = G.BitMap();
  Standard_Integer flag = bm.FlagNumber (thename.ToCString());
  if (flag == 0) return res;
  Interface_EntityIterator inp = InputResult(G);

  for (inp.Start(); inp.More(); inp.Next()) {
    Standard_Integer num = G.EntityNumber(inp.Value());
    if (num == 0) continue;
    if (direct == bm.Value(num,flag)) res.AddItem (G.Entity(num));
  }
  return res;
}

    Standard_Boolean  IFSelect_SelectFlag::Sort
  (const Standard_Integer ,
   const Handle(Standard_Transient)&,
   const Handle(Interface_InterfaceModel)& ) const
      {  return Standard_False;  }

    TCollection_AsciiString  IFSelect_SelectFlag::ExtractLabel () const
{
  TCollection_AsciiString lab ("Entities Flagged by ");
  lab.AssignCat (thename);
  return lab;
}
