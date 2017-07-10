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

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom_Circle.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Circ.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
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
#include <TopTools_IndexedMapOfShape.hxx>

#define BVH_PRIMITIVE_LIMIT 800000

//==================================================
// function: PreBuildBVH
// purpose : Pre-builds BVH tree for heavyweight
//           sensitive entities with sub-elements
//           amount more than BVH_PRIMITIVE_LIMIT
//==================================================
void StdSelect_BRepSelectionTool::PreBuildBVH (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    const Handle(SelectBasics_SensitiveEntity)& aSensitive = theSelection->Sensitive()->BaseSensitive();
    if (aSensitive->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
    {
      aSensitive->BVH();
    }

    if (!aSensitive->IsInstance (STANDARD_TYPE(Select3D_SensitiveGroup)))
    {
      continue;
    }

    Handle(Select3D_SensitiveGroup) aGroup = Handle(Select3D_SensitiveGroup)::DownCast (aSensitive);
    for (Select3D_IndexedMapOfEntity::Iterator aSubEntitiesIter (aGroup->Entities()); aSubEntitiesIter.More(); aSubEntitiesIter.Next())
    {
      const Handle(Select3D_SensitiveEntity)& aSubEntity = aSubEntitiesIter.Value();
      if (aSubEntity->NbSubElements() >= BVH_PRIMITIVE_LIMIT)
      {
        aSubEntity->BVH();
      }
    }
  }
}

//==================================================
// Function: Load
// Purpose :
//==================================================
void StdSelect_BRepSelectionTool::Load (const Handle(SelectMgr_Selection)& theSelection,
                                        const TopoDS_Shape& theShape,
                                        const TopAbs_ShapeEnum theType,
                                        const Standard_Real theDeflection,
                                        const Standard_Real theDeviationAngle,
                                        const Standard_Boolean isAutoTriangulation,
                                        const Standard_Integer thePriority,
                                        const Standard_Integer theNbPOnEdge,
                                        const Standard_Real theMaxParam)
{
  Standard_Integer aPriority = (thePriority == -1) ? GetStandardPriority (theShape, theType) : thePriority;

  if( isAutoTriangulation && !BRepTools::Triangulation (theShape, Precision::Infinite()) )
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
    case TopAbs_COMPSOLID:
    {
      TopTools_IndexedMapOfShape aSubShapes;
      TopExp::MapShapes (theShape, theType, aSubShapes);

      Standard_Boolean isComesFromDecomposition = !((aSubShapes.Extent() == 1) && (theShape == aSubShapes (1)));
      for (Standard_Integer aShIndex = 1; aShIndex <= aSubShapes.Extent(); ++aShIndex)
      {
        const TopoDS_Shape& aSubShape = aSubShapes (aShIndex);
        aBrepOwner = new StdSelect_BRepOwner (aSubShape, aPriority, isComesFromDecomposition);
        ComputeSensitive (aSubShape, aBrepOwner,
                          theSelection,
                          theDeflection,
                          theDeviationAngle,
                          theNbPOnEdge,
                          theMaxParam,
                          isAutoTriangulation);
      }
      break;
    }
    default:
    {
      aBrepOwner = new StdSelect_BRepOwner (theShape, aPriority);
      ComputeSensitive (theShape, aBrepOwner,
                        theSelection,
                        theDeflection,
                        theDeviationAngle,
                        theNbPOnEdge,
                        theMaxParam,
                        isAutoTriangulation);
    }
  }
}

//==================================================
// Function: Load
// Purpose :
//==================================================
void StdSelect_BRepSelectionTool::Load (const Handle(SelectMgr_Selection)& theSelection,
                                        const Handle(SelectMgr_SelectableObject)& theSelectableObj,
                                        const TopoDS_Shape& theShape,
                                        const TopAbs_ShapeEnum theType,
                                        const Standard_Real theDeflection,
                                        const Standard_Real theDeviationAngle,
                                        const Standard_Boolean isAutoTriangulation,
                                        const Standard_Integer thePriority,
                                        const Standard_Integer theNbPOnEdge,
                                        const Standard_Real theMaxParam)
{
  Load (theSelection,
        theShape,
        theType,
        theDeflection,
        theDeviationAngle,
        isAutoTriangulation,
        thePriority,
        theNbPOnEdge,
        theMaxParam);

  // loading of selectables...
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    Handle(SelectMgr_EntityOwner) anOwner
      = Handle(SelectMgr_EntityOwner)::DownCast (theSelection->Sensitive()->BaseSensitive()->OwnerId());
    anOwner->Set (theSelectableObj);
  }

  PreBuildBVH (theSelection);
}

//==================================================
// Function: ComputeSensitive
// Purpose :
//==================================================
void StdSelect_BRepSelectionTool::ComputeSensitive (const TopoDS_Shape& theShape,
                                                    const Handle(SelectMgr_EntityOwner)& theOwner,
                                                    const Handle(SelectMgr_Selection)& theSelection,
                                                    const Standard_Real theDeflection,
                                                    const Standard_Real theDeviationAngle,
                                                    const Standard_Integer theNbPOnEdge,
                                                    const Standard_Real theMaxParam,
                                                    const Standard_Boolean isAutoTriangulation)
{
  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX:
    {
      theSelection->Add (new Select3D_SensitivePoint
                         (theOwner, BRep_Tool::Pnt (TopoDS::Vertex (theShape))));
      break;
    }
    case TopAbs_EDGE:
    {
      Handle(Select3D_SensitiveEntity) aSensitive;
      GetEdgeSensitive (theShape, theOwner, theSelection,
                        theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam,
                        aSensitive);
      if (!aSensitive.IsNull())
      {
        theSelection->Add (aSensitive);
      }
      break;
    }
    case TopAbs_WIRE:
    {
      BRepTools_WireExplorer aWireExp (TopoDS::Wire (theShape));
      Handle (Select3D_SensitiveEntity) aSensitive;
      Handle (Select3D_SensitiveWire) aWireSensitive = new Select3D_SensitiveWire (theOwner);
      theSelection->Add (aWireSensitive);
      while (aWireExp.More())
      {
        GetEdgeSensitive (aWireExp.Current(), theOwner, theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam,
                          aSensitive);
        if (!aSensitive.IsNull())
        {
          aWireSensitive->Add (aSensitive);
        }
        aWireExp.Next();
      }
      break;
    }
    case TopAbs_FACE:
    {
      const TopoDS_Face& aFace = TopoDS::Face (theShape);
      Select3D_EntitySequence aSensitiveList;
      GetSensitiveForFace (aFace, theOwner,
                           aSensitiveList,
                           isAutoTriangulation, theNbPOnEdge, theMaxParam);
      for (Select3D_EntitySequenceIter aSensIter (aSensitiveList);
           aSensIter.More(); aSensIter.Next())
      {
        theSelection->Add (aSensIter.Value());
      }
      break;
    }
    case TopAbs_SHELL:
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID:
    {
      TopTools_IndexedMapOfShape aSubfacesMap;
      TopExp::MapShapes (theShape, TopAbs_FACE, aSubfacesMap);
      for (Standard_Integer aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
      {
        ComputeSensitive (aSubfacesMap (aShIndex), theOwner,
                          theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam, isAutoTriangulation);
      }
      break;
    }
    case TopAbs_COMPOUND:
    default:
    {
      TopExp_Explorer anExp;
      // sub-vertices
      for (anExp.Init (theShape, TopAbs_VERTEX, TopAbs_EDGE); anExp.More(); anExp.Next())
      {
        ComputeSensitive (anExp.Current(), theOwner,
                          theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam, isAutoTriangulation);
      }
      // sub-edges
      for (anExp.Init (theShape, TopAbs_EDGE, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        ComputeSensitive (anExp.Current(), theOwner,
                          theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam, isAutoTriangulation);
      }
      // sub-wires
      for (anExp.Init (theShape, TopAbs_WIRE, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        ComputeSensitive (anExp.Current(), theOwner,
                          theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam, isAutoTriangulation);
      }

      // sub-faces
      TopTools_IndexedMapOfShape aSubfacesMap;
      TopExp::MapShapes (theShape, TopAbs_FACE, aSubfacesMap);
      for (Standard_Integer aShIndex = 1; aShIndex <= aSubfacesMap.Extent(); ++aShIndex)
      {
        ComputeSensitive (aSubfacesMap (aShIndex), theOwner,
                          theSelection,
                          theDeflection, theDeviationAngle, theNbPOnEdge, theMaxParam, isAutoTriangulation);
      }
    }
  }
}

//==================================================
// Function: GetPointsFromPolygon
// Purpose :
//==================================================
static Handle(TColgp_HArray1OfPnt) GetPointsFromPolygon (const TopoDS_Edge& theEdge)
{
  Handle(TColgp_HArray1OfPnt) aResultPoints;

  TopLoc_Location aLocation;
  Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D (theEdge, aLocation);
  if (!aPolygon.IsNull())
  {
    const TColgp_Array1OfPnt& aNodes = aPolygon->Nodes();
    aResultPoints = new TColgp_HArray1OfPnt (1, aNodes.Length());
    if (aLocation.IsIdentity())
    {
      for (Standard_Integer aNodeId (aNodes.Lower()), aPntId (1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue (aPntId, aNodes.Value (aNodeId));
      }
    }
    else
    {
      for (Standard_Integer aNodeId (aNodes.Lower()), aPntId (1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
      {
        aResultPoints->SetValue (aPntId, aNodes.Value (aNodeId).Transformed (aLocation));
      }
    }
    return aResultPoints;
  }

  Handle(Poly_Triangulation) aTriangulation;
  Handle(Poly_PolygonOnTriangulation) anHIndices;
  BRep_Tool::PolygonOnTriangulation (theEdge, anHIndices, aTriangulation, aLocation);
  if (!anHIndices.IsNull())
  {
    const TColStd_Array1OfInteger& anIndices = anHIndices->Nodes();
    const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();

    aResultPoints = new TColgp_HArray1OfPnt (1, anIndices.Length());

    if (aLocation.IsIdentity())
    {
      for (Standard_Integer anIndex (anIndices.Lower()), aPntId (1); anIndex <= anIndices.Upper(); ++anIndex, ++aPntId)
      {
        aResultPoints->SetValue (aPntId, aNodes (anIndices (anIndex)));
      }
    }
    else
    {
      for (Standard_Integer anIndex (anIndices.Lower()), aPntId (1); anIndex <= anIndices.Upper(); ++anIndex, ++aPntId)
      {
        aResultPoints->SetValue (aPntId, aNodes (anIndices (anIndex)).Transformed (aLocation));
      }
    }
    return aResultPoints;
  }
  return aResultPoints;
}

//==================================================
// Function: FindLimits
// Purpose :
//==================================================
static Standard_Boolean FindLimits (const Adaptor3d_Curve& theCurve,
                                    const Standard_Real    theLimit,
                                          Standard_Real&   theFirst,
                                          Standard_Real&   theLast)
{
  theFirst = theCurve.FirstParameter();
  theLast  = theCurve.LastParameter();
  Standard_Boolean isFirstInf = Precision::IsNegativeInfinite (theFirst);
  Standard_Boolean isLastInf  = Precision::IsPositiveInfinite (theLast);
  if (isFirstInf || isLastInf)
  {
    gp_Pnt aPnt1, aPnt2;
    Standard_Real aDelta = 1.0;
    Standard_Integer anIterCount = 0;
    if (isFirstInf && isLastInf)
    {
      do {
        if (anIterCount++ >= 100000) return Standard_False;
        aDelta *= 2.0;
        theFirst = - aDelta;
        theLast  =   aDelta;
        theCurve.D0 (theFirst, aPnt1);
        theCurve.D0 (theLast,  aPnt2);
      } while (aPnt1.Distance (aPnt2) < theLimit);
    }
    else if (isFirstInf)
    {
      theCurve.D0 (theLast, aPnt2);
      do {
        if (anIterCount++ >= 100000) return Standard_False;
        aDelta *= 2.0;
        theFirst = theLast - aDelta;
        theCurve.D0 (theFirst, aPnt1);
      } while (aPnt1.Distance (aPnt2) < theLimit);
    }
    else if (isLastInf)
    {
      theCurve.D0 (theFirst, aPnt1);
      do {
        if (anIterCount++ >= 100000) return Standard_False;
        aDelta *= 2.0;
        theLast = theFirst + aDelta;
        theCurve.D0 (theLast, aPnt2);
      } while (aPnt1.Distance (aPnt2) < theLimit);
    }
  }
  return Standard_True;
}

//=====================================================
// Function : GetEdgeSensitive
// Purpose  :
//=====================================================
void StdSelect_BRepSelectionTool::GetEdgeSensitive (const TopoDS_Shape& theShape,
                                                    const Handle(SelectMgr_EntityOwner)& theOwner,
                                                    const Handle(SelectMgr_Selection)& theSelection,
                                                    const Standard_Real theDeflection,
                                                    const Standard_Real theDeviationAngle,
                                                    const Standard_Integer theNbPOnEdge,
                                                    const Standard_Real theMaxParam,
                                                    Handle(Select3D_SensitiveEntity)& theSensitive)
{
  const TopoDS_Edge& anEdge = TopoDS::Edge (theShape);
  BRepAdaptor_Curve cu3d;
  try {
    OCC_CATCH_SIGNALS
    cu3d.Initialize (anEdge);
  } catch (Standard_NullObject) {
    return;
  }

  // try to get points from existing polygons
  Handle(TColgp_HArray1OfPnt) aPoints = GetPointsFromPolygon (anEdge);
  if (!aPoints.IsNull() && aPoints->Length() > 0)
  {
    theSensitive = new Select3D_SensitiveCurve (theOwner, aPoints);
    return;
  }

  Standard_Real aParamFirst = cu3d.FirstParameter();
  Standard_Real aParamLast  = cu3d.LastParameter();
  switch (cu3d.GetType())
  {
    case GeomAbs_Line:
    {
      BRep_Tool::Range (anEdge, aParamFirst, aParamLast);
      theSensitive = new Select3D_SensitiveSegment (theOwner,
                                                    cu3d.Value (aParamFirst),
                                                    cu3d.Value (aParamLast));
      break;
    }
    case GeomAbs_Circle:
    {
      Handle (Geom_Circle) aCircle = new Geom_Circle (cu3d.Circle());
      if (aCircle->Radius() <= Precision::Confusion())
      {
        theSelection->Add (new Select3D_SensitivePoint (theOwner, aCircle->Location()));
      }
      else
      {
        theSensitive = new Select3D_SensitiveCircle (theOwner, aCircle,
                                                     aParamFirst, aParamLast, Standard_False, 16);
      }
      break;
    }
    default:
    {
      // reproduce drawing behaviour
      // TODO: remove copy-paste from StdPrs_Curve and some others...
      if (FindLimits (cu3d, theMaxParam, aParamFirst, aParamLast))
      {
        Standard_Integer aNbIntervals = cu3d.NbIntervals (GeomAbs_C1);
        TColStd_Array1OfReal anIntervals (1, aNbIntervals + 1);
        cu3d.Intervals (anIntervals, GeomAbs_C1);
        Standard_Real aV1, aV2;
        Standard_Integer aNumberOfPoints;
        TColgp_SequenceOfPnt aPointsSeq;
        for (Standard_Integer anIntervalId = 1; anIntervalId <= aNbIntervals; ++anIntervalId)
        {
          aV1 = anIntervals (anIntervalId);
          aV2 = anIntervals (anIntervalId + 1);
          if (aV2 > aParamFirst && aV1 < aParamLast)
          {
            aV1 = Max (aV1, aParamFirst);
            aV2 = Min (aV2, aParamLast);

            GCPnts_TangentialDeflection anAlgo (cu3d, aV1, aV2, theDeviationAngle, theDeflection);
            aNumberOfPoints = anAlgo.NbPoints();

            for (Standard_Integer aPntId = 1; aPntId < aNumberOfPoints; ++aPntId)
            {
              aPointsSeq.Append (anAlgo.Value (aPntId));
            }
            if (aNumberOfPoints > 0 && anIntervalId == aNbIntervals)
            {
              aPointsSeq.Append (anAlgo.Value (aNumberOfPoints));
            }
          }
        }

        aPoints = new TColgp_HArray1OfPnt (1, aPointsSeq.Length());
        for (Standard_Integer aPntId = 1; aPntId <= aPointsSeq.Length(); ++aPntId)
        {
          aPoints->SetValue (aPntId, aPointsSeq.Value (aPntId));
        }
        theSensitive = new Select3D_SensitiveCurve (theOwner, aPoints);
        break;
      }

      // simple subdivisions
      Standard_Integer nbintervals = 1;
      if (cu3d.GetType() == GeomAbs_BSplineCurve)
      {
        nbintervals = cu3d.NbKnots() - 1;
        nbintervals = Max (1, nbintervals / 3);
      }

      Standard_Real aParam;
      Standard_Integer aPntNb = Max (2, theNbPOnEdge * nbintervals);
      Standard_Real aParamDelta = (aParamLast - aParamFirst) / (aPntNb - 1);
      Handle(TColgp_HArray1OfPnt) aPointArray = new TColgp_HArray1OfPnt (1, aPntNb);
      for (Standard_Integer aPntId = 1; aPntId <= aPntNb; ++aPntId)
      {
        aParam = aParamFirst + aParamDelta * (aPntId - 1);
        aPointArray->SetValue (aPntId, cu3d.Value (aParam));
      }
      theSensitive = new Select3D_SensitiveCurve (theOwner, aPointArray);
    }
    break;
  }
}

//=======================================================================
//function : GetSensitiveEntityForFace
//purpose  :
//=======================================================================
Standard_Boolean StdSelect_BRepSelectionTool::GetSensitiveForFace (const TopoDS_Face& theFace,
                                                                   const Handle(SelectMgr_EntityOwner)& theOwner,
                                                                   Select3D_EntitySequence& theSensitiveList,
                                                                   const Standard_Boolean /*theAutoTriangulation*/,
                                                                   const Standard_Integer NbPOnEdge,
                                                                   const Standard_Real    theMaxParam,
                                                                   const Standard_Boolean theInteriorFlag)
{
  // check if there is triangulation of the face...
  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (theFace, aLoc);

  if (!aTriangulation.IsNull())
  {
    Handle(Select3D_SensitiveTriangulation) STG = new Select3D_SensitiveTriangulation (theOwner, aTriangulation, aLoc, theInteriorFlag);
    theSensitiveList.Append (STG);
    return Standard_True;
  }

  // for faces with triangulation bugs or without autotriangulation ....
  // very ugly and should not even exist ...
  BRepAdaptor_Surface BS;
  BS.Initialize (theFace);

  Standard_Real FirstU = BS.FirstUParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstUParameter();
  Standard_Real LastU  = BS.LastUParameter()  >=  Precision::Infinite() ?  theMaxParam : BS.LastUParameter();
  Standard_Real FirstV = BS.FirstVParameter() <= -Precision::Infinite() ? -theMaxParam : BS.FirstVParameter();
  Standard_Real LastV  = BS.LastVParameter()  >=  Precision::Infinite() ?  theMaxParam : BS.LastVParameter();

  if (BS.GetType() == GeomAbs_Plane)
  {
    gp_Pnt pcur;
    Handle(TColgp_HArray1OfPnt) P = new TColgp_HArray1OfPnt (1, 5);
    BS.D0 (FirstU, FirstV, pcur);
    P->SetValue (1, pcur);
    BS.D0 (LastU, FirstV, pcur);
    P->SetValue (2, pcur);
    BS.D0 (LastU, LastV, pcur);
    P->SetValue (3, pcur);
    BS.D0 (FirstU, LastV, pcur);
    P->SetValue (4, pcur);
    P->SetValue (5, P->Value (1));
    // if the plane is "infinite", it is sensitive only on the border limited by MaxParam
    if (FirstU == -theMaxParam && LastU == theMaxParam && FirstV == -theMaxParam && LastV == theMaxParam)
    {
      theSensitiveList.Append (new Select3D_SensitiveFace (theOwner, P, Select3D_TOS_BOUNDARY));
    }
    else
    {
      Select3D_TypeOfSensitivity TS = theInteriorFlag ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
      theSensitiveList.Append (new Select3D_SensitiveFace (theOwner, P, TS));
    }
    return Standard_True;
  }

  // This is construction of a sevsitive polygon from the exterior contour of the face...
  // It is not good at all, but...
  TopoDS_Wire aWire;
  TopExp_Explorer anExpWiresInFace (theFace, TopAbs_WIRE);
  if (anExpWiresInFace.More())
  {
    // believing that this is the first... to be seen
    aWire = TopoDS::Wire (anExpWiresInFace.Current());
  }
  if (aWire.IsNull())
  {
    return Standard_False;
  }

  TColgp_SequenceOfPnt WirePoints;
  Standard_Boolean FirstExp = Standard_True;
  Standard_Real wf, wl;
  BRepAdaptor_Curve cu3d;
  for (BRepTools_WireExplorer aWireExplorer (aWire);
       aWireExplorer.More(); aWireExplorer.Next())
  {
    cu3d.Initialize (aWireExplorer.Current());
    BRep_Tool::Range (aWireExplorer.Current(), wf, wl);
    if (Abs (wf - wl) <= Precision::Confusion())
    {
    #ifdef OCCT_DEBUG
      cout<<" StdSelect_BRepSelectionTool : Curve where ufirst = ulast ...."<<endl;
    #endif
    }
    else
    {
      if (FirstExp)
      {
        if (aWireExplorer.Orientation() == TopAbs_FORWARD)
        {
          WirePoints.Append (cu3d.Value (wf));
        }
        else
        {
          WirePoints.Append (cu3d.Value (wl));
        }
        FirstExp = Standard_False;
      }

      switch (cu3d.GetType())
      {
        case GeomAbs_Line:
        {
          WirePoints.Append (cu3d.Value ((aWireExplorer.Orientation() == TopAbs_FORWARD) ? wl : wf));
          break;
        }
        case GeomAbs_Circle:
        {
          if (2 * M_PI - Abs (wl - wf) <= Precision::Confusion())
          {
            if (BS.GetType() == GeomAbs_Cylinder ||
                BS.GetType() == GeomAbs_Torus ||
                BS.GetType() == GeomAbs_Cone  ||
                BS.GetType() == GeomAbs_BSplineSurface) // beuurkk pour l'instant...
            {
              Standard_Real ff = wf ,ll = wl;
              Standard_Real dw =(Max (wf, wl) - Min (wf, wl)) / (Standard_Real )Max (2, NbPOnEdge - 1);
              if (aWireExplorer.Orientation() == TopAbs_FORWARD)
              {
                for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
                {
                  WirePoints.Append (cu3d.Value (wc));
                }
              }
              else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
              {
                for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
                {
                  WirePoints.Append (cu3d.Value (wc));
                }
              }
            }
            else
            {
              if (cu3d.Circle().Radius() <= Precision::Confusion())
              {
                theSensitiveList.Append (new Select3D_SensitivePoint (theOwner, cu3d.Circle().Location()));
              }
              else
              {
                theSensitiveList.Append (new Select3D_SensitiveCircle (theOwner, new Geom_Circle (cu3d.Circle()), theInteriorFlag, 16));
              }
            }
          }
          else
          {
            Standard_Real ff = wf, ll = wl;
            Standard_Real dw = (Max (wf, wl) - Min (wf, wl)) / (Standard_Real )Max (2, NbPOnEdge - 1);
            if (aWireExplorer.Orientation() == TopAbs_FORWARD)
            {
              for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
              {
                WirePoints.Append (cu3d.Value (wc));
              }
            }
            else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
            {
              for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
              {
                WirePoints.Append (cu3d.Value (wc));
              }
            }
          }
          break;
        }
        default:
        {
          Standard_Real ff = wf, ll = wl;
          Standard_Real dw = (Max (wf, wl) - Min (wf, wl)) / (Standard_Real )Max (2, NbPOnEdge - 1);
          if (aWireExplorer.Orientation()==TopAbs_FORWARD)
          {
            for (Standard_Real wc = wf + dw; wc <= wl; wc += dw)
            {
              WirePoints.Append (cu3d.Value (wc));
            }
          }
          else if (aWireExplorer.Orientation() == TopAbs_REVERSED)
          {
            for (Standard_Real wc = ll - dw; wc >= ff; wc -= dw)
            {
              WirePoints.Append (cu3d.Value (wc));
            }
          }
        }
      }
    }
  }
  Standard_Integer ArrayPosition = WirePoints.Length();

  Handle(TColgp_HArray1OfPnt) facepoints = new TColgp_HArray1OfPnt (1, ArrayPosition);
  for (Standard_Integer I = 1; I <= ArrayPosition; ++I)
  {
    facepoints->SetValue (I, WirePoints.Value(I));
  }

  if ((facepoints->Array1()).Length() > 1)
  { //1 if only one circular edge
    Select3D_TypeOfSensitivity TS = theInteriorFlag ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
    theSensitiveList.Append (new Select3D_SensitiveFace (theOwner, facepoints, TS));
  }
  return Standard_True;
}
