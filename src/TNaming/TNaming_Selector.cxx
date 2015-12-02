// Created on: 1999-09-30
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Identifier.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_Selector.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//#define MDTV_DEB_SEL
#ifdef OCCT_DEBUG_SEL
//#define MDTV_DEB_BNP
#include <TopExp_Explorer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TNaming_Tool.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TNaming_UsedShapes.hxx>
void PrintEntry(const TDF_Label&       label, const Standard_Boolean allLevels)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  cout << "LabelEntry = "<< entry << endl;
  if(allLevels) {
    TDF_ChildIterator it (label, allLevels);
    for (; it.More(); it.Next()) {
      TDF_Tool::Entry(it.Value(), entry);
	cout << "ChildLabelEntry = "<< entry << endl;
      }
  }
}
#include <BRepTools.hxx>
static void Write(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 255) return;
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
#endif

#define ORIENTATION_DSOPT
#ifdef ORIENTATION_DSOPT
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapIteratorOfMapOfOrientedShape.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_NamedShape.hxx>
//==========================================================================================
inline static void MapOfOrientedShapes(const TopoDS_Shape& S, TopTools_MapOfOrientedShape& M) 
{
  M.Add(S);
  TopoDS_Iterator It(S,Standard_True,Standard_True);
  while (It.More()) {
    MapOfOrientedShapes(It.Value(),M);
    It.Next();
  }
}
//=======================================================================
static void BuildAtomicMap(const TopoDS_Shape& S, TopTools_MapOfOrientedShape& M)
{
  if(S.ShapeType() > TopAbs_COMPSOLID) return;
  TopoDS_Iterator it(S);
  for(;it.More();it.Next()) {
    if(it.Value().ShapeType() > TopAbs_COMPSOLID) 
      M.Add(it.Value());
    else 
      BuildAtomicMap(it.Value(), M);   
  }
}

//==========================================================================================
static const Handle(TNaming_NamedShape) FindPrevNDS(const Handle(TNaming_NamedShape)& CNS)
{
  Handle(TNaming_NamedShape) aNS;
  TNaming_Iterator it(CNS);
  if(it.More()) {
    if(!it.OldShape().IsNull()) {
      aNS = TNaming_Tool::NamedShape(it.OldShape(), CNS->Label());
      return aNS;
    }
  }
  return aNS;
}

//==========================================================================================
// Purpose: checks correspondens between orientation of sub-shapes of Context and orientation
//          of sub-shapes registered in DF and put under result label
//==========================================================================================
static Standard_Boolean IsSpecificCase(const  TDF_Label& F, const TopoDS_Shape& Context)
{
  Standard_Boolean isFound(Standard_False);
  TopTools_MapOfOrientedShape shapesOfContext;
  MapOfOrientedShapes(Context,shapesOfContext);
  Handle(TNaming_NamedShape) CNS = TNaming_Tool::NamedShape(Context, F);
#ifdef OCCT_DEBUG_BNP
  PrintEntry (CNS->Label(),0);
#endif
  if(!CNS.IsNull()) {
    TNaming_ListOfNamedShape aLNS;
    TDF_ChildIDIterator cit(CNS->Label(), TNaming_NamedShape::GetID(), Standard_False);
    if(!cit.More()) {
      // Naming data structure is empty - no sub-shapes under resulting shape
      const Handle(TNaming_NamedShape) aNS = FindPrevNDS(CNS); //look to old shape data structure if exist
      if(!aNS.IsNull()) {
#ifdef OCCT_DEBUG_BNP
	PrintEntry (aNS->Label(),0);
#endif
	cit.Initialize(aNS->Label(), TNaming_NamedShape::GetID(), Standard_False);
      } else
	return Standard_True;
    }

    for(;cit.More();cit.Next()) {
      Handle(TNaming_NamedShape) NS (Handle(TNaming_NamedShape)::DownCast(cit.Value())); 
      if(!NS.IsNull()) {  
	TopoDS_Shape aS = TNaming_Tool::CurrentShape(NS);
	if(aS.IsNull()) continue;
#ifdef OCCT_DEBUG_BNP
	PrintEntry(NS->Label(), 0);
	cout <<"ShapeType =" << aS.ShapeType() <<endl;
	Write (aS, "BNProblem.brep");
#endif	
	if(aS.ShapeType() != TopAbs_COMPOUND) {//single shape at the child label
	  if(!shapesOfContext.Contains(aS)) {
	    isFound = Standard_True;
	    break;
	  }
	}
	else {
	  TopTools_MapOfOrientedShape M;
	  BuildAtomicMap(aS, M); 
	  TopTools_MapIteratorOfMapOfOrientedShape it(M);
	  for(;it.More();it.Next()) {	      
	    if(!shapesOfContext.Contains(it.Key())) {
#ifdef OCCT_DEBUG_BNP
	      cout <<"BNProblem: ShapeType in AtomicMap = " << it.Key().ShapeType() << " TShape = " <<it.Key().TShape() <<" OR = " <<it.Key().Orientation()  <<endl;
	      Write (it.Key(), "BNProblem_AtomicMap_Item.brep");	      
	      TopTools_MapIteratorOfMapOfOrientedShape itC(shapesOfContext);
	      for(;itC.More(); itC.Next())
		cout <<" ShapeType = " << itC.Key().ShapeType() << " TShape = " << itC.Key().TShape() << " OR = " << itC.Key().Orientation() << endl;
	      
#endif	
	      isFound = Standard_True;
	      break;
	    }
	    if(isFound) break;
	  }
	}
      }
    }
  }
  return isFound;
}

//==========================================================================================
static Standard_Boolean IsSpecificCase2(const  TDF_Label& F, const TopoDS_Shape& Selection)
{
  Standard_Boolean isTheCase(Standard_False);
  if(Selection.ShapeType() == TopAbs_EDGE) {
    Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(Selection, F);
    if(!aNS.IsNull()) { //presented in DF
#ifdef OCCT_DEBUG_BNP
      PrintEntry (aNS->Label(),0);
#endif
      const TopoDS_Shape& aS = TNaming_Tool::CurrentShape(aNS);
      if(!aS.IsNull() && aS.ShapeType() == Selection.ShapeType()) {
	if(Selection.Orientation() != aS.Orientation()) {
	  isTheCase = Standard_True;
	}
      }
    }
  }
  return isTheCase;
}
#endif
//=======================================================================
//function : FindGenerated
//purpose  : Finds all generated from the <S>
//=======================================================================

static void FindGenerated(const Handle(TNaming_NamedShape)& NS, const TopoDS_Shape& S, 
			        TopTools_ListOfShape& theList)

{
  const TDF_Label& LabNS = NS->Label();
  for (TNaming_NewShapeIterator it (S, LabNS); it.More(); it.Next()) {
    if (it.Label() == LabNS) {
      theList.Append(it.Shape());
    }
  }
}
//=======================================================================
//function : IsIdentified
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Selector::IsIdentified (const TDF_Label& L,
						 const TopoDS_Shape& Selection, 
						 Handle(TNaming_NamedShape)& NS,
						 const Standard_Boolean Geometry)
{  
  TopoDS_Shape Context;
  Standard_Boolean OnlyOne =!Geometry;
  TNaming_Identifier Ident(L,Selection,Context,OnlyOne);   
  if (Ident.IsFeature()) {  
    if   (!OnlyOne)  return Standard_False;
    else {
      NS =   Ident.FeatureArg();

      // mpv : external condition
      TDF_LabelMap Forbiden,Valid;
      TopTools_IndexedMapOfShape MS;
      TNaming_NamingTool::CurrentShape(Valid,Forbiden,NS,MS);
      return (MS.Contains(Selection) && MS.Extent() == 1);
    }
  }
  else if(Ident.Type() == TNaming_GENERATION) {
    NS = Ident.NamedShapeOfGeneration();
    if(!NS.IsNull()) {
      TDF_LabelMap Forbiden,Valid;
      TopTools_IndexedMapOfShape MS;
      TNaming_NamingTool::CurrentShape(Valid,Forbiden,NS,MS);
      if(MS.Contains(Selection) && MS.Extent() == 1) {
	const TopoDS_Shape& aS = Ident.ShapeArg();
	TopTools_ListOfShape aList;
	FindGenerated(NS, aS, aList);
	Ident.NextArg();
	while(Ident.MoreArgs()) {
	  const TopoDS_Shape& aShape = Ident.ShapeArg();
	  FindGenerated(NS, aShape, aList);
	  Ident.NextArg();
	}
	const TopoDS_Shape& aC = MS (1);
	Standard_Boolean isEq(Standard_False);
	TopTools_ListIteratorOfListOfShape itl(aList);
	for(;itl.More();itl.Next()) {
	  if(itl.Value() == aC) 
	    isEq = Standard_True;
	  else {
	    isEq = Standard_False;
	    break;
	  }
	}
	return isEq;
      }
    } else 
      return Standard_False;
  }
  return Standard_False;
}

//=======================================================================
//function : TNaming_Selector
//purpose  : 
//=======================================================================

TNaming_Selector::TNaming_Selector (const TDF_Label& L) 
{
  myLabel = L;
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Selector::Select (const TopoDS_Shape& Selection, 
					   const TopoDS_Shape& Context,
					   const Standard_Boolean Geometry,
					   const Standard_Boolean KeepOrientation) const
{
  myLabel.ForgetAllAttributes();  
  Handle(TNaming_NamedShape)NS;
  Standard_Boolean aKeepOrientation((Selection.ShapeType() == TopAbs_VERTEX) ? Standard_False : KeepOrientation);
  if(Selection.ShapeType() == TopAbs_COMPOUND) {
    Standard_Boolean isVertex(Standard_True);
    TopoDS_Iterator it(Selection);
    for(;it.More();it.Next())
      if(it.Value().ShapeType() != TopAbs_VERTEX) {
	isVertex = Standard_False;
	break;
      }
    if(isVertex) aKeepOrientation = Standard_False;
  }
  /* 
 // for debug opposite orientation
 TopoDS_Shape selection;						 
 Standard_Boolean found(Standard_False);
 TopExp_Explorer exp(Context,TopAbs_EDGE);
 for(;exp.More();exp.Next()) {
   TopoDS_Shape E = exp.Current(); 
   if(E.IsSame(Selection) && E.Orientation() != Selection.Orientation()) {
     selection = E;
   found = Standard_True;
   cout <<" FOUND: Entity orientation = " << selection.Orientation() <<endl;
   }
 }
 if (!found)
   selection = Selection;
  */

#ifdef OCCT_DEBUG_SEL
  cout << "SELECTION ORIENTATION = " << Selection.Orientation() <<", TShape = " << Selection.TShape() <<endl;
  //cout << "SELECTION ORIENTATION = " << selection.Orientation() <<", TShape = " << selection.TShape() <<endl;
  PrintEntry(myLabel, 0);
  TNaming::Print(myLabel, cout);
#endif

  if(aKeepOrientation) {
#ifdef ORIENTATION_DSOPT
    const Standard_Boolean aBNproblem = IsSpecificCase(myLabel, Context) || IsSpecificCase2(myLabel, Selection);

    NS = TNaming_Naming::Name (myLabel,Selection,Context,Geometry,aKeepOrientation,aBNproblem);
#else
      NS = TNaming_Naming::Name (myLabel,Selection,Context,Geometry,aKeepOrientation);
#endif    
  }
  else
    if (!IsIdentified (myLabel,Selection,NS,Geometry)) { 
      NS = TNaming_Naming::Name (myLabel,Selection,Context,Geometry,aKeepOrientation);
    }
  if (NS.IsNull()) return Standard_False; 
  //
  // namedshape with SELECTED Evolution
  //
  TNaming_Builder B (myLabel);
  // mpv: if oldShape for selection is some shape from used map of shapes,
  //      then naming structure becomes more complex, can be cycles
  const TopoDS_Shape& aSelection = TNaming_Tool::CurrentShape(NS); //szy
#ifdef OCCT_DEBUG_CHECK_TYPE
  if(!Selection.IsSame(aSelection) && Selection.ShapeType() != TopAbs_COMPOUND) {
    TCollection_AsciiString entry;
    TDF_Tool::Entry(NS->Label(), entry);
    cout << "Selection is Not Same (NSLabel = " <<entry<<"): TShape1 = " << 
      Selection.TShape()->This() << " TShape2 = " <<aSelection.TShape()->This() <<endl;
  }
#endif
  if(aSelection.ShapeType() == TopAbs_COMPOUND && aSelection.ShapeType() != Selection.ShapeType())
    B.Select(aSelection,aSelection); // type migration
  else
    B.Select(Selection,Selection);
  //
  // naming with IDENTITY NameType
  //
  Handle(TNaming_Naming) N = new TNaming_Naming (); 
  N->ChangeName().Type(TNaming_IDENTITY);  
  N->ChangeName().Append(NS);
  N->ChangeName().Orientation(Selection.Orientation());
// inserted by vro 06.09.00:
  N->ChangeName().ShapeType(Selection.ShapeType());

  myLabel.AddAttribute(N);  
  return Standard_True; 
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Selector::Select (const TopoDS_Shape& Selection,
					   const Standard_Boolean Geometry,
					   const Standard_Boolean KeepOrientation) const
{  
  // we give a Null shape. How to guess what is the good context ?
  TopoDS_Shape Context;
//  return Select (Selection,Context,Geometry);
// temporary!!!
  return Select (Selection,Selection,Geometry, KeepOrientation);

}

//=======================================================================
//function : Solve
//purpose  : 
//=======================================================================
Standard_Boolean TNaming_Selector::Solve (TDF_LabelMap& Valid) const
{
  Handle(TNaming_Naming) name;
#ifdef OCCT_DEBUG_SEL
	cout <<"TNaming_Selector::Solve==> "; 
	PrintEntry(myLabel,0);
#endif
  if (myLabel.FindAttribute(TNaming_Naming::GetID(),name)) {
    return name->Solve(Valid);
  }
  return Standard_False;
}

//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
void TNaming_Selector::Arguments (TDF_AttributeMap& args) const
{  
  TDF_Tool::OutReferences(myLabel,args);
}

//=======================================================================
//function : TNaming_Selector
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_Selector::NamedShape() const
{
  Handle(TNaming_NamedShape) NS;
  myLabel.FindAttribute(TNaming_NamedShape::GetID(),NS);
  return NS;
}
