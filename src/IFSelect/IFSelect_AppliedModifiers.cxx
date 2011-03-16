#include <IFSelect_AppliedModifiers.ixx>


    IFSelect_AppliedModifiers::IFSelect_AppliedModifiers
  (const Standard_Integer nbmax, const Standard_Integer nbent)
    : thelists (nbmax+1)
{
  thenbent = nbent;  theentcnt = 0;
}

   Standard_Boolean  IFSelect_AppliedModifiers::AddModif
  (const Handle(IFSelect_GeneralModifier)& modif)
{
  if (themodifs.Length() >= thelists.NbEntities()) return Standard_False;
  themodifs.Append(modif);
  thelists.SetNumber (themodifs.Length());
  return Standard_True;
}

    Standard_Boolean  IFSelect_AppliedModifiers::AddNum
  (const Standard_Integer nument)
{
  thelists.Add (nument);
  return Standard_True;
}


    Standard_Integer  IFSelect_AppliedModifiers::Count () const
      {  return themodifs.Length();  }

    Standard_Boolean  IFSelect_AppliedModifiers::Item
  (const Standard_Integer num,
   Handle(IFSelect_GeneralModifier)& modif,
   Standard_Integer& entcount)
{
  if (num < 1 || num > themodifs.Length()) return Standard_False;
  modif  = themodifs.Value(num);
  thelists.SetNumber (num);
  theentcnt = thelists.Length();
  entcount  = (theentcnt > 0 ? theentcnt : thenbent);
  return Standard_True;
}

    Standard_Integer  IFSelect_AppliedModifiers::ItemNum
  (const Standard_Integer nument) const
      {  return (theentcnt > 0 ? thelists.Value(nument) : nument);  }

    Handle(TColStd_HSequenceOfInteger) IFSelect_AppliedModifiers::ItemList () const
{
  Handle(TColStd_HSequenceOfInteger) list = new TColStd_HSequenceOfInteger();
  Standard_Integer i, nb = (theentcnt > 0 ? theentcnt : thenbent);
  for (i = 1; i <= nb; i ++)  list->Append (ItemNum(i));
  return list;
}

    Standard_Boolean  IFSelect_AppliedModifiers::IsForAll () const
      {  return (theentcnt == 0);  }
