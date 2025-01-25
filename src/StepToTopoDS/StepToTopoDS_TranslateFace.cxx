// Created on: 1995-01-03
// Created by: Frederic MAUPAS
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

// pdn 30.11.98: fixes improved
// pdn 20.12.98: to keep pcurves
// pdn 28.12.98: PRO10366 shifting pcurve between two singularities
//: p4 abv, pdn 23.02.99: PRO9234 #15720: call BRepTools::Update() for faces
//: q7 abv 23.03.99: bm4_al_eye.stp #53710: remove pseudo-seams
//    rln 31.03.99 S4135: prohibit fixing intersection of non-adjacent edges (temporarily)
// #4  szv          S4163: optimization
//%19 pdn 17.04.99 using ShapeFix_Wire::FixEdgeCurves instead of ShapeFix_PCurves
//    smh 31.01.01 Bad data in file : case of vertex loop on plane face
// sln 01.10.2001 BUC61003. StepToTopoDS_TranslateFace::Init function is corrected (verifying
// Handle(...).IsNull() is added)

#include <BRep_Builder.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <MoniTool_Macros.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_OffsetSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_FaceOuterBound.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateEdgeLoop.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepToTopoDS_TranslatePolyLoop.hxx>
#include <StepToTopoDS_TranslateVertexLoop.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepVisual_TriangulatedFace.hxx>
#include <StepVisual_ComplexTriangulatedFace.hxx>
#include <StepVisual_TriangulatedSurfaceSet.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>

// #3 rln 16/02/98
// #include <GeomAdaptor_Curve.hxx>
// #include <GeomAdaptor_CurveOnSurface.hxx>
// #3 rln 16/02/98
// rln 28/01/98
// rln 28/01/98
//   Provisoire, pour VertexLoop
//: d4
// To proceed with I-DEAS-like STP (ssv; 15.11.2010)
// #define DEBUG

namespace
{
// ============================================================================
// Method  : SetNodes
// Purpose : Set nodes to the triangulation from an array
// ============================================================================
static void SetNodes(const Handle(Poly_Triangulation)& theMesh,
                     Handle(TColgp_HArray1OfXYZ)&      theNodes,
                     const Standard_Integer            theNumPnindex,
                     Handle(TColStd_HArray1OfInteger)& thePnindices,
                     const Standard_Real               theLengthFactor)
{
  for (Standard_Integer aPnIndex = 1; aPnIndex <= theMesh->NbNodes(); ++aPnIndex)
  {
    const gp_XYZ& aPoint =
      theNodes->Value((theNumPnindex > 0) ? thePnindices->Value(aPnIndex) : aPnIndex);
    theMesh->SetNode(aPnIndex, theLengthFactor * aPoint);
  }
}

// ============================================================================
// Method  : SetNormals
// Purpose : Set normals to the triangulation from an array
// ============================================================================
static void SetNormals(const Handle(Poly_Triangulation)&    theMesh,
                       const Handle(TColStd_HArray2OfReal)& theNormals,
                       const Standard_Integer               theNormNum,
                       const Standard_Integer               theNumPnindex)
{
  if (theNormals->RowLength() != 3)
  {
    return;
  }
  gp_XYZ aNorm;
  if (theNormNum == 1)
  {
    aNorm.SetX(theNormals->Value(1, 1));
    aNorm.SetY(theNormals->Value(1, 2));
    aNorm.SetZ(theNormals->Value(1, 3));
    for (Standard_Integer aPnIndex = 1; aPnIndex <= theNumPnindex; ++aPnIndex)
    {
      theMesh->SetNormal(aPnIndex, aNorm);
    }
  }
  else if (theNumPnindex == theNormNum)
  {
    for (Standard_Integer aNormIndex = 1; aNormIndex <= theNormNum; ++aNormIndex)
    {
      aNorm.SetX(theNormals->Value(aNormIndex, 1));
      aNorm.SetY(theNormals->Value(aNormIndex, 2));
      aNorm.SetZ(theNormals->Value(aNormIndex, 3));
      theMesh->SetNormal(aNormIndex, aNorm);
    }
  }
}

// ============================================================================
// Method  : SetTriangles
// Purpose : Set triangles to the triangulation from an array
// ============================================================================
static void SetTriangles(const Handle(Poly_Triangulation)&         theMesh,
                         const Handle(TColStd_HArray2OfInteger)    theTriangles,
                         const Standard_Integer                    theTrianStripsNum,
                         const Handle(TColStd_HArray1OfTransient)& theTrianStrips,
                         const Standard_Integer                    theTrianFansNum,
                         const Handle(TColStd_HArray1OfTransient)& theTrianFans)
{
  if (theTrianStripsNum == 0 && theTrianFansNum == 0)
  {
    for (Standard_Integer aTrianIndex = 1; aTrianIndex <= theMesh->NbTriangles(); ++aTrianIndex)
    {
      theMesh->SetTriangle(aTrianIndex,
                           Poly_Triangle(theTriangles->Value(aTrianIndex, 1),
                                         theTriangles->Value(aTrianIndex, 2),
                                         theTriangles->Value(aTrianIndex, 3)));
    }
  }
  else
  {
    Standard_Integer aTriangleIndex = 1;
    for (Standard_Integer aTrianStripIndex = 1; aTrianStripIndex <= theTrianStripsNum;
         ++aTrianStripIndex)
    {
      Handle(TColStd_HArray1OfInteger) aTriangleStrip =
        Handle(TColStd_HArray1OfInteger)::DownCast(theTrianStrips->Value(aTrianStripIndex));
      for (Standard_Integer anIndex = 3; anIndex <= aTriangleStrip->Length(); anIndex += 2)
      {
        if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2)
            && aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
        {
          theMesh->SetTriangle(aTriangleIndex++,
                               Poly_Triangle(aTriangleStrip->Value(anIndex - 2),
                                             aTriangleStrip->Value(anIndex),
                                             aTriangleStrip->Value(anIndex - 1)));
        }
      }
      for (Standard_Integer anIndex = 4; anIndex <= aTriangleStrip->Length(); anIndex += 2)
      {
        if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2)
            && aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
        {
          theMesh->SetTriangle(aTriangleIndex++,
                               Poly_Triangle(aTriangleStrip->Value(anIndex - 2),
                                             aTriangleStrip->Value(anIndex - 1),
                                             aTriangleStrip->Value(anIndex)));
        }
      }
    }
    for (Standard_Integer aTrianFanIndex = 1; aTrianFanIndex <= theTrianFansNum; ++aTrianFanIndex)
    {
      Handle(TColStd_HArray1OfInteger) aTriangleFan =
        Handle(TColStd_HArray1OfInteger)::DownCast(theTrianFans->Value(aTrianFanIndex));
      for (Standard_Integer anIndex = 3; anIndex <= aTriangleFan->Length(); ++anIndex)
      {
        if (aTriangleFan->Value(anIndex) != aTriangleFan->Value(anIndex - 2)
            && aTriangleFan->Value(anIndex - 1) != aTriangleFan->Value(anIndex - 2))
        {
          theMesh->SetTriangle(aTriangleIndex++,
                               Poly_Triangle(aTriangleFan->Value(1),
                                             aTriangleFan->Value(anIndex),
                                             aTriangleFan->Value(anIndex - 1)));
        }
      }
    }
  }
}

// ============================================================================
// Method  : GetSimpleFaceElements
// Purpose : Get elements from simple face
// ============================================================================
template <class Type>
static void GetSimpleFaceElements(Type                              theFace,
                                  Handle(TColgp_HArray1OfXYZ)&      theNodes,
                                  Handle(TColStd_HArray2OfReal)&    theNormals,
                                  Handle(TColStd_HArray2OfInteger)& theTriangles,
                                  Standard_Integer&                 thePnIndNb,
                                  Standard_Integer&                 theNormNb,
                                  Standard_Integer&                 theTriNb,
                                  Handle(TColStd_HArray1OfInteger)& thePnindices)
{
  theNodes     = theFace->Coordinates()->Points();
  theNormals   = theFace->Normals();
  theTriangles = theFace->Triangles();
  thePnIndNb   = theFace->NbPnindex();
  theNormNb    = theFace->NbNormals();
  theTriNb     = theFace->NbTriangles();
  thePnindices = new TColStd_HArray1OfInteger(1, thePnIndNb);
  for (Standard_Integer anIndx = 1; anIndx <= thePnIndNb; ++anIndx)
  {
    thePnindices->SetValue(anIndx, theFace->PnindexValue(anIndx));
  }
}

// ============================================================================
// Method  : GetComplexFaceElements
// Purpose : Get elements from complex face
// ============================================================================
template <class Type>
static void GetComplexFaceElements(Type                                theFace,
                                   Handle(TColgp_HArray1OfXYZ)&        theNodes,
                                   Handle(TColStd_HArray2OfReal)&      theNormals,
                                   Handle(TColStd_HArray1OfTransient)& theTriangleStrips,
                                   Handle(TColStd_HArray1OfTransient)& theTriangleFans,
                                   Standard_Integer&                   thePnIndNb,
                                   Standard_Integer&                   theNormNb,
                                   Standard_Integer&                   theTriStripsNb,
                                   Standard_Integer&                   theTriFansNb,
                                   Handle(TColStd_HArray1OfInteger)&   thePnindices)
{
  theNodes          = theFace->Coordinates()->Points();
  theNormals        = theFace->Normals();
  theTriangleStrips = theFace->TriangleStrips();
  theTriangleFans   = theFace->TriangleFans();
  thePnIndNb        = theFace->NbPnindex();
  theNormNb         = theFace->NbNormals();
  theTriStripsNb    = theFace->NbTriangleStrips();
  theTriFansNb      = theFace->NbTriangleFans();
  thePnindices      = new TColStd_HArray1OfInteger(1, thePnIndNb);
  for (Standard_Integer anIndx = 1; anIndx <= thePnIndNb; ++anIndx)
  {
    thePnindices->SetValue(anIndx, theFace->PnindexValue(anIndx));
  }
}

// ============================================================================
// Method  : CreatePolyTriangulation
// Purpose : Create PolyTriangulation
// ============================================================================
static Handle(Poly_Triangulation) CreatePolyTriangulation(
  const Handle(StepVisual_TessellatedItem)& theTI,
  const StepData_Factors&                   theLocalFactors)
{
  Handle(Poly_Triangulation) aMesh;
  if (theTI.IsNull())
  {
    return Handle(Poly_Triangulation)();
  }

  Handle(TColgp_HArray1OfXYZ)      aNodes;
  Handle(TColStd_HArray2OfReal)    aNormals;
  Handle(TColStd_HArray2OfInteger) aTriangles;
  Standard_Integer                 aNumPnindex = 0;
  Standard_Integer                 aNormNum    = 0;
  Standard_Integer                 aTrianNum   = 0;
  Handle(TColStd_HArray1OfInteger) aPnindices;

  Handle(TColStd_HArray1OfTransient) aTriaStrips;
  Handle(TColStd_HArray1OfTransient) aTriaFans;
  Standard_Integer                   aTrianStripsNum = 0;
  Standard_Integer                   aTrianFansNum   = 0;

  if (theTI->IsKind(STANDARD_TYPE(StepVisual_TriangulatedFace)))
  {
    Handle(StepVisual_TriangulatedFace) aTF = Handle(StepVisual_TriangulatedFace)::DownCast(theTI);
    GetSimpleFaceElements(aTF,
                          aNodes,
                          aNormals,
                          aTriangles,
                          aNumPnindex,
                          aNormNum,
                          aTrianNum,
                          aPnindices);
  }
  else if (theTI->IsKind(STANDARD_TYPE(StepVisual_TriangulatedSurfaceSet)))
  {
    Handle(StepVisual_TriangulatedSurfaceSet) aTSS =
      Handle(StepVisual_TriangulatedSurfaceSet)::DownCast(theTI);
    GetSimpleFaceElements(aTSS,
                          aNodes,
                          aNormals,
                          aTriangles,
                          aNumPnindex,
                          aNormNum,
                          aTrianNum,
                          aPnindices);
  }
  else if (theTI->IsKind(STANDARD_TYPE(StepVisual_ComplexTriangulatedFace)))
  {
    Handle(StepVisual_ComplexTriangulatedFace) aTF =
      Handle(StepVisual_ComplexTriangulatedFace)::DownCast(theTI);
    GetComplexFaceElements(aTF,
                           aNodes,
                           aNormals,
                           aTriaStrips,
                           aTriaFans,
                           aNumPnindex,
                           aNormNum,
                           aTrianStripsNum,
                           aTrianFansNum,
                           aPnindices);
  }
  else if (theTI->IsKind(STANDARD_TYPE(StepVisual_ComplexTriangulatedSurfaceSet)))
  {
    Handle(StepVisual_ComplexTriangulatedSurfaceSet) aTSS =
      Handle(StepVisual_ComplexTriangulatedSurfaceSet)::DownCast(theTI);
    GetComplexFaceElements(aTSS,
                           aNodes,
                           aNormals,
                           aTriaStrips,
                           aTriaFans,
                           aNumPnindex,
                           aNormNum,
                           aTrianStripsNum,
                           aTrianFansNum,
                           aPnindices);
  }
  else
  {
    return Handle(Poly_Triangulation)();
  }

  const Standard_Boolean aHasUVNodes = Standard_False;
  const Standard_Boolean aHasNormals = (aNormNum > 0);
  const Standard_Integer aNbNodes    = (aNumPnindex > 0) ? aNumPnindex : aNodes->Length();

  if (aTrianStripsNum == 0 && aTrianFansNum == 0)
  {
    aMesh = new Poly_Triangulation(aNbNodes, aTrianNum, aHasUVNodes, aHasNormals);
  }
  else
  {
    Standard_Integer aNbTriaStrips = 0;
    Standard_Integer aNbTriaFans   = 0;

    for (Standard_Integer aTrianStripIndex = 1; aTrianStripIndex <= aTrianStripsNum;
         ++aTrianStripIndex)
    {
      Handle(TColStd_HArray1OfInteger) aTriangleStrip =
        Handle(TColStd_HArray1OfInteger)::DownCast(aTriaStrips->Value(aTrianStripIndex));
      for (Standard_Integer anIndex = 3; anIndex <= aTriangleStrip->Length(); anIndex += 2)
      {
        if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2)
            && aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
          ++aNbTriaStrips;
      }
      for (Standard_Integer anIndex = 4; anIndex <= aTriangleStrip->Length(); anIndex += 2)
      {
        if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2)
            && aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
          ++aNbTriaStrips;
      }
    }

    for (Standard_Integer aTrianFanIndex = 1; aTrianFanIndex <= aTrianFansNum; ++aTrianFanIndex)
    {
      Handle(TColStd_HArray1OfInteger) aTriangleFan =
        Handle(TColStd_HArray1OfInteger)::DownCast(aTriaFans->Value(aTrianFanIndex));
      aNbTriaFans += aTriangleFan->Length() - 2;
    }

    aMesh = new Poly_Triangulation(aNbNodes, aNbTriaStrips + aNbTriaFans, aHasUVNodes, aHasNormals);
  }

  SetNodes(aMesh, aNodes, aNumPnindex, aPnindices, theLocalFactors.LengthFactor());

  if (aHasNormals)
  {
    SetNormals(aMesh, aNormals, aNormNum, aNbNodes);
  }

  SetTriangles(aMesh, aTriangles, aTrianStripsNum, aTriaStrips, aTrianFansNum, aTriaFans);

  return aMesh;
}
} // namespace

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Empty Constructor
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace()
    : myError(StepToTopoDS_TranslateFaceOther)
{
  done = Standard_False;
}

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Constructor with a FaceSurface and a Tool
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace(const Handle(StepShape_FaceSurface)& FS,
                                                       StepToTopoDS_Tool&                   T,
                                                       StepToTopoDS_NMTool&                 NMTool,
                                                       const StepData_Factors& theLocalFactors)
{
  Init(FS, T, NMTool, theLocalFactors);
}

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Constructor with either TriangulatedFace or
//           ComplexTriangulatedFace and a Tool
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace(
  const Handle(StepVisual_TessellatedFace)& theTF,
  StepToTopoDS_Tool&                        theTool,
  StepToTopoDS_NMTool&                      theNMTool,
  const Standard_Boolean                    theReadTessellatedWhenNoBRepOnly,
  Standard_Boolean&                         theHasGeom,
  const StepData_Factors&                   theLocalFactors)
{
  Init(theTF, theTool, theNMTool, theReadTessellatedWhenNoBRepOnly, theHasGeom, theLocalFactors);
}

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Constructor with either TriangulatedSurfaceSet or
//           ComplexTriangulatedSurfaceSet and a Tool
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace(
  const Handle(StepVisual_TessellatedSurfaceSet)& theTSS,
  StepToTopoDS_Tool&                              theTool,
  StepToTopoDS_NMTool&                            theNMTool,
  const StepData_Factors&                         theLocalFactors)
{
  Init(theTSS, theTool, theNMTool, theLocalFactors);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceSurface and a Tool
// ============================================================================
static inline Standard_Boolean isReversed(const Handle(StepGeom_Surface)& theStepSurf)
{
  if (theStepSurf->IsKind(STANDARD_TYPE(StepGeom_RectangularTrimmedSurface)))
  {
    return isReversed(
      Handle(StepGeom_RectangularTrimmedSurface)::DownCast(theStepSurf)->BasisSurface());
  }
  else
  {
    Handle(StepGeom_ToroidalSurface) aStepTorSur =
      Handle(StepGeom_ToroidalSurface)::DownCast(theStepSurf);
    return !aStepTorSur.IsNull() && aStepTorSur->MajorRadius() < 0.;
  }
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceSurface and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepShape_FaceSurface)& theFaceSurface,
                                      StepToTopoDS_Tool&                   theTopoDSTool,
                                      StepToTopoDS_NMTool&                 theTopoDSToolNM,
                                      const StepData_Factors&              theLocalFactors)
{
  done = Standard_True;
  if (theTopoDSTool.IsBound(theFaceSurface))
  {
    myResult = TopoDS::Face(theTopoDSTool.Find(theFaceSurface));
    myError  = StepToTopoDS_TranslateFaceDone;
    done     = Standard_True;
    return;
  }

  // Within a context of this method this object is used for message handling only.
  Handle(Transfer_TransientProcess) aMessageHandler = theTopoDSTool.TransientProcess();

  // ----------------------------------------------
  // Map the Face Geometry and create a TopoDS_Face
  // ----------------------------------------------
  Handle(StepGeom_Surface) aStepGeomSurface = theFaceSurface->FaceGeometry();
  // sln 01.10.2001 BUC61003. If corresponding entity was read with error StepSurface may be NULL.
  // In this case we exit from function
  if (aStepGeomSurface.IsNull())
  {
    aMessageHandler->AddFail(aStepGeomSurface, " Surface has not been created");
    myError = StepToTopoDS_TranslateFaceOther;
    done    = Standard_False;
    return;
  }

  // [BEGIN] Added to process non-manifold topology (ssv; 14.11.2010)
  if (theTopoDSToolNM.IsActive() && theTopoDSToolNM.IsBound(aStepGeomSurface))
  {
    TopoDS_Shape anExistingShape = theTopoDSToolNM.Find(aStepGeomSurface);
    // Reverse shape's orientation for the next shell
    anExistingShape.Reverse();
    myResult = anExistingShape;
    myError  = StepToTopoDS_TranslateFaceDone;
    done     = Standard_True;
    return;
  }
  // [END] Added to process non-manifold topology (ssv; 14.11.2010)

  if (aStepGeomSurface->IsKind(STANDARD_TYPE(StepGeom_OffsetSurface))) //: d4 abv 12 Mar 98
  {
    aMessageHandler->AddWarning(aStepGeomSurface, " Type OffsetSurface is out of scope of AP 214");
  }

  Handle(Geom_Surface) aGeomSurface = StepToGeom::MakeSurface(aStepGeomSurface, theLocalFactors);
  if (aGeomSurface.IsNull())
  {
    aMessageHandler->AddFail(aStepGeomSurface, " Surface has not been created");
    myError = StepToTopoDS_TranslateFaceOther;
    done    = Standard_False;
    return;
  }

  // pdn to force bsplsurf to be periodic
  if (!Handle(StepGeom_BSplineSurface)::DownCast(aStepGeomSurface).IsNull())
  {
    Handle(Geom_Surface) periodicSurf = ShapeAlgo::AlgoContainer()->ConvertToPeriodic(aGeomSurface);
    if (!periodicSurf.IsNull())
    {
      aMessageHandler->AddWarning(aStepGeomSurface, "Surface forced to be periodic");
      aGeomSurface = periodicSurf;
    }
  }

  // fix for bug 0026376 Solid Works wrote face based on toroidal surface having negative major
  // radius seems that such case is interpreted  by "Solid Works" and "ProE" as face having reversed
  // orientation.
  const Standard_Boolean aSameSense =
    isReversed(aStepGeomSurface) ? !theFaceSurface->SameSense() : theFaceSurface->SameSense();

  // -- Statistics --
  theTopoDSTool.AddContinuity(aGeomSurface);

  TopoDS_Face  aResultFace;
  BRep_Builder aFaceBuilder;
  aFaceBuilder.MakeFace(aResultFace, aGeomSurface, Precision::Confusion());

  // ----------------------------------
  // Iterate on each FaceBounds (Wires)
  // ----------------------------------
  // - Simple sewing criterion (CKY, Jan97)
  // Periodic surface (typically a cylinder)
  // 2 face bounds, each with an edge loop from a single edge.
  // This edge is closed, c-a-d vertex-begin = vertex-end (for the two edges)
  // Is it sufficient (check that these are two outer-bounds... ?? How?)
  // Then we can say: face with two edges whose seam is missing
  // The seam is between the two vertex
  for (Standard_Integer aBoundIndex = 1; aBoundIndex <= theFaceSurface->NbBounds(); ++aBoundIndex)
  {
    Handle(StepShape_FaceBound) aFaceBound = theFaceSurface->BoundsValue(aBoundIndex);
    if (aFaceBound.IsNull())
    {
      continue;
    }
    Handle(StepShape_Loop) aFaceLoop = aFaceBound->Bound();
    if (aFaceLoop.IsNull())
    {
      continue;
    }

    // ------------------------
    // The Loop is a VertexLoop
    // ------------------------
    if (aFaceLoop->IsKind(STANDARD_TYPE(StepShape_VertexLoop)))
    {
      //: S4136      STF.Closed() = Standard_False;
      //  PROBLEM if SPHERE or TORE
      // It will be necessary to make a complete wire, provided that the point carries on the face
      // In the meantime, we do nothing

      // abv 10.07.00 pr1sy.stp: vertex_loop can be wrong; so just make natural bounds
      if ((aGeomSurface->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
           || aGeomSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
           || aGeomSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
          && (theFaceSurface->NbBounds() == 1))
      {
        // Modification to create natural bounds for face based on the spherical and Bspline
        // surface and having only one bound represented by Vertex loop was made.
        // According to the specification of ISO - 10303 part 42:
        // "If the face has only one bound and this is of type vertex_loop, then the interior of
        // the face is the domain of the face_surface.face_geometry. In such a case the underlying
        // surface shall be closed (e.g. a spherical_surface.)"
        // - natural bounds are applied only in case if VertexLoop is only the one defined face
        // bound.
        BRepBuilderAPI_MakeFace anAuxiliaryFaceBuilder(aGeomSurface, Precision());
        for (TopoDS_Iterator aFaceIt(anAuxiliaryFaceBuilder); aFaceIt.More(); aFaceIt.Next())
        {
          aFaceBuilder.Add(aResultFace, aFaceIt.Value());
        }
        continue;
      }

      if (aGeomSurface->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
      {
        continue;
      }

      Handle(StepShape_VertexLoop) aVertexLoop = Handle(StepShape_VertexLoop)::DownCast(aFaceLoop);
      if (aGeomSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
      {
        aMessageHandler->AddWarning(aVertexLoop, "VertexLoop on plane is ignored");
        continue; // smh : BUC60809
      }

      StepToTopoDS_TranslateVertexLoop aVertexLoopTranslator;
      aVertexLoopTranslator.SetPrecision(Precision()); // gka
      aVertexLoopTranslator.SetMaxTol(MaxTol());
      aVertexLoopTranslator.Init(aVertexLoop, theTopoDSTool, theTopoDSToolNM, theLocalFactors);
      if (aVertexLoopTranslator.IsDone())
      {
        aFaceBuilder.Add(aResultFace, aVertexLoopTranslator.Value());
      }
      else
      {
        aMessageHandler->AddWarning(aVertexLoop, " a VertexLoop not mapped to TopoDS");
      }
    }
    // ----------------------
    // The Loop is a PolyLoop
    // ----------------------
    else if (aFaceLoop->IsKind(STANDARD_TYPE(StepShape_PolyLoop)))
    {
      Handle(StepShape_PolyLoop) aPolyLoop = Handle(StepShape_PolyLoop)::DownCast(aFaceLoop);
      aResultFace.Orientation(theFaceSurface->SameSense() ? TopAbs_FORWARD : TopAbs_REVERSED);
      StepToTopoDS_TranslatePolyLoop aPolyLoopTranslator;
      aPolyLoopTranslator.SetPrecision(Precision()); // gka
      aPolyLoopTranslator.SetMaxTol(MaxTol());
      aPolyLoopTranslator.Init(aPolyLoop,
                               theTopoDSTool,
                               aGeomSurface,
                               aResultFace,
                               theLocalFactors);
      if (aPolyLoopTranslator.IsDone())
      {
        TopoDS_Wire aPolyLoopWire = TopoDS::Wire(aPolyLoopTranslator.Value());
        aPolyLoopWire.Orientation(aFaceBound->Orientation() ? TopAbs_FORWARD : TopAbs_REVERSED);
        aFaceBuilder.Add(aResultFace, aPolyLoopWire);
      }
      else
      {
        aMessageHandler->AddWarning(aPolyLoop, " a PolyLoop not mapped to TopoDS");
      }
    }
    // -----------------------
    // The Loop is an EdgeLoop
    // -----------------------
    else if (aFaceLoop->IsKind(STANDARD_TYPE(StepShape_EdgeLoop)))
    {
      StepToTopoDS_TranslateEdgeLoop anEdgeLoopTranslator;
      anEdgeLoopTranslator.SetPrecision(Precision()); // gka
      anEdgeLoopTranslator.SetMaxTol(MaxTol());
      anEdgeLoopTranslator.Init(aFaceBound,
                                aResultFace,
                                aGeomSurface,
                                aStepGeomSurface,
                                aSameSense,
                                theTopoDSTool,
                                theTopoDSToolNM,
                                theLocalFactors);

      if (anEdgeLoopTranslator.IsDone())
      {
        TopoDS_Wire anEdgeLoopWire = TopoDS::Wire(anEdgeLoopTranslator.Value());

        // STEP Face_Surface orientation :
        // if the topological orientation is opposite to the geometric
        // orientation of the surface => the underlying topological
        // orientation are not implicitly reversed
        // this is the case in CAS.CADE => If the face_surface is reversed,
        // the wire orientation has to be explicitly reversed
        if (aFaceBound->Orientation())
        {
          anEdgeLoopWire.Orientation(aSameSense ? TopAbs_FORWARD : TopAbs_REVERSED);
        }
        else
        {
          anEdgeLoopWire.Orientation(aSameSense ? TopAbs_REVERSED : TopAbs_FORWARD);
        }
        // -----------------------------
        // The Wire is added to the Face
        // -----------------------------
        aFaceBuilder.Add(aResultFace, anEdgeLoopWire);
      }
      else
      {
        // There was a problem in the mapping: We lost Face (so to speak...).
        // No way today to recover at least all the geometries (Points, 3D Curves, Surface).
        aMessageHandler->AddFail(aFaceLoop, " EdgeLoop not mapped to TopoDS");

        // CKY JAN-97: a Wire is missing, well we continue anyway
        // unless OuterBound: that's still not quite normal...
        if (aFaceBound->IsKind(STANDARD_TYPE(StepShape_FaceOuterBound)))
        {
          aMessageHandler->AddWarning(theFaceSurface, "No Outer Bound : Face not done");
        }
        continue;
      }
    }
    else
    {
      // Type not yet implemented or non sens
      aMessageHandler->AddFail(aFaceLoop, " Type of loop not yet implemented");
      continue;
    }
  }

  aResultFace.Orientation(theFaceSurface->SameSense() ? TopAbs_FORWARD : TopAbs_REVERSED);
  theTopoDSTool.Bind(theFaceSurface, aResultFace);

  // Register face in NM tool (ssv; 14.11.2010)
  if (theTopoDSToolNM.IsActive())
  {
    theTopoDSToolNM.Bind(aStepGeomSurface, aResultFace);
  }

  myResult = aResultFace;
  myError  = StepToTopoDS_TranslateFaceDone;
  done     = Standard_True;
}

// ============================================================================
// Method  : Init
// Purpose : Init with either StepVisual_TriangulatedFace or
//           StepVisual_ComplexTriangulatedFace and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepVisual_TessellatedFace)& theTF,
                                      StepToTopoDS_Tool&                        theTool,
                                      StepToTopoDS_NMTool&                      theNMTool,
                                      const Standard_Boolean  theReadTessellatedWhenNoBRepOnly,
                                      Standard_Boolean&       theHasGeom,
                                      const StepData_Factors& theLocalFactors)
{
  if (theTF.IsNull())
    return;

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();

  BRep_Builder aB;
  TopoDS_Face  aF;

  if (theTF->HasGeometricLink())
  {
    Handle(TransferBRep_ShapeBinder) aBinder =
      Handle(TransferBRep_ShapeBinder)::DownCast(aTP->Find(theTF->GeometricLink().Face()));
    if (aBinder)
    {
      aF         = aBinder->Face();
      theHasGeom = Standard_True;
    }
    if (!aF.IsNull() && !BRep_Tool::Surface(aF).IsNull() && theReadTessellatedWhenNoBRepOnly)
    {
      // Face has BRep but OnNoBRep param is specified
      return;
    }
  }

  if (aF.IsNull())
  {
    aB.MakeFace(aF);
    theHasGeom = Standard_False;
  }

  Handle(Poly_Triangulation) aMesh;
  if (DeclareAndCast(StepVisual_TriangulatedFace, aTriaF, theTF))
  {
    aMesh = createMesh(aTriaF, theLocalFactors);
  }
  else if (DeclareAndCast(StepVisual_ComplexTriangulatedFace, aCompTriaF, theTF))
  {
    aMesh = createMesh(aCompTriaF, theLocalFactors);
  }
  else
  {
    aTP->AddWarning(theTF, " Triangulated or ComplexTriangulated entity is supported only.");
    return;
  }

  if (aMesh.IsNull())
  {
    aTP->AddWarning(theTF, " Poly triangulation is not set to TopoDS face.");
    return;
  }

  aB.UpdateFace(aF, aMesh);

  if (theNMTool.IsActive())
    theNMTool.Bind(theTF, aF);

  aTP->Bind(theTF, new TransferBRep_ShapeBinder(aF));

  myResult = aF;
  myError  = StepToTopoDS_TranslateFaceDone;
  done     = Standard_True;
}

// ============================================================================
// Method  : Init
// Purpose : Init with either StepVisual_TriangulatedSurfaceSet or
//           StepVisual_ComplexTriangulatedSurfaceSet and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepVisual_TessellatedSurfaceSet)& theTSS,
                                      StepToTopoDS_Tool&                              theTool,
                                      StepToTopoDS_NMTool&                            theNMTool,
                                      const StepData_Factors& theLocalFactors)
{
  if (theTSS.IsNull())
    return;

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();
  BRep_Builder                      aB;
  TopoDS_Face                       aF;
  aB.MakeFace(aF);

  Handle(Poly_Triangulation) aMesh;
  if (DeclareAndCast(StepVisual_TriangulatedSurfaceSet, aTriaSS, theTSS))
  {
    aMesh = createMesh(aTriaSS, theLocalFactors);
  }
  else if (DeclareAndCast(StepVisual_ComplexTriangulatedSurfaceSet, aCompTriaSS, theTSS))
  {
    aMesh = createMesh(aCompTriaSS, theLocalFactors);
  }
  else
  {
    aTP->AddWarning(theTSS, " Triangulated or ComplexTriangulated entity is supported only.");
    return;
  }
  if (aMesh.IsNull())
  {
    aTP->AddWarning(theTSS, " Poly triangulation is not set to TopoDS face.");
    return;
  }
  aB.UpdateFace(aF, aMesh);
  if (theNMTool.IsActive())
    theNMTool.Bind(theTSS, aF);

  myResult = aF;
  myError  = StepToTopoDS_TranslateFaceDone;
  done     = Standard_True;
}

// ============================================================================
// Method  : createMesh
// Purpose : creates a Poly_Triangulation from simple/complex
//           TriangulatedFace or TriangulatedSurfaceSet
// ============================================================================
Handle(Poly_Triangulation) StepToTopoDS_TranslateFace::createMesh(
  const Handle(StepVisual_TessellatedItem)& theTI,
  const StepData_Factors&                   theLocalFactors) const
{
  return CreatePolyTriangulation(theTI, theLocalFactors);
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================
const TopoDS_Shape& StepToTopoDS_TranslateFace::Value() const
{
  StdFail_NotDone_Raise_if(!done, "StepToTopoDS_TranslateFace::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateFace error
// ============================================================================
StepToTopoDS_TranslateFaceError StepToTopoDS_TranslateFace::Error() const
{
  return myError;
}
