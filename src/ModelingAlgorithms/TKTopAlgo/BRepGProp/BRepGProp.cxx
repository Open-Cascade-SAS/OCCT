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

#include <BRepGProp.hxx>
#include <BRepGProp_Cinert.hxx>
#include <BRepGProp_Sinert.hxx>
#include <BRepGProp_Vinert.hxx>
#include <BRepGProp_MeshProps.hxx>
#include <BRepGProp_MeshCinert.hxx>
#include <BRepGProp_VinertGK.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Domain.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <BRepCheck_Shell.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#ifdef OCCT_DEBUG
static int AffichEps = 0;
#endif

namespace
{

//! Helper to transform GProp_GProps by accessing protected members.
//! Used for solid-level caching: compute properties once per unique solid,
//! then transform for duplicate instances at different locations.
class GPropTransformer : public GProp_GProps
{
public:
  GPropTransformer(const GProp_GProps& theSource)
      : GProp_GProps(theSource)
  {
  }

  //! Apply a rigid transform to the stored properties.
  //! Transforms reference point (loc) as absolute point, gravity center offset (g)
  //! as relative vector (rotation only, no translation), and rotates inertia tensor.
  void ApplyTransform(const gp_Trsf& theTrsf)
  {
    loc.Transform(theTrsf);
    const gp_TrsfForm aForm = theTrsf.Form();
    if (aForm != gp_Identity && aForm != gp_Translation)
    {
      // g is a relative offset from loc to center of mass — rotate only.
      const gp_Mat aR    = theTrsf.VectorialPart();
      gp_XYZ       aGxyz = g.XYZ();
      aGxyz.Multiply(aR);
      g.SetXYZ(aGxyz);
      const gp_Mat aRt = aR.Transposed();
      inertia          = aR.Multiplied(inertia).Multiplied(aRt);
    }
  }

  //! Negate volume contribution (for reversed solid orientation).
  void Negate()
  {
    dim = -dim;
    inertia.Multiply(-1.0);
  }
};

} // anonymous namespace

static gp_Pnt roughBaryCenter(const TopoDS_Shape& S)
{
  int             i;
  TopExp_Explorer ex;
  gp_XYZ          xyz(0, 0, 0);
  for (ex.Init(S, TopAbs_VERTEX), i = 0; ex.More(); ex.Next(), i++)
    xyz += BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())).XYZ();
  if (i > 0)
  {
    xyz /= i;
  }
  else
  {
    // Try using triangulation
    ex.Init(S, TopAbs_FACE);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Shape&                    aF = ex.Current();
      TopLoc_Location                        aLocDummy;
      const occ::handle<Poly_Triangulation>& aTri =
        BRep_Tool::Triangulation(TopoDS::Face(aF), aLocDummy);
      if (!aTri.IsNull() && aTri->NbNodes() > 0)
      {
        xyz = aTri->Node(1).XYZ();
        if (!aLocDummy.IsIdentity())
        {
          aLocDummy.Transformation().Transforms(xyz);
        }
        break;
      }
    }
  }
  return gp_Pnt(xyz);
}

void BRepGProp::LinearProperties(const TopoDS_Shape& S,
                                 GProp_GProps&       SProps,
                                 const bool          SkipShared,
                                 const bool          UseTriangulation)
{
  // find the origin
  gp_Pnt P(0, 0, 0);
  P.Transform(S.Location());
  SProps = GProp_GProps(P);

  BRepAdaptor_Curve                                      BAC;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anEMap;
  TopExp_Explorer                                        ex;
  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(ex.Current());
    if (SkipShared && !anEMap.Add(aE))
    {
      continue;
    }

    occ::handle<NCollection_HArray1<gp_Pnt>> theNodes;
    bool                                     IsGeom = BRep_Tool::IsGeometric(aE);
    if (UseTriangulation || !IsGeom)
    {
      BRepGProp_MeshCinert::PreparePolygon(aE, theNodes);
    }
    if (!theNodes.IsNull())
    {
      BRepGProp_MeshCinert MG;
      MG.SetLocation(P);
      MG.Perform(theNodes->Array1());
      SProps.Add(MG);
    }
    else
    {
      if (IsGeom)
      {
        BAC.Initialize(aE);
        BRepGProp_Cinert CG(BAC, P);
        SProps.Add(CG);
      }
    }
  }
}

static double surfaceProperties(const TopoDS_Shape& S,
                                GProp_GProps&       Props,
                                const double        Eps,
                                const bool          SkipShared,
                                const bool          UseTriangulation)
{
  int i;
#ifdef OCCT_DEBUG
  int iErrorMax = 0;
#endif
  double           ErrorMax = 0.0, Error;
  TopExp_Explorer  ex;
  gp_Pnt           P(roughBaryCenter(S));
  BRepGProp_Sinert G;
  G.SetLocation(P);
  BRepGProp_MeshProps MG(BRepGProp_MeshProps::Sinert);
  MG.SetLocation(P);

  BRepGProp_Face                                         BF;
  BRepGProp_Domain                                       BD;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFMap;
  TopLoc_Location                                        aLocDummy;

  for (ex.Init(S, TopAbs_FACE), i = 1; ex.More(); ex.Next(), i++)
  {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    if (SkipShared && !aFMap.Add(F))
    {
      continue;
    }

    bool NoSurf = false, NoTri = false;
    {
      const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(F, aLocDummy);
      if (aSurf.IsNull())
      {
        NoSurf = true;
      }
      const occ::handle<Poly_Triangulation>& aTri = BRep_Tool::Triangulation(F, aLocDummy);
      if (aTri.IsNull() || aTri->NbNodes() == 0 || aTri->NbTriangles() == 0)
      {
        NoTri = true;
      }
      if (NoTri && NoSurf)
      {
        continue;
      }
    }

    if ((UseTriangulation && !NoTri) || (NoSurf && !NoTri))
    {
      TopAbs_Orientation                     anOri = F.Orientation();
      const occ::handle<Poly_Triangulation>& aTri  = BRep_Tool::Triangulation(F, aLocDummy);
      MG.Perform(aTri, aLocDummy, anOri);
      Props.Add(MG);
    }
    else
    {
      BF.Load(F);
      bool IsNatRestr = (F.NbChildren() == 0);
      if (!IsNatRestr)
        BD.Init(F);
      if (Eps < 1.0)
      {
        G.Perform(BF, BD, Eps);
        Error = G.GetEpsilon();
        if (ErrorMax < Error)
        {
          ErrorMax = Error;
#ifdef OCCT_DEBUG
          iErrorMax = i;
#endif
        }
      }
      else
      {
        if (IsNatRestr)
          G.Perform(BF);
        else
          G.Perform(BF, BD);
      }
      Props.Add(G);
#ifdef OCCT_DEBUG
      if (AffichEps)
        std::cout << "\n" << i << ":\tEpsArea = " << G.GetEpsilon();
#endif
    }
  }
#ifdef OCCT_DEBUG
  if (AffichEps)
    std::cout << "\n-----------------\n" << iErrorMax << ":\tMaxError = " << ErrorMax << "\n";
#endif
  return ErrorMax;
}

void BRepGProp::SurfaceProperties(const TopoDS_Shape& S,
                                  GProp_GProps&       Props,
                                  const bool          SkipShared,
                                  const bool          UseTriangulation)
{
  // find the origin
  gp_Pnt P(0, 0, 0);
  P.Transform(S.Location());
  Props = GProp_GProps(P);
  surfaceProperties(S, Props, 1.0, SkipShared, UseTriangulation);
}

double BRepGProp::SurfaceProperties(const TopoDS_Shape& S,
                                    GProp_GProps&       Props,
                                    const double        Eps,
                                    const bool          SkipShared)
{
  // find the origin
  gp_Pnt P(0, 0, 0);
  P.Transform(S.Location());
  Props           = GProp_GProps(P);
  double ErrorMax = surfaceProperties(S, Props, Eps, SkipShared, false);
  return ErrorMax;
}

//=================================================================================================

//! Process faces of a shape for volume properties computation.
//! This is the core face-level integration loop used both for unique solids
//! and for shapes without shared solids.
static double volumePropertiesFaces(const TopoDS_Shape& S,
                                    GProp_GProps&       Props,
                                    const gp_Pnt&       theRefPnt,
                                    const double        Eps,
                                    const bool          SkipShared,
                                    const bool          UseTriangulation)
{
  int i;
#ifdef OCCT_DEBUG
  int iErrorMax = 0;
#endif
  double           ErrorMax = 0.0, Error = 0.0;
  TopExp_Explorer  ex;
  BRepGProp_Vinert G;
  G.SetLocation(theRefPnt);
  BRepGProp_MeshProps MG(BRepGProp_MeshProps::Vinert);
  MG.SetLocation(theRefPnt);

  BRepGProp_Face                                         BF;
  BRepGProp_Domain                                       BD;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFwdFMap;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aRvsFMap;
  TopLoc_Location                                        aLocDummy;

  for (ex.Init(S, TopAbs_FACE), i = 1; ex.More(); ex.Next(), i++)
  {
    const TopoDS_Face& F     = TopoDS::Face(ex.Current());
    TopAbs_Orientation anOri = F.Orientation();
    bool               isFwd = anOri == TopAbs_FORWARD;
    bool               isRvs = false;
    if (!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if (SkipShared)
    {
      if ((isFwd && !aFwdFMap.Add(F)) || (isRvs && !aRvsFMap.Add(F)))
      {
        continue;
      }
    }
    bool NoSurf = false, NoTri = false;
    {
      const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(F, aLocDummy);
      if (aSurf.IsNull())
      {
        NoSurf = true;
      }
      const occ::handle<Poly_Triangulation>& aTri = BRep_Tool::Triangulation(F, aLocDummy);
      if (aTri.IsNull() || aTri->NbNodes() == 0 || aTri->NbTriangles() == 0)
      {
        NoTri = true;
      }
      if (NoTri && NoSurf)
      {
        continue;
      }
    }

    if (isFwd || isRvs)
    {
      if ((UseTriangulation && !NoTri) || (NoSurf && !NoTri))
      {
        const occ::handle<Poly_Triangulation>& aTri = BRep_Tool::Triangulation(F, aLocDummy);
        MG.Perform(aTri, aLocDummy, anOri);
        Props.Add(MG);
      }
      else
      {
        BF.Load(F);
        bool IsNatRestr = (F.NbChildren() == 0);
        if (!IsNatRestr)
          BD.Init(F);
        if (Eps < 1.0)
        {
          G.Perform(BF, BD, Eps);
          Error = G.GetEpsilon();
          if (ErrorMax < Error)
          {
            ErrorMax = Error;
#ifdef OCCT_DEBUG
            iErrorMax = i;
#endif
          }
        }
        else
        {
          if (IsNatRestr)
            G.Perform(BF);
          else
            G.Perform(BF, BD);
        }
        Props.Add(G);
#ifdef OCCT_DEBUG
        if (AffichEps)
          std::cout << "\n" << i << ":\tEpsVolume = " << G.GetEpsilon();
#endif
      }
    }
  }
#ifdef OCCT_DEBUG
  if (AffichEps)
    std::cout << "\n-----------------\n" << iErrorMax << ":\tMaxError = " << ErrorMax << "\n";
#endif
  return ErrorMax;
}

//=================================================================================================

static double volumeProperties(const TopoDS_Shape& S,
                               GProp_GProps&       Props,
                               const double        Eps,
                               const bool          SkipShared,
                               const bool          UseTriangulation)
{
  const gp_Pnt P(roughBaryCenter(S));

  // Check for shared solids: if the same TShape appears multiple times
  // (via compound nesting with different locations), we compute properties
  // once per unique solid and reuse via rigid transform for duplicates.
  bool hasSharedSolids = false;
  {
    NCollection_Map<opencascade::handle<TopoDS_TShape>> aSeenSolids;
    for (TopExp_Explorer exS(S, TopAbs_SOLID); exS.More(); exS.Next())
    {
      if (!aSeenSolids.Add(exS.Current().TShape()))
      {
        hasSharedSolids = true;
        break;
      }
    }
  }

  if (!hasSharedSolids)
  {
    // No shared solids: use direct face-level iteration (original path).
    return volumePropertiesFaces(S, Props, P, Eps, SkipShared, UseTriangulation);
  }

  // Shared solids detected: iterate at solid level with caching.
  // For each unique TShape, compute properties once via face integration.
  // For duplicate instances, transform the cached result by the relative
  // location and add to Props (using Huygens theorem via GProp_GProps::Add).
  struct SolidCacheEntry
  {
    GProp_GProps       Props;
    TopLoc_Location    Location;
    TopAbs_Orientation Orientation;
  };

  NCollection_DataMap<opencascade::handle<TopoDS_TShape>, SolidCacheEntry> aSolidCache;

  double ErrorMax = 0.0;

  for (TopExp_Explorer exS(S, TopAbs_SOLID); exS.More(); exS.Next())
  {
    const TopoDS_Shape&                       aSolid = exS.Current();
    const opencascade::handle<TopoDS_TShape>& aTS    = aSolid.TShape();

    if (aSolidCache.IsBound(aTS))
    {
      // Duplicate instance: transform cached properties.
      const SolidCacheEntry& aCached = aSolidCache(aTS);
      const TopLoc_Location  aRelLoc = aSolid.Location().Multiplied(aCached.Location.Inverted());

      // When SkipShared is enabled, skip exact duplicate instances
      // (same placement and orientation) — matches original face-level dedup behavior.
      if (SkipShared && aRelLoc.IsIdentity() && aSolid.Orientation() == aCached.Orientation)
      {
        continue;
      }

      GPropTransformer aTransformed(aCached.Props);
      if (!aRelLoc.IsIdentity())
      {
        aTransformed.ApplyTransform(aRelLoc.Transformation());
      }
      if (aSolid.Orientation() != aCached.Orientation)
      {
        aTransformed.Negate();
      }
      Props.Add(aTransformed);
    }
    else
    {
      // First instance of this TShape: compute via face integration.
      GProp_GProps aSolidProps(P);
      const double anError =
        volumePropertiesFaces(aSolid, aSolidProps, P, Eps, SkipShared, UseTriangulation);
      if (ErrorMax < anError)
      {
        ErrorMax = anError;
      }

      SolidCacheEntry anEntry;
      anEntry.Props       = aSolidProps;
      anEntry.Location    = aSolid.Location();
      anEntry.Orientation = aSolid.Orientation();
      aSolidCache.Bind(aTS, anEntry);

      Props.Add(aSolidProps);
    }
  }

  // Handle faces not belonging to any solid (free shells/faces in the compound).
  {
    bool            hasFree = false;
    TopoDS_Compound aFreeComp;
    BRep_Builder    aBld;
    aBld.MakeCompound(aFreeComp);
    for (TopExp_Explorer exF(S, TopAbs_FACE, TopAbs_SOLID); exF.More(); exF.Next())
    {
      aBld.Add(aFreeComp, exF.Current());
      hasFree = true;
    }
    if (hasFree)
    {
      GProp_GProps aFreeProps(P);
      const double aFreeError =
        volumePropertiesFaces(aFreeComp, aFreeProps, P, Eps, SkipShared, UseTriangulation);
      if (ErrorMax < aFreeError)
      {
        ErrorMax = aFreeError;
      }
      Props.Add(aFreeProps);
    }
  }

  return ErrorMax;
}

void BRepGProp::VolumeProperties(const TopoDS_Shape& S,
                                 GProp_GProps&       Props,
                                 const bool          OnlyClosed,
                                 const bool          SkipShared,
                                 const bool          UseTriangulation)
{
  // find the origin
  gp_Pnt P(0, 0, 0);
  P.Transform(S.Location());
  Props = GProp_GProps(P);
  if (OnlyClosed)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShMap;
    TopExp_Explorer                                        ex(S, TopAbs_SHELL);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Shape& Sh = ex.Current();
      if (SkipShared && !aShMap.Add(Sh))
      {
        continue;
      }
      if (BRep_Tool::IsClosed(Sh))
        volumeProperties(Sh, Props, 1.0, SkipShared, UseTriangulation);
    }
  }
  else
    volumeProperties(S, Props, 1.0, SkipShared, UseTriangulation);
}

//=================================================================================================

double BRepGProp::VolumeProperties(const TopoDS_Shape& S,
                                   GProp_GProps&       Props,
                                   const double        Eps,
                                   const bool          OnlyClosed,
                                   const bool          SkipShared)
{
  // find the origin
  gp_Pnt P(0, 0, 0);
  P.Transform(S.Location());
  Props = GProp_GProps(P);
  int i;
#ifdef OCCT_DEBUG
  int iErrorMax = 0;
#endif
  double ErrorMax = 0.0, Error = 0.0;
  if (OnlyClosed)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShMap;
    TopExp_Explorer                                        ex(S, TopAbs_SHELL);
    for (i = 1; ex.More(); ex.Next(), i++)
    {
      const TopoDS_Shape& Sh = ex.Current();
      if (SkipShared && !aShMap.Add(Sh))
      {
        continue;
      }
      if (BRep_Tool::IsClosed(Sh))
      {
        Error = volumeProperties(Sh, Props, Eps, SkipShared, false);
        if (ErrorMax < Error)
        {
          ErrorMax = Error;
#ifdef OCCT_DEBUG
          iErrorMax = i;
#endif
        }
      }
    }
  }
  else
    ErrorMax = volumeProperties(S, Props, Eps, SkipShared, false);
#ifdef OCCT_DEBUG
  if (AffichEps)
    std::cout << "\n\n===================" << iErrorMax << ":\tMaxEpsVolume = " << ErrorMax << "\n";
#endif
  return ErrorMax;
}

//===========================================================================================//
// Volume properties by Gauss-Kronrod integration
//===========================================================================================//
//=================================================================================================

static double volumePropertiesGK(const TopoDS_Shape& theShape,
                                 GProp_GProps&       theProps,
                                 const double        theTol,
                                 const bool          IsUseSpan,
                                 const bool          CGFlag,
                                 const bool          IFlag,
                                 const bool          SkipShared)
{
  TopExp_Explorer anExp;
  anExp.Init(theShape, TopAbs_FACE);

  double aTol = theTol;

  // Compute properties.
  gp_Pnt                                                 aLoc(roughBaryCenter(theShape));
  BRepGProp_VinertGK                                     aVProps;
  BRepGProp_Face                                         aPropFace(IsUseSpan);
  BRepGProp_Domain                                       aPropDomain;
  double                                                 aLocalError;
  double                                                 anError = 0.;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFwdFMap;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aRvsFMap;
  TopLoc_Location                                        aLocDummy;

  aVProps.SetLocation(aLoc);

  for (; anExp.More(); anExp.Next())
  {
    TopoDS_Face        aFace = TopoDS::Face(anExp.Current());
    TopAbs_Orientation anOri = aFace.Orientation();
    bool               isFwd = anOri == TopAbs_FORWARD;
    bool               isRvs = false;
    if (!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if (SkipShared)
    {
      if ((isFwd && !aFwdFMap.Add(aFace)) || (isRvs && !aRvsFMap.Add(aFace)))
      {
        continue;
      }
    }
    {
      const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(aFace, aLocDummy);
      if (aSurf.IsNull())
      {
        // skip faces without geometry
        continue;
      }
    }

    if (isFwd || isRvs)
    {
      aPropFace.Load(aFace);

      bool IsNatRestr = (aFace.NbChildren() == 0);
      if (IsNatRestr)
        aLocalError = aVProps.Perform(aPropFace, aTol, CGFlag, IFlag);
      else
      {
        aPropDomain.Init(aFace);
        aLocalError = aVProps.Perform(aPropFace, aPropDomain, aTol, CGFlag, IFlag);
      }

      if (aLocalError < 0.)
        return aLocalError;

      anError += aLocalError;
      theProps.Add(aVProps);
    }
  }

  return anError;
}

//=================================================================================================

double BRepGProp::VolumePropertiesGK(const TopoDS_Shape& S,
                                     GProp_GProps&       Props,
                                     const double        Eps,
                                     const bool          OnlyClosed,
                                     const bool          IsUseSpan,
                                     const bool          CGFlag,
                                     const bool          IFlag,
                                     const bool          SkipShared)
{
  gp_Pnt P(0, 0, 0);
  double anError = 0.;

  P.Transform(S.Location());
  Props = GProp_GProps(P);

  if (OnlyClosed)
  {
    // To select closed shells.
    TopExp_Explorer                                        anExp;
    NCollection_List<TopoDS_Shape>                         aClosedShells;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShMap;

    anExp.Init(S, TopAbs_SHELL);

    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aShell = anExp.Current();
      if (SkipShared && !aShMap.Add(aShell))
      {
        continue;
      }

      BRepCheck_Shell  aChecker(TopoDS::Shell(aShell));
      BRepCheck_Status aStatus = aChecker.Closed(false);

      if (aStatus == BRepCheck_NoError)
        aClosedShells.Append(aShell);
    }

    if (aClosedShells.IsEmpty())
      return -1.;

    // Compute the properties for each closed shell.
    double                                   aTol = Eps;
    double                                   aLocalError;
    NCollection_List<TopoDS_Shape>::Iterator anIter(aClosedShells);

    for (; anIter.More(); anIter.Next())
    {
      const TopoDS_Shape& aShell = anIter.Value();

      aLocalError = volumePropertiesGK(aShell, Props, aTol, IsUseSpan, CGFlag, IFlag, SkipShared);

      if (aLocalError < 0)
        return aLocalError;

      anError += aLocalError;
    }
  }
  else
    anError = volumePropertiesGK(S, Props, Eps, IsUseSpan, CGFlag, IFlag, SkipShared);

  double vol = Props.Mass();
  if (vol > Epsilon(1.))
    anError /= vol;
  return anError;
}

//=================================================================================================

static double volumePropertiesGK(const TopoDS_Shape& theShape,
                                 GProp_GProps&       theProps,
                                 const gp_Pln&       thePln,
                                 const double        theTol,
                                 const bool          IsUseSpan,
                                 const bool          CGFlag,
                                 const bool          IFlag,
                                 const bool          SkipShared)
{
  TopExp_Explorer anExp;
  anExp.Init(theShape, TopAbs_FACE);

  double aTol = theTol;

  // Compute properties.
  gp_Pnt                                                 aLoc(roughBaryCenter(theShape));
  BRepGProp_VinertGK                                     aVProps;
  BRepGProp_Face                                         aPropFace(IsUseSpan);
  BRepGProp_Domain                                       aPropDomain;
  double                                                 aLocalError;
  double                                                 anError = 0.;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aFwdFMap;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aRvsFMap;
  TopLoc_Location                                        aLocDummy;

  aVProps.SetLocation(aLoc);

  for (; anExp.More(); anExp.Next())
  {
    TopoDS_Face        aFace = TopoDS::Face(anExp.Current());
    TopAbs_Orientation anOri = aFace.Orientation();
    bool               isFwd = anOri == TopAbs_FORWARD;
    bool               isRvs = false;
    if (!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if (SkipShared)
    {
      if ((isFwd && !aFwdFMap.Add(aFace)) || (isRvs && !aRvsFMap.Add(aFace)))
      {
        continue;
      }
    }
    {
      const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(aFace, aLocDummy);
      if (aSurf.IsNull())
      {
        // skip faces without geometry
        continue;
      }
    }

    if (isFwd || isRvs)
    {
      aPropFace.Load(aFace);

      bool IsNatRestr = (aFace.NbChildren() == 0);
      if (IsNatRestr)
        aLocalError = aVProps.Perform(aPropFace, thePln, aTol, CGFlag, IFlag);
      else
      {
        aPropDomain.Init(aFace);
        aLocalError = aVProps.Perform(aPropFace, aPropDomain, thePln, aTol, CGFlag, IFlag);
      }

      if (aLocalError < 0.)
        return aLocalError;

      anError += aLocalError;
      theProps.Add(aVProps);
    }
  }

  return anError;
}

//=================================================================================================

double BRepGProp::VolumePropertiesGK(const TopoDS_Shape& S,
                                     GProp_GProps&       Props,
                                     const gp_Pln&       thePln,
                                     const double        Eps,
                                     const bool          OnlyClosed,
                                     const bool          IsUseSpan,
                                     const bool          CGFlag,
                                     const bool          IFlag,
                                     const bool          SkipShared)
{
  gp_Pnt P(0, 0, 0);
  double anError = 0.;

  P.Transform(S.Location());
  Props = GProp_GProps(P);

  if (OnlyClosed)
  {
    // To select closed shells.
    TopExp_Explorer                                        anExp;
    NCollection_List<TopoDS_Shape>                         aClosedShells;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShMap;

    anExp.Init(S, TopAbs_SHELL);

    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aShell = anExp.Current();
      if (SkipShared && !aShMap.Add(aShell))
      {
        continue;
      }

      BRepCheck_Shell  aChecker(TopoDS::Shell(aShell));
      BRepCheck_Status aStatus = aChecker.Closed(false);

      if (aStatus == BRepCheck_NoError)
        aClosedShells.Append(aShell);
    }

    if (aClosedShells.IsEmpty())
      return -1.;

    // Compute the properties for each closed shell.
    double                                   aTol = Eps;
    double                                   aLocalError;
    NCollection_List<TopoDS_Shape>::Iterator anIter(aClosedShells);

    for (; anIter.More(); anIter.Next())
    {
      const TopoDS_Shape& aShell = anIter.Value();

      aLocalError =
        volumePropertiesGK(aShell, Props, thePln, aTol, IsUseSpan, CGFlag, IFlag, SkipShared);

      if (aLocalError < 0)
        return aLocalError;

      anError += aLocalError;
    }
  }
  else
    anError = volumePropertiesGK(S, Props, thePln, Eps, IsUseSpan, CGFlag, IFlag, SkipShared);

  double vol = Props.Mass();
  if (vol > Epsilon(1.))
    anError /= vol;

  return anError;
}
