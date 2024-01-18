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
//:p4 abv, pdn 23.02.99: PRO9234 #15720: call BRepTools::Update() for faces
//:q7 abv 23.03.99: bm4_al_eye.stp #53710: remove pseudo-seams
//    rln 31.03.99 S4135: prohibit fixing intersection of non-adjacent edges (temporarily)
//#4  szv          S4163: optimization
//%19 pdn 17.04.99 using ShapeFix_Wire::FixEdgeCurves instead of ShapeFix_PCurves
//    smh 31.01.01 Bad data in file : case of vertex loop on plane face
// sln 01.10.2001 BUC61003. StepToTopoDS_TranslateFace::Init function is corrected (verifying  Handle(...).IsNull() is added)

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

//#3 rln 16/02/98
//#include <GeomAdaptor_Curve.hxx>
//#include <GeomAdaptor_CurveOnSurface.hxx>
//#3 rln 16/02/98
//rln 28/01/98
//rln 28/01/98
//  Provisoire, pour VertexLoop
//:d4
// To proceed with I-DEAS-like STP (ssv; 15.11.2010)
//#define DEBUG

namespace {
  // ============================================================================
  // Method  : SetNodes
  // Purpose : Set nodes to the triangulation from an array
  // ============================================================================
  static void SetNodes(const Handle(Poly_Triangulation)& theMesh,
                       Handle(TColgp_HArray1OfXYZ)& theNodes,
                       Handle(TColStd_HArray1OfInteger)& thePnindices,
                       const Standard_Real theLengthFactor)
  {
    for (Standard_Integer aPnIndex = 1; aPnIndex <= theMesh->NbNodes(); ++aPnIndex)
    {
      const gp_XYZ& aPoint = theNodes->Value(thePnindices->Value(aPnIndex));
      theMesh->SetNode(aPnIndex, theLengthFactor * aPoint);
    }
  }

  // ============================================================================
  // Method  : SetNormals
  // Purpose : Set normals to the triangulation from an array
  // ============================================================================
  static void SetNormals(const Handle(Poly_Triangulation)& theMesh,
                         const Handle(TColStd_HArray2OfReal)& theNormals,
                         const Standard_Integer theNormNum,
                         const Standard_Integer theNumPnindex)
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
  static void SetTriangles(const Handle(Poly_Triangulation)& theMesh,
                           const Handle(TColStd_HArray2OfInteger) theTriangles,
                           const Standard_Integer theTrianStripsNum,
                           const Handle(TColStd_HArray1OfTransient)& theTrianStrips,
                           const Standard_Integer theTrianFansNum,
                           const Handle(TColStd_HArray1OfTransient)& theTrianFans)
  {
    if (theTrianStripsNum == 0 && theTrianFansNum == 0)
    {
      for (Standard_Integer aTrianIndex = 1; aTrianIndex <= theMesh->NbTriangles(); ++aTrianIndex)
      {
        theMesh->SetTriangle(aTrianIndex, Poly_Triangle(theTriangles->Value(aTrianIndex, 1),
                                                        theTriangles->Value(aTrianIndex, 2),
                                                        theTriangles->Value(aTrianIndex, 3)));
      }
    }
    else
    {
      Standard_Integer aTriangleIndex = 1;
      for (Standard_Integer aTrianStripIndex = 1; aTrianStripIndex <= theTrianStripsNum; ++aTrianStripIndex)
      {
        Handle(TColStd_HArray1OfInteger) aTriangleStrip = Handle(TColStd_HArray1OfInteger)::DownCast(theTrianStrips->Value(aTrianStripIndex));
        for (Standard_Integer anIndex = 3; anIndex <= aTriangleStrip->Length(); anIndex += 2)
        {
          if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2) &&
              aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
          {
            theMesh->SetTriangle(aTriangleIndex++, Poly_Triangle(aTriangleStrip->Value(anIndex - 2),
                                                                 aTriangleStrip->Value(anIndex),
                                                                 aTriangleStrip->Value(anIndex - 1)));
          }
        }
        for (Standard_Integer anIndex = 4; anIndex <= aTriangleStrip->Length(); anIndex += 2)
        {
          if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2) &&
              aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
          {
            theMesh->SetTriangle(aTriangleIndex++, Poly_Triangle(aTriangleStrip->Value(anIndex - 2),
                                                                 aTriangleStrip->Value(anIndex - 1),
                                                                 aTriangleStrip->Value(anIndex)));
          }
        }
      }
      for (Standard_Integer aTrianFanIndex = 1; aTrianFanIndex <= theTrianFansNum; ++aTrianFanIndex)
      {
        Handle(TColStd_HArray1OfInteger) aTriangleFan = Handle(TColStd_HArray1OfInteger)::DownCast(theTrianFans->Value(aTrianFanIndex));
        for (Standard_Integer anIndex = 3; anIndex <= aTriangleFan->Length(); ++anIndex)
        {
          if (aTriangleFan->Value(anIndex) != aTriangleFan->Value(anIndex - 2) &&
              aTriangleFan->Value(anIndex - 1) != aTriangleFan->Value(anIndex - 2))
          {
            theMesh->SetTriangle(aTriangleIndex++, Poly_Triangle(aTriangleFan->Value(1),
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
  template<class Type>
  static void GetSimpleFaceElements(Type theFace,
                                    Handle(TColgp_HArray1OfXYZ)& theNodes,
                                    Handle(TColStd_HArray2OfReal)& theNormals,
                                    Handle(TColStd_HArray2OfInteger)& theTriangles,
                                    Standard_Integer& thePnIndNb,
                                    Standard_Integer& theNormNb,
                                    Standard_Integer& theTriNb,
                                    Handle(TColStd_HArray1OfInteger)& thePnindices)
  {
    theNodes = theFace->Coordinates()->Points();
    theNormals = theFace->Normals();
    theTriangles = theFace->Triangles();
    thePnIndNb = theFace->NbPnindex();
    theNormNb = theFace->NbNormals();
    theTriNb = theFace->NbTriangles();
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
  template<class Type>
  static void GetComplexFaceElements(Type theFace,
                                     Handle(TColgp_HArray1OfXYZ)& theNodes,
                                     Handle(TColStd_HArray2OfReal)& theNormals,
                                     Handle(TColStd_HArray1OfTransient)& theTriangleStrips,
                                     Handle(TColStd_HArray1OfTransient)& theTriangleFans,
                                     Standard_Integer& thePnIndNb,
                                     Standard_Integer& theNormNb,
                                     Standard_Integer& theTriStripsNb,
                                     Standard_Integer& theTriFansNb,
                                     Handle(TColStd_HArray1OfInteger)& thePnindices)
  {
    theNodes = theFace->Coordinates()->Points();
    theNormals = theFace->Normals();
    theTriangleStrips = theFace->TriangleStrips();
    theTriangleFans = theFace->TriangleFans();
    thePnIndNb = theFace->NbPnindex();
    theNormNb = theFace->NbNormals();
    theTriStripsNb = theFace->NbTriangleStrips();
    theTriFansNb = theFace->NbTriangleFans();
    thePnindices = new TColStd_HArray1OfInteger(1, thePnIndNb);
    for (Standard_Integer anIndx = 1; anIndx <= thePnIndNb; ++anIndx)
    {
      thePnindices->SetValue(anIndx, theFace->PnindexValue(anIndx));
    }
  }

  // ============================================================================
  // Method  : CreatePolyTriangulation
  // Purpose : Create PolyTriangulation
  // ============================================================================
  static Handle(Poly_Triangulation) CreatePolyTriangulation(const Handle(StepVisual_TessellatedItem)& theTI,
                                                            const StepData_Factors& theLocalFactors)
  {
    Handle(Poly_Triangulation) aMesh;
    if (theTI.IsNull())
    {
      return Handle(Poly_Triangulation)();
    }

    Handle(TColgp_HArray1OfXYZ) aNodes;
    Handle(TColStd_HArray2OfReal) aNormals;
    Handle(TColStd_HArray2OfInteger) aTriangles;
    Standard_Integer aNumPnindex = 0;
    Standard_Integer aNormNum = 0;
    Standard_Integer aTrianNum = 0;
    Handle(TColStd_HArray1OfInteger) aPnindices;

    Handle(TColStd_HArray1OfTransient) aTriaStrips;
    Handle(TColStd_HArray1OfTransient) aTriaFans;
    Standard_Integer aTrianStripsNum = 0;
    Standard_Integer aTrianFansNum = 0;

    if (theTI->IsKind(STANDARD_TYPE(StepVisual_TriangulatedFace)))
    {
      Handle(StepVisual_TriangulatedFace) aTF = Handle(StepVisual_TriangulatedFace)::DownCast(theTI);
      GetSimpleFaceElements(aTF, aNodes, aNormals, aTriangles, aNumPnindex, aNormNum, aTrianNum, aPnindices);
    }
    else if (theTI->IsKind(STANDARD_TYPE(StepVisual_TriangulatedSurfaceSet)))
    {
      Handle(StepVisual_TriangulatedSurfaceSet) aTSS = Handle(StepVisual_TriangulatedSurfaceSet)::DownCast(theTI);
      GetSimpleFaceElements(aTSS, aNodes, aNormals, aTriangles, aNumPnindex, aNormNum, aTrianNum, aPnindices);
    }
    else if (theTI->IsKind(STANDARD_TYPE(StepVisual_ComplexTriangulatedFace)))
    {
      Handle(StepVisual_ComplexTriangulatedFace) aTF = Handle(StepVisual_ComplexTriangulatedFace)::DownCast(theTI);
      GetComplexFaceElements(aTF, aNodes, aNormals, aTriaStrips, aTriaFans, aNumPnindex, aNormNum, aTrianStripsNum, aTrianFansNum, aPnindices);
    }
    else if (theTI->IsKind(STANDARD_TYPE(StepVisual_ComplexTriangulatedSurfaceSet)))
    {
      Handle(StepVisual_ComplexTriangulatedSurfaceSet) aTSS = Handle(StepVisual_ComplexTriangulatedSurfaceSet)::DownCast(theTI);
      GetComplexFaceElements(aTSS, aNodes, aNormals, aTriaStrips, aTriaFans, aNumPnindex, aNormNum, aTrianStripsNum, aTrianFansNum, aPnindices);
    }
    else
    {
      return Handle(Poly_Triangulation)();
    }

    const Standard_Boolean aHasUVNodes = Standard_False;
    const Standard_Boolean aHasNormals = (aNormNum > 0);

    if (aTrianStripsNum == 0 && aTrianFansNum == 0)
    {
      aMesh = new Poly_Triangulation(aNumPnindex, aTrianNum, aHasUVNodes, aHasNormals);
    }
    else
    {
      Standard_Integer aNbTriaStrips = 0;
      Standard_Integer aNbTriaFans = 0;

      for (Standard_Integer aTrianStripIndex = 1; aTrianStripIndex <= aTrianStripsNum; ++aTrianStripIndex)
      {
        Handle(TColStd_HArray1OfInteger) aTriangleStrip = Handle(TColStd_HArray1OfInteger)::DownCast(aTriaStrips->Value(aTrianStripIndex));
        for (Standard_Integer anIndex = 3; anIndex <= aTriangleStrip->Length(); anIndex += 2)
        {
          if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2) &&
              aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
            ++aNbTriaStrips;
        }
        for (Standard_Integer anIndex = 4; anIndex <= aTriangleStrip->Length(); anIndex += 2)
        {
          if (aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 2) &&
              aTriangleStrip->Value(anIndex) != aTriangleStrip->Value(anIndex - 1))
            ++aNbTriaStrips;
        }
      }

      for (Standard_Integer aTrianFanIndex = 1; aTrianFanIndex <= aTrianFansNum; ++aTrianFanIndex)
      {
        Handle(TColStd_HArray1OfInteger) aTriangleFan = Handle(TColStd_HArray1OfInteger)::DownCast(aTriaFans->Value(aTrianFanIndex));
        aNbTriaFans += aTriangleFan->Length() - 2;
      }

      aMesh = new Poly_Triangulation(aNumPnindex, aNbTriaStrips + aNbTriaFans, aHasUVNodes, aHasNormals);
    }

    SetNodes(aMesh, aNodes, aPnindices, theLocalFactors.LengthFactor());

    if (aHasNormals)
    {
      SetNormals(aMesh, aNormals, aNormNum, aNumPnindex);
    }

    SetTriangles(aMesh, aTriangles, aTrianStripsNum, aTriaStrips, aTrianFansNum, aTriaFans);

    return aMesh;
  }
}

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
                                                       StepToTopoDS_Tool& T,
                                                       StepToTopoDS_NMTool& NMTool,
                                                       const StepData_Factors& theLocalFactors)
{
  Init(FS, T, NMTool, theLocalFactors);
}

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Constructor with either TriangulatedFace or 
//           ComplexTriangulatedFace and a Tool
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace(const Handle(StepVisual_TessellatedFace)& theTF,
                                                       StepToTopoDS_Tool& theTool,
                                                       StepToTopoDS_NMTool& theNMTool,
                                                       const Standard_Boolean theReadTessellatedWhenNoBRepOnly,
                                                       Standard_Boolean& theHasGeom,
                                                       const StepData_Factors& theLocalFactors)
{
  Init(theTF, theTool, theNMTool, theReadTessellatedWhenNoBRepOnly, theHasGeom, theLocalFactors);
}

// ============================================================================
// Method  : StepToTopoDS_TranslateFace
// Purpose : Constructor with either TriangulatedSurfaceSet or 
//           ComplexTriangulatedSurfaceSet and a Tool
// ============================================================================
StepToTopoDS_TranslateFace::StepToTopoDS_TranslateFace(const Handle(StepVisual_TessellatedSurfaceSet)& theTSS,
                                                       StepToTopoDS_Tool& theTool,
                                                       StepToTopoDS_NMTool& theNMTool,
                                                       const StepData_Factors& theLocalFactors)
{
  Init(theTSS, theTool, theNMTool, theLocalFactors);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceSurface and a Tool
// ============================================================================
static inline Standard_Boolean isReversed(const Handle(StepGeom_Surface)& theStepSurf)
{
  Handle(StepGeom_ToroidalSurface) aStepTorSur;
  if(theStepSurf->IsKind(STANDARD_TYPE(StepGeom_RectangularTrimmedSurface)))
    return isReversed(Handle(StepGeom_RectangularTrimmedSurface)::DownCast(theStepSurf)->BasisSurface());
  
  else
    aStepTorSur = Handle(StepGeom_ToroidalSurface)::DownCast(theStepSurf);
  
  return (!aStepTorSur.IsNull() && aStepTorSur->MajorRadius() < 0 ? Standard_True : Standard_False);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a FaceSurface and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepShape_FaceSurface)& FS,
                                      StepToTopoDS_Tool& aTool,
                                      StepToTopoDS_NMTool& NMTool,
                                      const StepData_Factors& theLocalFactors)
{
  done = Standard_True;
  if (aTool.IsBound(FS)) {
    myResult = TopoDS::Face(aTool.Find(FS));
    myError = StepToTopoDS_TranslateFaceDone;
    done = Standard_True;
    return;
  }
  
  Handle(Transfer_TransientProcess) TP = aTool.TransientProcess();
  
  // ----------------------------------------------
  // Map the Face Geometry and create a TopoDS_Face
  // ----------------------------------------------
  Handle(StepGeom_Surface) StepSurf = FS->FaceGeometry();

   // sln 01.10.2001 BUC61003. If corresponding entity was read with error StepSurface may be NULL. In this case we exit from function
  if ( StepSurf.IsNull() ) {
    TP->AddFail(StepSurf," Surface has not been created");
    myError = StepToTopoDS_TranslateFaceOther;
    done = Standard_False;
    return;
  }

  // [BEGIN] Added to process non-manifold topology (ssv; 14.11.2010)
  if ( NMTool.IsActive() && NMTool.IsBound(StepSurf) ) {
    TopoDS_Shape existingShape = NMTool.Find(StepSurf);
    // Reverse shape's orientation for the next shell
    existingShape.Reverse();
    myResult = existingShape;
    myError  = StepToTopoDS_TranslateFaceDone;
    done = Standard_True;
    return;
  }
  // [END] Added to process non-manifold topology (ssv; 14.11.2010)

  if (StepSurf->IsKind(STANDARD_TYPE(StepGeom_OffsetSurface))) //:d4 abv 12 Mar 98
    TP->AddWarning(StepSurf," Type OffsetSurface is out of scope of AP 214");
  Handle(Geom_Surface) GeomSurf = StepToGeom::MakeSurface (StepSurf, theLocalFactors);
  if (GeomSurf.IsNull())
  {
    TP->AddFail(StepSurf," Surface has not been created");
    myError = StepToTopoDS_TranslateFaceOther;
    done = Standard_False;
    return;
  }
  // pdn to force bsplsurf to be periodic
  Handle(StepGeom_BSplineSurface) sgbss = Handle(StepGeom_BSplineSurface)::DownCast(StepSurf);
  if (!sgbss.IsNull()) {
    Handle(Geom_Surface) periodicSurf = ShapeAlgo::AlgoContainer()->ConvertToPeriodic(GeomSurf);
    if (!periodicSurf.IsNull()) {
      TP->AddWarning(StepSurf, "Surface forced to be periodic");
      GeomSurf = periodicSurf;
    }
  }
    
  Standard_Boolean sameSenseFace = FS->SameSense();

  //fix for bug 0026376 Solid Works wrote face based on toroidal surface having negative major radius
  //seems that such case is interpreted  by "Solid Works" and "ProE" as face having reversed orientation.
  Standard_Boolean sameSense = (isReversed(StepSurf) ? !sameSenseFace : sameSenseFace);
  
  // -- Statistics --
  aTool.AddContinuity (GeomSurf);
  
  TopoDS_Face   F;
  BRep_Builder B;
  B.MakeFace ( F, GeomSurf, Precision::Confusion() );
  
  // ----------------------------------
  // Iterate on each FaceBounds (Wires)
  // ----------------------------------
  Handle(StepShape_FaceBound) FaceBound;
  Handle(StepShape_Loop)      Loop;
  
  StepToTopoDS_TranslateVertexLoop myTranVL;
  StepToTopoDS_TranslatePolyLoop   myTranPL;
  StepToTopoDS_TranslateEdgeLoop   myTranEdgeLoop;
  
  Standard_Integer NbBnd = FS->NbBounds();

  // --  Critere de couture simple (CKY, JAN97)
  // surface periodique (typiquement un cylindre)
  // 2 face bounds, chacun avec un edge loop d une seule edge
  //  cette edge est fermee, c-a-d vtx-deb = vtx-fin (pour les deux edges)
  // est-ce suffisant (verifier que ce sont deux outer-bounds ... ?? comment ?)
  // Alors on peut dire : face a deux bords dont la couture manque
  // La couture est entre les deux vertex

  for (Standard_Integer i = 1; i <= NbBnd; i ++) {

#ifdef OCCT_DEBUG
    std::cout << "    Processing Wire : " << i << std::endl;
#endif    
    FaceBound = FS->BoundsValue(i);
    Loop      = FaceBound->Bound();
    if (Loop.IsNull())
    {
      continue;
    }
    // ------------------------
    // The Loop is a VertexLoop
    // ------------------------
    
    if (Loop->IsKind(STANDARD_TYPE(StepShape_VertexLoop))) {
//:S4136      STF.Closed() = Standard_False;
//  PROBLEME si SPHERE ou TORE
//  Il faudra faire un wire complet, a condition que le point porte sur la face
//  En attendant, on ne fait rien
      Handle(StepShape_VertexLoop) VL = Handle(StepShape_VertexLoop)::DownCast(Loop);

      // abv 10.07.00 pr1sy.stp: vertex_loop can be wrong; so just make natural bounds
      if (GeomSurf->IsKind (STANDARD_TYPE(Geom_SphericalSurface)) ||
          GeomSurf->IsKind (STANDARD_TYPE(Geom_BSplineSurface)) || 
          GeomSurf->IsKind (STANDARD_TYPE(Geom_SurfaceOfRevolution)))
      {

        //  Modification to create natural bounds for face based on the spherical and Bspline surface and having only one bound represented by Vertex loop was made.
        //  According to the specification of ISO - 10303 part 42:
        //  "If the face has only one bound and this is of type vertex_loop, then the interior of the face is the domain of the face_surface.face_geometry.
        //   In such a case the underlying surface shall be closed (e.g. a spherical_surface.)"
        //  - natural bounds are applied only in case if VertexLoop is only the one  defined face bound.
        if (NbBnd == 1)
        {
          BRepBuilderAPI_MakeFace mf(GeomSurf, Precision());
          for (TopoDS_Iterator it(mf); it.More(); it.Next())
          {
            B.Add(F, it.Value());
          }
          continue;
        }
      }

    if (GeomSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
      continue;
    }
    if (GeomSurf->IsKind(STANDARD_TYPE(Geom_Plane))) {
      TP->AddWarning(VL, "VertexLoop on plane is ignored");
      continue; //smh : BUC60809
    }
    myTranVL.SetPrecision(Precision());//gka
    myTranVL.SetMaxTol(MaxTol());
    myTranVL.Init(VL, aTool, NMTool, theLocalFactors);
    if (myTranVL.IsDone()) {
      B.Add(F, myTranVL.Value());
    }
    else {
      TP->AddWarning(VL, " a VertexLoop not mapped to TopoDS");
    }
  }
    
  // ----------------------
  // The Loop is a PolyLoop
  // ----------------------
    else if (Loop->IsKind(STANDARD_TYPE(StepShape_PolyLoop))) {
//:S4136      STF.Closed() = Standard_False;
      Handle(StepShape_PolyLoop) PL = Handle(StepShape_PolyLoop)::DownCast(Loop);
      F.Orientation ( FS->SameSense() ? TopAbs_FORWARD : TopAbs_REVERSED);
      myTranPL.SetPrecision(Precision()); //gka
      myTranPL.SetMaxTol(MaxTol());
      myTranPL.Init(PL, aTool, GeomSurf, F, theLocalFactors);
      if (myTranPL.IsDone()) {
        TopoDS_Wire W = TopoDS::Wire(myTranPL.Value());
        W.Orientation(FaceBound->Orientation() ? TopAbs_FORWARD : TopAbs_REVERSED);
        B.Add(F, W);
      }
      else {
        TP->AddWarning(PL, " a PolyLoop not mapped to TopoDS");
      }
    }
    
    // -----------------------
    // The Loop is an EdgeLoop
    // -----------------------
  else if (Loop->IsKind(STANDARD_TYPE(StepShape_EdgeLoop))) {
    //:S4136      if (STF.Closed()) {
    //:S4136	Handle(StepShape_EdgeLoop) EL = 
    //:S4136	  Handle(StepShape_EdgeLoop)::DownCast(FaceBound->Bound());
    //:S4136	if (EL->NbEdgeList() != 1) STF.Closed() = Standard_False;
    //:S4136      }

    TopoDS_Wire   W;
    myTranEdgeLoop.SetPrecision(Precision());  //gka
    myTranEdgeLoop.SetMaxTol(MaxTol());
    myTranEdgeLoop.Init(FaceBound, F, GeomSurf, StepSurf, sameSense, aTool, NMTool, theLocalFactors);

    if (myTranEdgeLoop.IsDone()) {
      W = TopoDS::Wire(myTranEdgeLoop.Value());

      // STEP Face_Surface orientation :
      // if the topological orientation is opposite to the geometric
      // orientation of the surface => the underlying topological 
      // orientation are not implicitly reversed
      // this is the case in CAS.CADE => If the face_surface is reversed,
      // the wire orientation has to be explicitly reversed
      if (FaceBound->Orientation()) {
        // *DTH*	  if (sameSense || GeomSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
        W.Orientation(sameSense ? TopAbs_FORWARD : TopAbs_REVERSED);
      }
      else {
        // *DTH*	  if (sameSense || GeomSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
        W.Orientation(sameSense ? TopAbs_REVERSED : TopAbs_FORWARD);
      }
      // -----------------------------
      // The Wire is added to the Face      
      // -----------------------------
      B.Add(F, W);
    }
    else {
      // Il y a eu un probleme dans le mapping : On perd la Face
      // (facon de parler ...) Pas de moyen aujourd hui de recuperer
      // au moins toutes les geometries (Points, Courbes 3D, Surface)
      TP->AddFail(Loop, " EdgeLoop not mapped to TopoDS");

      // CKY JAN-97 : un Wire manque, eh bien on continue quand meme !!
      //  sauf si OuterBound : la c est quand meme pas bien normal ...
      if (FaceBound->IsKind(STANDARD_TYPE(StepShape_FaceOuterBound))) {
        TP->AddWarning(FS, "No Outer Bound : Face not done");
      }
      continue;
    }
    }    
    else { 
      // Type not yet implemented or non sens
      TP->AddFail(Loop," Type of loop not yet implemented");
#ifdef OCCT_DEBUG
      std::cout << Loop->DynamicType() << std::endl;
#endif
      continue;
    }
  }

  F.Orientation ( FS->SameSense() ? TopAbs_FORWARD : TopAbs_REVERSED);
  aTool.Bind(FS,F);

  // Register face in NM tool (ssv; 14.11.2010)
  if ( NMTool.IsActive() )
    NMTool.Bind(StepSurf, F);

  myResult = F;
  myError  = StepToTopoDS_TranslateFaceDone;
  done     = Standard_True;
}

// ============================================================================
// Method  : Init
// Purpose : Init with either StepVisual_TriangulatedFace or 
//           StepVisual_ComplexTriangulatedFace and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepVisual_TessellatedFace)& theTF,
                                      StepToTopoDS_Tool& theTool,
                                      StepToTopoDS_NMTool& theNMTool,
                                      const Standard_Boolean theReadTessellatedWhenNoBRepOnly,
                                      Standard_Boolean& theHasGeom,
                                      const StepData_Factors& theLocalFactors)
{
  if (theTF.IsNull())
    return;

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();

  BRep_Builder aB;
  TopoDS_Face aF;

  if (theTF->HasGeometricLink()) 
  {
    Handle(TransferBRep_ShapeBinder) aBinder
      = Handle(TransferBRep_ShapeBinder)::DownCast(aTP->Find(theTF->GeometricLink().Face()));
    if (aBinder) 
    {
      aF = aBinder->Face();
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

  myResult = aF;
  myError = StepToTopoDS_TranslateFaceDone;
  done = Standard_True;
}

// ============================================================================
// Method  : Init
// Purpose : Init with either StepVisual_TriangulatedSurfaceSet or
//           StepVisual_ComplexTriangulatedSurfaceSet and a Tool
// ============================================================================
void StepToTopoDS_TranslateFace::Init(const Handle(StepVisual_TessellatedSurfaceSet)& theTSS,
                                      StepToTopoDS_Tool& theTool,
                                      StepToTopoDS_NMTool& theNMTool,
                                      const StepData_Factors& theLocalFactors)
{
  if (theTSS.IsNull())
    return;

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();
  BRep_Builder aB;
  TopoDS_Face aF;
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
  myError = StepToTopoDS_TranslateFaceDone;
  done = Standard_True;
}

// ============================================================================
// Method  : createMesh 
// Purpose : creates a Poly_Triangulation from simple/complex
//           TriangulatedFace or TriangulatedSurfaceSet
// ============================================================================
Handle(Poly_Triangulation)
  StepToTopoDS_TranslateFace::createMesh(const Handle(StepVisual_TessellatedItem)& theTI,
                                         const StepData_Factors& theLocalFactors) const
{
  return CreatePolyTriangulation(theTI, theLocalFactors);
}

// ============================================================================
// Method  : Value 
// Purpose : Return the mapped Shape
// ============================================================================
const TopoDS_Shape& StepToTopoDS_TranslateFace::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_TranslateFace::Value() - no result");
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
