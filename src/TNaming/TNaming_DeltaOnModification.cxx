// Created on: 1997-12-03
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <TNaming_DeltaOnModification.ixx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Evolution.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : TNaming_DeltaOnModification
//purpose  : 
//=======================================================================

TNaming_DeltaOnModification::TNaming_DeltaOnModification(const Handle(TNaming_NamedShape)& NS)
: TDF_DeltaOnModification(NS)
{
  Standard_Integer NbShapes = 0;
  for (TNaming_Iterator it(NS); it.More(); it.Next()) { NbShapes++;}
  
  if (NbShapes == 0) return;
  
  TNaming_Evolution Evol = NS->Evolution();
  Standard_Integer i = 1;
  
  if (Evol == TNaming_PRIMITIVE) {
    myNew = new TopTools_HArray1OfShape(1,NbShapes); 
    for (TNaming_Iterator it2(NS) ; it2.More(); it2.Next(),i++) {
      myNew->SetValue(i,it2.NewShape());
    }
  } 
  else if (Evol == TNaming_DELETE) { 
    myOld = new TopTools_HArray1OfShape(1,NbShapes);  
    for (TNaming_Iterator it2(NS); it2.More(); it2.Next(),i++) {
      myOld->SetValue(i,it2.OldShape());
    }
  }
  else {
    myOld = new TopTools_HArray1OfShape(1,NbShapes);
    myNew = new TopTools_HArray1OfShape(1,NbShapes);
    
    for (TNaming_Iterator it2(NS); it2.More(); it2.Next(), i++) {
      myNew->SetValue(i,it2.NewShape());
      myOld->SetValue(i,it2.OldShape());
    }
  }
}

//=======================================================================
//function : LoadNamedShape
//purpose  : 
//=======================================================================

static void LoadNamedShape (TNaming_Builder& B, 
			    TNaming_Evolution Evol, 
			    const TopoDS_Shape& OS, 
			    const TopoDS_Shape& NS)
{    
  switch (Evol) {
  case TNaming_PRIMITIVE :
    {
      B.Generated(NS);
      break;
    }
  case TNaming_GENERATED :
    {
      B.Generated(OS,NS);
      break;
    }
  case TNaming_MODIFY : 
    {
      B.Modify(OS,NS);
      break;
    }
  case TNaming_DELETE : 
    {
      B.Delete (OS);
      break;
    }
  case TNaming_SELECTED :
    {
      B.Select(NS,OS);
      break;
    }
  case TNaming_REPLACE :
    {
      B.Replace(OS,NS);
      break;
    }
  }
}

//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TNaming_DeltaOnModification::Apply()
{

  Handle(TDF_Attribute) TDFAttribute = Attribute();
  Handle(TNaming_NamedShape) NS = (*((Handle(TNaming_NamedShape)*)&TDFAttribute));
  

  // If there is no attribute, reinsert the previous. Otherwise a new one 
  // is created automatically, and all referencing the previous are incorrect! FID 24/12/97
  Handle(TDF_Attribute) dummyAtt;
  //if (!Ins.Find(NS->ID(),dummyAtt)) Ins.Add(NS);
  if (!Label().FindAttribute(NS->ID(),dummyAtt)) {

    Label().AddAttribute(NS);
  }
  
  if (myOld.IsNull() && myNew.IsNull())
    return;
  else if (myOld.IsNull()) {
    //TNaming_Builder B(Ins);
    TNaming_Builder B(Label());
    TopoDS_Shape Old;
    for (Standard_Integer i = 1; i <= myNew->Upper(); i++) {
      LoadNamedShape (B,NS->Evolution(),Old,myNew->Value(i));
    }
  }
  else if (myNew.IsNull()) {
    //TNaming_Builder B(Ins);   
    TNaming_Builder B(Label());
    TopoDS_Shape New;
    for (Standard_Integer i = 1; i <= myOld->Upper(); i++) {
      LoadNamedShape (B,NS->Evolution(),myOld->Value(i),New);
    }
  }
  else {
    //TNaming_Builder B(Ins);   
    TNaming_Builder B(Label());
    for (Standard_Integer i = 1; i <= myOld->Upper(); i++) {
      LoadNamedShape (B,NS->Evolution(),myOld->Value(i),myNew->Value(i));
    }
  }
}


