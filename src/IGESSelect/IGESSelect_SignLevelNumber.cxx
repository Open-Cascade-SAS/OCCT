#include <IGESSelect_SignLevelNumber.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>

static TCollection_AsciiString laval;


    IGESSelect_SignLevelNumber::IGESSelect_SignLevelNumber
  (const Standard_Boolean countmode)
    : IFSelect_Signature ("Level Number") ,
      thecountmode (countmode)    {  }

    Standard_CString  IGESSelect_SignLevelNumber::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /* model */) const
{
  char carlev[20];
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
//JR/Hp
  if (igesent.IsNull()) {
    Standard_CString astr ;
    astr = (Standard_CString ) (thecountmode ? " NO LEVEL" : "/0/");
    return astr ;
  }
//  if (igesent.IsNull()) return (thecountmode ? " NO LEVEL" : "/0/");
  DeclareAndCast(IGESGraph_DefinitionLevel,levelist,igesent->LevelList());
  Standard_Integer level = igesent->Level();

  if (levelist.IsNull()) {
//JR/Hp
    if (level < 0) {
      Standard_CString astr ;
      astr = (Standard_CString ) (thecountmode ? " NO LEVEL" : "/0/") ;
      return astr ;
    }
//    if (level < 0) return (thecountmode ? " NO LEVEL" : "/0/");
    laval.Clear();
    if (thecountmode) sprintf (carlev,"%7d",level);
    else sprintf (carlev,"/%d/",level);
    laval.AssignCat (carlev);
  }
  else if (thecountmode) return "LEVEL LIST";
  else {
    Standard_Integer i, nblev = levelist->NbLevelNumbers();
    laval.Clear();
    laval.AssignCat("LIST:/");
    for (i = 1; i <= nblev; i ++) {
      sprintf(carlev,"%d/",levelist->LevelNumber(i));
      laval.AssignCat(carlev);
    }
  }
  return laval.ToCString();
}
