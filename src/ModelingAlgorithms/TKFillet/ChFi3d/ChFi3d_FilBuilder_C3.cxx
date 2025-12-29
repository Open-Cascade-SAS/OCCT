// Created on: 1995-04-26
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepBlend_ConstRad.hxx>
#include <BRepBlend_ConstRadInv.hxx>
#include <BRepBlend_EvolRad.hxx>
#include <BRepBlend_EvolRadInv.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_FilBuilder.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_ConstrainedFilling.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Law_S.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>

#ifdef OCCT_DEBUG
  #include <Geom_TrimmedCurve.hxx>
extern bool ChFi3d_GettraceDRAWSPINE();
extern bool ChFi3d_GetcontextSPINEBEZIER();
extern bool ChFi3d_GetcontextSPINECIRCLE();
extern bool ChFi3d_GetcontextSPINECE();
extern bool ChFi3d_GetcontextFORCEFILLING();
  #include <OSD_Chronometer.hxx>

extern double t_t3cornerinit, t_spherique, t_torique, t_notfilling, t_filling, t_t3cornerDS;
extern void   ChFi3d_InitChron(OSD_Chronometer& ch);
extern void   ChFi3d_ResultChron(OSD_Chronometer& ch, double& time);
#endif

//=================================================================================================

static int SearchPivot(int* s, double u[3][3], const double t)
{
  bool bondeb, bonfin;
  for (int i = 0; i <= 2; i++)
  {
    if (s[(i + 1) % 3] == 1)
    {
      bondeb = (u[(i + 1) % 3][i] - u[(i + 1) % 3][(i + 2) % 3] >= -t);
    }
    else
    {
      bondeb = (u[(i + 1) % 3][i] - u[(i + 1) % 3][(i + 2) % 3] <= t);
    }
    if (s[(i + 2) % 3] == 1)
    {
      bonfin = (u[(i + 2) % 3][i] - u[(i + 2) % 3][(i + 1) % 3] >= -t);
    }
    else
    {
      bonfin = (u[(i + 2) % 3][i] - u[(i + 2) % 3][(i + 1) % 3] <= t);
    }
    if (bondeb && bonfin)
    {
      return i;
    }
  }
  return -1;
}

//=================================================================================================

static bool SearchFD(TopOpeBRepDS_DataStructure&       DStr,
                     const occ::handle<ChFiDS_Stripe>& cd1,
                     const occ::handle<ChFiDS_Stripe>& cd2,
                     const int                         sens1,
                     const int                         sens2,
                     int&                              i1,
                     int&                              i2,
                     double&                           p1,
                     double&                           p2,
                     const int                         ind1,
                     const int                         ind2,
                     TopoDS_Face&                      face,
                     bool&                             sameside,
                     int&                              jf1,
                     int&                              jf2)
{
  bool          found = false;
  int           id1 = ind1, id2 = ind2;
  int           if1 = ind1, if2 = ind2;
  int           l1 = cd1->SetOfSurfData()->Length();
  int           l2 = cd2->SetOfSurfData()->Length();
  int           i;
  bool          fini1 = false, fini2 = false;
  bool          visavis;
  TopoDS_Vertex Vtx;
  while (!found)
  {
    for (i = id1; (i * sens1) <= (if1 * sens1) && !found && !fini2; i = i + sens1)
    {
      if (ChFi3d_IsInFront(DStr,
                           cd1,
                           cd2,
                           i,
                           if2,
                           sens1,
                           sens2,
                           p1,
                           p2,
                           face,
                           sameside,
                           jf1,
                           jf2,
                           visavis,
                           Vtx,
                           false,
                           false))
      {
        i1    = i;
        i2    = if2;
        found = true;
      }
    }
    if (!fini1)
    {
      if1 = if1 + sens1;
      if (if1 < 1 || if1 > l1)
      {
        if1   = if1 - sens1;
        fini1 = true;
      }
    }

    for (i = id2; (i * sens2) <= (if2 * sens2) && !found && !fini1; i = i + sens2)
    {
      if (ChFi3d_IsInFront(DStr,
                           cd1,
                           cd2,
                           if1,
                           i,
                           sens1,
                           sens2,
                           p1,
                           p2,
                           face,
                           sameside,
                           jf1,
                           jf2,
                           visavis,
                           Vtx,
                           false,
                           false))
      {
        i1    = if1;
        i2    = i;
        found = true;
      }
    }
    if (!fini2)
    {
      if2 = if2 + sens2;
      if (if2 < 1 || if2 > l2)
      {
        if2   = if2 - sens2;
        fini2 = true;
      }
    }
    if (fini1 && fini2)
      break;
  }
  return found;
}

//=======================================================================
// function : ToricCorner
// purpose  : Test if this is a particular case of a torus corner
//           (or spherical limited by isos).
//=======================================================================

static bool ToricCorner(const TopoDS_Face& F, const double rd, const double rf, const gp_Vec& v)
{
  if (std::abs(rd - rf) > Precision::Confusion())
  {
    return false;
  }
  BRepAdaptor_Surface bs(F);
  if (bs.GetType() != GeomAbs_Plane)
  {
    return false;
  }
  double scal1 = std::abs(bs.Plane().Position().XDirection().Dot(v));
  double scal2 = std::abs(bs.Plane().Position().YDirection().Dot(v));
  return (scal1 <= Precision::Confusion() && scal2 <= Precision::Confusion());
}

//=======================================================================
// function : PerformThreeCorner
// purpose  : Calculate fillet on a top with three edges
//           incident carrying each edge.
//=======================================================================

void ChFi3d_FilBuilder::PerformThreeCorner(const int Jndex)
{

#ifdef OCCT_DEBUG
  OSD_Chronometer ch;
  ChFi3d_InitChron(ch); // init perf initialisation
#endif

  TopOpeBRepDS_DataStructure&                            DStr = myDS->ChangeDS();
  const TopoDS_Vertex&                                   Vtx  = myVDataMap.FindKey(Jndex);
  NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator It;
  int                                                    Index[3], pivot, deb, fin, ii, jj, kk;
  // double R = 0.;
  bool                       pivdif    = true;
  bool                       c1pointu  = false;
  bool                       c1toric   = false;
  bool                       c1spheric = false;
  occ::handle<ChFiDS_Stripe> CD[3];
  TopoDS_Face                face[3];
  int                        jf[3][3];
  bool                       sameside[3], oksea[3];
  for (int g = 0; g <= 2; g++)
  {
    oksea[g] = false;
  }
  int    i[3][3];
  int    sens[3];
  double p[3][3];

  bool filling = false;

  for (It.Initialize(myVDataMap(Jndex)), ii = 0; It.More() && ii < 3; It.Next(), ii++)
  {
    Index[ii] = ChFi3d_IndexOfSurfData(Vtx, It.Value(), sens[ii]);
    CD[ii]    = It.Value();
  }
  // It is checked if one of CD is not present twice in which
  // case it is necessary to modify the return of IndexOfSurfData
  // that takes the first solution.
  if (CD[0] == CD[1])
  {
    Index[1] = CD[1]->SetOfSurfData()->Length();
    sens[1]  = -1;
  }
  else if (CD[1] == CD[2])
  {
    Index[2] = CD[2]->SetOfSurfData()->Length();
    sens[2]  = -1;
  }
  else if (CD[0] == CD[2])
  {
    Index[2] = CD[2]->SetOfSurfData()->Length();
    sens[2]  = -1;
  }
  oksea[2] = SearchFD(DStr,
                      CD[0],
                      CD[1],
                      sens[0],
                      sens[1],
                      i[0][1],
                      i[1][0],
                      p[0][1],
                      p[1][0],
                      Index[0],
                      Index[1],
                      face[2],
                      sameside[2],
                      jf[0][1],
                      jf[1][0]);
  oksea[1] = SearchFD(DStr,
                      CD[0],
                      CD[2],
                      sens[0],
                      sens[2],
                      i[0][2],
                      i[2][0],
                      p[0][2],
                      p[2][0],
                      Index[0],
                      Index[2],
                      face[1],
                      sameside[1],
                      jf[0][2],
                      jf[2][0]);
  oksea[0] = SearchFD(DStr,
                      CD[1],
                      CD[2],
                      sens[1],
                      sens[2],
                      i[1][2],
                      i[2][1],
                      p[1][2],
                      p[2][1],
                      Index[1],
                      Index[2],
                      face[0],
                      sameside[0],
                      jf[1][2],
                      jf[2][1]);
  //
  // Analyze concavities of 3 fillets :
  //        - 2 concavities identic and 1 inverted.
  //        - 3 concavities identic
  //
  if (oksea[2] && oksea[1] && !sameside[2] && !sameside[1])
  {
    pivot = 0;
    deb   = 1;
    fin   = 2;
  }
  else if (oksea[2] && oksea[0] && !sameside[2] && !sameside[0])
  {
    pivot = 1;
    deb   = 2;
    fin   = 0;
  }
  else if (oksea[1] && oksea[0] && !sameside[1] && !sameside[0])
  {
    pivot = 2;
    deb   = 0;
    fin   = 1;
  }
  else if (oksea[0] && oksea[1] && oksea[2])
  {
    // 3 concavities identic.
    pivot = SearchPivot(sens, p, tol2d);
    if (pivot < 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "pivot not found, plate is called" << std::endl;
#endif
      PerformMoreThreeCorner(Jndex, 3);
      return;
    }
    else
    {
      deb = (pivot + 1) % 3;
      fin = (pivot + 2) % 3;
    }
    pivdif = false;
    if (std::abs(p[0][1] - p[0][2]) <= tol2d && std::abs(p[1][0] - p[1][2]) <= tol2d
        && std::abs(p[2][0] - p[2][1]) <= tol2d)
    {
      c1pointu = true;
    }
  }
  else
  {
    PerformMoreThreeCorner(Jndex, 3);
    return;
  }
  int ifacdeb, ifacfin;
  ifacdeb = CD[deb]->ChangeSetOfSurfData()->Value(i[deb][pivot])->Index(3 - jf[deb][pivot]);
  ifacfin = CD[fin]->ChangeSetOfSurfData()->Value(i[fin][pivot])->Index(3 - jf[fin][pivot]);
  if (ifacfin != ifacdeb)
  {
#ifdef OCCT_DEBUG
    std::cout << "several base faces, plate is called" << std::endl;
#endif
    PerformMoreThreeCorner(Jndex, 3);
    return;
  }
  if (i[pivot][deb] != i[pivot][fin])
  {
#ifdef OCCT_DEBUG
    std::cout << "load surfdata on the pivot, plate is called" << std::endl;
#endif
    PerformMoreThreeCorner(Jndex, 3);
    return;
  }

  double Rdeb, Rfin, Rdp, Rfp;
  gp_Pnt Pdeb, Pfin, Pdp, Pfp;
  gp_Vec Vdeb, Vfin, Vdp, Vfp;
  if (c1pointu)
  {
    gp_Pnt pbid;
    gp_Vec qv[3];
    double qr[3];
    for (ii = 0; ii <= 2; ii++)
    {
      jj = (ii + 1) % 3;
      kk = (ii + 2) % 3;
      ChFi3d_ExtrSpineCarac(DStr, CD[jj], i[jj][ii], p[jj][ii], 1, sens[jj], pbid, qv[jj], qr[jj]);
    }
    for (ii = 0; ii <= 2 && !c1toric; ii++)
    {
      jj = (ii + 1) % 3;
      kk = (ii + 2) % 3;
      if (ToricCorner(face[ii], qr[jj], qr[kk], qv[ii]))
      {
        c1toric = true;
        pivot   = ii;
        deb     = jj;
        fin     = kk;
      }
    }
    if (!c1toric)
      c1spheric = (std::abs(qr[0] - qr[1]) < tolapp3d && std::abs(qr[0] - qr[2]) < tolapp3d);
  }

  //  Previously to avoid loops the points were always located
  //  inside, which could impede the construction of the
  //  guideline of the corner which now is a circle.
  //  int jjjd = jf[deb][fin], jjjf = jf[fin][deb];
  //  if (pivdif) jjjd = jf[deb][pivot], jjjf = jf[fin][pivot];
  int jjjd = jf[deb][pivot], jjjf = jf[fin][pivot];
  ChFi3d_ExtrSpineCarac(DStr,
                        CD[deb],
                        i[deb][pivot],
                        p[deb][pivot],
                        jjjd,
                        sens[deb],
                        Pdeb,
                        Vdeb,
                        Rdeb);
  ChFi3d_ExtrSpineCarac(DStr,
                        CD[fin],
                        i[fin][pivot],
                        p[fin][pivot],
                        jjjf,
                        sens[fin],
                        Pfin,
                        Vfin,
                        Rfin);
  ChFi3d_ExtrSpineCarac(DStr,
                        CD[pivot],
                        i[pivot][deb],
                        p[pivot][deb],
                        0,
                        sens[pivot],
                        Pdp,
                        Vdp,
                        Rdp);
  ChFi3d_ExtrSpineCarac(DStr,
                        CD[pivot],
                        i[pivot][fin],
                        p[pivot][fin],
                        0,
                        sens[pivot],
                        Pfp,
                        Vfp,
                        Rfp);
  // in cas of allsame it is checked that points on the face are not
  // too close, which can stop the work.
  if (!pivdif)
  {
    gp_Pnt ptestdeb, ptestfin;
    gp_Vec bidvec;
    double bidr;
    ChFi3d_ExtrSpineCarac(DStr,
                          CD[deb],
                          i[deb][pivot],
                          p[deb][pivot],
                          jf[deb][fin],
                          sens[deb],
                          ptestdeb,
                          bidvec,
                          bidr);
    ChFi3d_ExtrSpineCarac(DStr,
                          CD[fin],
                          i[fin][pivot],
                          p[fin][pivot],
                          jf[fin][deb],
                          sens[fin],
                          ptestfin,
                          bidvec,
                          bidr);
    double distest = ptestdeb.Distance(ptestfin);
    if (distest < (Rdp + Rfp) * 0.05)
      filling = true;
    if (distest < (Rdp + Rfp) * 0.005)
      c1pointu = true;
  }

  if (!c1pointu)
  {
    if (!pivdif)
      c1pointu = (std::abs(p[deb][pivot] - p[deb][fin]) <= tol2d
                  && std::abs(p[fin][pivot] - p[fin][deb]) <= tol2d);
    if (std::abs(p[pivot][deb] - p[pivot][fin]) <= tol2d)
      c1toric = ToricCorner(face[pivot], Rdeb, Rfin, Vdp);
  }
  // there is a pivot, the start and the end CD (finally !?!) :
  // -------------------------------------------------------------
  // the criterions determining if the corner is a torus or a sphere
  // are based only on the configuration of sections at end and the
  // nature of faces, it is necessary to make tests to
  // determine if a more detailed analysis of incident fillets
  // is necessare to provide tangency between them and
  // the corner (in particular in the case with variable radius).

  occ::handle<ChFiDS_SurfData>& fddeb = CD[deb]->ChangeSetOfSurfData()->ChangeValue(i[deb][pivot]);
  occ::handle<ChFiDS_SurfData>& fdfin = CD[fin]->ChangeSetOfSurfData()->ChangeValue(i[fin][pivot]);
  occ::handle<ChFiDS_SurfData>& fdpiv =
    CD[pivot]->ChangeSetOfSurfData()->ChangeValue(i[pivot][deb]);

  // HSurfaces and other suitable tools are constructed.
  // ----------------------------------------------------------

  TopAbs_Orientation               OFac = face[pivot].Orientation();
  occ::handle<BRepAdaptor_Surface> Fac  = new BRepAdaptor_Surface(face[pivot]);
  gp_Pnt2d                         ppp1, ppp2;
  const ChFiDS_FaceInterference&   bid1 =
    CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->InterferenceOnS1();
  ppp1 = bid1.PCurveOnSurf()->Value(bid1.FirstParameter());
  const ChFiDS_FaceInterference& bid2 =
    CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->InterferenceOnS2();
  ppp2                    = bid2.PCurveOnSurf()->Value(bid2.LastParameter());
  double              uu1 = ppp1.X(), uu2 = ppp2.X(), vv1 = ppp1.Y(), vv2 = ppp2.Y();
  GeomAdaptor_Surface gasurf(
    (DStr.Surface(CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->Surf())).Surface(),
    uu1,
    uu2,
    vv1,
    vv2);
  GeomAbs_SurfaceType styp = gasurf.GetType();
  if (styp == GeomAbs_Cylinder)
  {
    double h = vv2 - vv1;
    vv1 -= 0.5 * h;
    vv2 += 0.5 * h;
    gasurf.Load((DStr.Surface(CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->Surf())).Surface(),
                uu1,
                uu2,
                vv1,
                vv2);
  }
  else if (styp == GeomAbs_Torus)
  {
    double h = uu2 - uu1;
    uu1 -= 0.1 * h;
    uu2 += 0.1 * h;
    gasurf.Load((DStr.Surface(CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->Surf())).Surface(),
                uu1,
                uu2,
                vv1,
                vv2);
  }
  else if (styp == GeomAbs_BezierSurface || styp == GeomAbs_BSplineSurface)
  {
    gasurf.Load((DStr.Surface(CD[pivot]->SetOfSurfData()->Value(i[pivot][deb])->Surf())).Surface());
  }

  occ::handle<GeomAdaptor_Surface> Surf = new GeomAdaptor_Surface(gasurf);
  //  occ::handle<BRepTopAdaptor_TopolTool> IFac = new BRepTopAdaptor_TopolTool(Fac);
  // Try to not classify on the face for cases of reentering fillets which naturally depass
  // the border.
  occ::handle<GeomAdaptor_Surface> bidsurf =
    new GeomAdaptor_Surface(Fac->ChangeSurface().Surface());
  occ::handle<Adaptor3d_TopolTool> IFac = new Adaptor3d_TopolTool(bidsurf);
  // end of the attempt.
  occ::handle<Adaptor3d_TopolTool> ISurf  = new Adaptor3d_TopolTool(Surf);
  occ::handle<ChFiDS_Stripe>       corner = new ChFiDS_Stripe();
  occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>>& cornerset =
    corner->ChangeSetOfSurfData();
  cornerset                         = new NCollection_HSequence<occ::handle<ChFiDS_SurfData>>();
  occ::handle<ChFiDS_SurfData> coin = new ChFiDS_SurfData();
  cornerset->Append(coin);
  TopAbs_Orientation o1, o2, os1, os2, oo1, oo2;
  int                choix = CD[deb]->Choix();
  o1                       = face[pivot].Orientation();
  o2                       = fdpiv->Orientation();
  oo1                      = o1;
  oo2                      = o2;
  os1                      = CD[deb]->OrientationOnFace1();
  os2                      = CD[deb]->OrientationOnFace2();
  if (jf[deb][fin] == 1)
  {
    choix = ChFi3d::NextSide(o1, o2, os1, os2, choix);
    if (sens[deb] == 1)
    {
      if (choix % 2 == 1)
        choix++;
      else
        choix--;
    }
  }
  else
  {
    choix = ChFi3d::NextSide(o2, o1, os1, os2, -choix);
    if (sens[deb] == -1)
    {
      if (choix % 2 == 1)
        choix++;
      else
        choix--;
    }
  }

  gp_Pnt2d pfac1, pfac2, psurf1, psurf2;
  gp_Vec2d vfac1, vfac2;
  CD[deb]
    ->SetOfSurfData()
    ->Value(i[deb][pivot])
    ->Interference(jf[deb][fin])
    .PCurveOnFace()
    ->D1(p[deb][pivot], pfac1, vfac1);
  CD[fin]
    ->SetOfSurfData()
    ->Value(i[fin][pivot])
    ->Interference(jf[fin][deb])
    .PCurveOnFace()
    ->D1(p[fin][pivot], pfac2, vfac2);
  psurf1 = CD[pivot]
             ->SetOfSurfData()
             ->Value(i[pivot][deb])
             ->Interference(jf[pivot][deb])
             .PCurveOnSurf()
             ->Value(p[pivot][deb]);
  psurf2 = CD[pivot]
             ->SetOfSurfData()
             ->Value(i[pivot][fin])
             ->Interference(jf[pivot][fin])
             .PCurveOnSurf()
             ->Value(p[pivot][fin]);

  done = false;
#ifdef OCCT_DEBUG
  if (ChFi3d_GetcontextFORCEFILLING())
    c1spheric = c1toric = 0;
#endif

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_t3cornerinit); // result perf initialisations
#endif

  if (c1toric)
  {

#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch); // init perf case torus
#endif

    // Direct Construction.
    // ---------------------
    done = ChFiKPart_ComputeData::ComputeCorner(DStr,
                                                coin,
                                                Fac,
                                                Surf,
                                                oo1,
                                                oo2,
                                                o1,
                                                o2,
                                                Rdeb,
                                                Rdp,
                                                pfac1,
                                                pfac2,
                                                psurf1,
                                                psurf2);

#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch, t_torique); // result perf case torus
#endif
  }
  else if (c1spheric)
  {

#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch); // init perf case sphere
#endif

    done = ChFiKPart_ComputeData::ComputeCorner(DStr,
                                                coin,
                                                Fac,
                                                Surf,
                                                oo1,
                                                oo2,
                                                o1,
                                                o2,
                                                Rdp,
                                                pfac1,
                                                psurf1,
                                                psurf2);

#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch, t_spherique); // result perf cas sphere
#endif
  }
  else if (c1pointu)
  {
    filling = true;
  }
  if (!done)
  {
    if (!filling)
    {

#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch); // init perf not filling
#endif

      // Calculate a guideline,
      //------------------------------
      // Numerous problems with loops and half-turns connected to
      // the curvature of the guideline !!!!!!
      // FOR CIRCLE.
      // If the nature of guideline is changed it is necessary to
      // reset points Pdeb and Pfin at the inside (see the
      // comments about it in the calculation of Pdeb and Pfin).

      double radpondere = (Rdp + Rfp) / 2.;
      double locfleche  = fleche;

      double                  WFirst, WLast;
      occ::handle<Geom_Curve> spinecoin =
        ChFi3d_CircularSpine(WFirst, WLast, Pdeb, Vdeb, Pfin, Vfin, radpondere);
      if (spinecoin.IsNull())
      {
        // This is a bad case when the intersection of
        // section planes is done out of the sector.
        spinecoin = ChFi3d_Spine(Pdeb, Vdeb, Pfin, Vfin, radpondere);
        WFirst    = 0.;
        WLast     = 1.;
      }
      else
        locfleche = radpondere * (WLast - WFirst) * fleche;
      double                      pasmax      = (WLast - WFirst) * 0.05;
      occ::handle<ChFiDS_ElSpine> cornerspine = new ChFiDS_ElSpine();
      cornerspine->SetCurve(spinecoin);
      cornerspine->FirstParameter(WFirst - pasmax);
      cornerspine->LastParameter(WLast + pasmax);
      // Just to confuse Compute that should not require this
      // in this exact case ...
      occ::handle<ChFiDS_Spine> NullSpine;
      // The fillet is calculated - from beginning to end
      //                       - from the face to the surface
      //
      math_Vector Soldep(1, 4);
      Soldep(1) = pfac1.X();
      Soldep(2) = pfac1.Y();
      Soldep(3) = psurf1.X();
      Soldep(4) = psurf1.Y();

      bool                        Gd1, Gd2, Gf1, Gf2;
      occ::handle<BRepBlend_Line> lin;
      double                      ffi = WFirst, lla = WLast + pasmax;

      if (std::abs(Rdeb - Rfin) <= tolapp3d)
      {

        BRepBlend_ConstRad    func(Fac, Surf, cornerspine);
        BRepBlend_ConstRadInv finv(Fac, Surf, cornerspine);
        func.Set(Rdeb, choix);
        func.Set(myShape);
        finv.Set(Rdeb, choix);
        double TolGuide = cornerspine->Resolution(tolapp3d);

        int intf = 3, intl = 3;
        done = ComputeData(coin,
                           cornerspine,
                           NullSpine,
                           lin,
                           Fac,
                           IFac,
                           Surf,
                           ISurf,
                           func,
                           finv,
                           ffi,
                           pasmax,
                           locfleche,
                           TolGuide,
                           ffi,
                           lla,
                           false,
                           false,
                           true,
                           Soldep,
                           intf,
                           intl,
                           Gd1,
                           Gd2,
                           Gf1,
                           Gf2,
                           false,
                           true);
#ifdef OCCT_DEBUG
        if (ChFi3d_GetcontextFORCEFILLING())
          done = 0;
#endif
        if (done && Gf2)
        {
          done    = CompleteData(coin, func, lin, Fac, Surf, OFac, Gd1, false, Gf1, false);
          filling = !done;
        }
        else
          filling = true;
      }
      else
      {
        occ::handle<Law_S> law = new Law_S();
        law->Set(WFirst, Rdeb, WLast, Rfin);
        BRepBlend_EvolRad    func(Fac, Surf, cornerspine, law);
        BRepBlend_EvolRadInv finv(Fac, Surf, cornerspine, law);
        func.Set(choix);
        func.Set(myShape);
        finv.Set(choix);
        double TolGuide = cornerspine->Resolution(tolapp3d);
        int    intf = 3, intl = 3;
        done = ComputeData(coin,
                           cornerspine,
                           NullSpine,
                           lin,
                           Fac,
                           IFac,
                           Surf,
                           ISurf,
                           func,
                           finv,
                           ffi,
                           pasmax,
                           locfleche,
                           TolGuide,
                           ffi,
                           lla,
                           false,
                           false,
                           true,
                           Soldep,
                           intf,
                           intl,
                           Gd1,
                           Gd2,
                           Gf1,
                           Gf2,
                           false,
                           true);
#ifdef OCCT_DEBUG
        if (ChFi3d_GetcontextFORCEFILLING())
          done = 0;
#endif
        if (done && Gf2)
        {
          done    = CompleteData(coin, func, lin, Fac, Surf, OFac, Gd1, false, Gf1, false);
          filling = !done;
        }
        else
          filling = true;
      }

#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch, t_notfilling); // result perf not filling
#endif
    }

    if (filling)
    {

#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch); // init perf filling
#endif

      // the contour to be fillet consists of straight lines uv in beginning and end
      // of two pcurves (only one if c1pointu) calculated as possible
      // on piv and the opposite face.
      occ::handle<GeomFill_Boundary> Bdeb, Bfin, Bpiv, Bfac;
      occ::handle<Geom2d_Curve>      PCurveOnFace;
      if (!c1pointu)
        Bfac = ChFi3d_mkbound(Fac,
                              PCurveOnFace,
                              sens[deb],
                              pfac1,
                              vfac1,
                              sens[fin],
                              pfac2,
                              vfac2,
                              tolapp3d,
                              2.e-4);
      int    kkk;
      gp_Pnt ppbid;
      gp_Vec vp1, vp2;
      kkk =
        CD[deb]->SetOfSurfData()->Value(i[deb][pivot])->Interference(jf[deb][pivot]).LineIndex();
      DStr.Curve(kkk).Curve()->D1(p[deb][pivot], ppbid, vp1);
      kkk =
        CD[fin]->SetOfSurfData()->Value(i[fin][pivot])->Interference(jf[fin][pivot]).LineIndex();
      DStr.Curve(kkk).Curve()->D1(p[fin][pivot], ppbid, vp2);
      occ::handle<Geom2d_Curve> PCurveOnPiv;
      //      Bpiv = ChFi3d_mkbound(Surf,PCurveOnPiv,sens[deb],psurf1,vp1,
      //			    sens[fin],psurf2,vp2,tolesp,2.e-4);
      Bpiv           = ChFi3d_mkbound(Surf, PCurveOnPiv, psurf1, psurf2, tolapp3d, 2.e-4, false);
      double pardeb2 = p[deb][pivot];
      double parfin2 = p[fin][pivot];
      if (c1pointu)
      {
        pardeb2 = p[deb][fin];
        parfin2 = p[fin][deb];
      }
      gp_Pnt2d pdeb1 = CD[deb]
                         ->SetOfSurfData()
                         ->Value(i[deb][pivot])
                         ->Interference(jf[deb][pivot])
                         .PCurveOnSurf()
                         ->Value(p[deb][pivot]);
      gp_Pnt2d pdeb2 = CD[deb]
                         ->SetOfSurfData()
                         ->Value(i[deb][pivot])
                         ->Interference(jf[deb][fin])
                         .PCurveOnSurf()
                         ->Value(pardeb2);
      gp_Pnt2d pfin1 = CD[fin]
                         ->SetOfSurfData()
                         ->Value(i[fin][pivot])
                         ->Interference(jf[fin][pivot])
                         .PCurveOnSurf()
                         ->Value(p[fin][pivot]);
      gp_Pnt2d pfin2 = CD[fin]
                         ->SetOfSurfData()
                         ->Value(i[fin][pivot])
                         ->Interference(jf[fin][deb])
                         .PCurveOnSurf()
                         ->Value(parfin2);
      occ::handle<Geom_Surface> sdeb =
        DStr.Surface(CD[deb]->SetOfSurfData()->Value(i[deb][pivot])->Surf()).Surface();
      occ::handle<Geom_Surface> sfin =
        DStr.Surface(CD[fin]->SetOfSurfData()->Value(i[fin][pivot])->Surf()).Surface();

      Bdeb = ChFi3d_mkbound(sdeb, pdeb1, pdeb2, tolapp3d, 2.e-4);
      Bfin = ChFi3d_mkbound(sfin, pfin1, pfin2, tolapp3d, 2.e-4);

      GeomFill_ConstrainedFilling fil(11, 20);
      if (c1pointu)
        fil.Init(Bpiv, Bfin, Bdeb, true);
      else
        fil.Init(Bpiv, Bfin, Bfac, Bdeb, true);

      occ::handle<Geom_Surface> Surfcoin = fil.Surface();
      Surfcoin->VReverse(); // revert to direction face surface;
      done = CompleteData(coin,
                          Surfcoin,
                          Fac,
                          PCurveOnFace,
                          Surf,
                          PCurveOnPiv,
                          fdpiv->Orientation(),
                          false,
                          false,
                          false,
                          false,
                          false);

#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch, t_filling); // result perf filling
#endif
    }
  }
  double P1deb, P2deb, P1fin, P2fin;
  if (!c1pointu)
  {
    p[deb][fin] = p[deb][pivot];
    p[fin][deb] = p[fin][pivot];
  }

  if (done)
  {
    // Update of 4 Stripes and the DS
    // -------------------------------------

#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch); // init perf update DS
#endif

    gp_Pnt2d                  pp1, pp2;
    double                    parpp1, parpp2;
    int                       If1, If2, Il1, Il2, Icf, Icl;
    const ChFiDS_CommonPoint& Pf1 = coin->VertexFirstOnS1();
    const ChFiDS_CommonPoint& Pf2 = coin->VertexFirstOnS2();
    ChFiDS_CommonPoint&       Pl1 = coin->ChangeVertexLastOnS1();
    if (c1pointu)
      Pl1 = coin->ChangeVertexFirstOnS1();
    const ChFiDS_CommonPoint& Pl2 = coin->VertexLastOnS2();

    Bnd_Box  bf1, bl1, bf2, bl2;
    Bnd_Box *pbf1 = &bf1, *pbl1 = &bl1, *pbf2 = &bf2, *pbl2 = &bl2;
    if (c1pointu)
      pbl1 = pbf1;
    pbf1->Add(Pf1.Point());
    pbf2->Add(Pf2.Point());
    pbl1->Add(Pl1.Point());
    pbl2->Add(Pl2.Point());

    // the start corner,
    // -----------------------
    ChFiDS_Regul regdeb, regfin;
    If1 = ChFi3d_IndexPointInDS(Pf1, DStr);
    If2 = ChFi3d_IndexPointInDS(Pf2, DStr);
    if (c1pointu)
      Il1 = If1;
    else
      Il1 = ChFi3d_IndexPointInDS(Pl1, DStr);
    Il2 = ChFi3d_IndexPointInDS(Pl2, DStr);
    pp1 = coin->InterferenceOnS1().PCurveOnSurf()->Value(coin->InterferenceOnS1().FirstParameter());
    pp2 = coin->InterferenceOnS2().PCurveOnSurf()->Value(coin->InterferenceOnS2().FirstParameter());
    if (c1pointu)
      coin->ChangeIndexOfS1(0);
    else
      coin->ChangeIndexOfS1(DStr.AddShape(face[pivot]));
    coin->ChangeIndexOfS2(-fdpiv->Surf());

    occ::handle<Geom_Curve> C3d;
    double                  tolreached;
    ChFi3d_ComputeArete(Pf1,
                        pp1,
                        Pf2,
                        pp2,
                        DStr.Surface(coin->Surf()).Surface(),
                        C3d,
                        corner->ChangeFirstPCurve(),
                        P1deb,
                        P2deb,
                        tolapp3d,
                        tol2d,
                        tolreached,
                        0);
    TopOpeBRepDS_Curve Tcurv1(C3d, tolreached);
    Icf = DStr.AddCurve(Tcurv1);
    regdeb.SetCurve(Icf);
    regdeb.SetS1(coin->Surf(), false);
    regdeb.SetS2(fddeb->Surf(), false);
    myRegul.Append(regdeb);
    corner->ChangeFirstCurve(Icf);
    corner->ChangeFirstParameters(P1deb, P2deb);
    corner->ChangeIndexFirstPointOnS1(If1);
    corner->ChangeIndexFirstPointOnS2(If2);
    ChFi3d_EnlargeBox(DStr, corner, coin, *pbf1, *pbf2, true);

    pp1 = coin->InterferenceOnS1().PCurveOnSurf()->Value(coin->InterferenceOnS1().LastParameter());
    pp2 = coin->InterferenceOnS2().PCurveOnSurf()->Value(coin->InterferenceOnS2().LastParameter());
    ChFi3d_ComputeArete(Pl1,
                        pp1,
                        Pl2,
                        pp2,
                        DStr.Surface(coin->Surf()).Surface(),
                        C3d,
                        corner->ChangeLastPCurve(),
                        P1fin,
                        P2fin,
                        tolapp3d,
                        tol2d,
                        tolreached,
                        0);
    TopOpeBRepDS_Curve Tcurv2(C3d, tolreached);
    Icl = DStr.AddCurve(Tcurv2);
    regfin.SetCurve(Icl);
    regfin.SetS1(coin->Surf(), false);
    regfin.SetS2(fdfin->Surf(), false);
    myRegul.Append(regfin);
    corner->ChangeLastCurve(Icl);
    corner->ChangeLastParameters(P1fin, P2fin);
    corner->ChangeIndexLastPointOnS1(Il1);
    corner->ChangeIndexLastPointOnS2(Il2);
    ChFi3d_EnlargeBox(DStr, corner, coin, *pbl1, *pbl2, false);

    // then CornerData of the beginning,
    // --------------------------------
    bool isfirst = (sens[deb] == 1), rev = (jf[deb][fin] == 2);
    int  isurf1 = 1, isurf2 = 2;
    parpp1 = p[deb][fin];
    parpp2 = p[deb][pivot];
    if (rev)
    {
      isurf1 = 2;
      isurf2 = 1;
      parpp1 = p[deb][pivot];
      parpp2 = p[deb][fin];
      CD[deb]->SetOrientation(TopAbs_REVERSED, isfirst);
    }
    pp1 = fddeb->InterferenceOnS1().PCurveOnSurf()->Value(parpp1);
    pp2 = fddeb->InterferenceOnS2().PCurveOnSurf()->Value(parpp2);
    CD[deb]->SetCurve(Icf, isfirst);
    CD[deb]->SetIndexPoint(If1, isfirst, isurf1);
    CD[deb]->SetIndexPoint(If2, isfirst, isurf2);
    CD[deb]->SetParameters(isfirst, P1deb, P2deb);
    fddeb->ChangeVertex(isfirst, isurf1) = Pf1;
    fddeb->ChangeVertex(isfirst, isurf2) = Pf2;
    fddeb->ChangeInterferenceOnS1().SetParameter(parpp1, isfirst);
    fddeb->ChangeInterferenceOnS2().SetParameter(parpp2, isfirst);
    TopOpeBRepDS_Curve&     tcdeb   = DStr.ChangeCurve(Icf);
    occ::handle<Geom_Curve> crefdeb = tcdeb.Curve();
    double                  tolrdeb;
    ChFi3d_ComputePCurv(crefdeb,
                        pp1,
                        pp2,
                        CD[deb]->ChangePCurve(isfirst),
                        DStr.Surface(fddeb->Surf()).Surface(),
                        P1deb,
                        P2deb,
                        tolapp3d,
                        tolrdeb,
                        rev);
    tcdeb.Tolerance(std::max(tolrdeb, tcdeb.Tolerance()));
    if (rev)
      ChFi3d_EnlargeBox(DStr, CD[deb], fddeb, *pbf2, *pbf1, isfirst);
    else
      ChFi3d_EnlargeBox(DStr, CD[deb], fddeb, *pbf1, *pbf2, isfirst);

    // then the end CornerData,
    // ------------------------
    isfirst = (sens[fin] == 1);
    rev     = (jf[fin][deb] == 2);
    isurf1  = 1;
    isurf2  = 2;
    parpp1  = p[fin][deb];
    parpp2  = p[fin][pivot];
    if (rev)
    {
      isurf1 = 2;
      isurf2 = 1;
      parpp1 = p[fin][pivot];
      parpp2 = p[fin][deb];
      CD[fin]->SetOrientation(TopAbs_REVERSED, isfirst);
    }
    pp1 = fdfin->InterferenceOnS1().PCurveOnSurf()->Value(parpp1);
    pp2 = fdfin->InterferenceOnS2().PCurveOnSurf()->Value(parpp2);
    CD[fin]->SetCurve(Icl, isfirst);
    CD[fin]->SetIndexPoint(Il1, isfirst, isurf1);
    CD[fin]->SetIndexPoint(Il2, isfirst, isurf2);
    CD[fin]->SetParameters(isfirst, P1fin, P2fin);
    fdfin->ChangeVertex(isfirst, isurf1) = Pl1;
    fdfin->ChangeVertex(isfirst, isurf2) = Pl2;
    fdfin->ChangeInterferenceOnS1().SetParameter(parpp1, isfirst);
    fdfin->ChangeInterferenceOnS2().SetParameter(parpp2, isfirst);
    TopOpeBRepDS_Curve&     tcfin   = DStr.ChangeCurve(Icl);
    occ::handle<Geom_Curve> creffin = tcfin.Curve();
    double                  tolrfin;
    ChFi3d_ComputePCurv(creffin,
                        pp1,
                        pp2,
                        CD[fin]->ChangePCurve(isfirst),
                        DStr.Surface(fdfin->Surf()).Surface(),
                        P1fin,
                        P2fin,
                        tolapp3d,
                        tolrfin,
                        rev);
    tcfin.Tolerance(std::max(tolrfin, tcfin.Tolerance()));
    if (rev)
      ChFi3d_EnlargeBox(DStr, CD[fin], fdfin, *pbl2, *pbl1, isfirst);
    else
      ChFi3d_EnlargeBox(DStr, CD[fin], fdfin, *pbl1, *pbl2, isfirst);

    // anf finally the pivot.
    // ------------------
    ChFiDS_FaceInterference& fi = coin->ChangeInterferenceOnS2();
    isfirst                     = (sens[pivot] == 1);
    rev                         = (jf[pivot][deb] == 2);
    isurf1                      = 1;
    isurf2                      = 2;
    if (rev)
    {
      isurf1 = 2;
      isurf2 = 1;
      CD[pivot]->SetOrientation(TopAbs_REVERSED, isfirst);
    }
    int                 ICcoinpiv = fi.LineIndex();
    TopOpeBRepDS_Curve& TCcoinpiv = DStr.ChangeCurve(ICcoinpiv);
    CD[pivot]->SetCurve(ICcoinpiv, isfirst);
    occ::handle<Geom_Curve>    Ccoinpiv = DStr.Curve(ICcoinpiv).Curve();
    occ::handle<Geom2d_Curve>& C2dOnPiv = fi.ChangePCurveOnFace();
    occ::handle<Geom_Surface>  Spiv     = DStr.Surface(fdpiv->Surf()).Surface();
    double                     tolr;
    ChFi3d_SameParameter(Ccoinpiv,
                         C2dOnPiv,
                         Spiv,
                         fi.FirstParameter(),
                         fi.LastParameter(),
                         tolapp3d,
                         tolr);
    TCcoinpiv.Tolerance(std::max(TCcoinpiv.Tolerance(), tolr));
    CD[pivot]->ChangePCurve(isfirst) = C2dOnPiv;
    CD[pivot]->SetIndexPoint(If2, isfirst, isurf1);
    CD[pivot]->SetIndexPoint(Il2, isfirst, isurf2);
    CD[pivot]->SetParameters(isfirst, fi.FirstParameter(), fi.LastParameter());
    fdpiv->ChangeVertex(isfirst, isurf1) = Pf2;
    fdpiv->ChangeVertex(isfirst, isurf2) = Pl2;
    fdpiv->ChangeInterference(isurf1).SetParameter(p[pivot][deb], isfirst);
    fdpiv->ChangeInterference(isurf2).SetParameter(p[pivot][fin], isfirst);
    CD[pivot]->InDS(isfirst); // filDS already does it from the corner.
    if (rev)
      ChFi3d_EnlargeBox(DStr, CD[pivot], fdpiv, *pbl2, *pbf2, isfirst);
    else
      ChFi3d_EnlargeBox(DStr, CD[pivot], fdpiv, *pbf2, *pbl2, isfirst);

    // To end the tolerances of points are rescaled.
    ChFi3d_SetPointTolerance(DStr, *pbf1, If1);
    ChFi3d_SetPointTolerance(DStr, *pbf2, If2);
    ChFi3d_SetPointTolerance(DStr, *pbl1, Il1);
    ChFi3d_SetPointTolerance(DStr, *pbl2, Il2);
  }

  // The data corners are truncated and index is updated.
  //----------------------------------------------------

  if (i[deb][pivot] < Index[deb])
  {
    CD[deb]->ChangeSetOfSurfData()->Remove(i[deb][pivot] + 1, Index[deb]);
    Index[deb] = i[deb][pivot];
  }
  else if (i[deb][pivot] > Index[deb])
  {
    CD[deb]->ChangeSetOfSurfData()->Remove(Index[deb], i[deb][pivot] - 1);
    i[deb][pivot] = Index[deb];
  }
  if (i[fin][pivot] < Index[fin])
  {
    CD[fin]->ChangeSetOfSurfData()->Remove(i[fin][pivot] + 1, Index[fin]);
    Index[fin] = i[fin][pivot];
  }
  else if (i[fin][pivot] > Index[fin])
  {
    CD[fin]->ChangeSetOfSurfData()->Remove(Index[fin], i[fin][pivot] - 1);
    i[fin][pivot] = Index[fin];
  }
  // it is necessary to take into account mutant corners.
  if (i[pivot][deb] < Index[pivot])
  {
    CD[pivot]->ChangeSetOfSurfData()->Remove(i[pivot][deb] + 1, Index[pivot]);
    Index[pivot] = i[pivot][deb];
  }
  else if (i[pivot][deb] > Index[pivot])
  {
    CD[pivot]->ChangeSetOfSurfData()->Remove(Index[pivot], i[pivot][deb] - 1);
    i[pivot][deb] = Index[pivot];
  }
  if (!myEVIMap.IsBound(Vtx))
  {
    NCollection_List<int> li;
    myEVIMap.Bind(Vtx, li);
  }
  myEVIMap.ChangeFind(Vtx).Append(coin->Surf());
  corner->SetSolidIndex(CD[pivot]->SolidIndex());
  myListStripe.Append(corner);

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_t3cornerDS); // result perf update DS
#endif
}
