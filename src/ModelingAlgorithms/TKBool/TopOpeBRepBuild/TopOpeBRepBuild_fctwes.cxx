// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
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

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT void debfctwes(const int /*i*/) {}

Standard_EXPORT void debfctwesmess(const int i, const TCollection_AsciiString& s = "")
{
  std::cout << "+ + + debfctwes " << s << "F" << i << std::endl;
  debfctwes(i);
}

extern void debaddpwes(const int                           iFOR,
                       const TopAbs_State                  TB1,
                       const int                           iEG,
                       const TopAbs_Orientation            neworiE,
                       const TopOpeBRepBuild_PBuilder&     PB,
                       const TopOpeBRepBuild_PWireEdgeSet& PWES,
                       const TCollection_AsciiString&      str1,
                       const TCollection_AsciiString&      str2);
#endif

bool TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

#define M_IN(st) (st == TopAbs_IN)
#define M_OUT(st) (st == TopAbs_OUT)
#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillCurveTopologyWES(const TopoDS_Shape&          F1,
                                                    const TopOpeBRepBuild_GTopo& G1,
                                                    TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  TopAbs_ShapeEnum t1, t2, ShapeInterf;
  G1.Type(t1, t2);
  ShapeInterf = t1;

#ifdef OCCT_DEBUG
  int  iF;
  bool tSPS = GtraceSPS(F1, iF);
  if (tSPS)
    GdumpSHASTA(iF, TB1, WES, "--- GFillCurveTopologyWES");
  if (tSPS)
  {
    std::cout << " ShapeInterf ";
    TopAbs::Print(ShapeInterf, std::cout);
    std::cout << std::endl;
  }
  if (tSPS)
  {
    debfctwesmess(iF);
  }
#endif

  TopOpeBRepDS_CurveIterator FCit(myDataStructure->FaceCurves(F1));
  myFaceReference = TopoDS::Face(F1);
  myFaceToFill    = TopoDS::Face(F1);
  // modified by NIZHNY-MZV  Thu Feb 24 09:15:33 2000
  // sometimes by the problem of tolerances we have intersection
  // lines between SameDomain faces, but Same domain faces can not
  // have intersection lines other than by its original edges
  // so we skip it if we find that two SameDomain faces have
  // new intersection edge
  bool hsd = myDataStructure->HasSameDomain(F1);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSDMap;
  if (hsd)
  {
    NCollection_List<TopoDS_Shape>::Iterator it = myDataStructure->SameDomain(F1);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& SDF = it.Value();
      aSDMap.Add(SDF);
    }
  }
  // End modified by NIZHNY-MZV  Thu Feb 24 09:21:08 2000

  for (; FCit.More(); FCit.Next())
  {
    if (ShapeInterf != TopAbs_SHAPE)
    {
      const occ::handle<TopOpeBRepDS_Interference>& I    = FCit.Value();
      const TopOpeBRepDS_Transition&                T    = I->Transition();
      TopAbs_ShapeEnum                              shab = T.ShapeBefore(), shaa = T.ShapeAfter();
      if ((shaa != ShapeInterf) || (shab != ShapeInterf))
        continue;
      // modified by NIZHNY-MZV  Thu Feb 24 09:14:31 2000

      int          si = I->Support();
      TopoDS_Shape SS = myDataStructure->Shape(si);
      // see comment above
      if (aSDMap.Contains(SS))
        continue;
      // End modified by NIZHNY-MZV  Thu Feb 24 09:21:34 2000
    }
    GFillCurveTopologyWES(FCit, G1, WES);
  }

  return;
} // GFillCurveTopologyWES

//=================================================================================================

void TopOpeBRepBuild_Builder::GFillCurveTopologyWES(const TopOpeBRepDS_CurveIterator& FCit,
                                                    const TopOpeBRepBuild_GTopo&      G1,
                                                    TopOpeBRepBuild_WireEdgeSet&      WES) const
{
  bool more = FCit.More();
  if (!more)
    return;

  TopAbs_State TB1, TB2;
  G1.StatesON(TB1, TB2);
  TopOpeBRepDS_Config Conf = G1.Config1();
  TopAbs_State        TB   = TB1;
  if (Conf == TopOpeBRepDS_DIFFORIENTED)
  { // -jyl980525
    //    if      (TB1 == TopAbs_OUT) TB = TopAbs_IN;
    //    else if (TB1 == TopAbs_IN ) TB = TopAbs_OUT;
  }

  TopoDS_Face& WESF = *((TopoDS_Face*)((void*)&WES.Face()));
  TopoDS_Face& FTF  = *((TopoDS_Face*)((void*)&myFaceToFill));
#ifdef OCCT_DEBUG
//  bool FTFeqWESF = myFaceReference.IsEqual(WESF);
#endif

#ifdef OCCT_DEBUG
  int iWESF = myDataStructure->Shape(WESF);
  int iref  = myDataStructure->Shape(myFaceReference);
  int ifil  = myDataStructure->Shape(myFaceToFill);
#endif

  bool opeCut   = Opec12() || Opec21();
  bool ComOfCut = opeCut && (TB1 == TB2) && (TB1 == TopAbs_IN);

  const TopOpeBRepDS_Transition& T        = FCit.Value()->Transition();
  TopAbs_Orientation             neworiE  = T.Orientation(TB);
  bool                           samegeom = TopOpeBRepBuild_FUN_aresamegeom(FTF, WESF);
  if (!samegeom)
  {
    neworiE = TopAbs::Complement(neworiE);
  }

#ifdef OCCT_DEBUG
  bool tSPS = GtraceSPS(iWESF);
  if (tSPS)
  {
    std::cout << "ifil : " << ifil << " iref : " << iref << " iwes : " << iWESF << std::endl;
    std::cout << "face " << ifil << " is ";
    TopOpeBRepDS::Print(Conf, std::cout);
    std::cout << std::endl;
    std::cout << "ComOfCut " << ComOfCut << std::endl;
    debfctwesmess(iWESF);
  }
#endif

  if (ComOfCut)
    return;

  int                                      iG    = FCit.Current();
  const NCollection_List<TopoDS_Shape>&    LnewE = NewEdges(iG);
  NCollection_List<TopoDS_Shape>::Iterator Iti(LnewE);
  for (; Iti.More(); Iti.Next())
  {
    TopoDS_Shape EE = Iti.Value();
    TopoDS_Edge& E  = TopoDS::Edge(EE);

    // modified by NIZHNY-MZV  Fri Mar 17 12:51:03 2000
    if (BRep_Tool::Degenerated(E))
      continue;

    //    TopAbs_Orientation neworiE = FCit.Orientation(TB);
    E.Orientation(neworiE);

    const occ::handle<Geom2d_Curve>& PC = FCit.PCurve();

    bool EhasPConFTF =

      FC2D_HasCurveOnSurface(E, FTF);
    // modified by NIZHNY-MZV  Mon Mar 27 15:24:39 2000
    if (!EhasPConFTF)
      myBuildTool.PCurve(FTF, E, PC);

    bool EhasPConWESF = FC2D_HasCurveOnSurface(E, WESF);

    if (!EhasPConWESF)
    {
      //      double tolE = BRep_Tool::Tolerance(E);
      double                    f2, l2, tolpc;
      occ::handle<Geom2d_Curve> C2D;
      C2D = FC2D_CurveOnSurface(E, WESF, f2, l2, tolpc);
      if (C2D.IsNull())
        throw Standard_ProgramError("GFillCurveTopologyWES");
#ifdef OCCT_DEBUG
//      double tol = std::max(tolE,tolpc);
#endif

      myBuildTool.PCurve(WESF, E, C2D);

#ifdef OCCT_DEBUG
      EhasPConWESF = FC2D_HasCurveOnSurface(E, WESF);
      if (!EhasPConWESF)
        std::cout << "TopOpeBRepBuild_Builder::GFillCurveTopologyWES : Null PCurve on F" << iWESF
                  << std::endl;
#endif
    }

#ifdef OCCT_DEBUG
    if (tSPS)
      debaddpwes(iWESF,
                 TB,
                 iG,
                 neworiE,
                 (TopOpeBRepBuild_Builder* const)this,
                 &WES,
                 "GFillCurveTopology ",
                 "WES+ EofC ");
#endif

    WES.AddStartElement(E);
  }

} // GFillCurveTopologyWES
