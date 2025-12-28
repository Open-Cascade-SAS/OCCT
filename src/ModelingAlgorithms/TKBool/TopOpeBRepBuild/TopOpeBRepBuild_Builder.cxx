// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceCU();
extern bool TopOpeBRepBuild_GettraceCUV();
extern bool TopOpeBRepBuild_GettraceSPF();
extern bool TopOpeBRepBuild_GettraceSPS();
extern bool TopOpeBRepBuild_GetcontextSF2();

Standard_EXPORT void debmarksplit(const int i)
{
  std::cout << "++ debmarksplit " << i << std::endl;
}

Standard_EXPORT void debchangesplit(const int i)
{
  std::cout << "++ debchangesplit " << i << std::endl;
}

Standard_EXPORT void debspf(const int i)
{
  std::cout << "++  debspf" << i << std::endl;
}
#endif

static int STATIC_SOLIDINDEX = 0;

//=================================================================================================

TopOpeBRepBuild_Builder::TopOpeBRepBuild_Builder(const TopOpeBRepDS_BuildTool& BT)
    : myBuildTool(BT),
      mySectionDone(false),
      myIsKPart(0),
      myClassifyDef(false),
      myClassifyVal(true),
      myProcessON(false)
{
  InitSection();
}

// modified by NIZHNY-MZV  Sat May  6 10:04:49 2000
//=======================================================================
// function : ~TopOpeBRepBuild_Builder
// purpose  : virtual destructor
//=======================================================================
TopOpeBRepBuild_Builder::~TopOpeBRepBuild_Builder() {}

//=================================================================================================

TopOpeBRepDS_BuildTool& TopOpeBRepBuild_Builder::ChangeBuildTool()
{
  return myBuildTool;
}

//=================================================================================================

const TopOpeBRepDS_BuildTool& TopOpeBRepBuild_Builder::BuildTool() const
{
  return myBuildTool;
}

//=================================================================================================

occ::handle<TopOpeBRepDS_HDataStructure> TopOpeBRepBuild_Builder::DataStructure() const
{
  return myDataStructure;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
#ifdef OCCT_DEBUG
  GdumpSHASETreset();
#endif
  Clear();
  myDataStructure = HDS;
  BuildVertices(HDS);
  SplitEvisoONperiodicF();
  BuildEdges(HDS);
  BuildFaces(HDS);
  myIsKPart = 0;
  InitSection();
  SplitSectionEdges();
  TopOpeBRepDS_Filter F(HDS, &myShapeClassifier);
  F.ProcessFaceInterferences(mySplitON);
  TopOpeBRepDS_Reducer R(HDS);
  R.ProcessFaceInterferences(mySplitON);
} // Perform

//=================================================================================================

void TopOpeBRepBuild_Builder::Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                      const TopoDS_Shape&                             S1,
                                      const TopoDS_Shape&                             S2)
{
  Perform(HDS);
  myShape1  = S1;
  myShape2  = S2;
  myIsKPart = FindIsKPart();
} // Perform

//=================================================================================================

void TopOpeBRepBuild_Builder::AddIntersectionEdges(TopoDS_Shape&             aFace,
                                                   const TopAbs_State        ToBuild1,
                                                   const bool                RevOri1,
                                                   TopOpeBRepBuild_ShapeSet& WES) const
{
  TopoDS_Shape               anEdge;
  TopOpeBRepDS_CurveIterator FCurves = myDataStructure->FaceCurves(aFace);
  for (; FCurves.More(); FCurves.Next())
  {
    int                                   iC    = FCurves.Current();
    const NCollection_List<TopoDS_Shape>& LnewE = NewEdges(iC);
    for (NCollection_List<TopoDS_Shape>::Iterator Iti(LnewE); Iti.More(); Iti.Next())
    {
      anEdge                    = Iti.Value();
      TopAbs_Orientation ori    = FCurves.Orientation(ToBuild1);
      TopAbs_Orientation newori = Orient(ori, RevOri1);

      if (newori == TopAbs_EXTERNAL)
        continue;

      myBuildTool.Orientation(anEdge, newori);
      const occ::handle<Geom2d_Curve>& PC = FCurves.PCurve();
      myBuildTool.PCurve(aFace, anEdge, PC);
      WES.AddStartElement(anEdge);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::Clear()
{
  if (myDataStructure.IsNull())
  {
    myMergedOUT.Clear();
    myMergedIN.Clear();
    myMergedON.Clear();
    return;
  }

  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  for (NCollection_DataMap<TopoDS_Shape,
                           TopOpeBRepDS_ListOfShapeOn1State,
                           TopTools_ShapeMapHasher>::Iterator it(mySplitOUT);
       it.More();
       it.Next())
  {
    const TopoDS_Shape& e = it.Key();
    if (e.ShapeType() == TopAbs_EDGE)
    {
      bool isse = BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse)
        mySplitOUT.ChangeFind(e).Clear();
    }
  }
  for (NCollection_DataMap<TopoDS_Shape,
                           TopOpeBRepDS_ListOfShapeOn1State,
                           TopTools_ShapeMapHasher>::Iterator it(mySplitIN);
       it.More();
       it.Next())
  {
    const TopoDS_Shape& e = it.Key();
    if (e.ShapeType() == TopAbs_EDGE)
    {
      bool isse = BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse)
        mySplitIN.ChangeFind(e).Clear();
    }
  }
  for (NCollection_DataMap<TopoDS_Shape,
                           TopOpeBRepDS_ListOfShapeOn1State,
                           TopTools_ShapeMapHasher>::Iterator it(mySplitON);
       it.More();
       it.Next())
  {
    const TopoDS_Shape& e = it.Key();
    if (e.ShapeType() == TopAbs_EDGE)
    {
      bool isse = BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse)
        mySplitON.ChangeFind(e).Clear();
    }
  }

  myMergedOUT.Clear();
  myMergedIN.Clear();
  myMergedON.Clear();
} // Clear

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::NewFaces(const int I) const
{
  const NCollection_List<TopoDS_Shape>& L = myNewFaces->Array1().Value(I);
  return L;
} // NewFaces

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::ChangeNewFaces(const int I)
{
  NCollection_List<TopoDS_Shape>& L = myNewFaces->ChangeArray1().ChangeValue(I);
  return L;
} // ChangeNewFaces

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::NewEdges(const int I) const
{
  if (myNewEdges.IsBound(I))
  {
    return myNewEdges.Find(I);
  }
  else
  {
    return myEmptyShapeList;
  }
} // NewEdges

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::ChangeNewEdges(const int I)
{
  if (!myNewEdges.IsBound(I))
  {
    NCollection_List<TopoDS_Shape> thelist;
    myNewEdges.Bind(I, thelist);
  }
  NCollection_List<TopoDS_Shape>& L = myNewEdges.ChangeFind(I);
  return L;
} // ChangeNewEdges

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_Builder::NewVertex(const int I) const
{
  const TopoDS_Shape& V = myNewVertices->Array1().Value(I);
  return V;
} // NewVertex

//=================================================================================================

TopoDS_Shape& TopOpeBRepBuild_Builder::ChangeNewVertex(const int I)
{
  TopoDS_Shape& V = myNewVertices->ChangeArray1().ChangeValue(I);
  return V;
} // ChangeNewVertex

//=================================================================================================

bool TopOpeBRepBuild_Builder::ToSplit(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  bool issplit = IsSplit(S, ToBuild);
  bool hasgeom = myDataStructure->HasGeometry(S);
  bool hassame = myDataStructure->HasSameDomain(S);
  bool tosplit = (!issplit) && (hasgeom || hassame);

#ifdef OCCT_DEBUG
  int  iS;
  bool tSPS = GtraceSPS(S, iS);
  if (tSPS)
  {
    std::cout << "tosplit " << tosplit << " : !issplit " << (!issplit);
    std::cout << " && (hasgeom || hassame) (" << hasgeom << " || " << hassame << ")" << std::endl;
  }
#endif

  return tosplit;
} // ToSplit

//=================================================================================================

void TopOpeBRepBuild_Builder::MarkSplit(const TopoDS_Shape& S,
                                        const TopAbs_State  ToBuild,
                                        const bool          Bval)
{
  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>* p =
    nullptr;
  if (ToBuild == TopAbs_OUT)
    p = &mySplitOUT;
  else if (ToBuild == TopAbs_IN)
    p = &mySplitIN;
  else if (ToBuild == TopAbs_ON)
    p = &mySplitON;
  if (p == nullptr)
    return;

  TopOpeBRepDS_ListOfShapeOn1State thelist;
  if (!(*p).IsBound(S))
    (*p).Bind(S, thelist);
  TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).ChangeFind(S);
  losos.Split(Bval);

#ifdef OCCT_DEBUG
  int  iS;
  bool tSPS = GtraceSPS(S, iS);
  if (tSPS)
  {
    GdumpSHA(S, (char*)"MarkSplit ");
    std::cout << " ";
    TopAbs::Print(ToBuild, std::cout);
    std::cout << " " << Bval << std::endl;
    debmarksplit(iS);
  }
#endif

} // MarkSplit

//=================================================================================================

bool TopOpeBRepBuild_Builder::IsSplit(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  bool                                                res = false;
  const NCollection_DataMap<TopoDS_Shape,
                            TopOpeBRepDS_ListOfShapeOn1State,
                            TopTools_ShapeMapHasher>* p   = nullptr;
  if (ToBuild == TopAbs_OUT)
    p = &mySplitOUT;
  else if (ToBuild == TopAbs_IN)
    p = &mySplitIN;
  else if (ToBuild == TopAbs_ON)
    p = &mySplitON;
  if (p == nullptr)
    return res;

  if ((*p).IsBound(S))
  {
    const TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).Find(S);
    res                                           = losos.IsSplit();
#ifdef OCCT_DEBUG
//    int n = losos.ListOnState().Extent();
#endif
  }
  return res;
} // IsSplit

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::Splits(
  const TopoDS_Shape& S,
  const TopAbs_State  ToBuild) const
{
  const NCollection_DataMap<TopoDS_Shape,
                            TopOpeBRepDS_ListOfShapeOn1State,
                            TopTools_ShapeMapHasher>* p = nullptr;
  if (ToBuild == TopAbs_OUT)
    p = &mySplitOUT;
  else if (ToBuild == TopAbs_IN)
    p = &mySplitIN;
  else if (ToBuild == TopAbs_ON)
    p = &mySplitON;
  if (p == nullptr)
    return myEmptyShapeList;

  if ((*p).IsBound(S))
  {
    const TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).Find(S);
    const NCollection_List<TopoDS_Shape>&   L     = losos.ListOnState();
    return L;
  }
  return myEmptyShapeList;
} // Splits

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_Builder::ChangeSplit(const TopoDS_Shape& S,
                                                                     const TopAbs_State  ToBuild)
{
#ifdef OCCT_DEBUG
  int  iS;
  bool tSPS = GtraceSPS(S, iS);
  if (tSPS)
  {
    GdumpSHA(S, (char*)"ChangeSplit ");
    std::cout << " ";
    TopAbs::Print(ToBuild, std::cout);
    std::cout << std::endl;
    debchangesplit(iS);
  }
#endif

  NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>* p =
    nullptr;
  if (ToBuild == TopAbs_OUT)
    p = &mySplitOUT;
  else if (ToBuild == TopAbs_IN)
    p = &mySplitIN;
  else if (ToBuild == TopAbs_ON)
    p = &mySplitON;
  if (p == nullptr)
    return myEmptyShapeList;
  TopOpeBRepDS_ListOfShapeOn1State thelist1;
  if (!(*p).IsBound(S))
    (*p).Bind(S, thelist1);
  TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).ChangeFind(S);
  NCollection_List<TopoDS_Shape>&   L     = losos.ChangeListOnState();
  return L;
} // ChangeSplit

//=======================================================================
// function : ShapePosition
// purpose  : compute position of shape S compared with the shapes of list LS
//           if S is found IN any shape of LS, return IN
//           else return OUT
//=======================================================================
TopAbs_State TopOpeBRepBuild_Builder::ShapePosition(const TopoDS_Shape&                   S,
                                                    const NCollection_List<TopoDS_Shape>& LS)
{
  TopAbs_State state = TopAbs_UNKNOWN;

  // take the edges of myEdgeAvoid as shape to avoid
  // during face classification
  const NCollection_List<TopoDS_Shape>* PLOS = &myEmptyShapeList;
  TopAbs_ShapeEnum                      tS   = S.ShapeType();
  if (tS == TopAbs_FACE)
    PLOS = &myEdgeAvoid;
  // NYI : idem with myFaceAvoid if (tS == TopAbs_SOLID)

  for (NCollection_List<TopoDS_Shape>::Iterator Iti(LS); Iti.More(); Iti.Next())
  {
    const TopoDS_Shape& SLS = Iti.Value();
#ifdef OCCT_DEBUG
//    TopAbs_ShapeEnum tSLS = SLS.ShapeType();
#endif
    state = myShapeClassifier.StateShapeShape(S, (*PLOS), SLS);
    if (state != TopAbs_OUT && state != TopAbs_UNKNOWN)
      return state;
  }
  if (state == TopAbs_UNKNOWN)
    return state;
  return TopAbs_OUT;
}

//=======================================================================
// function : KeepShape
// purpose  : compute <pos2> = position of shape <S1> / shapes of list <LS2>
//           shape S1 is kept
//           - if LS2 is empty
//           - if (pos2 == ToBuild1)
//=======================================================================
bool TopOpeBRepBuild_Builder::KeepShape(const TopoDS_Shape&                   S1,
                                        const NCollection_List<TopoDS_Shape>& LS2,
                                        const TopAbs_State                    ToBuild1)
{
  bool keep = true;
  if (!LS2.IsEmpty())
  {
    TopAbs_State pos2 = ShapePosition(S1, LS2);
    if (pos2 != ToBuild1)
      keep = false;
  }
  return keep;
}

//=======================================================================
// function : TopType
// purpose  : return the type of upper subshape found in <S>
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepBuild_Builder::TopType(const TopoDS_Shape& S)
{
  TopAbs_ShapeEnum             t;
  TopOpeBRepTool_ShapeExplorer e;

  t = TopAbs_COMPOUND;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_COMPSOLID;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_SOLID;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_SHELL;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_FACE;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_WIRE;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_EDGE;
  e.Init(S, t);
  if (e.More())
    return t;
  t = TopAbs_VERTEX;
  e.Init(S, t);
  if (e.More())
    return t;

  return TopAbs_SHAPE;
}

//=======================================================================
// function : Reverse
// purpose  : compute orientation reversibility according to build states
//=======================================================================
bool TopOpeBRepBuild_Builder::Reverse(const TopAbs_State ToBuild1, const TopAbs_State ToBuild2)
{
  bool rev;
  if (ToBuild1 == TopAbs_IN && ToBuild2 == TopAbs_IN)
    rev = false;
  else
    rev = (ToBuild1 == TopAbs_IN);
  return rev;
}

//=======================================================================
// function : Orient
// purpose  : reverse the orientation
//=======================================================================
TopAbs_Orientation TopOpeBRepBuild_Builder::Orient(const TopAbs_Orientation Ori, const bool Reverse)
{
  return !Reverse ? Ori : TopAbs::Complement(Ori);
}

//=======================================================================
// function : FindSameDomain
// purpose  : complete the lists L1,L2 with the shapes of the DS
//           having same domain :
//           L1 = shapes sharing the same domain of L2 shapes
//           L2 = shapes sharing the same domain of L1 shapes
// (L1 contains a face)
//=======================================================================
void TopOpeBRepBuild_Builder::FindSameDomain(NCollection_List<TopoDS_Shape>& L1,
                                             NCollection_List<TopoDS_Shape>& L2) const
{
  int i;
  int nl1 = L1.Extent(), nl2 = L2.Extent();

  while (nl1 > 0 || nl2 > 0)
  {

    NCollection_List<TopoDS_Shape>::Iterator it1(L1);
    for (i = 1; i <= nl1; i++)
    {
      const TopoDS_Shape& S1 = it1.Value();
#ifdef OCCT_DEBUG
//      int iS1 = myDataStructure->Shape(S1);  // DEB
#endif
      NCollection_List<TopoDS_Shape>::Iterator itsd(myDataStructure->SameDomain(S1));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S2 = itsd.Value();
#ifdef OCCT_DEBUG
//	int iS2 = myDataStructure->Shape(S2);// DEB
#endif
        bool found = Contains(S2, L2);
        if (!found)
        {
          L2.Prepend(S2);
          nl2++;
        }
      }
      it1.Next();
    }
    nl1 = 0;

    NCollection_List<TopoDS_Shape>::Iterator it2(L2);
    for (i = 1; i <= nl2; i++)
    {
      const TopoDS_Shape& S2 = it2.Value();
#ifdef OCCT_DEBUG
//      int iS2 = myDataStructure->Shape(S2);// DEB
#endif
      NCollection_List<TopoDS_Shape>::Iterator itsd(myDataStructure->SameDomain(S2));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S1 = itsd.Value();
#ifdef OCCT_DEBUG
//	int iS1 = myDataStructure->Shape(S1);// DEB
#endif
        bool found = Contains(S1, L1);
        if (!found)
        {
          L1.Prepend(S1);
          nl1++;
        }
      }
      it2.Next();
    }
    nl2 = 0;
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::FindSameDomainSameOrientation(
  NCollection_List<TopoDS_Shape>& L1,
  NCollection_List<TopoDS_Shape>& L2) const
{
  FindSameDomain(L1, L2);
  NCollection_List<TopoDS_Shape>::Iterator it(L1);
  if (!it.More())
    return;

  const TopoDS_Shape& sref = it.Value();
#ifdef OCCT_DEBUG
//  int  iref = myDataStructure->SameDomainReference(sref);
#endif
  TopOpeBRepDS_Config oref = myDataStructure->SameDomainOrientation(sref);

  NCollection_List<TopoDS_Shape> LL1, LL2;

  for (it.Initialize(L1); it.More(); it.Next())
  {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
    if (o == oref && !Contains(s, LL1))
      LL1.Append(s);
    else if (o != oref && !Contains(s, LL2))
      LL2.Append(s);
  }

  for (it.Initialize(L2); it.More(); it.Next())
  {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
    if (o == oref && !Contains(s, LL1))
      LL1.Append(s);
    else if (o != oref && !Contains(s, LL2))
      LL2.Append(s);
  }

  L1 = LL1;
  L2 = LL2;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::MapShapes(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  bool S1null = S1.IsNull();
  bool S2null = S2.IsNull();
  ClearMaps();
  if (!S1null)
    TopExp::MapShapes(S1, myMAP1);
  if (!S2null)
    TopExp::MapShapes(S2, myMAP2);
}

//=================================================================================================

void TopOpeBRepBuild_Builder::ClearMaps()
{
  myMAP1.Clear();
  myMAP2.Clear();
}

//=================================================================================================

void TopOpeBRepBuild_Builder::FindSameRank(const NCollection_List<TopoDS_Shape>& L1,
                                           const int                             rank,
                                           NCollection_List<TopoDS_Shape>&       L2) const
{
  for (NCollection_List<TopoDS_Shape>::Iterator it1(L1); it1.More(); it1.Next())
  {
    const TopoDS_Shape& s = it1.Value();
    int                 r = ShapeRank(s);
    if (r == rank && !Contains(s, L2))
    {
      L2.Append(s);
    }
  }
}

//=================================================================================================

int TopOpeBRepBuild_Builder::ShapeRank(const TopoDS_Shape& s) const
{
  bool isof1   = IsShapeOf(s, 1);
  bool isof2   = IsShapeOf(s, 2);
  int  ancetre = (isof1 || isof2) ? ((isof1) ? 1 : 2) : 0;
  return ancetre;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::IsShapeOf(const TopoDS_Shape& s, const int i) const
{
  bool b = false;
  if (i == 1)
    b = myMAP1.Contains(s);
  else if (i == 2)
    b = myMAP2.Contains(s);
  return b;
}

//=======================================================================
// function : Contains
// purpose  : returns True if S is in the list L.
//=======================================================================
bool TopOpeBRepBuild_Builder::Contains(const TopoDS_Shape&                   S,
                                       const NCollection_List<TopoDS_Shape>& L)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(L); it.More(); it.Next())
  {
    const TopoDS_Shape& SL     = it.Value();
    bool                issame = SL.IsSame(S);
    if (issame)
      return true;
  }
  return false;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::Opec12() const
{
  bool b = (myState1 == TopAbs_OUT) && (myState2 == TopAbs_IN);
  return b;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::Opec21() const
{
  bool b = (myState1 == TopAbs_IN) && (myState2 == TopAbs_OUT);
  return b;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::Opecom() const
{
  bool b = (myState1 == TopAbs_IN) && (myState2 == TopAbs_IN);
  return b;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::Opefus() const
{
  bool b = (myState1 == TopAbs_OUT) && (myState2 == TopAbs_OUT);
  return b;
}

//=================================================================================================

const NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>&
  TopOpeBRepBuild_Builder::MSplit(const TopAbs_State s) const
{
  if (s == TopAbs_IN)
    return mySplitIN;
  else if (s == TopAbs_OUT)
    return mySplitOUT;
  else if (s == TopAbs_ON)
    return mySplitON;
  return mySplitIN;
}

//=================================================================================================

NCollection_DataMap<TopoDS_Shape, TopOpeBRepDS_ListOfShapeOn1State, TopTools_ShapeMapHasher>&
  TopOpeBRepBuild_Builder::ChangeMSplit(const TopAbs_State s)
{
  if (s == TopAbs_IN)
    return mySplitIN;
  else if (s == TopAbs_OUT)
    return mySplitOUT;
  else if (s == TopAbs_ON)
    return mySplitON;
  return mySplitIN;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitEdge(const TopoDS_Shape& E,
                                        const TopAbs_State  ToBuild1,
                                        const TopAbs_State  ToBuild2)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GetcontextSF2())
  {
    SplitEdge2(E, ToBuild1, ToBuild2);
    return;
  }
#endif
  SplitEdge1(E, ToBuild1, ToBuild2);
  return;
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitEdge1(const TopoDS_Shape& Eoriented,
                                         const TopAbs_State  ToBuild1,
                                         const TopAbs_State  ToBuild2)
{
  // work on a FORWARD edge <Eforward>

  TopoDS_Shape Eforward = Eoriented;
  Eforward.Orientation(TopAbs_FORWARD);

  bool tosplit = ToSplit(Eoriented, ToBuild1);

#ifdef OCCT_DEBUG
  int  iEdge;
  bool tSPS = GtraceSPS(Eoriented, iEdge);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHASTA(Eoriented, ToBuild1, "--- SplitEdge ");
    std::cout << std::endl;
  }
#endif

  if (!tosplit)
    return;

  Reverse(ToBuild1, ToBuild2);
  Reverse(ToBuild2, ToBuild1);
  bool ConnectTo1 = true;
  bool ConnectTo2 = false;

  // build the list of edges to split : LE1, LE2
  NCollection_List<TopoDS_Shape> LE1, LE2;
  LE1.Append(Eforward);
  FindSameDomain(LE1, LE2);

#ifdef OCCT_DEBUG
  if (tSPS)
  {
    GdumpSAMDOM(LE1, (char*)"1 : ");
  }
  if (tSPS)
  {
    GdumpSAMDOM(LE2, (char*)"2 : ");
  }
  if (tSPS)
  {
    std::cout << std::endl;
  }
  if (tSPS)
  {
    std::cout << "V of edge ";
    TopAbs::Print(Eforward.Orientation(), std::cout);
  }
  if (tSPS)
  {
    std::cout << std::endl;
  }
  if (tSPS)
  {
    GdumpEDG(Eforward);
  }
#endif

  // SplitEdge on a edge having other same domained edges on the
  // other shape : do not reverse orientation of edges in FillEdge

  // Make a PaveSet <PVS> on edge <Eforward>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT, ToBuild1, PVS);

  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  bool                           equalpar = PVS.HasEqualParameters();
  if (equalpar)
    VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices,
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward, ToBuild1);

  PVS.InitLoop();
  if (!PVS.MoreLoop())
  {
#ifdef OCCT_DEBUG
    if (tSPS)
    {
      std::cout << "NO VERTEX split ";
      TopAbs::Print(ToBuild1, std::cout);
      std::cout << std::endl;
    }
#endif
    return;
  }

  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS, VCL);

  // Build the new edges
  // -------------------
  NCollection_List<TopoDS_Shape>& EdgeList = ChangeMerged(Eforward, ToBuild1);
  MakeEdges(Eforward, EBU, EdgeList);

  NCollection_List<TopoDS_Shape>::Iterator itLE1, itLE2;

  // connect new edges as edges built <ToBuild1> on LE1 edge
  // --------------------------------------------------------
  for (itLE1.Initialize(LE1); itLE1.More(); itLE1.Next())
  {
    TopoDS_Shape Ecur = itLE1.Value();
    MarkSplit(Ecur, ToBuild1);
    NCollection_List<TopoDS_Shape>& EL = ChangeSplit(Ecur, ToBuild1);
    if (ConnectTo1)
      EL = EdgeList;
  }

  // connect new edges as edges built <ToBuild2> on LE2 edges
  // --------------------------------------------------------
  for (itLE2.Initialize(LE2); itLE2.More(); itLE2.Next())
  {
    TopoDS_Shape Ecur = itLE2.Value();
    MarkSplit(Ecur, ToBuild2);
    NCollection_List<TopoDS_Shape>& EL = ChangeSplit(Ecur, ToBuild2);
    if (ConnectTo2)
      EL = EdgeList;
  }

} // SplitEdge1

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitEdge2(const TopoDS_Shape& Eoriented,
                                         const TopAbs_State  ToBuild1,
                                         const TopAbs_State /*ToBuild2*/)
{
  bool tosplit = ToSplit(Eoriented, ToBuild1);
  if (!tosplit)
    return;

  // work on a FORWARD edge <Eforward>
  TopoDS_Shape Eforward = Eoriented;
  myBuildTool.Orientation(Eforward, TopAbs_FORWARD);

#ifdef OCCT_DEBUG
  int  iEdge;
  bool tSPS = GtraceSPS(Eoriented, iEdge);
  if (tSPS)
  {
    std::cout << std::endl;
  }
  if (tSPS)
  {
    GdumpSHASTA(Eoriented, ToBuild1, "--- SplitEdge2 ");
  }
#endif

  // Make a PaveSet <PVS> on edge <Eforward>
  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT, ToBuild1, PVS);

  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  bool                           equalpar = PVS.HasEqualParameters();
  if (equalpar)
    VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices,
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward, ToBuild1);

  PVS.InitLoop();
  if (!PVS.MoreLoop())
  {
#ifdef OCCT_DEBUG
    if (tSPS)
    {
      std::cout << "NO VERTEX split ";
      TopAbs::Print(ToBuild1, std::cout);
      std::cout << std::endl;
    }
#endif
    return;
  }

  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS, VCL);

  // connect the new edges as split parts <ToBuild1> built on <Eforward>
  NCollection_List<TopoDS_Shape>& EL = ChangeSplit(Eforward, ToBuild1);
  MakeEdges(Eforward, EBU, EL);

} // SplitEdge2

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitFace(const TopoDS_Shape& Foriented,
                                        const TopAbs_State  ToBuild1,
                                        const TopAbs_State  ToBuild2)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GetcontextSF2())
  {
    SplitFace2(Foriented, ToBuild1, ToBuild2);
    return;
  }
#endif
  SplitFace1(Foriented, ToBuild1, ToBuild2);
  return;
}

//=======================================================================
// function : SplitFace1
// purpose  : tout dans le meme edge set
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace1(const TopoDS_Shape& Foriented,
                                         const TopAbs_State  ToBuild1,
                                         const TopAbs_State  ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no
  //
  bool tosplit = ToSplit(Foriented, ToBuild1);
  if (!tosplit)
    return;

  bool RevOri1    = Reverse(ToBuild1, ToBuild2);
  bool RevOri2    = Reverse(ToBuild2, ToBuild1);
  bool ConnectTo1 = true;
  bool ConnectTo2 = false;

  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented;
  myBuildTool.Orientation(Fforward, TopAbs_FORWARD);

  // build the list of faces to split : LF1, LF2
  NCollection_List<TopoDS_Shape> LF1, LF2;
  LF1.Append(Fforward);
  FindSameDomain(LF1, LF2);
  int n1 = LF1.Extent();
  int n2 = LF2.Extent();

  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2)
    RevOri1 = false;
  if (!n1)
    RevOri2 = false;

  // Create an edge set <WES> connected by vertices
  // ----------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES(Fforward, this);

#ifdef OCCT_DEBUG
  bool tSPF  = TopOpeBRepBuild_GettraceSPF();
  int  iFace = myDataStructure->Shape(Foriented);
  if (tSPF)
  {
    std::cout << std::endl;
    GdumpSHASTA(Foriented, ToBuild1, "=== SplitFace ");
  }
  if (tSPF)
  {
    GdumpSAMDOM(LF1, (char*)"1 : ");
    GdumpSAMDOM(LF2, (char*)"2 : ");
  }
  if (tSPF)
    debspf(iFace);
#endif

  NCollection_List<TopoDS_Shape>::Iterator itLF1, itLF2;

  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next())
  {
    const TopoDS_Shape& Fcur = itLF1.Value();
    //                     myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur, ToBuild1, LF2, ToBuild2, WES, RevOri1);
  }

  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next())
  {
    const TopoDS_Shape& Fcur = itLF2.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur, ToBuild2, LF1, ToBuild1, WES, RevOri2);
  }

  // Add the intersection edges to edge set WES
  // -----------------------------------------
  AddIntersectionEdges(Fforward, ToBuild1, RevOri1, WES);

#ifdef OCCT_DEBUG
  int  iF;
  bool tSPS = GtraceSPS(Fforward, iF);
  if (tSPS)
    WES.DumpSS();
#endif

  // Create a Face Builder FBU
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU;
  FBU.InitFaceBuilder(WES, Fforward, false); // forceclass = False

  // Build the new faces
  // -------------------
  NCollection_List<TopoDS_Shape>& FaceList = ChangeMerged(Fforward, ToBuild1);
  MakeFaces(Fforward, FBU, FaceList);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next())
  {
    TopoDS_Shape Fcur = itLF1.Value();
    MarkSplit(Fcur, ToBuild1);
    NCollection_List<TopoDS_Shape>& FL = ChangeSplit(Fcur, ToBuild1);
    if (ConnectTo1)
      FL = FaceList;
  }

  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next())
  {
    TopoDS_Shape Fcur = itLF2.Value();
    MarkSplit(Fcur, ToBuild2);
    NCollection_List<TopoDS_Shape>& FL = ChangeSplit(Fcur, ToBuild2);
    if (ConnectTo2)
      FL = FaceList;
  }

} // SplitFace1

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitFace2(const TopoDS_Shape& Foriented,
                                         const TopAbs_State  ToBuild1,
                                         const TopAbs_State  ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no
  //
  bool tosplit = ToSplit(Foriented, ToBuild1);
  if (!tosplit)
    return;

  bool RevOri1    = Reverse(ToBuild1, ToBuild2);
  bool RevOri2    = Reverse(ToBuild2, ToBuild1);
  bool ConnectTo1 = true;
  bool ConnectTo2 = false;

  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented;
  myBuildTool.Orientation(Fforward, TopAbs_FORWARD);

  NCollection_List<TopoDS_Shape> LF1; // liste des faces de 1   samedomain
  NCollection_List<TopoDS_Shape> LF2; // liste des faces de   2 samedomain
  LF1.Append(Fforward);
  FindSameDomain(LF1, LF2);
  int n1 = LF1.Extent();
  int n2 = LF2.Extent();

#ifdef OCCT_DEBUG
  bool tSPF = TopOpeBRepBuild_GettraceSPF();
  //  int iFace = myDataStructure->Shape(Foriented);
  if (tSPF)
  {
    std::cout << std::endl;
    GdumpSHASTA(Foriented, ToBuild1, "=== SplitFace ");
    GdumpSAMDOM(LF1, (char*)"samedomain 1 : ");
    GdumpSAMDOM(LF2, (char*)"samedomain 2 : ");
  }
#endif

  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2)
    RevOri1 = false;
  if (!n1)
    RevOri2 = false;

  NCollection_List<TopoDS_Shape> LFSO; // liste des faces de 1,2 samedomainsameorientation
  NCollection_List<TopoDS_Shape> LFOO; // liste des faces de 1,2 samedomainoppositeorient

  // LFSO : faces des shapes 1 ou 2, de meme orientation que Fforward.
  // LFOO : faces des shapes 1 ou 2, d'orientation contraire que Fforward.
  LFSO.Append(Fforward);
  FindSameDomainSameOrientation(LFSO, LFOO);

  NCollection_List<TopoDS_Shape> LFSO1, LFOO1; // same domain, same orientation, et du shape de F
  NCollection_List<TopoDS_Shape> LFSO2, LFOO2; // "" "",du shape autre que celui de F

  // on construit les parties ToBuild1 de F
  int rankF = ShapeRank(Foriented);
  int rankX = (rankF) ? ((rankF == 1) ? 2 : 1) : 0;

  FindSameRank(LFSO, rankF, LFSO1);
  FindSameRank(LFOO, rankF, LFOO1);
  FindSameRank(LFSO, rankX, LFSO2);
  FindSameRank(LFOO, rankX, LFOO2);

#ifdef OCCT_DEBUG
  if (tSPF)
  {
    GdumpSAMDOM(LFSO1, (char*)"LFSO1 : ");
    GdumpSAMDOM(LFOO1, (char*)"LFOO1 : ");
    GdumpSAMDOM(LFSO2, (char*)"LFSO2 : ");
    GdumpSAMDOM(LFOO2, (char*)"LFOO2 : ");
  }
#endif

  TopAbs_State tob1     = ToBuild1;
  TopAbs_State tob2     = ToBuild2;
  TopAbs_State tob1comp = (ToBuild1 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  TopAbs_State tob2comp = (ToBuild2 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  NCollection_List<TopoDS_Shape>::Iterator itLF;

  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES1
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES1(Fforward, this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur, tob1, LF2, tob2, WES1, RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur, tob2, LF1, tob1, WES1, RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur, tob1comp, LF2, ToBuild2, WES1, !RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                      myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur, tob2comp, LF1, ToBuild1, WES1, !RevOri2);
  }

  // Add the intersection edges to edge set WES1
  // ------------------------------------------
  AddIntersectionEdges(Fforward, ToBuild1, RevOri1, WES1);

  // Create a Face Builder FBU1
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU1(WES1, Fforward);

  // Build the new faces
  // -------------------
  NCollection_List<TopoDS_Shape>& FaceList1 = ChangeMerged(Fforward, ToBuild1);
  MakeFaces(Fforward, FBU1, FaceList1);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF1); itLF.More(); itLF.Next())
  {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur, ToBuild1);
    NCollection_List<TopoDS_Shape>& FL = ChangeSplit(Fcur, ToBuild1);
    if (ConnectTo1)
      FL = FaceList1;
  }

  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES2
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES2(Fforward, this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur, tob1comp, LF2, tob2, WES2, !RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur, tob2comp, LF1, tob1, WES2, !RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur, tob1, LF2, ToBuild2, WES2, RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next())
  {
    const TopoDS_Shape& Fcur = itLF.Value();
    //                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur, tob2, LF1, ToBuild1, WES2, RevOri2);
  }

  // Add the intersection edges to edge set WES2
  // ------------------------------------------
  AddIntersectionEdges(Fforward, ToBuild2, RevOri2, WES2);

  // Create a Face Builder FBU2
  // -------------------------
  TopOpeBRepBuild_FaceBuilder FBU2(WES2, Fforward);

  // Build the new faces
  // -------------------
  NCollection_List<TopoDS_Shape>& FaceList2 = ChangeMerged(Fforward, ToBuild2);
  MakeFaces(Fforward, FBU2, FaceList2);

  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF2); itLF.More(); itLF.Next())
  {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur, ToBuild2);
    NCollection_List<TopoDS_Shape>& FL = ChangeSplit(Fcur, ToBuild2);
    if (ConnectTo2)
      FL = FaceList2;
  }

} // SplitFace2

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitSolid(const TopoDS_Shape& S1oriented,
                                         const TopAbs_State  ToBuild1,
                                         const TopAbs_State  ToBuild2)
{
  // modified by IFV for treating shell
  bool tosplit = false;
  bool IsShell = (S1oriented.ShapeType() == TopAbs_SHELL);
  if (IsShell)
  {
    TopExp_Explorer ex;
    ex.Init(S1oriented, TopAbs_FACE);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Shape& sh = ex.Current();
      tosplit                = ToSplit(sh, ToBuild1);
      if (tosplit)
        break;
    }
  }
  else
    tosplit = ToSplit(S1oriented, ToBuild1);

  if (!tosplit)
    return;
  // end IFV

  bool RevOri1    = Reverse(ToBuild1, ToBuild2);
  bool RevOri2    = Reverse(ToBuild2, ToBuild1);
  bool ConnectTo1 = true;
  bool ConnectTo2 = false;

  // work on a FORWARD solid <S1forward>
  TopoDS_Shape S1forward = S1oriented;
  myBuildTool.Orientation(S1forward, TopAbs_FORWARD);

  // build the list of solids to split : LS1, LS2
  NCollection_List<TopoDS_Shape> LS1, LS2;
  LS1.Append(S1forward);
  FindSameDomain(LS1, LS2);
  int n1 = LS1.Extent();
  int n2 = LS2.Extent();

  if (!n2)
    RevOri1 = false;
  if (!n1)
    RevOri2 = false;

  // Create a face set <FS> connected by edges
  // -----------------------------------------
  TopOpeBRepBuild_ShellFaceSet SFS;

#ifdef OCCT_DEBUG
  bool tSPS = TopOpeBRepBuild_GettraceSPS();
  //  int iSolid = myDataStructure->Shape(S1oriented);
  if (tSPS)
  {
    std::cout << std::endl;
    GdumpSHASTA(S1oriented, ToBuild1, "___ SplitSolid ");
    GdumpSAMDOM(LS1, (char*)"1 : ");
    GdumpSAMDOM(LS2, (char*)"2 : ");
  }
  SFS.DEBNumber(GdumpSHASETindex());
#endif

  STATIC_SOLIDINDEX = 1;
  NCollection_List<TopoDS_Shape>::Iterator itLS1;
  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next())
  {
    TopoDS_Shape Scur = itLS1.Value();
    FillSolid(Scur, ToBuild1, LS2, ToBuild2, SFS, RevOri1);
  }

  STATIC_SOLIDINDEX = 2;
  NCollection_List<TopoDS_Shape>::Iterator itLS2;
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next())
  {
    TopoDS_Shape Scur = itLS2.Value();
    FillSolid(Scur, ToBuild2, LS1, ToBuild1, SFS, RevOri2);
  }

  // Add the intersection surfaces
  // -----------------------------
  if (myDataStructure->NbSurfaces() > 0)
  {
    TopOpeBRepDS_SurfaceIterator SSurfaces = myDataStructure->SolidSurfaces(S1forward);
    for (; SSurfaces.More(); SSurfaces.Next())
    {
      int                                   iS    = SSurfaces.Current();
      const NCollection_List<TopoDS_Shape>& LnewF = NewFaces(iS);
      for (NCollection_List<TopoDS_Shape>::Iterator Iti(LnewF); Iti.More(); Iti.Next())
      {
        TopoDS_Shape       aFace = Iti.Value();
        TopAbs_Orientation ori   = SSurfaces.Orientation(ToBuild1);
        myBuildTool.Orientation(aFace, ori);

#ifdef OCCT_DEBUG
        if (tSPS)
        {
          TCollection_AsciiString ss("--- SplitSolid ");
          ss = ss + SFS.DEBNumber() + " AddElement SFS+ face ";
          GdumpSHA(aFace, (void*)ss.ToCString());
          std::cout << " ";
          TopAbs::Print(ToBuild1, std::cout) << " : 1 face ";
          TopAbs::Print(ori, std::cout);
          std::cout << std::endl;
        }
#endif
        SFS.AddElement(aFace);
      }
    }
  }

  // Create a Solid Builder SOBU
  // -------------------------
  TopOpeBRepBuild_SolidBuilder SOBU(SFS);

  // Build the new solids on S1
  // --------------------------
  NCollection_List<TopoDS_Shape>& SolidList = ChangeMerged(S1oriented, ToBuild1);
  if (IsShell)
    MakeShells(SOBU, SolidList);
  else
    MakeSolids(SOBU, SolidList);

  // connect list of new solids <SolidList> as solids built on LS1 solids
  // --------------------------------------------------------------------

  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next())
  {
    TopoDS_Shape Scur = itLS1.Value();
    MarkSplit(Scur, ToBuild1);
    NCollection_List<TopoDS_Shape>& SL = ChangeSplit(Scur, ToBuild1);
    if (ConnectTo1)
      SL = SolidList;
  }

  // connect list of new solids <SolidList> as solids built on LS2 solids
  // --------------------------------------------------------------------
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next())
  {
    TopoDS_Shape Scur = itLS2.Value();
    MarkSplit(Scur, ToBuild2);
    NCollection_List<TopoDS_Shape>& SL = ChangeSplit(Scur, ToBuild2);
    if (ConnectTo2)
      SL = SolidList;
  }

} // SplitSolid

static bool FUN_touched(const TopOpeBRepDS_DataStructure& BDS, const TopoDS_Edge& EOR)
{
  TopoDS_Vertex vf, vl;
  TopExp::Vertices(EOR, vf, vl);
  bool hvf = BDS.HasShape(vf);
  bool hvl = BDS.HasShape(vl);
  return (hvf || hvl);
}

//=================================================================================================

void TopOpeBRepBuild_Builder::SplitShapes(TopOpeBRepTool_ShapeExplorer& Ex,
                                          const TopAbs_State            ToBuild1,
                                          const TopAbs_State            ToBuild2,
                                          TopOpeBRepBuild_ShapeSet&     aSet,
                                          const bool                    RevOri)
{
  TopoDS_Shape       aShape;
  TopAbs_Orientation newori;

  for (; Ex.More(); Ex.Next())
  {
    aShape = Ex.Current();

    // compute new orientation <newori> to give to the new shapes
    newori = Orient(myBuildTool.Orientation(aShape), RevOri);

    TopAbs_ShapeEnum t = aShape.ShapeType();

    if (t == TopAbs_SOLID || t == TopAbs_SHELL)
      SplitSolid(aShape, ToBuild1, ToBuild2);
    else if (t == TopAbs_FACE)
      SplitFace(aShape, ToBuild1, ToBuild2);
    else if (t == TopAbs_EDGE)
      SplitEdge(aShape, ToBuild1, ToBuild2);
    else
      continue;

    if (IsSplit(aShape, ToBuild1))
    {
      TopoDS_Shape                             newShape;
      NCollection_List<TopoDS_Shape>::Iterator It;
      //----------------------- IFV
      bool IsLSon = false;
      //----------------------- IFV
      const NCollection_List<TopoDS_Shape>& LS = Splits(aShape, ToBuild1);
      //----------------------- IFV
      if (t == TopAbs_EDGE && ToBuild1 == TopAbs_IN && LS.Extent() == 0)
      {
        const NCollection_List<TopoDS_Shape>& LSon = Splits(aShape, TopAbs_ON);
        It.Initialize(LSon);
        IsLSon = true;
      }
      else
      {
        It.Initialize(LS);
      }
      //----------------------- IFV
      for (; It.More(); It.Next())
      {
        newShape = It.Value();
        myBuildTool.Orientation(newShape, newori);
#ifdef OCCT_DEBUG
//	TopAbs_ShapeEnum tns = TopType(newShape);
#endif
        //----------------------- IFV
        if (IsLSon)
        {
          bool add = true;
          if (!myListOfFace.IsEmpty())
          { // 2d pur
            add = KeepShape(newShape, myListOfFace, ToBuild1);
          }
          if (add)
            aSet.AddStartElement(newShape);
        }
        else
        {
          //----------------------- IFV
          aSet.AddStartElement(newShape);
        }
      }
    }
    else
    {
      // aShape n'a pas de devenir de split par ToBuild1
      // on construit les parties ToBuild1 de aShape (de S1)
      bool add      = true;
      bool testkeep = false;
      bool isedge   = (t == TopAbs_EDGE);
      bool hs       = (myDataStructure->HasShape(aShape));
      bool hg       = (myDataStructure->HasGeometry(aShape));

      testkeep = isedge && hs && (!hg);

      // xpu010399 : USA60299 (!hs)&&(!hg), but vertex on bound is touched (v7)
      //             -> testkeep
      bool istouched = isedge && (!hs) && (!hg);
      if (istouched)
        istouched = FUN_touched(myDataStructure->DS(), TopoDS::Edge(aShape));
      testkeep = testkeep || istouched;

      if (testkeep)
      {
        if (!myListOfFace.IsEmpty())
        { // 2d pur
          bool keep = KeepShape(aShape, myListOfFace, ToBuild1);
          add       = keep;
        }
        else
        { // 3d
          // on classifie en solide uniqt si
          // E dans la DS et E a ete purgee de ses interfs car en bout
          TopoDS_Shape sol;
          if (STATIC_SOLIDINDEX == 1)
            sol = myShape2;
          else
            sol = myShape1;
          if (!sol.IsNull())
          {
            double                  first, last;
            occ::handle<Geom_Curve> C3D;
            C3D = BRep_Tool::Curve(TopoDS::Edge(aShape), first, last);
            if (!C3D.IsNull())
            {
              double                      tt    = 0.127956477;
              double                      par   = (1 - tt) * first + tt * last;
              gp_Pnt                      P3D   = C3D->Value(par);
              double                      tol3d = Precision::Confusion();
              BRepClass3d_SolidClassifier SCL(sol, P3D, tol3d);
              TopAbs_State                state = SCL.State();
              add                               = (state == ToBuild1);
            }
            else
            {
              throw Standard_ProgramError("SplitShapes no 3D curve on edge");
              // NYI pas de courbe 3d : prendre un point sur (courbe 2d,face)
            }
          }
          else
          { //  sol.IsNull
            add = true;
          }
        }
      }
      if (add)
      {
        myBuildTool.Orientation(aShape, newori);
        aSet.AddElement(aShape);
      }
    }

  } // Ex.More

} // SplitShapes

//=================================================================================================

void TopOpeBRepBuild_Builder::FillShape(const TopoDS_Shape&                   S1,
                                        const TopAbs_State                    ToBuild1,
                                        const NCollection_List<TopoDS_Shape>& LS2,
                                        const TopAbs_State                    ToBuild2,
                                        TopOpeBRepBuild_ShapeSet&             aSet,
                                        const bool                            In_RevOri)
{
  bool             RevOri = In_RevOri;
  TopAbs_ShapeEnum t      = S1.ShapeType();
  TopAbs_ShapeEnum t1 = TopAbs_COMPOUND, t11 = TopAbs_COMPOUND;

  if (t == TopAbs_FACE)
  {
    t1  = TopAbs_WIRE;
    t11 = TopAbs_EDGE;
  }
  else if (t == TopAbs_SOLID || t == TopAbs_SHELL)
  {
    t1  = TopAbs_SHELL;
    t11 = TopAbs_FACE;
  }

  // if the shape S1 is a SameDomain one, get its orientation compared
  // with the shape taken as reference for all of the SameDomain shape of S1.
  bool hsd = myDataStructure->HasSameDomain(S1);
  if (hsd)
  {
    TopOpeBRepDS_Config ssc = myDataStructure->SameDomainOrientation(S1);
    if (ssc == TopOpeBRepDS_DIFFORIENTED)
    {
      RevOri = !RevOri;
#ifdef OCCT_DEBUG
//      int iFace = myDataStructure->Shape(S1);
//      std::cout<<std::endl<<"********** ";
//      std::cout<<"retournement d'orientation de ";TopAbs::Print(t,std::cout);
//      std::cout<<" "<<iFace<<std::endl;
#endif
    }
  }

  // work on a FORWARD shape <aShape>
  TopoDS_Shape aShape = S1;
  myBuildTool.Orientation(aShape, TopAbs_FORWARD);

  TopoDS_Shape       aSubShape;
  TopAbs_Orientation newori;

  // Explore the SubShapes of type <t1>
  for (TopOpeBRepTool_ShapeExplorer ex1(aShape, t1); ex1.More(); ex1.Next())
  {
    aSubShape = ex1.Current();

    if (!myDataStructure->HasShape(aSubShape))
    {
      // SubShape is not in DS : classify it with shapes of LS2
      bool keep = KeepShape(aSubShape, LS2, ToBuild1);
      if (keep)
      {
        newori = Orient(myBuildTool.Orientation(aSubShape), RevOri);
        myBuildTool.Orientation(aSubShape, newori);
        aSet.AddShape(aSubShape);
      }
    }
    else
    {
      // SubShape has geometry : split the <t11> SubShapes of the SubShape
      TopOpeBRepTool_ShapeExplorer ex11(aSubShape, t11);
      SplitShapes(ex11, ToBuild1, ToBuild2, aSet, RevOri);
    }
  } // exploration of SubShapes of type <t1> of shape <S1>

} // FillShape

//=================================================================================================

void TopOpeBRepBuild_Builder::FillFace(const TopoDS_Shape&                   F1,
                                       const TopAbs_State                    ToBuild1,
                                       const NCollection_List<TopoDS_Shape>& LF2,
                                       const TopAbs_State                    ToBuild2,
                                       TopOpeBRepBuild_WireEdgeSet&          WES,
                                       const bool                            RevOri)
{
#ifdef OCCT_DEBUG
  bool tSPF = TopOpeBRepBuild_GettraceSPF();
  //  int iFace = myDataStructure->Shape(F1);
  if (tSPF)
  {
    std::cout << std::endl;
  }
  if (tSPF)
  {
    GdumpSHASTA(F1, ToBuild1, "=-= FillFace ");
  }
#endif
  myListOfFace = LF2;
  FillShape(F1, ToBuild1, LF2, ToBuild2, WES, RevOri);
  myListOfFace.Clear();
} // FillFace

//=======================================================================
// function : FillSolid
// purpose  : load shells and faces from the solid in the ShellFaceSet <aSet>
//=======================================================================
void TopOpeBRepBuild_Builder::FillSolid(const TopoDS_Shape&                   S1,
                                        const TopAbs_State                    ToBuild1,
                                        const NCollection_List<TopoDS_Shape>& LS2,
                                        const TopAbs_State                    ToBuild2,
                                        TopOpeBRepBuild_ShapeSet&             aSet,
                                        const bool                            RevOri)
{
  FillShape(S1, ToBuild1, LS2, ToBuild2, aSet, RevOri);
} // FillSolid

//=================================================================================================

void TopOpeBRepBuild_Builder::FillVertexSet(TopOpeBRepDS_PointIterator& IT,
                                            const TopAbs_State          ToBuild,
                                            TopOpeBRepBuild_PaveSet&    PVS) const
{
  for (; IT.More(); IT.Next())
  {
    FillVertexSetOnValue(IT, ToBuild, PVS);
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::FillVertexSetOnValue(const TopOpeBRepDS_PointIterator& IT,
                                                   const TopAbs_State                ToBuild,
                                                   TopOpeBRepBuild_PaveSet&          PVS) const
{
  TopoDS_Shape V;

  // ind = index of new point or existing vertex
  int  ind     = IT.Current();
  bool ispoint = IT.IsPoint();
  //**!
  // if (ispoint) V = NewVertex(ind);
  if (ispoint && ind <= myDataStructure->NbPoints())
    V = NewVertex(ind);
  //**!
  else
    V = myDataStructure->Shape(ind);
  double             par = IT.Parameter();
  TopAbs_Orientation ori = IT.Orientation(ToBuild);

  bool keep = true;
  //    if (ori==TopAbs_EXTERNAL || ori==TopAbs_INTERNAL) keep = false;

  if (keep)
  {
    myBuildTool.Orientation(V, ori);
    occ::handle<TopOpeBRepBuild_Pave> PV = new TopOpeBRepBuild_Pave(V, par, false);
    PVS.Append(PV);
  }

#ifdef OCCT_DEBUG
  const TopoDS_Edge& EDEB = PVS.Edge();
  int                iE;
  bool               tSPS = GtraceSPS(EDEB, iE);
  if (tSPS)
  {
    if (keep)
      std::cout << "+";
    else
      std::cout << "-";
    if (ispoint)
      std::cout << " PDS ";
    else
      std::cout << " VDS ";
    std::cout << ind << " : ";
    GdumpORIPARPNT(ori, par, BRep_Tool::Pnt(TopoDS::Vertex(V)));
    std::cout << std::endl;
  }
#endif
}
