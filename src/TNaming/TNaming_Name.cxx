// Created on: 1997-03-21
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


#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NCollections.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_ShapesSet.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

// mpv modifications 08.04.2002
// end of mpv modifications 08.04.2002
#ifdef OCCT_DEBUG_DBGTOOLS_WRITE
#define MDTV_DEB
#define MDTV_DEB_OR
#define MDTV_DEB_UNN
#define  MDTV_DEB_INT
#define MDTV_DEB_GEN
#define  MDTV_DEB_MODUN
#define MDTV_DEB_FNB
#define  MDTV_DEB_WIN
#define MDTV_DEB_ARG
#define MDTV_DEB_SHELL
#endif
#ifdef OCCT_DEBUG
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <BRepTools.hxx>
#endif
#ifdef OCCT_DEBUG
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
//=======================================================================
void PrintEntry(const TDF_Label&       label)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  cout << "LabelEntry = "<< entry << endl;
}
//=======================================================================
void PrintEntries(const TDF_LabelMap& map)
{
  cout << "=== Labels Map ===" <<endl;
  TCollection_AsciiString entry;
  TDF_MapIteratorOfLabelMap it(map);
  for(;it.More();it.Next()) {
    TDF_Tool::Entry(it.Key(), entry);
      cout << "LabelEntry = "<< entry << endl;
    }
}
#ifdef OCCT_DEBUG_DBGTOOLS_WRITE
//=======================================================================
static void DbgTools_Write(const TopoDS_Shape& shape,
		      const Standard_CString filename) 
{
  char buf[256];
  if(strlen(filename) > 256) return;
  strcpy (buf, filename);
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
static void DbgTools_Write(const TopTools_IndexedMapOfShape& MS,  const Standard_CString filename) 
{
  if (!MS.IsEmpty ()) {
    TCollection_AsciiString aNam (filename);
    for (Standard_Integer anIt = 1; anIt <= MS.Extent(); ++anIt) {
      TCollection_AsciiString aName = aNam + "_" + (anIt - 1) + ".brep";
      DbgTools_Write (MS (anIt), aName.ToCString());
    }
  }
}
//=======================================================================
static void DbgTools_WriteNSOnLabel (const Handle(TNaming_NamedShape)& NS,  
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
	DbgTools_Write ( oldShape, aName1.ToCString());
      if(!newShape.IsNull())
	DbgTools_Write ( newShape, aName2.ToCString());      
    }
  }
}
#endif
#endif
//
//====================================================================
static Standard_Boolean ValidArgs(const TNaming_ListOfNamedShape& Args)
{
  TNaming_ListIteratorOfListOfNamedShape it(Args);
  for (;it.More();it.Next()) {
    const Handle(TNaming_NamedShape)& aNS = it.Value();
    if(aNS.IsNull()) {
#ifdef OCCT_DEBUG_ARG 
      cout << "ValidArgs:: NS (Naming argument) is NULL" <<endl;
#endif	
      return Standard_False;
    }
    else 
      if(aNS->IsEmpty()) {
#ifdef OCCT_DEBUG_ARG
	TCollection_AsciiString entry;
	TDF_Tool::Entry(aNS->Label(), entry);
	cout << "ValidArgs:: Empty NS, Label = " << entry <<endl;
#endif	
      return Standard_False;
    }
      else  
	if(!aNS->IsValid()) {
#ifdef OCCT_DEBUG_ARG 
	  TCollection_AsciiString entry;
	  TDF_Tool::Entry(aNS->Label(), entry);
	  cout << "ValidArgs::Not valid NS Label = " << entry <<endl;
#endif	
	  return Standard_False; 
	}
    }
  return Standard_True;
}

//=======================================================================
//function : TNaming_Name
//purpose  : 
//=======================================================================

TNaming_Name::TNaming_Name() : 
   myType(TNaming_UNKNOWN),
   myIndex(-1)
{
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

void TNaming_Name::Type(const TNaming_NameType aType) 
{
  myType = aType;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

void TNaming_Name::ShapeType(const TopAbs_ShapeEnum T) 
{
  myShapeType = T;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

void TNaming_Name::Shape(const TopoDS_Shape& theShape) 
{
  myShape = theShape;
  
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_Name::Shape() const
{
  return myShape;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void TNaming_Name::Append(const Handle(TNaming_NamedShape)& arg) 
{
  myArgs.Append (arg);
}

//=======================================================================
//function : StopNamedShape
//purpose  : 
//=======================================================================

void TNaming_Name::StopNamedShape (const Handle(TNaming_NamedShape)& arg) 
{
  myStop = arg;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

void TNaming_Name::Index (const Standard_Integer I)
{
  myIndex = I;
}


//=======================================================================
//function : TNaming_NameType
//purpose  : 
//=======================================================================

TNaming_NameType TNaming_Name::Type() const 
{
  return myType;
}

//=======================================================================
//function : TNaming_NameType
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TNaming_Name::ShapeType() const 
{
  return myShapeType;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TNaming_Name::Paste (TNaming_Name& into,
			  const Handle(TDF_RelocationTable)& RT) const
{
  into.myType      = myType;
  into.myShapeType = myShapeType;
  into.myShape     = myShape;
  into.myIndex     = myIndex;
  into.myArgs.Clear();
//  into.myOrientation = myOrientation;
  Handle(TNaming_NamedShape) NS;

  for (TNaming_ListIteratorOfListOfNamedShape it(myArgs); it.More(); it.Next()) {
    RT->HasRelocation(it.Value(),NS);
    into.myArgs.Append (NS);
  }
  if (!myStop.IsNull()) {
    RT->HasRelocation(myStop,NS);
    into.myStop = NS;
  }
  if (!myContextLabel.IsNull()) {
    RT->HasRelocation(myContextLabel,into.myContextLabel);
  }
}

//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
      
const TNaming_ListOfNamedShape& TNaming_Name::Arguments() const
{
  return myArgs;
}

//=======================================================================
//function : Arguments
//purpose  : 
//=======================================================================
      
Handle(TNaming_NamedShape) TNaming_Name::StopNamedShape() const
{
  return myStop;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
      
Standard_Integer TNaming_Name::Index() const
{
  return myIndex;
}

//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================

static TopoDS_Shape MakeShape (const TopTools_IndexedMapOfShape& MS) 
{  
  if (!MS.IsEmpty ()) {
    if (MS.Extent() == 1) {
      return MS (1);
    }
    else {
      TopoDS_Compound C;
      BRep_Builder B;
      B.MakeCompound(C);
      for (Standard_Integer anIt = 1; anIt <= MS.Extent(); ++anIt)
        B.Add (C, MS (anIt));
      return C;
    }
  }
  return TopoDS_Shape();  
}

//=======================================================================
//function : ShapeWithType
//purpose  : Tries to make shape with given type from the given shape
//=======================================================================

static TopoDS_Shape ShapeWithType(const TopoDS_Shape     theShape,
				  const TopAbs_ShapeEnum theType ) {
  if (theShape.IsNull() || theType == TopAbs_SHAPE) return theShape;
  Standard_Integer aType = theShape.ShapeType();
  if (aType == theType) return theShape;

  TopTools_ListOfShape aShapes;
  if (aType == TopAbs_COMPOUND) {
    TopoDS_Iterator anIter(theShape);
    if (anIter.More()) aType = anIter.Value().ShapeType();
    for(;anIter.More();anIter.Next()) aShapes.Append(anIter.Value());
    if (aType == theType) {
      if (aShapes.Extent() == 1) return aShapes.First();
      else return theShape;
    }
  } else aShapes.Append(theShape);

  TopoDS_Shape aResult;
  TopTools_ListIteratorOfListOfShape aListIter(aShapes);

  if (aType < theType) {
    Standard_Integer aCount;
    for(aCount=0;aListIter.More();aListIter.Next()) {
      TopExp_Explorer anExp(aListIter.Value(),theType);
      if (anExp.More()) {
	if (!anExp.Current().IsNull()) {
	  aResult = anExp.Current();
	  aCount++;
	  if (aCount > 1) return theShape;
	}
      }
    }
    if (aCount == 1) return aResult;
  } else { // if the shape type more complex than shapes from aShapes list, try make it
    switch (aType) {
    case TopAbs_VERTEX: // can't do something from vertex
      break;
    case TopAbs_EDGE: {// make wire from edges
      if (theType <= TopAbs_SOLID) break;
      BRepBuilderAPI_MakeWire aMakeWire;
      aMakeWire.Add(aShapes);
      if (!aMakeWire.IsDone()) return theShape;
      if (theType == TopAbs_WIRE) return aMakeWire.Wire();
      aShapes.Clear(); // don't break: we can do something more of it
      aShapes.Append(aMakeWire.Wire());
      aListIter.Initialize(aShapes);
    }
    case TopAbs_WIRE: {// make faceS from wires (one per one)
      if (theType < TopAbs_SOLID) break;
      TopTools_ListOfShape aFaces;
      for(;aListIter.More();aListIter.Next()) {
	BRepBuilderAPI_MakeFace aMakeFace(TopoDS::Wire(aListIter.Value()));
	if (!aMakeFace.IsDone()) aFaces.Append(aMakeFace.Face());
      }
      if (theType == TopAbs_FACE) {
	if (aFaces.Extent() == 1) return aFaces.First();
	return theShape;
      }
      aShapes.Assign(aFaces); // don't break: we can do something more of it
      aListIter.Initialize(aShapes);
    }
    case TopAbs_FACE: {// make shell from faces
      if (theType < TopAbs_SOLID) break;
      BRep_Builder aShellBuilder;
      TopoDS_Shell aShell;
      aShellBuilder.MakeShell(aShell);
      for(;aListIter.More();aListIter.Next()) aShellBuilder.Add(aShell,TopoDS::Face(aListIter.Value()));
      aShell.Closed (BRep_Tool::IsClosed (aShell));
      if (theType == TopAbs_SHELL) return aShell;
      aShapes.Clear(); // don't break: we can do something more of it
      aShapes.Append(aShell);
      aListIter.Initialize(aShapes);
    }
    case TopAbs_SHELL: {// make solids from shells (one per one)
      TopTools_ListOfShape aSolids;
      for(;aListIter.More();aListIter.Next()) {
	BRepBuilderAPI_MakeSolid aMakeSolid(TopoDS::Shell(aListIter.Value()));
	if (aMakeSolid.IsDone()) aSolids.Append(aMakeSolid.Solid());
      }
      if (theType == TopAbs_SOLID) {
	if (aSolids.Extent() == 1) return aSolids.First();
	return theShape;
      }
      aShapes.Assign(aSolids); // don't break: we can do something more of it
      aListIter.Initialize(aShapes);
    }
    case TopAbs_SOLID: {// make compsolid from solids
      BRep_Builder aCompBuilder;
      TopoDS_CompSolid aCompSolid;
      aCompBuilder.MakeCompSolid(aCompSolid);
      for(;aListIter.More();aListIter.Next()) aCompBuilder.Add(aCompSolid,TopoDS::Solid(aListIter.Value()));
      if (theType == TopAbs_COMPSOLID) return aCompSolid;
    }
    }
  }
  return theShape;
}

//=======================================================================
//function : FindModifUntil
//purpose  : 
//=======================================================================

static Standard_Boolean FindModifUntil (TNaming_NewShapeIterator&         it,
					TopTools_IndexedMapOfShape& MS,
					const TopoDS_Shape&               S,
					const Handle(TNaming_NamedShape)& Context)
{ 
#ifdef OCCT_DEBUG_MODUN
  if(!Context.IsNull())
    PrintEntry(Context->Label());
#endif
  Standard_Boolean found = Standard_False;
  for (; it.More(); it.Next()) {
    if (!it.Shape().IsNull()) {
#ifdef OCCT_DEBUG_MODUN
      if(!it.NamedShape().IsNull())
        PrintEntry(it.NamedShape()->Label());
#endif
      if (it.NamedShape() == Context) {
	MS.Add(S);
	found = Standard_True;
      }
      else { // New shape != Context
	TNaming_NewShapeIterator it2(it);
	found = FindModifUntil (it2,MS,it.Shape(),Context);
      }
    }
  }
  return found;
}

//=======================================================================
//function : ModifUntil
//purpose  : returns map <theMS> of generators of Target
//=======================================================================

static void SearchModifUntil (const TDF_LabelMap&               /*Valid*/,
			      const Handle(TNaming_NamedShape)& Target,
			      const TNaming_ListOfNamedShape& theListOfGenerators,
			      TopTools_IndexedMapOfShape& theMS)
{

#ifdef OCCT_DEBUG_MODUN
   DbgTools_WriteNSOnLabel(Target, "SMUntil_"); // Target <== generated
  Standard_Integer i = 0;
  TCollection_AsciiString aGen1("Gens_New_"), aGen2("Gented_Old_"), Und("_");
#endif 
  // Test si S apparait comme oldshape dans Context. : Test if S appears as oldshape in Context.
  Standard_Boolean found = Standard_False;
  for (TNaming_ListIteratorOfListOfNamedShape it(theListOfGenerators); it.More(); it.Next()) {
    const Handle(TNaming_NamedShape)& aNS = it.Value();
#ifdef OCCT_DEBUG_MODUN  
    i++;
    Standard_Integer j = 0;
#endif
    for (TNaming_Iterator itL (aNS); itL.More(); itL.Next()) { // <- generators
      const TopoDS_Shape& S = itL.NewShape();
      found = Standard_False;
      
#ifdef OCCT_DEBUG_MODUN
      j++;
      Standard_Integer k = 0;
      TCollection_AsciiString aNam1 = aGen1 + i + Und + j + ".brep";
      DbgTools_Write(S, aNam1.ToCString());
      PrintEntry(aNS->Label());//NSLabel
#endif   
      TNaming_Iterator itC (Target);
      for  (; itC.More(); itC.Next()) {  // <- generated
	const TopoDS_Shape& OS = itC.OldShape();
#ifdef OCCT_DEBUG_MODUN
	k++;
	TCollection_AsciiString aNam2 = aGen2 + i + Und + j + Und + k + ".brep";
	DbgTools_Write(OS, aNam2.ToCString());
	PrintEntry(Target->Label());//Target Label
#endif 
	if (OS.IsSame(S)) {
	  theMS.Add(S);
	  found = Standard_True;
#ifdef OCCT_DEBUG_MODUN
	  cout << aNam2 << " is Same with " << aNam1 <<endl;
#endif
	  break;
	}
      }
      if (!found) {
	TNaming_NewShapeIterator it1(itL);
	found = FindModifUntil (it1,theMS,S,Target);
      }
    }
  }
}

//=======================================================================
//function : ModifUntil
//purpose  : 
//=======================================================================
// NamedShape for this type is assembled from all last modifications of the 
// last argument shapes (see method  TNaming_NamingTool::CurrentShape), 
// which are not descendants (see method TNaming_NamingTool::BuildDescendants) 
// of the stop shape. This type of naming is used for identification shapes, 
// which has only one parent with evolution PRIMITIVE (or itself), which 
// uniquely identifies it. In most cases stop shape is empty and this algorithm 
// is equal to the algorithm for IDENTITY. 
//=======================================================================
static Standard_Boolean ModifUntil (const TDF_Label&                  L,
				    const TDF_LabelMap&               Valid,
				    const TNaming_ListOfNamedShape&   Args,
				    const Handle(TNaming_NamedShape)& Stop)
{
  TopTools_IndexedMapOfShape MS; 
  TDF_LabelMap Forbiden;
  if(!ValidArgs(Args)) return Standard_False;
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden); // fills Forbidden from Stop

#ifdef OCCT_DEBUG_GEN  
  cout <<"Regenerating ModifUntil => ";
  PrintEntry(L);
  DbgTools_WriteNSOnLabel(Args.Last(), "ModifUntil-");
 
#endif
  // all last modifications of the last argument
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,Args.Last(),MS); 
#ifdef OCCT_DEBUG_GEN  
  Standard_Integer i(0);
  TCollection_AsciiString aNam("ModifUnti_MS_");
  TCollection_AsciiString ext(".brep"); 
#endif
  TNaming_Builder B(L);
  for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS) {
    const TopoDS_Shape& S = MS (anItMS);
    B.Select(S,S);
#ifdef OCCT_DEBUG_GEN  
    TCollection_AsciiString aName = aNam + ++i + ext;
    DbgTools_Write(S, aName.ToCString()) ;
    cout << aName.ToCString() << " TS = " << S.TShape()->This() <<endl;
#endif
  }
  return Standard_True;
}

//=======================================================================
//function : ContShape
//purpose  : 
//=======================================================================
// from the NS of the first argument TNaming_Iterator is started, shape "S" 
// is the NewShape from Iterator with index "myIndex" of the Name, this 
// shape and all last modifications (except NamedShapes - descendants of 
// the stop shape) are the parts of resulting NamedShape.
//=======================================================================
static Standard_Boolean ConstShape (const TDF_Label&                  L,
				    const TDF_LabelMap&               Valid,
				    const TNaming_ListOfNamedShape&   Args,
				    const Handle(TNaming_NamedShape)& Stop,
				    const Standard_Integer            Index)
{
  TopTools_IndexedMapOfShape MS; 
  TDF_LabelMap Forbiden;
  if(!ValidArgs(Args)) return Standard_False;
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden);

  TopoDS_Shape S;
  Standard_Integer i = 1;
  for (TNaming_Iterator it(Args.First()); it.More(); it.Next(), i++) {
    if (Index == i) {
      S = it.NewShape();
      break;
    }
  }
  if (S.IsNull()) return Standard_False;

  TNaming_NamingTool::CurrentShapeFromShape  (Valid,Forbiden,L,S,MS);

 
  TNaming_Builder B(L);
  for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS) {
    const TopoDS_Shape& SS = MS (anItMS);
    B.Select(SS,SS);
  }
  return Standard_True;
}

//=======================================================================
//function : Intersection
//purpose  : 
//=======================================================================
//algorithm does next steps:
// 1. Sets to the "Forbiden" map  all shapes, which are descendants of stop 
//    shape. Named shapes at these labels can't be used.
// 2. Takes first argument (with method CurrentShape) and sets map "S" of 
//    ancestors (shapes, which belong to this one) of its shape with type 
//    "ShapeType" of Name.
// 3. Takes next argument of Name (with method CurrentShape) and removes 
//    from the map "S" all ancestors, which not belongs to the shape of 
//    this argument. This step is repeated for all arguments of this Name.
// 4. Adds to the result NamedShape all rest of shapes from the map "S".
//=======================================================================
static Standard_Boolean Intersection (const TDF_Label&                  L,
				      const TDF_LabelMap&               Valid,
				      const TNaming_ListOfNamedShape&   Args,
				      const Handle(TNaming_NamedShape)& Stop,
				      const TopAbs_ShapeEnum            ShapeType,
				      const Standard_Integer            Index)
{
  if (Args.IsEmpty()) return Standard_False;
  if(!ValidArgs(Args)) return Standard_False;
  TNaming_ListIteratorOfListOfNamedShape it(Args); 
  TopTools_IndexedMapOfShape MS; 
  TDF_LabelMap        Forbiden;

#ifdef OCCT_DEBUG_INT
  if(!Stop.IsNull() && !Stop->Get().IsNull()) {
    DbgTools_Write(Stop->Get(), "Ints_Stop.brep");
    PrintEntry(Stop->Label());
  }
  cout <<"Ints: ShapeType = " << ShapeType << endl;
  cout <<"Argument 1 at ";
  PrintEntry(it.Value()->Label());
#endif 

  TNaming_NamingTool::BuildDescendants (Stop, Forbiden); // <==<1>

#ifdef OCCT_DEBUG_INT
  cout << "Intersection:: Valid Map: "<<endl;
  PrintEntries(Valid);
  cout << "Intersection:: Forbidden Map: "<<endl;
  PrintEntries(Forbiden);
#endif
  TopTools_ListOfShape aListOfAnc;
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,it.Value(),MS); // first argument
  TopoDS_Shape  CS = MakeShape(MS);
  TNaming_ShapesSet S(CS,ShapeType); // <==<2>
  aListOfAnc.Append(CS);
#ifdef OCCT_DEBUG_INT
  if(!CS.IsNull())
    DbgTools_Write(CS, "Int_CS_1.brep");
  Standard_Integer i=2;
  TCollection_AsciiString aNam("Int_CS_");
  TCollection_AsciiString ext(".brep");
 
#endif 
  it.Next();  // <<===<3.1>
  for (; it.More(); it.Next()) {
    MS.Clear();
    TNaming_NamingTool::CurrentShape (Valid,Forbiden,it.Value(),MS);
    CS = MakeShape(MS);
    aListOfAnc.Append(CS);
#ifdef OCCT_DEBUG_INT
    TCollection_AsciiString aName = aNam + i++ + ext;      
    DbgTools_Write(CS, aName.ToCString()) ;
    cout <<"Argument " << i << " at ";
    PrintEntry(it.Value()->Label());
#endif  

    TNaming_ShapesSet OS(CS,ShapeType);
    S.Filter(OS); //<<===<3.2>
#ifdef OCCT_DEBUG_INT
    Standard_Integer j = 1;
    TCollection_AsciiString aNam2("SSMap_"), aName3;
    TopTools_MapIteratorOfMapOfShape itm(S.Map());
    for(;itm.More();itm.Next(), j++) {
      aName3 = aNam2 + i + "_" + j + ".brep";
      DbgTools_Write(itm.Key(), aName3.ToCString());
    }
#endif 
  }

#ifdef OCCT_DEBUG_INT
  aNam = "Int_S_";
  i =1;
#endif

  TNaming_Builder B(L); //<<===<4>
  Standard_Boolean isOK(Standard_False);
  if(S.Map().Extent() > 1 && Index > 0 && ShapeType == TopAbs_EDGE) {
    Standard_Integer indxE(0), nbE(0), indxW(0),nbW(0), indxF(0);
    indxE =  Index & 0x000000FF;
    nbE   = (Index & 0x0000FF00) >> 8;
    indxW = (Index & 0x000F0000) >> 16;
    nbW   = (Index & 0x00F00000) >> 20;
    indxF = (Index & 0x0F000000) >> 24;
    Standard_Integer k(1);
    TopoDS_Shape aS;
    TopTools_ListIteratorOfListOfShape itl(aListOfAnc);
    for(;itl.More();itl.Next(),k++) {
      if(indxF == k) {
	aS = itl.Value();
	break;
      }      
    }
#ifdef OCCT_DEBUG_INT
    cout <<"Kept: indxE = " << indxE  <<" maxENum = " << nbE << " indxW = " <<indxW << " nbW = " <<nbW<<endl;
#endif      
    Standard_Integer aNbW(0), aCaseW(0);
    TopoDS_Iterator it2(aS);
    for (;it2.More();it2.Next()) aNbW++;  
    if(aNbW == nbW) aCaseW = 1;//exact solution for wire (nb of wires is kept)
    else aCaseW = 2; // indefinite description ==> compound which can include expected wire    
    if(aCaseW == 1) {      
      TopoDS_Shape aWire;
      Standard_Integer i(1);
      it2.Initialize(aS);
      for (;it2.More();it2.Next(),i++) {
	if(indxW == i) {
	  aWire = it2.Value();
	  break;
	}
      }
      Standard_Integer aNbE(0), aCaseE(0);
      it2.Initialize(aWire);
      for (;it2.More();it2.Next()) aNbE++;
      if(aNbE == nbE) aCaseE = 1;//exact solution for edge
      else aCaseE = 2;
      if(aCaseE == 1) {
	i=1;
	TopoDS_Shape anEdge;
	it2.Initialize(aWire);
	for (;it2.More();it2.Next(),i++) {
	  if(indxE == i) {
	    anEdge = it2.Value();
	    break;
	  }
	}
	if(!anEdge.IsNull()) {
	  B.Select(anEdge, anEdge);
	  isOK = Standard_True;
	}
      }
    }   
  } 
  if(!isOK)
#ifdef OCCT_DEBUG_INT
    for (TopTools_MapIteratorOfMapOfShape itM(S.Map()); itM.More(); itM.Next(),i++) {
#else
      
    for (TopTools_MapIteratorOfMapOfShape itM(S.Map()); itM.More(); itM.Next()) {
#endif
      const TopoDS_Shape& S1 = itM.Key();
#ifdef OCCT_DEBUG_INT
      TCollection_AsciiString aName = aNam + i + ext;      
      DbgTools_Write(S1, aName.ToCString()) ;
#endif  

      B.Select(S1,S1);
      isOK = Standard_True;
    }
  return isOK;
}
//=======================================================================
static void KeepInList(const TopoDS_Shape& CS, const TopAbs_ShapeEnum Type, TopTools_ListOfShape& aList)   
{
  if (CS.IsNull()) return;

  if (Type == TopAbs_SHAPE) { 
    if (CS.ShapeType() == TopAbs_SOLID ||
	CS.ShapeType() == TopAbs_FACE  ||
	CS.ShapeType() == TopAbs_EDGE  ||
	CS.ShapeType() == TopAbs_VERTEX ) {
      aList.Append(CS);
    }
    else {
      for (TopoDS_Iterator it(CS) ; it.More(); it.Next()) {
	aList.Append(it.Value());
      }
    }
  }
  else {
    if (Type > CS.ShapeType()) {
      for (TopExp_Explorer exp(CS,Type) ; exp.More(); exp.Next()) {
	aList.Append(exp.Current());
      }
    } else {
     aList.Append(CS);
    }
  }
}

//=======================================================================
//function : Union
//purpose  : 
//=======================================================================
// Resulting NamedShape contains compound of next shapes: 
// compound of last modifications of each argument (see CurrentShape method) 
// without descendants of the stop shape.
//=======================================================================
static Standard_Boolean Union (const TDF_Label&                  L,
			       const TDF_LabelMap&               Valid,
			       const TNaming_ListOfNamedShape&   Args,
			       const Handle(TNaming_NamedShape)& Stop,
			       const TopAbs_ShapeEnum            ShapeType,
			       const TDF_Label&                  ContextLabel)
{  
  if (Args.IsEmpty()) return Standard_False;
  if(!ValidArgs(Args)) return Standard_False;
  // temporary solution for Orientation name
  Standard_Boolean isOr(Standard_True);
/* not completed
  const TDF_Label& aLabel = L.Father();
  if(!aLabel.IsNull()) {
  PrintEntry(L);
  PrintEntry(aLabel);
    Handle (TNaming_Naming)  Naming;
    if(aLabel.FindAttribute(TNaming_Naming::GetID(), Naming)) {
      const TNaming_Name& aName = Naming->GetName();
      if(aName.Type() == TNaming_ORIENTATION) {
	const TNaming_ListOfNamedShape&   Args = aName.Arguments();
	if(Args.Extent() > 2) {
	  const Handle(TNaming_NamedShape)& A = Args.First();
	  if(!A.IsNull()) {
	    PrintEntry(A->Label());
	    if (A->Label() == L) 
	      isOr = Standard_True;	    
	  }
	} else 
	  if(!Args.Extent())
	    isOr = Standard_True;
      }
    }
  }
*/
  // end of temp. sol.

  TNaming_ListIteratorOfListOfNamedShape it(Args);
  TopTools_IndexedMapOfShape MS; 
  TDF_LabelMap        Forbiden;
  
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden);//fill Forbidden
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,it.Value(),MS); // fill MS with last modifications of the first argument
  TopoDS_Shape  CS = MakeShape(MS);

  TopTools_ListOfShape aListS;
  if(isOr)
    KeepInList(CS,ShapeType,aListS);
  TNaming_ShapesSet S(CS,ShapeType);//fill internal map of shapeset by shapes of the specified type
#ifdef OCCT_DEBUG_UNN
  TCollection_AsciiString entry; 
  TDF_Tool::Entry(it.Value()->Label(), entry);
  TCollection_AsciiString Nam("Arg_");
  TCollection_AsciiString aNam = Nam + entry + "_" + "1.brep";
  DbgTools_Write(CS, aNam.ToCString());
  Standard_Integer ii = 1;
#endif
  it.Next();
  for (; it.More(); it.Next()) {
#ifdef OCCT_DEBUG_UNN  
    TDF_Tool::Entry(it.Value()->Label(), entry);
#endif
      MS.Clear();
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,it.Value(),MS);// fill MS with last modifications of the it.Value()
      CS = MakeShape(MS);
      if(isOr)
	KeepInList(CS,ShapeType,aListS); 
      TNaming_ShapesSet OS(CS,ShapeType);
      S.Add(OS); //concatenate both shapesets
 
#ifdef OCCT_DEBUG_UNN 
    ii++;
    TCollection_AsciiString aNm = Nam + entry + "_" + ii + ".brep";
    DbgTools_Write(CS, aNm.ToCString());
    cout <<"Arg: Entry = " <<entry <<"  TShape = " << CS.TShape() <<endl;
#endif
  }

// start szy 27.05.08
  TopoDS_Shape aCand;
  Standard_Boolean found = Standard_False;
  if(!ContextLabel.IsNull()) {
    Handle(TNaming_NamedShape) CNS;
    ContextLabel.FindAttribute(TNaming_NamedShape::GetID(),CNS);
    TopoDS_Shape aContext;
    if(!CNS.IsNull()) {
      MS.Clear();
      TNaming_NamingTool::CurrentShape (Valid, Forbiden, CNS, MS);
      aContext = MakeShape(MS);
#ifdef OCCT_DEBUG_UNN 
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(ContextLabel, anEntry);
      cout << "UNION: Context Label = " <<  anEntry << endl;
      DbgTools_Write(aContext, "Union_Context.brep");
      TCollection_AsciiString aN ("aMap_");
      TopTools_MapIteratorOfMapOfShape it(S.Map());
      for(Standard_Integer i=1; it.More();it.Next(), i++) {
	TCollection_AsciiString aName = aN + i + ".brep";
	DbgTools_Write(it.Key(), aName.ToCString());
      }
#endif
    }
    TopTools_ListOfShape aList;
    TopExp_Explorer anExpl(aContext, ShapeType);
    for(;anExpl.More(); anExpl.Next()) 
      aList.Append(anExpl.Current());
#ifdef OCCT_DEBUG_UNN
    cout <<"UNION: ShapeType = " << ShapeType << " List ext = " << aList.Extent()<<endl;
    TopAbs_ShapeEnum aTyp = TopAbs_SHAPE;
    TopTools_MapIteratorOfMapOfShape it1 (S.Map());
    for (int i=1;it1.More();it1.Next(),i++) {
      cout << "Map("<<i<<"): TShape = " << it1.Key().TShape() << " Orient = " << it1.Key().Orientation() <<endl;
      aTyp = it1.Key().ShapeType();
    }
    
    TopExp_Explorer exp(aContext, aTyp);
    for(int i =1;exp.More();exp.Next(), i++) {
     cout << "Context("<<i<<"): TShape = " << exp.Current().TShape() << " Orient = " << exp.Current().Orientation() <<endl;
    }
	    
#endif
    TopTools_ListIteratorOfListOfShape itl(aList);
    for(;itl.More();itl.Next()) {
      aCand = itl.Value(); 
#ifdef OCCT_DEBUG_UNN 
      DbgTools_Write(aCand, "Cand.brep");
#endif
      Standard_Integer num = S.Map().Extent();
      anExpl.Init(aCand, (ShapeType == TopAbs_WIRE) ? TopAbs_EDGE : TopAbs_FACE); 
      for(;anExpl.More();anExpl.Next()) {
	if(S.Map().Contains(anExpl.Current()))
	  num--;
      } 
      if(num == 0) {
	found = Standard_True; 
	break;
      }
    }
// end szy 27.05.08
  }

  TNaming_Builder B(L);
#ifdef OCCT_DEBUG_UNN
  if(!ContextLabel.IsNull()) {
    if(found) cout << "UNION: Shape is found in Context" <<endl;
    else cout << "UNION: Shape is NOT found in Context" <<endl;
  }
#endif
  if(found) 
    B.Select(aCand, aCand);
  else {
    BRep_Builder aCompoundBuilder;
    TopoDS_Compound aCompound;
    aCompoundBuilder.MakeCompound(aCompound);
    if(!isOr)
      for (TopTools_MapIteratorOfMapOfShape itM(S.Map()); itM.More(); itM.Next()) {
	aCompoundBuilder.Add(aCompound,itM.Key());
      }
    else
      for (TopTools_ListIteratorOfListOfShape itL(aListS); itL.More(); itL.Next()) {
	aCompoundBuilder.Add(aCompound,itL.Value());
      }
    TopoDS_Shape aShape = ShapeWithType(aCompound,ShapeType);
#ifdef OCCT_DEBUG_UNN 
    DbgTools_Write(aShape, "Union_Selected.brep");
    DbgTools_Write(aCompound, "Union_Compound.brep");
#endif
    B.Select(aShape,aShape);
  }
  return Standard_True;
}

//=======================================================================
static TopoDS_Shape FindShape(const TNaming_DataMapOfShapeMapOfShape& DM) 
{
  TopoDS_Shape aResult;
  Standard_Integer aNum = DM.Extent();
  if(aNum < 1) return aResult;  
  TopTools_ListOfShape List;
  TNaming_DataMapIteratorOfDataMapOfShapeMapOfShape it(DM);
  if(it.More()) {
    const TopoDS_Shape& aKey1 = it.Key();
    const TNaming_MapOfShape& aMap = it.Value();

    TNaming_MapIteratorOfMapOfShape itm(aMap); // iterate first map
    for (;itm.More();itm.Next()) {
      const TopoDS_Shape& aS = itm.Key(); // element of the first map
      Standard_Boolean isCand(Standard_True); // aS is a Candidate
      TNaming_DataMapIteratorOfDataMapOfShapeMapOfShape it2(DM);
      for (;it2.More();it2.Next()) {
	const TopoDS_Shape& aKey2 = it2.Key();
	if(aKey2 == aKey1) continue;
	const TNaming_MapOfShape& aMap2 = it2.Value();
	if(!aMap2.Contains(aS)) isCand = Standard_False;
      } 
      if(isCand)
	List.Append(aS);
    }
  }
  if(List.IsEmpty()) return aResult;
  if(List.Extent() == 1) return List.First();
  TopTools_ListIteratorOfListOfShape itl (List);
  TopoDS_Compound Compound;
  BRep_Builder B;
  B.MakeCompound(Compound);
  for (; itl.More(); itl.Next()){ 
    B.Add(Compound,itl.Value());
  }
  return Compound; 
}

//=======================================================================
//function : Generation
//purpose  : Resolves Name from arguments: arg1 - generated (target shape)
//         : arg2 - the generator: the oldest ancestor (usually NS with 
//         : PRIMITIVE evolution. (See TNaming_Localizer::FindGenerator).
//         : Resulting NamedShape contains shape, which is in the first 
//         : argument NamedShape and is modification of the last argument NS.
//=======================================================================

static Standard_Boolean  Generated (const TDF_Label&                L,
				    const TDF_LabelMap&             Valid,
				    const TNaming_ListOfNamedShape& Args)
{
  if (Args.Extent() < 2) {
    throw Standard_ConstructionError("TNaming_Name::Solve: => Generated");
  }
  // First argument : label of generation
  // Next arguments : generators.

  if(!ValidArgs(Args)) return Standard_False;

  TDF_Label   LabelOfGeneration = Args.First()->Label();
#ifdef OCCT_DEBUG_GEN
      DbgTools_Write(Args.First()->Get(),  "Generated.brep") ;
#endif
  // Nouvell valeurs des generateurs dans l attribut de generation
  TopTools_IndexedMapOfShape aMS;
  TNaming_ListOfNamedShape aGenerators; 
  aGenerators.Assign(Args); 
  aGenerators.RemoveFirst(); 
#ifdef OCCT_DEBUG_GEN
      DbgTools_Write(aGenerators.First()->Get(),  "Generators.brep") ;
#endif
  SearchModifUntil (Valid, Args.First(), aGenerators, aMS);
  Handle(TNaming_Naming) aNaming;
  TopoDS_Shape aSelection;
  L.FindAttribute(TNaming_Naming::GetID(),aNaming);
  if(!aNaming.IsNull()) 
    aSelection = aNaming->GetName().Shape();
#ifdef OCCT_DEBUG_GEN
  DbgTools_Write(aSelection,  "G_Selection.brep") ;
  cout << "Generated::SearchModifUntil aMS.Extent() = " << aMS.Extent() <<endl;
  DbgTools_Write(aMS, "SearchModifUntil_Result");
#endif
   Handle(TNaming_NamedShape) anOldNS;
   Standard_Integer aVer = -1;// Initial build of name
   L.FindAttribute(TNaming_NamedShape::GetID(),anOldNS);
   if(!anOldNS.IsNull()) 
     aVer = anOldNS->Version();

#ifdef OCCT_DEBUG_GEN
  Standard_Integer i = 0, j=1;
  TCollection_AsciiString aNam2("Gen_New_");
  TCollection_AsciiString aNam1("Gen_Old_");
  TCollection_AsciiString ext(".brep");
#endif
  TNaming_Builder B(L); // NS
  TopTools_ListOfShape aList;
  TNaming_DataMapOfShapeMapOfShape aDM; 
  for (Standard_Integer anItMS = 1; anItMS <= aMS.Extent(); ++anItMS) {
    const TopoDS_Shape& OS = aMS (anItMS);
#ifdef OCCT_DEBUG_GEN
    TCollection_AsciiString aName = aNam1 + ++i + ext;      
    DbgTools_Write(OS, aName.ToCString()) ;
    Standard_Integer j=0;
#endif
    TNaming_MapOfShape aMapDM; 
    for (TNaming_NewShapeIterator itNew(OS,L); itNew.More(); itNew.Next()) 
      if (itNew.Label() == LabelOfGeneration) {
	aMapDM.Add(itNew.Shape());
	aList.Append(itNew.Shape());//szy 21.10.03
#ifdef OCCT_DEBUG_GEN
	TCollection_AsciiString aName = aNam2 + i +  "_" + ++j + ext;      
	DbgTools_Write(itNew.Shape(), aName.ToCString()) ;
#endif
      }
    if(aMapDM.Extent())
      aDM.Bind(OS, aMapDM);
  }

  if(aVer == -1) { //initial 
    Standard_Integer i = 1;
    TNaming_Name& aName =  aNaming->ChangeName();
    TopTools_ListIteratorOfListOfShape it(aList);
    if(!aSelection.IsNull()) {
      for(;it.More();it.Next(),i++) {
	if(it.Value().IsSame(aSelection)) {
	  B.Select(it.Value(), it.Value());
	  aName.Index(i); // for debug - index of selection in the list
	  break;
	}
      }
    } else {// Selection == Null
      for(;it.More();it.Next())
	B.Select(it.Value(), it.Value()); 
    }
  }
  else { 
    // *** Regeneration *** //
    if(aList.Extent() == 1) { // trivial case
      B.Select(aList.Last(), aList.Last()); 
    } 
    else {
      TNaming_Name& aName =  aNaming->ChangeName();
      const TopAbs_ShapeEnum aType = aName.ShapeType();
      TopTools_ListOfShape aList2; 
      TopTools_ListIteratorOfListOfShape it(aList);
      for(;it.More();it.Next()) {
	if(it.Value().ShapeType() == aType) //collect only the same type
	  aList2.Append(it.Value());
      }
      if(!aList2.Extent()) return Standard_False; // Empty

      Standard_Boolean found = Standard_False;
      TopoDS_Shape aShape = FindShape(aDM);
#ifdef OCCT_DEBUG_GEN
      if(!aShape.IsNull())
	DbgTools_Write(aShape,  "G_FindShape.brep") ;
#endif
      if(!aShape.IsNull()) found = Standard_True;
#ifdef OCCT_DEBUG_GEN
      cout << "Generated ==>aGenerators.Extent() = " <<aGenerators.Extent() <<" aMS.Extent()= " <<aMS.Extent()<<endl;
#endif
      if(found) {
#ifdef OCCT_DEBUG_GEN
      cout << "Generated ==> Shape is found!" <<endl;
#endif
	TopTools_ListOfShape aLM;
	Standard_Boolean aHas = Standard_False;
	Standard_Boolean a1NB = Standard_False;
	if(aGenerators.Extent() != aMS.Extent()) { //missed generators
	  aHas = Standard_True;//has lost generatos
#ifdef OCCT_DEBUG_GEN
      cout << "Generated ==> has lost generatos!" <<endl;
#endif
	  for (TNaming_ListIteratorOfListOfNamedShape itg(aGenerators); itg.More(); itg.Next()) {
	    if(!aMS.Contains(itg.Value()->Get())) 
	      aLM.Append(itg.Value()->Get());
	  }
	  if(aLM.Extent() == 1) {//lost 1
	    TopTools_ListIteratorOfListOfShape itm(aLM);
	    TopoDS_Shape aSM = itm.Value(); // Missed
      for (Standard_Integer anItMS1 = 1; anItMS1 <= aMS.Extent(); ++anItMS1) {
	      const TopoDS_Shape& aS = aMS (anItMS1);
	      if(aSM.ShapeType() == aS.ShapeType()) {
		if(aS.ShapeType() == TopAbs_EDGE) {
		  TopoDS_Vertex aVCom;
		  if(TopExp::CommonVertex(TopoDS::Edge(aS), TopoDS::Edge(aSM), aVCom))
		    {a1NB = Standard_True; 
		     break;} //lost only 1 neigbour
		} else if(aS.ShapeType() == TopAbs_FACE) {
		  TopExp_Explorer expl1(aS, TopAbs_EDGE);
		  for(;expl1.More();expl1.Next()) {
		    TopExp_Explorer expl2(aSM, TopAbs_EDGE);
		    for(;expl2.More();expl2.Next()) {
		      if(expl1.Current().IsSame(expl2.Current()))
			{a1NB = Standard_True;
			 break;} //lost only 1 neigbour
		    }
		  }
		}
	      }
	    }
// 	    if(aShape.ShapeType() == TopAbs_VERTEX && a1NBE) {
// 	//if atleast 1 Gen was missed and the Gen is Edge
// 	      TopTools_ListIteratorOfListOfShape it(aList);
// 	      for(;it.More();it.Next()) {
// 		if(it.Value().ShapeType() == TopAbs_EDGE) {
// 		  const TopoDS_Shape& aV1 = TopExp::FirstVertex(TopoDS::Edge(it.Value()));
// 		  const TopoDS_Shape& aV2 = TopExp::LastVertex(TopoDS::Edge(it.Value()));
// 		  if(aShape.IsSame(aV1)) {aShape2 =  aV2; 	cout << "##### => V2 " <<endl;break;}
// 		  else 
// 		    if(aShape.IsSame(aV2)) {aShape2 =  aV1;	cout << "##### => V1 " <<endl;break;}
// 		}	  
// 	      }
// 	    }
	  }
	}
	if(!aHas) // all arguments were kept
	  B.Select(aShape, aShape); //only this case is correct on 100%
	else {
	  if (a1NB) //Has, but may be ...
	    B.Select(aShape, aShape); 	
	  else { 
	    // put Compound, may be if possible processed later in Sel. Driver
	    TopTools_ListIteratorOfListOfShape it1(aList2);
	    for(;it1.More();it1.Next())
	      B.Select(it1.Value(), it1.Value()); 
	  }
	}
    } else 
      {	//not found
#ifdef OCCT_DEBUG_GEN
	cout << "Generated ==> Shape is NOT found! Probably Compound will be built" <<endl;
#endif

	TopTools_ListIteratorOfListOfShape it2(aList2);
	for(;it2.More();it2.Next())
	  B.Select(it2.Value(), it2.Value()); 
      }
    }
  }    
  return Standard_True;
}

//=======================================================================
//function : Identity
//purpose  : Regenerates Naming attribute with Name = IDENTITY
//=======================================================================
// Name with this type must contain only one NamedShape attribute as argument. 
// Algorithm takes all last modifications of NamedShape of this argument  
// starting with this one ( see method TNaming_NamingTool::CurrentShape ). 
// Algorithm takes only NamedShapes belonging to the labels from the Valid 
// labels map (if it's not empty) and put to the resulting NamedShape as compound. 
//=======================================================================
static Standard_Boolean Identity (const TDF_Label&                L,
                                  const TDF_LabelMap&             Valid,
                                  const TNaming_ListOfNamedShape& Args,
                                  const TopAbs_ShapeEnum          ShapeType)
{
  if (Args.Extent() > 2) {
    throw Standard_ConstructionError("TNaming_Name::Solve");
    }
  if(!ValidArgs(Args)) return Standard_False;
  const Handle(TNaming_NamedShape)& A = Args.First();
  TopTools_IndexedMapOfShape MS;
  TDF_LabelMap        Forbiden;
  TNaming_NamingTool::CurrentShape (Valid,Forbiden,A,MS);
#ifdef OCCT_DEBUG_SOL2
  //TCollection_AsciiString entry;
  //TDF_Tool::Entry(L, entry);
  //TDF_Tool::Entry(A->Label(), entry);
#endif  
  TNaming_Builder B(L);
  for (Standard_Integer anItMS = 1; anItMS <= MS.Extent(); ++anItMS) {
    const TopoDS_Shape& S = ShapeWithType (MS (anItMS), ShapeType);
#ifdef OCCT_DEBUG_SOL2
    //TopAbs_Orientation Or = S.Orientation();
#endif
    B.Select(S,S);
  }
  return Standard_True;
}

//=======================================================================
//function : FilterByNeighbourgs
//purpose  : regenerated the specified shape with help of its neighbours
//=======================================================================
// result -  is a subshape of the first argument of the Name with type = 
// ShapeType of this Name, which has a common subshapes (boundaries) with 
// each neighbour - shapes from the other arguments of the Name.
//=======================================================================
static Standard_Boolean  FilterByNeighbourgs (const TDF_Label&                L,
					      const TDF_LabelMap&             Valid,
					      const TNaming_ListOfNamedShape& Args,
					      const Handle(TNaming_NamedShape)& Stop,
					      const TopAbs_ShapeEnum          ShapeType)
{  

  TNaming_Builder B(L); 

  TDF_LabelMap        Forbiden;
  if(!ValidArgs(Args)) return Standard_False;
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden); //all descendants of Stop (New shapes) are forbidden
  if (!Stop.IsNull())    Forbiden.Remove(Stop->Label());
  //----------------------------------------
  // First argument: collection has to be filtered.
  //----------------------------------------
  Handle(TNaming_NamedShape) Cand  = Args.First(); //collection of candidates
   
#ifdef OCCT_DEBUG_FNB
  Standard_Integer i = 1;
  TCollection_AsciiString aNam("Cand_");
  TCollection_AsciiString ext(".brep");
  DbgTools_WriteNSOnLabel(Cand, aNam.ToCString());  
  cout << "Cand (Args.First()) Label = ";
  PrintEntry(Cand->Label());  
  cout << "Valid Label map:" <<endl;
  PrintEntries(Valid);
#endif

  TopTools_IndexedMapOfShape SCand;
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,Cand,SCand);//fills SCand with last modifications of Cand. CandNS should be at the same level (before) as NS of FilterByNBS

#ifdef OCCT_DEBUG_FNB
  TCollection_AsciiString aNam2("SCand");
  DbgTools_Write(SCand, aNam2.ToCString());  
  cout <<"SCand Extent = " << SCand.Extent() << " Expected ShapeType = " << ShapeType << endl;
#endif 

  //------------------------------------------------------------
  // Autres arguments : contiennent les voisins du bon candidat.
  // Other arguments  : contains the neighbors of the good candidate.
  //------------------------------------------------------------
  TopAbs_ShapeEnum              TC = TopAbs_EDGE;
  if (ShapeType == TopAbs_EDGE) TC = TopAbs_VERTEX;
  if (ShapeType == TopAbs_VERTEX) TC = TopAbs_VERTEX; // szy 31.03.10 - to process case when Candidate is of type Vertex
 
#ifdef OCCT_DEBUG_FNB
  i=1;
  aNam = "Boundaries";
#endif
  Standard_Boolean isDone = Standard_False;
  if(SCand.Extent() == 1) { // check if a collection is inside
    TopoDS_Shape aS = SCand (1);
    if(!aS.IsNull()) 
      if(aS.ShapeType() == TopAbs_COMPOUND && aS.ShapeType() != ShapeType) {
        SCand.Clear();
	TopoDS_Iterator itt(aS);
	for(;itt.More();itt.Next()) 
	  SCand.Add(itt.Value());      
      }
  }
  for (Standard_Integer anItSCand = 1; anItSCand <= SCand.Extent(); ++anItSCand) { //1
    const TopoDS_Shape& S = SCand (anItSCand);
    TopTools_MapOfShape Boundaries;
    if(S.ShapeType() == TopAbs_VERTEX) //# szy 31.03.10
      Boundaries.Add (S); //#
    else  //#
      for (TopExp_Explorer exp(S,TC); exp.More(); exp.Next()) { //put boundaries of each candidate (from SCand) to the Boundaries map
	Boundaries.Add (exp.Current());
#ifdef OCCT_DEBUG_FNB
	TCollection_AsciiString aName = aNam + i++ + ext;      
	DbgTools_Write(exp.Current(),  aName.ToCString()) ;
#endif
      }
    
    TNaming_ListIteratorOfListOfNamedShape it(Args); 
    it.Next(); 
    Standard_Boolean Keep = 1;
#ifdef OCCT_DEBUG_FNB
    cout <<"Args number = " << Args.Extent() <<endl;
    i=1;
    aNam = "Boundaries";
#endif
    for ( ; it.More(); it.Next()) { //2 ==> for each Arg
      Standard_Boolean Connected = Standard_False;
      // Le candidat doit etre  connexe a au moins un shape de
      // chaque NamedShape des voisins.
      // The candidate should be connectedand and have at least one shape of NamedShape
      // of each neighbor.
      const Handle(TNaming_NamedShape)& NSVois = it.Value();  //neighbor 

#ifdef OCCT_DEBUG_FNB
      DbgTools_WriteNSOnLabel(NSVois, "Next_Neighbor_") ;
#endif
      
      TopTools_IndexedMapOfShape SVois;
      TNaming_NamingTool::CurrentShape  (Valid, Forbiden,NSVois,SVois); // fills SVois with last modifications of NSVois

#ifdef OCCT_DEBUG_FNB
      TCollection_AsciiString aNam2("SVois");
      DbgTools_Write(SVois, aNam2.ToCString());
#endif 

      for (Standard_Integer anItSVois = 1; anItSVois <= SVois.Extent(); ++anItSVois) { //6
        const TopoDS_Shape& Vois = SVois (anItSVois);
	for (TopExp_Explorer exp1(Vois,TC); exp1.More(); exp1.Next()) { //7
	  if (Boundaries.Contains(exp1.Current())) {
	    Connected = Standard_True; // has common boundaries with candidate shape
#ifdef OCCT_DEBUG_FNB
	    DbgTools_Write(Vois, "Neighbor_Connected.brep");
#endif
	    break;
	  }
	} //7
	if (Connected) break;
      } //6
      if (!Connected) {
	Keep = 0;
	break;
      }
    } //2
    if (Keep) {
      B.Select (S,S);
	  isDone = Standard_True;
#ifdef OCCT_DEBUG_FNB
      DbgTools_Write(S,  "FilterByNbs_Sel.brep") ;
#endif
    }
  } //1
  return isDone;
}

//=======================================================================
static const TopoDS_Shape FindSubShapeInAncestor(const TopoDS_Shape& Selection, const TopoDS_Shape& Context )
{
#ifdef OCCT_DEBUG_OR_AG
  DbgTools_Write(Selection, "Orientation_Selection.brep");
  DbgTools_Write(Context, "Orientation_Context.brep");
  TopExp_Explorer expl1(Context, Selection.ShapeType());
  int i = 0;
  TCollection_AsciiString SS( "Orientation_Current_");
  for(;expl1.More(); expl1.Next()) {      
    if(expl1.Current().IsSame(Selection)) {
      i++;
      cout <<"FindSubShape:  = " <<expl1.Current().ShapeType() << " TS = " <<expl1.Current().TShape() << endl;
      TCollection_AsciiString nam = SS + i + ".brep";
      DbgTools_Write(expl1.Current(), nam.ToCString());	
    }
  }
#endif 
  if(Selection.ShapeType() != TopAbs_COMPOUND) {
    TopExp_Explorer anExpl(Context, Selection.ShapeType());
    for(;anExpl.More(); anExpl.Next()) {
#ifdef OCCT_DEBUG_OR_AG
      cout <<"FindSubShape:  = " <<anExpl.Current().ShapeType() << " TS = " <<anExpl.Current().TShape()->This() << endl;
      DbgTools_Write(anExpl.Current(), "Orientation_Current.brep");
#endif    
      if(anExpl.Current().IsSame(Selection)) 
	return anExpl.Current();
    }
  }

  return TopoDS_Shape();
}

//=======================================================================
//static Standard_Integer Count(const TopoDS_Shape& S)
//{
//  Standard_Integer N(0);
//  TopoDS_Iterator it(S);
//  for(;it.More();it.Next()) {
//    if(it.Value().ShapeType() != TopAbs_COMPOUND && it.Value().ShapeType() != TopAbs_COMPSOLID)
//      N++;
//    else {
//      N += Count(it.Value());
//    }
//  }
//  return N;
//}
//=======================================================================
static Standard_Integer Aggregation (const TopoDS_Shape& S, const TopoDS_Shape& AS, TNaming_Builder& B)
{
  Standard_Integer N(0);
  TopoDS_Iterator it(S);
  for(;it.More();it.Next()) {
    const TopoDS_Shape& sel = it.Value();
    if(sel.ShapeType() > TopAbs_COMPSOLID) {
      const TopoDS_Shape& CS = FindSubShapeInAncestor(sel, AS);
      if(!CS.IsNull()) {
	B.Select(CS, CS);
	N++;
      }
    } else 
      N += Aggregation(sel, AS, B);    
  }
  return N;
}

//==========================================================================
//function : Orientation
//purpose  : to solve  ORIENTATION name
// this function explores the second argument | arguments (Context) and 
// keeps at the label (L) the first argument (S) with the orientation it  
// has in the context. Index is used only for Seam edge recomputing 
//==========================================================================
static Standard_Boolean ORientation (const TDF_Label&                L,
				     const TDF_LabelMap&             Valid,
				     const TNaming_ListOfNamedShape& Args,
				     const Handle(TNaming_NamedShape)& Stop,
				     const Standard_Integer          Index)
{

  if(!ValidArgs(Args)) return Standard_False;

  const Handle(TNaming_NamedShape)& A = Args.First();
  TopTools_IndexedMapOfShape MS; 
  TDF_LabelMap        Forbiden;
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden);
  TNaming_NamingTool::CurrentShape (Valid,Forbiden,A,MS);

  TopoDS_Shape aShape;
  Standard_Boolean isSplit(Standard_False);
  if (!MS.IsEmpty ()) {
    if (MS.Extent() == 1) {
      aShape = MS (1);
    } 
    else {
      isSplit = Standard_True;
      aShape = MakeShape(MS);
#ifdef OCCT_DEBUG_OR
      for(Standard_Integer anItMS = 1; anItMS <= MS.Extent(); anItMS++) {
	TCollection_AsciiString aNam("OR_Selection_");
	TCollection_AsciiString aName = aNam + anItMS + ".brep";
	DbgTools_Write(MS (anItMS), aName.ToCString());
      }
#endif
    }
  }

  TNaming_Builder B(L);
  if(aShape.IsNull())
    return Standard_False;
#ifdef OCCT_DEBUG_OR
  DbgTools_Write(S, "Orientation_S.brep");
#endif

  TopTools_ListOfShape aSList;
  // tmp. solution
  if(aShape.ShapeType() == TopAbs_COMPOUND && !isSplit) {
    TopoDS_Iterator it(aShape);
    for(;it.More();it.Next())
      aSList.Append(it.Value());
  } //
 
  TopTools_IndexedMapOfShape MSC; 
  if(aSList.Extent() == 0) {
    const Handle(TNaming_NamedShape)& Anc = Args.Last();
#ifdef OCCT_DEBUG_OR
    cout << "### ORIENTATION: Ancestor ";
    PrintEntry(Anc->Label());
#endif
    MSC.Clear();
    TNaming_NamingTool::CurrentShape (Valid,Forbiden,Anc,MSC);
    if(MSC.Extent() == 1) {
      for (Standard_Integer anItMSC = 1; anItMSC <= MSC.Extent(); ++anItMSC) {
        const TopoDS_Shape& AS = MSC (anItMSC);
// <=== start 21.10.2009
	TopoDS_Shape CS;
	if(Index > 0) { //only for seam edge
	  TopoDS_Iterator itw(AS);
	  for(;itw.More();itw.Next()) {
	    Standard_Boolean found(Standard_False);
	    TopoDS_Iterator it(itw.Value());
	    for(int i=1;it.More();it.Next(),i++) {
	      if(i == Index && it.Value().IsSame(aShape)) {
		CS = it.Value();
		found = Standard_True;
#ifdef OCCT_DEBUG_OR
		cout << "ORIENTATION => ORDER = " << i <<endl;
#endif
		break;
	      }
	    }
	    if(found) break;
	  }
	} else
	  CS =  FindSubShapeInAncestor(aShape, AS);
// <=== end 21.10.2009
#ifdef OCCT_DEBUG_OR
	cout << "ORIENTATION: Selection TShape = " <<CS.TShape() <<" Orientation = " << CS.Orientation() <<endl;
	cout << "ORIENTATION: Context ShapeType = "<<AS.ShapeType() << " TShape = " <<AS.TShape() <<endl;
	DbgTools_Write(AS, "Orientation_Cnt.brep");
#endif
	if(!CS.IsNull()) {
	  B.Select(CS, CS);
	} else {
	  if(!Aggregation(aShape, AS, B))
	    return Standard_False;        
	}
      }  
    } else {
      const TopoDS_Shape  AS = MakeShape(MSC);
      const TopoDS_Shape& CS =  FindSubShapeInAncestor(aShape, AS);
      if(!CS.IsNull()) {
	B.Select(CS, CS);
      }  else {
	if(!Aggregation(aShape, AS, B)) 
	  return Standard_False;        
      }
    }
  } else {
    TNaming_ListIteratorOfListOfNamedShape it(Args); 
    it.Next(); //skip first

    // temporary solution. To be optimized (+ has connection with Union name)
    Handle(TopTools_HArray2OfShape) Arr; // Arr(1,1) - selection; Arr(1,2) - Context shape
    Arr = new TopTools_HArray2OfShape (1, aSList.Extent(), 1, 2);
    TopTools_ListIteratorOfListOfShape it1(aSList);
    Standard_Integer i = 1;
    for(; it1.More(); it1.Next(), it.Next(), i++) {
      Arr->SetValue(i, 1, it1.Value());
      MSC.Clear();
      TNaming_NamingTool::CurrentShape (Valid,Forbiden,it.Value(),MSC);
      if(MSC.Extent() == 1) {
        Arr->SetValue(i, 2, MSC (1));
      } else {
	const TopoDS_Shape  AS = MakeShape(MSC);
	Arr->SetValue(i, 2, AS);
      }
    } 
    
    if(aSList.Extent() == 1) {
      const TopoDS_Shape& S = Arr->Value(1,1);
      if(S.ShapeType() != TopAbs_COMPOUND) {	    
	const TopoDS_Shape& CS =  FindSubShapeInAncestor(S, Arr->Value(1,2));
	if(!CS.IsNull()) {
	  B.Select(CS, CS);
	} else 
	  return Standard_False;
      } 
      else {
#ifdef OCCT_DEBUG_OR
	DbgTools_Write(Arr->Value(1,2), "Ancestor.brep");
#endif
	if(!Aggregation(S, Arr->Value(1,2), B)) { 
	  return Standard_False;
	}
      }
    }
    else { // > 1   
      for(Standard_Integer k = Arr->LowerRow();k <= Arr->UpperRow();k++) {
	const TopoDS_Shape& S = Arr->Value(k,1);
	const TopoDS_Shape& AC = Arr->Value(k,2);
	if(S.ShapeType() != TopAbs_COMPOUND) {	    
	  const TopoDS_Shape& CS =  FindSubShapeInAncestor(S, AC);
	  if(!CS.IsNull()) {
	    B.Select(CS, CS);
	  } else 
	    return Standard_False;
	}
	else {
#ifdef OCCT_DEBUG_OR
	  DbgTools_Write(AC, "Aggregation.brep");
#endif
	  if(!Aggregation(S, AC, B)) { 
	    return Standard_False;
	  }
	}
      }
    }
  } // end of tmp. solution 

  return Standard_True;
}

//===========================================================================
//function : WireIN
//purpose  : to solve  WIREIN name
//=======================================================================
static Standard_Boolean WireIN(const TDF_Label&                L,
			       const TDF_LabelMap&             Valid,
			       const TNaming_ListOfNamedShape& Args, 
				   const Handle(TNaming_NamedShape)& Stop,
				   Standard_Integer Index)
{
  Standard_Boolean aResult(Standard_False);
  if(!ValidArgs(Args)) return aResult;
  TopTools_IndexedMapOfShape aMapOfSh;
  TDF_LabelMap        aForbiden;
  if (Args.Extent() < 1 ) 
    throw Standard_ConstructionError("TNaming_Name::Solve");
  const Handle(TNaming_NamedShape)& A = Args.First();
  TNaming_NamingTool::CurrentShape (Valid,aForbiden,A, aMapOfSh);
  if (aMapOfSh.Extent() != 1) return aResult;
  const TopoDS_Shape& aCF = aMapOfSh(1);
#ifdef OCCT_DEBUG_WIN
  cout <<"MS Extent = " <<MS.Extent() <<endl;
  DbgTools_Write(aCF, "Context_Face.brep");
#endif
  TNaming_Builder B(L);
  if(Index == 1 ){ //Outer wire case 
    TopoDS_Wire anOuterWire;
    TNaming::OuterWire(TopoDS::Face(aCF), anOuterWire);
	if(!anOuterWire.IsNull()) {
      B.Select(anOuterWire, anOuterWire);
	  aResult = Standard_True;
	}
  } else { //has internal wires
	TNaming_ListOfNamedShape ArgsE;
    ArgsE.Assign(Args);
    ArgsE.RemoveFirst();
	// fill Map with edges 
    TNaming_ListIteratorOfListOfNamedShape anIter(ArgsE);
    TopTools_IndexedMapOfShape MS; 
    TDF_LabelMap        Forbiden;
  
    TNaming_NamingTool::BuildDescendants (Stop, Forbiden);//fill Forbidden
    TNaming_NamingTool::CurrentShape  (Valid, Forbiden, anIter.Value(),MS); // fill MS with last modifications of the first additional argument
    TopoDS_Shape  CS = MakeShape(MS);

    TNaming_ShapesSet aSet(CS,TopAbs_EDGE);//fill internal map of shapeset by shapes of the specified type
#ifdef OCCT_DEBUG_WIN
    TCollection_AsciiString entry; 
    TDF_Tool::Entry(it.Value()->Label(), entry);
    TCollection_AsciiString Nam("Arg_");
    TCollection_AsciiString aNam = Nam + entry + "_" + "2.brep";
    DbgTools_Write(CS, aNam.ToCString());
    Standard_Integer ii = 2;
#endif
    anIter.Next();
    for (; anIter.More(); anIter.Next()) {
#ifdef OCCT_DEBUG_WIN 
      TDF_Tool::Entry(it.Value()->Label(), entry);
#endif
      MS.Clear();
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,anIter.Value(),MS);// fill MS with last modifications of the it.Value()
      CS = MakeShape(MS); 
      TNaming_ShapesSet OS(CS,TopAbs_EDGE);
      aSet.Add(OS); //concatenate both shapesets
 
#ifdef OCCT_DEBUG_WIN
      ii++;
      TCollection_AsciiString aNm = Nam + entry + "_" + ii + ".brep";
      DbgTools_Write(CS, aNm.ToCString());
      cout <<"Arg: Entry = " <<entry <<"  TShape = " << CS.TShape() <<endl;
#endif
	}

#ifdef OCCT_DEBUG_WIN
    cout <<"WIREIN:  Internal Map ext = " << aSet.Map().Extent()<<endl;
    TopTools_MapIteratorOfMapOfShape it1 (aSet.Map());
    for (int i=1;it1.More();it1.Next(),i++) {
      cout << "Map("<<i<<"): TShape = " << it1.Key().TShape() << " Orient = " << it1.Key().Orientation() <<" Type = " <<
		  it1.Key().ShapeType()<<endl;
    }
    
    TopExp_Explorer exp(aCF, TopAbs_EDGE);
    for(int i =1;exp.More();exp.Next(), i++) {
     cout << "Context_Face("<<i<<"): TShape = " << exp.Current().TShape() << " Orient = " << exp.Current().Orientation() <<endl;
    }	    
#endif
//end for edges
	
  for (TopoDS_Iterator itF(aCF); itF.More(); itF.Next()) {// find the expected wire in the face
    const TopoDS_Shape& S = itF.Value();//wire
	if(!S.IsNull()) {
#ifdef OCCT_DEBUG_WIN    
      DbgTools_Write(S, "WireIN_S.brep");
      cout <<"WIREIN: ShapeType = " << S.ShapeType() << " TS = " << S.TShape()->This() <<endl;
#endif       
      if(S.ShapeType() == TopAbs_WIRE) {
		TopTools_MapOfShape aView;
		Standard_Integer aNum(0x7FFFFFFF);
	    for (TopoDS_Iterator it(S);it.More();it.Next())
          aView.Add(it.Value());// edges of wire of the face in map

        TopTools_MapIteratorOfMapOfShape it (aSet.Map());
		aNum = aView.Extent();
		if(aNum == aSet.Map().Extent()) {
          for (;it.More();it.Next()) {
			if(aView.Contains(it.Key())) {
				aNum--;
			}
		  }
		}
		if(aNum == 0) {
		  B.Select(S, S);
	      aResult = Standard_True;
	      break;
		}
	  }
	}	
  } //

  if(!aResult) {
	TopoDS_Wire anOuterWire;
    TNaming::OuterWire(TopoDS::Face(aCF), anOuterWire);
	if(!anOuterWire.IsNull()) {
      for (TopoDS_Iterator itF(aCF); itF.More(); itF.Next()) {
        const TopoDS_Shape& S = itF.Value();//wire
	    if(!S.IsNull()&& S.ShapeType() == TopAbs_WIRE) {
		  if(S.IsEqual(anOuterWire)) continue;
		  B.Select(S, S);
		}
	  }
	}
  }
  }
  return aResult;
}
//===========================================================================
//function : ShellIN
//purpose  : to solve  SHELLIN name
//===========================================================================
static Standard_Boolean ShellIN(const TDF_Label&                L,
			       const TDF_LabelMap&             Valid,
			       const TNaming_ListOfNamedShape& Args, 
				   const Handle(TNaming_NamedShape)& Stop,
				   Standard_Integer Index)
{
  Standard_Boolean aResult(Standard_False);
  if(!ValidArgs(Args)) 
	  return aResult;
  TopTools_IndexedMapOfShape aMapOfSh; 
  TDF_LabelMap        aForbiden;
  if (Args.Extent() < 1 ) 
    throw Standard_ConstructionError("TNaming_Name::Solve");
  const Handle(TNaming_NamedShape)& A = Args.First();
  TNaming_NamingTool::CurrentShape (Valid,aForbiden,A, aMapOfSh);
  if (aMapOfSh.Extent() != 1) return aResult;
  const TopoDS_Shape& aCSO = aMapOfSh(1);
#ifdef OCCT_DEBUG_SHELL
  cout <<"MS Extent = " <<MS.Extent() <<endl;
  DbgTools_Write(aCSO, "Context_Solid.brep");
#endif
  TNaming_Builder B(L);
  if(Index == 1 ){ //Outer Shell case  
	TopoDS_Shell anOuterShell;
	TNaming::OuterShell(TopoDS::Solid(aCSO), anOuterShell);
	if(!anOuterShell.IsNull()) {
      B.Select(anOuterShell, anOuterShell);
	  aResult = Standard_True;
#ifdef OCCT_DEBUG_SHELL      
	  cout << "Outer Shell case" <<endl;
      PrintEntry(L);
	  DbgTools_Write(anOuterShell, "ShellOut_S.brep");
	  TopoDS_Iterator it (aCSO);
		for(;it.More();it.Next()){ 
          DbgTools_Write(it.Value(), "ShOut_S.brep");
		}
#endif       
	}
  } else { //has internal Shells
	TNaming_ListOfNamedShape ArgsF;
    ArgsF.Assign(Args);
    ArgsF.RemoveFirst();
	// fill Map with faces 
    TNaming_ListIteratorOfListOfNamedShape anIter(ArgsF);
    TopTools_IndexedMapOfShape MS; 
    TDF_LabelMap        Forbiden;
  
    TNaming_NamingTool::BuildDescendants (Stop, Forbiden);//fill Forbidden
    TNaming_NamingTool::CurrentShape  (Valid, Forbiden, anIter.Value(),MS); // fill MS with last modifications of the first additional argument
    TopoDS_Shape  CS = MakeShape(MS);

    TNaming_ShapesSet aSet(CS,TopAbs_FACE);//fill internal map of shapeset by shapes of the specified type
#ifdef OCCT_DEBUG_SHELL
    TCollection_AsciiString entry; 
    TDF_Tool::Entry(anIter.Value()->Label(), entry);
    TCollection_AsciiString Nam("Arg_");
    TCollection_AsciiString aNam = Nam + entry + "_" + "2.brep";
    DbgTools_Write(CS, aNam.ToCString());
    Standard_Integer ii = 2;
#endif
    anIter.Next();
    for (; anIter.More(); anIter.Next()) {
#ifdef OCCT_DEBUG_SHELL 
      TDF_Tool::Entry(anIter.Value()->Label(), entry);
#endif
      MS.Clear();
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,anIter.Value(),MS);// fill MS with last modifications of the it.Value()
      CS = MakeShape(MS); 
      TNaming_ShapesSet OS(CS,TopAbs_FACE);
      aSet.Add(OS); //concatenate both shapesets
 
#ifdef OCCT_DEBUG_SHELL
      ii++;
      TCollection_AsciiString aNm = Nam + entry + "_" + ii + ".brep";
      DbgTools_Write(CS, aNm.ToCString());
      cout <<"Arg: Entry = " <<entry <<"  TShape = " << CS.TShape() <<endl;
#endif
	}

#ifdef OCCT_DEBUG_SHELL
    cout <<"SHELLIN:  Internal Map ext = " << aSet.Map().Extent()<<endl;
    TopTools_MapIteratorOfMapOfShape it1 (aSet.Map());
    for (int i=1;it1.More();it1.Next(),i++) {
      cout << "Map("<<i<<"): TShape = " << it1.Key().TShape() << " Orient = " << it1.Key().Orientation() <<" Type = " <<
		  it1.Key().ShapeType()<<endl;
    }
    
    TopExp_Explorer exp(aCSO, TopAbs_FACE);
    for(int i = 1;exp.More();exp.Next(), i++) {
     cout << "Context_Solid("<<i<<"): TShape = " << exp.Current().TShape() << " Orient = " << exp.Current().Orientation() <<endl;
    }	    
#endif
//end for faces
	
  for (TopoDS_Iterator itS(aCSO); itS.More(); itS.Next()) {// find the expected shell in the solid
    const TopoDS_Shape& S = itS.Value();//shell
	if(!S.IsNull()) {
#ifdef OCCT_DEBUG_SHELL    
      DbgTools_Write(S, "ShellIN_S.brep");
      cout <<"SHELLIN: ShapeType = " << S.ShapeType() << " TS = " << S.TShape()->This() <<endl;
#endif       
	  if(S.ShapeType() == TopAbs_SHELL) {
		TopTools_MapOfShape aView;
		Standard_Integer aNum(0x7FFFFFFF);
	    for (TopoDS_Iterator it(S);it.More();it.Next())
          aView.Add(it.Value());// faces of shell of the solid in map
        
		aNum = aView.Extent();
		if(aNum == aSet.Map().Extent()) {
		  TopTools_MapIteratorOfMapOfShape it (aSet.Map());
          for (;it.More();it.Next()) {
			if(aView.Contains(it.Key())) {
				aNum--;
			}
		  }
		}
		if(aNum == 0) {
		  B.Select(S, S);
	      aResult = Standard_True;
	      break;
		}
	  }
	}	
  } //

  if(!aResult) {    
	TopoDS_Shell anOuterShell; 
	TNaming::OuterShell(TopoDS::Solid(aCSO), anOuterShell);
	if(!anOuterShell.IsNull()) {
      for (TopoDS_Iterator itS(aCSO); itS.More(); itS.Next()) {
        const TopoDS_Shape& S = itS.Value();//shell
	    if(!S.IsNull()&& S.ShapeType() == TopAbs_SHELL) {
		  if(S.IsEqual(anOuterShell)) continue;
		  B.Select(S, S);
		}
	  }
	}
  }
  }
  return aResult;
}
#ifdef OCCT_DEBUG
//=======================================================================
static  Standard_CString NameTypeToString (const TNaming_NameType Type)
{
  switch(Type)
    {
    case  TNaming_UNKNOWN             : return "UNKNOWN";
    case  TNaming_IDENTITY            : return "IDENTITY";
    case  TNaming_MODIFUNTIL          : return "MODIFUNTIL";
    case  TNaming_GENERATION          : return "GENERATION";
    case  TNaming_INTERSECTION        : return "INTERSECTION";
    case  TNaming_UNION               : return "UNION";
    case  TNaming_SUBSTRACTION        : return "SUBSTRACTION";
    case  TNaming_CONSTSHAPE          : return "CONSTSHAPE";
    case  TNaming_FILTERBYNEIGHBOURGS : return "FILTERBYNEIGHBOURGS";
    case  TNaming_ORIENTATION         : return "ORIENTATION";
    case  TNaming_WIREIN              : return "WIREIN";
      default :
        throw Standard_DomainError("TNaming_NameType; enum term unknown ");
    }
}
#endif
//=======================================================================
//function : Solve
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Name::Solve(const TDF_Label&    aLab,
				     const TDF_LabelMap& Valid) const
{
  Standard_Boolean Done = 0;
#ifdef OCCT_DEBUG_WIN
  PrintEntry(aLab);
#endif
  try {
  OCC_CATCH_SIGNALS
  switch (myType) {
  case TNaming_UNKNOWN :
    {
      break;
    }  
  case TNaming_IDENTITY :
    {  
      Done = Identity(aLab,Valid,myArgs,myShapeType); 
      break;
    }
  case TNaming_MODIFUNTIL:
    {
      Done = ModifUntil (aLab,Valid,myArgs,myStop);
      break;
    }
  case TNaming_GENERATION:
    {
      Done = Generated (aLab,Valid,myArgs);
      break;
    }
  case TNaming_INTERSECTION:
    {
      Done = Intersection (aLab,Valid,myArgs,myStop,myShapeType,myIndex); 
      break;
    }
  case TNaming_UNION:
    {
      Done = Union (aLab,Valid,myArgs,myStop,myShapeType, myContextLabel);
      break;
    }
  case TNaming_SUBSTRACTION: 
    {
      throw Standard_NotImplemented();
//      Done = Substraction (aLab,Valid,myArgs);
      break;
    }
  case TNaming_CONSTSHAPE:
    {
      Done = ConstShape (aLab,Valid,myArgs,myStop,myIndex);
      break;
    }
  case TNaming_FILTERBYNEIGHBOURGS:
    {
      Done = FilterByNeighbourgs (aLab,Valid,myArgs,myStop,myShapeType);
      break;
    }
  case TNaming_ORIENTATION: 
    {
      Done = ORientation (aLab,Valid,myArgs,myStop,myIndex);
      break;
    }
  case TNaming_WIREIN: 
    {
#ifdef OCCT_DEBUG_WIN  
      cout << "Name::Solve: NameType = " << myType << "  ";
  PrintEntry(aLab);
#endif
      Done = WireIN (aLab,Valid,myArgs,myStop,myIndex);
      break;
    }
case TNaming_SHELLIN: 
    {
#ifdef OCCT_DEBUG_SHELL
      cout << "Name::Solve: NameType = " << myType << "  ";
      PrintEntry(aLab);
#endif
      Done = ShellIN (aLab,Valid,myArgs,myStop,myIndex);      
      break;
    }
  }
} catch (Standard_Failure) {
#ifdef OCCT_DEBUG
  cout << "Name::Solve: EXCEPTION==> NameType = " << NameTypeToString(myType) << "  ";
  PrintEntry(aLab);
#endif
}
  return Done;
}

//=======================================================================
//function : ContextLabel
//purpose  : Set
//=======================================================================

void TNaming_Name::ContextLabel(const TDF_Label& theLabel)
{
  myContextLabel = theLabel;
}

//=======================================================================
//function : ContextLabel
//purpose  : Get
//=======================================================================

const TDF_Label&  TNaming_Name::ContextLabel() const
{ 
  return myContextLabel;
}

//=======================================================================
//function : Orientation
//purpose  : Set
//=======================================================================
void TNaming_Name::Orientation(const TopAbs_Orientation theOrientation) 
{
  myOrientation = theOrientation;
}

