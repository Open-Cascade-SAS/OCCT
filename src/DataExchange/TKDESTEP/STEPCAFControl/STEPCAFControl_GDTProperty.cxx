// Created on: 2015-09-10
// Created by: Irina Krylova
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <STEPCAFControl_GDTProperty.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>

namespace
{
//=======================================================================
// function : GenerateCoordinateList
// purpose  : Generates a coordinate_list by filling it with coordinates
//           of the nodes of theTriangulation. Each node will be
//           transformed with theTransformation.
//=======================================================================
occ::handle<StepVisual_CoordinatesList> GenerateCoordinateList(
  const occ::handle<Poly_Triangulation>& theTriangulation,
  const gp_Trsf&                    theTransformation)
{
  occ::handle<NCollection_HArray1<gp_XYZ>> thePoints = new NCollection_HArray1<gp_XYZ>(1, theTriangulation->NbNodes());
  for (int aNodeIndex = 1; aNodeIndex <= theTriangulation->NbNodes(); ++aNodeIndex)
  {
    const gp_Pnt aCurrentNode = theTriangulation->Node(aNodeIndex).Transformed(theTransformation);
    thePoints->SetValue(aNodeIndex, aCurrentNode.XYZ());
  }
  occ::handle<StepVisual_CoordinatesList> aCoordinatesList = new StepVisual_CoordinatesList;
  aCoordinatesList->Init(new TCollection_HAsciiString(), thePoints);
  return aCoordinatesList;
}

//=======================================================================
// function : CountNormals
// purpose  : Returns a number of normals that theTriangulation contains
//           for the purpose of generating
//           StepVisual_ComplexTriangulatedSurfaceSet.
//           Possible outputs are:
//           0, if theTriangulation has no normals.
//           1, if all normals contained in theTriangulation are equal.
//             Note that Poly_Triangulation supports only 2 options:
//             either no normals or a normal associated with each node.
//             So when source complex_triangulated_surface_set has just
//             one normal, it will be just associated with every node in
//             Poly_Triangulation. Return value of one indicates that
//             that's what probably happen during reading.
//           theTriangulation->NbNodes(), if each vertex has a unique
//             node associated with it.
//=======================================================================
int CountNormals(const occ::handle<Poly_Triangulation>& theTriangulation)
{
  if (!theTriangulation->HasNormals())
  {
    return 0;
  }

  // Function to compare normal coordinates values.
  auto isEqual = [](const double theVal1, const double theVal2) {
    return std::abs(theVal1 - theVal2) < Precision::Confusion();
  };
  // Checking if all normals are equal.
  const gp_Dir aReferenceNormal = theTriangulation->Normal(1);
  for (int aNodeIndex = 1; aNodeIndex <= theTriangulation->NbNodes(); ++aNodeIndex)
  {
    const gp_Dir aCurrentNormal = theTriangulation->Normal(aNodeIndex);
    if (!isEqual(aReferenceNormal.X(), aCurrentNormal.X())
        || !isEqual(aReferenceNormal.Y(), aCurrentNormal.Y())
        || !isEqual(aReferenceNormal.Z(), aCurrentNormal.Z()))
    {
      return theTriangulation->NbNodes();
    }
  }

  // All normals were equal, so we can use just one normal.
  return 1;
}

//=======================================================================
// function : GenerateNormalsArray
// purpose  : Generates array of normals from theTriangulation. Normals
//           will be transformed with theTransformation.
//           IMPORTANT: Output will be nullptr if theTriangulation has
//           no normals.
//=======================================================================
occ::handle<NCollection_HArray2<double>> GenerateNormalsArray(
  const occ::handle<Poly_Triangulation>& theTriangulation,
  const gp_Trsf&                    theTransformation)
{
  const int aNormalCount = CountNormals(theTriangulation);
  if (aNormalCount == 0)
  {
    return nullptr;
  }
  else if (aNormalCount == 1)
  {
    occ::handle<NCollection_HArray2<double>> aNormals = new NCollection_HArray2<double>(1, 1, 1, 3);
    const gp_Dir aNormal = theTriangulation->Normal(1).Transformed(theTransformation);
    aNormals->SetValue(1, 1, aNormal.X());
    aNormals->SetValue(1, 2, aNormal.Y());
    aNormals->SetValue(1, 3, aNormal.Z());
    return aNormals;
  }
  else
  {
    occ::handle<NCollection_HArray2<double>> aNormals =
      new NCollection_HArray2<double>(1, theTriangulation->NbNodes(), 1, 3);

    for (int aNodeIndex = 1; aNodeIndex <= theTriangulation->NbNodes(); ++aNodeIndex)
    {
      const gp_Dir aCurrentNormal =
        theTriangulation->Normal(aNodeIndex).Transformed(theTransformation);
      aNormals->SetValue(aNodeIndex, 1, aCurrentNormal.X());
      aNormals->SetValue(aNodeIndex, 2, aCurrentNormal.Y());
      aNormals->SetValue(aNodeIndex, 3, aCurrentNormal.Z());
    }
    return aNormals;
  }
}

//=======================================================================
// function : GenerateTriangleStrips
// purpose  : Generates an array of triangle strips from theTriangulation.
//           Since Poly_Triangulation doesn't support triangle strips,
//           all triangles from it would just be imported as tringle
//           strips of one triangle.
//=======================================================================
occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> GenerateTriangleStrips(
  const occ::handle<Poly_Triangulation>& theTriangulation)
{
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleStrips =
    new NCollection_HArray1<occ::handle<Standard_Transient>>(1, theTriangulation->NbTriangles());
  for (int aTriangleIndex = 1; aTriangleIndex <= theTriangulation->NbTriangles();
       ++aTriangleIndex)
  {
    // Since Poly_Triangulation doesn't support triangle strips or triangle fans,
    // we just write each thriangle as triangle strip.
    const Poly_Triangle&             aCurrentTriangle = theTriangulation->Triangle(aTriangleIndex);
    occ::handle<NCollection_HArray1<int>> aTriangleStrip   = new NCollection_HArray1<int>(1, 3);
    aTriangleStrip->SetValue(1, aCurrentTriangle.Value(1));
    aTriangleStrip->SetValue(2, aCurrentTriangle.Value(2));
    aTriangleStrip->SetValue(3, aCurrentTriangle.Value(3));
    aTriangleStrips->SetValue(aTriangleIndex, aTriangleStrip);
  }
  return aTriangleStrips;
}

//=======================================================================
// function : GenerateComplexTriangulatedSurfaceSet
// purpose  : Generates complex_triangulated_surface_set from theFace.
//           Returns nullptr if face has no triangulation.
//=======================================================================
occ::handle<StepVisual_ComplexTriangulatedSurfaceSet> GenerateComplexTriangulatedSurfaceSet(
  const TopoDS_Face& theFace)
{
  TopLoc_Location                  aFaceLoc;
  const occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(theFace, aFaceLoc);
  if (aTriangulation.IsNull())
  {
    return nullptr;
  }
  const gp_Trsf aFaceTransform = aFaceLoc.Transformation();

  // coordinates
  occ::handle<StepVisual_CoordinatesList> aCoordinatesList =
    GenerateCoordinateList(aTriangulation, aFaceTransform);
  // pnmax
  int aPnmax = aTriangulation->NbNodes();
  // normals
  occ::handle<NCollection_HArray2<double>> aNormals = GenerateNormalsArray(aTriangulation, aFaceTransform);
  // pnindex
  // From "Recommended Practices Recommended Practices for 3D Tessellated Geometry", Release 1.1:
  // "pnindex is the table of indices of the points used in the definition of the triangles.
  //  It is an index to the coordinates_list. Its size may be:
  //  pnmax: this is the size of normals when each point has a normal.
  //  0: no indirection."
  // In our case there is no indirection, so it's always empty.
  occ::handle<NCollection_HArray1<int>> aPnindex = new NCollection_HArray1<int>;
  // triangle_strips
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleStrips = GenerateTriangleStrips(aTriangulation);
  // triangle_fans
  // All triangles were already written as triangle strips.
  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleFans = new NCollection_HArray1<occ::handle<Standard_Transient>>;

  // Initialization of complex_triangulated_surface_set.
  occ::handle<StepVisual_ComplexTriangulatedSurfaceSet> aCTSS =
    new StepVisual_ComplexTriangulatedSurfaceSet;
  aCTSS->Init(new TCollection_HAsciiString(),
              aCoordinatesList,
              aPnmax,
              aNormals,
              aPnindex,
              aTriangleStrips,
              aTriangleFans);
  return aCTSS;
}

//=======================================================================
// function : GenerateTessellatedCurveSet
// purpose  : Generates tesselated_curve_set from theShape.
//           If no valid curves were found, return nullptr.
//=======================================================================
occ::handle<StepVisual_TessellatedCurveSet> GenerateTessellatedCurveSet(const TopoDS_Shape& theShape)
{
  NCollection_Handle<NCollection_Vector<occ::handle<NCollection_HSequence<int>>>> aLineStrips =
    new NCollection_Vector<occ::handle<NCollection_HSequence<int>>>;
  // Temporary contanier for points. We need points in NCollection_HArray1<gp_XYZ> type of
  // container, however in order to create it we need to know it's size.
  // Currently number of points is unknown, so we will put all the points in a
  // temporary container and then just copy them after all edges will be processed.
  NCollection_Vector<gp_XYZ> aTmpPointsContainer;
  for (TopExp_Explorer aCurveIt(theShape, TopAbs_EDGE); aCurveIt.More(); aCurveIt.Next())
  {
    // Find out type of edge curve
    double            aFirstParam = 0, aLastParam = 0;
    const occ::handle<Geom_Curve> anEdgeCurve =
      BRep_Tool::Curve(TopoDS::Edge(aCurveIt.Current()), aFirstParam, aLastParam);
    if (anEdgeCurve.IsNull())
    {
      continue;
    }
    occ::handle<NCollection_HSequence<int>> aCurrentCurve = new NCollection_HSequence<int>;
    if (anEdgeCurve->IsKind(STANDARD_TYPE(Geom_Line))) // Line
    {
      for (TopExp_Explorer aVertIt(aCurveIt.Current(), TopAbs_VERTEX); aVertIt.More();
           aVertIt.Next())
      {
        aTmpPointsContainer.Append(BRep_Tool::Pnt(TopoDS::Vertex(aVertIt.Current())).XYZ());
        aCurrentCurve->Append(aTmpPointsContainer.Size());
      }
    }
    else // BSpline
    {
      ShapeConstruct_Curve      aSCC;
      occ::handle<Geom_BSplineCurve> aBSCurve =
        aSCC.ConvertToBSpline(anEdgeCurve, aFirstParam, aLastParam, Precision::Confusion());
      for (int aPoleIndex = 1; aPoleIndex <= aBSCurve->NbPoles(); ++aPoleIndex)
      {
        aTmpPointsContainer.Append(aBSCurve->Pole(aPoleIndex).XYZ());
        aCurrentCurve->Append(aTmpPointsContainer.Size());
      }
    }
    aLineStrips->Append(aCurrentCurve);
  }

  if (aTmpPointsContainer.IsEmpty())
  {
    return occ::handle<StepVisual_TessellatedCurveSet>{};
  }

  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints = new NCollection_HArray1<gp_XYZ>(1, aTmpPointsContainer.Size());
  for (int aPointIndex = 1; aPointIndex <= aPoints->Size(); ++aPointIndex)
  {
    aPoints->SetValue(aPointIndex, aTmpPointsContainer.Value(aPointIndex - 1));
  }
  // STEP entities
  occ::handle<StepVisual_CoordinatesList> aCoordinates = new StepVisual_CoordinatesList();
  aCoordinates->Init(new TCollection_HAsciiString(), aPoints);
  occ::handle<StepVisual_TessellatedCurveSet> aTCS = new StepVisual_TessellatedCurveSet();
  aTCS->Init(new TCollection_HAsciiString(), aCoordinates, aLineStrips);
  return aTCS;
}
} // namespace

//=================================================================================================

STEPCAFControl_GDTProperty::STEPCAFControl_GDTProperty() {}

//=================================================================================================

void STEPCAFControl_GDTProperty::GetDimModifiers(
  const occ::handle<StepRepr_CompoundRepresentationItem>& theCRI,
  NCollection_Sequence<XCAFDimTolObjects_DimensionModif>&      theModifiers)
{
  for (int l = 1; l <= theCRI->ItemElement()->Length(); l++)
  {
    occ::handle<StepRepr_DescriptiveRepresentationItem> aDRI =
      occ::down_cast<StepRepr_DescriptiveRepresentationItem>(theCRI->ItemElement()->Value(l));
    if (aDRI.IsNull())
      continue;
    XCAFDimTolObjects_DimensionModif aModifier = XCAFDimTolObjects_DimensionModif_ControlledRadius;
    const TCollection_AsciiString    aModifStr = aDRI->Description()->String();
    bool                 aFound    = false;
    if (aModifStr.IsEqual("controlled radius"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_ControlledRadius;
    }
    else if (aModifStr.IsEqual("square"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_Square;
    }
    else if (aModifStr.IsEqual("statistical"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_StatisticalTolerance;
    }
    else if (aModifStr.IsEqual("continuous feature"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_ContinuousFeature;
    }
    else if (aModifStr.IsEqual("two point size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_TwoPointSize;
    }
    else if (aModifStr.IsEqual("local size defined by a sphere"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_LocalSizeDefinedBySphere;
    }
    else if (aModifStr.IsEqual("least squares association criteria"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_LeastSquaresAssociationCriterion;
    }
    else if (aModifStr.IsEqual("maximum inscribed association criteria"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MaximumInscribedAssociation;
    }
    else if (aModifStr.IsEqual("minimum circumscribed association criteria"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MinimumCircumscribedAssociation;
    }
    else if (aModifStr.IsEqual("circumference diameter calculated size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_CircumferenceDiameter;
    }
    else if (aModifStr.IsEqual("area diameter calculated size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_AreaDiameter;
    }
    else if (aModifStr.IsEqual("volume diameter calculated size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_VolumeDiameter;
    }
    else if (aModifStr.IsEqual("maximum rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MaximumSize;
    }
    else if (aModifStr.IsEqual("minimum rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MinimumSize;
    }
    else if (aModifStr.IsEqual("average rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_AverageSize;
    }
    else if (aModifStr.IsEqual("median rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MedianSize;
    }
    else if (aModifStr.IsEqual("mid range rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_MidRangeSize;
    }
    else if (aModifStr.IsEqual("range rank order size"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_RangeOfSizes;
    }
    else if (aModifStr.IsEqual("any part of the feature"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_AnyRestrictedPortionOfFeature;
    }
    else if (aModifStr.IsEqual("any cross section"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_AnyCrossSection;
    }
    else if (aModifStr.IsEqual("specific fixed cross section"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_SpecificFixedCrossSection;
    }
    else if (aModifStr.IsEqual("common tolerance"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_CommonTolerance;
    }
    else if (aModifStr.IsEqual("free state condition"))
    {
      aFound    = true;
      aModifier = XCAFDimTolObjects_DimensionModif_FreeStateCondition;
    }
    if (aFound)
      theModifiers.Append(aModifier);
  }
}

//=================================================================================================

void STEPCAFControl_GDTProperty::GetDimClassOfTolerance(
  const occ::handle<StepShape_LimitsAndFits>&   theLAF,
  bool&                        theHolle,
  XCAFDimTolObjects_DimensionFormVariance& theFV,
  XCAFDimTolObjects_DimensionGrade&        theG)
{
  occ::handle<TCollection_HAsciiString> aFormV = theLAF->FormVariance();
  occ::handle<TCollection_HAsciiString> aGrade = theLAF->Grade();
  theFV                                   = XCAFDimTolObjects_DimensionFormVariance_None;
  bool aFound;
  theHolle = false;
  // it is not verified information
  for (int c = 0; c <= 1 && !aFormV.IsNull(); c++)
  {
    aFound                     = false;
    bool aCaseSens = false;
    if (c == 1)
      aCaseSens = true;
    occ::handle<TCollection_HAsciiString> aStr = new TCollection_HAsciiString("a");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_A;
      continue;
    }
    aStr = new TCollection_HAsciiString("b");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_B;
      continue;
    }
    aStr = new TCollection_HAsciiString("c");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_C;
      continue;
    }
    aStr = new TCollection_HAsciiString("cd");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_CD;
      continue;
    }
    aStr = new TCollection_HAsciiString("d");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_D;
      continue;
    }
    aStr = new TCollection_HAsciiString("e");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_E;
      continue;
    }
    aStr = new TCollection_HAsciiString("ef");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_EF;
      continue;
    }
    aStr = new TCollection_HAsciiString("f");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_F;
      continue;
    }
    aStr = new TCollection_HAsciiString("fg");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_FG;
      continue;
    }
    aStr = new TCollection_HAsciiString("g");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_G;
      continue;
    }
    aStr = new TCollection_HAsciiString("h");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_H;
      continue;
    }
    aStr = new TCollection_HAsciiString("js");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_JS;
      continue;
    }
    aStr = new TCollection_HAsciiString("k");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_K;
      continue;
    }
    aStr = new TCollection_HAsciiString("m");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_M;
      continue;
    }
    aStr = new TCollection_HAsciiString("n");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_N;
      continue;
    }
    aStr = new TCollection_HAsciiString("p");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_P;
      continue;
    }
    aStr = new TCollection_HAsciiString("r");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_R;
      continue;
    }
    aStr = new TCollection_HAsciiString("s");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_S;
      continue;
    }
    aStr = new TCollection_HAsciiString("t");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_T;
      continue;
    }
    aStr = new TCollection_HAsciiString("u");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_U;
      continue;
    }
    aStr = new TCollection_HAsciiString("v");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_V;
      continue;
    }
    aStr = new TCollection_HAsciiString("x");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_X;
      continue;
    }
    aStr = new TCollection_HAsciiString("y");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_Y;
      continue;
    }
    aStr = new TCollection_HAsciiString("b");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_B;
      continue;
    }
    aStr = new TCollection_HAsciiString("z");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_Z;
      continue;
    }
    aStr = new TCollection_HAsciiString("za");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_ZA;
      continue;
    }
    aStr = new TCollection_HAsciiString("zb");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_ZB;
      continue;
    }
    aStr = new TCollection_HAsciiString("zc");
    if (aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = true;
      theFV  = XCAFDimTolObjects_DimensionFormVariance_ZC;
      continue;
    }

    if (c == 1 && !aFound)
      theHolle = true;
  }
  occ::handle<TCollection_HAsciiString> aStr = new TCollection_HAsciiString("01");
  theG                                  = XCAFDimTolObjects_DimensionGrade_IT01;
  if (!aGrade.IsNull() && !aGrade->String().IsEqual("01") && aGrade->IsIntegerValue())
  {
    theG = (XCAFDimTolObjects_DimensionGrade)(aGrade->IntegerValue() + 1);
  }
}

//=================================================================================================

bool STEPCAFControl_GDTProperty::GetDimType(
  const occ::handle<TCollection_HAsciiString>& theName,
  XCAFDimTolObjects_DimensionType&        theType)
{
  TCollection_AsciiString aName = theName->String();
  aName.LowerCase();
  theType = XCAFDimTolObjects_DimensionType_Location_None;
  if (aName.IsEqual("curve length"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_CurveLength;
  }
  else if (aName.IsEqual("diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_Diameter;
  }
  else if (aName.IsEqual("spherical diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_SphericalDiameter;
  }
  else if (aName.IsEqual("radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_Radius;
  }
  else if (aName.IsEqual("spherical radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_SphericalRadius;
  }
  else if (aName.IsEqual("toroidal minor diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter;
  }
  else if (aName.IsEqual("toroidal major diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter;
  }
  else if (aName.IsEqual("toroidal minor radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius;
  }
  else if (aName.IsEqual("toroidal major radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius;
  }
  else if (aName.IsEqual("toroidal high major diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter;
  }
  else if (aName.IsEqual("toroidal low major diameter"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter;
  }
  else if (aName.IsEqual("toroidal high major radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius;
  }
  else if (aName.IsEqual("toroidal low major radius"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius;
  }
  else if (aName.IsEqual("thickness"))
  {
    theType = XCAFDimTolObjects_DimensionType_Size_Thickness;
  }
  else if (aName.IsEqual("curved distance"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_CurvedDistance;
  }
  else if (aName.IsEqual("linear distance"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance;
  }
  else if (aName.IsEqual("linear distance centre outer"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter;
  }
  else if (aName.IsEqual("linear distance centre inner"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner;
  }
  else if (aName.IsEqual("linear distance outer centre"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter;
  }
  else if (aName.IsEqual("linear distance outer outer"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter;
  }
  else if (aName.IsEqual("linear distance outer inner"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner;
  }
  else if (aName.IsEqual("linear distance inner centre"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter;
  }
  else if (aName.IsEqual("linear distance inner outer"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter;
  }
  else if (aName.IsEqual("linear distance inner inner"))
  {
    theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner;
  }

  if (theType != XCAFDimTolObjects_DimensionType_Location_None
      && theType != XCAFDimTolObjects_DimensionType_CommonLabel)
  {
    return true;
  }
  return false;
}

//=================================================================================================

bool STEPCAFControl_GDTProperty::GetDatumTargetType(
  const occ::handle<TCollection_HAsciiString>& theDescription,
  XCAFDimTolObjects_DatumTargetType&      theType)
{
  TCollection_AsciiString aName = theDescription->String();
  aName.LowerCase();
  if (aName.IsEqual("area"))
  {
    theType = XCAFDimTolObjects_DatumTargetType_Area;
    return true;
  }
  else if (aName.IsEqual("line"))
  {
    theType = XCAFDimTolObjects_DatumTargetType_Line;
    return true;
  }
  else if (aName.IsEqual("circle"))
  {
    theType = XCAFDimTolObjects_DatumTargetType_Circle;
    return true;
  }
  else if (aName.IsEqual("rectangle"))
  {
    theType = XCAFDimTolObjects_DatumTargetType_Rectangle;
    return true;
  }
  else if (aName.IsEqual("point"))
  {
    theType = XCAFDimTolObjects_DatumTargetType_Point;
    return true;
  }
  return false;
}

//=================================================================================================

bool STEPCAFControl_GDTProperty::GetDimQualifierType(
  const occ::handle<TCollection_HAsciiString>& theDescription,
  XCAFDimTolObjects_DimensionQualifier&   theType)
{
  TCollection_AsciiString aName = theDescription->String();
  aName.LowerCase();
  theType = XCAFDimTolObjects_DimensionQualifier_None;
  if (aName.IsEqual("maximum"))
  {
    theType = XCAFDimTolObjects_DimensionQualifier_Max;
  }
  else if (aName.IsEqual("minimum"))
  {
    theType = XCAFDimTolObjects_DimensionQualifier_Min;
  }
  else if (aName.IsEqual("average"))
  {
    theType = XCAFDimTolObjects_DimensionQualifier_Avg;
  }
  if (theType != XCAFDimTolObjects_DimensionQualifier_None)
  {
    return true;
  }
  return false;
}

//=================================================================================================

bool STEPCAFControl_GDTProperty::GetTolValueType(
  const occ::handle<TCollection_HAsciiString>&   theDescription,
  XCAFDimTolObjects_GeomToleranceTypeValue& theType)
{
  TCollection_AsciiString aName = theDescription->String();
  aName.LowerCase();
  theType = XCAFDimTolObjects_GeomToleranceTypeValue_None;
  if (aName.IsEqual("cylindrical or circular"))
  {
    theType = XCAFDimTolObjects_GeomToleranceTypeValue_Diameter;
  }
  else if (aName.IsEqual("spherical"))
  {
    theType = XCAFDimTolObjects_GeomToleranceTypeValue_SphericalDiameter;
  }
  if (theType != XCAFDimTolObjects_GeomToleranceTypeValue_None)
  {
    return true;
  }
  return false;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPCAFControl_GDTProperty::GetDimTypeName(
  const XCAFDimTolObjects_DimensionType theType)
{
  occ::handle<TCollection_HAsciiString> aName;
  switch (theType)
  {
    // Dimensional_Location
    case XCAFDimTolObjects_DimensionType_Location_CurvedDistance:
      aName = new TCollection_HAsciiString("curved distance");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance:
      aName = new TCollection_HAsciiString("linear distance");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter:
      aName = new TCollection_HAsciiString("linear distance centre outer");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner:
      aName = new TCollection_HAsciiString("linear distance centre inner");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter:
      aName = new TCollection_HAsciiString("linear distance outer centre");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter:
      aName = new TCollection_HAsciiString("linear distance outer outer");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner:
      aName = new TCollection_HAsciiString("linear distance outer inner");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter:
      aName = new TCollection_HAsciiString("linear distance inner centre");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter:
      aName = new TCollection_HAsciiString("linear distance inner outer");
      break;
    case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner:
      aName = new TCollection_HAsciiString("linear distance inner inner");
      break;
    // Dimensional_Size
    case XCAFDimTolObjects_DimensionType_Size_CurveLength:
      aName = new TCollection_HAsciiString("curve length");
      break;
    case XCAFDimTolObjects_DimensionType_Size_Diameter:
      aName = new TCollection_HAsciiString("diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_SphericalDiameter:
      aName = new TCollection_HAsciiString("spherical diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_Radius:
      aName = new TCollection_HAsciiString("radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_SphericalRadius:
      aName = new TCollection_HAsciiString("spherical radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter:
      aName = new TCollection_HAsciiString("toroidal minor diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter:
      aName = new TCollection_HAsciiString("toroidal major diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius:
      aName = new TCollection_HAsciiString("toroidal minor radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius:
      aName = new TCollection_HAsciiString("toroidal major radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter:
      aName = new TCollection_HAsciiString("toroidal high major diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter:
      aName = new TCollection_HAsciiString("toroidal low major diameter");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius:
      aName = new TCollection_HAsciiString("toroidal high major radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius:
      aName = new TCollection_HAsciiString("toroidal low major radius");
      break;
    case XCAFDimTolObjects_DimensionType_Size_Thickness:
      aName = new TCollection_HAsciiString("thickness");
      break;
    // Other entities
    default:
      aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPCAFControl_GDTProperty::GetDimQualifierName(
  const XCAFDimTolObjects_DimensionQualifier theQualifier)
{
  occ::handle<TCollection_HAsciiString> aName;
  switch (theQualifier)
  {
    case XCAFDimTolObjects_DimensionQualifier_Min:
      aName = new TCollection_HAsciiString("minimum");
      break;
    case XCAFDimTolObjects_DimensionQualifier_Avg:
      aName = new TCollection_HAsciiString("average");
      break;
    case XCAFDimTolObjects_DimensionQualifier_Max:
      aName = new TCollection_HAsciiString("maximum");
      break;
    default:
      aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPCAFControl_GDTProperty::GetDimModifierName(
  const XCAFDimTolObjects_DimensionModif theModifier)
{
  occ::handle<TCollection_HAsciiString> aName;
  switch (theModifier)
  {
    case XCAFDimTolObjects_DimensionModif_ControlledRadius:
      aName = new TCollection_HAsciiString("controlled radius");
      break;
    case XCAFDimTolObjects_DimensionModif_Square:
      aName = new TCollection_HAsciiString("square");
      break;
    case XCAFDimTolObjects_DimensionModif_StatisticalTolerance:
      aName = new TCollection_HAsciiString("statistical");
      break;
    case XCAFDimTolObjects_DimensionModif_ContinuousFeature:
      aName = new TCollection_HAsciiString("continuous feature");
      break;
    case XCAFDimTolObjects_DimensionModif_TwoPointSize:
      aName = new TCollection_HAsciiString("two point size");
      break;
    case XCAFDimTolObjects_DimensionModif_LocalSizeDefinedBySphere:
      aName = new TCollection_HAsciiString("local size defined by a sphere");
      break;
    case XCAFDimTolObjects_DimensionModif_LeastSquaresAssociationCriterion:
      aName = new TCollection_HAsciiString("least squares association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_MaximumInscribedAssociation:
      aName = new TCollection_HAsciiString("maximum inscribed association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_MinimumCircumscribedAssociation:
      aName = new TCollection_HAsciiString("minimum circumscribed association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_CircumferenceDiameter:
      aName = new TCollection_HAsciiString("circumference diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_AreaDiameter:
      aName = new TCollection_HAsciiString("area diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_VolumeDiameter:
      aName = new TCollection_HAsciiString("volume diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_MaximumSize:
      aName = new TCollection_HAsciiString("maximum rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MinimumSize:
      aName = new TCollection_HAsciiString("minimum rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_AverageSize:
      aName = new TCollection_HAsciiString("average rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MedianSize:
      aName = new TCollection_HAsciiString("median rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MidRangeSize:
      aName = new TCollection_HAsciiString("mid range rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_RangeOfSizes:
      aName = new TCollection_HAsciiString("range rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_AnyRestrictedPortionOfFeature:
      aName = new TCollection_HAsciiString("any part of the feature");
      break;
    case XCAFDimTolObjects_DimensionModif_AnyCrossSection:
      aName = new TCollection_HAsciiString("any cross section");
      break;
    case XCAFDimTolObjects_DimensionModif_SpecificFixedCrossSection:
      aName = new TCollection_HAsciiString("specific fixed cross section");
      break;
    case XCAFDimTolObjects_DimensionModif_CommonTolerance:
      aName = new TCollection_HAsciiString("common tolerance");
      break;
    case XCAFDimTolObjects_DimensionModif_FreeStateCondition:
      aName = new TCollection_HAsciiString("free state condition");
      break;
    default:
      aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=================================================================================================

occ::handle<StepShape_LimitsAndFits> STEPCAFControl_GDTProperty::GetLimitsAndFits(
  bool                        theHole,
  XCAFDimTolObjects_DimensionFormVariance theFormVariance,
  XCAFDimTolObjects_DimensionGrade        theGrade)
{
  occ::handle<StepShape_LimitsAndFits>  aLAF = new StepShape_LimitsAndFits();
  occ::handle<TCollection_HAsciiString> aGradeStr, aFormStr, aHoleStr;

  if (theGrade == XCAFDimTolObjects_DimensionGrade_IT01)
    aGradeStr = new TCollection_HAsciiString("01");
  else
    aGradeStr = new TCollection_HAsciiString((int)theGrade + 1);

  switch (theFormVariance)
  {
    case XCAFDimTolObjects_DimensionFormVariance_None:
      aFormStr = new TCollection_HAsciiString("");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_A:
      aFormStr = new TCollection_HAsciiString("A");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_B:
      aFormStr = new TCollection_HAsciiString("B");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_C:
      aFormStr = new TCollection_HAsciiString("C");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_CD:
      aFormStr = new TCollection_HAsciiString("CD");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_D:
      aFormStr = new TCollection_HAsciiString("D");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_E:
      aFormStr = new TCollection_HAsciiString("E");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_EF:
      aFormStr = new TCollection_HAsciiString("EF");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_F:
      aFormStr = new TCollection_HAsciiString("F");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_FG:
      aFormStr = new TCollection_HAsciiString("FG");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_G:
      aFormStr = new TCollection_HAsciiString("G");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_H:
      aFormStr = new TCollection_HAsciiString("H");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_JS:
      aFormStr = new TCollection_HAsciiString("JS");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_J:
      aFormStr = new TCollection_HAsciiString("J");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_K:
      aFormStr = new TCollection_HAsciiString("K");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_M:
      aFormStr = new TCollection_HAsciiString("M");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_N:
      aFormStr = new TCollection_HAsciiString("N");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_P:
      aFormStr = new TCollection_HAsciiString("P");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_R:
      aFormStr = new TCollection_HAsciiString("R");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_S:
      aFormStr = new TCollection_HAsciiString("S");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_T:
      aFormStr = new TCollection_HAsciiString("T");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_U:
      aFormStr = new TCollection_HAsciiString("U");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_V:
      aFormStr = new TCollection_HAsciiString("V");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_X:
      aFormStr = new TCollection_HAsciiString("X");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_Y:
      aFormStr = new TCollection_HAsciiString("Y");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_Z:
      aFormStr = new TCollection_HAsciiString("Z");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZA:
      aFormStr = new TCollection_HAsciiString("ZA");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZB:
      aFormStr = new TCollection_HAsciiString("ZB");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZC:
      aFormStr = new TCollection_HAsciiString("ZC");
      break;
  }

  if (theHole)
  {
    aHoleStr = new TCollection_HAsciiString("hole");
  }
  else
  {
    aHoleStr = new TCollection_HAsciiString("shaft");
    aFormStr->LowerCase();
  }
  aLAF->Init(aFormStr, aHoleStr, aGradeStr, new TCollection_HAsciiString);
  return aLAF;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPCAFControl_GDTProperty::GetDatumTargetName(
  const XCAFDimTolObjects_DatumTargetType theDatumType)
{
  occ::handle<TCollection_HAsciiString> aName;
  switch (theDatumType)
  {
    case XCAFDimTolObjects_DatumTargetType_Point:
      aName = new TCollection_HAsciiString("point");
      break;
    case XCAFDimTolObjects_DatumTargetType_Line:
      aName = new TCollection_HAsciiString("line");
      break;
    case XCAFDimTolObjects_DatumTargetType_Rectangle:
      aName = new TCollection_HAsciiString("rectangle");
      break;
    case XCAFDimTolObjects_DatumTargetType_Circle:
      aName = new TCollection_HAsciiString("circle");
      break;
    case XCAFDimTolObjects_DatumTargetType_Area:
      aName = new TCollection_HAsciiString("area");
      break;
    default:
      aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=================================================================================================

StepDimTol_GeometricToleranceType STEPCAFControl_GDTProperty::GetGeomToleranceType(
  const XCAFDimTolObjects_GeomToleranceType theType)
{
  switch (theType)
  {
    case XCAFDimTolObjects_GeomToleranceType_Angularity:
      return StepDimTol_GTTAngularityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_CircularRunout:
      return StepDimTol_GTTCircularRunoutTolerance;
    case XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness:
      return StepDimTol_GTTRoundnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Coaxiality:
      return StepDimTol_GTTCoaxialityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Concentricity:
      return StepDimTol_GTTConcentricityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Cylindricity:
      return StepDimTol_GTTCylindricityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Flatness:
      return StepDimTol_GTTFlatnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Parallelism:
      return StepDimTol_GTTParallelismTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Perpendicularity:
      return StepDimTol_GTTPerpendicularityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Position:
      return StepDimTol_GTTPositionTolerance;
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfLine:
      return StepDimTol_GTTLineProfileTolerance;
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface:
      return StepDimTol_GTTSurfaceProfileTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Straightness:
      return StepDimTol_GTTStraightnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Symmetry:
      return StepDimTol_GTTSymmetryTolerance;
    case XCAFDimTolObjects_GeomToleranceType_TotalRunout:
      return StepDimTol_GTTTotalRunoutTolerance;
    default:
      return StepDimTol_GTTPositionTolerance;
  }
}

//=================================================================================================

XCAFDimTolObjects_GeomToleranceType STEPCAFControl_GDTProperty::GetGeomToleranceType(
  const StepDimTol_GeometricToleranceType theType)
{
  switch (theType)
  {
    case StepDimTol_GTTAngularityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Angularity;
    case StepDimTol_GTTCircularRunoutTolerance:
      return XCAFDimTolObjects_GeomToleranceType_CircularRunout;
    case StepDimTol_GTTRoundnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness;
    case StepDimTol_GTTCoaxialityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Coaxiality;
    case StepDimTol_GTTConcentricityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Concentricity;
    case StepDimTol_GTTCylindricityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Cylindricity;
    case StepDimTol_GTTFlatnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Flatness;
    case StepDimTol_GTTParallelismTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Parallelism;
    case StepDimTol_GTTPerpendicularityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Perpendicularity;
    case StepDimTol_GTTPositionTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Position;
    case StepDimTol_GTTLineProfileTolerance:
      return XCAFDimTolObjects_GeomToleranceType_ProfileOfLine;
    case StepDimTol_GTTSurfaceProfileTolerance:
      return XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface;
    case StepDimTol_GTTStraightnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Straightness;
    case StepDimTol_GTTSymmetryTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Symmetry;
    case StepDimTol_GTTTotalRunoutTolerance:
      return XCAFDimTolObjects_GeomToleranceType_TotalRunout;
    default:
      return XCAFDimTolObjects_GeomToleranceType_Position;
  }
}

//=================================================================================================

occ::handle<StepDimTol_GeometricTolerance> STEPCAFControl_GDTProperty::GetGeomTolerance(
  const XCAFDimTolObjects_GeomToleranceType theType)
{
  switch (theType)
  {
    case XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness:
      return new StepDimTol_RoundnessTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Cylindricity:
      return new StepDimTol_CylindricityTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Flatness:
      return new StepDimTol_FlatnessTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Position:
      return new StepDimTol_PositionTolerance();
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfLine:
      return new StepDimTol_LineProfileTolerance();
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface:
      return new StepDimTol_SurfaceProfileTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Straightness:
      return new StepDimTol_StraightnessTolerance();
    default:
      return NULL;
  }
}

//=================================================================================================

StepDimTol_GeometricToleranceModifier STEPCAFControl_GDTProperty::GetGeomToleranceModifier(
  const XCAFDimTolObjects_GeomToleranceModif theModifier)
{
  switch (theModifier)
  {
    case XCAFDimTolObjects_GeomToleranceModif_Any_Cross_Section:
      return StepDimTol_GTMAnyCrossSection;
    case XCAFDimTolObjects_GeomToleranceModif_Common_Zone:
      return StepDimTol_GTMCommonZone;
    case XCAFDimTolObjects_GeomToleranceModif_Each_Radial_Element:
      return StepDimTol_GTMEachRadialElement;
    case XCAFDimTolObjects_GeomToleranceModif_Free_State:
      return StepDimTol_GTMFreeState;
    case XCAFDimTolObjects_GeomToleranceModif_Least_Material_Requirement:
      return StepDimTol_GTMLeastMaterialRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Line_Element:
      return StepDimTol_GTMLineElement;
    case XCAFDimTolObjects_GeomToleranceModif_Major_Diameter:
      return StepDimTol_GTMMajorDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Maximum_Material_Requirement:
      return StepDimTol_GTMMaximumMaterialRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Minor_Diameter:
      return StepDimTol_GTMMinorDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Not_Convex:
      return StepDimTol_GTMNotConvex;
    case XCAFDimTolObjects_GeomToleranceModif_Pitch_Diameter:
      return StepDimTol_GTMPitchDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Reciprocity_Requirement:
      return StepDimTol_GTMReciprocityRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Separate_Requirement:
      return StepDimTol_GTMSeparateRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Statistical_Tolerance:
      return StepDimTol_GTMStatisticalTolerance;
    case XCAFDimTolObjects_GeomToleranceModif_Tangent_Plane:
      return StepDimTol_GTMTangentPlane;
    default:
      return StepDimTol_GTMMaximumMaterialRequirement;
  }
}

//=======================================================================
// function : GetDatumRefModifiers
// purpose  : Note: this function does not add anything to model
//=======================================================================
occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> STEPCAFControl_GDTProperty::GetDatumRefModifiers(
  const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>& theModifiers,
  const XCAFDimTolObjects_DatumModifWithValue&    theModifWithVal,
  const double                             theValue,
  const StepBasic_Unit&                           theUnit)
{
  if ((theModifiers.Length() == 0)
      && (theModifWithVal == XCAFDimTolObjects_DatumModifWithValue_None))
    return NULL;
  int aModifNb = theModifiers.Length();
  if (theModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None)
    aModifNb++;
  occ::handle<NCollection_HArray1<StepDimTol_DatumReferenceModifier>> aModifiers =
    new NCollection_HArray1<StepDimTol_DatumReferenceModifier>(1, aModifNb);

  // Modifier with value
  if (theModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None)
  {
    StepDimTol_DatumReferenceModifierType aType;
    switch (theModifWithVal)
    {
      case XCAFDimTolObjects_DatumModifWithValue_CircularOrCylindrical:
        aType = StepDimTol_CircularOrCylindrical;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Distance:
        aType = StepDimTol_Distance;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Projected:
        aType = StepDimTol_Projected;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Spherical:
        aType = StepDimTol_Spherical;
        break;
      default:
        aType = StepDimTol_Distance;
    }
    occ::handle<StepBasic_LengthMeasureWithUnit> aLMWU        = new StepBasic_LengthMeasureWithUnit();
    occ::handle<StepBasic_MeasureValueMember>    aValueMember = new StepBasic_MeasureValueMember();
    aValueMember->SetName("LENGTH_MEASURE");
    aValueMember->SetReal(theValue);
    aLMWU->Init(aValueMember, theUnit);
    occ::handle<StepDimTol_DatumReferenceModifierWithValue> aModifWithVal =
      new StepDimTol_DatumReferenceModifierWithValue();
    aModifWithVal->Init(aType, aLMWU);
    StepDimTol_DatumReferenceModifier aModif;
    aModif.SetValue(aModifWithVal);
    aModifiers->SetValue(aModifNb, aModif);
  }

  // Simple modifiers
  for (int i = 1; i <= theModifiers.Length(); i++)
  {
    occ::handle<StepDimTol_SimpleDatumReferenceModifierMember> aSimpleModifMember =
      new StepDimTol_SimpleDatumReferenceModifierMember();
    switch (theModifiers.Value(i))
    {
      case XCAFDimTolObjects_DatumSingleModif_AnyCrossSection:
        aSimpleModifMember->SetEnumText(0, ".ANY_CROSS_SECTION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Any_LongitudinalSection:
        aSimpleModifMember->SetEnumText(0, ".ANY_LONGITUDINAL_SECTION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Basic:
        aSimpleModifMember->SetEnumText(0, ".BASIC.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_ContactingFeature:
        aSimpleModifMember->SetEnumText(0, ".CONTACTING_FEATURE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintU:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_U.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintV:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_V.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintW:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_W.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintX:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_X.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintY:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_Y.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintZ:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_Z.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DistanceVariable:
        aSimpleModifMember->SetEnumText(0, ".DISTANCE_VARIABLE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_FreeState:
        aSimpleModifMember->SetEnumText(0, ".FREE_STATE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_LeastMaterialRequirement:
        aSimpleModifMember->SetEnumText(0, ".LEAST_MATERIAL_REQUIREMENT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Line:
        aSimpleModifMember->SetEnumText(0, ".LINE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MajorDiameter:
        aSimpleModifMember->SetEnumText(0, ".MAJOR_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MaximumMaterialRequirement:
        aSimpleModifMember->SetEnumText(0, ".MAXIMUM_MATERIAL_REQUIREMENT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MinorDiameter:
        aSimpleModifMember->SetEnumText(0, ".MINOR_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Orientation:
        aSimpleModifMember->SetEnumText(0, ".ORIENTATION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_PitchDiameter:
        aSimpleModifMember->SetEnumText(0, ".PITCH_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Plane:
        aSimpleModifMember->SetEnumText(0, ".PLANE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Point:
        aSimpleModifMember->SetEnumText(0, ".POINT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Translation:
        aSimpleModifMember->SetEnumText(0, ".TRANSLATION.");
        break;
    }
    StepDimTol_DatumReferenceModifier aModif;
    aModif.SetValue(aSimpleModifMember);
    aModifiers->SetValue(i, aModif);
  }

  return aModifiers;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> STEPCAFControl_GDTProperty::GetTolValueType(
  const XCAFDimTolObjects_GeomToleranceTypeValue& theType)
{
  switch (theType)
  {
    case XCAFDimTolObjects_GeomToleranceTypeValue_Diameter:
      return new TCollection_HAsciiString("cylindrical or circular");
    case XCAFDimTolObjects_GeomToleranceTypeValue_SphericalDiameter:
      return new TCollection_HAsciiString("spherical");
    default:
      return new TCollection_HAsciiString("unknown");
  }
}

//=================================================================================================

occ::handle<StepVisual_TessellatedGeometricSet> STEPCAFControl_GDTProperty::GetTessellation(
  const TopoDS_Shape& theShape)
{
  // Build complex_triangulated_surface_set.
  std::vector<occ::handle<StepVisual_ComplexTriangulatedSurfaceSet>> aCTSSs;
  for (TopExp_Explorer aFaceIt(theShape, TopAbs_FACE); aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(aFaceIt.Current());
    aCTSSs.emplace_back(GenerateComplexTriangulatedSurfaceSet(aFace));
  }

  // Build tesselated_curve_set.
  occ::handle<StepVisual_TessellatedCurveSet> aTCS = GenerateTessellatedCurveSet(theShape);

  // Fill the container of tessellated items.
  NCollection_Handle<NCollection_Array1<occ::handle<StepVisual_TessellatedItem>>> aTesselatedItems =
    new NCollection_Array1<occ::handle<StepVisual_TessellatedItem>>(1, static_cast<int>(aCTSSs.size()) + 1);
  aTesselatedItems->SetValue(1, aTCS);
  for (size_t aCTSSIndex = 0; aCTSSIndex < aCTSSs.size(); ++aCTSSIndex)
  {
    aTesselatedItems->SetValue(static_cast<int>(aCTSSIndex) + 2, aCTSSs[aCTSSIndex]);
  }

  // Build tessellated_geometric_set.
  occ::handle<StepVisual_TessellatedGeometricSet> aTGS = new StepVisual_TessellatedGeometricSet();
  aTGS->Init(new TCollection_HAsciiString(), aTesselatedItems);
  return aTGS;
}
