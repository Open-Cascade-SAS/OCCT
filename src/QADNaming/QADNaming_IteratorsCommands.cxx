// Created on: 2002-04-25
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <stdio.h>

#include <QADNaming.hxx>
#include <TDF_Label.hxx>
#include <Draw_Interpretor.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <TNaming.hxx>
#include <TNaming_NamedShape.hxx>

#include <DDF.hxx>

#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>

static const char* EvolutionString(TNaming_Evolution theEvolution) {
  switch(theEvolution){
  case TNaming_PRIMITIVE :
    return "PRIMITIVE";
  case TNaming_GENERATED :
    return "GENERATED";
  case TNaming_MODIFY :
    return "MODIFY";
  case TNaming_DELETE :
    return "DELETE";
  case TNaming_SELECTED :
    return "SELECTED";
  case TNaming_REPLACE :
    return "MODIFY";
  }
  return "UNKNOWN_Evolution";
}


static Standard_Integer GetNewShapes (Draw_Interpretor& di,
				      Standard_Integer nb, 
				      const char** arg) {
  if (nb==3 || nb==4) {
    TDF_Label aLabel;
    if (!QADNaming::Entry(arg, aLabel)) return 1;
    Handle(TNaming_NamedShape) aNS;
    if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
      di<<"Label has no NamedShape\n";
      return 1;
    }
    di<<EvolutionString(aNS->Evolution());
    TNaming_Iterator anIter(aNS);
    Standard_Integer a;
    char aName[200];
    for(a=1;anIter.More();anIter.Next(),a++) {
      if (anIter.NewShape().IsNull()) a--;
      else if (nb==4) {
	Sprintf(aName,"%s_%d",arg[3],a);
	DBRep::Set (aName,anIter.NewShape());
      }
    }
    di<<" "<<a-1;
  } else {
    di<<"Usage: GetNewShapes df entry [res]\n";
    return 1;
  }
  return 0;
}

static Standard_Integer GetOldShapes (Draw_Interpretor& di,
				      Standard_Integer nb, 
				      const char** arg) {
  if (nb==3 || nb==4) {
    TDF_Label aLabel;
    if (!QADNaming::Entry(arg, aLabel)) return 1;
    Handle(TNaming_NamedShape) aNS;
    if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
      di<<"Label has no NamedShape\n";
      return 1;
    }
    di<<EvolutionString(aNS->Evolution());
    TNaming_Iterator anIter(aNS);
    Standard_Integer a;
    char aName[200];
    for(a=1;anIter.More();anIter.Next(),a++) {
      if (anIter.OldShape().IsNull()) a--;
      else if (nb==4) {
	Sprintf(aName,"%s_%d",arg[3],a);
	DBRep::Set (aName,anIter.OldShape());
      }
    }
    di<<" "<<a-1;
  } else {
    di<<"Usage: GetOldShapes df entry [res]\n";
    return 1;
  }
  return 0;
}

static int GetAllNew(const TopoDS_Shape& theShape, const TDF_Label& theAccess, 
                     const TCollection_AsciiString& theName, Standard_Integer theIndex)
{
  TNaming_NewShapeIterator anIter(theShape,theAccess);
  TCollection_AsciiString aName;
  for(;anIter.More();anIter.Next())
  {
    if (!anIter.Shape().IsNull())
    {
      theIndex++;
      if (!theName.IsEmpty())
      {
        aName = theName + "_" + theIndex;
        DBRep::Set(aName.ToCString(),anIter.Shape());
      }
      theIndex = GetAllNew(anIter.Shape(),theAccess,theName,theIndex);
    }
  }
  return theIndex;
}

static Standard_Integer GetAllNewShapes (Draw_Interpretor& di,
					 Standard_Integer nb, 
					 const char** arg) {
  Standard_Integer aResult = 0;
  if (nb==3 || nb==4) {
    TCollection_AsciiString aName ((nb==4) ? arg[3] : "");

    if (arg[2][0]=='0') { // label
      TDF_Label aLabel;
      if (!QADNaming::Entry(arg, aLabel)) return 1;
      Handle(TNaming_NamedShape) aNS;
      if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
	di<<"Label has no NamedShape\n";
	return 1;
      }
      Standard_Integer a;
      TNaming_Iterator anIter(aNS);
      for(a=1;anIter.More();anIter.Next(),a++) {
	if (!anIter.NewShape().IsNull()) {
      TCollection_AsciiString aSubName;
      if (!aName.IsEmpty())
      {
        aSubName += aName + "_";
        aSubName += a;
      }
      aResult+=GetAllNew(anIter.NewShape(),aLabel,aSubName,0);
	}
      }
    } else { // shape
      Handle(TDF_Data) DF;
      if (!DDF::GetDF(arg[1],DF)) {
	di<<"Wrong df\n";
	return 1;
      }
      TopoDS_Shape aShape = DBRep::Get(arg[2]);
      aResult=GetAllNew(aShape,DF->Root(),aName,0);
    }
  } else {
    di<<"Usage: GetAllNewShapes df entry/shape [res]\n";
    return 1;
  }
  di<<aResult;
  return 0;
}

static int GetAllOld(const TopoDS_Shape& theShape, const TDF_Label& theAccess, 
                     const TCollection_AsciiString& theName, Standard_Integer theIndex)
{
  TCollection_AsciiString aName;
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(theShape,theAccess);
  if (aNS.IsNull()) return theIndex;
  TNaming_Iterator anIter(aNS);
  for(;anIter.More();anIter.Next())
  {
    if (!anIter.OldShape().IsNull() && !anIter.NewShape().IsNull()) if (anIter.NewShape().IsSame(theShape))
    {
      theIndex++;
      if (!theName.IsEmpty())
      {
        aName = theName + "_" + theIndex;
        DBRep::Set(aName.ToCString(),anIter.OldShape());
      }
      theIndex = GetAllOld(anIter.OldShape(),theAccess,theName,theIndex);
    }
  }
  return theIndex;
}

static Standard_Integer GetAllOldShapes (Draw_Interpretor& di,
					 Standard_Integer nb, 
					 const char** arg) {
  Standard_Integer aResult = 0;
  if (nb==3 || nb==4) {
    TCollection_AsciiString aName((nb==4) ? arg[3] : "");

    if (arg[2][0]=='0') { // label
      TDF_Label aLabel;
      if (!QADNaming::Entry(arg, aLabel)) return 1;
      Handle(TNaming_NamedShape) aNS;
      if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
	di<<"Label has no NamedShape\n";
	return 1;
      }
      Standard_Integer a;
      TNaming_Iterator anIter(aNS);
      for(a=1;anIter.More();anIter.Next(),a++) {
	if (!anIter.NewShape().IsNull()) {
      TCollection_AsciiString aSubName;
      if (!aName.IsEmpty())
      {
        aSubName += aName + "_";
        aSubName += a;
	  }
      aResult+=GetAllOld(anIter.NewShape(),aLabel,aSubName,0);
	}
      }
    } else { // shape
      Handle(TDF_Data) DF;
      if (!DDF::GetDF(arg[1],DF)) {
	di<<"Wrong df\n";
	return 1;
      }
      TopoDS_Shape aShape = DBRep::Get(arg[2]);
      aResult=GetAllOld(aShape,DF->Root(),aName,0);
    }
  } else {
    di<<"Usage: GetAllNewShapes df entry/shape [res]\n";
    return 1;
  }
  di<<aResult;
  return 0;
}

static Standard_Integer GetSameShapes (Draw_Interpretor& di,
				       Standard_Integer nb, 
				       const char** arg) {
  TCollection_AsciiString aRes;
  if (nb == 3) {
    Standard_Integer aResult = 0;
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) {
      di<<"Wrong df\n";
      return 1;
    }
    TopoDS_Shape aShape = DBRep::Get(arg[2]);
    TNaming_SameShapeIterator anIter(aShape,DF->Root());
    for(;anIter.More();anIter.Next()) {
      if (!anIter.Label().IsNull()) {
	TCollection_AsciiString Name;
	TDF_Tool::Entry(anIter.Label(),Name);
	if (aResult != 0) aRes=aRes+Name+" "; else aRes=Name;
	aResult++;
      }
    }
  } else {
    di<<"Usage: GetSameShapes df shape\n";
    return 1;
  }
  di<<aRes.ToCString();
  return 0;
}

void QADNaming::IteratorsCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "Naming builder commands";

  theCommands.Add("GetNewShapes","GetNewShapes df entry [res]",__FILE__,GetNewShapes,g);

  theCommands.Add("GetOldShapes","GetOldShapes df entry [res]",__FILE__,GetOldShapes,g);

  theCommands.Add("GetAllNewShapes","GetAllNewShapes df entry/shape [res]",__FILE__,GetAllNewShapes,g);

  theCommands.Add("GetAllOldShapes","GetAllOldShapes df entry/shape [res]",__FILE__,GetAllOldShapes,g);

  theCommands.Add("GetSameShapes","GetSameShapes df shape",__FILE__,GetSameShapes,g);

}
