// Created on: 2002-04-25
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
    return "REPLACE";
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
      di<<"Label has no NamedShape"<<"\n";
      return 1;
    }
    di<<EvolutionString(aNS->Evolution());
    TNaming_Iterator anIter(aNS);
    Standard_Integer a;
    char aName[200];
    for(a=1;anIter.More();anIter.Next(),a++) {
      if (anIter.NewShape().IsNull()) a--;
      else if (nb==4) {
	sprintf(aName,"%s_%d",arg[3],a);
	DBRep::Set (aName,anIter.NewShape());
      }
    }
    di<<" "<<a-1;
  } else {
    di<<"Usage: GetNewShapes df entry [res]"<<"\n";
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
      di<<"Label has no NamedShape"<<"\n";
      return 1;
    }
    di<<EvolutionString(aNS->Evolution());
    TNaming_Iterator anIter(aNS);
    Standard_Integer a;
    char aName[200];
    for(a=1;anIter.More();anIter.Next(),a++) {
      if (anIter.OldShape().IsNull()) a--;
      else if (nb==4) {
	sprintf(aName,"%s_%d",arg[3],a);
	DBRep::Set (aName,anIter.OldShape());
      }
    }
    di<<" "<<a-1;
  } else {
    di<<"Usage: GetOldShapes df entry [res]"<<"\n";
    return 1;
  }
  return 0;
}

static int GetAllNew(const TopoDS_Shape& theShape, const TDF_Label& theAccess, const char* theName, Standard_Integer theIndex) {
  TNaming_NewShapeIterator anIter(theShape,theAccess);
  char aName[200];
  for(;anIter.More();anIter.Next()) {
    if (!anIter.Shape().IsNull()) {
      theIndex++;
      if (theName != NULL) {
	sprintf(aName,"%s_%d",theName,theIndex);
	DBRep::Set(aName,anIter.Shape());
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
    const char* aName = (nb==4) ? arg[3] : NULL;

    if (arg[2][0]=='0') { // label
      TDF_Label aLabel;
      if (!QADNaming::Entry(arg, aLabel)) return 1;
      Handle(TNaming_NamedShape) aNS;
      if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
	di<<"Label has no NamedShape"<<"\n";
	return 1;
      }
      Standard_Integer a;
      TNaming_Iterator anIter(aNS);
      for(a=1;anIter.More();anIter.Next(),a++) {
	if (!anIter.NewShape().IsNull()) {
	  char* aSubName;
	  if (aName!=NULL) {
	    aSubName = new char[200];
	    sprintf(aSubName,"%s_%d",aName,a);
	  } else aSubName = NULL;
	  aResult+=GetAllNew(anIter.NewShape(),aLabel,aSubName,0);
	  if (aSubName != NULL) delete(aSubName);
	}
      }
    } else { // shape
      Handle(TDF_Data) DF;
      if (!DDF::GetDF(arg[1],DF)) {
	di<<"Wrong df"<<"\n";
	return 1;
      }
      TopoDS_Shape aShape = DBRep::Get(arg[2]);
      aResult=GetAllNew(aShape,DF->Root(),aName,0);
    }
  } else {
    di<<"Usage: GetAllNewShapes df entry/shape [res]"<<"\n";
    return 1;
  }
  di<<aResult;
  return 0;
}

static int GetAllOld(const TopoDS_Shape& theShape, const TDF_Label& theAccess, const char* theName, Standard_Integer theIndex) {
  char aName[200];
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(theShape,theAccess);
  if (aNS.IsNull()) return theIndex;
  TNaming_Iterator anIter(aNS);
  for(;anIter.More();anIter.Next()) {
    if (!anIter.OldShape().IsNull() && !anIter.NewShape().IsNull()) if (anIter.NewShape().IsSame(theShape)) {
      theIndex++;
      if (theName!=NULL) {
	sprintf(aName,"%s_%d",theName,theIndex);
	DBRep::Set(aName,anIter.OldShape());
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
    const char* aName = (nb==4) ? arg[3] : NULL;

    if (arg[2][0]=='0') { // label
      TDF_Label aLabel;
      if (!QADNaming::Entry(arg, aLabel)) return 1;
      Handle(TNaming_NamedShape) aNS;
      if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
	di<<"Label has no NamedShape"<<"\n";
	return 1;
      }
      Standard_Integer a;
      TNaming_Iterator anIter(aNS);
      for(a=1;anIter.More();anIter.Next(),a++) {
	if (!anIter.NewShape().IsNull()) {
	  char* aSubName;
	  if (aName!=NULL) {
	    aSubName = new char[200];
	    sprintf(aSubName,"%s_%d",aName,a);
	  } else aSubName = NULL;
	  aResult+=GetAllOld(anIter.NewShape(),aLabel,aSubName,0);
	}
      }
    } else { // shape
      Handle(TDF_Data) DF;
      if (!DDF::GetDF(arg[1],DF)) {
	di<<"Wrong df"<<"\n";
	return 1;
      }
      TopoDS_Shape aShape = DBRep::Get(arg[2]);
      aResult=GetAllOld(aShape,DF->Root(),aName,0);
    }
  } else {
    di<<"Usage: GetAllNewShapes df entry/shape [res]"<<"\n";
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
      di<<"Wrong df"<<"\n";
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
    di<<"Usage: GetSameShapes df shape"<<"\n";
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
