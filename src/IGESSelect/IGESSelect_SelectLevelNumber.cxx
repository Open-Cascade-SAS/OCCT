#include <IGESSelect_SelectLevelNumber.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>



    IGESSelect_SelectLevelNumber::IGESSelect_SelectLevelNumber ()    {  }


    void  IGESSelect_SelectLevelNumber::SetLevelNumber
  (const Handle(IFSelect_IntParam)& levnum)
      {  thelevnum = levnum;  }

    Handle(IFSelect_IntParam)  IGESSelect_SelectLevelNumber::LevelNumber () const
      {  return thelevnum;  }


    Standard_Boolean  IGESSelect_SelectLevelNumber::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  Standard_Integer numlev = 0;
  if (!thelevnum.IsNull()) numlev = thelevnum->Value();
  DeclareAndCast(IGESGraph_DefinitionLevel,levelist,igesent->LevelList());
  Standard_Integer level = igesent->Level();
  if (levelist.IsNull()) return (level == numlev);
//  Cas d une liste
  if (numlev == 0) return Standard_False;
  Standard_Integer nb = levelist->NbPropertyValues();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    level = levelist->LevelNumber(i);
    if (level == numlev) return Standard_True;
  }
  return Standard_False;
}


    TCollection_AsciiString  IGESSelect_SelectLevelNumber::ExtractLabel
  () const
{
  char labl [50];
  Standard_Integer numlev = 0;
  if (!thelevnum.IsNull()) numlev = thelevnum->Value();
  if (numlev == 0) return TCollection_AsciiString
    ("IGES Entity attached to no Level");

  sprintf(labl,"IGES Entity, Level Number admitting %d",numlev);
  return TCollection_AsciiString (labl);
}
