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

// szv#4 S4163

#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireVertex.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

// ied_modif_for_compil_Nov-19-1998
//=================================================================================================

ShapeAnalysis_WireVertex::ShapeAnalysis_WireVertex()
{
  myDone  = false;
  myPreci = Precision::Confusion();
}

//=================================================================================================

void ShapeAnalysis_WireVertex::Init(const TopoDS_Wire& wire, const double preci)
{
  Init(new ShapeExtend_WireData(wire), preci);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::Init(const occ::handle<ShapeExtend_WireData>& sbwd,
                                    const double /*preci*/)
{
  int nb = sbwd->NbEdges();
  if (nb == 0)
    return;
  myDone = false;
  myWire = sbwd;
  myStat = new NCollection_HArray1<int>(1, nb);
  myStat->Init(0);
  myPos  = new NCollection_HArray1<gp_XYZ>(1, nb);
  myUPre = new NCollection_HArray1<double>(1, nb);
  myUPre->Init(0.0);
  myUFol = new NCollection_HArray1<double>(1, nb);
  myUFol->Init(0.0);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::Load(const TopoDS_Wire& wire)
{
  Init(wire, myPreci);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::Load(const occ::handle<ShapeExtend_WireData>& sbwd)
{
  Init(sbwd, myPreci);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetPrecision(const double preci)
{
  myPreci = preci;
  myDone  = false;
}

//=================================================================================================

void ShapeAnalysis_WireVertex::Analyze()
{
  if (myStat.IsNull())
    return;
  myDone = true;
  //  Analyse des vertex qui se suivent
  occ::handle<Geom_Curve> c1, c2;
  double                  cf, cl, upre, ufol;
  int                     i, j, nb = myStat->Length(), stat;
  ShapeAnalysis_Edge      EA;
  for (i = 1; i <= nb; i++)
  {
    stat = -1; // au depart

    j                  = (i == nb ? 1 : i + 1);
    TopoDS_Edge   E1   = myWire->Edge(i);
    TopoDS_Edge   E2   = myWire->Edge(j);
    TopoDS_Vertex V1   = EA.LastVertex(myWire->Edge(i));
    TopoDS_Vertex V2   = EA.FirstVertex(myWire->Edge(j));
    gp_Pnt        PV1  = BRep_Tool::Pnt(V1);
    gp_Pnt        PV2  = BRep_Tool::Pnt(V2);
    double        tol1 = BRep_Tool::Tolerance(V1);
    double        tol2 = BRep_Tool::Tolerance(V2);
    EA.Curve3d(myWire->Edge(i), c1, cf, upre);
    EA.Curve3d(myWire->Edge(j), c2, ufol, cl);
    if (c1.IsNull() || c2.IsNull())
      continue; // on ne peut rien faire ...
    gp_Pnt P1 = c1->Value(upre);
    gp_Pnt P2 = c2->Value(ufol);

    //   Est-ce que le jeu de vertex convient ? (meme si V1 == V2, on verifie)
    double d1 = PV1.Distance(P1);
    double d2 = PV2.Distance(P2);
    double dd = PV1.Distance(PV2);
    if (d1 <= tol1 && d2 <= tol2 && dd <= (tol1 + tol2))
      stat = 1;
    else if (d1 <= myPreci && d2 <= myPreci && dd <= myPreci)
      stat = 2;
    myStat->SetValue(i, -1); // par defaut
    if (stat > 0)
    {
      if (V1 == V2)
        stat = 0;
    }
    if (stat >= 0)
    {
      myStat->SetValue(i, stat);
      continue;
    }
    //    Restent les autres cas !

    //    Une edge se termine sur l autre : il faudra simplement relimiter
    //    Projection calculee sur une demi-edge (pour eviter les pbs de couture)
    gp_Pnt PJ1, PJ2;
    double U1, U2;
    double dj1 = ShapeAnalysis_Curve().Project(c1, P2, myPreci, PJ1, U1, (cf + upre) / 2, upre);
    double dj2 = ShapeAnalysis_Curve().Project(c2, P1, myPreci, PJ2, U2, ufol, (ufol + cl) / 2);
    if (dj1 <= myPreci)
    {
      SetStart(i, PJ1.XYZ(), U1);
      continue;
    }
    else if (dj2 <= myPreci)
    {
      SetEnd(i, PJ2.XYZ(), U2);
      continue;
    }

    //    Restent a verifier les intersections et prolongations !
  }
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetSameVertex(const int num)
{
  myStat->SetValue(num, 0);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetSameCoords(const int num)
{
  myStat->SetValue(num, 1);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetClose(const int num)
{
  myStat->SetValue(num, 2);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetEnd(const int num, const gp_XYZ& pos, const double ufol)
{
  myStat->SetValue(num, 3);
  myPos->SetValue(num, pos);
  myUFol->SetValue(num, ufol);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetStart(const int num, const gp_XYZ& pos, const double upre)
{
  myStat->SetValue(num, 4);
  myPos->SetValue(num, pos);
  myUFol->SetValue(num, upre);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetInters(const int     num,
                                         const gp_XYZ& pos,
                                         const double  upre,
                                         const double  ufol)
{
  myStat->SetValue(num, 5);
  myPos->SetValue(num, pos);
  myUPre->SetValue(num, upre);
  myUFol->SetValue(num, ufol);
}

//=================================================================================================

void ShapeAnalysis_WireVertex::SetDisjoined(const int num)
{
  myStat->SetValue(num, -1);
}

//=================================================================================================

bool ShapeAnalysis_WireVertex::IsDone() const
{
  return myDone;
}

//=================================================================================================

double ShapeAnalysis_WireVertex::Precision() const
{
  return myPreci;
}

//=================================================================================================

int ShapeAnalysis_WireVertex::NbEdges() const
{
  return myWire->NbEdges();
}

//=================================================================================================

const occ::handle<ShapeExtend_WireData>& ShapeAnalysis_WireVertex::WireData() const
{
  return myWire;
}

//=================================================================================================

int ShapeAnalysis_WireVertex::Status(const int num) const
{
  return myStat->Value(num);
}

//=================================================================================================

gp_XYZ ShapeAnalysis_WireVertex::Position(const int num) const
{
  return myPos->Value(num);
}

//=================================================================================================

// szv#4:S4163:12Mar99 was bug: returned Integer
double ShapeAnalysis_WireVertex::UPrevious(const int num) const
{
  return myUPre->Value(num);
}

//=================================================================================================

// szv#4:S4163:12Mar99 was bug: returned Integer
double ShapeAnalysis_WireVertex::UFollowing(const int num) const
{
  return myUFol->Value(num);
}

//=================================================================================================

int ShapeAnalysis_WireVertex::Data(const int num, gp_XYZ& pos, double& upre, double& ufol) const
{
  pos  = myPos->Value(num);
  upre = myUPre->Value(num);
  ufol = myUFol->Value(num);
  return myStat->Value(num);
}

//=================================================================================================

int ShapeAnalysis_WireVertex::NextStatus(const int stat, const int num) const
{
  // szv#4:S4163:12Mar99 optimized
  if (!myStat.IsNull())
  {
    int i, nb = myStat->Length();
    for (i = num + 1; i <= nb; i++)
      if (myStat->Value(i) == stat)
        return i;
  }
  return 0;
}

//=================================================================================================

int ShapeAnalysis_WireVertex::NextCriter(const int crit, const int num) const
{
  // szv#4:S4163:12Mar99 optimized
  if (!myStat.IsNull())
  {
    int i, nb = myStat->Length();
    for (i = num + 1; i <= nb; i++)
    {
      int stat = myStat->Value(i);
      if ((crit == -1 && stat < 0) || (crit == 0 && stat == 0) || (crit == 1 && stat > 0)
          || (crit == 2 && (stat >= 0 && stat <= 2)) || (crit == 3 && (stat == 1 || stat == 2))
          || (crit == 4 && stat > 2))
        return i;
    }
  }
  return 0;
}
