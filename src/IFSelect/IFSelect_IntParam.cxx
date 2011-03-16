#include <IFSelect_IntParam.ixx>
#include <Interface_Static.hxx>


    IFSelect_IntParam::IFSelect_IntParam ()
      {  theval = 0;  }

    void  IFSelect_IntParam::SetStaticName (const Standard_CString statname)
      {  thestn.Clear();  thestn.AssignCat (statname);  }

    Standard_Integer  IFSelect_IntParam::Value () const 
{
  if (thestn.Length() == 0) return theval;
  if (!Interface_Static::IsSet(thestn.ToCString()) ) return theval;
  return Interface_Static::IVal(thestn.ToCString());
}

    void  IFSelect_IntParam::SetValue (const Standard_Integer val)
{
  theval = val;
  if (thestn.Length() == 0) return;
  if (!Interface_Static::IsPresent(thestn.ToCString()) ) return;
  Interface_Static::SetIVal (thestn.ToCString(),theval);
}
