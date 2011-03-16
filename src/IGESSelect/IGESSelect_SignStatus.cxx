#include <IGESSelect_SignStatus.ixx>
#include <IGESData_IGESEntity.hxx>
#include <stdio.h>


static char theval[10];

    IGESSelect_SignStatus::IGESSelect_SignStatus ()
    : IFSelect_Signature ("D.E. Status")    {  }

    Standard_CString  IGESSelect_SignStatus::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(IGESData_IGESEntity) igesent = Handle(IGESData_IGESEntity)::DownCast(ent);
  if (igesent.IsNull()) return "";
  Standard_Integer i,j,k,l;
  i = igesent->BlankStatus ();
  j = igesent->SubordinateStatus ();
  k = igesent->UseFlag ();
  l = igesent->HierarchyStatus ();
  sprintf (theval,"%d,%d,%d,%d",i,j,k,l);
  return theval;
}

    Standard_Boolean  IGESSelect_SignStatus::Matches
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model,
   const TCollection_AsciiString& text, const Standard_Boolean exact) const
{
  Handle(IGESData_IGESEntity) igesent = Handle(IGESData_IGESEntity)::DownCast(ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer i,j,k,l;
  i = igesent->BlankStatus ();
  j = igesent->SubordinateStatus ();
  k = igesent->UseFlag ();
  l = igesent->HierarchyStatus ();
  Standard_Integer n,nb = text.Length();
  if (nb > 9) nb = 9;
  for (n = 1; n <= nb; n ++)  theval[n-1] = text.Value(n);
  theval[nb] = '\0';

  Standard_Integer vir = 0, val = 0;
  for (n = 0; n < nb; n ++) {
    char car = theval[n];
    if (car == ',')  {  vir ++;  continue;  }
    val = Standard_Integer (car-48);
    if (car == 'V' && vir == 0) val = 0;
    if (car == 'B' && vir == 0) val = 1;
    if (car == 'I' && vir == 1) val = 0;
    if (car == 'P' && vir == 1) val = 1;
    if (car == 'L' && vir == 1) val = 2;
    if (car == 'D' && vir == 1) val = 3;
//    Matches ?
    if (vir == 0) {  // Blank
      if (i == val && !exact) return Standard_True;
      if (i != val &&  exact) return Standard_False;
    }
    if (vir == 1) {  // Subord.
      if (j == val && !exact) return Standard_True;
      if (j != val &&  exact) return Standard_False;
    }
    if (vir == 2) {  // UseFlag
      if (k == val && !exact) return Standard_True;
      if (k != val &&  exact) return Standard_False;
    }
    if (vir == 3) {  // Hierarchy
      if (l == val && !exact) return Standard_True;
      if (l != val &&  exact) return Standard_False;
    }
  }
  return exact;  // un brin de reflexion pour arriver
}
