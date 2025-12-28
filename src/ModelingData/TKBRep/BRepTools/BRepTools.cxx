// Created on: 1993-01-21
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

#include <BRepTools.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Message.hxx>
#include <OSD_FileSystem.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <errno.h>
#include <BRepTools_Modifier.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

//=================================================================================================

void BRepTools::UVBounds(const TopoDS_Face& F,
                         double&            UMin,
                         double&            UMax,
                         double&            VMin,
                         double&            VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F, B);
  if (!B.IsVoid())
  {
    B.Get(UMin, VMin, UMax, VMax);
  }
  else
  {
    UMin = UMax = VMin = VMax = 0.0;
  }
}

//=================================================================================================

void BRepTools::UVBounds(const TopoDS_Face& F,
                         const TopoDS_Wire& W,
                         double&            UMin,
                         double&            UMax,
                         double&            VMin,
                         double&            VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F, W, B);
  if (!B.IsVoid())
  {
    B.Get(UMin, VMin, UMax, VMax);
  }
  else
  {
    UMin = UMax = VMin = VMax = 0.0;
  }
}

//=================================================================================================

void BRepTools::UVBounds(const TopoDS_Face& F,
                         const TopoDS_Edge& E,
                         double&            UMin,
                         double&            UMax,
                         double&            VMin,
                         double&            VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F, E, B);
  if (!B.IsVoid())
  {
    B.Get(UMin, VMin, UMax, VMax);
  }
  else
  {
    UMin = UMax = VMin = VMax = 0.0;
  }
}

//=================================================================================================

void BRepTools::AddUVBounds(const TopoDS_Face& FF, Bnd_Box2d& B)
{
  TopoDS_Face F = FF;
  F.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex(F, TopAbs_EDGE);

  // fill box for the given face
  Bnd_Box2d aBox;
  for (; ex.More(); ex.Next())
  {
    BRepTools::AddUVBounds(F, TopoDS::Edge(ex.Current()), aBox);
  }

  // if the box is empty (face without edges or without pcurves),
  // get natural bounds
  if (aBox.IsVoid())
  {
    double                           UMin, UMax, VMin, VMax;
    TopLoc_Location                  L;
    const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(F, L);
    if (aSurf.IsNull())
    {
      return;
    }

    aSurf->Bounds(UMin, UMax, VMin, VMax);
    aBox.Update(UMin, VMin, UMax, VMax);
  }

  // add face box to result
  B.Add(aBox);
}

//=================================================================================================

void BRepTools::AddUVBounds(const TopoDS_Face& F, const TopoDS_Wire& W, Bnd_Box2d& B)
{
  TopExp_Explorer ex;
  for (ex.Init(W, TopAbs_EDGE); ex.More(); ex.Next())
  {
    BRepTools::AddUVBounds(F, TopoDS::Edge(ex.Current()), B);
  }
}

//=================================================================================================

void BRepTools::AddUVBounds(const TopoDS_Face& aF, const TopoDS_Edge& aE, Bnd_Box2d& aB)
{
  double          aT1, aT2, aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  double          aUmin, aUmax, aVmin, aVmax;
  Bnd_Box2d       aBoxC, aBoxS;
  TopLoc_Location aLoc;
  //
  const occ::handle<Geom2d_Curve> aC2D = BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
  if (aC2D.IsNull())
  {
    return;
  }
  //
  BndLib_Add2dCurve::Add(aC2D, aT1, aT2, 0., aBoxC);
  if (!aBoxC.IsVoid())
  {
    aBoxC.Get(aXmin, aYmin, aXmax, aYmax);
  }
  //
  occ::handle<Geom_Surface> aS = BRep_Tool::Surface(aF, aLoc);
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);

  if (aS->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    const occ::handle<Geom_RectangularTrimmedSurface> aSt =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aS);
    aS = aSt->BasisSurface();
  }

  //
  if (!aS->IsUPeriodic())
  {
    bool isUPeriodic = false;

    // Additional verification for U-periodicity for B-spline surfaces
    // 1. Verify that the surface is U-closed (if such flag is false). Verification uses 2 points
    // 2. Verify periodicity of surface inside UV-bounds of the edge. Verification uses 3 or 6
    // points.
    if (aS->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface) && (aXmin < aUmin || aXmax > aUmax))
    {
      constexpr double aTol2 = 100 * Precision::Confusion() * Precision::Confusion();
      isUPeriodic            = true;
      gp_Pnt P1, P2;
      // 1. Verify that the surface is U-closed
      if (!aS->IsUClosed())
      {
        double aVStep = aVmax - aVmin;
        for (double aV = aVmin; aV <= aVmax; aV += aVStep)
        {
          P1 = aS->Value(aUmin, aV);
          P2 = aS->Value(aUmax, aV);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isUPeriodic = false;
            break;
          }
        }
      }
      // 2. Verify periodicity of surface inside UV-bounds of the edge
      if (isUPeriodic) // the flag still not changed
      {
        double aV = (aVmin + aVmax) * 0.5;
        double aU[6];   // values of U lying out of surface boundaries
        double aUpp[6]; // corresponding U-values plus/minus period
        int    aNbPnt = 0;
        if (aXmin < aUmin)
        {
          aU[0]   = aXmin;
          aU[1]   = (aXmin + aUmin) * 0.5;
          aU[2]   = aUmin;
          aUpp[0] = aU[0] + aUmax - aUmin;
          aUpp[1] = aU[1] + aUmax - aUmin;
          aUpp[2] = aU[2] + aUmax - aUmin;
          aNbPnt += 3;
        }
        if (aXmax > aUmax)
        {
          aU[aNbPnt]       = aUmax;
          aU[aNbPnt + 1]   = (aXmax + aUmax) * 0.5;
          aU[aNbPnt + 2]   = aXmax;
          aUpp[aNbPnt]     = aU[aNbPnt] - aUmax + aUmin;
          aUpp[aNbPnt + 1] = aU[aNbPnt + 1] - aUmax + aUmin;
          aUpp[aNbPnt + 2] = aU[aNbPnt + 2] - aUmax + aUmin;
          aNbPnt += 3;
        }
        for (int anInd = 0; anInd < aNbPnt; anInd++)
        {
          P1 = aS->Value(aU[anInd], aV);
          P2 = aS->Value(aUpp[anInd], aV);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isUPeriodic = false;
            break;
          }
        }
      }
    }

    if (!isUPeriodic)
    {
      if ((aXmin < aUmin) && (aUmin < aXmax))
      {
        aXmin = aUmin;
      }
      if ((aXmin < aUmax) && (aUmax < aXmax))
      {
        aXmax = aUmax;
      }
    }
  }

  if (!aS->IsVPeriodic())
  {
    bool isVPeriodic = false;

    // Additional verification for V-periodicity for B-spline surfaces
    // 1. Verify that the surface is V-closed (if such flag is false). Verification uses 2 points
    // 2. Verify periodicity of surface inside UV-bounds of the edge. Verification uses 3 or 6
    // points.
    if (aS->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface) && (aYmin < aVmin || aYmax > aVmax))
    {
      constexpr double aTol2 = 100 * Precision::Confusion() * Precision::Confusion();
      isVPeriodic            = true;
      gp_Pnt P1, P2;
      // 1. Verify that the surface is V-closed
      if (!aS->IsVClosed())
      {
        double aUStep = aUmax - aUmin;
        for (double aU = aUmin; aU <= aUmax; aU += aUStep)
        {
          P1 = aS->Value(aU, aVmin);
          P2 = aS->Value(aU, aVmax);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isVPeriodic = false;
            break;
          }
        }
      }
      // 2. Verify periodicity of surface inside UV-bounds of the edge
      if (isVPeriodic) // the flag still not changed
      {
        double aU = (aUmin + aUmax) * 0.5;
        double aV[6];   // values of V lying out of surface boundaries
        double aVpp[6]; // corresponding V-values plus/minus period
        int    aNbPnt = 0;
        if (aYmin < aVmin)
        {
          aV[0]   = aYmin;
          aV[1]   = (aYmin + aVmin) * 0.5;
          aV[2]   = aVmin;
          aVpp[0] = aV[0] + aVmax - aVmin;
          aVpp[1] = aV[1] + aVmax - aVmin;
          aVpp[2] = aV[2] + aVmax - aVmin;
          aNbPnt += 3;
        }
        if (aYmax > aVmax)
        {
          aV[aNbPnt]       = aVmax;
          aV[aNbPnt + 1]   = (aYmax + aVmax) * 0.5;
          aV[aNbPnt + 2]   = aYmax;
          aVpp[aNbPnt]     = aV[aNbPnt] - aVmax + aVmin;
          aVpp[aNbPnt + 1] = aV[aNbPnt + 1] - aVmax + aVmin;
          aVpp[aNbPnt + 2] = aV[aNbPnt + 2] - aVmax + aVmin;
          aNbPnt += 3;
        }
        for (int anInd = 0; anInd < aNbPnt; anInd++)
        {
          P1 = aS->Value(aU, aV[anInd]);
          P2 = aS->Value(aU, aVpp[anInd]);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isVPeriodic = false;
            break;
          }
        }
      }
    }

    if (!isVPeriodic)
    {
      if ((aYmin < aVmin) && (aVmin < aYmax))
      {
        aYmin = aVmin;
      }
      if ((aYmin < aVmax) && (aVmax < aYmax))
      {
        aYmax = aVmax;
      }
    }
  }

  aBoxS.Update(aXmin, aYmin, aXmax, aYmax);

  aB.Add(aBoxS);
}

//=================================================================================================

void BRepTools::Update(const TopoDS_Vertex&) {}

//=================================================================================================

void BRepTools::Update(const TopoDS_Edge&) {}

//=================================================================================================

void BRepTools::Update(const TopoDS_Wire&) {}

//=================================================================================================

void BRepTools::Update(const TopoDS_Face& F)
{
  if (!F.Checked())
  {
    UpdateFaceUVPoints(F);
    F.TShape()->Checked(true);
  }
}

//=================================================================================================

void BRepTools::Update(const TopoDS_Shell& S)
{
  TopExp_Explorer ex(S, TopAbs_FACE);
  while (ex.More())
  {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=================================================================================================

void BRepTools::Update(const TopoDS_Solid& S)
{
  TopExp_Explorer ex(S, TopAbs_FACE);
  while (ex.More())
  {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=================================================================================================

void BRepTools::Update(const TopoDS_CompSolid& CS)
{
  TopExp_Explorer ex(CS, TopAbs_FACE);
  while (ex.More())
  {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=================================================================================================

void BRepTools::Update(const TopoDS_Compound& C)
{
  TopExp_Explorer ex(C, TopAbs_FACE);
  while (ex.More())
  {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=================================================================================================

void BRepTools::Update(const TopoDS_Shape& S)
{
  switch (S.ShapeType())
  {

    case TopAbs_VERTEX:
      Update(TopoDS::Vertex(S));
      break;

    case TopAbs_EDGE:
      Update(TopoDS::Edge(S));
      break;

    case TopAbs_WIRE:
      Update(TopoDS::Wire(S));
      break;

    case TopAbs_FACE:
      Update(TopoDS::Face(S));
      break;

    case TopAbs_SHELL:
      Update(TopoDS::Shell(S));
      break;

    case TopAbs_SOLID:
      Update(TopoDS::Solid(S));
      break;

    case TopAbs_COMPSOLID:
      Update(TopoDS::CompSolid(S));
      break;

    case TopAbs_COMPOUND:
      Update(TopoDS::Compound(S));
      break;

    default:
      break;
  }
}

//=======================================================================
// function : UpdateFaceUVPoints
// purpose  : Reset the UV points of edges on the Face
//=======================================================================
void BRepTools::UpdateFaceUVPoints(const TopoDS_Face& theF)
{
  // For each edge of the face <F> reset the UV points to the bounding
  // points of the parametric curve of the edge on the face.

  // Get surface of the face
  TopLoc_Location                  aLoc;
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(theF, aLoc);
  // Iterate on edges and reset UV points
  TopExp_Explorer anExpE(theF, TopAbs_EDGE);
  for (; anExpE.More(); anExpE.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(anExpE.Current());

    const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&aE.TShape());
    if (TE->Locked())
      return;

    const TopLoc_Location aELoc = aLoc.Predivided(aE.Location());
    // Edge representations
    NCollection_List<occ::handle<BRep_CurveRepresentation>>&          aLCR = TE->ChangeCurves();
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itLCR(aLCR);
    for (; itLCR.More(); itLCR.Next())
    {
      occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itLCR.Value());
      if (!GC.IsNull() && GC->IsCurveOnSurface(aSurf, aELoc))
      {
        // Update UV points
        GC->Update();
        break;
      }
    }
  }
}

//=================================================================================================

bool BRepTools::Compare(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2)
{
  if (V1.IsSame(V2))
    return true;
  gp_Pnt p1 = BRep_Tool::Pnt(V1);
  gp_Pnt p2 = BRep_Tool::Pnt(V2);
  double l  = p1.Distance(p2);
  if (l <= BRep_Tool::Tolerance(V1))
    return true;
  if (l <= BRep_Tool::Tolerance(V2))
    return true;
  return false;
}

//=================================================================================================

bool BRepTools::Compare(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
  if (E1.IsSame(E2))
    return true;
  return false;
}

//=================================================================================================

TopoDS_Wire BRepTools::OuterWire(const TopoDS_Face& F)
{
  TopoDS_Wire     Wres;
  TopExp_Explorer expw(F, TopAbs_WIRE);

  if (expw.More())
  {
    Wres = TopoDS::Wire(expw.Current());
    expw.Next();
    if (expw.More())
    {
      double UMin, UMax, VMin, VMax;
      double umin, umax, vmin, vmax;
      BRepTools::UVBounds(F, Wres, UMin, UMax, VMin, VMax);
      while (expw.More())
      {
        const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
        BRepTools::UVBounds(F, W, umin, umax, vmin, vmax);
        if (((umin - UMin) <= Precision::PConfusion())
            && ((umax - UMax) >= -Precision::PConfusion())
            && ((vmin - VMin) <= Precision::PConfusion())
            && ((vmax - VMax) >= -Precision::PConfusion()))
        {
          Wres = W;
          UMin = umin;
          UMax = umax;
          VMin = vmin;
          VMax = vmax;
        }
        expw.Next();
      }
    }
  }
  return Wres;
}

//=================================================================================================

void BRepTools::Map3DEdges(const TopoDS_Shape&                                            S,
                           NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& M)
{
  TopExp_Explorer Ex;
  for (Ex.Init(S, TopAbs_EDGE); Ex.More(); Ex.Next())
  {
    if (!BRep_Tool::Degenerated(TopoDS::Edge(Ex.Current())))
      M.Add(Ex.Current());
  }
}

//=================================================================================================

void BRepTools::Dump(const TopoDS_Shape& Sh, Standard_OStream& S)
{
  BRepTools_ShapeSet SS;
  SS.Add(Sh);
  SS.Dump(Sh, S);
  SS.Dump(S);
}

//=================================================================================================

void BRepTools::Write(const TopoDS_Shape&          theShape,
                      Standard_OStream&            theStream,
                      const bool                   theWithTriangles,
                      const bool                   theWithNormals,
                      const TopTools_FormatVersion theVersion,
                      const Message_ProgressRange& theProgress)
{
  BRepTools_ShapeSet aShapeSet(theWithTriangles, theWithNormals);
  aShapeSet.SetFormatNb(theVersion);
  aShapeSet.Add(theShape);
  aShapeSet.Write(theStream, theProgress);
  aShapeSet.Write(theShape, theStream);
}

//=================================================================================================

void BRepTools::Read(TopoDS_Shape&                Sh,
                     std::istream&                S,
                     const BRep_Builder&          B,
                     const Message_ProgressRange& theProgress)
{
  BRepTools_ShapeSet SS(B);
  SS.Read(S, theProgress);
  SS.Read(Sh, S);
}

//=================================================================================================

bool BRepTools::Write(const TopoDS_Shape&          theShape,
                      const char*                  theFile,
                      const bool                   theWithTriangles,
                      const bool                   theWithNormals,
                      const TopTools_FormatVersion theVersion,
                      const Message_ProgressRange& theProgress)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream>      aStream =
    aFileSystem->OpenOStream(theFile, std::ios::out | std::ios::binary);
  if (aStream.get() == NULL || !aStream->good())
  {
    return false;
  }

  bool isGood = (aStream->good() && !aStream->eof());
  if (!isGood)
    return isGood;

  BRepTools_ShapeSet SS(theWithTriangles, theWithNormals);
  SS.SetFormatNb(theVersion);
  SS.Add(theShape);

  *aStream << "DBRep_DrawableShape\n"; // for easy Draw read
  SS.Write(*aStream, theProgress);
  isGood = aStream->good();
  if (isGood)
  {
    SS.Write(theShape, *aStream);
  }
  aStream->flush();
  isGood = aStream->good();

  errno  = 0;
  isGood = aStream->good() && isGood && !errno;
  aStream.reset();

  return isGood;
}

//=================================================================================================

bool BRepTools::Read(TopoDS_Shape&                Sh,
                     const char*                  File,
                     const BRep_Builder&          B,
                     const Message_ProgressRange& theProgress)
{
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aStream     = aFileSystem->OpenIStream(File, std::ios::in);
  if (aStream.get() == NULL)
  {
    return false;
  }
  BRepTools_ShapeSet SS(B);
  SS.Read(*aStream, theProgress);
  if (!SS.NbShapes())
    return false;
  SS.Read(Sh, *aStream);
  return true;
}

//=================================================================================================

void BRepTools::Clean(const TopoDS_Shape& theShape, const bool theForce)
{
  if (theShape.IsNull())
    return;

  BRep_Builder                             aBuilder;
  occ::handle<Poly_Triangulation>          aNullTriangulation;
  occ::handle<Poly_PolygonOnTriangulation> aNullPoly;

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  const TopLoc_Location                                  anEmptyLoc;

  TopExp_Explorer aFaceIt(theShape, TopAbs_FACE);
  for (; aFaceIt.More(); aFaceIt.Next())
  {
    TopoDS_Shape aFaceNoLoc = aFaceIt.Value();
    aFaceNoLoc.Location(anEmptyLoc);
    if (!aShapeMap.Add(aFaceNoLoc))
    {
      // the face has already been processed
      continue;
    }

    const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());
    if (!BRep_Tool::IsGeometric(aFace))
    {
      // Do not remove triangulation as there is no surface to recompute it.
      continue;
    }

    TopLoc_Location                        aLoc;
    const occ::handle<Poly_Triangulation>& aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);

    if (aTriangulation.IsNull())
      continue;

    // Nullify edges
    // Theoretically, the edges on the face (with surface) may have no geometry
    // (no curve 3d or 2d or both). Such faces should be considered as invalid and
    // are not supported by current implementation. So, both triangulation of the face
    // and polygon on triangulation of the edges are removed unconditionally.
    TopExp_Explorer aEdgeIt(aFace, TopAbs_EDGE);
    for (; aEdgeIt.More(); aEdgeIt.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(aEdgeIt.Current());
      aBuilder.UpdateEdge(anEdge, aNullPoly, aTriangulation, aLoc);
    }

    aBuilder.UpdateFace(aFace, aNullTriangulation);
  }

  // Iterate over all edges seeking for 3d polygons
  occ::handle<Poly_Polygon3D> aNullPoly3d;
  TopExp_Explorer             aEdgeIt(theShape, TopAbs_EDGE);
  for (; aEdgeIt.More(); aEdgeIt.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aEdgeIt.Value());
    anEdge.Location(anEmptyLoc);

    if (!aShapeMap.Add(anEdge))
    {
      // the edge has already been processed
      continue;
    }

    if (!BRep_Tool::IsGeometric(TopoDS::Edge(anEdge)))
    {
      // Do not remove polygon 3d as there is no curve to recompute it.
      continue;
    }

    TopLoc_Location             aLoc;
    occ::handle<Poly_Polygon3D> aPoly3d = BRep_Tool::Polygon3D(anEdge, aLoc);

    if (!aPoly3d.IsNull())
    {
      aBuilder.UpdateEdge(anEdge, aNullPoly3d);
    }
    if (theForce)
    {
      occ::handle<BRep_CurveRepresentation> aCR;
      BRep_TEdge*                           aTE = static_cast<BRep_TEdge*>(anEdge.TShape().get());
      NCollection_List<occ::handle<BRep_CurveRepresentation>>&          aLCR = aTE->ChangeCurves();
      NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator anIterCR(aLCR);

      // find and remove all representations
      while (anIterCR.More())
      {
        aCR = anIterCR.Value();
        if (aCR->IsPolygonOnTriangulation())
        {
          aLCR.Remove(anIterCR);
        }
        else
        {
          anIterCR.Next();
        }
      }
      aTE->Modified(true);
    }
  }
}

//=================================================================================================

void BRepTools::CleanGeometry(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return;

  BRep_Builder aBuilder;

  for (TopExp_Explorer aFaceIt(theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
  {
    TopLoc_Location                  aLocation;
    const TopoDS_Face&               aFace    = TopoDS::Face(aFaceIt.Current());
    const occ::handle<Geom_Surface>& aSurface = BRep_Tool::Surface(aFace, aLocation);

    for (TopExp_Explorer aEdgeIt(aFace, TopAbs_EDGE); aEdgeIt.More(); aEdgeIt.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(aEdgeIt.Current());
      aBuilder.UpdateEdge(anEdge,
                          occ::handle<Geom2d_Curve>(),
                          aSurface,
                          aLocation,
                          BRep_Tool::Tolerance(anEdge));
    }

    aBuilder.UpdateFace(aFace,
                        occ::handle<Geom_Surface>(),
                        aFace.Location(),
                        BRep_Tool::Tolerance(aFace));
  }

  for (TopExp_Explorer aEdgeIt2(theShape, TopAbs_EDGE); aEdgeIt2.More(); aEdgeIt2.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(aEdgeIt2.Current());
    aBuilder.UpdateEdge(anEdge,
                        occ::handle<Geom_Curve>(),
                        TopLoc_Location(),
                        BRep_Tool::Tolerance(anEdge));
  }

  RemoveUnusedPCurves(theShape);
}

//=================================================================================================

void BRepTools::RemoveUnusedPCurves(const TopoDS_Shape& S)
{
  NCollection_Map<occ::handle<Standard_Transient>> UsedSurfaces;

  TopExp_Explorer Explo(S, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Face               aFace = TopoDS::Face(Explo.Current());
    TopLoc_Location           aLoc;
    occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace, aLoc);
    UsedSurfaces.Add(aSurf);
  }

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Emap;
  TopExp::MapShapes(S, TopAbs_EDGE, Emap);

  int i;
  for (i = 1; i <= Emap.Extent(); i++)
  {
    const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&Emap(i).TShape());
    NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrep(lcr);
    while (itrep.More())
    {
      bool ToRemove = false;

      occ::handle<BRep_CurveRepresentation> CurveRep = itrep.Value();
      if (CurveRep->IsCurveOnSurface())
      {
        occ::handle<Geom_Surface> aSurface = CurveRep->Surface();
        if (!UsedSurfaces.Contains(aSurface))
          ToRemove = true;
      }
      else if (CurveRep->IsRegularity())
      {
        occ::handle<Geom_Surface> Surf1 = CurveRep->Surface();
        occ::handle<Geom_Surface> Surf2 = CurveRep->Surface2();
        ToRemove = (!UsedSurfaces.Contains(Surf1) || !UsedSurfaces.Contains(Surf2));
      }

      if (ToRemove)
        lcr.Remove(itrep);
      else
        itrep.Next();
    }
  }
}

//=================================================================================================

bool BRepTools::Triangulation(const TopoDS_Shape& theShape,
                              const double        theLinDefl,
                              const bool          theToCheckFreeEdges)
{
  TopExp_Explorer anEdgeIter;
  TopLoc_Location aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&                     aFace = TopoDS::Face(aFaceIter.Current());
    const occ::handle<Poly_Triangulation>& aTri  = BRep_Tool::Triangulation(aFace, aDummyLoc);
    if (aTri.IsNull() || aTri->Deflection() > theLinDefl)
    {
      return false;
    }

    for (anEdgeIter.Init(aFace, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
    {
      const TopoDS_Edge&                              anEdge = TopoDS::Edge(anEdgeIter.Current());
      const occ::handle<Poly_PolygonOnTriangulation>& aPoly =
        BRep_Tool::PolygonOnTriangulation(anEdge, aTri, aDummyLoc);
      if (aPoly.IsNull())
      {
        return false;
      }
    }
  }
  if (!theToCheckFreeEdges)
  {
    return true;
  }

  occ::handle<Poly_Triangulation> anEdgeTri;
  for (anEdgeIter.Init(theShape, TopAbs_EDGE, TopAbs_FACE); anEdgeIter.More(); anEdgeIter.Next())
  {
    const TopoDS_Edge&                 anEdge   = TopoDS::Edge(anEdgeIter.Current());
    const occ::handle<Poly_Polygon3D>& aPolygon = BRep_Tool::Polygon3D(anEdge, aDummyLoc);
    if (!aPolygon.IsNull())
    {
      if (aPolygon->Deflection() > theLinDefl)
      {
        return false;
      }
    }
    else
    {
      const occ::handle<Poly_PolygonOnTriangulation>& aPoly =
        BRep_Tool::PolygonOnTriangulation(anEdge, anEdgeTri, aDummyLoc);
      if (aPoly.IsNull() || anEdgeTri.IsNull() || anEdgeTri->Deflection() > theLinDefl)
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

bool BRepTools::LoadTriangulation(const TopoDS_Shape&                theShape,
                                  const int                          theTriangulationIdx,
                                  const bool                         theToSetAsActive,
                                  const occ::handle<OSD_FileSystem>& theFileSystem)
{
  Standard_ASSERT_RAISE(theTriangulationIdx >= -1, "Invalid negative triangulation index!");

  bool                               wasLoaded = false;
  BRep_Builder                       aBuilder;
  TopLoc_Location                    aDummyLoc;
  const occ::handle<OSD_FileSystem>& aFileSystem =
    !theFileSystem.IsNull() ? theFileSystem : OSD_FileSystem::DefaultFileSystem();
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&              aFace = TopoDS::Face(aFaceIter.Current());
    occ::handle<Poly_Triangulation> aTriangulation;
    if (theTriangulationIdx == -1)
    {
      // load an active triangulation
      aTriangulation = BRep_Tool::Triangulation(aFace, aDummyLoc);
    }
    else
    {
      const NCollection_List<occ::handle<Poly_Triangulation>>& aTriangulations =
        BRep_Tool::Triangulations(aFace, aDummyLoc);
      if (theTriangulationIdx >= aTriangulations.Size())
      {
        // triangulation index is out of range
        continue;
      }
      int aTriangulationIdx = 0;
      for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(aTriangulations);
           anIter.More();
           anIter.Next(), aTriangulationIdx++)
      {
        if (aTriangulationIdx != theTriangulationIdx)
        {
          continue;
        }
        aTriangulation = anIter.Value();
        break;
      }
    }
    if (aTriangulation.IsNull() || !aTriangulation->HasDeferredData())
    {
      // NULL triangulation, already loaded triangulation or triangulation without deferred storage
      // cannot be loaded
      continue;
    }
    if (aTriangulation->LoadDeferredData(aFileSystem))
    {
      wasLoaded = true;
      if (theToSetAsActive && (theTriangulationIdx != -1)) // triangulation is already active
      {
        aBuilder.UpdateFace(aFace, aTriangulation, false);
      }
    }
  }
  return wasLoaded;
}

//=================================================================================================

bool BRepTools::LoadAllTriangulations(const TopoDS_Shape&                theShape,
                                      const occ::handle<OSD_FileSystem>& theFileSystem)
{
  bool                               wasLoaded = false;
  TopLoc_Location                    aDummyLoc;
  const occ::handle<OSD_FileSystem>& aFileSystem =
    !theFileSystem.IsNull() ? theFileSystem : OSD_FileSystem::DefaultFileSystem();
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIter.Current());
    for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(
           BRep_Tool::Triangulations(aFace, aDummyLoc));
         anIter.More();
         anIter.Next())
    {
      const occ::handle<Poly_Triangulation>& aTriangulation = anIter.Value();
      if (aTriangulation.IsNull() || !aTriangulation->HasDeferredData())
      {
        // NULL triangulation, already loaded triangulation or triangulation without deferred
        // storage cannot be loaded
        continue;
      }
      wasLoaded = aTriangulation->LoadDeferredData(aFileSystem);
    }
  }
  return wasLoaded;
}

//=================================================================================================

bool BRepTools::UnloadTriangulation(const TopoDS_Shape& theShape, const int theTriangulationIdx)
{
  Standard_ASSERT_RAISE(theTriangulationIdx >= -1, "Invalid negative triangulation index!");

  bool            wasUnloaded = false;
  TopLoc_Location aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&              aFace = TopoDS::Face(aFaceIter.Current());
    occ::handle<Poly_Triangulation> aTriangulation;
    if (theTriangulationIdx == -1)
    {
      // unload an active triangulation
      aTriangulation = BRep_Tool::Triangulation(aFace, aDummyLoc);
    }
    else
    {
      int                                                      aTriangulationIdx = 0;
      const NCollection_List<occ::handle<Poly_Triangulation>>& aTriangulations =
        BRep_Tool::Triangulations(aFace, aDummyLoc);
      if (theTriangulationIdx >= aTriangulations.Size())
      {
        // triangulation index is out of range
        continue;
      }
      for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(aTriangulations);
           anIter.More();
           anIter.Next(), aTriangulationIdx++)
      {
        if (aTriangulationIdx != theTriangulationIdx)
        {
          continue;
        }
        aTriangulation = anIter.Value();
        break;
      }
    }
    if (aTriangulation.IsNull() || !aTriangulation->HasDeferredData())
    {
      // NULL triangulation or triangulation without deferred storage cannot be unloaded
      continue;
    }
    wasUnloaded = aTriangulation->UnloadDeferredData();
  }
  return wasUnloaded;
}

//=================================================================================================

bool BRepTools::UnloadAllTriangulations(const TopoDS_Shape& theShape)
{
  bool            wasUnloaded = false;
  TopLoc_Location aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face&              aFace = TopoDS::Face(aFaceIter.Current());
    occ::handle<Poly_Triangulation> aTriangulation;
    for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(
           BRep_Tool::Triangulations(aFace, aDummyLoc));
         anIter.More();
         anIter.Next())
    {
      aTriangulation = anIter.Value();
      if (aTriangulation.IsNull() || !aTriangulation->HasDeferredData())
      {
        // NULL triangulation or triangulation without deferred storage cannot be unloaded
        continue;
      }
      wasUnloaded = aTriangulation->UnloadDeferredData();
    }
  }
  return wasUnloaded;
}

//=================================================================================================

bool BRepTools::ActivateTriangulation(const TopoDS_Shape& theShape,
                                      const int           theTriangulationIdx,
                                      const bool          theToActivateStrictly)
{
  Standard_ASSERT_RAISE(theTriangulationIdx > -1, "Invalid negative triangulation index!");

  bool            wasActivated = false;
  BRep_Builder    aBuilder;
  TopLoc_Location aDummyLoc;
  for (TopExp_Explorer aFaceIter(theShape, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Face& aFace             = TopoDS::Face(aFaceIter.Current());
    int                aTriangulationIdx = theTriangulationIdx;
    const NCollection_List<occ::handle<Poly_Triangulation>>& aTriangulations =
      BRep_Tool::Triangulations(aFace, aDummyLoc);
    const int aTriangulationsNb = aTriangulations.Size();
    if (theTriangulationIdx >= aTriangulationsNb)
    {
      // triangulation index is out of range
      if (theToActivateStrictly)
      {
        // skip activation
        continue;
      }
      // use last available
      aTriangulationIdx = aTriangulationsNb - 1;
    }
    occ::handle<Poly_Triangulation> anActiveTriangulation;
    int                             aTriangulationIter = 0;
    for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(aTriangulations);
         anIter.More();
         anIter.Next(), aTriangulationIter++)
    {
      if (aTriangulationIter != aTriangulationIdx)
      {
        continue;
      }
      anActiveTriangulation = anIter.Value();
      break;
    }
    if (anActiveTriangulation.IsNull())
    {
      continue;
    }
    aBuilder.UpdateFace(aFace, anActiveTriangulation, false);
    wasActivated = true;
  }
  return wasActivated;
}

//=================================================================================================

bool BRepTools::IsReallyClosed(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  if (!BRep_Tool::IsClosed(E, F))
  {
    return false;
  }
  int             nbocc = 0;
  TopExp_Explorer exp;
  for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(E))
    {
      nbocc++;
    }
  }
  return nbocc == 2;
}

//=================================================================================================

void BRepTools::DetectClosedness(const TopoDS_Face& theFace, bool& theUclosed, bool& theVclosed)
{
  theUclosed = theVclosed = false;

  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    if (BRep_Tool::IsClosed(anEdge, theFace) && BRepTools::IsReallyClosed(anEdge, theFace))
    {
      double                    fpar, lpar;
      occ::handle<Geom2d_Curve> PCurve1 = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);
      occ::handle<Geom2d_Curve> PCurve2 =
        BRep_Tool::CurveOnSurface(TopoDS::Edge(anEdge.Reversed()), theFace, fpar, lpar);
      gp_Pnt2d Point1 = PCurve1->Value(fpar);
      gp_Pnt2d Point2 = PCurve2->Value(fpar);
      bool     IsUiso = (std::abs(Point1.X() - Point2.X()) > std::abs(Point1.Y() - Point2.Y()));
      if (IsUiso)
        theUclosed = true;
      else
        theVclosed = true;
    }
  }
}

//=================================================================================================

double BRepTools::EvalAndUpdateTol(const TopoDS_Edge&               theE,
                                   const occ::handle<Geom_Curve>&   C3d,
                                   const occ::handle<Geom2d_Curve>& C2d,
                                   const occ::handle<Geom_Surface>& S,
                                   const double                     f,
                                   const double                     l)
{
  double newtol = 0.;
  double first = f, last = l;
  // Set first, last to avoid ErrosStatus = 2 because of
  // too strong checking of limits in class CheckCurveOnSurface
  //
  if (!C3d->IsPeriodic())
  {
    first = std::max(first, C3d->FirstParameter());
    last  = std::min(last, C3d->LastParameter());
  }
  if (!C2d->IsPeriodic())
  {
    first = std::max(first, C2d->FirstParameter());
    last  = std::min(last, C2d->LastParameter());
  }
  const occ::handle<Adaptor3d_Curve> aGeomAdaptorCurve = new GeomAdaptor_Curve(C3d, first, last);

  occ::handle<Adaptor2d_Curve2d>   aGeom2dAdaptorCurve = new Geom2dAdaptor_Curve(C2d, first, last);
  occ::handle<GeomAdaptor_Surface> aGeomAdaptorSurface = new GeomAdaptor_Surface(S);

  occ::handle<Adaptor3d_CurveOnSurface> anAdaptor3dCurveOnSurface =
    new Adaptor3d_CurveOnSurface(aGeom2dAdaptorCurve, aGeomAdaptorSurface);

  GeomLib_CheckCurveOnSurface CT(aGeomAdaptorCurve);
  CT.Perform(anAdaptor3dCurveOnSurface);
  if (CT.IsDone())
  {
    newtol = CT.MaxDistance();
  }
  else
  {
    if (CT.ErrorStatus() == 3
        || (CT.ErrorStatus() == 2 && (C3d->IsPeriodic() || C2d->IsPeriodic())))
    {
      // Try to estimate by sample points
      int    nbint = 22;
      double dt    = (last - first) / nbint;
      dt           = std::max(dt, Precision::Confusion());
      double   d, dmax = 0.;
      gp_Pnt2d aP2d;
      gp_Pnt   aPC, aPS;
      int      cnt = 0;
      double   t   = first;
      for (; t <= last; t += dt)
      {
        cnt++;
        C2d->D0(t, aP2d);
        C3d->D0(t, aPC);
        S->D0(aP2d.X(), aP2d.Y(), aPS);
        d = aPS.SquareDistance(aPC);
        if (d > dmax)
        {
          dmax = d;
        }
      }
      if (cnt < nbint + 1)
      {
        t = last;
        C2d->D0(t, aP2d);
        C3d->D0(t, aPC);
        S->D0(aP2d.X(), aP2d.Y(), aPS);
        d = aPS.SquareDistance(aPC);
        if (d > dmax)
        {
          dmax = d;
        }
      }

      newtol = 1.2 * std::sqrt(dmax);
    }
  }
  double Tol = BRep_Tool::Tolerance(theE);
  if (newtol > Tol)
  {
    Tol = newtol;
    BRep_Builder B;
    B.UpdateEdge(theE, Tol);
  }

  return Tol;
}

//=================================================================================================

TopAbs_Orientation BRepTools::OriEdgeInFace(const TopoDS_Edge& E, const TopoDS_Face& F)

{
  TopExp_Explorer Exp(F.Oriented(TopAbs_FORWARD), TopAbs_EDGE);

  for (; Exp.More(); Exp.Next())
  {
    if (Exp.Current().IsSame(E))
    {
      return Exp.Current().Orientation();
    }
  }
  throw Standard_ConstructionError("BRepTools::OriEdgeInFace");
}

namespace
{
//=======================================================================
// function : findInternalsToKeep
// purpose  : Looks for internal sub-shapes which has to be kept to preserve
//           topological connectivity.
//=======================================================================
static void findInternalsToKeep(
  const TopoDS_Shape&                                     theS,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theAllNonInternals,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theAllInternals,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theShapesToKeep)
{
  for (TopoDS_Iterator it(theS, true); it.More(); it.Next())
  {
    const TopoDS_Shape& aSS = it.Value();
    findInternalsToKeep(aSS, theAllNonInternals, theAllInternals, theShapesToKeep);

    if (aSS.Orientation() == TopAbs_INTERNAL)
      theAllInternals.Add(aSS);
    else
      theAllNonInternals.Add(aSS);

    if (theAllNonInternals.Contains(aSS) && theAllInternals.Contains(aSS))
      theShapesToKeep.Add(aSS);
  }
}

//=======================================================================
// function : removeShapes
// purpose  : Removes sub-shapes from the shape
//=======================================================================
static void removeShapes(TopoDS_Shape& theS, const NCollection_List<TopoDS_Shape>& theLS)
{
  BRep_Builder aBB;
  bool         isFree = theS.Free();
  theS.Free(true);

  for (NCollection_List<TopoDS_Shape>::Iterator it(theLS); it.More(); it.Next())
  {
    aBB.Remove(theS, it.Value());
  }
  theS.Free(isFree);
}

//=======================================================================
// function : removeInternals
// purpose  : Removes recursively all internal sub-shapes from the given shape.
//           Returns true if all sub-shapes have been removed from the shape.
//=======================================================================
static bool removeInternals(
  TopoDS_Shape&                                                 theS,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* theShapesToKeep)
{
  NCollection_List<TopoDS_Shape> aLRemove;
  for (TopoDS_Iterator it(theS, true); it.More(); it.Next())
  {
    const TopoDS_Shape& aSS = it.Value();
    if (aSS.Orientation() == TopAbs_INTERNAL)
    {
      if (!theShapesToKeep || !theShapesToKeep->Contains(aSS))
        aLRemove.Append(aSS);
    }
    else
    {
      if (removeInternals(*(TopoDS_Shape*)&aSS, theShapesToKeep))
        aLRemove.Append(aSS);
    }
  }

  int aNbSToRemove = aLRemove.Extent();
  if (aNbSToRemove)
  {
    removeShapes(theS, aLRemove);
    return (theS.NbChildren() == 0);
  }
  return false;
}

} // namespace

//=================================================================================================

void BRepTools::RemoveInternals(TopoDS_Shape& theS, const bool theForce)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>*pMKeep = NULL, aMKeep;
  if (!theForce)
  {
    // Find all internal sub-shapes which has to be kept to preserve topological connectivity.
    // Note that if the multi-connected shape is not directly contained in some shape,
    // but as a part of bigger sub-shape which will be removed, the multi-connected
    // shape is going to be removed also, breaking topological connectivity.
    // For instance, <theS> is a compound of the face and edge, which does not
    // belong to the face. The face contains internal wire and the edge shares
    // the vertex with one of the vertices of that wire. The vertex is not directly
    // contained in the face, thus will be removed as part of internal wire, and topological
    // connectivity between edge and face will be lost.
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anAllNonInternals, anAllInternals;
    findInternalsToKeep(theS, anAllNonInternals, anAllInternals, aMKeep);
    if (aMKeep.Extent())
      pMKeep = &aMKeep;
  }

  removeInternals(theS, pMKeep);
}

//=================================================================================================

void BRepTools::CheckLocations(const TopoDS_Shape&             theS,
                               NCollection_List<TopoDS_Shape>& theProblemShapes)
{
  if (theS.IsNull())
    return;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapS;
  TopExp::MapShapes(theS, aMapS, false, false);

  int i;
  for (i = 1; i <= aMapS.Extent(); ++i)
  {
    const TopoDS_Shape&    anS   = aMapS(i);
    const TopLoc_Location& aLoc  = anS.Location();
    const gp_Trsf&         aTrsf = aLoc.Transformation();
    bool                   isBadTrsf =
      aTrsf.IsNegative()
      || (std::abs(std::abs(aTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

    if (isBadTrsf)
    {
      theProblemShapes.Append(anS);
    }
  }
}