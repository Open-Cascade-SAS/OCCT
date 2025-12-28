// Created on: 1993-10-14
// Created by: Remi LEQUETTE
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

// Modified by skv - Fri Mar  4 15:50:09 2005
// Add methods for supporting history.

#include <BRep_TEdge.hxx>
#include <BRepLib.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepSweep_Revol.hxx>
#include <gp_Ax1.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <BRepTools_ReShape.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom_Line.hxx>
#include <Adaptor3d_Curve.hxx>

// perform checks on the argument
static const TopoDS_Shape& check(const TopoDS_Shape& S)
{
  BRepLib::BuildCurves3d(S);
  return S;
}

//=================================================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S,
                                             const gp_Ax1&       A,
                                             const double        D,
                                             const bool          Copy)
    : myRevol(check(S), A, D, Copy),
      myIsBuild(false)

{
  if (!CheckValidity(check(S), A))
  {
    myShape.Nullify();
    myIsBuild = true;
  }
  else
  {
    Build();
  }
}

//=================================================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S,
                                             const gp_Ax1&       A,
                                             const bool          Copy)
    :

      myRevol(check(S), A, 2. * M_PI, Copy),
      myIsBuild(false)
{

  if (!CheckValidity(check(S), A))
  {
    myShape.Nullify();
    myIsBuild = true;
  }
  else
  {
    Build();
  }
}

//=================================================================================================

const BRepSweep_Revol& BRepPrimAPI_MakeRevol::Revol() const
{
  return myRevol;
}

//=================================================================================================

void BRepPrimAPI_MakeRevol::Build(const Message_ProgressRange& /*theRange*/)
{
  if (myIsBuild)
  {
    return;
  }
  myShape = myRevol.Shape();
  BRepLib::UpdateInnerTolerances(myShape);

  Done();
  myIsBuild = true;

  myHist.Nullify();
  myDegenerated.Clear();
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDegE;
  BRep_Builder                                                                               aBB;

  TopExp_Explorer anExp(myShape, TopAbs_EDGE);
  // Problem is that some degenerated edges can be shared by different faces.
  // It is not valid for correct shape.
  // To solve problem it is possible to copy shared degenerated edge for each face, which has it,
  // and replace shared edge by its copy
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Shape&     anEdge = anExp.Current();
    occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());

    if (aTEdge->Degenerated())
    {
      NCollection_List<TopoDS_Shape>* anL = aDegE.ChangeSeek(anEdge);
      if (anL)
      {
        // Make the copy if degenerated edge occurs more then once
        TopoDS_Shape aCopyE = anEdge.EmptyCopied();
        aCopyE.Orientation(TopAbs_FORWARD);
        TopoDS_Iterator aVIter(anEdge.Oriented(TopAbs_FORWARD), false);
        for (; aVIter.More(); aVIter.Next())
        {
          aBB.Add(aCopyE, aVIter.Value());
        }
        aCopyE.Orientation(anEdge.Orientation());
        anL->Append(aCopyE);
        myDegenerated.Append(aCopyE);
      }
      else
      {
        anL = aDegE.Bound(anEdge, NCollection_List<TopoDS_Shape>());
        anL->Append(anEdge);
        myDegenerated.Append(anEdge);
      }
    }
  }
  if (!myDegenerated.IsEmpty())
  {
    BRepTools_ReShape aSubs;
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
         aDegF;
    bool isReplaced = false;
    anExp.Init(myShape, TopAbs_FACE);
    // Replace degenerated edge by its copies for different faces
    // First, for each face list of d.e. is created
    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aF = anExp.Current();
      TopExp_Explorer     anExpE(aF, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape& anE = anExpE.Current();
        if (BRep_Tool::Degenerated(TopoDS::Edge(anE)))
        {
          NCollection_List<TopoDS_Shape>* anL = aDegF.ChangeSeek(aF);
          if (!anL)
          {
            anL = aDegF.Bound(aF, NCollection_List<TopoDS_Shape>());
          }
          anL->Append(anE);
        }
      }
    }
    //
    // Second, replace edges by copies using ReShape
    BRepTools_ReShape aSubsF;
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
      Iterator aFIter(aDegF);
    for (; aFIter.More(); aFIter.Next())
    {
      aSubs.Clear();
      isReplaced                                    = false;
      const TopoDS_Shape&                      aF   = aFIter.Key();
      const NCollection_List<TopoDS_Shape>&    aDEL = aFIter.ChangeValue();
      NCollection_List<TopoDS_Shape>::Iterator anEIter(aDEL);
      for (; anEIter.More(); anEIter.Next())
      {
        const TopoDS_Shape& anE = anEIter.Value();
        if (aDegE.IsBound(anE))
        {
          NCollection_List<TopoDS_Shape>&          aCEL = aDegE.ChangeFind(anE);
          NCollection_List<TopoDS_Shape>::Iterator anIt(aCEL);
          for (; anIt.More(); anIt.Next())
          {
            if (anIt.Value().IsEqual(anE))
            {
              // First occurrence of initial deg. edge is not replaced
              aCEL.Remove(anIt);
              break;
            }
            if (anIt.Value().Orientation() == anE.Orientation())
            {
              // All other occurrences of anE are replaced by any copy
              // with suitable orientation
              isReplaced = true;
              aSubs.Replace(anE, anIt.Value());
              aCEL.Remove(anIt);
              break;
            }
          }
        }
      }
      if (isReplaced)
      {
        TopoDS_Shape aNF = aSubs.Apply(aF);
        aSubsF.Replace(aF, aNF);
        if (myHist.IsNull())
        {
          myHist = aSubs.History();
        }
        else
        {
          myHist->Merge(aSubs.History());
        }
        myShape = aSubsF.Apply(myShape);
        myHist->Merge(aSubsF.History());
        // Pair aF->aNF is in history after first replacing of edge by aNF = aSubs.Apply(aF)
        // After merging history for replacing faces, modified list for aF contains two exemplar of
        // aNF So, using ReplaceModified clears modified list for aF and leaves only one exemplar of
        // aNF
        myHist->ReplaceModified(aF, aNF);
        aSubsF.Clear();
      }
    }
  }
}

//=======================================================================
// function : IsIntersect
// purpose  : used in CheckValidity to find out is there
//           intersection between curve and axe of revolution
//=======================================================================
static bool IsIntersect(const occ::handle<Adaptor3d_Curve>& theC, const gp_Ax1& theAxe)
{
  const gp_Lin anAxis(theAxe);
  // Quick test for circle
  if (theC->GetType() == GeomAbs_Circle)
  {
    gp_Circ       aCirc  = theC->Circle();
    const gp_Pnt& aCentr = aCirc.Location();
    double        anR2   = aCirc.Radius();
    anR2 -= Precision::Confusion();
    anR2 *= anR2;
    if (anAxis.SquareDistance(aCentr) > anR2)
    {
      return false;
    }
  }
  const occ::handle<Geom_Line> aL = new Geom_Line(anAxis);
  const GeomAdaptor_Curve      aLin(aL);
  const double                 aParTol = theC->Resolution(Precision::Confusion());
  const double aParF = theC->FirstParameter() + aParTol, aParL = theC->LastParameter() - aParTol;

  Extrema_ExtCC anExtr(*theC, aLin);
  anExtr.Perform();
  if (anExtr.IsDone() && anExtr.NbExt() > 0)
  {
    Extrema_POnCurv aP1, aP2;
    for (int i = 1; i <= anExtr.NbExt(); i++)
    {
      if (anExtr.SquareDistance(i) > Precision::SquareConfusion())
      {
        continue;
      }
      anExtr.Points(i, aP1, aP2);
      if ((aParF < aP1.Parameter()) && (aP1.Parameter() < aParL))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool BRepPrimAPI_MakeRevol::CheckValidity(const TopoDS_Shape& theShape, const gp_Ax1& theA)
{
  TopExp_Explorer anExp(theShape, TopAbs_EDGE);
  bool            IsValid = true;
  for (; anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anE = TopoDS::Edge(anExp.Current());

    if (BRep_Tool::Degenerated(anE))
    {
      continue;
    }

    TopLoc_Location         L;
    double                  First, Last;
    occ::handle<Geom_Curve> C  = BRep_Tool::Curve(anE, L, First, Last);
    gp_Trsf                 Tr = L.Transformation();
    C                          = occ::down_cast<Geom_Curve>(C->Copy());
    C                          = new Geom_TrimmedCurve(C, First, Last);
    C->Transform(Tr);

    occ::handle<GeomAdaptor_Curve> HC = new GeomAdaptor_Curve();
    HC->Load(C, First, Last);
    // Checking coincidence axe of revolution and basis curve
    // This code is taken directly from GeomAdaptor_SurfaceOfRevolution
    int    Ratio = 1;
    double Dist;
    gp_Pnt PP;
    do
    {
      PP   = HC->Value(First + (Last - First) / Ratio);
      Dist = gp_Lin(theA).Distance(PP);
      Ratio++;
    } while (Dist < Precision::Confusion() && Ratio < 100);
    //
    if (Ratio >= 100) // edge coincides with axes
    {
      IsValid = true; // Such edges are allowed by revol algo and treated
                      // by special way, so they must be considered as valid
    }
    else
    {
      IsValid = !IsIntersect(HC, theA);
    }
    if (!IsValid)
    {
      break;
    }
  }
  //
  return IsValid;
}

//=================================================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape()
{
  return myRevol.FirstShape();
}

//=================================================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape()
{
  return myRevol.LastShape();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepPrimAPI_MakeRevol::Generated(const TopoDS_Shape& S)
{
  myGenerated.Clear();

  if (!myRevol.IsUsed(S))
  {
    return myGenerated;
  }

  TopoDS_Shape aGS = myRevol.Shape(S);
  if (!aGS.IsNull())
  {
    if (BRepTools_History::IsSupportedType(aGS))
    {
      if (aGS.ShapeType() == TopAbs_EDGE)
      {
        bool isDeg = BRep_Tool::Degenerated(TopoDS::Edge(aGS));
        if (isDeg)
        {
          NCollection_List<TopoDS_Shape>::Iterator anIt(myDegenerated);
          for (; anIt.More(); anIt.Next())
          {
            if (aGS.IsSame(anIt.Value()))
            {
              myGenerated.Append(aGS);
              if (!myHist.IsNull())
              {
                NCollection_List<TopoDS_Shape>::Iterator anIt1(myHist->Modified(aGS));
                for (; anIt1.More(); anIt1.Next())
                {
                  myGenerated.Append(anIt1.Value());
                }
                return myGenerated;
              }
            }
          }
          return myGenerated;
        }
      }
      //
      if (myHist.IsNull())
      {
        myGenerated.Append(aGS);
        return myGenerated;
      }
      //
      if (myHist->Modified(aGS).IsEmpty())
      {
        myGenerated.Append(aGS);
        return myGenerated;
      }
      //
      NCollection_List<TopoDS_Shape>::Iterator anIt(myHist->Modified(aGS));
      for (; anIt.More(); anIt.Next())
      {
        myGenerated.Append(anIt.Value());
      }
    }
  }
  return myGenerated;
}

//=================================================================================================

bool BRepPrimAPI_MakeRevol::IsDeleted(const TopoDS_Shape& S)
{
  return !myRevol.IsUsed(S);
}

//=======================================================================
// function : FirstShape
// purpose  : This method returns the shape of the beginning of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape(const TopoDS_Shape& theShape)
{
  return myRevol.FirstShape(theShape);
}

//=======================================================================
// function : LastShape
// purpose  : This method returns the shape of the end of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape(const TopoDS_Shape& theShape)
{
  return myRevol.LastShape(theShape);
}

//=================================================================================================

bool BRepPrimAPI_MakeRevol::HasDegenerated() const
{
  return (!myDegenerated.IsEmpty());
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepPrimAPI_MakeRevol::Degenerated() const
{
  return myDegenerated;
}
