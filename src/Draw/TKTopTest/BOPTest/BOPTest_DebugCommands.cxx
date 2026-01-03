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
#include <NCollection_Map.hxx>
#include <BOPDS_CommonBlock.hxx>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_BuilderSolid.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>

#include <IntTools_Context.hxx>

static void GetTypeByName(const char* theName, TopAbs_ShapeEnum& theType);
static void GetNameByType(const TopAbs_ShapeEnum& theType, char* theName);

template <class InterfType>
static void DumpInterfs(const NCollection_Vector<InterfType>& theVInterf, Draw_Interpretor& di);

template <class InterfType>
static void SearchNewIndex(const char*                           theCType,
                           const int                             theInd,
                           const NCollection_Vector<InterfType>& theVInterf,
                           Draw_Interpretor&                     di);
static int  bopfinfo(Draw_Interpretor& di, int n, const char** a, const int iPriz);

// commands
// 1. filler commands
// 1.1 DS commands
static int bopds(Draw_Interpretor&, int, const char**);
static int bopiterator(Draw_Interpretor&, int, const char**);
static int bopinterf(Draw_Interpretor&, int, const char**);
static int bopnews(Draw_Interpretor&, int, const char**);
static int bopwho(Draw_Interpretor&, int, const char**);
static int bopindex(Draw_Interpretor&, int, const char**);
static int bopsd(Draw_Interpretor&, int, const char**);
static int bopsc(Draw_Interpretor&, int, const char**);

// 1.2 pave blocks commands
static int boppb(Draw_Interpretor&, int, const char**);
static int bopcb(Draw_Interpretor&, int, const char**);
static int bopsp(Draw_Interpretor&, int, const char**);

// 1.3 face info commands
static int bopfon(Draw_Interpretor&, int, const char**);
static int bopfin(Draw_Interpretor&, int, const char**);
static int bopfsc(Draw_Interpretor&, int, const char**);
static int bopfav(Draw_Interpretor&, int, const char**);

// 2. builder commands
// 2.1 images commands
static int bopimage(Draw_Interpretor&, int, const char**);
static int boporigin(Draw_Interpretor&, int, const char**);
static int bopfsd(Draw_Interpretor&, int, const char**);

// 2.2 building faces
static int bopbface(Draw_Interpretor&, int, const char**);
// 2.3 building solids
static int bopbsolid(Draw_Interpretor&, int, const char**);

//=================================================================================================

void BOPTest::DebugCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands
  theCommands.Add("bopds",
                  "Shows the shapes from DS. Use: bopds [v/e/w/f/sh/s/cs/c]",
                  __FILE__,
                  bopds,
                  g);
  theCommands.Add("bopiterator",
                  "Shows the pairs of interfered shapes. Use: bopiterator [type1 type2]",
                  __FILE__,
                  bopiterator,
                  g);
  theCommands.Add("bopinterf",
                  "Shows interferences of given type. Use: bopinterf type1 type2",
                  __FILE__,
                  bopinterf,
                  g);
  theCommands.Add("bopnews",
                  "Shows the newly created shapes. Use: bopnews [v,e,f]",
                  __FILE__,
                  bopnews,
                  g);
  theCommands.Add("bopwho",
                  "Shows where the new shape was created. Use: bopwho #",
                  __FILE__,
                  bopwho,
                  g);
  theCommands.Add("bopindex",
                  "Gets the index of the shape in the DS. Use: bopindex s",
                  __FILE__,
                  bopindex,
                  g);
  theCommands.Add("bopsd", "Gets the Same domain shape. Use: bopsd #", __FILE__, bopsd, g);
  theCommands.Add("bopsc", "Shows the section curves. Use: bopsc [nF1 [nF2]]", __FILE__, bopsc, g);
  theCommands.Add("boppb",
                  "Shows information about pave blocks. Use: boppb [#e]",
                  __FILE__,
                  boppb,
                  g);
  theCommands.Add("bopcb",
                  "Shows information about common blocks. Use: bopcb [#e]",
                  __FILE__,
                  bopcb,
                  g);
  theCommands.Add("bopsp", "Shows the splits of edges. Use: bopsp [#e]", __FILE__, bopsp, g);
  theCommands.Add("bopfon",
                  "Shows ON information for the face. Use: bopfon #f",
                  __FILE__,
                  bopfon,
                  g);
  theCommands.Add("bopfin",
                  "Shows IN information for the face. Use: bopfin #f",
                  __FILE__,
                  bopfin,
                  g);
  theCommands.Add("bopfsc",
                  "Shows SC information for the face. Use: bopfsc #f",
                  __FILE__,
                  bopfsc,
                  g);
  theCommands.Add("bopfav",
                  "Shows information about alone vertices for the face. Use: bopfav #f",
                  __FILE__,
                  bopfav,
                  g);
  theCommands.Add("bopimage",
                  "Shows split parts of the shape. Use: bopimage s",
                  __FILE__,
                  bopimage,
                  g);
  theCommands.Add("boporigin",
                  "Shows the original shape for the shape. Use: boporigin s",
                  __FILE__,
                  boporigin,
                  g);
  theCommands.Add("bopfsd", "Shows SD faces for the face: Use: bopfsd f", __FILE__, bopfsd, g);
  theCommands.Add("bopbsolid",
                  "Build solids from set of shared faces. Use: bopbsolid sr cx",
                  __FILE__,
                  bopbsolid,
                  g);
  theCommands.Add("bopbface",
                  "Splits the face by set of shared edges. Use: bopbface fr cx",
                  __FILE__,
                  bopbface,
                  g);
}

//=================================================================================================

int bopds(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 2)
  {
    di << "Shows the shapes from DS. Use: bopds [v/e/w/f/sh/s/cs/c]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char             buf[32];
  const char*      aText;
  int              i, aNbS;
  TopAbs_ShapeEnum aType, aTypeShape;
  Draw_Color       aTextColor(Draw_cyan);
  //
  BOPDS_DS& aDS = *pDS;
  aNbS          = aDS.NbSourceShapes();
  //
  aType = TopAbs_SHAPE;
  if (n == 2)
  {
    GetTypeByName(a[1], aType);
  }
  //
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = aDS.ShapeInfo(i);
    const TopoDS_Shape&    aS  = aSI.Shape();
    aTypeShape                 = aSI.ShapeType();
    if (n == 1)
    {
      if (aTypeShape == TopAbs_COMPOUND || aTypeShape == TopAbs_COMPSOLID
          || aTypeShape == TopAbs_SOLID || aTypeShape == TopAbs_SHELL || aTypeShape == TopAbs_WIRE)
      {
        continue;
      }
    }
    else
    {
      if (aTypeShape != aType)
      {
        continue;
      }
    }
    //
    Sprintf(buf, "z%d", i);
    aText                                      = buf;
    occ::handle<BOPTest_DrawableShape> aDShape = new BOPTest_DrawableShape(aS, aText, aTextColor);
    Draw::Set(aText, aDShape);
  }
  //
  return 0;
}

//=================================================================================================

int bopiterator(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 1 && n != 3)
  {
    di << "Shows the pairs of interfered shapes. Use: bopiterator [type1 type2]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  int            n1, n2;
  char           buf[64], aST1[10], aST2[10];
  BOPDS_Iterator aIt;
  //
  occ::handle<IntTools_Context> aCtx = new IntTools_Context();

  BOPDS_DS& aDS = *pDS;
  aIt.SetDS(&aDS);
  aIt.Prepare(aCtx, BOPTest_Objects::UseOBB(), BOPTest_Objects::FuzzyValue());
  //
  if (n == 1)
  {
    // type has not been defined. show all pairs
    int              i, j;
    TopAbs_ShapeEnum aT[4] = {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID};
    for (i = 0; i < 4; ++i)
    {
      GetNameByType(aT[i], aST1);
      //
      for (j = i; j < 4; ++j)
      {
        GetNameByType(aT[j], aST2);
        //
        aIt.Initialize(aT[i], aT[j]);
        for (; aIt.More(); aIt.Next())
        {
          aIt.Value(n1, n2);
          //
          Sprintf(buf, "%s/%s: (z%d z%d)\n", aST1, aST2, n1, n2);
          di << buf;
        }
      }
    }
  }
  else if (n == 3)
  {
    TopAbs_ShapeEnum aT1, aT2;
    //
    GetTypeByName(a[1], aT1);
    GetTypeByName(a[2], aT2);
    //
    GetNameByType(aT1, aST1);
    GetNameByType(aT2, aST2);
    //
    aIt.Initialize(aT1, aT2);
    for (; aIt.More(); aIt.Next())
    {
      aIt.Value(n1, n2);
      //
      Sprintf(buf, "%s/%s: (z%d z%d)\n", aST1, aST2, n1, n2);
      di << buf;
    }
  }
  //
  return 0;
}

//=================================================================================================

int bopinterf(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Shows interferences of given type. Use: bopinterf type1 type2\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aT1, aT2;
  GetTypeByName(a[1], aT1);
  GetTypeByName(a[2], aT2);
  //
  if (aT1 == TopAbs_VERTEX && aT2 == TopAbs_VERTEX)
  {
    di << "V/V: ";
    DumpInterfs<BOPDS_InterfVV>(pDS->InterfVV(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_EDGE)
           || (aT2 == TopAbs_VERTEX && aT1 == TopAbs_EDGE))
  {
    di << "V/E: ";
    DumpInterfs<BOPDS_InterfVE>(pDS->InterfVE(), di);
  }
  else if (aT1 == TopAbs_EDGE && aT2 == TopAbs_EDGE)
  {
    di << "E/E: ";
    DumpInterfs<BOPDS_InterfEE>(pDS->InterfEE(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_FACE)
           || (aT2 == TopAbs_VERTEX && aT1 == TopAbs_FACE))
  {
    di << "V/F: ";
    DumpInterfs<BOPDS_InterfVF>(pDS->InterfVF(), di);
  }
  else if ((aT1 == TopAbs_EDGE && aT2 == TopAbs_FACE) || (aT2 == TopAbs_EDGE && aT1 == TopAbs_FACE))
  {
    di << "E/F: ";
    DumpInterfs<BOPDS_InterfEF>(pDS->InterfEF(), di);
  }
  else if (aT1 == TopAbs_FACE && aT2 == TopAbs_FACE)
  {
    di << "F/F: ";
    DumpInterfs<BOPDS_InterfFF>(pDS->InterfFF(), di);
  }
  else if ((aT1 == TopAbs_VERTEX && aT2 == TopAbs_SOLID)
           || (aT2 == TopAbs_VERTEX && aT1 == TopAbs_SOLID))
  {
    di << "V/S: ";
    DumpInterfs<BOPDS_InterfVZ>(pDS->InterfVZ(), di);
  }
  else if ((aT1 == TopAbs_EDGE && aT2 == TopAbs_SOLID)
           || (aT2 == TopAbs_EDGE && aT1 == TopAbs_SOLID))
  {
    di << "E/S: ";
    DumpInterfs<BOPDS_InterfEZ>(pDS->InterfEZ(), di);
  }
  else if ((aT1 == TopAbs_FACE && aT2 == TopAbs_SOLID)
           || (aT2 == TopAbs_FACE && aT1 == TopAbs_SOLID))
  {
    di << "F/S: ";
    DumpInterfs<BOPDS_InterfFZ>(pDS->InterfFZ(), di);
  }
  else if (aT1 == TopAbs_SOLID && aT2 == TopAbs_SOLID)
  {
    di << "S/S: ";
    DumpInterfs<BOPDS_InterfZZ>(pDS->InterfZZ(), di);
  }
  //
  return 0;
}

//=================================================================================================

int bopwho(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows where the new shape was created. Use: bopwho #\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  int ind = Draw::Atoi(a[1]);
  if (ind <= 0)
  {
    di << " Index must be grater than 0\n";
    return 1;
  }
  //
  int i1, i2;
  //
  i1 = 0;
  i2 = pDS->NbShapes();
  if (ind < i1 || ind > i2)
  {
    di << " DS does not contain the shape\n";
    return 1;
  }
  //
  if (!pDS->IsNewShape(ind))
  {
    int iRank = pDS->Rank(ind);
    di << " Rank: " << iRank << "\n";
    return 0;
  }
  //
  // the shape is new
  di << "the shape is new\n";
  //
  const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
  if (aSI.ShapeType() != TopAbs_VERTEX)
  {
    return 0;
  }
  // search among interfs
  NCollection_Vector<BOPDS_InterfVV>& aVVs = pDS->InterfVV();
  NCollection_Vector<BOPDS_InterfVE>& aVEs = pDS->InterfVE();
  NCollection_Vector<BOPDS_InterfEE>& aEEs = pDS->InterfEE();
  NCollection_Vector<BOPDS_InterfVF>& aVFs = pDS->InterfVF();
  NCollection_Vector<BOPDS_InterfEF>& aEFs = pDS->InterfEF();
  //
  SearchNewIndex<BOPDS_InterfVV>("V/V: ", ind, aVVs, di);
  SearchNewIndex<BOPDS_InterfVE>("V/E: ", ind, aVEs, di);
  SearchNewIndex<BOPDS_InterfEE>("E/E: ", ind, aEEs, di);
  SearchNewIndex<BOPDS_InterfVF>("V/F: ", ind, aVFs, di);
  SearchNewIndex<BOPDS_InterfEF>("E/F: ", ind, aEFs, di);
  //
  //--------------------------------------FF
  char                                                     buf[64];
  bool                                                     bFound;
  int                                                      i, n1, n2, k, aNb, aNbC, aNbP, nV1, nV2;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB;
  //
  bFound                                   = false;
  NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  aNb                                      = aFFs.Length();
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_InterfFF& anInt = aFFs(i);
    anInt.Indices(n1, n2);
    //
    const NCollection_Vector<BOPDS_Curve>& aVNC = anInt.Curves();
    aNbC                                        = aVNC.Length();
    for (k = 0; k < aNbC; ++k)
    {
      const BOPDS_Curve&                                    aNC  = aVNC(k);
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aNC.PaveBlocks();
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
        aPB->Indices(nV1, nV2);
        if (ind == nV1 || ind == nV2)
        {
          if (!bFound)
          {
            di << " FF curves: ";
            bFound = true;
          }
          Sprintf(buf, "(%d, %d) ", n1, n2);
          di << buf;
          break;
        }
      }
    } // for (k=0; k<aNbC; ++k)
    if (bFound)
    {
      di << "\n";
    }
    //
    bFound                                      = false;
    const NCollection_Vector<BOPDS_Point>& aVNP = anInt.Points();
    aNbP                                        = aVNP.Length();
    for (k = 0; k < aNbP; ++k)
    {
      const BOPDS_Point& aNP = aVNP(k);
      nV1                    = aNP.Index();
      if (ind == nV1)
      {
        if (!bFound)
        {
          di << " FF points: ";
          bFound = true;
        }
        Sprintf(buf, "(%d, %d) ", n1, n2);
        di << buf;
      }
    } // for (k=0; k<aNbP; ++k)
    if (bFound)
    {
      di << "\n";
    }
  }
  //
  return 0;
}

//=================================================================================================

int bopnews(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows the newly created shapes. Use: bopnews v/e/f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aType;
  GetTypeByName(a[1], aType);
  //
  if (aType != TopAbs_VERTEX && aType != TopAbs_EDGE && aType != TopAbs_FACE)
  {
    di << "Use: bopnews v/e/f\n";
    return 1;
  }
  //
  char                               buf[32];
  const char*                        aText;
  bool                               bFound;
  int                                i, i1, i2;
  Draw_Color                         aTextColor(Draw_cyan);
  occ::handle<BOPTest_DrawableShape> aDShape;
  //
  bFound = false;
  i1     = pDS->NbSourceShapes();
  i2     = pDS->NbShapes();
  for (i = i1; i < i2; ++i)
  {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(i);
    if (aSI.ShapeType() == aType)
    {
      const TopoDS_Shape& aS = aSI.Shape();
      //
      Sprintf(buf, "z%d", i);
      aText   = buf;
      aDShape = new BOPTest_DrawableShape(aS, aText, aTextColor);
      Draw::Set(aText, aDShape);
      //
      Sprintf(buf, "z%d ", i);
      di << buf;
      //
      bFound = true;
    }
  }
  //
  if (bFound)
  {
    di << "\n";
  }
  else
  {
    di << " No new shapes found\n";
  }
  //
  return 0;
}

//=================================================================================================

int bopindex(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Gets the index of the shape in the DS. Use: bopindex s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  int  ind    = pDS->Index(aS);
  bool bFound = (ind > 0);
  if (bFound)
  {
    di << " Index: " << ind << "\n";
  }
  else
  {
    di << " DS does not contain the shape\n";
  }
  //
  return 0;
}

//=================================================================================================

int bopsd(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Gets the Same domain shape. Use: bopsd #\n";
    return 0;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  bool bHasSD;
  int  ind, i1, i2, iSD;
  //
  ind = Draw::Atoi(a[1]);
  //
  i1 = 0;
  i2 = pDS->NbShapes();
  if (ind < i1 || ind > i2)
  {
    di << " DS does not contain the shape\n";
    return 0;
  }
  //
  bHasSD = pDS->HasShapeSD(ind, iSD);
  if (bHasSD)
  {
    Sprintf(buf, " Shape %d has SD shape %d\n", ind, iSD);
    di << buf;
  }
  else
  {
    Sprintf(buf, " Shape: %d has no SD shape\n", ind);
    di << buf;
  }
  //
  return 0;
}

//=================================================================================================

int bopsc(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 3)
  {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char                                                     buf[32];
  const char*                                              aText;
  Draw_Color                                               aTextColor(Draw_cyan);
  int                                                      nSF1, nSF2, nF1, nF2;
  int                                                      aNb, j, iCnt, k, iPriz, aNbC, aNbP, nSp;
  int                                                      iX;
  occ::handle<BOPTest_DrawableShape>                       aDShape;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB;
  //
  nSF1 = nSF2 = -1;
  if (n > 1)
    nSF1 = Draw::Atoi(a[1]);
  if (n > 2)
    nSF2 = Draw::Atoi(a[2]);

  NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  //
  iCnt  = 0;
  iPriz = 0;
  aNb   = aFFs.Length();
  for (j = 0; j < aNb; ++j)
  {
    const BOPDS_InterfFF& aFF = aFFs(j);
    if (n == 3)
    {
      if (!aFF.Contains(nSF1) || !aFF.Contains(nSF2))
      {
        continue;
      }
      iPriz = 1;
    }
    else if (n == 2)
    {
      if (!aFF.Contains(nSF1))
        continue;
    }
    //
    aFF.Indices(nF1, nF2);
    //
    iX                                          = 0;
    const NCollection_Vector<BOPDS_Curve>& aVNC = aFF.Curves();
    aNbC                                        = aVNC.Length();
    for (k = 0; k < aNbC; ++k)
    {
      const BOPDS_Curve&                                    aNC  = aVNC(k);
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aNC.PaveBlocks();
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
        if (!aPB->HasEdge(nSp))
        {
          continue;
        }
        //
        if (!iX)
        {
          Sprintf(buf, "[%d %d] section edges: ", nF1, nF2);
          di << buf;
          iX = 1;
        }
        Sprintf(buf, "t_%d_%d", k, nSp);
        di << buf;
        //
        const TopoDS_Shape& aSp = pDS->Shape(nSp);
        aText                   = buf;
        aDShape                 = new BOPTest_DrawableShape(aSp, aText, aTextColor);
        Draw::Set(aText, aDShape);
        di << " ";
        ++iCnt;
      }
    }
    if (iX)
    {
      di << "\n";
    }
    //
    iX                                          = 0;
    const NCollection_Vector<BOPDS_Point>& aVNP = aFF.Points();
    aNbP                                        = aVNP.Length();
    for (k = 0; k < aNbP; ++k)
    {
      const BOPDS_Point& aNP = aVNP(k);
      nSp                    = aNP.Index();
      if (nSp < 0)
      {
        continue;
      }
      if (!iX)
      {
        Sprintf(buf, "[%d %d] section vertices: ", nF1, nF2);
        di << buf;
        iX = 1;
      }
      Sprintf(buf, "p_%d_%d", k, nSp);
      di << buf;
      //
      const TopoDS_Shape& aSp = pDS->Shape(nSp);
      aText                   = buf;
      aDShape                 = new BOPTest_DrawableShape(aSp, aText, aTextColor);
      Draw::Set(aText, aDShape);
      di << " ";
      ++iCnt;
    }
    if (iX)
    {
      di << "\n";
    }
    //
    if (iPriz)
    {
      break;
    }
  } // for (j=0; j<aNb; ++j) {
  //
  if (iCnt)
  {
    di << "\n";
  }
  else
  {
    di << " no sections found\n";
  }
  //
  return 0;
}

//=================================================================================================

int boppb(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 2)
  {
    di << "Shows information about pave blocks. Use: boppb [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  bool                                                     bHasPaveBlocks;
  int                                                      ind, i1, i2;
  TopAbs_ShapeEnum                                         aType;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2)
  {
    ind = Draw::Atoi(a[1]);
    i1  = ind;
    i2  = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind)
  {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType                      = aSI.ShapeType();
    if (aType != TopAbs_EDGE)
    {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks)
    {
      continue;
    }
    //
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
      aPB->Dump();
      printf("\n");
    }
  }
  //
  return 0;
}

//=================================================================================================

int bopcb(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 2)
  {
    di << "Shows information about common blocks. Use: bopcb [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  bool                                                     bHasPaveBlocks;
  int                                                      ind, i1, i2;
  TopAbs_ShapeEnum                                         aType;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB;
  NCollection_Map<occ::handle<BOPDS_CommonBlock>>          aMCB;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2)
  {
    ind = Draw::Atoi(a[1]);
    i1  = ind;
    i2  = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind)
  {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType                      = aSI.ShapeType();
    if (aType != TopAbs_EDGE)
    {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks)
    {
      continue;
    }
    //
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
      if (pDS->IsCommonBlock(aPB))
      {
        const occ::handle<BOPDS_CommonBlock>& aCB = pDS->CommonBlock(aPB);
        if (aMCB.Add(aCB))
        {
          aCB->Dump();
          printf("\n");
        }
      }
    }
  }
  //
  return 0;
}

//=================================================================================================

int bopsp(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 2)
  {
    di << "Shows the splits of edges. Use: bopsp [#e]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char                                                     buf[32];
  bool                                                     bHasPaveBlocks;
  int                                                      ind, i1, i2, nSp;
  TopAbs_ShapeEnum                                         aType;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB;
  const char*                                              aText;
  Draw_Color                                               aTextColor(Draw_cyan);
  occ::handle<BOPTest_DrawableShape>                       aDShape;
  //
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (n == 2)
  {
    ind = Draw::Atoi(a[1]);
    i1  = ind;
    i2  = ind + 1;
  }
  //
  for (ind = i1; ind < i2; ++ind)
  {
    const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(ind);
    aType                      = aSI.ShapeType();
    if (aType != TopAbs_EDGE)
    {
      continue;
    }
    //
    bHasPaveBlocks = pDS->HasPaveBlocks(ind);
    if (!bHasPaveBlocks)
    {
      continue;
    }
    //
    di << "Edge " << ind << ": ";
    //
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = pDS->PaveBlocks(ind);
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
      nSp                                     = aPB->Edge();
      const TopoDS_Shape& aSp                 = pDS->Shape(nSp);
      //
      Sprintf(buf, "z%d_%d", ind, nSp);
      aText   = buf;
      aDShape = new BOPTest_DrawableShape(aSp, aText, aTextColor);
      Draw::Set(aText, aDShape);
      di << buf << " ";
    }
    di << "\n";
  }
  //
  return 0;
}

//=================================================================================================

int bopfon(Draw_Interpretor& di, int n, const char** a)
{
  return bopfinfo(di, n, a, 0);
}

//=================================================================================================

int bopfin(Draw_Interpretor& di, int n, const char** a)
{
  return bopfinfo(di, n, a, 1);
}

//=================================================================================================

int bopfsc(Draw_Interpretor& di, int n, const char** a)
{
  return bopfinfo(di, n, a, 2);
}

//=================================================================================================

int bopfinfo(Draw_Interpretor& di, int n, const char** a, const int iPriz)
{
  if (n != 2)
  {
    di << "Shows " << ((iPriz == 0) ? "ON" : ((iPriz == 1) ? "IN" : "SC"))
       << " information for the face. Use: bopf* #f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  char                         aText[32];
  int                          nF, i1, i2, nV, i, aNb;
  occ::handle<BOPDS_PaveBlock> aPB;
  //
  nF = Draw::Atoi(a[1]);
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (nF < i1 || nF > i2)
  {
    di << " DS does not contain the shape\n";
    return 1;
  }
  //
  if (pDS->ShapeInfo(nF).ShapeType() != TopAbs_FACE)
  {
    di << " The shape is not a face\n";
    return 1;
  }
  //
  if (!pDS->HasFaceInfo(nF))
  {
    di << " The face has no face information\n";
    return 0;
  }
  //
  BOPDS_FaceInfo& aFI = pDS->ChangeFaceInfo(nF);
  //
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>> aMPB;
  NCollection_Map<int>                                 aMI;
  if (iPriz == 0)
  {
    strcpy(aText, "On");
    aMPB = aFI.ChangePaveBlocksOn();
    aMI  = aFI.ChangeVerticesOn();
  }
  else if (iPriz == 1)
  {
    strcpy(aText, "In");
    aMPB = aFI.ChangePaveBlocksIn();
    aMI  = aFI.ChangeVerticesIn();
  }
  else if (iPriz == 2)
  {
    strcpy(aText, "Sc");
    aMPB = aFI.ChangePaveBlocksSc();
    aMI  = aFI.ChangeVerticesSc();
  }
  //
  if (aMPB.Extent())
  {
    printf(" pave blocks %s:\n", aText);
    aNb = aMPB.Extent();
    for (i = 1; i <= aNb; ++i)
    {
      aPB = aMPB(i);
      aPB->Dump();
      printf(" \n");
    }
  }
  else
  {
    printf(" no pave blocks %s found\n", aText);
  }
  //
  if (aMI.Extent())
  {
    printf(" vertices %s:\n", aText);
    NCollection_Map<int>::Iterator aItMI(aMI);
    for (; aItMI.More(); aItMI.Next())
    {
      nV = aItMI.Value();
      printf(" %d", nV);
    }
    printf(" \n");
  }
  else
  {
    printf(" no verts %s found\n", aText);
  }
  //
  return 0;
}

//=======================================================================
// function : bopfav
// purpose  : alone vertices on face
//=======================================================================
int bopfav(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows information about alone vertices for the face. Use: bopfav #f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  int i1, i2, nF, nV;
  //
  nF = Draw::Atoi(a[1]);
  i1 = 0;
  i2 = pDS->NbSourceShapes();
  if (nF < i1 || nF > i2)
  {
    di << "DS does not contain the shape\n";
    return 1;
  }
  //
  if (pDS->ShapeInfo(nF).ShapeType() != TopAbs_FACE)
  {
    di << " The shape is not a face\n";
    return 1;
  }
  //
  if (!pDS->HasFaceInfo(nF))
  {
    di << " The face has no face information\n";
    return 0;
  }
  //
  NCollection_List<int> aLI;
  pDS->AloneVertices(nF, aLI);
  if (!aLI.Extent())
  {
    di << " no alone vertices found\n";
    return 0;
  }
  //
  di << " alone vertices: \n";
  NCollection_List<int>::Iterator aItLI(aLI);
  for (; aItLI.More(); aItLI.Next())
  {
    nV = aItLI.Value();
    di << nV << " ";
  }
  di << "\n";
  //
  return 0;
}

//=================================================================================================

int bopimage(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows split parts of the shape. Use: bopimage s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    anImages = aBuilder.Images();
  if (!anImages.IsBound(aS))
  {
    di << " no images found\n";
    return 0;
  }
  //
  char            buf[32];
  int             i;
  BRep_Builder    aBB;
  TopoDS_Compound aC;
  //
  aBB.MakeCompound(aC);
  //
  const NCollection_List<TopoDS_Shape>&    aLSIm = anImages.Find(aS);
  NCollection_List<TopoDS_Shape>::Iterator aIt(aLSIm);
  for (i = 0; aIt.More(); aIt.Next(), ++i)
  {
    const TopoDS_Shape& aSIm = aIt.Value();
    aBB.Add(aC, aSIm);
  }
  //
  di << i << " images found\n";
  Sprintf(buf, "%s_im", a[1]);
  DBRep::Set(buf, aC);
  di << buf << "\n";
  //
  return 0;
}

//=================================================================================================

int boporigin(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows the original shape for the shape. Use: boporigin s\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 0;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aDMI = aBuilder.Origins();
  if (!aDMI.IsBound(aS))
  {
    di << " no origins found\n";
    return 0;
  }
  //
  char buf[32];
  Sprintf(buf, "%s_or", a[1]);
  //
  const NCollection_List<TopoDS_Shape>& aLSx = aDMI.Find(aS);
  if (aLSx.Extent() == 1)
  {
    DBRep::Set(buf, aLSx.First());
    di << "1 origin found\n" << buf << "\n";
    return 0;
  }
  //
  TopoDS_Compound aCOr;
  BRep_Builder().MakeCompound(aCOr);
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLSx(aLSx);
  for (; aItLSx.More(); aItLSx.Next())
  {
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

//=================================================================================================

int bopfsd(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Shows SD faces for the face: Use: bopfsd f\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& aDMSD =
    aBuilder.ShapesSD();
  if (!aDMSD.IsBound(aS))
  {
    di << " shape has no sd shape\n";
    return 0;
  }
  //
  char            buf[32];
  int             i;
  BRep_Builder    aBB;
  TopoDS_Compound aC;
  //
  aBB.MakeCompound(aC);
  //
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItSD;
  aItSD.Initialize(aDMSD);
  for (i = 0; aItSD.More(); aItSD.Next())
  {
    const TopoDS_Shape& aSK = aItSD.Key();
    const TopoDS_Shape& aSV = aItSD.Value();
    if (aSK.IsEqual(aS))
    {
      if (!aSV.IsEqual(aS))
      {
        aBB.Add(aC, aS);
        ++i;
      }
    }
    //
    else if (aSV.IsEqual(aS))
    {
      if (!aSK.IsEqual(aS))
      {
        aBB.Add(aC, aS);
        ++i;
      }
    }
  }
  //
  di << i << " SD shapes found\n";
  //
  Sprintf(buf, "%s_sd", a[1]);
  DBRep::Set(buf, aC);
  //
  di << buf << "\n";
  return 0;
}

//=================================================================================================

int bopbface(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Build faces from surface and set of shared edges. Use: bopbface fr cx\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  TopoDS_Face                    aF;
  NCollection_List<TopoDS_Shape> aLE;
  int                            i;
  //
  TopoDS_Iterator aItS(aS);
  for (i = 0; aItS.More(); aItS.Next(), ++i)
  {
    const TopoDS_Shape& aSx = aItS.Value();
    if (!i)
    {
      if (aSx.ShapeType() != TopAbs_FACE)
      {
        di << " shape " << i << " is not a face\n";
        return 1;
      }
      aF = *(TopoDS_Face*)&aSx;
    }
    else
    {
      if (aSx.ShapeType() != TopAbs_EDGE)
      {
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
  BOPTest::ReportAlerts(aBF.GetReport());
  if (aBF.HasErrors())
  {
    return 0;
  }
  //
  char                                     buf[128];
  const NCollection_List<TopoDS_Shape>&    aLFR = aBF.Areas();
  NCollection_List<TopoDS_Shape>::Iterator aIt(aLFR);
  for (i = 1; aIt.More(); aIt.Next(), ++i)
  {
    const TopoDS_Shape& aFR = aIt.Value();
    Sprintf(buf, "%s_%d", a[1], i);
    DBRep::Set(buf, aFR);
    di << " " << buf;
  }
  //
  i = aLFR.Extent();
  if (i)
  {
    di << "\n " << i << " faces were built\n";
  }
  else
  {
    di << " No faces were built\n";
  }
  //
  return 0;
}

//=================================================================================================

int bopbsolid(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << "Build solids from set of shared faces. Use: bopbsolid sr cx\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull())
  {
    di << a[1] << " is a null shape\n";
    return 1;
  }
  //
  NCollection_List<TopoDS_Shape> aLF;
  TopExp_Explorer                aExp(aS, TopAbs_FACE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aF = aExp.Current();
    aLF.Append(aF);
  }
  //
  if (aLF.IsEmpty())
  {
    di << " No faces to build solids\n";
    return 1;
  }
  //
  BOPAlgo_BuilderSolid aBS;
  aBS.SetShapes(aLF);
  aBS.Perform();
  BOPTest::ReportAlerts(aBS.GetReport());
  if (aBS.HasErrors())
  {
    return 0;
  }
  //
  int             i;
  TopoDS_Compound aSolids;
  BRep_Builder    aBB;
  //
  aBB.MakeCompound(aSolids);
  //
  char                                     buf[128];
  const NCollection_List<TopoDS_Shape>&    aLSR = aBS.Areas();
  NCollection_List<TopoDS_Shape>::Iterator aIt(aLSR);
  for (i = 1; aIt.More(); aIt.Next(), ++i)
  {
    const TopoDS_Shape& aSR = aIt.Value();
    Sprintf(buf, "%s_%d", a[1], i);
    DBRep::Set(buf, aSR);
    di << " " << buf;
  }
  //
  i = aLSR.Extent();
  if (i)
  {
    di << "\n " << i << " solids were built\n";
  }
  else
  {
    di << " No solids were built\n";
  }
  //
  return 0;
}

//=================================================================================================

void GetTypeByName(const char* theName, TopAbs_ShapeEnum& theType)
{
  if (!strcmp(theName, "v") || !strcmp(theName, "V"))
  {
    theType = TopAbs_VERTEX;
  }
  else if (!strcmp(theName, "e") || !strcmp(theName, "E"))
  {
    theType = TopAbs_EDGE;
  }
  else if (!strcmp(theName, "w") || !strcmp(theName, "W"))
  {
    theType = TopAbs_WIRE;
  }
  else if (!strcmp(theName, "f") || !strcmp(theName, "F"))
  {
    theType = TopAbs_FACE;
  }
  else if (!strcmp(theName, "sh") || !strcmp(theName, "Sh") || !strcmp(theName, "SH"))
  {
    theType = TopAbs_SHELL;
  }
  else if (!strcmp(theName, "s") || !strcmp(theName, "S"))
  {
    theType = TopAbs_SOLID;
  }
  else if (!strcmp(theName, "cs") || !strcmp(theName, "Cs") || !strcmp(theName, "CS"))
  {
    theType = TopAbs_COMPSOLID;
  }
  else if (!strcmp(theName, "c") || !strcmp(theName, "C"))
  {
    theType = TopAbs_COMPOUND;
  }
  else
  {
    theType = TopAbs_SHAPE;
  }
}

//=================================================================================================

void GetNameByType(const TopAbs_ShapeEnum& theType, char* theName)
{
  switch (theType)
  {
    case TopAbs_VERTEX:
      strcpy(theName, "V");
      break;
    case TopAbs_EDGE:
      strcpy(theName, "E");
      break;
    case TopAbs_WIRE:
      strcpy(theName, "w");
      break;
    case TopAbs_FACE:
      strcpy(theName, "F");
      break;
    case TopAbs_SHELL:
      strcpy(theName, "Sh");
      break;
    case TopAbs_SOLID:
      strcpy(theName, "S");
      break;
    case TopAbs_COMPSOLID:
      strcpy(theName, "Cs");
      break;
    case TopAbs_COMPOUND:
      strcpy(theName, "c");
      break;
    default:
      strcpy(theName, "Shape");
      break;
  }
}

//=================================================================================================

template <class InterfType>
void DumpInterfs(const NCollection_Vector<InterfType>& theVInterf, Draw_Interpretor& di)
{
  int  i, aNb, n1, n2, nNew;
  char buf[64];
  //
  aNb = theVInterf.Length();
  if (aNb == 0)
  {
    di << "Not found\n";
    return;
  }
  //
  di << aNb << " interference(s) found\n";
  for (i = 0; i < aNb; ++i)
  {
    const InterfType& anInt = theVInterf(i);
    anInt.Indices(n1, n2);
    if (anInt.HasIndexNew())
    {
      nNew = anInt.IndexNew();
      Sprintf(buf, " (%d, %d, %d)\n", n1, n2, nNew);
    }
    else
    {
      Sprintf(buf, " (%d, %d)\n", n1, n2);
    }
    di << buf;
  }
}

//=================================================================================================

template <class InterfType>
void SearchNewIndex(const char*                           theCType,
                    const int                             theInd,
                    const NCollection_Vector<InterfType>& theVInterf,
                    Draw_Interpretor&                     di)
{
  char buf[64];
  bool bFound;
  int  i, aNb, n1, n2, nNew;
  //
  bFound = false;
  aNb    = theVInterf.Length();
  for (i = 0; i < aNb; ++i)
  {
    const InterfType& anInt = theVInterf(i);
    nNew                    = anInt.IndexNew();
    if (theInd == nNew)
    {
      if (!bFound)
      {
        di << theCType;
        bFound = true;
      }
      //
      anInt.Indices(n1, n2);
      Sprintf(buf, "(%d, %d) ", n1, n2);
      di << buf;
    }
  }
  if (bFound)
  {
    di << "\n";
  }
}
