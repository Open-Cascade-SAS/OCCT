// Created on: 1997-03-04
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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


#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Identifier.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Localizer.hxx>
#include <TNaming_MapIteratorOfMapOfNamedShape.hxx>
#include <TNaming_MapOfNamedShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//#define MDTV_DEB_IDF
#ifdef OCCT_DEBUG_IDF
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>

//unreferenced function, commented  
/*static void ModDbgTools_Write(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  ofstream save;
  save.open(filename);
  save << "DBRep_DrawableShape" << endl << endl;
  if(!shape.IsNull()) BRepTools::Write(shape, save);
  save.close();
}

static void ModDbgTools_WriteCurrentShape(const Handle(TNaming_NamedShape) & NS)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(NS->Label(), entry);
  if (!NS.IsNull())
    {
      TopoDS_Shape Sh = TNaming_Tool::CurrentShape (NS);
      if(!Sh.IsNull()) {
	TCollection_AsciiString Entry = entry.Cat("_Cur.brep");
	ModDbgTools_Write(Sh, Entry.ToCString());
      }
      else 
	cout << "ModDbgTools::Write>>> TopoDS_Shape IS NULL on Entry = "<< entry << endl;
    }
  else
    cout << "ModDbgTools::Write>>>  CurrentShape of TNaming_NamedShape IS NULL on Entry = "<< entry << endl;
}*/
#endif

//=======================================================================
//function : TNaming_Identifier
//purpose  : 
//=======================================================================

TNaming_Identifier::TNaming_Identifier(const TDF_Label&        LabAcces,
				       const TopoDS_Shape&     S,
				       const TopoDS_Shape&     Context,
				       const Standard_Boolean  /*OneOnly*/)
:myTDFAcces(LabAcces), myShape(S),
myDone(Standard_False),myIsFeature(Standard_False)
{
  Init(Context); 
}

//=======================================================================
//function : TNaming_Identifier
//purpose  : 
//=======================================================================

TNaming_Identifier::TNaming_Identifier(const TDF_Label&        LabAcces,
				       const TopoDS_Shape&                     S,
				       const Handle(TNaming_NamedShape)&       ContextNS,
				       const Standard_Boolean                  /*OneOnly*/)
:myTDFAcces(LabAcces), myShape(S),
myDone(Standard_False),myIsFeature(Standard_False)
{
  const TopoDS_Shape& aContext = TNaming_Tool::GetShape (ContextNS);
  Init(aContext); 
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================                                         
void TNaming_Identifier::Init(const TopoDS_Shape&     Context)
{
  Handle(TNaming_UsedShapes) US;
  myTDFAcces.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  
  TNaming_Localizer Localizer;
  Localizer.Init (US,myTDFAcces.Transaction());
  
  Handle(TNaming_NamedShape) NS;
  NS   = TNaming_Tool::NamedShape(myShape, myTDFAcces);
  if (NS.IsNull()) {
    AncestorIdentification(Localizer, Context);
    return;
  }
#ifdef OCCT_DEBUG_IDF
  TCollection_AsciiString entry;
  TDF_Tool::Entry(NS->Label(), entry);
  cout << "Identifier:: (S) Label = " << entry <<endl;
#endif
  //-------------------------------------
  // S est New dans un NamedShape
  //-------------------------------------
  TNaming_Evolution Evol = NS->Evolution();
  if (Evol == TNaming_PRIMITIVE) {
    PrimitiveIdentification(Localizer,NS);
  }
  else if (Evol == TNaming_GENERATED) {
    GeneratedIdentification(Localizer,NS);
  }
  else {
    Identification(Localizer,NS);
  }
}                                         

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TNaming_NameType TNaming_Identifier::Type() const
{
  return myType;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Identifier::IsDone() const 
{
  return myDone;
}


//=======================================================================
//function : IsFeature
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Identifier::IsFeature()
{
  return myIsFeature;
}


//=======================================================================
//function : Feature
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_Identifier::Feature() const
{ 
  return myPrimitiveArgs.First();
}


//=======================================================================
//function : InitArgs
//purpose  : 
//=======================================================================

void TNaming_Identifier::InitArgs() 
{
}


//=======================================================================
//function : MoreArgs
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Identifier::MoreArgs() const
{
  return (!myPrimitiveArgs.IsEmpty() || !myShapeArgs.IsEmpty());
}

//=======================================================================
//function : ArgIsFeature
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Identifier::ArgIsFeature() const
{
  return (!myPrimitiveArgs.IsEmpty());
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TNaming_Identifier::NextArg() 
{
  if      (!myPrimitiveArgs.IsEmpty()) myPrimitiveArgs.RemoveFirst();
  else if (!myShapeArgs    .IsEmpty()) myShapeArgs    .RemoveFirst();
}

//=======================================================================
//function : FeatureArg
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_Identifier::FeatureArg() 
{
  return myPrimitiveArgs.First();
}


//=======================================================================
//function : ShapeArg
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Identifier::ShapeArg() 
{
  return myShapeArgs.First();
}


//=======================================================================
//function : ShapeContext
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Identifier::ShapeContext() const
{
  const TopoDS_Shape& S = myShapeArgs.First();
  TopoDS_Shape  SC;
  Handle(TNaming_NamedShape) NS = TNaming_Tool::NamedShape(S,myTDFAcces);
  TNaming_Localizer::FindShapeContext (NS, S, SC); // szy ==> added par. S
  return SC;
}


//=======================================================================
//function : Handle(TNaming_NamedShape)
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_Identifier::NamedShapeOfGeneration() const
{
  return myNSContext;
}

//=======================================================================
//function : AncestorIdentification
//purpose  : 
//=======================================================================

void TNaming_Identifier::AncestorIdentification(TNaming_Localizer& Localizer,
						const TopoDS_Shape& Context)
{
 TopTools_MapOfShape AncInFeature;
 if (Context.IsNull()) {
   myType = TNaming_UNKNOWN;
   myDone = 0;
   return;
 }
 Localizer.FindFeaturesInAncestors (myShape, Context, AncInFeature); 
 
 if (AncInFeature.IsEmpty()) {
   myDone = 0;
   return;
 }
 myType = TNaming_INTERSECTION;
 TopTools_MapIteratorOfMapOfShape itS(AncInFeature);
 for (; itS.More(); itS.Next()) myShapeArgs.Append(itS.Key());
 myDone = 1;
}

//=======================================================================
//function : Imported
//purpose  : 
//=======================================================================

Standard_Boolean IsImported(const Handle(TNaming_NamedShape)& NS)
{
  TDF_Label Lab = NS->Label();
  TDF_Label Father = Lab.Father();
  for (TDF_ChildIterator  cit(Father); cit.More(); cit.Next()) {
    if (cit.Value() != Lab) return 0;
  }

  TNaming_Iterator it(NS);
  if (!it.More()) return 0;
  it.Next();  if (!it.More()) return 0;
  //plus d un shape.
#ifdef OCCT_DEBUG
  cout <<"WARNING IMPORTED"<<endl;
#endif
  return 1;
}

//=======================================================================
//function : PrimitiveIdentification
//purpose  : 
//=======================================================================

void TNaming_Identifier::PrimitiveIdentification (TNaming_Localizer& /*L*/,
						  const Handle(TNaming_NamedShape)& NS)
{
  myPrimitiveArgs.Append(NS);
  myIsFeature = Standard_True;
  myType      = TNaming_MODIFUNTIL;
  //Reconnaissance Shape Mort.
  if (IsImported(NS)) {
    myType = TNaming_CONSTSHAPE;
    myIsFeature = Standard_False;  
  }
  myDone = 1;
}



//=======================================================================
//function : GeneratedIdentification
//purpose  : Finds generators of identifying shape <myShape> and keeps
//         : them in <myShapeArgs>
//=======================================================================

void TNaming_Identifier::GeneratedIdentification(TNaming_Localizer& /*Localizer*/,
						 const Handle(TNaming_NamedShape)& NS)
{
//  TopoDS_Shape Gen; //szy 16.10.03
  TopTools_ListOfShape aListOfGenerators;
  TNaming_Localizer::FindGenerator (NS,myShape,aListOfGenerators);
  myShapeArgs.Append(aListOfGenerators);
  myNSContext = NS;
  myType      = TNaming_GENERATION;
  myDone = 1;  
}

//=======================================================================
//function : Identification
//purpose  : 
//=======================================================================

void TNaming_Identifier::Identification(TNaming_Localizer&          Localizer,
					const Handle(TNaming_NamedShape)& NS)
{  

  TNaming_MapOfNamedShape Primitives;
  TopTools_MapOfShape     Shapes;

  Localizer.Backward(NS,myShape,Primitives,Shapes);
  
  if (Primitives.IsEmpty() && Shapes.IsEmpty()) {
    myDone = 0;
    return;
  }
  myType = TNaming_INTERSECTION;
  TNaming_MapIteratorOfMapOfNamedShape itP(Primitives);
  for (; itP.More(); itP.Next()) myPrimitiveArgs.Append(itP.Key());
  TopTools_MapIteratorOfMapOfShape itS(Shapes);
  for (; itS.More(); itS.Next()) myShapeArgs.Append(itS.Key());
  
  if (myPrimitiveArgs.Extent() == 1 && myShapeArgs.IsEmpty()) {
    myType      = TNaming_MODIFUNTIL;
    myIsFeature = Standard_True;  
    //Reconnaissance Shape Mort.
    if (IsImported(myPrimitiveArgs.First())) {
      myType = TNaming_CONSTSHAPE;
      myIsFeature = Standard_False;  
    }
  }
  myDone = 1;
}
