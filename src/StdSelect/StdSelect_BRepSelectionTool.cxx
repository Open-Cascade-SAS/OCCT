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
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#include <array>

#define BVH_PRIMITIVE_LIMIT 800000

namespace
{
//=======================================================================
// function : getCylinderCircles
// purpose  : Extracts up to two circular edges from a hollow cylinder face
//=======================================================================
std::array<gp_Circ, 2> getCylinderCircles(const TopoDS_Face& theHollowCylinder,
                                          Standard_Size&     theNumCircles)
{
  std::array<gp_Circ, 2> aCircles; // Array to store up to two circles
  theNumCircles             = 0;   // Initialize the number of circles found
  Standard_Integer aLinesNb = 0;   // Counter for the number of edges processed

  for (TopExp_Explorer anEdgeExp(theHollowCylinder, TopAbs_EDGE); anEdgeExp.More();
       anEdgeExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
    BRepAdaptor_Curve  anAdaptor(anEdge);
    aLinesNb++;

    if (anAdaptor.GetType() == GeomAbs_Circle && BRep_Tool::IsClosed(anEdge))
    {
      theNumCircles++;
      aCircles[theNumCircles - 1] = anAdaptor.Circle();
    }
    else if (anAdaptor.GetType() != GeomAbs_Line || aLinesNb > 4)
    {
      theNumCircles = 0;
      return std::array<gp_Circ, 2>();
    }
    if (theNumCircles == 2)
    {
      break;
    }
  }

  return aCircles;
}
} // namespace

//==================================================
// function: PreBuildBVH
// purpose : Pre-builds BVH tree for heavyweight
//           sensitive entities with sub-elements
//           amount more than BVH_PRIMITIVE_LIMIT
//==================================================
void StdSelect_BRepSelectionTool::PreBuildBVH(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const Handle(Select3D_SensitiveEntity)& aSensitive = aSelEntIter.Value()->BaseSensitive();
    if (aSensitive->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
    {
      aSensitive->BVH();
    }

    if (!aSensitive->IsInstance(STANDARD_TYPE(Select3D_SensitiveGroup)))
    {
      continue;
    }

    Handle(Select3D_SensitiveGroup) aGroup = Handle(Select3D_SensitiveGroup)::DownCast(aSensitive);
    for (Select3D_IndexedMapOfEntity::Iterator aSubEntitiesIter(aGroup->Entities());
         aSubEntitiesIter.More();
         aSubEntitiesIter.Next())
    {
      const Handle(Select3D_SensitiveEntity)& aSubEntity = aSubEntitiesIter.Value();
      if (aSubEntity->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
      {
        aSubEntity->BVH();
      }
    }
  }
}

//=================================================================================================

void StdSelect_BRepSelectionTool::Load(const Handle(SelectMgr_Selection)& theSelection,
                                       const TopoDS_Shape&                theShape,
                                       const TopAbs_ShapeEnum             theType,
                                       const Standard_Real                theDeflection,
                                       const Standard_Real                theDeviationAngle,
                                       const Standard_Boolean             isAutoTriangulation,
                                       const Standard_Integer             thePriority,
                                       const Standard_Integer             theNbPOnEdge,
                                       const Standard_Real                theMaxParam)
{
  Standard_Integer aPriority =
    (thePriority == -1) ? GetStandardPriority(theShape, theType) : thePriority;
  if (isAutoTriangulation && !BRepTools::Triangulation(theShape, Precision::Infinite(), true))
  {
    BRepMesh_IncrementalMesh aMesher(theShape, theDeflection, Standard_False, theDeviationAngle);
  }

  Handle(StdSelect_BRepOwner) aBrepOwner;
  switch (theType)
  {
    case TopAbs_VERTEX:
    case TopAbs_EDGE:
    case TopAbs_WIRE:
    case TopAbs_FACE:
    case TopAbs_SHELL:
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID: {
      TopTools_IndexedMapOfShape aSubShapes;
      TopExp::MapShapes(theShape, theType, aSubShapes);

      Standard_Boolean isComesFromDecomposition =
        !((aSubShapes.Extent() == 1) && (theShape == aSubShapes(1)));
      for (Standard_Integer aShIndex = 1; aShIndex <= aSubShapes.Extent(); ++aShIndex)
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

void StdSelect_BRepSelectionTool::Load(const Handle(SelectMgr_Selection)&        theSelection,
                                       const Handle(SelectMgr_SelectableObject)& theSelectableObj,
                                       const TopoDS_Shape&                       theShape,
                                       const TopAbs_ShapeEnum                    theType,
                                       const Standard_Real                       theDeflection,
                                       const Standard_Real                       theDeviationAngle,
                                       const Standard_Boolean isAutoTriangulation,
                                       const Standard_Integer thePriority,
                                       const Standard_Integer theNbPOnEdge,
                                       const Standard_Real    theMaxParam)
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
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = aSelEntIter.Value()->BaseSensitive()->OwnerId();
    anOwner->SetSelectable(theSelectableObj);
  }
}

//=================================================================================================

void StdSelect_BRepSelectionTool::ComputeSensitive(const TopoDS_Shape&                  theShape,
                                                   const Handle(SelectMgr_EntityOwner)& theOwner,
                                                   const Handle(SelectMgr_Selection)& theSelection,
                                                   const Standard_Real                theDeflection,
                                                   const Standard_Real    theDeviationAngle,
                                                   const Standard_Integer theNbPOnEdge,
                                                   const Standard_Real    theMaxParam,
                                                   const Standard_Boolean isAutoTriangulation)
{
  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX: {
      theSelection->Add(
        new Select3D_SensitivePoint(theOwner, BRep_Tool::Pnt(TopoDS::Vertex(theShape))));
      break;
    }
    case TopAbs_EDGE: {
      Handle(Select3D_SensitiveEntity) aSensitive;
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
      BRepTools_WireExplorer           aWireExp(TopoDS::Wire(theShape));
      Handle(Select3D_SensitiveEntity) aSensitive;
      Handle(Select3D_SensitiveWire)   aWireSensitive = new Select3D_SensitiveWire(theOwner);
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

      Select3D_EntitySequence aSensitiveList;
      GetSensitiveForFace(aFace,
                          theOwner,
                          aSensitiveList,
                          isAutoTriangulation,
                          theNbPOnEdge,
                          theMaxParam);
      for (Select3D_EntitySequenceIter aSensIter(aSensitiveList); aSensIter.More();
           aSensIter.Next())
      {
        theSelection->Add(aSensIter.Value());
      }
      break;
    }
    case TopAbs_SHELL:
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID: {
      TopTools_IndexedMapOfShape aSubfacesMap;
      TopExp::MapShapes(theShape, TopAbs_FACE, aSubfacesMap);

      if (!GetSensitiveForCylinder(aSubfacesMap, theOwner, theSelection))
      {
        for (Standard_Integer aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
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
      TopTools_IndexedMapOfShape aSubfacesMap;
      TopExp::MapShapes(theShape, TopAbs_FACE, aSubfacesMap);
      for (Standard_Integer aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
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

static Handle(TColgp_HArray1OfPnt) GetPointsFromPolygon(const TopoDS_Edge& theEdge)
{
  Handle(TColgp_HArray1OfPnt) aResultPoints;

  TopLoc_Location        aLocation;
  Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D(theEdge, aLocation);
  if (!aPolygon.IsNull())
  {
    const TColgp_Array1OfPnt& aNodes = aPolygon->Nodes();
    aResultPoints                    = new TColgp_HArray1OfPnt(1, aNodes.Length());
    if (aLocation.IsIdentity())
    {
      for (Standard_Integer aNodeId(aNodes.Lower()), aPntId(1); aNodeId <= aNodes.Upper();
           ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aNodes.Value(aNodeId));
      }
    }
    else
    {
      for (Standard_Integer aNodeId(aNodes.Lower()), aPntId(1); aNodeId <= aNodes.Upper();
           ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aNodes.Value(aNodeId).Transformed(aLocation));
      }
    }
    return aResultPoints;
  }

  Handle(Poly_Triangulation)          aTriangulation;
  Handle(Poly_PolygonOnTriangulation) anHIndices;
  BRep_Tool::PolygonOnTriangulation(theEdge, anHIndices, aTriangulation, aLocation);
  if (!anHIndices.IsNull())
  {
    const TColStd_Array1OfInteger& anIndices = anHIndices->Nodes();

    aResultPoints = new TColgp_HArray1OfPnt(1, anIndices.Length());

    if (aLocation.IsIdentity())
    {
      for (Standard_Integer anIndex(anIndices.Lower()), aPntId(1); anIndex <= anIndices.Upper();
           ++anIndex, ++aPntId)
      {
        aResultPoints->SetValue(aPntId, aTriangulation->Node(anIndices[anIndex]));
      }
    }
    else
    {
      for (Standard_Integer anIndex(anIndices.Lower()), aPntId(1); anIndex <= anIndices.Upper();
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

static Standard_Boolean FindLimits(const Adaptor3d_Curve& theCurve,
                                   const Standard_Real    theLimit,
                                   Standard_Real&         theFirst,
                                   Standard_Real&         theLast)
{
  theFirst                    = theCurve.FirstParameter();
  theLast                     = theCurve.LastParameter();
  Standard_Boolean isFirstInf = Precision::IsNegativeInfinite(theFirst);
  Standard_Boolean isLastInf  = Precision::IsPositiveInfinite(theLast);
  if (isFirstInf || isLastInf)
  {
    gp_Pnt           aPnt1, aPnt2;
    Standard_Real    aDelta      = 1.0;
    Standard_Integer anIterCount = 0;
    if (isFirstInf && isLastInf)
    {
      do
      {
        if (anIterCount++ >= 100000)
          return Standard_False;
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
          return Standard_False;
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
          return Standard_False;
        aDelta *= 2.0;
        theLast = theFirst + aDelta;
        theCurve.D0(theLast, aPnt2);
      } while (aPnt1.Distance(aPnt2) < theLimit);
    }
  }
  return Standard_True;
}

//=================================================================================================

void StdSelect_BRepSelectionTool::GetEdgeSensitive(const TopoDS_Shape&                  theShape,
                                                   const Handle(SelectMgr_EntityOwner)& theOwner,
                                                   const Handle(SelectMgr_Selection)& theSelection,
                                                   const Standard_Real                theDeflection,
                                                   const Standard_Real    theDeviationAngle,
                                                   const Standard_Integer theNbPOnEdge,
                                                   const Standard_Real    theMaxParam,
                                                   Handle(Select3D_SensitiveEntity)& theSensitive)
{
  const TopoDS_Edge& anEdge = TopoDS::Edge(theShape);
  // try to get points from existing polygons
  Handle(TColgp_HArray1OfPnt) aPoints = GetPointsFromPolygon(anEdge);
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

  Standard_Real aParamFirst = cu3d.FirstParameter();
  Standard_Real aParamLast  = cu3d.LastParameter();
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
          new Select3D_SensitivePoly(theOwner, aCircle, aParamFirst, aParamLast, Standard_False);
      }
      break;
    }
    default: {
      // reproduce drawing behaviour
      // TODO: remove copy-paste from StdPrs_Curve and some others...
      if (FindLimits(cu3d, theMaxParam, aParamFirst, aParamLast))
      {
        Standard_Integer     aNbIntervals = cu3d.NbIntervals(GeomAbs_C1);
        TColStd_Array1OfReal anIntervals(1, aNbIntervals + 1);
        cu3d.Intervals(anIntervals, GeomAbs_C1);
        Standard_Real        aV1, aV2;
        Standard_Integer     aNumberOfPoints;
        TColgp_SequenceOfPnt aPointsSeq;
        for (Standard_Integer anIntervalId = 1; anIntervalId <= aNbIntervals; ++anIntervalId)
        {
          aV1 = anIntervals(anIntervalId);
          aV2 = anIntervals(anIntervalId + 1);
          if (aV2 > aParamFirst && aV1 < aParamLast)
          {
            aV1 = Max(aV1, aParamFirst);
            aV2 = Min(aV2, aParamLast);

            GCPnts_TangentialDeflection anAlgo(cu3d, aV1, aV2, theDeviationAngle, theDeflection);
            aNumberOfPoints = anAlgo.NbPoints();

            for (Standard_Integer aPntId = 1; aPntId < aNumberOfPoints; ++aPntId)
            {
              aPointsSeq.Append(anAlgo.Value(aPntId));
            }
            if (aNumberOfPoints > 0 && anIntervalId == aNbIntervals)
            {
              aPointsSeq.Append(anAlgo.Value(aNumberOfPoints));
            }
          }
        }

        aPoints = new TColgp_HArray1OfPnt(1, aPointsSeq.Length());
        for (Standard_Integer aPntId = 1; aPntId <= aPointsSeq.Length(); ++aPntId)
        {
          aPoints->SetValue(aPntId, aPointsSeq.Value(aPntId));
        }
        theSensitive = new Select3D_SensitiveCurve(theOwner, aPoints);
        break;
      }

      // simple subdivisions
      Standard_Integer nbintervals = 1;
      if (cu3d.GetType() == GeomAbs_BSplineCurve)
      {
        nbintervals = cu3d.NbKnots() - 1;
        nbintervals = Max(1, nbintervals / 3);
      }

      Standard_Real               aParam;
      Standard_Integer            aPntNb      = Max(2, theNbPOnEdge * nbintervals);
      Standard_Real               aParamDelta = (aParamLast - aParamFirst) / (aPntNb - 1);
      Handle(TColgp_HArray1OfPnt) aPointArray = new TColgp_HArray1OfPnt(1, aPntNb);
      for (Standard_Integer aPntId = 1; aPntId <= aPntNb; ++aPntId)
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

Standard_Boolean StdSelect_BRepSelectionTool::GetSensitiveForFace(
  const TopoDS_Face&                   theFace,
  const Handle(SelectMgr_EntityOwner)& theOwner,
  Select3D_EntitySequence&             theSensitiveList,
  const Standard_Boolean /*theAutoTriangulation*/,
  const Standard_Integer NbPOnEdge,
  const Standard_Real    theMaxParam,
  const Standard_Boolean theInteriorFlag)
{
  TopLoc_Location aLoc;
  if (Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(theFace, aLoc))
  {
    TopLoc_Location             aLocSurf;
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(theFace, aLocSurf);
    if (Handle(Geom_SphericalSurface) aGeomSphere = Handle(Geom_SphericalSurface)::DownCast(aSurf))
    {
      bool isFullSphere = theFace.NbChildren() == 0;
      if (theFace.NbChildren() == 1)
      {
        const TopoDS_Iterator aWireIter(theFace);
        const TopoDS_Wire&    aWire = TopoDS::Wire(aWireIter.Value());
        if (aWire.NbChildren() == 4)
        {
          Standard_Integer aNbSeamEdges = 0, aNbDegenEdges = 0;
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
        gp_Sphere                        aSphere = BRepAdaptor_Surface(theFace).Sphere();
        Handle(Select3D_SensitiveSphere) aSensSphere =
          new Select3D_SensitiveSphere(theOwner,
                                       aSphere.Position().Axis().Location(),
                                       aSphere.Radius());
        theSensitiveList.Append(aSensSphere);
        return Standard_True;
      }
    }
    else if (Handle(Geom_ConicalSurface) aGeomCone = Handle(Geom_ConicalSurface)::DownCast(aSurf))
    {
      Standard_Size                aNumCircles;
      const std::array<gp_Circ, 2> aCircles = getCylinderCircles(theFace, aNumCircles);

      if (aNumCircles > 0 && aNumCircles < 3)
      {
        const gp_Cone aCone = BRepAdaptor_Surface(theFace).Cone();

        gp_Trsf       aTrsf;
        Standard_Real aRad1, aRad2, aHeight;
        if (aNumCircles == 1)
        {
          aRad1   = 0.0;
          aRad2   = aCircles[0].Radius();
          aHeight = aRad2 * Tan(aCone.SemiAngle());
          aTrsf.SetTransformation(aCone.Position(), gp::XOY());
        }
        else
        {
          aRad1   = aCircles[0].Radius();
          aRad2   = aCircles[1].Radius();
          aHeight = aCircles[0].Location().Distance(aCircles[1].Location());

          const gp_Pnt aPos = aCircles[0].Location();
          const gp_Dir aDirection(aCircles[1].Location().XYZ() - aPos.XYZ());

          aTrsf.SetTransformation(gp_Ax3(aPos, aDirection), gp::XOY());
        }

        Handle(Select3D_SensitiveCylinder) aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad1, aRad2, aHeight, aTrsf, true);
        theSensitiveList.Append(aSensSCyl);
        return Standard_True;
      }
    }
    else if (Handle(Geom_CylindricalSurface) aGeomCyl =
               Handle(Geom_CylindricalSurface)::DownCast(aSurf))
    {
      Standard_Size                aNumCircles;
      const std::array<gp_Circ, 2> aCircles = getCylinderCircles(theFace, aNumCircles);

      if (aNumCircles == 2)
      {
        const gp_Cylinder aCyl = BRepAdaptor_Surface(theFace).Cylinder();

        const Standard_Real aRad = aCyl.Radius();
        const gp_Pnt        aPos = aCircles[0].Location();
        const gp_Dir        aDirection(aCircles[1].Location().XYZ() - aPos.XYZ());
        const Standard_Real aHeight = aPos.Distance(aCircles[1].Location());

        gp_Trsf aTrsf;
        aTrsf.SetTransformation(gp_Ax3(aPos, aDirection), gp::XOY());

        Handle(Select3D_SensitiveCylinder) aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad, aRad, aHeight, aTrsf, true);
        theSensitiveList.Append(aSensSCyl);
        return Standard_True;
      }
    }
    else if (Handle(Geom_Plane) aGeomPlane = Handle(Geom_Plane)::DownCast(aSurf))
    {
      TopTools_IndexedMapOfShape aSubfacesMap;
      TopExp::MapShapes(theFace, TopAbs_EDGE, aSubfacesMap);
      if (aSubfacesMap.Extent() == 1)
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(aSubfacesMap.FindKey(1));
        BRepAdaptor_Curve  anAdaptor(anEdge);
        if (anAdaptor.GetType() == GeomAbs_Circle && BRep_Tool::IsClosed(anEdge))
        {
          Handle(Select3D_SensitiveCircle) aSensSCyl =
            new Select3D_SensitiveCircle(theOwner, anAdaptor.Circle(), theInteriorFlag);
          theSensitiveList.Append(aSensSCyl);
          return Standard_True;
        }
      }
    }

    Handle(Select3D_SensitiveTriangulation) STG =
      new Select3D_SensitiveTriangulation(theOwner, aTriangulation, aLoc, theInteriorFlag);
    theSensitiveList.Append(STG);
    return Standard_True;
  }

  // for faces with triangulation bugs or without autotriangulation ....
  // very ugly and should not even exist ...
  BRepAdaptor_Surface BS(theFace);
  if (BS.GetType() == GeomAbs_Plane)
  {
    const Standard_Real aFirstU =
      BS.FirstUParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstUParameter();
    const Standard_Real aLastU =
      BS.LastUParameter() >= Precision::Infinite() ? theMaxParam : BS.LastUParameter();
    const Standard_Real aFirstV =
      BS.FirstVParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstVParameter();
    const Standard_Real aLastV =
      BS.LastVParameter() >= Precision::Infinite() ? theMaxParam : BS.LastVParameter();
    Handle(TColgp_HArray1OfPnt) aPlanePnts = new TColgp_HArray1OfPnt(1, 5);
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
    return Standard_True;
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
    return Standard_False;
  }

  TColgp_SequenceOfPnt aWirePoints;
  Standard_Boolean     isFirstExp = Standard_True;
  BRepAdaptor_Curve    cu3d;
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

    Standard_Real wf = 0.0, wl = 0.0;
    BRep_Tool::Range(aWireExplorer.Current(), wf, wl);
    if (Abs(wf - wl) <= Precision::Confusion())
    {
#ifdef OCCT_DEBUG
      std::cout << " StdSelect_BRepSelectionTool : Curve where ufirst = ulast ...." << std::endl;
#endif
      continue;
    }

    if (isFirstExp)
    {
      isFirstExp = Standard_False;
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
        if (2.0 * M_PI - Abs(wl - wf) <= Precision::Confusion())
        {
          if (BS.GetType() == GeomAbs_Cylinder || BS.GetType() == GeomAbs_Torus
              || BS.GetType() == GeomAbs_Cone
              || BS.GetType() == GeomAbs_BSplineSurface) // beuurkk pour l'instant...
          {
            Standard_Real ff = wf, ll = wl;
            Standard_Real dw = (Max(wf, wl) - Min(wf, wl)) / (Standard_Real)Max(2, NbPOnEdge - 1);
            if (aWireExplorer.Orientation() == TopAbs_FORWARD)
            {
              for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
              {
                aWirePoints.Append(cu3d.Value(wc));
              }
            }
            else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
            {
              for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
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
          Standard_Real ff = wf, ll = wl;
          Standard_Real dw = (Max(wf, wl) - Min(wf, wl)) / (Standard_Real)Max(2, NbPOnEdge - 1);
          if (aWireExplorer.Orientation() == TopAbs_FORWARD)
          {
            for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
            {
              aWirePoints.Append(cu3d.Value(wc));
            }
          }
          else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
          {
            for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
            {
              aWirePoints.Append(cu3d.Value(wc));
            }
          }
        }
        break;
      }
      default: {
        Standard_Real ff = wf, ll = wl;
        Standard_Real dw = (Max(wf, wl) - Min(wf, wl)) / (Standard_Real)Max(2, NbPOnEdge - 1);
        if (aWireExplorer.Orientation() == TopAbs_FORWARD)
        {
          for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
          {
            aWirePoints.Append(cu3d.Value(wc));
          }
        }
        else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
        {
          for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
          {
            aWirePoints.Append(cu3d.Value(wc));
          }
        }
      }
    }
  }

  Handle(TColgp_HArray1OfPnt) aFacePoints = new TColgp_HArray1OfPnt(1, aWirePoints.Length());
  {
    Standard_Integer aPntIndex = 1;
    for (TColgp_SequenceOfPnt::Iterator aPntIter(aWirePoints); aPntIter.More(); aPntIter.Next())
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
  return Standard_True;
}

//=================================================================================================

Standard_Boolean StdSelect_BRepSelectionTool::GetSensitiveForCylinder(
  const TopTools_IndexedMapOfShape&    theSubfacesMap,
  const Handle(SelectMgr_EntityOwner)& theOwner,
  const Handle(SelectMgr_Selection)&   theSelection)
{
  if (theSubfacesMap.Extent() == 2) // detect cone
  {
    const TopoDS_Face* aFaces[2] = {&TopoDS::Face(theSubfacesMap.FindKey(1)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(2))};

    TopLoc_Location             aLocSurf[2];
    const Handle(Geom_Surface)* aSurfaces[2] = {&BRep_Tool::Surface(*aFaces[0], aLocSurf[0]),
                                                &BRep_Tool::Surface(*aFaces[1], aLocSurf[1])};

    Standard_Integer            aConIndex = 0;
    Handle(Geom_ConicalSurface) aGeomCone = Handle(Geom_ConicalSurface)::DownCast(*aSurfaces[0]);
    Handle(Geom_Plane)          aGeomPln;
    if (!aGeomCone.IsNull())
    {
      aGeomPln = Handle(Geom_Plane)::DownCast(*aSurfaces[1]);
    }
    else
    {
      aConIndex = 1;
      aGeomCone = Handle(Geom_ConicalSurface)::DownCast(*aSurfaces[1]);
      aGeomPln  = Handle(Geom_Plane)::DownCast(*aSurfaces[0]);
    }
    if (!aGeomCone.IsNull() && !aGeomPln.IsNull()
        && aGeomPln->Position().Direction().IsEqual(aGeomCone->Position().Direction(),
                                                    Precision::Angular()))
    {
      const gp_Cone       aCone = BRepAdaptor_Surface(*aFaces[aConIndex]).Cone();
      const Standard_Real aRad1 = aCone.RefRadius();
      const Standard_Real aHeight =
        (aRad1 != 0.0) ? aRad1 / Abs(Tan(aCone.SemiAngle()))
                       : aCone.Location().Distance(
                           aGeomPln->Location().Transformed(aLocSurf[aConIndex == 0 ? 1 : 0]));
      const Standard_Real aRad2 = (aRad1 != 0.0) ? 0.0 : Tan(aCone.SemiAngle()) * aHeight;
      gp_Trsf             aTrsf;
      aTrsf.SetTransformation(aCone.Position(), gp::XOY());
      Handle(Select3D_SensitiveCylinder) aSensSCyl =
        new Select3D_SensitiveCylinder(theOwner, aRad1, aRad2, aHeight, aTrsf);
      theSelection->Add(aSensSCyl);
      return Standard_True;
    }
  }
  if (theSubfacesMap.Extent() == 3) // detect cylinder or truncated cone
  {
    const TopoDS_Face* aFaces[3] = {&TopoDS::Face(theSubfacesMap.FindKey(1)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(2)),
                                    &TopoDS::Face(theSubfacesMap.FindKey(3))};

    TopLoc_Location             aLocSurf[3];
    const Handle(Geom_Surface)* aSurfaces[3] = {&BRep_Tool::Surface(*aFaces[0], aLocSurf[0]),
                                                &BRep_Tool::Surface(*aFaces[1], aLocSurf[1]),
                                                &BRep_Tool::Surface(*aFaces[2], aLocSurf[2])};

    Standard_Integer                aConIndex = -1, aNbPlanes = 0;
    Handle(Geom_ConicalSurface)     aGeomCone;
    Handle(Geom_CylindricalSurface) aGeomCyl;
    Handle(Geom_Plane)              aGeomPlanes[2];
    const TopLoc_Location*          aGeomPlanesLoc[2];
    for (Standard_Integer aSurfIter = 0; aSurfIter < 3; ++aSurfIter)
    {
      const Handle(Geom_Surface)& aSurf = *aSurfaces[aSurfIter];
      if (aConIndex == -1)
      {
        aGeomCone = Handle(Geom_ConicalSurface)::DownCast(aSurf);
        if (!aGeomCone.IsNull())
        {
          aConIndex = aSurfIter;
          continue;
        }
        aGeomCyl = Handle(Geom_CylindricalSurface)::DownCast(aSurf);
        if (!aGeomCyl.IsNull())
        {
          aConIndex = aSurfIter;
          continue;
        }
      }
      if (aNbPlanes < 2)
      {
        aGeomPlanes[aNbPlanes] = Handle(Geom_Plane)::DownCast(aSurf);
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
        const gp_Cone       aCone = BRepAdaptor_Surface(*aFaces[aConIndex]).Cone();
        const Standard_Real aRad1 = aCone.RefRadius();
        const Standard_Real aHeight =
          aGeomPlanes[0]
            ->Location()
            .Transformed(*aGeomPlanesLoc[0])
            .Distance(aGeomPlanes[1]->Location().Transformed(*aGeomPlanesLoc[1]));
        gp_Trsf aTrsf;
        aTrsf.SetTransformation(aCone.Position(), gp::XOY());
        const Standard_Real aTriangleHeight = (aCone.SemiAngle() > 0.0)
                                                ? aRad1 / Tan(aCone.SemiAngle())
                                                : aRad1 / Tan(Abs(aCone.SemiAngle())) - aHeight;
        const Standard_Real aRad2           = (aCone.SemiAngle() > 0.0)
                                                ? aRad1 * (aTriangleHeight + aHeight) / aTriangleHeight
                                                : aRad1 * aTriangleHeight / (aTriangleHeight + aHeight);

        Handle(Select3D_SensitiveCylinder) aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad1, aRad2, aHeight, aTrsf);
        theSelection->Add(aSensSCyl);
        return Standard_True;
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
        const gp_Cylinder   aCyl = BRepAdaptor_Surface(*aFaces[aConIndex]).Cylinder();
        const Standard_Real aRad = aCyl.Radius();
        const Standard_Real aHeight =
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

        Handle(Select3D_SensitiveCylinder) aSensSCyl =
          new Select3D_SensitiveCylinder(theOwner, aRad, aRad, aHeight, aTrsf);
        theSelection->Add(aSensSCyl);
        return Standard_True;
      }
    }
  }

  return Standard_False;
}
