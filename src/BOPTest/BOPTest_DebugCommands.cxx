// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>
#include <BOPTest_DrawableShape.hxx>

#include <Draw.hxx>
#include <Draw_Color.hxx>

#include <DBRep.hxx>

#include <BRep_Builder.hxx>

#include <TopoDS_Compound.hxx>

#include <TopExp_Explorer.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_MapOfCommonBlock.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_BuilderSolid.hxx>

#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>


static
  void GetTypeByName(const char* theName,
                     TopAbs_ShapeEnum& theType);
static
  void GetNameByType(const TopAbs_ShapeEnum& theType,
                     char* theName);

template <class InterfType> static
  void DumpInterfs(const BOPCol_NCVector<InterfType>& theVInterf,
                   Draw_Interpretor& di);

template <class InterfType> static
  void SearchNewIndex(const char* theCType,
                      const Standard_Integer theInd,
                      const BOPCol_NCVector<InterfType>& theVInterf,
                      Draw_Interpretor& di);
static 
  Standard_Integer bopfinfo(Draw_Interpretor& di,
                            Standard_Integer n,
                            const char** a,
                            const Standard_Integer iPriz);


// commands
// 1. filler commands
// 1.1 DS commands
static Standard_Integer bopds       (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopiterator (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopinterf   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopnews     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopwho      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopindex    (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsd       (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsc       (Draw_Interpretor&, Standard_Integer, const char**);

// 1.2 pave blocks commands
static Standard_Integer boppb       (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopcb       (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsp       (Draw_Interpretor&, Standard_Integer, const char**);

// 1.3 face info commands
static Standard_Integer bopfon      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopfin      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopfsc      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopfav      (Draw_Interpretor&, Standard_Integer, const char**);

// 2. builder commands
// 2.1 images commands
static Standard_Integer bopimage    (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer boporigin   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopfsd      (Draw_Interpretor&, Standard_Integer, const char**);

// 2.2 building faces
static Standard_Integer bopbface    (Draw_Interpretor&, Standard_Integer, const char**);
// 2.3 building solids
static Standard_Integer bopbsolid   (Draw_Interpretor&, Standard_Integer, const char**);


//=======================================================================
//function : DebugCommands
//purpose  : 
//=======================================================================
void BOPTest::DebugCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands
  theCommands.Add("bopds",
                  "Shows the shapes from DS. Use: bopds [v/e/w/f/sh/s/cs/c]",
                  __FILE__, bopds,       g);
  theCommands.Add("bopiterator", 
                  "Shows the pairs of interfered shapes. Use: bopiterator [type1 type2]",
                  __FILE__, bopiterator, g);
  theCommands.Add("bopinterf", "Shows interferences of given type. Use: bopinterf type1 type2",
                  __FILE__, bopinterf,   g);
  theCommands.Add("bopnews", "Shows the newly created shapes. Use: bopnews [v,e,f]",
                  __FILE__, bopnews,     g);
  theCommands.Add("bopwho", "Shows where the new shape was created. Use: bopwho #",
                  __FILE__, bopwho,      g);
  theCommands.Add("bopindex", "Gets the index of the shape in the DS. Use: bopindex s",
                  __FILE__, bopindex,    g);
  theCommands.Add("bopsd", "Gets the Same domain shape. Use: bopsd #",
                   __FILE__, bopsd,      g);
  theCommands.Add("bopsc", "Shows the section curves. Use: bopsc [nF1 nF2]",
                  __FILE__, bopsc,       g);
  theCommands.Add("boppb", "Shows information about pave blocks. Use: boppb [#e]",
                  __FILE__, boppb,       g);
  theCommands.Add("bopcb", "Shows information about common blocks. Use: bopcb [#e]",
                  __FILE__, bopcb,       g);
  theCommands.Add("bopsp", "Shows the splits of edges. Use: bopsp [#e]",
                  __FILE__, bopsp,       g);
  theCommands.Add("bopfon", "Shows ON information for the face. Use: bopfon #f",
                  __FILE__, bopfon,      g);
  theCommands.Add("bopfin", "Shows IN information for the face. Use: bopfin #f",
                  __FILE__, bopfin,      g);
  theCommands.Add("bopfsc", "Shows SC information for the face. Use: bopfsc #f",
                  __FILE__, bopfsc,      g);
  theCommands.Add("bopfav", "Shows information about alone vertices for the face. Use: bopfav #f",
                  __FILE__, bopfav,      g);
  theCommands.Add("bopimage", "Shows split parts of the shape. Use: bopimage s",
                  __FILE__, bopimage,    g);
  theCommands.Add("boporigin", "Shows the original shape for the shape. Use: boporigin s",
                  __FILE__, boporigin,   g);
  theCommands.Add("bopfsd", "Shows SD faces for the face: Use: bopfsd f",
                  __FILE__, bopfsd,      g);
  theCommands.Add("bopbsolid", "Build solids from set of shared faces. Use: bopbsolid sr cx",
                  __FILE__, bopbsolid,   g);
  theCommands.Add("bopbface", "Splits the face by set of shared edges. Use: bopbface fr cx",
                  __FILE__, bopbface,    g);
}

//=======================================================================
//function : bopds
//purpose  : 
//=======================================================================
Standard_Integer bopds (Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{
  if (n > 2) {
    di << "Shows the shapes from DS. Use: bopds [v/e/w/f/sh/s/cs/c]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char buf[32];
  Standard_CString aText;
  Standard_Integer i, aNbS;
  TopAbs_ShapeEnum aType, aTypeShape;
  Draw_Color aTextColor(Draw_cyan);
  //
  BOPDS_DS& aDS = *pDS;
  aNbS = aDS.NbSourceShapes();
  //
  aType = TopAbs_SHAPE;
  if (n == 2) {
    GetTypeByName(a[1], aType);
  }
  //
  for (i = 0; i < aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI = aDS.ShapeInfo(i);
    const TopoDS_Shape& aS = aSI.Shape();
    aTypeShape = aSI.ShapeType();
    if (n == 1) {
      if (aTypeShape == TopAbs_COMPOUND ||
          aTypeShape == TopAbs_COMPSOLID ||
          aTypeShape == TopAbs_SOLID ||
          aTypeShape == TopAbs_SHELL || 
          aTypeShape == TopAbs_WIRE) {
        continue;
      }
    }
    else {
      if (aTypeShape != aType) {
        continue;
      }
    }
    //
    Sprintf(buf, "z%d", i);
    aText = buf;
    Handle(BOPTest_DrawableShape) aDShape = new BOPTest_DrawableShape(aS, aText, aTextColor);
    Draw::Set(aText, aDShape);
  }
  //
  return 0;
}

//=======================================================================
//function : bopiterator
//purpose  : 
//=======================================================================
Standard_Integer bopiterator (Draw_Interpretor& di,
                              Standard_Integer n,
                              const char** a)
{
  if (n != 1 && n != 3) {
    di << "Shows the pairs of interfered shapes. Use: bopiterator [type1 type2]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  Standard_Integer n1, n2;
  char buf[64], aST1[10], aST2[10];
  BOPDS_Iterator aIt;
  //
  BOPDS_DS& aDS = *pDS;
  aIt.SetDS(&aDS);
  aIt.Prepare();
  //
  if (n == 1) {
    // type has not been defined. show all pairs
    Standard_Integer i, j;
    TopAbs_ShapeEnum aT[4] = 
      {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID};
    for (i = 0; i < 4; ++i) {
      GetNameByType(aT[i], aST1);
      //
      for (j = i; j < 4; ++j) {
        GetNameByType(aT[j], aST2);
        //
        aIt.Initialize(aT[i], aT[j]);
        for (; aIt.More(); aIt.Next()) {
          aIt.Value(n1, n2);
          //
          Sprintf(buf, "%s/%s: (z%d z%d)\n", aST1, aST2, n1, n2);
          di << buf;
        }
      }
    }
  }
  else if (n == 3) {
    TopAbs_ShapeEnum aT1, aT2;
    //
    GetTypeByName(a[1], aT1);
    GetTypeByName(a[2], aT2);
    //
    GetNameByType(aT1, aST1);
    GetNameByType(aT2, aST2);
    //
    aIt.Initialize(aT1, aT2);
    for (; aIt.More(); aIt.Next()) {
      aIt.Value(n1, n2);
      //
      Sprintf(buf, "%s/%s: (z%d z%d)\n", aST1, aST2, n1, n2);
      di << buf;
    }
  }
  //
  return 0;
}

//=======================================================================
//function : bopinterf
//purpose  : 
//=======================================================================
Standard_Integer bopinterf (Draw_Interpretor& di,
                            Standard_Integer n,
                            const char** a)
{
  if (n != 3) {
    di << "Shows interferences of given type. Use: bopinterf type1 type2\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aT1, aT2;
  GetTypeByName(a[1], aT1);
  GetTypeByName(a[2], aT2);
  //
  if (aT1 == TopAbs_VERTEX && aT2 == TopAbs_VERTEX) {
    di << "V/V: ";
    DumpInterfs<BOPDS_InterfVV>(pDS->InterfVV(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_EDGE) ||
           (aT2 == TopAbs_VERTEX && aT1 == TopAbs_EDGE)) {
    di << "V/E: ";
    DumpInterfs<BOPDS_InterfVE>(pDS->InterfVE(), di);
  }
  else if (aT1 == TopAbs_EDGE && aT2 == TopAbs_EDGE) {
    di << "E/E: ";
    DumpInterfs<BOPDS_InterfEE>(pDS->InterfEE(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_FACE) ||
           (aT2 == TopAbs_VERTEX && aT1 == TopAbs_FACE)) {
    di << "V/F: ";
    DumpInterfs<BOPDS_InterfVF>(pDS->InterfVF(), di);
  }
  else if ((aT1 == TopAbs_EDGE && aT2 == TopAbs_FACE) ||
           (aT2 == TopAbs_EDGE && aT1 == TopAbs_FACE)) {
    di << "E/F: ";
    DumpInterfs<BOPDS_InterfEF>(pDS->InterfEF(), di);
  }
  else if (aT1 == TopAbs_FACE && aT2 == TopAbs_FACE) {
    di << "F/F: ";
    DumpInterfs<BOPDS_InterfFF>(pDS->InterfFF(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_SOLID) ||
           (aT2 == TopAbs_VERTEX && aT1 == TopAbs_SOLID)) {
    di << "V/S: ";
    DumpInterfs<BOPDS_InterfVZ>(pDS->InterfVZ(), di);
  }
  else if ((aT1 == TopAbs_EDGE && aT2 == TopAbs_SOLID) ||
           (aT2 == TopAbs_EDGE && aT1 == TopAbs_SOLID)) {
    di << "E/S: ";
    DumpInterfs<BOPDS_InterfEZ>(pDS->InterfEZ(), di);
  }
  else if ((aT1 == TopAbs_FACE && aT2 == TopAbs_SOLID) ||
           (aT2 == TopAbs_FACE && aT1 == TopAbs_SOLID)) {
    di << "F/S: ";
    DumpInterfs<BOPDS_InterfFZ>(pDS->InterfFZ(), di);
  }
  else if (aT1 == TopAbs_SOLID && aT2 == TopAbs_SOLID) {
    di << "S/S: ";
    DumpInterfs<BOPDS_InterfZZ>(pDS->InterfZZ(), di);
  }
  //
  return 0;
}

//=======================================================================
//function : bopwho
//purpose  : 
//=======================================================================
Standard_Integer bopwho (Draw_Interpretor& di,
                         Standard_Integer n,
                         const char** a)
{
  if (n != 2) {
    di << "Shows where the new shape was created. Use: bopwho #\n";
    return 1;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  Standard_Integer ind = Draw::Atoi(a[1]);
  if (ind <= 0) {
    di << " Index must be grater than 0\n";
    return 1;
  }
  //
  Standard_Integer i1, i2;
  //
  i1 = 0;
  i2 = pDS->NbShapes();
  if (ind < i1 || ind > i2) {
    di << " DS does not contain the shape\n";
    return 1;
  }
  //
  if (!pDS->IsNewShape(ind)) {
    Standard_Integer iRank = pDS->Rank(ind);
    di << " Rank: " << iRank << "\n";
    return 0;
  }
  //
  // the shape is new
  di << "the shape is new\n";
  //
  const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
  if (aSI.ShapeType() != TopAbs_VERTEX) {
    return 0;
  }
  // search among interfs
  BOPDS_VectorOfInterfVV& aVVs = pDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs = pDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs = pDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs = pDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs = pDS->InterfEF();
  //
  SearchNewIndex<BOPDS_InterfVV> ("V/V: ", ind, aVVs, di);
  SearchNewIndex<BOPDS_InterfVE> ("V/E: ", ind, aVEs, di);
  SearchNewIndex<BOPDS_InterfEE> ("E/E: ", ind, aEEs, di);
  SearchNewIndex<BOPDS_InterfVF> ("V/F: ", ind, aVFs, di);
  SearchNewIndex<BOPDS_InterfEF> ("E/F: ", ind, aEFs, di);
  //
  //--------------------------------------FF
  char buf[64];
  Standard_Boolean bFound;
  Standard_Integer i, n1, n2, k, aNb, aNbC, aNbP, nV1, nV2;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  //
  bFound = Standard_False;
  BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
  aNb = aFFs.Extent();
  for (i = 0; i < aNb; ++i) {
    const BOPDS_InterfFF& anInt = aFFs(i);
    anInt.Indices(n1, n2);
    //
    const BOPDS_VectorOfCurve& aVNC = anInt.Curves();
    aNbC = aVNC.Extent();
    for (k = 0; k < aNbC; ++k) {
      const BOPDS_Curve& aNC = aVNC(k);
      const BOPDS_ListOfPaveBlock& aLPB = aNC.PaveBlocks(); 
      aItLPB.Initialize(aLPB);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        aPB->Indices(nV1, nV2);
        if (ind == nV1 || ind == nV2) {
          if (!bFound) {
            di << " FF curves: ";
            bFound = Standard_True;
          }
          Sprintf (buf,"(%d, %d) ", n1, n2);
          di << buf;
          break;
        }
      }
    }//for (k=0; k<aNbC; ++k) 
    if (bFound) {
      di << "\n";
    }
    //
    bFound = Standard_False;
    const BOPDS_VectorOfPoint& aVNP = anInt.Points();
    aNbP = aVNP.Extent();
    for (k = 0; k < aNbP; ++k) {
      const BOPDS_Point& aNP = aVNP(k);
      nV1 = aNP.Index();
      if (ind == nV1) {
        if (!bFound) {
          di << " FF points: ";
          bFound = Standard_True;
        }
        sprintf (buf,"(%d, %d) ", n1, n2);
        di << buf;
      }
    }//for (k=0; k<aNbP; ++k) 
    if (bFound) {
      di << "\n";
    }
  }
  //
  return 0;
}

//=======================================================================
//function : bopnews
//purpose  : 
//=======================================================================
Standard_Integer bopnews(Draw_Interpretor& di,
                         Standard_Integer n,
                         const char** a)
{ 
  if (n != 2) {
    di << "Shows the newly created shapes. Use: bopnews v/e/f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aType;
  GetTypeByName(a[1], aType);
  //
  if (!(aType == TopAbs_VERTEX ||
        aType == TopAbs_EDGE ||
        aType == TopAbs_FACE)) {
    di << "Use: bopnews v/e/f\n";
    return 1;
  }
  //
  char buf[32];
  Standard_CString aText;
  Standard_Boolean bFound;
  Standard_Integer i, i1, i2;
  Draw_Color aTextColor(Draw_cyan);
  Handle(BOPTest_DrawableShape) aDShape;
  //
  bFound = Standard_False;
  i1 = pDS->NbSourceShapes();
  i2 = pDS->NbShapes();
  for (i = i1; i < i2; ++i) {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(i);
    if (aSI.ShapeType() == aType) {
      const TopoDS_Shape& aS = aSI.Shape();
      //
      sprintf (buf, "z%d", i);
      aText = buf;
      aDShape = new BOPTest_DrawableShape(aS, aText, aTextColor);
      Draw::Set(aText, aDShape);
      //
      sprintf (buf, "z%d ", i);
      di << buf;
      //
      bFound = Standard_True;
    }
  }
  //
  if (bFound) {
    di << "\n";
  }
  else {
    di << " No new shapes found\n";
  }
  //
  return 0;
}

//=======================================================================
//function : bopindex
//purpose  : 
//=======================================================================
Standard_Integer bopindex (Draw_Interpretor& di,
                           Standard_Integer n,
                           const char** a)
{
  if (n != 2) {
    di << "Gets the index of the shape in the DS. Use: bopindex s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  Standard_Integer ind = pDS->Index(aS);
  Standard_Boolean bFound = (ind > 0);
  if (bFound) {
    di << " Index: " << ind << "\n";
  }
  else {
    di << " DS does not contain the shape\n";
  }
  //
  return 0;
}
  
//=======================================================================
//function : bopsd
//purpose  : 
//=======================================================================
Standard_Integer bopsd(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{ 
  if (n != 2) {
    di << "Gets the Same domain shape. Use: bopsd #\n";
    return 0;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Boolean bHasSD;
  Standard_Integer ind, i1, i2, iSD;
  //
  ind = Draw::Atoi(a[1]);
  //
  i1 = 0;
  i2 = pDS->NbShapes();
  if (ind < i1 || ind > i2) {
    di << " DS does not contain the shape\n";
    return 0;
  }
  //
  bHasSD = pDS->HasShapeSD(ind, iSD);
  if (bHasSD) {
    Sprintf(buf, " Shape %d has SD shape %d\n", ind, iSD);
    di << buf;
  }
  else {
    Sprintf (buf, " Shape: %d has no SD shape\n", ind);
    di << buf;
  }
  //
  return 0;
}

//=======================================================================
//function : bopsc
//purpose  : 
//=======================================================================
Standard_Integer bopsc(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{
  if (n != 1 && n != 3) {
    di << "Shows the section curves. Use: bopsc [nF1 nF2]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  Standard_CString aText;
  Draw_Color aTextColor(Draw_cyan);
  Standard_Integer nSF1, nSF2, nF1, nF2;
  Standard_Integer aNb, j, iCnt, k, iPriz, aNbC, aNbP, nSp;
  Standard_Integer iX;
  Handle(BOPTest_DrawableShape) aDShape;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  //
  nSF1 = nSF2 = -1;
  if (n == 3) {
    nSF1 = Draw::Atoi(a[1]);
    nSF2 = Draw::Atoi(a[2]);
  }
  //
  BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
  //
  iCnt = 0;
  iPriz = 0;
  aNb = aFFs.Extent();
  for (j = 0; j < aNb; ++j) {
    const BOPDS_InterfFF& aFF = aFFs(j);
    if (n == 3) {
      if (!aFF.Contains(nSF1) || !aFF.Contains(nSF2)) {
        continue;
      }
      iPriz = 1;
    }
    //
    aFF.Indices(nF1, nF2);
    //
    iX = 0;
    const BOPDS_VectorOfCurve& aVNC = aFF.Curves();
    aNbC = aVNC.Extent();
    for (k = 0; k < aNbC; ++k) {
      const BOPDS_Curve& aNC = aVNC(k);
      const BOPDS_ListOfPaveBlock& aLPB = aNC.PaveBlocks();
      aItLPB.Initialize(aLPB);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        if (!aPB->HasEdge(nSp)) {
          continue;
        }
        //
        if (!iX) {
          Sprintf (buf, "[%d %d] section edges: ", nF1, nF2);
          di << buf;
          iX = 1;
        }
        sprintf (buf, "t_%d_%d", k, nSp);
        di << buf;
        //
        const TopoDS_Shape& aSp = pDS->Shape(nSp);
        aText = buf;
        aDShape = new BOPTest_DrawableShape(aSp, aText, aTextColor);
        Draw::Set(aText, aDShape);
        di << " ";
        ++iCnt;
      }
    }
    if (iX) {
      di << "\n";
    }
    //
    iX = 0;
    const BOPDS_VectorOfPoint& aVNP = aFF.Points();
    aNbP = aVNP.Extent();
    for (k = 0; k < aNbP; ++k) {
      const BOPDS_Point& aNP = aVNP(k);
      nSp = aNP.Index();
      if (nSp < 0) {
        continue;
      }
      if (!iX) {
        sprintf (buf, "[%d %d] section vertices: ", nF1, nF2);
        di << buf;
        iX = 1;
      }
      sprintf (buf, "p_%d_%d", k, nSp);
      di << buf;
      //
      const TopoDS_Shape& aSp = pDS->Shape(nSp);
      aText = buf;
      aDShape = new BOPTest_DrawableShape(aSp, aText, aTextColor);
      Draw::Set(aText, aDShape);
      di << " ";
      ++iCnt;
    }
    if (iX) {
      di << "\n";
    }
    //
    if (iPriz) {
      break;
    }
  }// for (j=0; j<aNb; ++j) {
  //
  if (iCnt) {
    di << "\n";
  }
  else {
    di << " no sections found\n";
  }
  //
  return 0;
}

//=======================================================================
//function : boppb
//purpose  : 
//=======================================================================
Standard_Integer boppb(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{ 
  if (n > 2) {
    di << "Shows information about pave blocks. Use: boppb [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  Standard_Boolean bHasPaveBlocks;
  Standard_Integer ind, i1, i2;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2) {
    ind = Draw::Atoi(a[1]);
    i1 = ind;
    i2 = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind) {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType = aSI.ShapeType();
    if (aType != TopAbs_EDGE) {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks) {
      continue;
    }
    //
    const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      aPB->Dump();
      printf("\n");
    }
  }
  //
  return 0;
}

//=======================================================================
//function : bopcb
//purpose  : 
//=======================================================================
Standard_Integer bopcb(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{ 
  if (n > 2) {
    di << "Shows information about common blocks. Use: bopcb [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  Standard_Boolean bHasPaveBlocks;
  Standard_Integer ind, i1, i2;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  BOPDS_MapOfCommonBlock aMCB;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2) {
    ind = Draw::Atoi(a[1]);
    i1 = ind;
    i2 = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind) {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType = aSI.ShapeType();
    if (aType != TopAbs_EDGE) {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks) {
      continue;
    }
    //
    const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      if (pDS->IsCommonBlock(aPB)) {
        const Handle(BOPDS_CommonBlock)& aCB = pDS->CommonBlock(aPB);
        if(aMCB.Add(aCB)) {
          aCB->Dump();
          printf("\n");
        }
      }
    }
  }
  //
  return 0;
}

//=======================================================================
//function : bopsp
//purpose  : 
//=======================================================================
Standard_Integer bopsp(Draw_Interpretor& di,
                       Standard_Integer n,
                       const char** a)
{
  if (n > 2) {
    di << "Shows the splits of edges. Use: bopsp [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char buf[32];
  Standard_Boolean bHasPaveBlocks;
  Standard_Integer ind, i1, i2, nSp;
  TopAbs_ShapeEnum aType;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  Standard_CString aText;
  Draw_Color aTextColor(Draw_cyan);
  Handle(BOPTest_DrawableShape) aDShape;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2) {
    ind = Draw::Atoi(a[1]);
    i1 = ind;
    i2 = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind) {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType = aSI.ShapeType();
    if (aType != TopAbs_EDGE) {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks) {
      continue;
    }
    //
    di << "Edge " << ind << ": ";
    //
    const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
      nSp = aPB->Edge();
      const TopoDS_Shape& aSp = pDS->Shape(nSp);
      //
      Sprintf(buf, "z%d_%d", ind, nSp);
      aText = buf;
      aDShape = new BOPTest_DrawableShape(aSp, aText, aTextColor);
      Draw::Set(aText, aDShape);
      di << buf << " ";
    }
    di << "\n";
  }
  //
  return 0;
}

//=======================================================================
//function : bopfon
//purpose  : 
//=======================================================================
Standard_Integer bopfon(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{
  return bopfinfo(di, n, a, 0);
}

//=======================================================================
//function : bopfin
//purpose  : 
//=======================================================================
Standard_Integer bopfin(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{
  return bopfinfo(di, n, a, 1);
}

//=======================================================================
//function : bopfspsc
//purpose  : 
//=======================================================================
Standard_Integer bopfsc(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{
  return bopfinfo(di, n, a, 2);
}

//=======================================================================
//function : bopfinfo
//purpose  : 
//=======================================================================
Standard_Integer bopfinfo(Draw_Interpretor& di,
                          Standard_Integer n,
                          const char** a,
                          const Standard_Integer iPriz)
{
  if (n != 2) {
    di << "Shows " << ((iPriz == 0) ? "ON" : ((iPriz == 1) ? "IN" : "SC")) <<
      " information for the face. Use: bopf* #f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char aText[32];
  Standard_Integer nF, i1, i2, nV, i, aNb;
  Handle(BOPDS_PaveBlock) aPB;
  //
  nF = Draw::Atoi(a[1]);
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (nF < i1 || nF > i2) {
    di << " DS does not contain the shape\n";
    return 1;
  }
  //
  if (pDS->ShapeInfo(nF).ShapeType() != TopAbs_FACE) {
    di << " The shape is not a face\n";
    return 1;
  }
  //
  if (!pDS->HasFaceInfo(nF)) {
    di << " The face has no face information\n";
    return 0;
  }
  //
  BOPDS_FaceInfo& aFI = pDS->ChangeFaceInfo(nF);
  //
  BOPDS_IndexedMapOfPaveBlock aMPB;
  BOPCol_MapOfInteger aMI;
  if (iPriz == 0) {
    strcpy(aText, "On");
    aMPB = aFI.ChangePaveBlocksOn();
    aMI = aFI.ChangeVerticesOn();
  }
  else if (iPriz == 1) {
    strcpy(aText, "In");
    aMPB = aFI.ChangePaveBlocksIn();
    aMI = aFI.ChangeVerticesIn();
  }
  else if (iPriz == 2) {
    strcpy(aText, "Sc");
    aMPB = aFI.ChangePaveBlocksSc();
    aMI = aFI.ChangeVerticesSc();
  }
  //
  if (aMPB.Extent()) {
    printf(" pave blocks %s:\n", aText);
    aNb = aMPB.Extent();
    for (i = 1; i <= aNb; ++i) {
      aPB = aMPB(i);
      aPB->Dump();
      printf(" \n" );
    }
  }
  else {
    printf(" no pave blocks %s found\n", aText);
  }
  //
  if (aMI.Extent()) {
    printf(" vertices %s:\n", aText);
    BOPCol_MapIteratorOfMapOfInteger aItMI(aMI);
    for (; aItMI.More(); aItMI.Next()) {
      nV = aItMI.Value();
      printf(" %d", nV);
    }
    printf(" \n" );
  }
  else {
    printf(" no verts %s found\n", aText);
  }
  //
  return 0;
}

//=======================================================================
//function : bopfav
//purpose  : alone vertices on face
//=======================================================================
Standard_Integer bopfav(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{
  if (n != 2) {
    di << "Shows information about alone vertices for the face. Use: bopfav #f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  Standard_Integer i1, i2, nF, nV;
  //
  nF = Draw::Atoi(a[1]);
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (nF < i1 || nF > i2) {
    di << "DS does not contain the shape\n";
    return 1;
  }
  //
  if (pDS->ShapeInfo(nF).ShapeType() != TopAbs_FACE) {
    di << " The shape is not a face\n";
    return 1;
  }
  //
  if (!pDS->HasFaceInfo(nF)) {
    di << " The face has no face information\n";
    return 0;
  }
  //
  BOPCol_ListOfInteger aLI;
  pDS->AloneVertices(nF, aLI);
  if (!aLI.Extent()) {
    di << " no alone vertices found\n";
    return 0;
  }
  //
  di << " alone vertices: \n";
  BOPCol_ListIteratorOfListOfInteger aItLI(aLI);
  for (; aItLI.More(); aItLI.Next()) {
    nV = aItLI.Value();
    di << nV << " ";
  }
  di <<"\n";
  //
  return 0;
}

//=======================================================================
//function : bopimage
//purpose  : 
//=======================================================================
Standard_Integer bopimage(Draw_Interpretor& di,
                          Standard_Integer n,
                          const char** a)
{ 
  if (n != 2) {
    di << "Shows split parts of the shape. Use: bopimage s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const BOPCol_DataMapOfShapeListOfShape& anImages = aBuilder.Images();
  if (!anImages.IsBound(aS)) {
    di << " no images found\n"; 
    return 0;
  }
  //
  char buf[32];
  Standard_Integer i;
  BRep_Builder aBB;
  TopoDS_Compound aC;
  //
  aBB.MakeCompound(aC);
  //
  const BOPCol_ListOfShape& aLSIm = anImages.Find(aS);
  BOPCol_ListIteratorOfListOfShape aIt(aLSIm);
  for (i = 0; aIt.More(); aIt.Next(), ++i) {
    const TopoDS_Shape& aSIm = aIt.Value();
    aBB.Add(aC, aSIm);
  }
  //
  di << i << " images found\n";
  sprintf(buf, "%s_im", a[1]);
  DBRep::Set(buf, aC);
  di << buf << "\n";
  //
  return 0;
}

//=======================================================================
//function : boporigin
//purpose  : 
//=======================================================================
Standard_Integer boporigin(Draw_Interpretor& di,
                           Standard_Integer n,
                           const char** a)
{ 
  if (n != 2) {
    di << "Shows the original shape for the shape. Use: boporigin s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 0;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const BOPCol_DataMapOfShapeListOfShape& aDMI = aBuilder.Origins();
  if (!aDMI.IsBound(aS)) {
    di << " no origins found\n"; 
    return 0;
  }
  //
  char buf[32];
  sprintf(buf, "%s_or", a[1]);
  //
  const BOPCol_ListOfShape& aLSx = aDMI.Find(aS);
  if (aLSx.Extent() == 1) {
    DBRep::Set(buf, aLSx.First());
    di << "1 origin found\n" << buf << "\n";
    return 0;
  }
  //
  TopoDS_Compound aCOr;
  BRep_Builder().MakeCompound(aCOr);
  //
  BOPCol_ListIteratorOfListOfShape aItLSx(aLSx);
  for (; aItLSx.More(); aItLSx.Next()) {
    BRep_Builder().Add(aCOr, aItLSx.Value());
  }
  //
  DBRep::Set(buf, aCOr);
  //
  di << aLSx.Extent() << " origins found\n";
  di << buf << "\n";
  //
  return 0;
}

//=======================================================================
//function : bopfsd
//purpose  : 
//=======================================================================
Standard_Integer bopfsd(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{ 
  if (n != 2) {
    di << "Shows SD faces for the face: Use: bopfsd f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const BOPCol_DataMapOfShapeShape& aDMSD = aBuilder.ShapesSD();
  if (!aDMSD.IsBound(aS)) {
    di << " shape has no sd shape\n"; 
    return 0;
  }
  //
  char buf[32];
  Standard_Integer i;
  BRep_Builder aBB;
  TopoDS_Compound aC;
  //
  aBB.MakeCompound(aC);
  //
  BOPCol_DataMapIteratorOfDataMapOfShapeShape aItSD;
  aItSD.Initialize(aDMSD);
  for (i = 0; aItSD.More(); aItSD.Next()) {
    const TopoDS_Shape& aSK = aItSD.Key();
    const TopoDS_Shape& aSV = aItSD.Value();
    if (aSK.IsEqual(aS)) {
      if (!aSV.IsEqual(aS)) {
        aBB.Add(aC, aS);
        ++i;
      }
    }
    //
    else if (aSV.IsEqual(aS)) {
      if (!aSK.IsEqual(aS)) {
        aBB.Add(aC, aS);
        ++i;
      }
    }
  }
  //
  di << i << " SD shapes found\n";
  //
  sprintf(buf, "%s_sd", a[1]);
  DBRep::Set(buf, aC);
  //
  di << buf << "\n";
  return 0;
}

//=======================================================================
//function : bopbface
//purpose  : 
//=======================================================================
Standard_Integer bopbface (Draw_Interpretor& di,
                           Standard_Integer n,
                           const char** a)
{
  if ( n!= 3) {
    di << "Build faces from surface and set of shared edges. Use: bopbface fr cx\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  TopoDS_Face aF;
  BOPCol_ListOfShape aLE;
  Standard_Integer i;
  //
  TopoDS_Iterator aItS(aS);
  for (i=0; aItS.More(); aItS.Next(), ++i) {
    const TopoDS_Shape& aSx = aItS.Value();
    if (!i) {
      if (aSx.ShapeType() != TopAbs_FACE) {
        di << " shape " << i << " is not a face\n";
        return 1;
      }
      aF = *(TopoDS_Face*)&aSx;
    }
    else {
      if (aSx.ShapeType() != TopAbs_EDGE) {
        di << " shape " << i << " is not an edge\n";
        return 1;
      }
      aLE.Append(aSx);
    }
  }
  //
  BOPAlgo_BuilderFace aBF;
  aBF.SetFace(aF);
  aBF.SetShapes(aLE);
  aBF.Perform();
  BOPTest::ReportAlerts(aBF);
  if (aBF.HasErrors()) {
    return 0;
  }
  //
  char buf[128];
  const BOPCol_ListOfShape& aLFR = aBF.Areas();
  BOPCol_ListIteratorOfListOfShape aIt(aLFR);
  for (i = 1; aIt.More(); aIt.Next(), ++i) {
    const TopoDS_Shape& aFR = aIt.Value();
    sprintf(buf, "%s_%d", a[1], i);
    DBRep::Set(buf, aFR);
    di << " " << buf;
  }
  //
  i = aLFR.Extent();
  if (i) {
    di << "\n " << i << " faces were built\n";
  }
  else {
    di << " No faces were built\n";
  }
  //
  return 0;
}

//=======================================================================
//function : bopbsolid
//purpose  : 
//=======================================================================
Standard_Integer bopbsolid (Draw_Interpretor& di,
                            Standard_Integer n,
                            const char** a)
{
  if ( n!= 3) {
    di << "Build solids from set of shared faces. Use: bopbsolid sr cx\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull()) {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  BOPCol_ListOfShape aLF;
  TopExp_Explorer aExp(aS, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aF = aExp.Current();
    aLF.Append(aF);
  }
  //
  if (aLF.IsEmpty()) {
    di << " No faces to build solids\n";
    return 1;
  }
  //
  BOPAlgo_BuilderSolid aBS;
  aBS.SetShapes(aLF);
  aBS.Perform();
  BOPTest::ReportAlerts(aBS);
  if (aBS.HasErrors()) {
    return 0;
  }
  //
  Standard_Integer i;
  TopoDS_Compound aSolids;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aSolids);
  //
  char buf[128];
  const BOPCol_ListOfShape& aLSR = aBS.Areas();
  BOPCol_ListIteratorOfListOfShape aIt(aLSR);
  for (i = 1; aIt.More(); aIt.Next(), ++i) {
    const TopoDS_Shape& aSR = aIt.Value();
    sprintf(buf, "%s_%d", a[1], i);
    DBRep::Set(buf, aSR);
    di << " " << buf;
  }
  //
  i = aLSR.Extent();
  if (i) {
    di << "\n " << i << " solids were built\n";
  }
  else {
    di << " No solids were built\n";
  }
  //
  return 0;
}

//=======================================================================
//function : GetTypeByName
//purpose  : 
//=======================================================================
void GetTypeByName(const char* theName,
                   TopAbs_ShapeEnum& theType)
{
  if (!strcmp (theName, "v") ||
      !strcmp (theName, "V")) {
    theType = TopAbs_VERTEX;
  }
  else if (!strcmp (theName, "e") ||
           !strcmp (theName, "E")) {
    theType = TopAbs_EDGE;
  }
  else if (!strcmp (theName, "w") ||
           !strcmp (theName, "W")) {
    theType = TopAbs_WIRE;
  }
  else if (!strcmp (theName, "f") ||
           !strcmp (theName, "F")) {
    theType = TopAbs_FACE;
  }
  else if (!strcmp (theName, "sh") ||
           !strcmp (theName, "Sh") ||
           !strcmp (theName, "SH")) {
    theType = TopAbs_SHELL;
  }
  else if (!strcmp (theName, "s") ||
           !strcmp (theName, "S")) {
    theType = TopAbs_SOLID;
  }
  else if (!strcmp (theName, "cs") ||
           !strcmp (theName, "Cs") ||
           !strcmp (theName, "CS")) {
    theType = TopAbs_COMPSOLID;
  }
  else if (!strcmp (theName, "c") ||
           !strcmp (theName, "C")) {
    theType = TopAbs_COMPOUND;
  }
  else {
    theType = TopAbs_SHAPE;
  }
}

//=======================================================================
//function : GetNameByType
//purpose  : 
//=======================================================================
void GetNameByType(const TopAbs_ShapeEnum& theType,
                   char* theName)
{
  switch (theType) {
  case TopAbs_VERTEX:
    strcpy (theName, "V");
    break;
  case TopAbs_EDGE:
    strcpy (theName, "E");
    break; 
  case TopAbs_WIRE:
    strcpy (theName, "w");
    break;
  case TopAbs_FACE:
    strcpy (theName, "F");
    break;
  case TopAbs_SHELL:
    strcpy (theName, "Sh");
    break; 
  case TopAbs_SOLID:
    strcpy (theName, "S");
    break; 
  case TopAbs_COMPSOLID:
    strcpy (theName, "Cs");
    break; 
  case TopAbs_COMPOUND:
    strcpy (theName, "c");
    break; 
  default:
    strcpy (theName, "Shape");
    break; 
  }
}

//=======================================================================
//function : DumpInterfs
//purpose  : 
//=======================================================================
template <class InterfType> void DumpInterfs
  (const BOPCol_NCVector<InterfType>& theVInterf,
   Draw_Interpretor& di)
{
  Standard_Integer i, aNb, n1, n2, nNew;
  char buf[64];
  //
  aNb = theVInterf.Extent();
  if (aNb == 0) {
    di << "Not found\n";
    return;
  }
  //
  di << aNb << " interference(s) found\n";
  for (i = 0; i < aNb; ++i) {
    const InterfType& anInt = theVInterf(i);
    anInt.Indices(n1, n2);
    if (anInt.HasIndexNew()) {
      nNew = anInt.IndexNew();
      Sprintf(buf, " (%d, %d, %d)\n", n1, n2, nNew);
    }
    else {
      Sprintf(buf, " (%d, %d)\n", n1, n2);
    }
    di << buf;
  }
}

//=======================================================================
//function : SearchNewIndex
//purpose  : 
//=======================================================================
template <class InterfType> void SearchNewIndex
  (const char* theCType,
   const Standard_Integer theInd,
   const BOPCol_NCVector<InterfType>& theVInterf,
   Draw_Interpretor& di)
{
  char buf[64];
  Standard_Boolean bFound;
  Standard_Integer i, aNb, n1, n2, nNew;
  //
  bFound = Standard_False;
  aNb = theVInterf.Extent();
  for (i = 0 ; i < aNb; ++i) {
    const InterfType& anInt = theVInterf(i);
    nNew = anInt.IndexNew();
    if (theInd == nNew) {
      if (!bFound) {
        di << theCType;
        bFound = Standard_True;
      }
      //
      anInt.Indices(n1, n2);
      sprintf(buf,"(%d, %d) ", n1, n2);
      di << buf;
    }
  }
  if (bFound) {
    di << "\n";
  }
}
