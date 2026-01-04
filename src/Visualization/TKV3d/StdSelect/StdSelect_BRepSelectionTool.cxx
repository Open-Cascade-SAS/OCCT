// Created on: 1995-03-14
// Created by: Robert COUBLANC
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

#include <StdSelect_BRepSelectionTool.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <ElSLib.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <gp_Circ.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveCylinder.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveSphere.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_TypeOfSensitivity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NullObject.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#define BVH_PRIMITIVE_LIMIT 800000

namespace
{
// Check if face represents a full cylinder or cone surface
// (single wire with 4 edges: 2 seam edges and 2 circular edges)
static bool isCylinderOrCone(const TopoDS_Face& theFace)
{
  if (theFace.NbChildren() != 1)
    return false;

  const TopoDS_Iterator aWireIt(theFace);
  const TopoDS_Shape&   aWire = aWireIt.Value();
  if (aWire.ShapeType() != TopAbs_WIRE || aWire.NbChildren() != 4)
    return false;

  int aNbSeams = 0, aNbCirles = 0;
  for (TopoDS_Iterator anEdgeIt(aWire); anEdgeIt.More(); anEdgeIt.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());
    if (BRep_Tool::IsClosed(anEdge, theFace))
      ++aNbSeams;

    BRepAdaptor_Curve anAdaptor(anEdge);
    if (anAdaptor.GetType() == GeomAbs_Circle)
      ++aNbCirles;
  }
  return aNbSeams == 2 && aNbCirles == 2;
}
} // namespace

//==================================================
// function: PreBuildBVH
// purpose : Pre-builds BVH tree for heavyweight
//           sensitive entities with sub-elements
//           amount more than BVH_PRIMITIVE_LIMIT
//==================================================
void StdSelect_BRepSelectionTool::PreBuildBVH(const occ::handle<SelectMgr_Selection>& theSelection)
{
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const occ::handle<Select3D_SensitiveEntity>& aSensitive = aSelEntIter.Value()->BaseSensitive();
    if (aSensitive->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
    {
      aSensitive->BVH();
    }

    if (!aSensitive->IsInstance(STANDARD_TYPE(Select3D_SensitiveGroup)))
    {
      continue;
    }

    occ::handle<Select3D_SensitiveGroup> aGroup =
      occ::down_cast<Select3D_SensitiveGroup>(aSensitive);
    for (NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>::Iterator aSubEntitiesIter(
           aGroup->Entities());
         aSubEntitiesIter.More();
         aSubEntitiesIter.Next())
    {
      const occ::handle<Select3D_SensitiveEntity>& aSubEntity = aSubEntitiesIter.Value();
      if (aSubEntity->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
      {
        aSubEntity->BVH();
      }
    }
  }
}

//=================================================================================================

void StdSelect_BRepSelectionTool::Load(const occ::handle<SelectMgr_Selection>& theSelection,
                                       const TopoDS_Shape&                     theShape,
                                       const TopAbs_ShapeEnum                  theType,
                                       const double                            theDeflection,
                                       const double                            theDeviationAngle,
                                       const bool                              isAutoTriangulation,
                                       const int                               thePriority,
                                       const int                               theNbPOnEdge,
                                       const double                            theMaxParam)
{
  int aPriority = (thePriority == -1) ? GetStandardPriority(theShape, theType) : thePriority;
  if (isAutoTriangulation && !BRepTools::Triangulation(theShape, Precision::Infinite(), true))
  {
    BRepMesh_IncrementalMesh aMesher(theShape, theDeflection, false, theDeviationAngle);
  }

  occ::handle<StdSelect_BRepOwner> aBrepOwner;
  switch (theType)
  {
    case TopAbs_VERTEX:
    case TopAbs_EDGE:
    case TopAbs_WIRE:
    case TopAbs_FACE:
    case TopAbs_SHELL:
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID: {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubShapes;
      TopExp::MapShapes(theShape, theType, aSubShapes);

      bool isComesFromDecomposition = (aSubShapes.Extent() != 1) || !(theShape == aSubShapes(1));
      for (int aShIndex = 1; aShIndex <= aSubShapes.Extent(); ++aShIndex)
      {
        const TopoDS_Shape& aSubShape = aSubShapes(aShIndex);
        aBrepOwner = new StdSelect_BRepOwner(aSubShape, aPriority, isComesFromDecomposition);
        ComputeSensitive(aSubShape,
                         aBrepOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         isAutoTriangulation);
      }
      break;
    }
    default: {
      aBrepOwner = new StdSelect_BRepOwner(theShape, aPriority);
      ComputeSensitive(theShape,
                       aBrepOwner,
                       theSelection,
                       theDeflection,
                       theDeviationAngle,
                       theNbPOnEdge,
                       theMaxParam,
                       isAutoTriangulation);
    }
  }
}

//=================================================================================================

void StdSelect_BRepSelectionTool::Load(
  const occ::handle<SelectMgr_Selection>&        theSelection,
  const occ::handle<SelectMgr_SelectableObject>& theSelectableObj,
  const TopoDS_Shape&                            theShape,
  const TopAbs_ShapeEnum                         theType,
  const double                                   theDeflection,
  const double                                   theDeviationAngle,
  const bool                                     isAutoTriangulation,
  const int                                      thePriority,
  const int                                      theNbPOnEdge,
  const double                                   theMaxParam)
{
  Load(theSelection,
       theShape,
       theType,
       theDeflection,
       theDeviationAngle,
       isAutoTriangulation,
       thePriority,
       theNbPOnEdge,
       theMaxParam);

  // loading of selectables...
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const occ::handle<SelectMgr_EntityOwner>& anOwner =
      aSelEntIter.Value()->BaseSensitive()->OwnerId();
    anOwner->SetSelectable(theSelectableObj);
  }
}

//=================================================================================================

void StdSelect_BRepSelectionTool::ComputeSensitive(
  const TopoDS_Shape&                       theShape,
  const occ::handle<SelectMgr_EntityOwner>& theOwner,
  const occ::handle<SelectMgr_Selection>&   theSelection,
  const double                              theDeflection,
  const double                              theDeviationAngle,
  const int                                 theNbPOnEdge,
  const double                              theMaxParam,
  const bool                                isAutoTriangulation)
{
  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX: {
      theSelection->Add(
        new Select3D_SensitivePoint(theOwner, BRep_Tool::Pnt(TopoDS::Vertex(theShape))));
      break;
    }
    case TopAbs_EDGE: {
      occ::handle<Select3D_SensitiveEntity> aSensitive;
      GetEdgeSensitive(theShape,
                       theOwner,
                       theSelection,
                       theDeflection,
                       theDeviationAngle,
                       theNbPOnEdge,
                       theMaxParam,
                       aSensitive);
      if (!aSensitive.IsNull())
      {
        theSelection->Add(aSensitive);
      }
      break;
    }
    case TopAbs_WIRE: {
      BRepTools_WireExplorer                aWireExp(TopoDS::Wire(theShape));
      occ::handle<Select3D_SensitiveEntity> aSensitive;
      occ::handle<Select3D_SensitiveWire>   aWireSensitive = new Select3D_SensitiveWire(theOwner);
      theSelection->Add(aWireSensitive);
      while (aWireExp.More())
      {
        GetEdgeSensitive(aWireExp.Current(),
                         theOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         aSensitive);
        if (!aSensitive.IsNull())
        {
          aWireSensitive->Add(aSensitive);
        }
        aWireExp.Next();
      }
      break;
    }
    case TopAbs_FACE: {
      const TopoDS_Face& aFace = TopoDS::Face(theShape);

      NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>> aSensitiveList;
      GetSensitiveForFace(aFace,
                          theOwner,
                          aSensitiveList,
                          isAutoTriangulation,
                          theNbPOnEdge,
                          theMaxParam);
      for (NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>::Iterator aSensIter(
             aSensitiveList);
           aSensIter.More();
           aSensIter.Next())
      {
        theSelection->Add(aSensIter.Value());
      }
      break;
    }
    case TopAbs_SHELL:
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID: {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubfacesMap;
      TopExp::MapShapes(theShape, TopAbs_FACE, aSubfacesMap);

      if (!GetSensitiveForCylinder(aSubfacesMap, theOwner, theSelection))
      {
        for (int aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
        {
          ComputeSensitive(aSubfacesMap.FindKey(aShIndex),
                           theOwner,
                           theSelection,
                           theDeflection,
                           theDeviationAngle,
                           theNbPOnEdge,
                           theMaxParam,
                           isAutoTriangulation);
        }
      }
      break;
    }
    case TopAbs_COMPOUND:
    default: {
      TopExp_Explorer anExp;
      // sub-vertices
      for (anExp.Init(theShape, TopAbs_VERTEX, TopAbs_EDGE); anExp.More(); anExp.Next())
      {
        ComputeSensitive(anExp.Current(),
                         theOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         isAutoTriangulation);
      }
      // sub-edges
      for (anExp.Init(theShape, TopAbs_EDGE, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        ComputeSensitive(anExp.Current(),
                         theOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         isAutoTriangulation);
      }
      // sub-wires
      for (anExp.Init(theShape, TopAbs_WIRE, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        ComputeSensitive(anExp.Current(),
                         theOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         isAutoTriangulation);
      }

      // sub-faces
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubfacesMap;
      TopExp::MapShapes(theShape, TopAbs_FACE, aSubfacesMap);
      for (int aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
      {
        ComputeSensitive(aSubfacesMap(aShIndex),
                         theOwner,
                         theSelection,
                         theDeflection,
                         theDeviationAngle,
                         theNbPOnEdge,
                         theMaxParam,
                         isAutoTriangulation);
      }
    }
  }
}

//=================================================================================================

static occ::handle<NCollection_HArray1<gp_Pnt>> GetPointsFromPolygon(const TopoDS_Edge& theEdge)
{
  occ::handle<NCollection_HArray1<gp_Pnt>> aResultPoints;

  TopLoc_Location             aLocation;
  occ::handle<Poly_Polygon3D> aPolygon = BRep_Tool::Polygon3D(theEdge, aLocation);
  if (!aPolygon.IsNull())
  {
    const NCollection_Array1<gp_Pnt>& aNodes = aPolygon->Nodes();
    aResultPoints                            = new NCollection_HArray1<gp_Pnt>(1, aNodes.Length());
    if (aLocation.IsIdentity())
    {
      for (int aNodeId(aNodes.Lower()), aPntId(1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aNodes.Value(aNodeId));
      }
    }
    else
    {
      for (int aNodeId(aNodes.Lower()), aPntId(1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aNodes.Value(aNodeId).Transformed(aLocation));
      }
    }
    return aResultPoints;
  }

  occ::handle<Poly_Triangulation>          aTriangulation;
  occ::handle<Poly_PolygonOnTriangulation> anHIndices;
  BRep_Tool::PolygonOnTriangulation(theEdge, anHIndices, aTriangulation, aLocation);
  if (!anHIndices.IsNull())
  {
    const NCollection_Array1<int>& anIndices = anHIndices->Nodes();

    aResultPoints = new NCollection_HArray1<gp_Pnt>(1, anIndices.Length());

    if (aLocation.IsIdentity())
    {
      for (int anIndex(anIndices.Lower()), aPntId(1); anIndex <= anIndices.Upper();
           ++anIndex, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aTriangulation->Node(anIndices[anIndex]));
      }
    }
    else
    {
      for (int anIndex(anIndices.Lower()), aPntId(1); anIndex <= anIndices.Upper();
           ++anIndex, ++aPntId)
      {
        aResultPoints->SetValue(aPntId,
                                aTriangulation->Node(anIndices[anIndex]).Transformed(aLocation));
      }
    }
    return aResultPoints;
  }
  return aResultPoints;
}

//=================================================================================================

static bool FindLimits(const Adaptor3d_Curve& theCurve,
                       const double           theLimit,
                       double&                theFirst,
                       double&                theLast)
{
  theFirst        = theCurve.FirstParameter();
  theLast         = theCurve.LastParameter();
  bool isFirstInf = Precision::IsNegativeInfinite(theFirst);
  bool isLastInf  = Precision::IsPositiveInfinite(theLast);
  if (isFirstInf || isLastInf)
  {
    gp_Pnt aPnt1, aPnt2;
    double aDelta      = 1.0;
    int    anIterCount = 0;
    if (isFirstInf && isLastInf)
    {
      do
      {
        if (anIterCount++ >= 100000)
          return false;
        aDelta *= 2.0;
        theFirst = -aDelta;
        theLast  = aDelta;
        theCurve.D0(theFirst, aPnt1);
        theCurve.D0(theLast, aPnt2);
      } while (aPnt1.Distance(aPnt2) < theLimit);
    }
    else if (isFirstInf)
    {
      theCurve.D0(theLast, aPnt2);
      do
      {
        if (anIterCount++ >= 100000)
          return false;
        aDelta *= 2.0;
        theFirst = theLast - aDelta;
        theCurve.D0(theFirst, aPnt1);
      } while (aPnt1.Distance(aPnt2) < theLimit);
    }
    else if (isLastInf)
    {
      theCurve.D0(theFirst, aPnt1);
      do
      {
        if (anIterCount++ >= 100000)
          return false;
        aDelta *= 2.0;
        theLast = theFirst + aDelta;
        theCurve.D0(theLast, aPnt2);
      } while (aPnt1.Distance(aPnt2) < theLimit);
    }
  }
  return true;
}

//=================================================================================================

void StdSelect_BRepSelectionTool::GetEdgeSensitive(
  const TopoDS_Shape&                       theShape,
  const occ::handle<SelectMgr_EntityOwner>& theOwner,
  const occ::handle<SelectMgr_Selection>&   theSelection,
  const double                              theDeflection,
  const double                              theDeviationAngle,
  const int                                 theNbPOnEdge,
  const double                              theMaxParam,
  occ::handle<Select3D_SensitiveEntity>&    theSensitive)
{
  const TopoDS_Edge& anEdge = TopoDS::Edge(theShape);
  // try to get points from existing polygons
  occ::handle<NCollection_HArray1<gp_Pnt>> aPoints = GetPointsFromPolygon(anEdge);
  if (!aPoints.IsNull() && !aPoints->IsEmpty())
  {
    if (aPoints->Length() == 2)
    {
      // don't waste memory, create a segment
      theSensitive = new Select3D_SensitiveSegment(theOwner, aPoints->First(), aPoints->Last());
    }
    else
    {
      theSensitive = new Select3D_SensitiveCurve(theOwner, aPoints);
    }
    return;
  }

  BRepAdaptor_Curve cu3d;
  try
  {
    OCC_CATCH_SIGNALS
    cu3d.Initialize(anEdge);
  }
  catch (Standard_NullObject const&)
  {
    return;
  }

  double aParamFirst = cu3d.FirstParameter();
  double aParamLast  = cu3d.LastParameter();
  switch (cu3d.GetType())
  {
    case GeomAbs_Line: {
      BRep_Tool::Range(anEdge, aParamFirst, aParamLast);
      theSensitive =
        new Select3D_SensitiveSegment(theOwner, cu3d.Value(aParamFirst), cu3d.Value(aParamLast));
      break;
    }
    case GeomAbs_Circle: {
      const gp_Circ aCircle = cu3d.Circle();
      if (aCircle.Radius() <= Precision::Confusion())
      {
        theSelection->Add(new Select3D_SensitivePoint(theOwner, aCircle.Location()));
      }
      else
      {
        theSensitive =
          new Select3D_SensitivePoly(theOwner, aCircle, aParamFirst, aParamLast, false);
      }
      break;
    }
    default: {
      // reproduce drawing behaviour
      // TODO: remove copy-paste from StdPrs_Curve and some others...
      if (FindLimits(cu3d, theMaxParam, aParamFirst, aParamLast))
      {
        int                        aNbIntervals = cu3d.NbIntervals(GeomAbs_C1);
        NCollection_Array1<double> anIntervals(1, aNbIntervals + 1);
        cu3d.Intervals(anIntervals, GeomAbs_C1);
        double                       aV1, aV2;
        int                          aNumberOfPoints;
        NCollection_Sequence<gp_Pnt> aPointsSeq;
        for (int anIntervalId = 1; anIntervalId <= aNbIntervals; ++anIntervalId)
        {
          aV1 = anIntervals(anIntervalId);
          aV2 = anIntervals(anIntervalId + 1);
          if (aV2 > aParamFirst && aV1 < aParamLast)
          {
            aV1 = std::max(aV1, aParamFirst);
            aV2 = std::min(aV2, aParamLast);

            GCPnts_TangentialDeflection anAlgo(cu3d, aV1, aV2, theDeviationAngle, theDeflection);
            aNumberOfPoints = anAlgo.NbPoints();

            for (int aPntId = 1; aPntId < aNumberOfPoints; ++aPntId)
            {
              aPointsSeq.Append(anAlgo.Value(aPntId));
            }
            if (aNumberOfPoints > 0 && anIntervalId == aNbIntervals)
            {
              aPointsSeq.Append(anAlgo.Value(aNumberOfPoints));
            }
          }
        }

        aPoints = new NCollection_HArray1<gp_Pnt>(1, aPointsSeq.Length());
        for (int aPntId = 1; aPntId <= aPointsSeq.Length(); ++aPntId)
        {
          aPoints->SetValue(aPntId, aPointsSeq.Value(aPntId));
        }
        theSensitive = new Select3D_SensitiveCurve(theOwner, aPoints);
        break;
      }

      // simple subdivisions
      int nbintervals = 1;
      if (cu3d.GetType() == GeomAbs_BSplineCurve)
      {
        nbintervals = cu3d.NbKnots() - 1;
        nbintervals = std::max(1, nbintervals / 3);
      }

      double aParam;
      int    aPntNb      = std::max(2, theNbPOnEdge * nbintervals);
      double aParamDelta = (aParamLast - aParamFirst) / (aPntNb - 1);
      occ::handle<NCollection_HArray1<gp_Pnt>> aPointArray =
        new NCollection_HArray1<gp_Pnt>(1, aPntNb);
      for (int aPntId = 1; aPntId <= aPntNb; ++aPntId)
      {
        aParam = aParamFirst + aParamDelta * (aPntId - 1);
        aPointArray->SetValue(aPntId, cu3d.Value(aParam));
      }
      theSensitive = new Select3D_SensitiveCurve(theOwner, aPointArray);
    }
    break;
  }
}

//=================================================================================================

bool StdSelect_BRepSelectionTool::GetSensitiveForFace(
  const TopoDS_Face&                                           theFace,
  const occ::handle<SelectMgr_EntityOwner>&                    theOwner,
  NCollection_Sequence<occ::handle<Select3D_SensitiveEntity>>& theSensitiveList,
  const bool /*theAutoTriangulation*/,
  const int    NbPOnEdge,
  const double theMaxParam,
  const bool   theInteriorFlag)
{
  TopLoc_Location                  aLocSurf;
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(theFace, aLocSurf);
  if (occ::handle<Geom_SphericalSurface> aGeomSphere = occ::down_cast<Geom_SphericalSurface>(aSurf))
  {
    bool isFullSphere = theFace.NbChildren() == 0;
    if (theFace.NbChildren() == 1)
    {
      const TopoDS_Iterator aWireIter(theFace);
      const TopoDS_Wire&    aWire = TopoDS::Wire(aWireIter.Value());
      if (aWire.NbChildren() == 4)
      {
        int aNbSeamEdges = 0, aNbDegenEdges = 0;
        for (TopoDS_Iterator anEdgeIter(aWire); anEdgeIter.More(); anEdgeIter.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIter.Value());
          aNbSeamEdges += BRep_Tool::IsClosed(anEdge, theFace);
          aNbDegenEdges += BRep_Tool::Degenerated(anEdge);
        }
        isFullSphere = aNbSeamEdges == 2 && aNbDegenEdges == 2;
      }
    }
    if (isFullSphere)
    {
      gp_Sphere                             aSphere = BRepAdaptor_Surface(theFace).Sphere();
      occ::handle<Select3D_SensitiveSphere> aSensSphere =
        new Select3D_SensitiveSphere(theOwner,
                                     aSphere.Position().Axis().Location(),
                                     aSphere.Radius());
      theSensitiveList.Append(aSensSphere);
      return true;
    }
  }
  else if (occ::handle<Geom_ConicalSurface> aGeomCone = occ::down_cast<Geom_ConicalSurface>(aSurf))
  {
    if (isCylinderOrCone(theFace))
    {
      double aURange[2] = {}, aVRange[2] = {};
      BRepTools::UVBounds(theFace, aURange[0], aURange[1], aVRange[0], aVRange[1]);

      const gp_Circ aCirc1  = ElSLib::ConeVIso(aGeomCone->Position(),
                                              aGeomCone->RefRadius(),
                                              aGeomCone->SemiAngle(),
                                              aVRange[0]);
      const gp_Circ aCirc2  = ElSLib::ConeVIso(aGeomCone->Position(),
                                              aGeomCone->RefRadius(),
                                              aGeomCone->SemiAngle(),
                                              aVRange[1]);
      const double  aHeight = aCirc1.Location().Distance(aCirc2.Location());

      gp_Ax3 aPos = aGeomCone->Position();
      aPos.SetLocation(aCirc1.Location());
      gp_Trsf aLocTrsf;
      aLocTrsf.SetTransformation(aPos, gp::XOY());

      gp_Trsf      aTrsf  = aLocSurf * aLocTrsf;
      const double aScale = aTrsf.ScaleFactor();
      aTrsf.SetScaleFactor(1.0);

      occ::handle<Select3D_SensitiveCylinder> aSensSCyl =
        new Select3D_SensitiveCylinder(theOwner,
                                       aCirc1.Radius() * aScale,
                                       aCirc2.Radius() * aScale,
                                       aHeight * aScale,
                                       aTrsf,
                                       true);
      theSensitiveList.Append(aSensSCyl);
      return true;
    }
  }
  else if (occ::handle<Geom_CylindricalSurface> aGeomCyl =
             occ::down_cast<Geom_CylindricalSurface>(aSurf))
  {
    if (isCylinderOrCone(theFace))
    {
      double aURange[2] = {}, aVRange[2] = {};
      BRepTools::UVBounds(theFace, aURange[0], aURange[1], aVRange[0], aVRange[1]);

      const double aRad    = aGeomCyl->Radius();
      const double aHeight = aVRange[1] - aVRange[0];

      gp_Ax3 aPos = aGeomCyl->Position();
      aPos.SetLocation(aGeomCyl->Location().XYZ() + aPos.Direction().XYZ() * aVRange[0]);
      gp_Trsf aLocTrsf;
      aLocTrsf.SetTransformation(aPos, gp::XOY());

      gp_Trsf      aTrsf  = aLocSurf * aLocTrsf;
      const double aScale = aTrsf.ScaleFactor();
      aTrsf.SetScaleFactor(1.0);

      occ::handle<Select3D_SensitiveCylinder> aSensSCyl =
        new Select3D_SensitiveCylinder(theOwner,
                                       aRad * aScale,
                                       aRad * aScale,
                                       aHeight * aScale,
                                       aTrsf,
                                       true);
      theSensitiveList.Append(aSensSCyl);
      return true;
    }
  }
  else if (occ::handle<Geom_Plane> aGeomPlane = occ::down_cast<Geom_Plane>(aSurf))
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aSubfacesMap;
    TopExp::MapShapes(theFace, TopAbs_EDGE, aSubfacesMap);
    if (aSubfacesMap.Extent() == 1)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(aSubfacesMap.FindKey(1));
      BRepAdaptor_Curve  anAdaptor(anEdge);
      if (anAdaptor.GetType() == GeomAbs_Circle && BRep_Tool::IsClosed(anEdge))
      {
        occ::handle<Select3D_SensitiveCircle> aSensSCyl =
          new Select3D_SensitiveCircle(theOwner, anAdaptor.Circle(), theInteriorFlag);
        theSensitiveList.Append(aSensSCyl);
        return true;
      }
    }
  }

  TopLoc_Location aLoc;
  if (occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(theFace, aLoc))
  {
    occ::handle<Select3D_SensitiveTriangulation> STG =
      new Select3D_SensitiveTriangulation(theOwner, aTriangulation, aLoc, theInteriorFlag);
    theSensitiveList.Append(STG);
    return true;
  }

  // for faces with triangulation bugs or without autotriangulation ....
  // very ugly and should not even exist ...
  BRepAdaptor_Surface BS(theFace);
  if (BS.GetType() == GeomAbs_Plane)
  {
    const double aFirstU =
      BS.FirstUParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstUParameter();
    const double aLastU =
      BS.LastUParameter() >= Precision::Infinite() ? theMaxParam : BS.LastUParameter();
    const double aFirstV =
      BS.FirstVParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstVParameter();
    const double aLastV =
      BS.LastVParameter() >= Precision::Infinite() ? theMaxParam : BS.LastVParameter();
    occ::handle<NCollection_HArray1<gp_Pnt>> aPlanePnts = new NCollection_HArray1<gp_Pnt>(1, 5);
    BS.D0(aFirstU, aFirstV, aPlanePnts->ChangeValue(1));
    BS.D0(aLastU, aFirstV, aPlanePnts->ChangeValue(2));
    BS.D0(aLastU, aLastV, aPlanePnts->ChangeValue(3));
    BS.D0(aFirstU, aLastV, aPlanePnts->ChangeValue(4));
    aPlanePnts->SetValue(5, aPlanePnts->Value(1));

    // if the plane is "infinite", it is sensitive only on the border limited by MaxParam
    const bool isInfinite = aFirstU == -theMaxParam && aLastU == theMaxParam
                            && aFirstV == -theMaxParam && aLastV == theMaxParam;
    theSensitiveList.Append(new Select3D_SensitiveFace(
      theOwner,
      aPlanePnts,
      theInteriorFlag && !isInfinite ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY));
    return true;
  }

  // This is construction of a sensitive polygon from the exterior contour of the face...
  // It is not good at all, but...
  TopoDS_Wire aWire;
  {
    TopExp_Explorer anExpWiresInFace(theFace, TopAbs_WIRE);
    if (anExpWiresInFace.More())
    {
      // believing that this is the first... to be seen
      aWire = TopoDS::Wire(anExpWiresInFace.Current());
    }
  }
  if (aWire.IsNull())
  {
    return false;
  }

  NCollection_Sequence<gp_Pnt> aWirePoints;
  bool                         isFirstExp = true;
  BRepAdaptor_Curve            cu3d;
  for (BRepTools_WireExplorer aWireExplorer(aWire); aWireExplorer.More(); aWireExplorer.Next())
  {
    try
    {
      OCC_CATCH_SIGNALS
      cu3d.Initialize(aWireExplorer.Current());
    }
    catch (Standard_NullObject const&)
    {
      continue;
    }

    double wf = 0.0, wl = 0.0;
    BRep_Tool::Range(aWireExplorer.Current(), wf, wl);
    if (std::abs(wf - wl) <= Precision::Confusion())
    {
#ifdef OCCT_DEBUG
      std::cout << " StdSelect_BRepSelectionTool : Curve where ufirst = ulast ...." << std::endl;
#endif
      continue;
    }

    if (isFirstExp)
    {
      isFirstExp = false;
      if (aWireExplorer.Orientation() == TopAbs_FORWARD)
      {
        aWirePoints.Append(cu3d.Value(wf));
      }
      else
      {
        aWirePoints.Append(cu3d.Value(wl));
      }
    }

    switch (cu3d.GetType())
    {
      case GeomAbs_Line: {
        aWirePoints.Append(cu3d.Value((aWireExplorer.Orientation() == TopAbs_FORWARD) ? wl : wf));
        break;
      }
      case GeomAbs_Circle: {
        if (2.0 * M_PI - std::abs(wl - wf) <= Precision::Confusion())
        {
          if (BS.GetType() == GeomAbs_Cylinder || BS.GetType() == GeomAbs_Torus
              || BS.GetType() == GeomAbs_Cone
              || BS.GetType() == GeomAbs_BSplineSurface) // beuurkk pour l'instant...
          {
            double ff = wf, ll = wl;
            double dw = (std::max(wf, wl) - std::min(wf, wl))
                        / static_cast<double>(std::max(2, NbPOnEdge - 1));
            if (aWireExplorer.Orientation() == TopAbs_FORWARD)
            {
              for (double wc = wf + dw; wc <= wl; wc += dw)
              {
                aWirePoints.Append(cu3d.Value(wc));
              }
            }
            else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
            {
              for (double wc = ll - dw; wc >= ff; wc -= dw)
              {
                aWirePoints.Append(cu3d.Value(wc));
              }
            }
          }
          else
          {
            if (cu3d.Circle().Radius() <= Precision::Confusion())
            {
              theSensitiveList.Append(
                new Select3D_SensitivePoint(theOwner, cu3d.Circle().Location()));
            }
            else
            {
              theSensitiveList.Append(
                new Select3D_SensitiveCircle(theOwner, cu3d.Circle(), theInteriorFlag));
            }
          }
        }
        else
        {
          double ff = wf, ll = wl;
          double dw =
            (std::max(wf, wl) - std::min(wf, wl)) / static_cast<double>(std::max(2, NbPOnEdge - 1));
          if (aWireExplorer.Orientation() == TopAbs_FORWARD)
          {
            for (double wc = wf + dw; wc <= wl; wc += dw)
            {
              aWirePoints.Append(cu3d.Value(wc));
            }
          }
          else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
          {
            for (double wc = ll - dw; wc >= ff; wc -= dw)
            {
              aWirePoints.Append(cu3d.Value(wc));
            }
          }
        }
        break;
      }
      default: {
        double ff = wf, ll = wl;
        double dw =
          (std::max(wf, wl) - std::min(wf, wl)) / static_cast<double>(std::max(2, NbPOnEdge - 1));
        if (aWireExplorer.Orientation() == TopAbs_FORWARD)
        {
          for (double wc = wf + dw; wc <= wl; wc += dw)
          {
            aWirePoints.Append(cu3d.Value(wc));
          }
        }
        else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
        {
          for (double wc = ll - dw; wc >= ff; wc -= dw)
          {
            aWirePoints.Append(cu3d.Value(wc));
          }
        }
      }
    }
  }

  occ::handle<NCollection_HArray1<gp_Pnt>> aFacePoints =
    new NCollection_HArray1<gp_Pnt>(1, aWirePoints.Length());
  {
    int aPntIndex = 1;
    for (NCollection_Sequence<gp_Pnt>::Iterator aPntIter(aWirePoints); aPntIter.More();
         aPntIter.Next())
    {
      aFacePoints->SetValue(aPntIndex++, aPntIter.Value());
    }
  }

  // 1 if only one circular edge
  if (aFacePoints->Array1().Length() == 2)
  {
    theSensitiveList.Append(new Select3D_SensitiveCurve(theOwner, aFacePoints));
  }
  else if (aFacePoints->Array1().Length() > 2)
  {
    theSensitiveList.Append(
      new Select3D_SensitiveFace(theOwner,
                                 aFacePoints,
                                 theInteriorFlag ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY));
  }
  return true;
}

//=================================================================================================

bool StdSelect_BRepSelectionTool::GetSensitiveForCylinder(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theSubfacesMap,
  const occ::handle<SelectMgr_EntityOwner>&                            theOwner,
  const occ::handle<SelectMgr_Selection>&                              theSelection)
{
  if (theSubfacesMap.Extent() == 2) // detect cone
  {
    const TopoDS_Face* aFaces[2] = {&TopoDS::Face(theSubfacesMap.FindKey(1)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(2))};

    TopLoc_Location                  aLocSurf[2];
    const occ::handle<Geom_Surface>* aSurfaces[2] = {&BRep_Tool::Surface(*aFaces[0], aLocSurf[0]),
                                                     &BRep_Tool::Surface(*aFaces[1], aLocSurf[1])};

    int                              aConIndex = 0;
    occ::handle<Geom_ConicalSurface> aGeomCone = occ::down_cast<Geom_ConicalSurface>(*aSurfaces[0]);
    occ::handle<Geom_Plane>          aGeomPln;
    if (!aGeomCone.IsNull())
    {
      aGeomPln = occ::down_cast<Geom_Plane>(*aSurfaces[1]);
    }
    else
    {
      aConIndex = 1;
      aGeomCone = occ::down_cast<Geom_ConicalSurface>(*aSurfaces[1]);
      aGeomPln  = occ::down_cast<Geom_Plane>(*aSurfaces[0]);
    }
    if (!aGeomCone.IsNull() && !aGeomPln.IsNull()
        && aGeomPln->Position().Direction().IsEqual(aGeomCone->Position().Direction(),
                                                    Precision::Angular()))
    {
      const gp_Cone aCone = BRepAdaptor_Surface(*aFaces[aConIndex]).Cone();
      const double  aRad1 = aCone.RefRadius();
      const double  aHeight =
        (aRad1 != 0.0) ? aRad1 / std::abs(std::tan(aCone.SemiAngle()))
                        : aCone.Location().Distance(
                           aGeomPln->Location().Transformed(aLocSurf[aConIndex == 0 ? 1 : 0]));
      const double aRad2 = (aRad1 != 0.0) ? 0.0 : std::tan(aCone.SemiAngle()) * aHeight;
      gp_Trsf      aTrsf;
      aTrsf.SetTransformation(aCone.Position(), gp::XOY());
      occ::handle<Select3D_SensitiveCylinder> aSensSCyl =
        new Select3D_SensitiveCylinder(theOwner, aRad1, aRad2, aHeight, aTrsf);
      theSelection->Add(aSensSCyl);
      return true;
    }
  }
  if (theSubfacesMap.Extent() == 3) // detect cylinder or truncated cone
  {
    const TopoDS_Face* aFaces[3] = {&TopoDS::Face(theSubfacesMap.FindKey(1)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(2)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(3))};

    TopLoc_Location                  aLocSurf[3];
    const occ::handle<Geom_Surface>* aSurfaces[3] = {&BRep_Tool::Surface(*aFaces[0], aLocSurf[0]),
                                                     &BRep_Tool::Surface(*aFaces[1], aLocSurf[1]),
                                                     &BRep_Tool::Surface(*aFaces[2], aLocSurf[2])};

    int                                  aConIndex = -1, aNbPlanes = 0;
    occ::handle<Geom_ConicalSurface>     aGeomCone;
    occ::handle<Geom_CylindricalSurface> aGeomCyl;
    occ::handle<Geom_Plane>              aGeomPlanes[2];
    const TopLoc_Location*               aGeomPlanesLoc[2];
    for (int aSurfIter = 0; aSurfIter < 3; ++aSurfIter)
    {
      const occ::handle<Geom_Surface>& aSurf = *aSurfaces[aSurfIter];
      if (aConIndex == -1)
      {
        aGeomCone = occ::down_cast<Geom_ConicalSurface>(aSurf);
        if (!aGeomCone.IsNull())
        {
          aConIndex = aSurfIter;
          continue;
        }
        aGeomCyl = occ::down_cast<Geom_CylindricalSurface>(aSurf);
        if (!aGeomCyl.IsNull())
        {
          aConIndex = aSurfIter;
          continue;
        }
      }
      if (aNbPlanes < 2)
      {
        aGeomPlanes[aNbPlanes] = occ::down_cast<Geom_Plane>(aSurf);
        if (!aGeomPlanes[aNbPlanes].IsNull())
        {
          aGeomPlanesLoc[aNbPlanes] = &aLocSurf[aSurfIter];
          ++aNbPlanes;
        }
      }
    }

    if (!aGeomCone.IsNull())
    {
      if (!aGeomPlanes[0].IsNull() && !aGeomPlanes[1].IsNull()
          && aGeomPlanes[0]->Position().Direction().IsEqual(aGeomCone->Position().Direction(),
                                                            Precision::Angular())
          && aGeomPlanes[1]->Position().Direction().IsEqual(aGeomCone->Position().Direction(),
                                                            Precision::Angular()))
      {
        const gp_Cone aCone = BRepAdaptor_Surface(*aFaces[aConIndex]).Cone();
        const double  aRad1 = aCone.RefRadius();
        const double  aHeight =
          aGeomPlanes[0]
            ->Location()
            .Transformed(*aGeomPlanesLoc[0])
            .Distance(aGeomPlanes[1]->Location().Transformed(*aGeomPlanesLoc[1]));
        gp_Trsf aTrsf;
        aTrsf.SetTransformation(aCone.Position(), gp::XOY());
        const double aTriangleHeight = (aCone.SemiAngle() > 0.0)
                                         ? aRad1 / std::tan(aCone.SemiAngle())
                                         : aRad1 / std::tan(std::abs(aCone.SemiAngle())) - aHeight;
        const double aRad2           = (aCone.SemiAngle() > 0.0)
                                         ? aRad1 * (aTriangleHeight + aHeight) / aTriangleHeight
                                         : aRad1 * aTriangleHeight / (aTriangleHeight + aHeight);

        occ::handle<Select3D_SensitiveCylinder> aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad1, aRad2, aHeight, aTrsf);
        theSelection->Add(aSensSCyl);
        return true;
      }
    }
    else if (!aGeomCyl.IsNull())
    {
      if (!aGeomPlanes[0].IsNull() && !aGeomPlanes[1].IsNull()
          && aGeomPlanes[0]->Position().Direction().IsParallel(aGeomCyl->Position().Direction(),
                                                               Precision::Angular())
          && aGeomPlanes[1]->Position().Direction().IsParallel(aGeomCyl->Position().Direction(),
                                                               Precision::Angular()))
      {
        const gp_Cylinder aCyl = BRepAdaptor_Surface(*aFaces[aConIndex]).Cylinder();
        const double      aRad = aCyl.Radius();
        const double      aHeight =
          aGeomPlanes[0]
            ->Location()
            .Transformed(*aGeomPlanesLoc[0])
            .Distance(aGeomPlanes[1]->Location().Transformed(*aGeomPlanesLoc[1]));

        gp_Trsf aTrsf;
        gp_Ax3  aPos = aCyl.Position();
        if (aGeomPlanes[0]->Position().IsCoplanar(aGeomPlanes[1]->Position(),
                                                  Precision::Angular(),
                                                  Precision::Angular()))
        {
          // cylinders created as a prism have an inverse vector of the cylindrical surface
          aPos.SetDirection(aPos.Direction().Reversed());
        }
        aTrsf.SetTransformation(aPos, gp::XOY());

        occ::handle<Select3D_SensitiveCylinder> aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad, aRad, aHeight, aTrsf);
        theSelection->Add(aSensSCyl);
        return true;
      }
    }
  }

  return false;
}
