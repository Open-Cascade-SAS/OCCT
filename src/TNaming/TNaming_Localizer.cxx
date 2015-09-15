// Created on: 1997-06-11
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


#include <TDF_Label.hxx>
#include <TNaming_DataMapOfShapeShapesSet.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Identifier.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_IteratorOnShapesSet.hxx>
#include <TNaming_ListIteratorOfListOfIndexedDataMapOfShapeListOfShape.hxx>
#include <TNaming_ListIteratorOfListOfMapOfShape.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_Localizer.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_ShapesSet.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#ifdef OCCT_DEBUG
//#define MDTV_DEB_SC
#ifdef OCCT_DEBUG_SC
#include <TDF_Tool.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>

#include <TCollection_AsciiString.hxx>
#include <BRepTools.hxx>
void LPrintEntry(const TDF_Label&       label)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  cout << "LabelEntry = "<< entry << endl;
}
static void LWrite(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 256) return;
#if defined _MSC_VER
  strcpy_s (buf, filename);
#else
  strcpy (buf, filename);
#endif
  char* p = buf;
  while (*p) {
    if(*p == ':')
      *p = '-';
    p++;
  }
  ofstream save (buf);
  if(!save) 
    cout << "File " << buf << " was not created: rdstate = " << save.rdstate() << endl;
  save << "DBRep_DrawableShape" << endl << endl;
  if(!shape.IsNull()) BRepTools::Write(shape, save);
  save.close();
}

//=======================================================================
static void LWriteNSOnLabel (const Handle(TNaming_NamedShape)& NS,  
				     const Standard_CString filename) 
{
  if(!NS.IsNull() && !NS->IsEmpty() ) {
    TCollection_AsciiString aNam (filename);
    TCollection_AsciiString oldS ("_Old");
    TCollection_AsciiString newS ("_New_");
    Standard_Integer i(0);
    TNaming_Iterator it(NS);
    for(;it.More(); it.Next(),i++) {
      TCollection_AsciiString aName1 = aNam + oldS + i + ".brep";
      TCollection_AsciiString aName2 = aNam + newS + i + ".brep";
      const TopoDS_Shape& oldShape = it.OldShape();
      const TopoDS_Shape& newShape = it.NewShape();
      if(!oldShape.IsNull())
	LWrite ( oldShape, aName1.ToCString());
      if(!newShape.IsNull())
	LWrite ( newShape, aName2.ToCString());      
    }
  }
}
#endif
#endif
//=======================================================================
 //function : FindFeatureInAncestors
//purpose  : Cherche les ancetres de S qui sont sous des labels
//=======================================================================

void TNaming_Localizer::FindFeaturesInAncestors 
(const TopoDS_Shape&                        S,
 const TopoDS_Shape&                        Context,
 TopTools_MapOfShape&                       AncInFeature)
{
#ifdef OCCT_DEBUG_SC
  LWrite(S, "Localizer_S.brep");
  LWrite(Context, "Localizer_Context.brep");
#endif

  const TopTools_IndexedDataMapOfShapeListOfShape& Anc = Ancestors(Context,S.ShapeType());
  
  if (Anc.Contains(S)) {
#ifdef OCCT_DEBUG_SC
    cout <<"Localizer: S in ancestor" <<endl;
#endif
    const TopTools_ListOfShape& L = Anc.FindFromKey(S);
    TopTools_ListIteratorOfListOfShape itL(L);
    for (; itL.More(); itL.Next()) {
      const TopoDS_Shape& AS = itL.Value();
#ifdef OCCT_DEBUG_SC
      LWrite(AS, "Localizer_AS.brep");
#endif      
      Handle(TNaming_NamedShape) NS = TNaming_Tool::NamedShape(AS,myUS->Label());
      if (!NS.IsNull()) {
//      if (TNaming_Tool::HasLabel(myUS,AS)) {
	AncInFeature.Add(AS);
      }
      else if (AS.ShapeType() > TopAbs_FACE) {
	FindFeaturesInAncestors (AS, Context, AncInFeature);
      }
      else {
#ifdef OCCT_DEBUG
	cout <<" TNaming_Localization : Failure in the research of ancetres in TDF"<<endl;
#endif
      }
    }
    
  }
  else {
#ifdef OCCT_DEBUG
    cout <<" TNaming_Localization : S n est pas dans le solide"<<endl;//S is not in the solid
#endif
  }
}

//=======================================================================
//function : SubShapes
//purpose  : 
//=======================================================================

const TopTools_MapOfShape& TNaming_Localizer::SubShapes (const TopoDS_Shape&    In,
							 const TopAbs_ShapeEnum TS)
{

  TopTools_ListIteratorOfListOfShape     itS(myShapeWithSubShapes) ;
  TNaming_ListIteratorOfListOfMapOfShape itSS(mySubShapes);
//  Standard_Boolean Found = Standard_False;
  for (; itS.More(); itS.Next(),itSS.Next()) {
    if (In.IsSame(itS.Value())) {
      TopTools_MapOfShape& SubShapes = itSS.Value();
      for (TopExp_Explorer exp(In,TS); exp.More(); exp.Next()) {
	const TopoDS_Shape& SS = exp.Current();
	if (SubShapes.Contains(SS)) {
	  break;
	}
	SubShapes.Add(SS);
      }
      return SubShapes;
    }
  }

  TopTools_MapOfShape emptyMap;
  mySubShapes.Prepend(emptyMap);
  myShapeWithSubShapes.Prepend(In);
  
  TopTools_MapOfShape& SubShapes = mySubShapes.First();
  for (TopExp_Explorer exp(In,TS); exp.More(); exp.Next()) {
    const TopoDS_Shape& SS = exp.Current();
    if (SubShapes.Contains(SS)) {
      break;
    }
    SubShapes.Add(SS);
  }
  return SubShapes;
}


//=======================================================================
//function : Ancestors
//purpose  : 
//=======================================================================

const TopTools_IndexedDataMapOfShapeListOfShape& TNaming_Localizer::Ancestors
(const TopoDS_Shape&     In,
 const TopAbs_ShapeEnum  TS)
{
  TopTools_ListIteratorOfListOfShape                           itS(myShapeWithAncestors) ;
  TNaming_ListIteratorOfListOfIndexedDataMapOfShapeListOfShape itA(myAncestors);
//  Standard_Boolean Found = Standard_False;
  for (; itS.More(); itS.Next(),itA.Next()) {
    if (In.IsSame(itS.Value())) {
      //-----------------------
      // Ancetres existent.
      //-----------------------
      TopTools_IndexedDataMapOfShapeListOfShape& Anc = itA.Value();

      TopExp_Explorer exp(In,TS);
#ifdef OCCT_DEBUG
      if (!exp.More())   cout <<" TNaming_Localization : Construction ancetres impossible"<<endl;
#endif
      const TopoDS_Shape& SS = exp.Current();
      
      if (Anc.Contains(SS)) {
	return Anc;
      }
      else {
	//----------------------------
	// Completion des ancetres.
	//----------------------------
	TopAbs_ShapeEnum TA = TopAbs_FACE;
	if (TS == TopAbs_EDGE)   TA = TopAbs_FACE;
	if (TS == TopAbs_VERTEX) TA = TopAbs_EDGE;
	if (TA >= In.ShapeType()) {
	  TopExp::MapShapesAndAncestors(In, TS, TA, Anc);
	}
	else {
#ifdef OCCT_DEBUG
	  cout <<" TNaming_Localization : Construction ancetres impossible"<<endl;
#endif
	}
      }
      return Anc;
    }
  }
  //-----------------------------------
  // Construction des ancetres
  //-----------------------------------
  TopTools_IndexedDataMapOfShapeListOfShape emptyAnc;
  myShapeWithAncestors.Prepend(In);
  myAncestors         .Prepend(emptyAnc);

  TopAbs_ShapeEnum TA=TopAbs_COMPOUND;

  if (TS == TopAbs_VERTEX)      TA = TopAbs_EDGE;
  else if (TS == TopAbs_EDGE)   TA = TopAbs_FACE;
  else if (TS == TopAbs_FACE)   TA = TopAbs_SOLID;
  if ((TS == TopAbs_EDGE || TS == TopAbs_VERTEX || TS == TopAbs_FACE) && TA >= In.ShapeType()) {
    TopExp::MapShapesAndAncestors(In, TS, TA, myAncestors.First());
  }
  else {
#ifdef OCCT_DEBUG
    cout <<" TNaming_Localization : Construction ancetres impossible"<<endl;
#endif
  }
  return myAncestors.First();
}

//=======================================================================
//function : IsNew
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Localizer::IsNew (const TopoDS_Shape&    S,
					   const Handle(TNaming_NamedShape)& NS)
{
  TNaming_Iterator itLab(NS);
  for (; itLab.More(); itLab.Next()) {
    if (itLab.OldShape().IsSame(S)) {
      return Standard_False;
    }
    if (itLab.NewShape().IsSame(S)) {
      return Standard_True;
    }
  }
#ifdef OCCT_DEBUG
  cout <<"TNaming_Localizer:IsNewInLab : Shape n est pas dans le Label."<<endl;
#endif
  return Standard_False;
}


//=======================================================================
//function : Back
//purpose  : 
//=======================================================================

void TNaming_Localizer::GoBack (const TopoDS_Shape&         S,
				const TDF_Label&            Lab,
				const TNaming_Evolution     Evol,
				TopTools_ListOfShape&       LBS,
				TNaming_ListOfNamedShape&   LBNS)
{
//  Standard_Integer TrDef;

  TNaming_OldShapeIterator it(S,myCurTrans,myUS);
  TopoDS_Shape             Sol; 
  if (!it.More()) {
    //-----------------------------------------------------------
    // Pas d'ascendants => Recherche et exploration  du contenant
    //----------------------------------------------------------
    const TDF_Label& Father  = Lab.Father();
    TNaming_Iterator itLab(Father);
    if(itLab.More()) 
      Sol = itLab.OldShape();      
    //-------------------------------------------
    // Recherche des ancetres dans des features.
    //-------------------------------------------
    if (!Sol.IsNull()) {
      TopTools_MapOfShape AncInFeature;
      FindFeaturesInAncestors (S, Sol, AncInFeature); 
      TopTools_MapIteratorOfMapOfShape itF(AncInFeature);
      for ( ; itF.More(); itF.Next()) {
        const TopoDS_Shape& AncOfS = itF.Key();
        LBS  .Append(AncOfS);
        LBNS.Append(TNaming_Tool::NamedShape(AncOfS,Lab));
      }
    }
  } 
  else {
    for ( ; it.More(); it.Next()) {
  //      if (it.NamedShape()->Evolution() != TNaming_SELECTED) {
      if (it.NamedShape()->Evolution() == Evol) {
        Handle(TNaming_NamedShape) NS = TNaming_Tool::NamedShape(it.Shape(),Lab);
        if (!NS.IsNull()) {
          LBS.Append  (it.Shape());
          LBNS.Append (TNaming_Tool::NamedShape(it.Shape(),Lab));
        }
        else {
#ifdef OCCT_DEBUG
          cout <<"TNaming_Localizer: Shape modifie sans avoir ete cree"<<endl;
#endif
        }
      }
    }
  }
}
/*
//=======================================================================
//function : Backward
//purpose  :
//=======================================================================

void TNaming_Localizer::Backward (const TopoDS_Shape&  S, 
				  TDF_LabelMap&        Primitives, 
				  TopTools_MapOfShape& ValidShapes)
{ 
  Standard_Integer  PrevTrans = myCurTrans - 1;
  Standard_Integer  TrDef;
  TDF_Label         Lab      = TNaming_Tool::Label (myUS, S, TrDef);
  TNaming_Evolution Evol     = Evolution(Lab);


  TopTools_ListOfShape  LBS;
  TDF_LabelList         LBLab;

  GoBack(S,Lab,LBS,LBLab);
  

  TopTools_ListIteratorOfListOfShape itLBS  (LBS);
  TDF_ListIteratorOfLabelList        itLBLab(LBLab);

  if (LBS.IsEmpty()) {
    Primitives.Add(Lab);
  }
  for ( ; itLBS.More(); itLBS.Next(), itLBLab.Next()) {
    const TopoDS_Shape& OS    = itLBS.Value();
    const TDF_Label&    LabOS = itLBLab.Value();

    Evol = Evolution(LabOS);

    if (TNaming_Tool::ValidUntil(OS,myUS) >= myCurTrans) {
      //---------------------------------------------------------
      // Le Shape est valid dans la transaction myCurTrans => STOP
      //---------------------------------------------------------
      ValidShapes.Add(OS);
    }
    else if (Evol == TNaming_PRIMITIVE) {    
      Primitives.Add(LabOS);
    }
    else if ((Evol == TNaming_GENERATED) && IsNewInLab (OS,LabOS,PrevTrans)) {
      //--------------------------------------------------------------
      // Passage  par une generation
      // le shape dans myCurTrans descendra d un element de cet attribut.
      // Localisation de OS dans la transaction courrante.
      // les shapes obtenus seront des antecedants du shape cherche.
      //--------------------------------------------------------------
      
      // A faire seulememt si OS est un newShape dans LabOS.
      TNaming_ShapesSet ResGen;    
      TopoDS_Shape      PrevIn;
      TDF_Label         Father  = LabOS.Father();
      TNaming_Iterator  itLab(Father,PrevTrans);
      for (; itLab.More(); itLab.Next()) {
	PrevIn= itLab.OldShape();
	break;
      }
      Localize(PrevIn,LabOS,OS,ResGen);
      for (TNaming_IteratorOnShapesSet itLoc(ResGen); itLoc.More(); itLoc.Next()) {
	ValidShapes.Add(itLoc.Value());
      } 
    }
    else if (Evol == TNaming_SELECTED) {
      //PAS FINI.
      TNaming_ShapesSet ResSel;
      TopoDS_Shape      PrevIn,CurIn;
//      FindIn  (LabOS,PrevIn,CurIn);  
      Localize(PrevIn,CurIn,OS,ResSel);
      for (TNaming_IteratorOnShapesSet itLoc(ResSel); itLoc.More(); itLoc.Next()) {
	ValidShapes.Add(itLoc.Value());
      }
    }
    else {
      Backward(itLBS.Value(),Primitives,ValidShapes);
    }
  }
}
*/

//=======================================================================
//function : NamedShape
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) NamedShape(const TDF_Label& Lab)
{
  Handle(TNaming_NamedShape) NS;
  Lab.FindAttribute(TNaming_NamedShape::GetID(),NS);
  return NS;
}

//=======================================================================
//function : Backward
//purpose  :
//=======================================================================

void TNaming_Localizer::Backward (const Handle(TNaming_NamedShape)& NS,
				  const TopoDS_Shape&               S, 
				  TNaming_MapOfNamedShape&          Primitives, 
				  TopTools_MapOfShape&              Shapes)
{ 
  TNaming_Evolution Evol     = NS->Evolution();
  TDF_Label         LabNS    = NS->Label();

  TopTools_ListOfShape      LBS;
  TNaming_ListOfNamedShape  LBNS;

  GoBack(S,LabNS,Evol,LBS,LBNS);
  

  TopTools_ListIteratorOfListOfShape     itLBS  (LBS);
  TNaming_ListIteratorOfListOfNamedShape itLBNS (LBNS);

  if (LBS.IsEmpty()) {
    Primitives.Add(NS);
  }
  for ( ; itLBS.More(); itLBS.Next(), itLBNS.Next()) {
    const TopoDS_Shape&        OS  = itLBS.Value();
    Handle(TNaming_NamedShape) NOS = itLBNS.Value();
    Evol = NOS->Evolution();
    if (Evol == TNaming_PRIMITIVE) {    
      Primitives.Add(NOS);
    }
    else if (Evol == TNaming_GENERATED) {
      Shapes.Add(OS);
    }
    else {
      Backward(NOS, itLBS.Value(),Primitives,Shapes);
    }
  }
}

//=======================================================================
//function : ValidCandidat
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
/*static Standard_Boolean StoreValid (const TopoDS_Shape&        S,
				    const TopTools_MapOfShape& ShapeOfSol,
				    TopAbs_ShapeEnum           TargetType,
				    TNaming_ShapesSet&         Res) 
{
  Standard_Boolean Valid = Standard_False;

  if (ShapeOfSol.Contains(S)) {
    if (S.ShapeType() == TargetType) {
      Res.Add(S);
      return Standard_True;
    }
    else  if (S.ShapeType() < TargetType) {
      for (TopExp_Explorer exp(S,TargetType); exp.More(); exp.Next()) {
	const TopoDS_Shape& SS = exp.Current();
	Res.Add(SS);
	Valid = Standard_True;
      }
    }
  }
  return Valid;
}*/
#endif

/*
//=======================================================================
//function : Forward
//purpose  : 
//=======================================================================

void TNaming_Localizer::GoForward(const TopoDS_Shape&               S,
				  const TopTools_MapOfShape&        Target,
				  const TopAbs_ShapeEnum            TargetType,
				  TNaming_ShapesSet&                Res)
					 
{
  Standard_Integer TrDef;
  TDF_Label Lab = TNaming_Tool::Label (myUS, S, TrDef);
  if (StoreValid (S, Target, TargetType, Res)) {
    return;
  }
  TNaming_Evolution        Evol = Evolution(Lab);
  TNaming_NewShapeIterator NewIt(S,myCurTrans,myUS);
  
  for ( ; NewIt.More(); NewIt.Next()) {
    const TopoDS_Shape& NS = NewIt.Shape();
    GoForward ( NS, Target, TargetType, Res);
  }
}

*/

//=======================================================================
//function : FindNeighbourg
//purpose  : 
//=======================================================================

void TNaming_Localizer::FindNeighbourg (const TopoDS_Shape&      Sol,
					const TopoDS_Shape&      S,
					TopTools_MapOfShape&     Neighbourg)
{  
  if(Sol.IsNull() || S.IsNull()) return;
  TopAbs_ShapeEnum       TA = S.ShapeType();
  TopAbs_ShapeEnum       TS=TopAbs_COMPOUND;

  if (TA == TopAbs_FACE) TS = TopAbs_EDGE;
  if (TA == TopAbs_EDGE) TS = TopAbs_VERTEX;
  if (TA == TopAbs_VERTEX) TS = TopAbs_VERTEX; // szy 30.03.10
  const TopTools_IndexedDataMapOfShapeListOfShape& Anc = Ancestors(Sol,TS); 
// szy 30.03.10 to process case when Candidate is of type Vertex
//  if (TA == TopAbs_VERTEX) {
//#ifdef OCCT_DEBUG
//    cout <<"construction voisins des vertex impossible"<<endl;
//#endif
//    return;
//  }
  for (TopExp_Explorer  Exp(S,TS); Exp.More(); Exp.Next()) {
    const TopoDS_Shape& SS = Exp.Current();
    if (!Anc.Contains(SS)) {
      //----------------------------------------------------
      // Construction des ancetres
      //----------------------------------------------------
      break;
    }
    else {
      TopTools_ListIteratorOfListOfShape itL(Anc.FindFromKey(SS));
      for ( ; itL.More(); itL.Next()) {
	if (!itL.Value().IsSame(S)) {
	  Neighbourg.Add(itL.Value());
	}
      }
    }
  }
}

//=======================================================================
//function : TNaming_Localizer
//purpose  : 
//=======================================================================

TNaming_Localizer::TNaming_Localizer() 
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TNaming_Localizer::Init(const Handle(TNaming_UsedShapes)& US,
			     const Standard_Integer            CurTrans) 
{
  myUS       = US;
  myCurTrans = CurTrans;
}


//=======================================================================
//function : Explode
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
/*static void Explode (TNaming_ShapesSet& Res,
		     TopAbs_ShapeEnum   TS,
		     TNaming_ShapesSet& ResGoodType) 
{
  TNaming_IteratorOnShapesSet it(Res);
  for ( ; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    TopExp_Explorer exp(S,TS);
    for (; exp.More(); exp.Next()) {
      ResGoodType.Add(exp.Current());
    }
  }
} */ 
#endif

/*
//=======================================================================
//function : Localize
//purpose  : 
//=======================================================================

void TNaming_Localizer::Localize(const TopoDS_Shape& PrevIn,
				 const TopoDS_Shape& CurIn ,
				 const TopoDS_Shape& S,
				 TNaming_ShapesSet& Res) 
{  
  Res.Clear();
  TDF_LabelMap                 Primitives;
  TopTools_MapOfShape          ValidShapes;
  Standard_Integer             PrevTrans = myCurTrans-1;

  const TopTools_MapOfShape& CurSubShapes = SubShapes (CurIn,S.ShapeType());

  Standard_Boolean HasLabel = TNaming_Tool::HasLabel(myUS,S);

  if (HasLabel) {
    Standard_Integer  TrDef;
    TDF_Label         Lab      = TNaming_Tool::Label (myUS, S, TrDef);
    TNaming_Evolution Evol     = Evolution(Lab);
    if (Evol == TNaming_PRIMITIVE) {
      //------------------------
      // Label est une primitive
      //------------------------
      Primitives.Add(Lab);
      Forward  (CurSubShapes,S.ShapeType(),Primitives,ValidShapes,Res);
      return;
    }
    if ((Evol == TNaming_GENERATED) && IsNewInLab (S,Lab,PrevTrans)) {
      Localize(PrevIn,Lab,S,Res);
      return;
    }
  }
  
  if (HasLabel && HasAscendant(myUS,S,PrevTrans)) {
    //-------------------
    // Remontee Descente.
    //-------------------
    Backward (S, Primitives, ValidShapes);
    Forward  (CurSubShapes,S.ShapeType(),Primitives,ValidShapes,Res);
  }
  else {
    //----------------------------------------------
    // Localisation des ancetres.
    //----------------------------------------------
    TopTools_MapOfShape AncInFeature;
    FindFeaturesInAncestors (S, PrevIn, AncInFeature); 
    TopTools_MapIteratorOfMapOfShape itF(AncInFeature);
    Standard_Boolean First = Standard_True;

    for ( ; itF.More(); itF.Next()) {
      const TopoDS_Shape& AncOfS = itF.Key();
      TNaming_ShapesSet   ResAnc;

      Localize (PrevIn, CurIn, AncOfS, ResAnc);

      //---------------------------
      // Res = Commun des resultats
      //---------------------------
      if (First) {
	Explode (ResAnc,S.ShapeType(),Res);
	First = 0;
      }
      else {
	TNaming_ShapesSet ResGoodType;
	Explode (ResAnc,S.ShapeType(),ResGoodType);
	Res.Filter(ResGoodType);
      }
    }
  }
}
*/

//=======================================================================
//function : FindGenerator
//purpose  : Finds all generators of the <S> kept in <NS>
//=======================================================================

void TNaming_Localizer::FindGenerator (const Handle(TNaming_NamedShape)& NS,
				       const TopoDS_Shape&               S,
				       TopTools_ListOfShape& theListOfGenerators)
     
{ 
  Handle(TNaming_UsedShapes) US;
  TDF_Label LabNS = NS->Label();
  (LabNS.Root()).FindAttribute(TNaming_UsedShapes::GetID(),US);
  
  for (TNaming_OldShapeIterator it (S,US); it.More(); it.Next()) {
    if (it.Label() == LabNS) {
      theListOfGenerators.Append(it.Shape());
//      break; //szy 16.10.03

    }
  }  
}

//=======================================================================
//function : FindShapeContext
//purpose  : Finds context of the shape <S>.
//         : Looks for all oldshapes kept at father label of <NS>.
//         : If <S> validated as subshape of one of the old shapes -
//         : this oldshape is Context.
//=======================================================================

void TNaming_Localizer::FindShapeContext (const Handle(TNaming_NamedShape)& NS,
					  const TopoDS_Shape&               S,
					        TopoDS_Shape&               SC)
     
{ 
#ifdef OCCT_DEBUG_SC
    LWrite(S, "FSC_Sel.brep"); LPrintEntry( NS->Label());
#endif
  TopTools_ListOfShape aList;
  TDF_Label Father = NS->Label().Father();
  TNaming_Iterator  itLab(Father);
  for (; itLab.More(); itLab.Next()) {
    aList.Append(itLab.OldShape()); //szy
  }
// szy 
  TopTools_ListIteratorOfListOfShape it(aList);
  Standard_Boolean found = 0;
  for(;it.More();it.Next()) {
    SC = it.Value();
#ifdef OCCT_DEBUG_SC
    LWrite(SC, "FSC_OldShape.brep");
#endif
    if (SC.IsNull()) continue;
    else {
      if (SC.ShapeType() < S.ShapeType()) {
	for (TopExp_Explorer exp(SC,S.ShapeType()); exp.More(); exp.Next()) {
	  if (exp.Current().IsSame(S)) {
	    found = 1;
#ifdef OCCT_DEBUG_SC
	    cout << "Find Context shape = " << SC.TShape() << "ShapeType = " << SC.ShapeType() <<endl;
#endif	    
	    break;
	  }
	}
	if(found) break;
      }
    }
  }
// end szy
  if (!SC.IsNull()) {
    Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(SC,Father);
    if (!aNS.IsNull()) {
#ifdef OCCT_DEBUG_SC
      cout << "FindShapeContext: ";LPrintEntry(aNS->Label());
#endif
      if (aNS->Label().Father().FindAttribute(TNaming_NamedShape::GetID(),aNS)) {
	TopoDS_Shape aShape;
#ifdef OCCT_DEBUG_SC
	LWriteNSOnLabel(aNS, "FSC");
#endif
	TNaming_Iterator anIter(aNS->Label());
	for(;anIter.More();anIter.Next()) {
	  aShape = anIter.NewShape();
	  if (!aShape.IsNull()) break;
	}
	if (!aShape.IsNull()) SC=aShape;
      }
    }
  }

}

/*
//=======================================================================
//function : Localize
//purpose  : 
//=======================================================================

void TNaming_Localizer::Localize(const TopoDS_Shape& PrevIn,
				 const TDF_Label&    InLab ,
				 const TopoDS_Shape& S,
				 TNaming_ShapesSet& Res) 
{  
  Res.Clear();
  TDF_LabelMap                 Primitives;
  TopTools_MapOfShape          ValidShapes;
  Standard_Integer             PrevTrans = myCurTrans - 1;

  //---------------------------------------------
  // Recuperation du nouveau contenant generateur.
  //---------------------------------------------
  TopoDS_Shape      CurIn;
  TDF_Label         Father  = InLab.Father();
  TNaming_Iterator  itLab(Father,myCurTrans);
  for (; itLab.More(); itLab.Next()) {
    CurIn= itLab.OldShape();
    break;
  }
  
  Standard_Boolean First = 1;
  TNaming_OldShapeIterator OldIt(S, PrevTrans,myUS);
  
  for (; OldIt.More(); OldIt.Next()) {
    if (OldIt.Label().IsEqual(InLab)) {
      TNaming_ShapesSet RO;
      TNaming_ShapesSet RInLab;      
      const TopoDS_Shape& OS = OldIt.Shape();
      //---------------------------------
      // Localisation des generateurs.
      //---------------------------------
      Localize(PrevIn,CurIn ,OS, RO);
      
      //--------------------------------------------------------------------
      // Resultat = intersection des descendants(dans InLab) des generateurs
      //--------------------------------------------------------------------
      TNaming_IteratorOnShapesSet itRO(RO);
      for (; itRO.More(); itRO.Next()) {
	const TopoDS_Shape& CO = itRO.Value();
	TNaming_NewShapeIterator NewIt(CO,myCurTrans,myUS);
	for (; NewIt.More(); NewIt.Next()) {
	  if (NewIt.Label().IsEqual(InLab)) {
	    if (First) Res.Add(NewIt.Shape());
	    else {
	      RInLab.Add(NewIt.Shape());
	    }
	  }
	}
      }
      if (!First) Res.Filter(RInLab);
      First = Standard_False;
    }
  }
}

//=======================================================================
//function : Forward
//purpose  : 
//=======================================================================

void  TNaming_Localizer::Forward(const TopTools_MapOfShape&  CurSubShapes,
				 const TopAbs_ShapeEnum      TS,
				 const TDF_LabelMap&         Primitives,
				 const TopTools_MapOfShape&  ValidShapes,
				 TNaming_ShapesSet&          Res)
{
  //-------------------------------------------------------
  // Descente dans la transaction courrante = myCurTrans
  //----------------------------------------------------------    
  TopTools_MapIteratorOfMapOfShape itV(ValidShapes);
  Standard_Boolean  First = 1;
  Standard_Boolean  YaFromValid      = 0;
  Standard_Boolean  YaFromPrimitives = 0;
  
  for (; itV.More(); itV.Next()) {
    YaFromValid = 1;
    const TopoDS_Shape& NS = itV.Key();
    if (First) {
      GoForward  (NS, CurSubShapes, TS, Res);    
      First = 0;
    }
    else {
      TNaming_ShapesSet   ResNS;
      GoForward  (NS, CurSubShapes, TS, ResNS);
      Res.Filter(ResNS);
    }
  } 

  TDF_MapIteratorOfLabelMap itP(Primitives);
  TNaming_ShapesSet ResPrim;

  for ( ; itP.More(); itP.Next()) {
    YaFromPrimitives = 1;
    const TDF_Label&  Lab = itP.Key();
    TNaming_Iterator  itL(Lab,myCurTrans);
    TNaming_ShapesSet ResLab;

    for (; itL.More(); itL.Next()) {
      const TopoDS_Shape& NS = itL.NewShape();
      GoForward  (NS, CurSubShapes, TS, ResLab);    
    }
    if (First) {
      ResPrim = ResLab;
      First   = 0;
    }
    else 
      ResPrim.Filter(ResLab);
  }

  if (YaFromValid) {
    if (YaFromPrimitives) {
      Res.Filter(ResPrim);
    }
  }
  else {
    Res = ResPrim;
  }
}


//=======================================================================
//function : FilterbyNeighbourgs
//purpose  : 
//=======================================================================

void TNaming_Localizer::FilterByNeighbourgs(const TopoDS_Shape& PrevIn,
					    const TopoDS_Shape& CurIn ,
					    const TopoDS_Shape& S,
					    TNaming_ShapesSet& Res) 
{
  TopTools_MapOfShape         Neighbourg;
  TopAbs_ShapeEnum            TA = S.ShapeType();
  TopAbs_ShapeEnum            TS = TopAbs_ShapeEnum (S.ShapeType()+1);
  TNaming_DataMapOfShapeShapesSet MapShapeRes;
  const TopTools_IndexedDataMapOfShapeListOfShape& PreAnc = 
    Ancestors(PrevIn,TS);

  //--------------------------------
  // Construction des Voisins.
  //--------------------------------
  FindNeighbourg (PrevIn,PreAnc,S,Neighbourg);
  TopTools_MapIteratorOfMapOfShape itNeig(Neighbourg);
  
  TNaming_ShapesSet NewNeig;
  for (; itNeig.More(); itNeig.Next()) {
    const TopoDS_Shape& Neig = itNeig.Key();
    //--------------------------------------------
    // Localisation des voisins.
    //--------------------------------------------
    if (!MapShapeRes.IsBound(Neig)) { 
      TNaming_ShapesSet ResNeig;
      Localize(PrevIn,CurIn,Neig,ResNeig);
      MapShapeRes.Bind(Neig,ResNeig);
      NewNeig.Add(ResNeig);
    }
    else {
      NewNeig.Add(MapShapeRes(Neig));
    }
  }

  //---------------------------------------------
  // Filtre la solution par le resultat du voisin.
  // ie : F est solution si ses voisins dans CurSol
  //      sont dans les descendants des voisins 
  //---------------------------------------------
  TNaming_ShapesSet Reject;
  TNaming_IteratorOnShapesSet itRes(Res);

  const TopTools_IndexedDataMapOfShapeListOfShape& CurAnc = 
    Ancestors(CurIn,TS);
  
  for (; itRes.More(); itRes.Next()) {
    const TopoDS_Shape& Cand = itRes.Value();
    TopTools_MapOfShape Neighbourg;
    FindNeighbourg (CurIn,CurAnc,Cand,Neighbourg);
    TopTools_MapIteratorOfMapOfShape itNeig(Neighbourg);
    for (; itNeig.More(); itNeig.Next()) {
      const TopoDS_Shape& Neig = itNeig.Key();
      if (!NewNeig.Contains(Neig))  {
	Reject.Add(Cand);
	break;
      }
    }
  }
  Res.Remove(Reject);
} 
*/
