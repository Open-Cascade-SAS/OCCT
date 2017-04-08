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

//:n5 abv 15 Feb 99: S4132: added complex type bounded_curve + surface_curve
//:j4 gka 11 Mar 99 S4134 :  added new types for DIS
//    gka 09.04.99: S4136: new name of parameter write.step.schema 

#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>
#include <StepAP214_Protocol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_Protocol,StepData_Protocol)

static Standard_CString schemaAP214CD  = "AUTOMOTIVE_DESIGN_CC2 { 1 2 10303 214 -1 1 5 4 }";
static Standard_CString schemaAP214DIS = "AUTOMOTIVE_DESIGN { 1 2 10303 214 0 1 1 1 }";
static Standard_CString schemaAP214IS  = "AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }";
static Standard_CString schemaAP203    = "CONFIG_CONTROL_DESIGN";
static Standard_CString schemaAP242DIS = "AP242_MANAGED_MODEL_BASED_3D_ENGINEERING_MIM_LF. {1 0 10303 442 1 1 4 }";

#include <HeaderSection_Protocol.hxx>

#include <StepBasic_Address.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_AdvancedFace.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationCurveOccurrence.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationFillArea.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationFillAreaOccurrence.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_AnnotationOccurrence.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationSubfigureOccurrence.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationSymbol.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_AnnotationSymbolOccurrence.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_AnnotationText.hxx>
#include <StepVisual_AnnotationTextOccurrence.hxx>

#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationContextElement.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ApprovalRelationship.hxx>
#include <StepBasic_ApprovalRole.hxx>
#include <StepBasic_ApprovalStatus.hxx>
#include <StepVisual_AreaInSet.hxx>
#include <StepAP214_AutoDesignActualDateAndTimeAssignment.hxx>
#include <StepAP214_AutoDesignActualDateAssignment.hxx>
#include <StepAP214_AutoDesignApprovalAssignment.hxx>
#include <StepAP214_AutoDesignDateAndPersonAssignment.hxx>
#include <StepAP214_AutoDesignGroupAssignment.hxx>
#include <StepAP214_AutoDesignNominalDateAndTimeAssignment.hxx>
#include <StepAP214_AutoDesignNominalDateAssignment.hxx>
#include <StepAP214_AutoDesignOrganizationAssignment.hxx>
#include <StepAP214_AutoDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AutoDesignPresentedItem.hxx>
#include <StepAP214_AutoDesignSecurityClassificationAssignment.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepAP214_AutoDesignViewArea.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepVisual_BackgroundColour.hxx>
#include <StepGeom_BezierCurve.hxx>
#include <StepGeom_BezierSurface.hxx>
#include <StepShape_Block.hxx>
#include <StepShape_BooleanResult.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepShape_BoxDomain.hxx>
#include <StepShape_BoxedHalfSpace.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepBasic_CalendarDate.hxx>
#include <StepVisual_CameraImage.hxx>
#include <StepVisual_CameraModel.hxx>
#include <StepVisual_CameraModelD2.hxx>
#include <StepVisual_CameraModelD3.hxx>
#include <StepVisual_CameraUsage.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianTransformationOperator.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Circle.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_ColourRgb.hxx>
#include <StepVisual_ColourSpecification.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveOnSurface.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_CompositeText.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_CompositeTextWithAssociatedCurves.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_CompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_CompositeTextWithExtent.hxx>

#include <StepGeom_Conic.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepVisual_ContextDependentInvisibility.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepShape_CsgRepresentation.hxx>
#include <StepShape_CsgShapeRepresentation.hxx>
#include <StepShape_CsgSolid.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_CurveReplica.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateRole.hxx>
#include <StepBasic_DateTimeRole.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DefinedSymbol.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_DegenerateToroidalSurface.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DimensionCurve.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DimensionCurveTerminator.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepGeom_Direction.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_DraughtingAnnotationOccurrence.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_DraughtingPreDefinedColour.hxx>
#include <StepVisual_DraughtingPreDefinedCurveFont.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DraughtingSubfigureRepresentation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DraughtingSymbolRepresentation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DraughtingTextLiteralWithDelineation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DrawingDefinition.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_DrawingRevision.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_EvaluatedDegeneratePcurve.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepVisual_ExternallyDefinedCurveFont.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_ExternallyDefinedHatchStyle.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_ExternallyDefinedSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_ExternallyDefinedTextFont.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_ExternallyDefinedTileStyle.hxx>
#include <StepShape_ExtrudedAreaSolid.hxx>
#include <StepShape_Face.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_FaceOuterBound.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_FillAreaStyleColour.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_FillAreaStyleHatching.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_FillAreaStyleTileSymbolWithStyle.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_FillAreaStyleTiles.hxx>
#include <StepRepr_FunctionallyDefinedTransformation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepShape_HalfSpaceSolid.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_IntersectionCurve.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_LengthUnit.hxx>
#include <StepGeom_Line.hxx>
#include <StepBasic_LocalTime.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationArea.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_MechanicalDesignPresentationArea.hxx>
#include <StepBasic_NamedUnit.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <StepGeom_OffsetSurface.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepAP214_OneDirectionRepeatFactor.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepBasic_OrdinalDate.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_OrganizationRole.hxx>
#include <StepBasic_OrganizationalAddress.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_OrientedFace.hxx>
#include <StepShape_OrientedOpenShell.hxx>
#include <StepShape_OrientedPath.hxx>
#include <StepGeom_OuterBoundaryCurve.hxx>
#include <StepVisual_OverRidingStyledItem.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepRepr_ParametricRepresentationContext.hxx>
#include <StepShape_Path.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepBasic_PersonalAddress.hxx>
#include <StepGeom_Placement.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <StepGeom_Plane.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepBasic_PlaneAngleUnit.hxx>
#include <StepGeom_Point.hxx>
#include <StepGeom_PointOnCurve.hxx>
#include <StepGeom_PointOnSurface.hxx>
#include <StepGeom_PointReplica.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepVisual_PreDefinedColour.hxx>
#include <StepVisual_PreDefinedCurveFont.hxx>
#include <StepVisual_PreDefinedItem.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_PreDefinedSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_PreDefinedTextFont.hxx>

#include <StepVisual_PresentationArea.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationRepresentation.hxx>
#include <StepVisual_PresentationSet.hxx>
#include <StepVisual_PresentationSize.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_PresentationView.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_MechanicalContext.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_ProductDataRepresentationView.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <StepBasic_ProductType.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepGeom_QuasiUniformCurve.hxx>
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepBasic_RatioMeasureWithUnit.hxx>
#include <StepGeom_RationalBSplineCurve.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepAP214_RepItemGroup.hxx>
#include <StepGeom_ReparametrisedCompositeCurveSegment.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepShape_RevolvedAreaSolid.hxx>
#include <StepShape_RightAngularWedge.hxx>
#include <StepShape_RightCircularCone.hxx>
#include <StepShape_RightCircularCylinder.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepBasic_SecurityClassificationLevel.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_FeatureForDatumTargetRelationship.hxx>
#include <StepRepr_ShapeAspectTransition.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SolidAngleMeasureWithUnit.hxx>
#include <StepShape_SolidModel.hxx>
#include <StepShape_SolidReplica.hxx>
#include <StepShape_Sphere.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <StepGeom_SurfaceCurveAndBoundedCurve.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepGeom_SurfaceReplica.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleBoundary.hxx>
#include <StepVisual_SurfaceStyleControlGrid.hxx>
#include <StepVisual_SurfaceStyleFillArea.hxx>
#include <StepVisual_SurfaceStyleParameterLine.hxx>
#include <StepVisual_SurfaceStyleSegmentationCurve.hxx>
#include <StepVisual_SurfaceStyleSilhouette.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <StepShape_SweptAreaSolid.hxx>
#include <StepGeom_SweptSurface.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_SymbolColour.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_SymbolRepresentation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_SymbolRepresentationMap.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_SymbolStyle.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_SymbolTarget.hxx>
#include <StepVisual_Template.hxx>
#include <StepVisual_TemplateInstance.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_TextLiteral.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TextLiteralWithAssociatedCurves.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TextLiteralWithBlankingBox.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TextLiteralWithDelineation.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_TextStyle.hxx>
#include <StepVisual_TextStyleForDefinedFont.hxx>
#include <StepVisual_TextStyleWithBoxCharacteristics.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepVisual_TextStyleWithMirror.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepShape_Torus.hxx>
#include <StepShape_TransitionalShapeRepresentation.hxx>
#include <StepGeom_TrimmedCurve.hxx>
// Removed from CC1-Rev2 to Rev4 : <StepAP214_TwoDirectionRepeatFactor.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <StepGeom_UniformCurve.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_Vector.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <StepBasic_WeekOfYearAndDayDate.hxx>
#include <StepGeom_UniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_BezierCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_BezierSurfaceAndRationalBSplineSurface.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepShape_LoopAndPath.hxx>

// Added by FMA (for Rev4)
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>

// Added by CKY (OCT-1996 for CC1-Rev4)
#include <StepBasic_DesignContext.hxx>

// Added from CC1-Rev2 to Rev4 (MAR-1997)
#include <StepBasic_TimeMeasureWithUnit.hxx>
#include <StepBasic_RatioUnit.hxx>
#include <StepBasic_TimeUnit.hxx>
#include <StepBasic_SiUnitAndRatioUnit.hxx>
#include <StepBasic_SiUnitAndTimeUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndRatioUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndTimeUnit.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepVisual_CameraImage2dWithScale.hxx>
#include <StepVisual_CameraImage3dWithScale.hxx>
#include <StepGeom_CartesianTransformationOperator2d.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepVisual_PresentedItemRepresentation.hxx>
#include <StepVisual_PresentationLayerUsage.hxx>

//  Added by CKY (JUL-1998) for AP214 CC1 -> CC2

#include <StepAP214_AutoDesignDocumentReference.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DigitalDocument.hxx>
#include <StepBasic_DocumentRelationship.hxx>
#include <StepBasic_DocumentType.hxx>
#include <StepBasic_DocumentUsageConstraint.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepBasic_ProductDefinitionEffectivity.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>

#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_PhysicallyModeledProductDefinition.hxx>


#include <StepRepr_ProductDefinitionUsage.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_PromissoryUsageOccurrence.hxx>
#include <StepRepr_QuantifiedAssemblyComponentUsage.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>
#include <StepRepr_SuppliedPartRelationship.hxx>
#include <StepRepr_ExternallyDefinedRepresentation.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_MaterialDesignation.hxx>

#include <StepShape_ContextDependentShapeRepresentation.hxx>

// Added by CKY (Resources)
#include <HeaderSection.hxx>


#include <Interface_DataMapOfTransientInteger.hxx>
// Added from CC2 to DIS March 1999 j4

#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_AppliedDateAssignment.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AppliedPresentedItem.hxx>
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx> 
#include <StepAP214_AppliedDocumentReference.hxx>

// Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepShape_ExtrudedFaceSolid.hxx>
#include <StepShape_RevolvedFaceSolid.hxx>
#include <StepShape_SweptFaceSolid.hxx>
#include <Interface_Static.hxx>
#include <StepBasic_AreaUnit.hxx>
#include <StepBasic_VolumeUnit.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>

// Added by ABV 10.11.99 for AP203
#include <StepBasic_Action.hxx>
#include <StepBasic_ActionAssignment.hxx>
#include <StepBasic_ActionMethod.hxx>
#include <StepBasic_ActionRequestAssignment.hxx>
#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignCertification.hxx>
#include <StepAP203_CcDesignContract.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepAP203_CcDesignSpecificationReference.hxx>
#include <StepBasic_Certification.hxx>
#include <StepBasic_CertificationAssignment.hxx>
#include <StepBasic_CertificationType.hxx>
#include <StepAP203_Change.hxx>
#include <StepAP203_ChangeRequest.hxx>
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationEffectivity.hxx>
#include <StepBasic_Contract.hxx>
#include <StepBasic_ContractAssignment.hxx>
#include <StepBasic_ContractType.hxx>
#include <StepRepr_ProductConcept.hxx>
#include <StepBasic_ProductConceptContext.hxx>
#include <StepAP203_StartRequest.hxx>
#include <StepAP203_StartWork.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepBasic_ActionRequestSolution.hxx>

// Added by ABV 13.01.00 for CAX-IF TRJ3
#include <StepVisual_DraughtingModel.hxx>

// Added by ABV 18.04.00 for CAX-IF TRJ4 (dimensions)
#include <StepShape_AngularLocation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>

// Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
#include <StepBasic_DocumentRepresentationType.hxx>
#include <StepBasic_ObjectRole.hxx>
#include <StepBasic_RoleAssociation.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepBasic_IdentificationAssignment.hxx>
#include <StepBasic_ExternalIdentificationAssignment.hxx>
#include <StepBasic_EffectivityAssignment.hxx>
#include <StepBasic_NameAssignment.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepAP214_Class.hxx>
#include <StepAP214_ExternallyDefinedClass.hxx>
#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepShape_DefinitionalRepresentationAndShapeRepresentation.hxx>

// Added by CKY , 25 APR 2001 for Dimensional Tolerances (CAX-IF TRJ7)
#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_Extension.hxx>
#include <StepShape_DirectedDimensionalLocation.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_PrecisionQualifier.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>

// Added by ABV 28.12.01 for CAX-IF TRJ9 (edge_based_wireframe_model)
#include <StepShape_CompoundShapeRepresentation.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_ConnectedFaceShapeRepresentation.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_EdgeBasedWireframeShapeRepresentation.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepGeom_OrientedSurface.hxx>
#include <StepShape_Subface.hxx>
#include <StepShape_Subedge.hxx>
#include <StepShape_SeamEdge.hxx>
#include <StepShape_ConnectedFaceSubSet.hxx>

//AP209 types
#include <StepBasic_EulerAngles.hxx>
#include <StepBasic_MassUnit.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepElement_Curve3dElementDescriptor.hxx>
#include <StepElement_CurveElementEndReleasePacket.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>
#include <StepElement_CurveElementSectionDerivedDefinitions.hxx>
#include <StepElement_ElementDescriptor.hxx>
#include <StepElement_ElementMaterial.hxx>
#include <StepElement_Surface3dElementDescriptor.hxx>
#include <StepElement_SurfaceElementProperty.hxx>
#include <StepElement_SurfaceSection.hxx>
#include <StepElement_SurfaceSectionField.hxx>
#include <StepElement_SurfaceSectionFieldConstant.hxx>
#include <StepElement_SurfaceSectionFieldVarying.hxx>
#include <StepElement_UniformSurfaceSection.hxx>
#include <StepElement_Volume3dElementDescriptor.hxx>
#include <StepFEA_AlignedCurve3dElementCoordinateSystem.hxx>
#include <StepFEA_ArbitraryVolume3dElementCoordinateSystem.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <StepFEA_Curve3dElementRepresentation.hxx>
#include <StepFEA_Node.hxx>
#include <StepFEA_CurveElementEndOffset.hxx>
#include <StepFEA_CurveElementEndRelease.hxx>
#include <StepFEA_CurveElementInterval.hxx>
#include <StepFEA_CurveElementIntervalConstant.hxx>
#include <StepFEA_DummyNode.hxx>
#include <StepFEA_CurveElementLocation.hxx>
#include <StepFEA_ElementGeometricRelationship.hxx>
#include <StepFEA_ElementGroup.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepFEA_FeaAreaDensity.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>
#include <StepFEA_FeaGroup.hxx>
#include <StepFEA_FeaLinearElasticity.hxx>
#include <StepFEA_FeaMassDensity.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentation.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentationItem.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepFEA_FeaMoistureAbsorption.hxx>
#include <StepFEA_FeaParametricPoint.hxx>
#include <StepFEA_FeaRepresentationItem.hxx>
#include <StepFEA_FeaSecantCoefficientOfLinearThermalExpansion.hxx>
#include <StepFEA_FeaShellBendingStiffness.hxx>
#include <StepFEA_FeaShellMembraneBendingCouplingStiffness.hxx>
#include <StepFEA_FeaShellMembraneStiffness.hxx>
#include <StepFEA_FeaShellShearStiffness.hxx>
#include <StepFEA_GeometricNode.hxx>
#include <StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion.hxx>
#include <StepFEA_NodeGroup.hxx>
#include <StepFEA_NodeRepresentation.hxx>
#include <StepFEA_NodeSet.hxx>
#include <StepFEA_NodeWithSolutionCoordinateSystem.hxx>
#include <StepFEA_NodeWithVector.hxx>
#include <StepFEA_ParametricCurve3dElementCoordinateDirection.hxx>
#include <StepFEA_ParametricCurve3dElementCoordinateSystem.hxx>
#include <StepFEA_ParametricSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_Surface3dElementRepresentation.hxx>
#include <StepFEA_Volume3dElementRepresentation.hxx>
#include <StepRepr_DataEnvironment.hxx>
#include <StepRepr_MaterialPropertyRepresentation.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <StepShape_PointRepresentation.hxx>
#include <StepRepr_MaterialProperty.hxx>
#include <StepFEA_FeaModelDefinition.hxx>
#include <StepFEA_FreedomAndCoefficient.hxx>
#include <StepFEA_FreedomsList.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepFEA_NodeDefinition.hxx>
#include <StepRepr_StructuralResponseProperty.hxx>
#include <StepRepr_StructuralResponsePropertyDefinitionRepresentation.hxx>

#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>

#include <StepFEA_AlignedSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_ConstantSurface3dElementCoordinateSystem.hxx>

// 23.01.2003
#include <StepFEA_CurveElementIntervalLinearlyVarying.hxx>
#include <StepFEA_FeaCurveSectionGeometricRelationship.hxx>
#include <StepFEA_FeaSurfaceSectionGeometricRelationship.hxx>

//added PTV TRJ11 8.02.2003
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>

//TR12J 4.06.2003 G&DT entities
#include <StepShape_ShapeRepresentationWithParameters.hxx>
#include <StepDimTol_ShapeToleranceSelect.hxx>
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_ParallelismTolerance.hxx>
#include <StepDimTol_PerpendicularityTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepDimTol_SymmetryTolerance.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceRelationship.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_CommonDatum.hxx>
#include <StepDimTol_DatumTarget.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>

#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>

// added by skl 10.02.2004 for TRJ13
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_MassMeasureWithUnit.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
#include <StepFEA_SymmetricTensor22d.hxx>
#include <StepFEA_SymmetricTensor42d.hxx>
#include <StepFEA_SymmetricTensor43d.hxx>

// Added by ika for GD&T AP242
#include <StepRepr_Apex.hxx>
#include <StepRepr_CentreOfSymmetry.hxx>
#include <StepRepr_GeometricAlignment.hxx>
#include <StepRepr_ParallelOffset.hxx>
#include <StepRepr_PerpendicularTo.hxx>
#include <StepRepr_Tangent.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepAP242_IdAttribute.hxx>
#include <StepAP242_ItemIdentifiedRepresentationUsage.hxx>
#include <StepRepr_AllAroundShapeAspect.hxx>
#include <StepRepr_BetweenShapeAspect.hxx>
#include <StepRepr_CompositeGroupShapeAspect.hxx>
#include <StepRepr_ContinuosShapeAspect.hxx>
#include <StepDimTol_GeometricToleranceWithDefinedAreaUnit.hxx>
#include <StepDimTol_GeometricToleranceWithDefinedUnit.hxx>
#include <StepDimTol_GeometricToleranceWithMaximumTolerance.hxx>
#include <StepDimTol_GeometricToleranceWithModifiers.hxx>
#include <StepDimTol_UnequallyDisposedGeometricTolerance.hxx>
#include <StepDimTol_NonUniformZoneDefinition.hxx>
#include <StepDimTol_ProjectedZoneDefinition.hxx>
#include <StepDimTol_RunoutZoneDefinition.hxx>
#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepDimTol_ToleranceZoneDefinition.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>
#include <StepShape_ValueFormatTypeQualifier.hxx>
#include <StepDimTol_DatumReferenceCompartment.hxx>
#include <StepDimTol_DatumReferenceElement.hxx>
#include <StepDimTol_DatumReferenceModifierWithValue.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepDimTol_GeneralDatumReference.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRef.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMod.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_CompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <StepRepr_ValueRepresentationItem.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthMaxTol.hxx>
#include <StepVisual_AnnotationCurveOccurrence.hxx>
#include <StepVisual_AnnotationPlane.hxx>
#include <StepVisual_DraughtingCallout.hxx>


#include <StepVisual_TessellatedAnnotationOccurrence.hxx>
#include <StepVisual_TessellatedItem.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepRepr_CharacterizedRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentationRelationship.hxx>
#include <StepVisual_CharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation.hxx>
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include <StepVisual_AnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem.hxx>

static int THE_StepAP214_Protocol_init = 0;
static Interface_DataMapOfTransientInteger types(800);

//=======================================================================
//function : StepAP214_Protocol
//purpose  : 
//=======================================================================

StepAP214_Protocol::StepAP214_Protocol ()
{
  if (THE_StepAP214_Protocol_init)
  {
    return;
  }
  THE_StepAP214_Protocol_init = 1;

  types.Bind (STANDARD_TYPE(StepBasic_Address), 1);
  types.Bind (STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation), 2);
  types.Bind (STANDARD_TYPE(StepShape_AdvancedFace), 3);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationCurveOccurrence), 4);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationFillArea), 5);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationFillAreaOccurrence), 6);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationOccurrence), 7);
//  types.Bind (STANDARD_TYPE(StepVisual_AnnotationSubfigureOccurrence), 8);
//  types.Bind (STANDARD_TYPE(StepVisual_AnnotationSymbol), 9);
//  types.Bind (STANDARD_TYPE(StepVisual_AnnotationSymbolOccurrence), 10);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationText), 11);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationTextOccurrence), 12);
  types.Bind (STANDARD_TYPE(StepBasic_ApplicationContext), 13);
  types.Bind (STANDARD_TYPE(StepBasic_ApplicationContextElement), 14);
  types.Bind (STANDARD_TYPE(StepBasic_ApplicationProtocolDefinition), 15);
  types.Bind (STANDARD_TYPE(StepBasic_Approval), 16);
  types.Bind (STANDARD_TYPE(StepBasic_ApprovalPersonOrganization), 18);
  types.Bind (STANDARD_TYPE(StepBasic_ApprovalRelationship), 19);
  types.Bind (STANDARD_TYPE(StepBasic_ApprovalRole), 20);
  types.Bind (STANDARD_TYPE(StepBasic_ApprovalStatus), 21);
  types.Bind (STANDARD_TYPE(StepVisual_AreaInSet), 22);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignActualDateAndTimeAssignment), 23);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignActualDateAssignment), 24);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignApprovalAssignment), 25);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignDateAndPersonAssignment), 26);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignGroupAssignment), 27);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignNominalDateAndTimeAssignment), 28);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignNominalDateAssignment), 29);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignOrganizationAssignment), 30);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignPersonAndOrganizationAssignment), 31);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignPresentedItem), 32);
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignSecurityClassificationAssignment), 33);
//  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignViewArea), 34);
  types.Bind (STANDARD_TYPE(StepGeom_Axis1Placement), 35);
  types.Bind (STANDARD_TYPE(StepGeom_Axis2Placement2d), 36);
  types.Bind (STANDARD_TYPE(StepGeom_Axis2Placement3d), 37);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineCurve), 38);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineCurveWithKnots), 39);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineSurface), 40);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnots), 41);
  types.Bind (STANDARD_TYPE(StepVisual_BackgroundColour), 42);
  types.Bind (STANDARD_TYPE(StepGeom_BezierCurve), 43);
  types.Bind (STANDARD_TYPE(StepGeom_BezierSurface), 44);
  types.Bind (STANDARD_TYPE(StepShape_Block), 45);
  types.Bind (STANDARD_TYPE(StepShape_BooleanResult), 46);
  types.Bind (STANDARD_TYPE(StepGeom_BoundaryCurve), 47);
  types.Bind (STANDARD_TYPE(StepGeom_BoundedCurve), 48);
  types.Bind (STANDARD_TYPE(StepGeom_BoundedSurface), 49);
  types.Bind (STANDARD_TYPE(StepShape_BoxDomain), 50);
  types.Bind (STANDARD_TYPE(StepShape_BoxedHalfSpace), 51);
  types.Bind (STANDARD_TYPE(StepShape_BrepWithVoids), 52);
  types.Bind (STANDARD_TYPE(StepBasic_CalendarDate), 53);
  types.Bind (STANDARD_TYPE(StepVisual_CameraImage), 54);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModel), 55);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModelD2), 56);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModelD3), 57);
  types.Bind (STANDARD_TYPE(StepVisual_CameraUsage), 58);
  types.Bind (STANDARD_TYPE(StepGeom_CartesianPoint), 59);
  types.Bind (STANDARD_TYPE(StepGeom_CartesianTransformationOperator), 60);
  types.Bind (STANDARD_TYPE(StepGeom_CartesianTransformationOperator3d), 61);
  types.Bind (STANDARD_TYPE(StepGeom_Circle), 62);
  types.Bind (STANDARD_TYPE(StepShape_ClosedShell), 63);
  types.Bind (STANDARD_TYPE(StepVisual_Colour), 64);
  types.Bind (STANDARD_TYPE(StepVisual_ColourRgb), 65);
  types.Bind (STANDARD_TYPE(StepVisual_ColourSpecification), 66);
  types.Bind (STANDARD_TYPE(StepGeom_CompositeCurve), 67);
  types.Bind (STANDARD_TYPE(StepGeom_CompositeCurveOnSurface), 68);
  types.Bind (STANDARD_TYPE(StepGeom_CompositeCurveSegment), 69);
  types.Bind (STANDARD_TYPE(StepVisual_CompositeText), 70);
//  types.Bind (STANDARD_TYPE(StepVisual_CompositeTextWithAssociatedCurves), 71);
//  types.Bind (STANDARD_TYPE(StepVisual_CompositeTextWithBlankingBox), 72);
  types.Bind (STANDARD_TYPE(StepVisual_CompositeTextWithExtent), 73);
  types.Bind (STANDARD_TYPE(StepGeom_Conic), 74);
  types.Bind (STANDARD_TYPE(StepGeom_ConicalSurface), 75);
  types.Bind (STANDARD_TYPE(StepShape_ConnectedFaceSet), 76);
  types.Bind (STANDARD_TYPE(StepVisual_ContextDependentInvisibility), 77);
  types.Bind (STANDARD_TYPE(StepVisual_ContextDependentOverRidingStyledItem), 78);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnit), 79);
  types.Bind (STANDARD_TYPE(StepBasic_CoordinatedUniversalTimeOffset), 80);
//  types.Bind (STANDARD_TYPE(StepShape_CsgRepresentation), 81);
  types.Bind (STANDARD_TYPE(StepShape_CsgShapeRepresentation), 82);
  types.Bind (STANDARD_TYPE(StepShape_CsgSolid), 83);
  types.Bind (STANDARD_TYPE(StepGeom_Curve), 84);
  types.Bind (STANDARD_TYPE(StepGeom_CurveBoundedSurface), 85);
  types.Bind (STANDARD_TYPE(StepGeom_CurveReplica), 86);
  types.Bind (STANDARD_TYPE(StepVisual_CurveStyle), 87);
  types.Bind (STANDARD_TYPE(StepVisual_CurveStyleFont), 88);
  types.Bind (STANDARD_TYPE(StepVisual_CurveStyleFontPattern), 89);
  types.Bind (STANDARD_TYPE(StepGeom_CylindricalSurface), 90);
  types.Bind (STANDARD_TYPE(StepBasic_Date), 91);
  types.Bind (STANDARD_TYPE(StepBasic_DateAndTime), 92);
  types.Bind (STANDARD_TYPE(StepBasic_DateRole), 95);
  types.Bind (STANDARD_TYPE(StepBasic_DateTimeRole), 96);
//  types.Bind (STANDARD_TYPE(StepVisual_DefinedSymbol), 97);
  types.Bind (STANDARD_TYPE(StepRepr_DefinitionalRepresentation), 98);
  types.Bind (STANDARD_TYPE(StepGeom_DegeneratePcurve), 99);
  types.Bind (STANDARD_TYPE(StepGeom_DegenerateToroidalSurface), 100);
  types.Bind (STANDARD_TYPE(StepRepr_DescriptiveRepresentationItem), 101);
//  types.Bind (STANDARD_TYPE(StepVisual_DimensionCurve), 102);
//  types.Bind (STANDARD_TYPE(StepVisual_DimensionCurveTerminator), 103);
  types.Bind (STANDARD_TYPE(StepBasic_DimensionalExponents), 104);
  types.Bind (STANDARD_TYPE(StepGeom_Direction), 105);
  types.Bind (STANDARD_TYPE(StepVisual_DraughtingAnnotationOccurrence), 106);
  types.Bind (STANDARD_TYPE(StepVisual_DraughtingCallout), 107);
  types.Bind (STANDARD_TYPE(StepVisual_DraughtingPreDefinedColour), 108);
  types.Bind (STANDARD_TYPE(StepVisual_DraughtingPreDefinedCurveFont), 109);
//  types.Bind (STANDARD_TYPE(StepVisual_DraughtingSubfigureRepresentation), 110);
//  types.Bind (STANDARD_TYPE(StepVisual_DraughtingSymbolRepresentation), 111);
//  types.Bind (STANDARD_TYPE(StepVisual_DraughtingTextLiteralWithDelineation), 112);
//  types.Bind (STANDARD_TYPE(StepVisual_DrawingDefinition), 113);
//  types.Bind (STANDARD_TYPE(StepVisual_DrawingRevision), 114);
  types.Bind (STANDARD_TYPE(StepShape_Edge), 115);
  types.Bind (STANDARD_TYPE(StepShape_EdgeCurve), 116);
  types.Bind (STANDARD_TYPE(StepShape_EdgeLoop), 117);
  types.Bind (STANDARD_TYPE(StepGeom_ElementarySurface), 118);
  types.Bind (STANDARD_TYPE(StepGeom_Ellipse), 119);
  types.Bind (STANDARD_TYPE(StepGeom_EvaluatedDegeneratePcurve), 120);
  types.Bind (STANDARD_TYPE(StepBasic_ExternalSource), 121);
  types.Bind (STANDARD_TYPE(StepVisual_ExternallyDefinedCurveFont), 122);
//  types.Bind (STANDARD_TYPE(StepVisual_ExternallyDefinedHatchStyle), 123);
  types.Bind (STANDARD_TYPE(StepBasic_ExternallyDefinedItem), 124);
//  types.Bind (STANDARD_TYPE(StepVisual_ExternallyDefinedSymbol), 125);
  types.Bind (STANDARD_TYPE(StepVisual_ExternallyDefinedTextFont), 126);
//  types.Bind (STANDARD_TYPE(StepVisual_ExternallyDefinedTileStyle), 127);
  types.Bind (STANDARD_TYPE(StepShape_ExtrudedAreaSolid), 128);
  types.Bind (STANDARD_TYPE(StepShape_Face), 129);
//  types.Bind (STANDARD_TYPE(StepShape_FaceBasedSurfaceModel), 130);
  types.Bind (STANDARD_TYPE(StepShape_FaceBound), 131);
  types.Bind (STANDARD_TYPE(StepShape_FaceOuterBound), 132);
  types.Bind (STANDARD_TYPE(StepShape_FaceSurface), 133);
  types.Bind (STANDARD_TYPE(StepShape_FacetedBrep), 134);
  types.Bind (STANDARD_TYPE(StepShape_FacetedBrepShapeRepresentation), 135);
  types.Bind (STANDARD_TYPE(StepVisual_FillAreaStyle), 136);
  types.Bind (STANDARD_TYPE(StepVisual_FillAreaStyleColour), 137);
//  types.Bind (STANDARD_TYPE(StepVisual_FillAreaStyleHatching), 138);
//  types.Bind (STANDARD_TYPE(StepVisual_FillAreaStyleTileSymbolWithStyle), 139);
//  types.Bind (STANDARD_TYPE(StepVisual_FillAreaStyleTiles), 140);
  types.Bind (STANDARD_TYPE(StepRepr_FunctionallyDefinedTransformation), 141);
  types.Bind (STANDARD_TYPE(StepShape_GeometricCurveSet), 142);
  types.Bind (STANDARD_TYPE(StepGeom_GeometricRepresentationContext), 143);
  types.Bind (STANDARD_TYPE(StepGeom_GeometricRepresentationItem), 144);
  types.Bind (STANDARD_TYPE(StepShape_GeometricSet), 145);
  types.Bind (STANDARD_TYPE(StepShape_GeometricallyBoundedSurfaceShapeRepresentation), 146);
  types.Bind (STANDARD_TYPE(StepShape_GeometricallyBoundedWireframeShapeRepresentation), 147);
  types.Bind (STANDARD_TYPE(StepRepr_GlobalUncertaintyAssignedContext), 148);
  types.Bind (STANDARD_TYPE(StepRepr_GlobalUnitAssignedContext), 149);
  types.Bind (STANDARD_TYPE(StepBasic_Group), 150);
  types.Bind (STANDARD_TYPE(StepBasic_GroupRelationship), 152);
  types.Bind (STANDARD_TYPE(StepShape_HalfSpaceSolid), 153);
  types.Bind (STANDARD_TYPE(StepGeom_Hyperbola), 154);
  types.Bind (STANDARD_TYPE(StepGeom_IntersectionCurve), 155);
  types.Bind (STANDARD_TYPE(StepVisual_Invisibility), 156);
  types.Bind (STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), 157);
  types.Bind (STANDARD_TYPE(StepBasic_LengthUnit), 158);
  types.Bind (STANDARD_TYPE(StepGeom_Line), 159);
  types.Bind (STANDARD_TYPE(StepBasic_LocalTime), 160);
  types.Bind (STANDARD_TYPE(StepShape_Loop), 161);
  types.Bind (STANDARD_TYPE(StepShape_ManifoldSolidBrep), 162);
  types.Bind (STANDARD_TYPE(StepShape_ManifoldSurfaceShapeRepresentation), 163);
  types.Bind (STANDARD_TYPE(StepRepr_MappedItem), 164);
  types.Bind (STANDARD_TYPE(StepBasic_MeasureWithUnit), 165);
  types.Bind (STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationArea), 166);
  types.Bind (STANDARD_TYPE(StepVisual_MechanicalDesignGeometricPresentationRepresentation), 167);
//  types.Bind (STANDARD_TYPE(StepVisual_MechanicalDesignPresentationArea), 168);
  types.Bind (STANDARD_TYPE(StepBasic_NamedUnit), 169);
//  types.Bind (STANDARD_TYPE(StepShape_NonManifoldSurfaceShapeRepresentation), 170);
  types.Bind (STANDARD_TYPE(StepGeom_OffsetCurve3d), 171);
  types.Bind (STANDARD_TYPE(StepGeom_OffsetSurface), 172);
//  types.Bind (STANDARD_TYPE(StepAP214_OneDirectionRepeatFactor), 173);
  types.Bind (STANDARD_TYPE(StepShape_OpenShell), 174);
  types.Bind (STANDARD_TYPE(StepBasic_OrdinalDate), 175);
  types.Bind (STANDARD_TYPE(StepBasic_Organization), 176);
  types.Bind (STANDARD_TYPE(StepBasic_OrganizationRole), 178);
  types.Bind (STANDARD_TYPE(StepBasic_OrganizationalAddress), 179);
  types.Bind (STANDARD_TYPE(StepShape_OrientedClosedShell), 180);
  types.Bind (STANDARD_TYPE(StepShape_OrientedEdge), 181);
  types.Bind (STANDARD_TYPE(StepShape_OrientedFace), 182);
  types.Bind (STANDARD_TYPE(StepShape_OrientedOpenShell), 183);
  types.Bind (STANDARD_TYPE(StepShape_OrientedPath), 184);
  types.Bind (STANDARD_TYPE(StepGeom_OuterBoundaryCurve), 185);
  types.Bind (STANDARD_TYPE(StepVisual_OverRidingStyledItem), 186);
  types.Bind (STANDARD_TYPE(StepGeom_Parabola), 187);
  types.Bind (STANDARD_TYPE(StepRepr_ParametricRepresentationContext), 188);
  types.Bind (STANDARD_TYPE(StepShape_Path), 189);
  types.Bind (STANDARD_TYPE(StepGeom_Pcurve), 190);
  types.Bind (STANDARD_TYPE(StepBasic_Person), 191);
  types.Bind (STANDARD_TYPE(StepBasic_PersonAndOrganization), 192);
  types.Bind (STANDARD_TYPE(StepBasic_PersonAndOrganizationRole), 194);
  types.Bind (STANDARD_TYPE(StepBasic_PersonalAddress), 195);
  types.Bind (STANDARD_TYPE(StepGeom_Placement), 196);
  types.Bind (STANDARD_TYPE(StepVisual_PlanarBox), 197);
  types.Bind (STANDARD_TYPE(StepVisual_PlanarExtent), 198);
  types.Bind (STANDARD_TYPE(StepGeom_Plane), 199);
  types.Bind (STANDARD_TYPE(StepBasic_PlaneAngleMeasureWithUnit), 200);
  types.Bind (STANDARD_TYPE(StepBasic_PlaneAngleUnit), 201);
  types.Bind (STANDARD_TYPE(StepGeom_Point), 202);
  types.Bind (STANDARD_TYPE(StepGeom_PointOnCurve), 203);
  types.Bind (STANDARD_TYPE(StepGeom_PointOnSurface), 204);
  types.Bind (STANDARD_TYPE(StepGeom_PointReplica), 205);
  types.Bind (STANDARD_TYPE(StepVisual_PointStyle), 206);
  types.Bind (STANDARD_TYPE(StepShape_PolyLoop), 207);
  types.Bind (STANDARD_TYPE(StepGeom_Polyline), 208);
  types.Bind (STANDARD_TYPE(StepVisual_PreDefinedColour), 209);
  types.Bind (STANDARD_TYPE(StepVisual_PreDefinedCurveFont), 210);
  types.Bind (STANDARD_TYPE(StepVisual_PreDefinedItem), 211);
//  types.Bind (STANDARD_TYPE(StepVisual_PreDefinedSymbol), 212);
  types.Bind (STANDARD_TYPE(StepVisual_PreDefinedTextFont), 213);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationArea), 214);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationLayerAssignment), 215);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationRepresentation), 216);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationSet), 217);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationSize), 218);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationStyleAssignment), 219);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationStyleByContext), 220);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationView), 221);
  types.Bind (STANDARD_TYPE(StepBasic_Product), 223);
  types.Bind (STANDARD_TYPE(StepBasic_ProductCategory), 224);
  types.Bind (STANDARD_TYPE(StepBasic_ProductContext), 225);
//  types.Bind (STANDARD_TYPE(StepVisual_ProductDataRepresentationView), 226);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinition), 227);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionContext), 228);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionFormation), 229);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionFormationWithSpecifiedSource), 230);
  types.Bind (STANDARD_TYPE(StepRepr_ProductDefinitionShape), 231);
  types.Bind (STANDARD_TYPE(StepBasic_ProductRelatedProductCategory), 232);
  types.Bind (STANDARD_TYPE(StepBasic_ProductType), 233);
  types.Bind (STANDARD_TYPE(StepRepr_PropertyDefinition), 234);
  types.Bind (STANDARD_TYPE(StepRepr_PropertyDefinitionRepresentation), 235);
  types.Bind (STANDARD_TYPE(StepGeom_QuasiUniformCurve), 236);
  types.Bind (STANDARD_TYPE(StepGeom_QuasiUniformSurface), 237);
  types.Bind (STANDARD_TYPE(StepBasic_RatioMeasureWithUnit), 238);
  types.Bind (STANDARD_TYPE(StepGeom_RationalBSplineCurve), 239);
  types.Bind (STANDARD_TYPE(StepGeom_RationalBSplineSurface), 240);
  types.Bind (STANDARD_TYPE(StepGeom_RectangularCompositeSurface), 241);
  types.Bind (STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), 242);
  types.Bind (STANDARD_TYPE(StepAP214_RepItemGroup), 243);
  types.Bind (STANDARD_TYPE(StepGeom_ReparametrisedCompositeCurveSegment), 244);
  types.Bind (STANDARD_TYPE(StepRepr_Representation), 245);
  types.Bind (STANDARD_TYPE(StepRepr_RepresentationContext), 246);
  types.Bind (STANDARD_TYPE(StepRepr_RepresentationItem), 247);
  types.Bind (STANDARD_TYPE(StepRepr_RepresentationMap), 248);
  types.Bind (STANDARD_TYPE(StepRepr_RepresentationRelationship), 249);
  types.Bind (STANDARD_TYPE(StepShape_RevolvedAreaSolid), 250);
  types.Bind (STANDARD_TYPE(StepShape_RightAngularWedge), 251);
  types.Bind (STANDARD_TYPE(StepShape_RightCircularCone), 252);
  types.Bind (STANDARD_TYPE(StepShape_RightCircularCylinder), 253);
  types.Bind (STANDARD_TYPE(StepGeom_SeamCurve), 254);
  types.Bind (STANDARD_TYPE(StepBasic_SecurityClassification), 255);
  types.Bind (STANDARD_TYPE(StepBasic_SecurityClassificationLevel), 257);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeAspect), 258);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeAspectRelationship), 259);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeAspectTransition), 260);
  types.Bind (STANDARD_TYPE(StepShape_ShapeDefinitionRepresentation), 261);
  types.Bind (STANDARD_TYPE(StepShape_ShapeRepresentation), 262);
  types.Bind (STANDARD_TYPE(StepShape_ShellBasedSurfaceModel), 263);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnit), 264);
  types.Bind (STANDARD_TYPE(StepBasic_SolidAngleMeasureWithUnit), 265);
  types.Bind (STANDARD_TYPE(StepShape_SolidModel), 266);
  types.Bind (STANDARD_TYPE(StepShape_SolidReplica), 267);
  types.Bind (STANDARD_TYPE(StepShape_Sphere), 268);
  types.Bind (STANDARD_TYPE(StepGeom_SphericalSurface), 269);
  types.Bind (STANDARD_TYPE(StepVisual_StyledItem), 270);
  types.Bind (STANDARD_TYPE(StepGeom_Surface), 271);
  types.Bind (STANDARD_TYPE(StepGeom_SurfaceCurve), 272);
  types.Bind (STANDARD_TYPE(StepGeom_SurfaceOfLinearExtrusion), 273);
  types.Bind (STANDARD_TYPE(StepGeom_SurfaceOfRevolution), 274);
  types.Bind (STANDARD_TYPE(StepGeom_SurfacePatch), 275);
  types.Bind (STANDARD_TYPE(StepGeom_SurfaceReplica), 276);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceSideStyle), 277);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleBoundary), 278);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleControlGrid), 279);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleFillArea), 280);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleParameterLine), 281);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleSegmentationCurve), 282);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleSilhouette), 283);
  types.Bind (STANDARD_TYPE(StepVisual_SurfaceStyleUsage), 284);
  types.Bind (STANDARD_TYPE(StepShape_SweptAreaSolid), 285);
  types.Bind (STANDARD_TYPE(StepGeom_SweptSurface), 286);
//  types.Bind (STANDARD_TYPE(StepVisual_SymbolColour), 287);
//  types.Bind (STANDARD_TYPE(StepVisual_SymbolRepresentation), 288);
//  types.Bind (STANDARD_TYPE(StepVisual_SymbolRepresentationMap), 289);
//  types.Bind (STANDARD_TYPE(StepVisual_SymbolStyle), 290);
//  types.Bind (STANDARD_TYPE(StepVisual_SymbolTarget), 291);
  types.Bind (STANDARD_TYPE(StepVisual_Template), 292);
  types.Bind (STANDARD_TYPE(StepVisual_TemplateInstance), 293);
//  types.Bind (STANDARD_TYPE(StepVisual_TerminatorSymbol), 294);
  types.Bind (STANDARD_TYPE(StepVisual_TextLiteral), 295);
//  types.Bind (STANDARD_TYPE(StepVisual_TextLiteralWithAssociatedCurves), 296);
//  types.Bind (STANDARD_TYPE(StepVisual_TextLiteralWithBlankingBox), 297);
//  types.Bind (STANDARD_TYPE(StepVisual_TextLiteralWithDelineation), 298);
//  types.Bind (STANDARD_TYPE(StepVisual_TextLiteralWithExtent), 299);
  types.Bind (STANDARD_TYPE(StepVisual_TextStyle), 300);
  types.Bind (STANDARD_TYPE(StepVisual_TextStyleForDefinedFont), 301);
  types.Bind (STANDARD_TYPE(StepVisual_TextStyleWithBoxCharacteristics), 302);
//  types.Bind (STANDARD_TYPE(StepVisual_TextStyleWithMirror), 303);
  types.Bind (STANDARD_TYPE(StepShape_TopologicalRepresentationItem), 304);
  types.Bind (STANDARD_TYPE(StepGeom_ToroidalSurface), 305);
  types.Bind (STANDARD_TYPE(StepShape_Torus), 306);
  types.Bind (STANDARD_TYPE(StepShape_TransitionalShapeRepresentation), 307);
  types.Bind (STANDARD_TYPE(StepGeom_TrimmedCurve), 308);
//  types.Bind (STANDARD_TYPE(StepAP214_TwoDirectionRepeatFactor), 309);
  types.Bind (STANDARD_TYPE(StepBasic_UncertaintyMeasureWithUnit), 310);
  types.Bind (STANDARD_TYPE(StepGeom_UniformCurve), 311);
  types.Bind (STANDARD_TYPE(StepGeom_UniformSurface), 312);
  types.Bind (STANDARD_TYPE(StepGeom_Vector), 313);
  types.Bind (STANDARD_TYPE(StepShape_Vertex), 314);
  types.Bind (STANDARD_TYPE(StepShape_VertexLoop), 315);
  types.Bind (STANDARD_TYPE(StepShape_VertexPoint), 316);
  types.Bind (STANDARD_TYPE(StepVisual_ViewVolume), 317);
  types.Bind (STANDARD_TYPE(StepBasic_WeekOfYearAndDayDate), 318);
  types.Bind (STANDARD_TYPE(StepGeom_UniformCurveAndRationalBSplineCurve), 319);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve), 320);
  types.Bind (STANDARD_TYPE(StepGeom_QuasiUniformCurveAndRationalBSplineCurve), 321);
  types.Bind (STANDARD_TYPE(StepGeom_BezierCurveAndRationalBSplineCurve), 322);
  types.Bind (STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface), 323);
  types.Bind (STANDARD_TYPE(StepGeom_UniformSurfaceAndRationalBSplineSurface), 324);
  types.Bind (STANDARD_TYPE(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface), 325);
  types.Bind (STANDARD_TYPE(StepGeom_BezierSurfaceAndRationalBSplineSurface), 326);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndLengthUnit), 327);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit), 328);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndLengthUnit), 329);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit), 330);
  types.Bind (STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext), 331);
  types.Bind (STANDARD_TYPE(StepShape_LoopAndPath), 332);
	// Added by FMA
  types.Bind (STANDARD_TYPE(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx), 333);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndSolidAngleUnit), 334);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndSolidAngleUnit), 335);
  types.Bind (STANDARD_TYPE(StepBasic_SolidAngleUnit), 336);
  types.Bind (STANDARD_TYPE(StepShape_FacetedBrepAndBrepWithVoids), 337);
  types.Bind (STANDARD_TYPE(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext), 338);
  types.Bind (STANDARD_TYPE(StepBasic_MechanicalContext), 339);
  types.Bind (STANDARD_TYPE(StepBasic_DesignContext), 340);

// full Rev4
  types.Bind (STANDARD_TYPE(StepBasic_TimeMeasureWithUnit), 341);
  types.Bind (STANDARD_TYPE(StepBasic_RatioUnit), 342);
  types.Bind (STANDARD_TYPE(StepBasic_TimeUnit), 343);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndRatioUnit), 344);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndTimeUnit), 345);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndRatioUnit), 346);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndTimeUnit), 347);

  types.Bind (STANDARD_TYPE(StepBasic_ApprovalDateTime), 348);
  types.Bind (STANDARD_TYPE(StepVisual_CameraImage2dWithScale), 349);
  types.Bind (STANDARD_TYPE(StepVisual_CameraImage3dWithScale), 350);
  types.Bind (STANDARD_TYPE(StepGeom_CartesianTransformationOperator2d),351);
  types.Bind (STANDARD_TYPE(StepBasic_DerivedUnit), 352);
  types.Bind (STANDARD_TYPE(StepBasic_DerivedUnitElement), 353);
  types.Bind (STANDARD_TYPE(StepRepr_ItemDefinedTransformation), 354);
  types.Bind (STANDARD_TYPE(StepVisual_PresentedItemRepresentation), 355);
  types.Bind (STANDARD_TYPE(StepVisual_PresentationLayerUsage), 356);
  types.Bind (STANDARD_TYPE(StepGeom_SurfaceCurveAndBoundedCurve), 358); //:n5

//  AP214 : CC1 -> CC2
  types.Bind (STANDARD_TYPE(StepAP214_AutoDesignDocumentReference),366);
  types.Bind (STANDARD_TYPE(StepBasic_Document), 367);
  types.Bind (STANDARD_TYPE(StepBasic_DigitalDocument), 368);
  types.Bind (STANDARD_TYPE(StepBasic_DocumentRelationship), 369);
  types.Bind (STANDARD_TYPE(StepBasic_DocumentType), 370);
  types.Bind (STANDARD_TYPE(StepBasic_DocumentUsageConstraint), 371);
  types.Bind (STANDARD_TYPE(StepBasic_Effectivity), 372);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionEffectivity), 373);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionRelationship), 374);

  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionWithAssociatedDocuments), 375);
  types.Bind (STANDARD_TYPE(StepBasic_PhysicallyModeledProductDefinition), 376);


  types.Bind (STANDARD_TYPE(StepRepr_ProductDefinitionUsage), 377);
  types.Bind (STANDARD_TYPE(StepRepr_MakeFromUsageOption), 378);
  types.Bind (STANDARD_TYPE(StepRepr_AssemblyComponentUsage), 379);
  types.Bind (STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence), 380);
  types.Bind (STANDARD_TYPE(StepRepr_PromissoryUsageOccurrence), 381);
  types.Bind (STANDARD_TYPE(StepRepr_QuantifiedAssemblyComponentUsage), 382);
  types.Bind (STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence), 383);
  types.Bind (STANDARD_TYPE(StepRepr_AssemblyComponentUsageSubstitute), 384);
  types.Bind (STANDARD_TYPE(StepRepr_SuppliedPartRelationship), 385);
  types.Bind (STANDARD_TYPE(StepRepr_ExternallyDefinedRepresentation), 386);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship), 387);
  types.Bind (STANDARD_TYPE(StepRepr_RepresentationRelationshipWithTransformation), 388);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeRepresentationRelationshipWithTransformation), 389);
  types.Bind (STANDARD_TYPE(StepRepr_MaterialDesignation),390);
  types.Bind (STANDARD_TYPE(StepShape_ContextDependentShapeRepresentation), 391);
  
  //Added from CD to DIS   :j4 
  
  types.Bind (STANDARD_TYPE(StepAP214_AppliedDateAndTimeAssignment), 392);  
  types.Bind (STANDARD_TYPE(StepAP214_AppliedDateAssignment), 393);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedApprovalAssignment), 394);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedGroupAssignment), 395);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedOrganizationAssignment), 396);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedPersonAndOrganizationAssignment), 397);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedPresentedItem), 398);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedSecurityClassificationAssignment), 399);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedDocumentReference), 400);
  
  types.Bind (STANDARD_TYPE(StepBasic_DocumentFile), 401);
  types.Bind (STANDARD_TYPE(StepBasic_CharacterizedObject), 402);
  types.Bind (STANDARD_TYPE(StepShape_ExtrudedFaceSolid), 403);
  types.Bind (STANDARD_TYPE(StepShape_RevolvedFaceSolid), 404);
  types.Bind (STANDARD_TYPE(StepShape_SweptFaceSolid), 405);

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  types.Bind (STANDARD_TYPE(StepRepr_MeasureRepresentationItem), 406);
  types.Bind (STANDARD_TYPE(StepBasic_AreaUnit), 407);
  types.Bind (STANDARD_TYPE(StepBasic_VolumeUnit), 408);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndAreaUnit), 409);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndVolumeUnit), 410);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndAreaUnit), 411);
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndVolumeUnit), 412);
  
  // Added by ABV 10.11.99 for AP203
  types.Bind (STANDARD_TYPE(StepBasic_Action), 413);
  types.Bind (STANDARD_TYPE(StepBasic_ActionAssignment), 414);
  types.Bind (STANDARD_TYPE(StepBasic_ActionMethod), 415);
  types.Bind (STANDARD_TYPE(StepBasic_ActionRequestAssignment), 416);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignApproval), 417);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignCertification), 418);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignContract), 419);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignDateAndTimeAssignment), 420);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignPersonAndOrganizationAssignment), 421);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignSecurityClassification), 422);
  types.Bind (STANDARD_TYPE(StepAP203_CcDesignSpecificationReference), 423);
  types.Bind (STANDARD_TYPE(StepBasic_Certification), 424);
  types.Bind (STANDARD_TYPE(StepBasic_CertificationAssignment), 425);
  types.Bind (STANDARD_TYPE(StepBasic_CertificationType), 426);
  types.Bind (STANDARD_TYPE(StepAP203_Change), 427);
  types.Bind (STANDARD_TYPE(StepAP203_ChangeRequest), 428);
  types.Bind (STANDARD_TYPE(StepRepr_ConfigurationDesign), 429);
  types.Bind (STANDARD_TYPE(StepRepr_ConfigurationEffectivity), 430);
  types.Bind (STANDARD_TYPE(StepBasic_Contract), 431);
  types.Bind (STANDARD_TYPE(StepBasic_ContractAssignment), 432);
  types.Bind (STANDARD_TYPE(StepBasic_ContractType), 433);
  types.Bind (STANDARD_TYPE(StepRepr_ProductConcept), 434);
  types.Bind (STANDARD_TYPE(StepBasic_ProductConceptContext), 435);
  types.Bind (STANDARD_TYPE(StepAP203_StartRequest), 436);
  types.Bind (STANDARD_TYPE(StepAP203_StartWork), 437);
  types.Bind (STANDARD_TYPE(StepBasic_VersionedActionRequest), 438);
  types.Bind (STANDARD_TYPE(StepBasic_ProductCategoryRelationship), 439);
  types.Bind (STANDARD_TYPE(StepBasic_ActionRequestSolution), 440);
  types.Bind (STANDARD_TYPE(StepVisual_DraughtingModel), 441);

  // Added by ABV 18.04.00 for CAX-IF TRJ4 (dimensional tolerances)
  types.Bind (STANDARD_TYPE(StepShape_AngularLocation), 442);
  types.Bind (STANDARD_TYPE(StepShape_AngularSize), 443);
  types.Bind (STANDARD_TYPE(StepShape_DimensionalCharacteristicRepresentation), 444);
  types.Bind (STANDARD_TYPE(StepShape_DimensionalLocation), 445);
  types.Bind (STANDARD_TYPE(StepShape_DimensionalLocationWithPath), 446);
  types.Bind (STANDARD_TYPE(StepShape_DimensionalSize), 447);
  types.Bind (STANDARD_TYPE(StepShape_DimensionalSizeWithPath), 448);
  types.Bind (STANDARD_TYPE(StepShape_ShapeDimensionRepresentation), 449);

  // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  types.Bind (STANDARD_TYPE(StepBasic_DocumentRepresentationType), 450);
  types.Bind (STANDARD_TYPE(StepBasic_ObjectRole), 451);
  types.Bind (STANDARD_TYPE(StepBasic_RoleAssociation), 452);
  types.Bind (STANDARD_TYPE(StepBasic_IdentificationRole), 453);
  types.Bind (STANDARD_TYPE(StepBasic_IdentificationAssignment), 454);
  types.Bind (STANDARD_TYPE(StepBasic_ExternalIdentificationAssignment), 455);
  types.Bind (STANDARD_TYPE(StepBasic_EffectivityAssignment), 456);
  types.Bind (STANDARD_TYPE(StepBasic_NameAssignment), 457);
  types.Bind (STANDARD_TYPE(StepBasic_GeneralProperty), 458);
  types.Bind (STANDARD_TYPE(StepAP214_Class), 459);
  types.Bind (STANDARD_TYPE(StepAP214_ExternallyDefinedClass), 460);
  types.Bind (STANDARD_TYPE(StepAP214_ExternallyDefinedGeneralProperty), 461);
  types.Bind (STANDARD_TYPE(StepAP214_AppliedExternalIdentificationAssignment), 462);

  // abv 11.07.00: CAX-IF TRJ4: k1_geo-ac.stp
  types.Bind (STANDARD_TYPE(StepShape_DefinitionalRepresentationAndShapeRepresentation), 463);

  // CKY 25 APR 2001 : CAX-IF TR7J , dimensional tolerances (contd)
  types.Bind (STANDARD_TYPE(StepRepr_CompositeShapeAspect),470);
  types.Bind (STANDARD_TYPE(StepRepr_DerivedShapeAspect),471);
  types.Bind (STANDARD_TYPE(StepRepr_Extension),472);
  types.Bind (STANDARD_TYPE(StepShape_DirectedDimensionalLocation),473);
  types.Bind (STANDARD_TYPE(StepShape_LimitsAndFits),474);
  types.Bind (STANDARD_TYPE(StepShape_ToleranceValue),475);
  types.Bind (STANDARD_TYPE(StepShape_MeasureQualification),476);
  types.Bind (STANDARD_TYPE(StepShape_PlusMinusTolerance),477);
  types.Bind (STANDARD_TYPE(StepShape_PrecisionQualifier),478);
  types.Bind (STANDARD_TYPE(StepShape_TypeQualifier),479);
  types.Bind (STANDARD_TYPE(StepShape_QualifiedRepresentationItem),480);
  types.Bind (STANDARD_TYPE(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem),481);
  types.Bind (STANDARD_TYPE(StepRepr_CompoundRepresentationItem),482);
  types.Bind (STANDARD_TYPE(StepRepr_ValueRange),483);
  types.Bind (STANDARD_TYPE(StepRepr_ShapeAspectDerivingRelationship),484);

  // abv 11.07.00: CAX-IF TRJ8: edge_based_wireframe
  types.Bind (STANDARD_TYPE(StepShape_CompoundShapeRepresentation),485);
  types.Bind (STANDARD_TYPE(StepShape_ConnectedEdgeSet),486);
  types.Bind (STANDARD_TYPE(StepShape_ConnectedFaceShapeRepresentation),487);
  types.Bind (STANDARD_TYPE(StepShape_EdgeBasedWireframeModel),488);
  types.Bind (STANDARD_TYPE(StepShape_EdgeBasedWireframeShapeRepresentation),489);
  types.Bind (STANDARD_TYPE(StepShape_FaceBasedSurfaceModel),490);
  types.Bind (STANDARD_TYPE(StepShape_NonManifoldSurfaceShapeRepresentation),491);
  
  //gka 08.01.02 TRJ9 IS->DIS
  types.Bind (STANDARD_TYPE(StepGeom_OrientedSurface),492);
  types.Bind (STANDARD_TYPE(StepShape_Subface),493);
  types.Bind (STANDARD_TYPE(StepShape_Subedge),494);
  types.Bind (STANDARD_TYPE(StepShape_SeamEdge),495);
  types.Bind (STANDARD_TYPE(StepShape_ConnectedFaceSubSet),496);
  
  //AP209 types
  types.Bind (STANDARD_TYPE(StepBasic_EulerAngles),500);
  types.Bind (STANDARD_TYPE(StepBasic_MassUnit),501);
  types.Bind (STANDARD_TYPE(StepBasic_ThermodynamicTemperatureUnit),502);
  types.Bind (STANDARD_TYPE(StepElement_AnalysisItemWithinRepresentation),503);
  types.Bind (STANDARD_TYPE(StepElement_Curve3dElementDescriptor),504);
  types.Bind (STANDARD_TYPE(StepElement_CurveElementEndReleasePacket),505);
  types.Bind (STANDARD_TYPE(StepElement_CurveElementSectionDefinition),506);
  types.Bind (STANDARD_TYPE(StepElement_CurveElementSectionDerivedDefinitions),507);
  types.Bind (STANDARD_TYPE(StepElement_ElementDescriptor),508);
  types.Bind (STANDARD_TYPE(StepElement_ElementMaterial),509);
  types.Bind (STANDARD_TYPE(StepElement_Surface3dElementDescriptor),510);
  types.Bind (STANDARD_TYPE(StepElement_SurfaceElementProperty),511);
  types.Bind (STANDARD_TYPE(StepElement_SurfaceSection),512);
  types.Bind (STANDARD_TYPE(StepElement_SurfaceSectionField),513);
  types.Bind (STANDARD_TYPE(StepElement_SurfaceSectionFieldConstant),514);
  types.Bind (STANDARD_TYPE(StepElement_SurfaceSectionFieldVarying),515);
  types.Bind (STANDARD_TYPE(StepElement_UniformSurfaceSection),516);
  types.Bind (STANDARD_TYPE(StepElement_Volume3dElementDescriptor),517);
  types.Bind (STANDARD_TYPE(StepFEA_AlignedCurve3dElementCoordinateSystem),518);
  types.Bind (STANDARD_TYPE(StepFEA_ArbitraryVolume3dElementCoordinateSystem),519);
  types.Bind (STANDARD_TYPE(StepFEA_Curve3dElementProperty),520);
  types.Bind (STANDARD_TYPE(StepFEA_Curve3dElementRepresentation),521);
  types.Bind (STANDARD_TYPE(StepFEA_Node),522);
//  types.Bind (STANDARD_TYPE(StepFEA_CurveElementEndCoordinateSystem),523);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementEndOffset),524);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementEndRelease),525);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementInterval),526);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementIntervalConstant),527);
  types.Bind (STANDARD_TYPE(StepFEA_DummyNode),528);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementLocation),529);
  types.Bind (STANDARD_TYPE(StepFEA_ElementGeometricRelationship),530);
  types.Bind (STANDARD_TYPE(StepFEA_ElementGroup),531);
  types.Bind (STANDARD_TYPE(StepFEA_ElementRepresentation),532);
  types.Bind (STANDARD_TYPE(StepFEA_FeaAreaDensity),533);
  types.Bind (STANDARD_TYPE(StepFEA_FeaAxis2Placement3d),534);
  types.Bind (STANDARD_TYPE(StepFEA_FeaGroup),535);
  types.Bind (STANDARD_TYPE(StepFEA_FeaLinearElasticity),536);
  types.Bind (STANDARD_TYPE(StepFEA_FeaMassDensity),537);
  types.Bind (STANDARD_TYPE(StepFEA_FeaMaterialPropertyRepresentation),538);
  types.Bind (STANDARD_TYPE(StepFEA_FeaMaterialPropertyRepresentationItem),539);
  types.Bind (STANDARD_TYPE(StepFEA_FeaModel),540);
  types.Bind (STANDARD_TYPE(StepFEA_FeaModel3d),541);
  types.Bind (STANDARD_TYPE(StepFEA_FeaMoistureAbsorption),542);
  types.Bind (STANDARD_TYPE(StepFEA_FeaParametricPoint),543);
  types.Bind (STANDARD_TYPE(StepFEA_FeaRepresentationItem),544);
  types.Bind (STANDARD_TYPE(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion),545);
  types.Bind (STANDARD_TYPE(StepFEA_FeaShellBendingStiffness),546);
  types.Bind (STANDARD_TYPE(StepFEA_FeaShellMembraneBendingCouplingStiffness),547);
  types.Bind (STANDARD_TYPE(StepFEA_FeaShellMembraneStiffness),548);
  types.Bind (STANDARD_TYPE(StepFEA_FeaShellShearStiffness),549);
  types.Bind (STANDARD_TYPE(StepFEA_GeometricNode),550);
  types.Bind (STANDARD_TYPE(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion),551);
  types.Bind (STANDARD_TYPE(StepFEA_NodeGroup),552);
  types.Bind (STANDARD_TYPE(StepFEA_NodeRepresentation),553);
  types.Bind (STANDARD_TYPE(StepFEA_NodeSet),554);
  types.Bind (STANDARD_TYPE(StepFEA_NodeWithSolutionCoordinateSystem),555);
  types.Bind (STANDARD_TYPE(StepFEA_NodeWithVector),556);
  types.Bind (STANDARD_TYPE(StepFEA_ParametricCurve3dElementCoordinateDirection),557);
  types.Bind (STANDARD_TYPE(StepFEA_ParametricCurve3dElementCoordinateSystem),558);
  types.Bind (STANDARD_TYPE(StepFEA_ParametricSurface3dElementCoordinateSystem),559);
  types.Bind (STANDARD_TYPE(StepFEA_Surface3dElementRepresentation),560);
//  types.Bind (STANDARD_TYPE(StepFEA_SymmetricTensor22d),561);
//  types.Bind (STANDARD_TYPE(StepFEA_SymmetricTensor42d),562);
//  types.Bind (STANDARD_TYPE(StepFEA_SymmetricTensor43d),563);
  types.Bind (STANDARD_TYPE(StepFEA_Volume3dElementRepresentation),564);
  types.Bind (STANDARD_TYPE(StepRepr_DataEnvironment),565);
  types.Bind (STANDARD_TYPE(StepRepr_MaterialPropertyRepresentation),566);
  types.Bind (STANDARD_TYPE(StepRepr_PropertyDefinitionRelationship),567);
  types.Bind (STANDARD_TYPE(StepShape_PointRepresentation),568);
  types.Bind (STANDARD_TYPE(StepRepr_MaterialProperty),569);
  types.Bind (STANDARD_TYPE(StepFEA_FeaModelDefinition),570);
  types.Bind (STANDARD_TYPE(StepFEA_FreedomAndCoefficient),571);
  types.Bind (STANDARD_TYPE(StepFEA_FreedomsList),572);
  types.Bind (STANDARD_TYPE(StepBasic_ProductDefinitionFormationRelationship),573);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndMassUnit),574);
  types.Bind (STANDARD_TYPE(StepFEA_NodeDefinition),575);
  types.Bind (STANDARD_TYPE(StepRepr_StructuralResponseProperty),576);
  types.Bind (STANDARD_TYPE(StepRepr_StructuralResponsePropertyDefinitionRepresentation),577);
  types.Bind (STANDARD_TYPE(StepBasic_SiUnitAndThermodynamicTemperatureUnit), 578);
  types.Bind (STANDARD_TYPE(StepFEA_AlignedSurface3dElementCoordinateSystem),579);
  types.Bind (STANDARD_TYPE(StepFEA_ConstantSurface3dElementCoordinateSystem),580);
  types.Bind (STANDARD_TYPE(StepFEA_CurveElementIntervalLinearlyVarying),581);
  types.Bind (STANDARD_TYPE(StepFEA_FeaCurveSectionGeometricRelationship),582);
  types.Bind (STANDARD_TYPE(StepFEA_FeaSurfaceSectionGeometricRelationship),583);
  
   // PTV 28.01.2003 TRJ11 AP214 external references
  types.Bind (STANDARD_TYPE(StepBasic_DocumentProductAssociation),600);
  types.Bind (STANDARD_TYPE(StepBasic_DocumentProductEquivalence),601);
   
  //TR12J 4.06.2003 G&DT entities
  types.Bind (STANDARD_TYPE(StepDimTol_CylindricityTolerance), 609);
  types.Bind (STANDARD_TYPE(StepShape_ShapeRepresentationWithParameters),610);
  types.Bind (STANDARD_TYPE(StepDimTol_AngularityTolerance),611);
  types.Bind (STANDARD_TYPE(StepDimTol_ConcentricityTolerance),612);
  types.Bind (STANDARD_TYPE(StepDimTol_CircularRunoutTolerance),613);
  types.Bind (STANDARD_TYPE(StepDimTol_CoaxialityTolerance),614);
  types.Bind (STANDARD_TYPE(StepDimTol_FlatnessTolerance),615);
  types.Bind (STANDARD_TYPE(StepDimTol_LineProfileTolerance),616);
  types.Bind (STANDARD_TYPE(StepDimTol_ParallelismTolerance),617);
  types.Bind (STANDARD_TYPE(StepDimTol_PerpendicularityTolerance),618);
  types.Bind (STANDARD_TYPE(StepDimTol_PositionTolerance),619);
  types.Bind (STANDARD_TYPE(StepDimTol_RoundnessTolerance),620);
  types.Bind (STANDARD_TYPE(StepDimTol_StraightnessTolerance),621);
  types.Bind (STANDARD_TYPE(StepDimTol_SurfaceProfileTolerance),622);
  types.Bind (STANDARD_TYPE(StepDimTol_SymmetryTolerance),623);
  types.Bind (STANDARD_TYPE(StepDimTol_TotalRunoutTolerance),624);
  
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricTolerance),625);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceRelationship),626);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceWithDatumReference),627);
  types.Bind (STANDARD_TYPE(StepDimTol_ModifiedGeometricTolerance),628);
  
  types.Bind (STANDARD_TYPE(StepDimTol_Datum),629);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumFeature),630);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumReference),631);
  types.Bind (STANDARD_TYPE(StepDimTol_CommonDatum),632);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumTarget),633);
  types.Bind (STANDARD_TYPE(StepDimTol_PlacedDatumTargetFeature),634);

  types.Bind (STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnit),635);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol),636);

  // added by skl 10.02.2004 for TRJ13
  types.Bind (STANDARD_TYPE(StepBasic_ConversionBasedUnitAndMassUnit),650);
  types.Bind (STANDARD_TYPE(StepBasic_MassMeasureWithUnit), 651);

  // Added by ika for GD&T AP242
  types.Bind (STANDARD_TYPE(StepRepr_Apex), 660);
  types.Bind (STANDARD_TYPE(StepRepr_CentreOfSymmetry), 661);
  types.Bind (STANDARD_TYPE(StepRepr_GeometricAlignment), 662);
  types.Bind (STANDARD_TYPE(StepRepr_PerpendicularTo), 663);
  types.Bind (STANDARD_TYPE(StepRepr_Tangent), 664);
  types.Bind (STANDARD_TYPE(StepRepr_ParallelOffset), 665);
  types.Bind (STANDARD_TYPE(StepAP242_GeometricItemSpecificUsage), 666);
  types.Bind (STANDARD_TYPE(StepAP242_IdAttribute), 667);
  types.Bind (STANDARD_TYPE(StepAP242_ItemIdentifiedRepresentationUsage), 668);
  types.Bind (STANDARD_TYPE(StepRepr_AllAroundShapeAspect), 669);
  types.Bind (STANDARD_TYPE(StepRepr_BetweenShapeAspect), 670);
  types.Bind (STANDARD_TYPE(StepRepr_CompositeGroupShapeAspect), 671);
  types.Bind (STANDARD_TYPE(StepRepr_ContinuosShapeAspect), 672);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceWithDefinedAreaUnit), 673);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceWithDefinedUnit), 674);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceWithMaximumTolerance), 675);
  types.Bind (STANDARD_TYPE(StepDimTol_GeometricToleranceWithModifiers), 676);
  types.Bind (STANDARD_TYPE(StepDimTol_UnequallyDisposedGeometricTolerance), 677);
  types.Bind (STANDARD_TYPE(StepDimTol_NonUniformZoneDefinition), 678);
  types.Bind (STANDARD_TYPE(StepDimTol_ProjectedZoneDefinition), 679);
  types.Bind (STANDARD_TYPE(StepDimTol_RunoutZoneDefinition), 680);
  types.Bind (STANDARD_TYPE(StepDimTol_RunoutZoneOrientation), 681);
  types.Bind (STANDARD_TYPE(StepDimTol_ToleranceZone), 682);
  types.Bind (STANDARD_TYPE(StepDimTol_ToleranceZoneDefinition), 683);
  types.Bind (STANDARD_TYPE(StepDimTol_ToleranceZoneForm), 684);
  types.Bind (STANDARD_TYPE(StepShape_ValueFormatTypeQualifier), 685);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumReferenceCompartment), 686);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumReferenceElement), 687);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumReferenceModifierWithValue), 688);
  types.Bind (STANDARD_TYPE(StepDimTol_DatumSystem), 689);
  types.Bind (STANDARD_TYPE(StepDimTol_GeneralDatumReference), 690);
  types.Bind (STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit), 691);
  types.Bind (STANDARD_TYPE(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI), 692);
  types.Bind (STANDARD_TYPE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI), 693);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRef), 694);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod), 695);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMod), 696);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol), 697);
  types.Bind (STANDARD_TYPE(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp), 698);
  types.Bind (STANDARD_TYPE(StepRepr_CompShAspAndDatumFeatAndShAsp), 699);
  types.Bind (STANDARD_TYPE(StepRepr_IntegerRepresentationItem), 700);
  types.Bind (STANDARD_TYPE(StepRepr_ValueRepresentationItem), 701);
  types.Bind (STANDARD_TYPE(StepRepr_FeatureForDatumTargetRelationship), 702);
  types.Bind (STANDARD_TYPE(StepAP242_DraughtingModelItemAssociation), 703);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationPlane), 704);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol), 705);
  types.Bind (STANDARD_TYPE(StepDimTol_GeoTolAndGeoTolWthMaxTol), 706);
  //AP242 tesselated
  types.Bind (STANDARD_TYPE(StepVisual_TessellatedAnnotationOccurrence), 707);
  types.Bind (STANDARD_TYPE(StepVisual_TessellatedItem), 708);
  types.Bind (STANDARD_TYPE(StepVisual_TessellatedGeometricSet), 709);
  types.Bind (STANDARD_TYPE(StepVisual_TessellatedCurveSet), 710);
  types.Bind (STANDARD_TYPE(StepVisual_CoordinatesList), 711);
  types.Bind (STANDARD_TYPE(StepRepr_ConstructiveGeometryRepresentation), 712);
  types.Bind (STANDARD_TYPE(StepRepr_ConstructiveGeometryRepresentationRelationship), 713);
  types.Bind (STANDARD_TYPE(StepRepr_CharacterizedRepresentation), 714);
  types.Bind (STANDARD_TYPE(StepVisual_CharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation), 715);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModelD3MultiClipping), 716);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingIntersection), 717);
  types.Bind (STANDARD_TYPE(StepVisual_CameraModelD3MultiClippingUnion), 718);
  types.Bind (STANDARD_TYPE(StepVisual_AnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem), 719);
}



//=======================================================================
//function : TypeNumber
//purpose  : 
//=======================================================================

Standard_Integer StepAP214_Protocol::TypeNumber(const 
Handle(Standard_Type)& atype) const
{
  if (types.IsBound (atype)) return types.Find(atype);
  else return 0;
}


//=======================================================================
//function : SchemaName
//purpose  : 
//=======================================================================

Standard_CString StepAP214_Protocol::SchemaName() const
{	
  switch (Interface_Static::IVal("write.step.schema")) { //:j4
  default:
  case 1 : return schemaAP214CD;  break; 
  case 2 : return schemaAP214DIS; break; 
  case 3 : return schemaAP203;    break;
  case 4:  return schemaAP214IS; break;
  case 5 : return schemaAP242DIS; break;
  }
}


//=======================================================================
//function : NbResources
//purpose  : 
//=======================================================================

Standard_Integer StepAP214_Protocol::NbResources () const
{
  return 1;  
}


//=======================================================================
//function : Resource
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) StepAP214_Protocol::Resource
  (const Standard_Integer /*num*/) const
{
  return HeaderSection::Protocol();
}

