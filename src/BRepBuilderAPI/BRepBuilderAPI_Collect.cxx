// Created on: 1996-04-09
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRepBuilderAPI_Collect.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
#include <stdio.h>
Standard_Boolean Affich;
#endif

#ifdef DRAW
#include <DBRep.hxx>
#endif
//=======================================================================
//function : BuilBack
//purpose  : 
//=======================================================================

static void BuildBack (const TopTools_DataMapOfShapeListOfShape& M1,
		             TopTools_DataMapOfShapeShape&       BM1)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it(M1);
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& KS = it.Key();
    TopTools_ListIteratorOfListOfShape itl(it.Value());
    for ( ; itl.More(); itl.Next()) {
      const TopoDS_Shape& VS = itl.Value();
      BM1.Bind(VS,KS);
    }
  }
}

//=======================================================================
//function : Replace
//purpose  : 
//=======================================================================

static void  Replace (      TopTools_ListOfShape& L,
		      const TopoDS_Shape          Old,
		      const TopTools_ListOfShape& New)
{
  //-----------------------------------
  // Suppression de Old dans la liste.
  //-----------------------------------
  TopTools_ListIteratorOfListOfShape it(L);
  while (it.More()) {
    if (it.Value().IsSame(Old)) {
      L.Remove(it);
      break;
    }
    if (it.More()) it.Next();
  }
  //---------------------------
  // Ajout de New a L.
  //---------------------------
  TopTools_ListOfShape copNew;
  copNew = New;
  L.Append(copNew);
}


//=======================================================================
//function : StoreImage
//purpose  : 
//=======================================================================

static void StoreImage (      TopTools_DataMapOfShapeListOfShape& MG,
			const TopoDS_Shape&                       S,
			const TopTools_DataMapOfShapeShape&       MGBack,
			const TopTools_ListOfShape&               LI)
{    
  if (!LI.IsEmpty()) {
    if (MGBack.IsBound(S)) {

      Replace (MG.ChangeFind(MGBack(S)),S,LI);
    }
    else {
      if (!MG.IsBound(S)) {
	TopTools_ListOfShape empty;
	MG.Bind(S,empty);
      }
      // Dans tous les cas on copie la liste pour eviter les pb de
      // const& dans BRepBuilderAPI.
      TopTools_ListIteratorOfListOfShape it;
      for (it.Initialize(LI); it.More(); it.Next()) {
	const TopoDS_Shape& SS = it.Value();
	MG(S).Append(SS);
      }
    }
  }
}

//=======================================================================
//function : UpdateGen
//purpose  : 
//=======================================================================

static void Update (   TopTools_DataMapOfShapeListOfShape& Mod,
		       TopTools_DataMapOfShapeListOfShape& Gen,
		       const TopTools_DataMapOfShapeShape& ModBack,
		       const TopTools_DataMapOfShapeShape& GenBack,
		       const TopoDS_Shape&                 SI,
		       BRepBuilderAPI_MakeShape&                  MKS,
		       const TopAbs_ShapeEnum              ShapeType)
{  
  
  TopTools_MapOfShape DejaVu;
  TopExp_Explorer     exp;
  
  for (exp.Init(SI,ShapeType); exp.More(); exp.Next()) {
    const TopoDS_Shape& S = exp.Current();
    if (!DejaVu.Add(S))  continue;

    //---------------------------------------
    // Recuperation de l image de S par MKS.
    //---------------------------------------
    const TopTools_ListOfShape& LIM = MKS.Modified(S);
    if (!LIM.IsEmpty()) {
      if (GenBack.IsBound(S)) {
	// Modif de generation => generation du shape initial
	StoreImage (Gen,S,GenBack,LIM);
      }
      else {
	StoreImage (Mod,S,ModBack,LIM);
      }
    }
    const TopTools_ListOfShape& LIG = MKS.Generated(S);
    if (!LIG.IsEmpty()) {
      if (ModBack.IsBound(S)) {
	// Generation de modif  => generation du shape initial
	TopoDS_Shape IS = ModBack(S);
	StoreImage (Gen,IS,GenBack,LIG);
      }
      else {
	StoreImage (Gen,S,GenBack,LIG);
      }
    }
  }
}
#ifdef OCCT_DEBUG
//=======================================================================
//function : DEBControl
//purpose  : 
//=======================================================================

static void DEBControl (const TopTools_DataMapOfShapeListOfShape& MG)
{
  char name[100];
  Standard_Integer IK = 0;
  
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it(MG);
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& OS = it.Key();
    sprintf(name, "SK_%d",++IK);
#ifdef DRAW
    DBRep::Set(name,OS);
#endif
    TopTools_ListIteratorOfListOfShape itl(MG(OS));
    Standard_Integer IV = 1;
    for (; itl.More(); itl.Next()) {
      sprintf(name, "SV_%d_%d",IK,IV++);
#ifdef DRAW
      DBRep::Set(name,NS);
#endif
    }
  }
}
#endif
//=======================================================================
//function : BRepBuilderAPI_Collect
//purpose  : 
//=======================================================================

BRepBuilderAPI_Collect::BRepBuilderAPI_Collect()
{}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Collect::Add (const TopoDS_Shape& SI, 
			   BRepBuilderAPI_MakeShape&  MKS)

{
  TopTools_DataMapOfShapeShape GenBack;
  TopTools_DataMapOfShapeShape ModBack;
  BuildBack (myGen, GenBack);   // Vraiment pas optimum a Revoir
  BuildBack (myMod, ModBack);

  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_COMPOUND);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_COMPSOLID);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_SOLID);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_SHELL);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_FACE);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_WIRE);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_EDGE);
  Update (myMod,myGen,ModBack,GenBack,SI,MKS,TopAbs_VERTEX);

#ifdef OCCT_DEBUG
  if (Affich) {
    DEBControl (myGen);
    DEBControl (myMod);
  }
#endif
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Collect::AddGenerated (const TopoDS_Shape& S,
				    const TopoDS_Shape& NS) 
{  
  TopTools_DataMapOfShapeShape GenBack;
  TopTools_DataMapOfShapeShape ModBack;
  BuildBack (myGen, GenBack);
  BuildBack (myMod, ModBack);

  TopTools_ListOfShape LIG;
  LIG.Append(NS);
  if (ModBack.IsBound(S)) {
    // Generation de modif  => generation du shape initial
    TopoDS_Shape IS = ModBack(S);
    StoreImage (myGen,IS,GenBack,LIG);
  }
  else {
    StoreImage (myGen,S,GenBack,LIG);
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Collect::AddModif  (const TopoDS_Shape& S, 
				 const TopoDS_Shape& NS)

{  
  TopTools_DataMapOfShapeShape GenBack;
  TopTools_DataMapOfShapeShape ModBack;
  BuildBack (myGen, GenBack);
  BuildBack (myMod, ModBack);
  
  TopTools_ListOfShape LIG;
  LIG.Append(NS);
  if (GenBack.IsBound(S)) {
    // Modif de generation => generation du shape initial
    StoreImage (myGen,S,GenBack,LIG);
  }
  else {
    StoreImage (myMod,S,ModBack,LIG);
  }
}


//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================

static void FilterByShape(TopTools_DataMapOfShapeListOfShape& MG,
			  const TopoDS_Shape&                 SF)
{
  TopTools_MapOfShape MSF;
  TopExp_Explorer     exp;
  Standard_Boolean    YaEdge   = Standard_False;
  Standard_Boolean    YaVertex = Standard_False;
  for (exp.Init(SF,TopAbs_FACE)  ; exp.More(); exp.Next()) MSF.Add(exp.Current());

  //-------------------------------------------------------------
  // Suppression de toutes les images qui ne sont pas dans MSF.
  //-------------------------------------------------------------
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape it(MG);
  for (; it.More(); it.Next()) {
    const TopoDS_Shape&   OS  = it.Key();
    TopTools_ListOfShape& LNS = MG.ChangeFind(OS); 
    TopTools_ListIteratorOfListOfShape itl(LNS);
    while (itl.More()) {
      const TopoDS_Shape& NS = itl.Value();
      //-------------------------------------------------------------------
      // Images contiennet des edges => ajout des edges resultat dans MSF.
      //-------------------------------------------------------------------
      if (!YaEdge && NS.ShapeType() == TopAbs_EDGE) {  
	for (exp.Init(SF,TopAbs_EDGE)  ; exp.More(); exp.Next()) { 
	  MSF.Add(exp.Current());
	}
	YaEdge = Standard_True;
      }
      //-------------------------------------------------------------------
      // Images contiennet des vertex => ajout des vertex resultat dans MSF.
      //-------------------------------------------------------------------
      if (!YaVertex && NS.ShapeType() == TopAbs_VERTEX) {  
	for (exp.Init(SF,TopAbs_VERTEX)  ; exp.More(); exp.Next()) { 
	  MSF.Add(exp.Current());
	}
	YaVertex = Standard_True;
      }
      //---------------------------------------
      // Si pas dans MSF suprresion de l image.
      //---------------------------------------
      if (!MSF.Contains(NS)) {
	LNS.Remove(itl);
      }
      else if (itl.More()) itl.Next();
    }
  }
#ifdef OCCT_DEBUG
  if (Affich) {
    DEBControl (MG);
  }
#endif

}

//=======================================================================
//function : Modification
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeListOfShape&   BRepBuilderAPI_Collect::Modification() const
{
  return myMod;
}

//=======================================================================
//function : Generation
//purpose  : 
//=======================================================================

const TopTools_DataMapOfShapeListOfShape&   BRepBuilderAPI_Collect::Generated() const
{
  return myGen;
}

//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================

void BRepBuilderAPI_Collect::Filter(const TopoDS_Shape& SF)
{
  FilterByShape (myGen,SF);
  FilterByShape (myMod,SF);
}






