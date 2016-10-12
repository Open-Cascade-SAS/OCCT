// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IGESData_IGESEntity.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESSelect_CounterOfLevelNumber.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_CounterOfLevelNumber,IFSelect_SignCounter)

IGESSelect_CounterOfLevelNumber::IGESSelect_CounterOfLevelNumber
  (const Standard_Boolean withmap, const Standard_Boolean withlist)
    : IFSelect_SignCounter (withmap,withlist)
      {  thehigh = thenblists = 0;  SetName("IGES Level Number");  }

    void  IGESSelect_CounterOfLevelNumber::Clear ()
      {  IFSelect_SignCounter::Clear();
	 thelevels.Nullify();  thehigh = thenblists = 0;  }


    void  IGESSelect_CounterOfLevelNumber::AddSign
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/)
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return;
  DeclareAndCast(IGESGraph_DefinitionLevel,levelist,igesent->LevelList());
  Standard_Integer level = igesent->Level();
  if (levelist.IsNull() && level < 0) return;

//  Enregistrer ce/ces niveau(x)
  if (levelist.IsNull()) AddLevel(ent,level);
  else {
    Standard_Integer nb = levelist->NbPropertyValues();
    for (Standard_Integer i = 1; i <= nb; i ++) {
      level = levelist->LevelNumber(i);
      AddLevel(ent,level);
    }
    AddLevel(ent,-1);
  }
}


    void  IGESSelect_CounterOfLevelNumber::AddLevel
  (const Handle(Standard_Transient)& ent, const Standard_Integer level)
{
  if (level < 0) {
    thenblists ++;
    Add (ent,"LEVEL LIST");
    return;
  }
  if (thelevels.IsNull()) { thelevels =
    new TColStd_HArray1OfInteger ( 0, (level > 100 ? level : 100) );
			    thelevels->Init(0); }
  Standard_Integer upper = thelevels->Upper();
  if (level > upper) {
    Handle(TColStd_HArray1OfInteger) levels =
      new TColStd_HArray1OfInteger (0,level + 100);  levels->Init(0);
    for (Standard_Integer i = 1; i <= upper; i ++)
      levels->SetValue(i,thelevels->Value(i));
    thelevels = levels;
  }
  thelevels->SetValue (level,thelevels->Value(level)+1);
  if (level > thehigh) thehigh = level;

//  if (level == 0) Add(ent," NO LEVEL");
//  else {
    char signature[30];
    sprintf (signature,"%7d",level);
    Add (ent,signature);
//  }
}


    Standard_Integer  IGESSelect_CounterOfLevelNumber::HighestLevel () const
      {  return thehigh;  }

    Standard_Integer  IGESSelect_CounterOfLevelNumber::NbTimesLevel
  (const Standard_Integer level) const
{
  if (level < 0) return thenblists;
  if (level > thehigh) return 0;
  return thelevels->Value(level);
}


    Handle(TColStd_HSequenceOfInteger)  IGESSelect_CounterOfLevelNumber::Levels
  () const
{
  Handle(TColStd_HSequenceOfInteger) list = new TColStd_HSequenceOfInteger ();
  for (Standard_Integer i = 1; i <= thehigh; i ++) {
    if (thelevels->Value(i) > 0) list->Append(i);
  }
  return list;
}


    Handle(TCollection_HAsciiString)  IGESSelect_CounterOfLevelNumber::Sign
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  Handle(TCollection_HAsciiString) res;
//  reprend les termes de AddSign pour la preparation (lecture du level) ...
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return res;
  DeclareAndCast(IGESGraph_DefinitionLevel,levelist,igesent->LevelList());
  Standard_Integer level = igesent->Level();
  if (levelist.IsNull() && level < 0) return res;

//  puis ceux de AddLevel pour calculer la signature
  if (level < 0) return new TCollection_HAsciiString ("LEVEL LIST");
  char signature[30];
  sprintf (signature,"%7d",level);
  return new TCollection_HAsciiString (signature);
}


    void  IGESSelect_CounterOfLevelNumber::PrintCount
  (const Handle(Message_Messenger)& S) const
{
  IFSelect_SignatureList::PrintCount(S);
  S<<" Highest value : " << thehigh << endl;
  if (thenblists > 0) S<<"REMARK for LEVEL LIST : Entities are counted in"
    <<" <LEVEL LIST>\n, and in each Level value of their list"<<endl;
}
