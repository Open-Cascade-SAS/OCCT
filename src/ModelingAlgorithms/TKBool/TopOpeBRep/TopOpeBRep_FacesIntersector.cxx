// Created on: 1993-11-18
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

#include <BRepTopAdaptor_TopolTool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_LineInter.hxx>


#include <IntPatch_LineConstructor.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <Precision.hxx>
#include <Geom_Curve.hxx>
#include <Standard_ProgramError.hxx>
#include <BRepTools.hxx>
#include <TopOpeBRepTool_tol.hxx>

Standard_EXPORT double GLOBAL_tolFF = 1.e-7;

#ifdef OCCT_DEBUG
  #include <TopAbs.hxx>
extern bool TopOpeBRep_GettraceFI();
extern bool TopOpeBRep_GettraceFITOL();
extern bool TopOpeBRep_GettraceSAVFF();

int SAVFFi1 = 0;
int SAVFFi2 = 0;

static void SAVFF(const TopoDS_Face& F1, const TopoDS_Face& F2)
{
  TCollection_AsciiString an1("SAVA");
  if (SAVFFi1)
    an1 = an1 + SAVFFi1;
  TCollection_AsciiString an2("SAVB");
  if (SAVFFi2)
    an2 = an2 + SAVFFi2;
  const char* n1 = an1.ToCString();
  const char* n2 = an2.ToCString();
  std::cout << "FaceIntersector : write " << n1 << "," << n2 << std::endl;
  BRepTools::Write(F1, n1);
  BRepTools::Write(F2, n2);
}

extern bool TopOpeBRepTool_GettraceKRO();
  #include <TopOpeBRepTool_KRO.hxx>
Standard_EXPORT TOPKRO KRO_DSFILLER_INTFF("intersection face/face");

#endif

// NYI
// NYI : IntPatch_Intersection : TolArc,TolTang exact definition
// NYI

// modified by NIZHNY-MKK  Mon Apr  2 12:14:32 2001.BEGIN
#include <IntPatch_WLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_Point.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>

static bool TestWLineAlongRestriction(const occ::handle<IntPatch_WLine>&      theWLine,
                                      const int                               theRank,
                                      const occ::handle<Adaptor3d_Surface>&   theSurface,
                                      const occ::handle<Adaptor3d_TopolTool>& theDomain,
                                      const double                            theTolArc);

static occ::handle<IntPatch_RLine> BuildRLineBasedOnWLine(
  const occ::handle<IntPatch_WLine>&    theWLine,
  const occ::handle<Adaptor2d_Curve2d>& theArc,
  const int                             theRank);

static occ::handle<IntPatch_RLine> BuildRLine(
  const NCollection_Sequence<occ::handle<IntPatch_Line>>& theSeqOfWLine,
  const int                                               theRank,
  const occ::handle<Adaptor3d_Surface>&                   theSurface,
  const occ::handle<Adaptor3d_TopolTool>&                 theDomain,
  const double                                            theTolArc);

static void TestWLinesToAnArc(NCollection_Sequence<occ::handle<IntPatch_Line>>& slin,
                              const occ::handle<Adaptor3d_Surface>&             theSurface1,
                              const occ::handle<Adaptor3d_TopolTool>&           theDomain1,
                              const occ::handle<Adaptor3d_Surface>&             theSurface2,
                              const occ::handle<Adaptor3d_TopolTool>&           theDomain2,
                              const double                                      theTolArc);
// modified by NIZHNY-MKK  Mon Apr  2 12:14:38 2001.END

// modified by NIZHNY-OFV  Fri Mar 29 12:37:21 2002.BEGIN
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>
static void MergeWLinesIfAllSegmentsAlongRestriction(
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
  const occ::handle<Adaptor3d_Surface>&             theSurface1,
  const occ::handle<Adaptor3d_TopolTool>&           theDomain1,
  const occ::handle<Adaptor3d_Surface>&             theSurface2,
  const occ::handle<Adaptor3d_TopolTool>&           theDomain2,
  const double                                      theTolArc);
//------------------------------------------------------------------------------------------------
static int GetArc(NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
                  const int&                                        theRankS,
                  const occ::handle<Adaptor3d_Surface>&             theSurfaceObj,
                  const occ::handle<Adaptor3d_TopolTool>&           theDomainObj,
                  const occ::handle<Adaptor3d_Surface>&             theSurfaceTool,
                  const gp_Pnt&                                     theTestPoint,
                  double&                                           theVrtxTol,
                  occ::handle<IntSurf_LineOn2S>&                    theLineOn2S,
                  double&                                           theFirst,
                  double&                                           theLast);
//------------------------------------------------------------------------------------------------
static bool IsPointOK(const gp_Pnt&            theTestPnt,
                      const Adaptor3d_Surface& theTestSurface,
                      const double&            theTol);
//-------------------------------------------------------------------------------------------------
static bool GetPointOn2S(const gp_Pnt&            theTestPnt,
                         const Adaptor3d_Surface& theTestSurface,
                         const double&            theTol,
                         Extrema_POnSurf&         theResultPoint);
//-------------------------------------------------------------------------------------------------------------------------
static occ::handle<IntPatch_WLine> GetMergedWLineOnRestriction(
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
  const double&                                     theVrtxTol,
  const occ::handle<IntSurf_LineOn2S>&              theLineOn2S);

//---------------------------------------------------------------------------------------------------------------------------
// modified by NIZHNY-OFV  Fri Mar 29 12:41:02 2002.END

//=================================================================================================

TopOpeBRep_FacesIntersector::TopOpeBRep_FacesIntersector()
{
  ResetIntersection();
  myTol1 = myTol2   = Precision::Confusion();
  myForceTolerances = false;
  mySurface1        = new BRepAdaptor_Surface();
  mySurface2        = new BRepAdaptor_Surface();
  myDomain1         = new BRepTopAdaptor_TopolTool();
  myDomain2         = new BRepTopAdaptor_TopolTool();
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::Perform(const TopoDS_Shape& F1,
                                          const TopoDS_Shape& F2,
                                          const Bnd_Box&      B1,
                                          const Bnd_Box&      B2)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceSAVFF())
    SAVFF(TopoDS::Face(F1), TopoDS::Face(F2));
#endif

  ResetIntersection();
  if (!myForceTolerances)
    ShapeTolerances(F1, F2);

  myFace1 = TopoDS::Face(F1);
  myFace1.Orientation(TopAbs_FORWARD);
  myFace2 = TopoDS::Face(F2);
  myFace2.Orientation(TopAbs_FORWARD);
  BRepAdaptor_Surface& S1 = *mySurface1;
  S1.Initialize(myFace1);
  BRepAdaptor_Surface& S2 = *mySurface2;
  S2.Initialize(myFace2);
  mySurfaceType1                               = S1.GetType();
  mySurfaceType2                               = S2.GetType();
  const occ::handle<Adaptor3d_Surface>& aSurf1 = mySurface1; // to avoid ambiguity
  myDomain1->Initialize(aSurf1);
  const occ::handle<Adaptor3d_Surface>& aSurf2 = mySurface2; // to avoid ambiguity
  myDomain2->Initialize(aSurf2);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_DSFILLER_INTFF.Start();
#endif

  double Deflection = 0.01, MaxUV = 0.01;
  if (!myForceTolerances)
  {
    FTOL_FaceTolerances3d(B1, B2, myFace1, myFace2, S1, S2, myTol1, myTol2, Deflection, MaxUV);
    myTol1 = (myTol1 > 1.e-4) ? 1.e-4 : myTol1;
    myTol2 = (myTol2 > 1.e-4) ? 1.e-4 : myTol2;
  }

  double tol1  = myTol1;
  double tol2  = myTol2;
  GLOBAL_tolFF = std::max(tol1, tol2);

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
  {
    std::cout << "FacesIntersector : Perform tol1 = " << tol1 << std::endl;
    std::cout << "                           tol2 = " << tol2 << std::endl;
    std::cout << "                           defl = " << Deflection << "  MaxUV = " << MaxUV
              << std::endl;
  }
#endif

  myIntersector.SetTolerances(myTol1, myTol2, MaxUV, Deflection);
  myIntersector
    .Perform(mySurface1, myDomain1, mySurface2, myDomain2, myTol1, myTol2, true, true, false);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO())
    KRO_DSFILLER_INTFF.Stop();
#endif

  // xpu180998 : cto900Q1
  bool done = myIntersector.IsDone();
  if (!done)
    return;

  PrepareLines();
  myIntersectionDone = true;

  // mySurfacesSameOriented : a mettre dans IntPatch NYI
  if (SameDomain())
  {
    mySurfacesSameOriented = TopOpeBRepTool_ShapeTool::SurfacesSameOriented(S1, S2);
  }

  // build the map of edges found as RESTRICTION
  for (InitLine(); MoreLine(); NextLine())
  {
    TopOpeBRep_LineInter& L = CurrentLine();
    if (L.TypeLineCurve() == TopOpeBRep_RESTRICTION)
    {
      const TopoDS_Shape& E = L.Arc();
      myEdgeRestrictionMap.Add(E);
    }
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFI())
    std::cout << "Perform : isempty " << IsEmpty() << std::endl;
#endif
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::Perform(const TopoDS_Shape& F1, const TopoDS_Shape& F2)
{
  Bnd_Box B1, B2;
  Perform(F1, F2, B1, B2);
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::IsEmpty()
{
  if (!myIntersectionDone)
    return false;
  bool done  = myIntersector.IsDone();
  bool empty = myIntersector.IsEmpty();
  if (!done || empty)
    return true;
  else
  {
    // ElemIntersector is done and is not empty
    // returns True if at least one VPoint is found
    empty = true;
    for (InitLine(); MoreLine(); NextLine())
    {
      empty = (CurrentLine().NbVPoint() == 0);
      if (!empty)
        break;
    }
    return empty;
  }
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::IsDone() const
{
  return myIntersectionDone;
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::SameDomain() const
{
  if (!myIntersectionDone)
    throw Standard_ProgramError("FacesIntersector : bad SameDomain");

  bool sd = myIntersector.TangentFaces();

  // bool plpl = (mySurfaceType1 == GeomAbs_Plane) && (mySurfaceType2 == GeomAbs_Plane);

  //  if (!plpl) return false;
  return sd;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRep_FacesIntersector::Face(const int Index) const
{
  if (Index == 1)
    return myFace1;
  else if (Index == 2)
    return myFace2;
  else
    throw Standard_ProgramError("TopOpeBRep_FacesIntersector::Face");
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::SurfacesSameOriented() const
{
  if (SameDomain())
  {
    return mySurfacesSameOriented;
  }
  throw Standard_ProgramError("FacesIntersector : bad SurfacesSameOriented");
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::IsRestriction(const TopoDS_Shape& E) const
{
  bool isrest = myEdgeRestrictionMap.Contains(E);
  return isrest;
}

//=================================================================================================

const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& TopOpeBRep_FacesIntersector::
  Restrictions() const
{
  return myEdgeRestrictionMap;
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::PrepareLines()
{
  myLineNb                = 0;
  int n                   = myIntersector.NbLines();
  myHAL                   = new NCollection_HArray1<TopOpeBRep_LineInter>(0, n);
  BRepAdaptor_Surface& S1 = *mySurface1;
  BRepAdaptor_Surface& S2 = *mySurface2;

  // modified by NIZHNY-MKK  Mon Apr  2 12:14:58 2001.BEGIN
  if (n == 0)
    return;
  // modified by NIZHNY-MKK  Mon Apr  2 12:15:09 2001.END

  bool newV = true;

  if (!newV)
  {
  /*for (  int i=1; i<=n; i++) {
    TopOpeBRep_LineInter& LI = myHAL->ChangeValue(i);
    const occ::handle<IntPatch_Line>& L = myIntersector.Line(i);
    LI.SetLine(L,S1,S2);
    LI.Index(i);
    myLineNb++;
  }*/}

  if (newV)
  {
    //-- lbr

    // modified by NIZHNY-MKK  Mon Apr  2 12:16:04 2001
    NCollection_Sequence<occ::handle<IntPatch_Line>> aSeqOfLines, aSeqOfResultLines;

    int i;
    //    int nbl=0;
    IntPatch_LineConstructor** Ptr =
      (IntPatch_LineConstructor**)malloc(n * sizeof(IntPatch_LineConstructor*));
    for (i = 1; i <= n; i++)
    {
      Ptr[i - 1] = new IntPatch_LineConstructor(2);
      Ptr[i - 1]->Perform(myIntersector.SequenceOfLine(),
                          myIntersector.Line(i),
                          mySurface1,
                          myDomain1,
                          mySurface2,
                          myDomain2,
                          myTol1);
      // modified by NIZHNY-MKK  Mon Apr  2 12:16:26 2001.BEGIN
      aSeqOfLines.Clear();
      for (int k = 1; k <= Ptr[i - 1]->NbLines(); k++)
      {
        aSeqOfLines.Append(Ptr[i - 1]->Line(k));
      }

      TestWLinesToAnArc(aSeqOfLines, mySurface1, myDomain1, mySurface2, myDomain2, myTol1);

      for (int j = 1; j <= aSeqOfLines.Length(); j++)
      {
        aSeqOfResultLines.Append(aSeqOfLines.Value(j));
      }
      delete Ptr[i - 1];
      //       nbl+=Ptr[i-1]->NbLines();
      // modified by NIZHNY-MKK  Mon Apr  2 12:16:31 2001.END
    }

    // modified by NIZHNY-MKK  Mon Apr  2 12:17:22 2001.BEGIN
    //     myHAL = new NCollection_HArray1<TopOpeBRep_LineInter>(0,nbl);
    myLineNb = aSeqOfResultLines.Length();

    // Fun_ConvertWLinesToRLine(aSeqOfResultLines,mySurface1, myDomain1, mySurface2, myDomain2,
    // myTol1);
    MergeWLinesIfAllSegmentsAlongRestriction(aSeqOfResultLines,
                                             mySurface1,
                                             myDomain1,
                                             mySurface2,
                                             myDomain2,
                                             myTol1);
    myLineNb = aSeqOfResultLines.Length();

    if (myLineNb > 0)
    {
      myHAL = new NCollection_HArray1<TopOpeBRep_LineInter>(1, myLineNb);
      for (int index = 1; index <= myLineNb; index++)
      {
        TopOpeBRep_LineInter&             LI = myHAL->ChangeValue(index);
        const occ::handle<IntPatch_Line>& L  = aSeqOfResultLines.Value(index);
        LI.SetLine(L, S1, S2);
        LI.Index(index);
      }
    }

    //     nbl=1;
    //     for(i=1;i<=n;i++) {
    //       for(int k=1;k<=Ptr[i-1]->NbLines();k++) {
    // 	TopOpeBRep_LineInter& LI = myHAL->ChangeValue(nbl);
    // 	const occ::handle<IntPatch_Line>& L = Ptr[i-1]->Line(k);
    // 	LI.SetLine(L,S1,S2);
    // 	LI.Index(nbl);
    // 	myLineNb++;
    // 	nbl++;
    //       }
    //       delete Ptr[i-1];
    //     }
    // modified by NIZHNY-MKK  Mon Apr  2 12:17:57 2001.END
    free(Ptr);
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<TopOpeBRep_LineInter>> TopOpeBRep_FacesIntersector::Lines()
{
  return myHAL;
}

//=================================================================================================

int TopOpeBRep_FacesIntersector::NbLines() const
{
  return myLineNb;
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::InitLine()
{
  myLineIndex = 1;
  FindLine();
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::FindLine()
{
  myLineFound = false;
  if (!myIntersectionDone)
    return;

  while (myLineIndex <= myLineNb)
  {
    const TopOpeBRep_LineInter& L = myHAL->Value(myLineIndex);
    myLineFound                   = L.OK();
    if (myLineFound)
      break;
    else
      myLineIndex++;
  }
}

//=================================================================================================

bool TopOpeBRep_FacesIntersector::MoreLine() const
{
  return myLineFound;
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::NextLine()
{
  myLineIndex++;
  FindLine();
}

//=================================================================================================

TopOpeBRep_LineInter& TopOpeBRep_FacesIntersector::CurrentLine()
{
  TopOpeBRep_LineInter& TLI = myHAL->ChangeValue(myLineIndex);
  return TLI;
}

//=================================================================================================

int TopOpeBRep_FacesIntersector::CurrentLineIndex() const
{
  return myLineIndex;
}

//=================================================================================================

TopOpeBRep_LineInter& TopOpeBRep_FacesIntersector::ChangeLine(const int IL)
{
  TopOpeBRep_LineInter& TLI = myHAL->ChangeValue(IL);
  return TLI;
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::ResetIntersection()
{
  myIntersectionDone = false;
  myLineIndex        = 1;
  myLineNb           = 0;
  myEdgeRestrictionMap.Clear();
  myLineFound = false;
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::ForceTolerances(const double Tol1, const double Tol2)
{
  myTol1            = Tol1;
  myTol2            = Tol2;
  myForceTolerances = true;
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
    std::cout << "ForceTolerances : myTol1,myTol2 = " << myTol1 << "," << myTol2 << std::endl;
#endif
}

//=================================================================================================

void TopOpeBRep_FacesIntersector::GetTolerances(double& Tol1, double& Tol2) const
{
  Tol1 = myTol1;
  Tol2 = myTol2;
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRep_FacesIntersector::ShapeTolerances(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
#else
void TopOpeBRep_FacesIntersector::ShapeTolerances(const TopoDS_Shape&, const TopoDS_Shape&)
#endif
{
  //  myTol1 = std::max(ToleranceMax(S1,TopAbs_EDGE),ToleranceMax(S2,TopAbs_EDGE));
  myTol1            = Precision::Confusion();
  myTol2            = myTol1;
  myForceTolerances = false;
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
  {
    std::cout << "ShapeTolerances on S1 = ";
    TopAbs::Print(S1.ShapeType(), std::cout);
    std::cout << " S2 = ";
    TopAbs::Print(S2.ShapeType(), std::cout);
    std::cout << " : myTol1,myTol2 = " << myTol1 << "," << myTol2 << std::endl;
  }
#endif
}

//=================================================================================================

double TopOpeBRep_FacesIntersector::ToleranceMax(const TopoDS_Shape&    S,
                                                 const TopAbs_ShapeEnum T) const
{
  TopExp_Explorer e(S, T);
  if (!e.More())
    return Precision::Intersection();
  else
  {
    double tol = RealFirst();
    for (; e.More(); e.Next())
      tol = std::max(tol, TopOpeBRepTool_ShapeTool::Tolerance(e.Current()));
    return tol;
  }
}

// modified by NIZHNY-MKK  Mon Apr  2 12:18:30 2001.BEGIN
// ================================================================================================
// static function: TestWLineAlongRestriction
// purpose:
// ================================================================================================
static bool TestWLineAlongRestriction(const occ::handle<IntPatch_WLine>&      theWLine,
                                      const int                               theRank,
                                      const occ::handle<Adaptor3d_Surface>&   theSurface,
                                      const occ::handle<Adaptor3d_TopolTool>& theDomain,
                                      const double                            theTolArc)
{

  bool result = false;
  int  NbPnts = theWLine->NbPnts();
  int  along  = 0;

  for (int i = 1; i <= NbPnts; i++)
  {
    const IntSurf_PntOn2S& Pmid = theWLine->Point(i);
    double                 u = 0., v = 0.;
    if (theRank == 1)
      Pmid.ParametersOnS1(u, v);
    else
      Pmid.ParametersOnS2(u, v);
    //------------------------------------------
    gp_Pnt ap;
    gp_Vec ad1u, ad1v;
    // double nad1u, nad1v, tolu, tolv;

    theSurface->D1(u, v, ap, ad1u, ad1v);
    // nad1u=ad1u.Magnitude();
    // nad1v=ad1v.Magnitude();
    // if(nad1u>1e-12) tolu=theTolArc/nad1u; else tolu=0.1;
    // if(nad1v>1e-12) tolv=theTolArc/nad1v; else tolv=0.1;
    // if(tolu>tolv)  tolu=tolv;
    //------------------------------------------

    // if(theDomain->IsThePointOn(gp_Pnt2d(u, v),tolu)) {
    //   along++;
    // }

    if (theDomain->IsThePointOn(gp_Pnt2d(u, v), theTolArc))
      along++;
    // if(along!=i) break;
  }
  if (along == NbPnts)
    result = true;
  return result;
}

// ================================================================================================
// static function: BuildRLineBasedOnWLine
// purpose:
// ================================================================================================
static occ::handle<IntPatch_RLine> BuildRLineBasedOnWLine(
  const occ::handle<IntPatch_WLine>&    theWLine,
  const occ::handle<Adaptor2d_Curve2d>& theArc,
  const int                             theRank)
{
  occ::handle<IntPatch_RLine> anRLine;

  if ((theRank != 1) && (theRank != 2))
    return anRLine;

  gp_Pnt2d              aPOnLine;
  double                u = 0., v = 0.;
  int                   nbvtx = theWLine->NbVertex();
  const IntPatch_Point& Vtx1  = theWLine->Vertex(1);
  const IntPatch_Point& Vtx2  = theWLine->Vertex(nbvtx);

  if (theRank == 1)
  {
    Vtx1.ParametersOnS1(u, v);
  }
  else
  {
    Vtx1.ParametersOnS2(u, v);
  }

  aPOnLine    = gp_Pnt2d(u, v);
  double par1 = Geom2dInt_TheProjPCurOfGInter::FindParameter(*theArc, aPOnLine, 1.e-7);

  if (theRank == 1)
  {
    Vtx2.ParametersOnS1(u, v);
  }
  else
  {
    Vtx2.ParametersOnS2(u, v);
  }
  aPOnLine    = gp_Pnt2d(u, v);
  double par2 = Geom2dInt_TheProjPCurOfGInter::FindParameter(*theArc, aPOnLine, 1.e-7);

  double tol = (Vtx1.Tolerance() > Vtx2.Tolerance()) ? Vtx1.Tolerance() : Vtx2.Tolerance();

  if (std::abs(par1 - par2) < theArc->Resolution(tol))
    return anRLine;

  bool IsOnFirst = (theRank == 1);

  occ::handle<IntSurf_LineOn2S>        aLineOn2S = new IntSurf_LineOn2S();
  const occ::handle<IntSurf_LineOn2S>& Lori      = theWLine->Curve();
  IntSurf_Transition                   TransitionUndecided;

  anRLine = new IntPatch_RLine(false, theWLine->TransitionOnS1(), theWLine->TransitionOnS2());

  if (IsOnFirst)
  {
    anRLine->SetArcOnS1(theArc);
  }
  else
  {
    anRLine->SetArcOnS2(theArc);
  }

  int k = 0;
  if (par1 < par2)
  {

    for (k = 1; k <= Lori->NbPoints(); k++)
    {
      aLineOn2S->Add(Lori->Value(k));
    }
    anRLine->Add(aLineOn2S);
    IntPatch_Point VtxFirst = Vtx1;

    VtxFirst.SetArc(IsOnFirst, //-- On First
                    theArc,
                    par1,
                    TransitionUndecided,
                    TransitionUndecided);
    VtxFirst.SetParameter(par1);
    anRLine->AddVertex(VtxFirst);

    for (k = 2; k < nbvtx; k++)
    {
      IntPatch_Point Vtx = theWLine->Vertex(k);
      if (theRank == 1)
      {
        Vtx.ParametersOnS1(u, v);
      }
      else
      {
        Vtx.ParametersOnS2(u, v);
      }
      gp_Pnt2d atmpPoint(u, v);
      double   apar = Geom2dInt_TheProjPCurOfGInter::FindParameter(*theArc, atmpPoint, 1.e-7);
      Vtx.SetParameter(apar);
      anRLine->AddVertex(Vtx);
    }

    IntPatch_Point VtxLast = Vtx2;
    VtxLast.SetArc(IsOnFirst, //-- On First
                   theArc,
                   par2,
                   TransitionUndecided,
                   TransitionUndecided);
    VtxLast.SetParameter(par2);
    anRLine->AddVertex(VtxLast);
    anRLine->SetFirstPoint(1);
    anRLine->SetLastPoint(nbvtx);
    anRLine->ComputeVertexParameters(Precision::Confusion());
  }
  else
  {

    for (k = Lori->NbPoints(); k >= 1; k--)
    {
      aLineOn2S->Add(Lori->Value(k));
    }
    anRLine->Add(aLineOn2S);

    IntPatch_Point VtxFirst = Vtx2;
    VtxFirst.SetArc(IsOnFirst, //-- On First
                    theArc,
                    par2,
                    TransitionUndecided,
                    TransitionUndecided);
    VtxFirst.SetParameter(par2);
    anRLine->AddVertex(VtxFirst);

    for (k = nbvtx - 1; k >= 2; k--)
    {
      IntPatch_Point Vtx = theWLine->Vertex(k);
      Vtx.ReverseTransition();
      if (theRank == 1)
      {
        Vtx.ParametersOnS1(u, v);
      }
      else
      {
        Vtx.ParametersOnS2(u, v);
      }
      gp_Pnt2d atmpPoint(u, v);
      double   apar = Geom2dInt_TheProjPCurOfGInter::FindParameter(*theArc, atmpPoint, 1.e-7);
      Vtx.SetParameter(apar);
      anRLine->AddVertex(Vtx);
    }
    IntPatch_Point VtxLast = Vtx1;
    VtxLast.SetArc(IsOnFirst, //-- On First
                   theArc,
                   par1,
                   TransitionUndecided,
                   TransitionUndecided);
    VtxLast.SetParameter(par1);
    anRLine->AddVertex(VtxLast);
    anRLine->SetFirstPoint(1);
    anRLine->SetLastPoint(nbvtx);
    anRLine->ComputeVertexParameters(Precision::Confusion());
  }

  return anRLine;
}

// ================================================================================================
// static function: BuildRLine
// purpose: build rline based on group of wlines
//          return null handle if it is not possible to build rline
// ================================================================================================
static occ::handle<IntPatch_RLine> BuildRLine(
  const NCollection_Sequence<occ::handle<IntPatch_Line>>& theSeqOfWLine,
  const int                                               theRank,
  const occ::handle<Adaptor3d_Surface>&                   theSurface,
  const occ::handle<Adaptor3d_TopolTool>&                 theDomain,
  const double                                            theTolArc)
{
  occ::handle<IntPatch_RLine>        anRLine;
  const occ::handle<IntPatch_WLine>& aWLine1 =
    *((occ::handle<IntPatch_WLine>*)&(theSeqOfWLine.Value(1)));
  const occ::handle<IntPatch_WLine>& aWLine2 =
    *((occ::handle<IntPatch_WLine>*)&(theSeqOfWLine.Value(theSeqOfWLine.Length())));
  const IntPatch_Point&                 P1  = aWLine1->Vertex(1);
  const IntPatch_Point&                 P2  = aWLine2->Vertex(aWLine2->NbVertex());
  const occ::handle<Adaptor3d_HVertex>& aV1 = (theRank == 1) ? P1.VertexOnS1() : P1.VertexOnS2();
  const occ::handle<Adaptor3d_HVertex>& aV2 = (theRank == 1) ? P2.VertexOnS1() : P2.VertexOnS2();

  // avoid closed and degenerated edges
  if (aV1->IsSame(aV2))
    return anRLine;

  for (theDomain->Init(); theDomain->More(); theDomain->Next())
  {
    theDomain->Initialize(theDomain->Value());
    bool foundVertex1 = false;
    bool foundVertex2 = false;

    for (theDomain->InitVertexIterator();
         (!foundVertex1 || !foundVertex2) && theDomain->MoreVertex();
         theDomain->NextVertex())
    {

      if (!foundVertex1 && aV1->IsSame(theDomain->Vertex()))
        foundVertex1 = true;
      if (!foundVertex2 && aV2->IsSame(theDomain->Vertex()))
        foundVertex2 = true;
    }

    if (foundVertex1 && foundVertex2)
    {
      bool buildrline = (theSeqOfWLine.Length() > 0);

      for (int i = 1; buildrline && i <= theSeqOfWLine.Length(); i++)
      {
        const occ::handle<IntPatch_WLine>& aWLine =
          *((occ::handle<IntPatch_WLine>*)&(theSeqOfWLine.Value(i)));

        int indexpnt = aWLine->NbPnts() / 2;
        if (indexpnt < 1)
          buildrline = false;
        else
        {
          double                 u = RealLast(), v = RealLast();
          const IntSurf_PntOn2S& POn2S = aWLine->Point(indexpnt);
          if (theRank == 1)
          {
            POn2S.ParametersOnS1(u, v);
          }
          else
          {
            POn2S.ParametersOnS2(u, v);
          }
          gp_Pnt2d aPOnArc, aPOnLine(u, v);
          double   par =
            Geom2dInt_TheProjPCurOfGInter::FindParameter(*theDomain->Value(), aPOnLine, 1e-7);
          aPOnArc = theDomain->Value()->Value(par);
          gp_Pnt ap;
          gp_Vec ad1u, ad1v;
          double nad1u, nad1v, tolu, tolv;

          theSurface->D1(u, v, ap, ad1u, ad1v);
          nad1u = ad1u.Magnitude();
          nad1v = ad1v.Magnitude();
          if (nad1u > 1e-12)
            tolu = theTolArc / nad1u;
          else
            tolu = 0.1;
          if (nad1v > 1e-12)
            tolv = theTolArc / nad1v;
          else
            tolv = 0.1;
          double aTolerance = (tolu > tolv) ? tolv : tolu;

          if (aPOnArc.Distance(aPOnLine) > aTolerance)
          {
            buildrline = false;
          }
        }
      } // end for

      if (buildrline)
      {
        IntSurf_TypeTrans trans1 = IntSurf_Undecided, trans2 = IntSurf_Undecided;

        occ::handle<IntSurf_LineOn2S> aLineOn2S = new IntSurf_LineOn2S();

        for (int j = 1; j <= theSeqOfWLine.Length(); j++)
        {
          const occ::handle<IntPatch_WLine>& atmpWLine =
            *((occ::handle<IntPatch_WLine>*)&(theSeqOfWLine.Value(j)));

          const occ::handle<IntSurf_LineOn2S>& Lori = atmpWLine->Curve();

          if (atmpWLine->TransitionOnS1() != IntSurf_Undecided
              && atmpWLine->TransitionOnS1() != IntSurf_Touch)
          {
            trans1 = atmpWLine->TransitionOnS1();
          }
          if (atmpWLine->TransitionOnS2() != IntSurf_Undecided
              && atmpWLine->TransitionOnS2() != IntSurf_Touch)
          {
            trans2 = atmpWLine->TransitionOnS2();
          }

          int ParamMinOnLine = (int)atmpWLine->Vertex(1).ParameterOnLine();
          int ParamMaxOnLine = (int)atmpWLine->Vertex(atmpWLine->NbVertex()).ParameterOnLine();

          for (int k = ParamMinOnLine; k <= ParamMaxOnLine; k++)
          {
            aLineOn2S->Add(Lori->Value(k));
          }
        }

        occ::handle<IntPatch_WLine> emulatedWLine =
          new IntPatch_WLine(aLineOn2S, false, trans1, trans2);

        IntPatch_Point aFirstVertex = P1;
        IntPatch_Point aLastVertex  = P2;
        aFirstVertex.SetParameter(1);
        aLastVertex.SetParameter(aLineOn2S->NbPoints());

        emulatedWLine->AddVertex(aFirstVertex);
        int apointindex = 0;

        for (apointindex = 2; apointindex <= aWLine1->NbVertex(); apointindex++)
        {
          IntPatch_Point aPoint = aWLine1->Vertex(apointindex);
          double         aTolerance =
            (aPoint.Tolerance() > P1.Tolerance()) ? aPoint.Tolerance() : P1.Tolerance();
          if (aPoint.Value().IsEqual(P1.Value(), aTolerance))
          {
            aPoint.SetParameter(1);
            emulatedWLine->AddVertex(aPoint);
          }
        }

        for (apointindex = 1; apointindex < aWLine2->NbVertex(); apointindex++)
        {
          IntPatch_Point aPoint = aWLine2->Vertex(apointindex);
          double         aTolerance =
            (aPoint.Tolerance() > P2.Tolerance()) ? aPoint.Tolerance() : P2.Tolerance();
          if (aPoint.Value().IsEqual(P2.Value(), aTolerance))
          {
            aPoint.SetParameter(aLineOn2S->NbPoints());
            emulatedWLine->AddVertex(aPoint);
          }
        }

        emulatedWLine->AddVertex(aLastVertex);

        anRLine = BuildRLineBasedOnWLine(emulatedWLine, theDomain->Value(), theRank);

        break;
      }
    }
  } // end for

  return anRLine;
}

// ================================================================================================
// static function: TestWLinesToAnArc
// purpose: test if possible to replace group of wlines by rline and replace in the sequence slin
// ================================================================================================
static void TestWLinesToAnArc(NCollection_Sequence<occ::handle<IntPatch_Line>>& slin,
                              const occ::handle<Adaptor3d_Surface>&             theSurface1,
                              const occ::handle<Adaptor3d_TopolTool>&           theDomain1,
                              const occ::handle<Adaptor3d_Surface>&             theSurface2,
                              const occ::handle<Adaptor3d_TopolTool>&           theDomain2,
                              const double                                      theTolArc)
{

  NCollection_Sequence<occ::handle<IntPatch_Line>> aSeqOfWLine;
  NCollection_Sequence<occ::handle<IntPatch_Line>> aSeqOfRLine;
  for (int rank = 1; rank <= 2; rank++)
  {
    for (int i = 1; i <= slin.Length(); i++)
    {
      if (slin.Value(i)->ArcType() != IntPatch_Walking)
        continue;
      const occ::handle<IntPatch_WLine>& aWLine = *((occ::handle<IntPatch_WLine>*)&(slin.Value(i)));
      int                                nbvtx  = aWLine->NbVertex();
      const IntPatch_Point&              Vtx1   = aWLine->Vertex(1);
      const IntPatch_Point&              Vtx2   = aWLine->Vertex(nbvtx);
      bool                               isvertex = false, wlineWasAppended = false;

      if (rank == 1)
        isvertex = Vtx1.IsVertexOnS1();
      else
        isvertex = Vtx1.IsVertexOnS2();

      if (isvertex)
      {
        bool appendwline = true;
        if (rank == 1)
        {
          if (!aWLine->HasArcOnS1()
              && !TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc))
          {
            appendwline = false;
          }
        }
        if (rank == 2)
        {
          if (!aWLine->HasArcOnS2()
              && !TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc))
          {
            appendwline = false;
          }
        }
        wlineWasAppended = appendwline;
        if (appendwline)
          aSeqOfWLine.Append(aWLine);
      }
      else
      {
        if (aSeqOfWLine.Length() == 0)
          continue;
        const occ::handle<IntPatch_WLine>& aLastWLine =
          *((occ::handle<IntPatch_WLine>*)&(aSeqOfWLine.Value(aSeqOfWLine.Length())));
        const IntPatch_Point& aLastPoint = aLastWLine->Vertex(aLastWLine->NbVertex());
        double                aTolerance =
          (aLastPoint.Tolerance() > Vtx1.Tolerance()) ? aLastPoint.Tolerance() : Vtx1.Tolerance();
        if (aLastPoint.Value().IsEqual(Vtx1.Value(), aTolerance))
        {
          bool appendwline = true;
          if (rank == 1)
          {
            if (!aWLine->HasArcOnS1()
                && !TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc))
            {
              appendwline = false;
            }
          }
          if (rank == 2)
          {
            if (!aWLine->HasArcOnS2()
                && !TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc))
            {
              appendwline = false;
            }
          }
          wlineWasAppended = appendwline;
          if (appendwline)
            aSeqOfWLine.Append(aWLine);
        }
        else
        {
          aSeqOfWLine.Clear();
        }
      }

      isvertex = false;
      if (rank == 1)
        isvertex = Vtx2.IsVertexOnS1();
      else
        isvertex = Vtx2.IsVertexOnS2();

      if (wlineWasAppended && isvertex)
      {
        // build rline based on sequence of wlines.
        occ::handle<IntPatch_RLine> anRLine;
        if (rank == 1)
        {
          anRLine = BuildRLine(aSeqOfWLine, rank, theSurface1, theDomain1, theTolArc);
        }
        else
        {
          anRLine = BuildRLine(aSeqOfWLine, rank, theSurface2, theDomain2, theTolArc);
        }

        if (!anRLine.IsNull())
        {
          aSeqOfRLine.Append(anRLine);
          for (int k = 1; k <= aSeqOfWLine.Length(); k++)
          {
            for (int j = 1; j <= slin.Length(); j++)
            {
              if (aSeqOfWLine(k) == slin(j))
              {
                slin.Remove(j);
                break;
              }
            }
          }
        }
        aSeqOfWLine.Clear();
      }
    }
  }

  for (int i = 1; i <= aSeqOfRLine.Length(); i++)
  {
    slin.Append(aSeqOfRLine.Value(i));
  }
}

// modified by NIZHNY-MKK  Mon Apr  2 12:18:34 2001.END

//====================================================================================
// function: MergeWLinesIfAllSegmentsAlongRestriction
//
//  purpose: If the result of LineConstructor is a set of WLines segments which are
//           placed along RESTRICTION, we can suppose that this result is not correct:
//           here we should have a RLine. If it is not possible to construct RLine
//           we should merge segments of WLines into single WLine equals to the same
//           RLine.
//====================================================================================
static void MergeWLinesIfAllSegmentsAlongRestriction(
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
  const occ::handle<Adaptor3d_Surface>&             theSurface1,
  const occ::handle<Adaptor3d_TopolTool>&           theDomain1,
  const occ::handle<Adaptor3d_Surface>&             theSurface2,
  const occ::handle<Adaptor3d_TopolTool>&           theDomain2,
  const double                                      theTolArc)
{
  int    i = 0, rank = 0;
  double tol = 1.e-9;

  // here we check that all segments of WLines placed along restriction
  int                          WLOnRS1  = 0;
  int                          WLOnRS2  = 0;
  int                          NbWLines = 0;
  NCollection_Sequence<gp_Pnt> sqVertexPoints;

  for (rank = 1; rank <= 2; rank++)
  {
    NbWLines = 0;
    for (i = 1; i <= theSlin.Length(); i++)
    {
      if (theSlin.Value(i)->ArcType() != IntPatch_Walking)
        continue;
      NbWLines++;
      const occ::handle<IntPatch_WLine>& aWLine =
        *((occ::handle<IntPatch_WLine>*)&(theSlin.Value(i)));
      int                   nbvtx = aWLine->NbVertex();
      const IntPatch_Point& Vtx1  = aWLine->Vertex(1);
      const IntPatch_Point& Vtx2  = aWLine->Vertex(nbvtx);
      if (rank == 1)
      {
        sqVertexPoints.Append(Vtx1.Value());
        sqVertexPoints.Append(Vtx2.Value());
        if (TestWLineAlongRestriction(aWLine, rank, theSurface1, theDomain1, theTolArc))
          WLOnRS1++;
      }
      else
      {
        if (TestWLineAlongRestriction(aWLine, rank, theSurface2, theDomain2, theTolArc))
          WLOnRS2++;
      }
    }
    if (NbWLines == WLOnRS1 || NbWLines == WLOnRS2)
      break;
  }

  int WLineRank = 0; // not possible to merge WLines

  if (WLOnRS1 == NbWLines)
    WLineRank = 1; // create merged WLine based on arc of S1
  else if (WLOnRS2 == NbWLines)
    WLineRank = 2; // create merged WLine based on arc of S2
  else
    return;

  // avoid closed (degenerated) edges
  if (sqVertexPoints.Length() <= 2)
    return;
  if (sqVertexPoints.Value(1).IsEqual(sqVertexPoints.Value(sqVertexPoints.Length()), tol))
    return;

  double TolVrtx        = 1.e-5;
  int    testPointIndex = (sqVertexPoints.Length() > 3) ? ((int)sqVertexPoints.Length() / 2) : 2;
  gp_Pnt testPoint      = sqVertexPoints.Value(testPointIndex);
  double Fp = 0., Lp = 0.;

  occ::handle<IntSurf_LineOn2S> aLineOn2S = new IntSurf_LineOn2S();
  //
  int arcnumber = (WLineRank == 1) ? GetArc(theSlin,
                                            WLineRank,
                                            theSurface1,
                                            theDomain1,
                                            theSurface2,
                                            testPoint,
                                            TolVrtx,
                                            aLineOn2S,
                                            Fp,
                                            Lp)
                                   : GetArc(theSlin,
                                            WLineRank,
                                            theSurface2,
                                            theDomain2,
                                            theSurface1,
                                            testPoint,
                                            TolVrtx,
                                            aLineOn2S,
                                            Fp,
                                            Lp);
  //
  if (arcnumber == 0)
  {
    return;
  }
  //
  occ::handle<IntPatch_WLine> anWLine = GetMergedWLineOnRestriction(theSlin, TolVrtx, aLineOn2S);
  if (!anWLine.IsNull())
  {
    theSlin.Clear();
    theSlin.Append(anWLine);
#ifdef OCCT_DEBUG
    std::cout << "*** TopOpeBRep_FaceIntersector: Merge WLines on Restriction "
              << ((WLineRank == 1) ? "S1" : "S2") << " to WLine***" << std::endl;
#endif
  }
}

//=========================================================================================
// function: GetArc
//
//  purpose: Define arc on (OBJ) surface which all WLine segments are placed along.
//           Check states of points in the gaps between segments on (TOOL). If those states
//           are IN or ON return the LineOn2S based on points3D were given from detected arc.
//           Returns 0 if it is not possible to create merged WLine.
//========================================================================================
static int GetArc(NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
                  const int&                                        theRankS,
                  const occ::handle<Adaptor3d_Surface>&             theSurfaceObj,
                  const occ::handle<Adaptor3d_TopolTool>&           theDomainObj,
                  const occ::handle<Adaptor3d_Surface>&             theSurfaceTool,
                  const gp_Pnt&                                     theTestPoint,
                  double&                                           theVrtxTol,
                  occ::handle<IntSurf_LineOn2S>&                    theLineOn2S,
                  double&                                           theFirst,
                  double&                                           theLast)
{
  // 1. find number of arc (edge) on which the WLine segments are placed.

  double MinDistance2 = 1.e+200, firstES1 = 0., lastES1 = 0.;
  int    ArcNumber = 0, CurArc = 0, i = 0, j = 0;
  theFirst = 0.;
  theLast  = 0.;

  for (theDomainObj->Init(); theDomainObj->More(); theDomainObj->Next())
  {
    CurArc++;
    void* anEAddress = theDomainObj->Edge();

    if (anEAddress == nullptr)
      continue;

    TopoDS_Edge*            anE    = (TopoDS_Edge*)anEAddress;
    occ::handle<Geom_Curve> aCEdge = BRep_Tool::Curve(*anE, firstES1, lastES1);
    if (aCEdge.IsNull()) // e.g. degenerated edge, see OCC21770
      continue;
    GeomAdaptor_Curve CE;
    CE.Load(aCEdge);
    Extrema_ExtPC epc(theTestPoint, CE, 1.e-7);

    if (epc.IsDone())
    {
      for (i = 1; i <= epc.NbExt(); i++)
      {
        if (epc.SquareDistance(i) < MinDistance2)
        {
          MinDistance2 = epc.SquareDistance(i);
          ArcNumber    = CurArc;
        }
      }
    }
  }

  if (ArcNumber == 0)
    return 0;

  // 2. load parameters of founded edge and its arc.
  CurArc = 0;
  NCollection_Sequence<gp_Pnt>   PointsFromArc;
  occ::handle<Adaptor2d_Curve2d> arc = nullptr;
  double                         tol = 1.e-7;
  NCollection_Sequence<double>   WLVertexParameters;
  bool                           classifyOK = true;
  double                         CheckTol   = 1.e-5;

  for (theDomainObj->Init(); theDomainObj->More(); theDomainObj->Next())
  {
    CurArc++;
    if (CurArc != ArcNumber)
      continue;

    arc = theDomainObj->Value();

    for (i = 1; i <= theSlin.Length(); i++)
    {
      if (theSlin.Value(i)->ArcType() != IntPatch_Walking)
        continue;

      const occ::handle<IntPatch_WLine>& aWLine =
        *((occ::handle<IntPatch_WLine>*)&(theSlin.Value(i)));

      int                    nbpnts  = aWLine->NbPnts();
      const IntSurf_PntOn2S& POn2S_F = aWLine->Point(1);
      const IntSurf_PntOn2S& POn2S_L = aWLine->Point(nbpnts);

      double Upf = 0., Vpf = 0., Upl = 0., Vpl = 0.;

      if (theRankS == 1)
      {
        POn2S_F.ParametersOnS1(Upf, Vpf);
        POn2S_L.ParametersOnS1(Upl, Vpl);
      }
      else
      {
        POn2S_F.ParametersOnS2(Upf, Vpf);
        POn2S_L.ParametersOnS2(Upl, Vpl);
      }

      gp_Pnt2d aPOnLine_F(Upf, Vpf);
      gp_Pnt2d aPOnLine_L(Upl, Vpl);

      double par_F = Geom2dInt_TheProjPCurOfGInter::FindParameter(*arc, aPOnLine_F, tol);
      double par_L = Geom2dInt_TheProjPCurOfGInter::FindParameter(*arc, aPOnLine_L, tol);

      WLVertexParameters.Append(par_F);
      WLVertexParameters.Append(par_L);
    }

    for (i = 1; i <= WLVertexParameters.Length(); i++)
    {
      for (j = i; j <= WLVertexParameters.Length(); j++)
      {
        if (j == i)
          continue;

        if (WLVertexParameters.Value(i) > WLVertexParameters.Value(j))
        {
          double pol = WLVertexParameters.Value(i);
          WLVertexParameters.SetValue(i, WLVertexParameters.Value(j));
          WLVertexParameters.SetValue(j, pol);
        }
      }
    }

    void*         anEAddress = theDomainObj->Edge();
    TopoDS_Edge*  anE        = (TopoDS_Edge*)anEAddress;
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(*anE, V1, V2);
    double MaxVertexTol            = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
    theVrtxTol                     = MaxVertexTol;
    double EdgeTol                 = BRep_Tool::Tolerance(*anE);
    CheckTol                       = std::max(MaxVertexTol, EdgeTol);
    occ::handle<Geom_Curve> aCEdge = BRep_Tool::Curve(*anE, firstES1, lastES1);
    // classification gaps
    //  a. min - first
    if (std::abs(firstES1 - WLVertexParameters.Value(1)) > arc->Resolution(MaxVertexTol))
    {
      double param = (firstES1 + WLVertexParameters.Value(1)) / 2.;
      gp_Pnt point;
      aCEdge->D0(param, point);
      if (!IsPointOK(point, *theSurfaceTool, CheckTol))
      {
        classifyOK = false;
        break;
      }
    }
    //  b. max - last
    if (std::abs(lastES1 - WLVertexParameters.Value(WLVertexParameters.Length()))
        > arc->Resolution(MaxVertexTol))
    {
      double param = (lastES1 + WLVertexParameters.Value(WLVertexParameters.Length())) / 2.;
      gp_Pnt point;
      aCEdge->D0(param, point);
      if (!IsPointOK(point, *theSurfaceTool, CheckTol))
      {
        classifyOK = false;
        break;
      }
    }
    //  c. all middle gaps
    int NbChkPnts = WLVertexParameters.Length() / 2 - 1;
    for (i = 1; i <= NbChkPnts; i++)
    {
      if (std::abs(WLVertexParameters.Value(i * 2 + 1) - WLVertexParameters.Value(i * 2))
          > arc->Resolution(MaxVertexTol))
      {
        double param = (WLVertexParameters.Value(i * 2) + WLVertexParameters.Value(i * 2 + 1)) / 2.;
        gp_Pnt point;
        aCEdge->D0(param, point);
        if (!IsPointOK(point, *theSurfaceTool, CheckTol))
        {
          classifyOK = false;
          break;
        }
      }
    }

    if (!classifyOK)
      break;

    // if classification gaps OK, fill sequence by the points from arc (edge)
    double ParamFirst = WLVertexParameters.Value(1);
    double ParamLast  = WLVertexParameters.Value(WLVertexParameters.Length());
    double dParam     = std::abs(ParamLast - ParamFirst) / 100.;
    double cParam     = ParamFirst;
    for (i = 0; i < 100; i++)
    {
      if (i == 99)
        cParam = ParamLast;

      gp_Pnt cPnt;
      aCEdge->D0(cParam, cPnt);
      PointsFromArc.Append(cPnt);
      cParam += dParam;
    }
    theFirst = ParamFirst;
    theLast  = ParamLast;
  }

  if (!classifyOK)
    return 0;

  // create IntSurf_LineOn2S from points < PointsFromArc >
  for (i = 1; i <= PointsFromArc.Length(); i++)
  {
    Extrema_POnSurf pOnS1;
    Extrema_POnSurf pOnS2;
    gp_Pnt          arcpoint = PointsFromArc.Value(i);
    bool            isOnS1   = GetPointOn2S(arcpoint, *theSurfaceObj, CheckTol, pOnS1);
    bool            isOnS2   = GetPointOn2S(arcpoint, *theSurfaceTool, CheckTol, pOnS2);
    if (isOnS1 && isOnS2)
    {
      double u1 = 0., v1 = 0., u2 = 0., v2 = 0.;
      pOnS1.Parameter(u1, v1);
      pOnS2.Parameter(u2, v2);
      IntSurf_PntOn2S pOn2S;
      pOn2S.SetValue(arcpoint, u1, v1, u2, v2);
      theLineOn2S->Add(pOn2S);
    }
  }

  return ArcNumber;
}

//=========================================================================================
// function: IsPointOK
//
//  purpose: returns the state of testPoint on OTHER face.
//========================================================================================
static bool IsPointOK(const gp_Pnt&            theTestPnt,
                      const Adaptor3d_Surface& theTestSurface,
                      const double&            theTol)
{
  bool          result = false;
  double        ExtTol = theTol; // 1.e-7;
  Extrema_ExtPS extPS(theTestPnt, theTestSurface, ExtTol, ExtTol);
  if (extPS.IsDone() && extPS.NbExt() > 0)
  {
    int    i        = 0;
    double MinDist2 = 1.e+200;
    for (i = 1; i <= extPS.NbExt(); i++)
    {
      if (extPS.SquareDistance(i) < MinDist2)
      {
        MinDist2 = extPS.SquareDistance(i);
      }
    }
    if (MinDist2 <= theTol * theTol)
      result = true;
  }
  return result;
}

//=========================================================================================
// function: GetPointOn2S
//
//  purpose: check state of testPoint and returns result point if state is OK.
//========================================================================================
static bool GetPointOn2S(const gp_Pnt&            theTestPnt,
                         const Adaptor3d_Surface& theTestSurface,
                         const double&            theTol,
                         Extrema_POnSurf&         theResultPoint)
{
  bool          result = false;
  double        ExtTol = theTol; // 1.e-7;
  Extrema_ExtPS extPS(theTestPnt, theTestSurface, ExtTol, ExtTol);
  if (extPS.IsDone() && extPS.NbExt() > 0)
  {
    int    i = 0, minext = 1;
    double MinDist2 = 1.e+200;
    for (i = 1; i <= extPS.NbExt(); i++)
    {
      if (extPS.SquareDistance(i) < MinDist2)
      {
        minext   = i;
        MinDist2 = extPS.SquareDistance(i);
      }
    }
    if (MinDist2 <= theTol * theTol)
    {
      result         = true;
      theResultPoint = extPS.Point(minext);
    }
  }
  return result;
}

//=========================================================================================
// function: GetMergedWLineOnRestriction
//
//  purpose: merge sequence of WLines all placed along restriction if the conditions of
//           merging are OK.
//========================================================================================
static occ::handle<IntPatch_WLine> GetMergedWLineOnRestriction(
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theSlin,
  const double&                                     theVrtxTol,
  const occ::handle<IntSurf_LineOn2S>&              theLineOn2S)
{
  occ::handle<IntPatch_WLine> mWLine;
  if (theLineOn2S->NbPoints() == 0)
  {
    return mWLine;
  }
  //
  IntSurf_TypeTrans trans1 = IntSurf_Undecided;
  IntSurf_TypeTrans trans2 = IntSurf_Undecided;
  int               i      = 0;

  for (i = 1; i <= theSlin.Length(); i++)
  {
    if (theSlin.Value(i)->ArcType() != IntPatch_Walking)
      continue;

    const occ::handle<IntPatch_WLine>& aWLine =
      *((occ::handle<IntPatch_WLine>*)&(theSlin.Value(i)));

    if (aWLine->TransitionOnS1() != IntSurf_Undecided && aWLine->TransitionOnS1() != IntSurf_Touch)
      trans1 = aWLine->TransitionOnS1();
    if (aWLine->TransitionOnS2() != IntSurf_Undecided && aWLine->TransitionOnS2() != IntSurf_Touch)
      trans2 = aWLine->TransitionOnS2();
  }

  mWLine = new IntPatch_WLine(theLineOn2S, false, trans1, trans2);

  int            NbPnts = mWLine->NbPnts();
  IntPatch_Point aFirstVertex, aLastVertex;

  aFirstVertex.SetValue(mWLine->Point(1).Value(), theVrtxTol, false);
  aLastVertex.SetValue(mWLine->Point(NbPnts).Value(), theVrtxTol, false);

  double u1 = 0., v1 = 0., u2 = 0., v2 = 0.;

  mWLine->Point(1).Parameters(u1, v1, u2, v2);
  aFirstVertex.SetParameters(u1, v1, u2, v2);

  mWLine->Point(NbPnts).Parameters(u1, v1, u2, v2);
  aLastVertex.SetParameters(u1, v1, u2, v2);

  aFirstVertex.SetParameter(1);
  aLastVertex.SetParameter(theLineOn2S->NbPoints());

  mWLine->AddVertex(aFirstVertex);
  mWLine->AddVertex(aLastVertex);

  mWLine->ComputeVertexParameters(theVrtxTol);

  return mWLine;
}
