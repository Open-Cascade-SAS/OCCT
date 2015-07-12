// Created on: 2000-12-08
// Created by: Michael SAZONOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <gp_Pnt.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

static TopoDS_Shape RemoveCompounds(const TopoDS_Shape& TheS)
{

  if(TheS.IsNull()) return TheS;

  TopAbs_ShapeEnum aType = TheS.ShapeType();

  if(aType != TopAbs_COMPOUND) return TheS;

  TopTools_MapOfShape aMap;
  TopoDS_Shape aResult;

  BRep_Builder aBld;
  aBld.MakeCompound(TopoDS::Compound(aResult));
  Standard_Integer n = 0;
  TopExp_Explorer anExp;

  anExp.Init(TheS, TopAbs_COMPSOLID);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  anExp.Init(TheS, TopAbs_SOLID, TopAbs_COMPSOLID);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  anExp.Init(TheS, TopAbs_SHELL, TopAbs_SOLID);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  anExp.Init(TheS, TopAbs_FACE, TopAbs_SHELL);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  anExp.Init(TheS, TopAbs_WIRE, TopAbs_FACE);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  anExp.Init(TheS, TopAbs_EDGE, TopAbs_WIRE);
  for(; anExp.More(); anExp.Next()) {
    n++;
    if(aMap.Add(anExp.Current())) aBld.Add(aResult, anExp.Current());
  }

  if(n == 1) {
    TopoDS_Iterator anIter(aResult);
    aResult = anIter.Value();
  }

  if(n == 0) aResult.Nullify();

  return aResult;
}


//=======================================================================
//function : QANewModTopOpe_Glue
//purpose  : 
//=======================================================================
QANewModTopOpe_Glue::QANewModTopOpe_Glue(const TopoDS_Shape& theS1,
			       const TopoDS_Shape& theS2,
			       const Standard_Boolean theAllowCutting,
			       const Standard_Boolean thePerformNow)
: BRepAlgoAPI_BooleanOperation (theS1,theS2, BOPAlgo_FUSE),
  myCompleted (Standard_False),
  myAllowCutting (theAllowCutting)
{
  NotDone();
  myGenerated.Clear();
  myMapModif.Clear();
  myMapGener.Clear();
  if (thePerformNow)
    Build();
}
//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void QANewModTopOpe_Glue::Build() 
{
  TopoDS_Shape& myS1=myArguments.First();
  TopoDS_Shape& myS2=myTools.First();

  if (myCompleted) return;

  TopAbs_ShapeEnum aType1, aType2;
  aType1 = myS1.ShapeType();
  aType2 = myS2.ShapeType();

  TopoDS_Shape aCopyS1 = myS1;
  TopoDS_Shape aCopyS2 = myS2;
  TopoDS_Shape aResult;

  if(aType2 == TopAbs_VERTEX) {
 
    PerformVertex();

    myCompleted = Standard_True;
    return;

  } else if (aType1 == TopAbs_VERTEX) {

    myS2 = aCopyS1;
    myS1 = aCopyS2;

    PerformVertex();

    myCompleted = Standard_True;
    myS1 = aCopyS1;
    myS2 = aCopyS2;
    return;
  }

  Standard_Boolean aContains1 = Standard_False;
  Standard_Boolean aContains2 = Standard_False;

  TopExp_Explorer anExp(myS1, TopAbs_FACE);
  aContains1 = anExp.More();
  anExp.Init(myS2, TopAbs_FACE);
  aContains2 = anExp.More();

  if(aContains1 && aContains2) {

    if(myS1.ShapeType() == TopAbs_FACE) {
      BRep_Builder aBld;
      TopoDS_Shape aCmp;
      aBld.MakeCompound(TopoDS::Compound(aCmp));
      aBld.Add(aCmp, myS1);
      myS1 =  aCmp;
    }
    if(myS2.ShapeType() == TopAbs_FACE) {
      BRep_Builder aBld;
      TopoDS_Shape aCmp;
      aBld.MakeCompound(TopoDS::Compound(aCmp));
      aBld.Add(aCmp, myS2);
      myS2 =  aCmp;
    }

    mySubst.Clear();
    PerformSDFaces();

    if(!myShape.IsNull()) {

      TopoDS_Iterator anIter(myShape);
      if(anIter.More()) {
	myS1 = anIter.Value();
	anIter.Next();
	myS2 = anIter.Value();
      }

      aResult = myShape;
      myShape.Nullify();

    }
    
    mySubst.Clear();
    PerformShell();
 
    if(!myShape.IsNull()) {

      TopoDS_Iterator anIter(myShape);
      if(anIter.More()) {
	myS1 = anIter.Value();
	anIter.Next();
	myS2 = anIter.Value();
      }

      aResult = myShape;
      myShape.Nullify();
     
    }
    
  }

  if(aContains1 || aContains2) {
    BRep_Builder aBld;
    TopoDS_Shape aS1, aS2;
    TopoDS_Shape aCpS1 = myS1, aCpS2 = myS2;
    aBld.MakeCompound(TopoDS::Compound(aS1));
    aBld.MakeCompound(TopoDS::Compound(aS2));

    anExp.Init(myS1, TopAbs_WIRE, TopAbs_FACE);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(aS1, anExp.Current());
    }
  
    anExp.Init(myS1, TopAbs_EDGE, TopAbs_WIRE);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(aS1, anExp.Current());
    }

    anExp.Init(myS2, TopAbs_WIRE, TopAbs_FACE);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(aS2, anExp.Current());
    }
  
    anExp.Init(myS2, TopAbs_EDGE, TopAbs_WIRE);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(aS2, anExp.Current());
    }

    TopoDS_Iterator anIt1(aS1);
    TopoDS_Iterator anIt2(aS2);

    Standard_Boolean aShellWire = Standard_False;

    if(anIt1.More() && anIt2.More()) {
      aShellWire = Standard_True;
      myS1 = aS1;
      myS2 = aS2;
      mySubst.Clear();

      PerformWires();

      myS1 = aCpS1;
      myS2 = aCpS2;

      if(!myShape.IsNull()) {
	aS2 = myShape;
	aS1.Nullify();
	aBld.MakeCompound(TopoDS::Compound(aS1));
      }
      else {
	for(; anIt1.More(); anIt1.Next()) aBld.Add(aS2, anIt1.Value());
	aS1.Nullify();
	aBld.MakeCompound(TopoDS::Compound(aS1));
      }
    }
    else if(anIt1.More()) {
      aShellWire = Standard_True;
      aS2 = aS1;
      aS1.Nullify();
      aBld.MakeCompound(TopoDS::Compound(aS1));
    }
    else if(anIt2.More()) {
      aShellWire = Standard_True;
    }

    if(aShellWire) {

      if(aContains1) {

	anExp.Init(myS1, TopAbs_COMPSOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

	anExp.Init(myS1, TopAbs_SOLID, TopAbs_COMPSOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

	anExp.Init(myS1, TopAbs_SHELL, TopAbs_SOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

	anExp.Init(myS1, TopAbs_FACE, TopAbs_SHELL);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

      }

      if(aContains2) {
  
	anExp.Init(myS2, TopAbs_COMPSOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}
	
	anExp.Init(myS2, TopAbs_SOLID, TopAbs_COMPSOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}
	
	anExp.Init(myS2, TopAbs_SHELL, TopAbs_SOLID);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

	anExp.Init(myS2, TopAbs_FACE, TopAbs_SHELL);
	for(; anExp.More(); anExp.Next()) {
	  aBld.Add(aS1, anExp.Current());
	}

      }
      
      myS1 = aS1;
      myS2 = aS2;

      mySubst.Clear();
      PerformShellWire();

      if(!myShape.IsNull()) {
	aResult = myShape;
      }
      
    }

  }

  if(!aContains1 && !aContains2) {

    mySubst.Clear();
    PerformWires();

    if(!myShape.IsNull()) {
      aResult = myShape;
    }

  }

  myS1 = aCopyS1;
  myS2 = aCopyS2;
  myShape = RemoveCompounds(aResult);
  if(myShape.IsNull()) NotDone();
  myCompleted = Standard_True;
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&
QANewModTopOpe_Glue::Generated (const TopoDS_Shape& theS)
{
  if (IsDone() && (myMapGener.IsBound(theS) || myMapModif.IsBound(theS))) {
    TopTools_ListIteratorOfListOfShape anItl;
    if(myMapGener.IsBound(theS)) anItl.Initialize(myMapGener(theS));
    TopTools_ListIteratorOfListOfShape anItl1;
    myGenerated.Clear();
    Standard_Boolean aNonEmpty = Standard_False;
    TopTools_ListOfShape aL1, aL;

    for(; anItl.More(); anItl.Next()) aL.Append(anItl.Value());

    TopTools_MapOfShape aMapModif;
    anItl.Initialize(Modified(theS));
    for(; anItl.More(); anItl.Next()) aMapModif.Add(anItl.Value());
    myGenerated.Clear();

    anItl.Initialize(myMapModif(theS));
    for(; anItl.More(); anItl.Next()) {
      if(!aMapModif.Contains(anItl.Value())) {
	aL.Append(anItl.Value());
      }
    }

    do 
      {

	aNonEmpty = Standard_False;
	anItl.Initialize(aL);

	for(; anItl.More(); anItl.Next()) {

	  if(myMapGener.IsBound(anItl.Value())) {
	    aNonEmpty = Standard_True;
	    anItl1.Initialize(myMapGener(anItl.Value()));
	    for(; anItl1.More(); anItl1.Next()) {
	      if(!anItl.Value().IsSame(anItl1.Value()))  aL1.Append(anItl1.Value());
	    }
	  }
	  else {
	    if(myMapModif.IsBound(anItl.Value())) {
	      aNonEmpty = Standard_True;
	      anItl1.Initialize(myMapModif(anItl.Value()));
	      for(; anItl1.More(); anItl1.Next()) {
		if(!anItl.Value().IsSame(anItl1.Value())) aL1.Append(anItl1.Value());
	      }
	    }
	    else {
	      if(!aMapModif.Contains(anItl.Value())) myGenerated.Append(anItl.Value());
	    }
	  }

	}

	if(!aL1.IsEmpty()) {
	  aL.Clear();
	  aL.Append(aL1);
	  aL1.Clear();
	}
	else aNonEmpty = Standard_False;
	    
      }
    while (aNonEmpty);

    return myGenerated;

  }


  myGenerated.Clear();
  return myGenerated;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&
QANewModTopOpe_Glue::Modified (const TopoDS_Shape& theS)
{
  if (IsDone() && myMapModif.IsBound(theS)) {
    TopTools_ListIteratorOfListOfShape anItl(myMapModif(theS));
    TopTools_ListIteratorOfListOfShape anItl1;
    myGenerated.Clear();
    Standard_Boolean aNonEmpty = Standard_False;
    TopTools_ListOfShape aL1, aL;
    for(; anItl.More(); anItl.Next()) aL.Append(anItl.Value());

    myGenerated.Clear();

    do 
      {

	aNonEmpty = Standard_False;
	anItl.Initialize(aL);

	for(; anItl.More(); anItl.Next()) {
	  if(myMapModif.IsBound(anItl.Value())) {
	    aNonEmpty = Standard_True;
	    anItl1.Initialize(myMapModif(anItl.Value()));
	    for(; anItl1.More(); anItl1.Next()) {
	      if(!anItl.Value().IsSame(anItl1.Value())) aL1.Append(anItl1.Value());
	    }
	  }
	  else {
	    myGenerated.Append(anItl.Value());
	  }
	}

	if(!aL1.IsEmpty()) {
	  aL.Clear();
	  aL.Append(aL1);
	  aL1.Clear();
	}
	else aNonEmpty = Standard_False;
	    
      }
    while (aNonEmpty);

    return myGenerated;

  }

  myGenerated.Clear();
  return myGenerated;
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::IsDeleted (const TopoDS_Shape& theS)
{
  if (IsDone() && myMapModif.IsBound(theS)) {
    const TopTools_ListOfShape &aList = myMapModif.Find(theS);

    if (aList.IsEmpty())
      return Standard_True;

    TopTools_ListIteratorOfListOfShape anIter(aList);

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aSplit = anIter.Value();

      if (!IsDeleted(aSplit))
	return Standard_False;
    }

    return Standard_True;
  }

  return Standard_False;
}
//=======================================================================
//function : HasGenerated
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::HasGenerated () const
{
  if (IsDone() && myMapGener.Extent() > 0)
    return Standard_True;
  return Standard_False;
}
//=======================================================================
//function : HasModified
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::HasModified () const
{
  
  if (IsDone() && myMapModif.Extent() > 0) {
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape anIter(myMapModif);
    for(; anIter.More(); anIter.Next()) {
      if(anIter.Value().Extent() > 0) return Standard_True;
    }
  }
  return Standard_False;
}
//=======================================================================
//function : HasDeleted
//purpose  : 
//=======================================================================

Standard_Boolean
QANewModTopOpe_Glue::HasDeleted () const
{
  if (IsDone() && myMapModif.Extent() > 0) {
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape anIter(myMapModif);
    for(; anIter.More(); anIter.Next()) {
      if(anIter.Value().Extent() == 0) return Standard_True;
    }
  }
  return Standard_False;
}
