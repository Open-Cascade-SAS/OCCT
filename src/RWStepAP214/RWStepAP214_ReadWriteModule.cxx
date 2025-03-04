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

// pdn 24.12.98 t3d_opt.stp: treatment of unsorted uncertainties
//: n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve -> surface_curve
// :j4 gka 15.03.99 S4134
// sln 03.10.2001. BUC61003. Correction of alphabetic order of complex entity's items

#include <Interface_Check.hxx>
#include <Interface_ParamType.hxx>
#include <Interface_ReaderLib.hxx>
#include "RWStepAP214_ReadWriteModule.pxx"
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_WriterLib.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWStepAP214_ReadWriteModule, StepData_ReadWriteModule)

#include <MoniTool_Macros.hxx>

#include <StepBasic_Address.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_AdvancedFace.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationCurveOccurrence.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationFillArea.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationFillAreaOccurrence.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_AnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationSubfigureOccurrence.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationSymbol.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_AnnotationSymbolOccurrence.hxx>
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
#include <StepBasic_AreaUnit.hxx>
#include <StepVisual_AreaInSet.hxx>
#include <StepBasic_VolumeUnit.hxx>
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
// Removed from Rev2 to Rev4 : <StepAP214_AutoDesignViewArea.hxx>
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
// Removed from Rev2 to Rev4 : <StepVisual_CompositeTextWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_CompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_CompositeTextWithExtent.hxx>

#include <StepGeom_Conic.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepVisual_ContextDependentInvisibility.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
// Removed from Rev2 to Rev4 : <StepShape_CsgRepresentation.hxx>
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
// Removed from Rev2 to Rev4 : <StepVisual_DefinedSymbol.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_DegenerateToroidalSurface.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DimensionCurve.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DimensionCurveTerminator.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepGeom_Direction.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_DraughtingAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DraughtingCallout.hxx>
#include <StepVisual_DraughtingPreDefinedColour.hxx>
#include <StepVisual_DraughtingPreDefinedCurveFont.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DraughtingSubfigureRepresentation.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DraughtingSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DraughtingTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DrawingDefinition.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_DrawingRevision.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_EvaluatedDegeneratePcurve.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepVisual_ExternallyDefinedCurveFont.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_ExternallyDefinedHatchStyle.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_ExternallyDefinedSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_ExternallyDefinedTextFont.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_ExternallyDefinedTileStyle.hxx>
#include <StepShape_ExtrudedAreaSolid.hxx>
#include <StepShape_Face.hxx>
// Removed from Rev2 to Rev4 : <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_FaceOuterBound.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_FillAreaStyleColour.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_FillAreaStyleHatching.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_FillAreaStyleTileSymbolWithStyle.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_FillAreaStyleTiles.hxx>
#include <StepRepr_FunctionallyDefinedTransformation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepBasic_GeneralPropertyAssociation.hxx>
#include <StepBasic_GeneralPropertyRelationship.hxx>
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
// Removed from Rev2 to Rev4 : <StepVisual_MechanicalDesignPresentationArea.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <StepGeom_OffsetSurface.hxx>
// Removed from Rev2 to Rev4 : <StepAP214_OneDirectionRepeatFactor.hxx>
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
// Removed from Rev2 to Rev4 : <StepVisual_PreDefinedSymbol.hxx>
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
#include <StepVisual_RepositionedTessellatedGeometricSet.hxx>
#include <StepVisual_RepositionedTessellatedItem.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductContext.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_ProductDataRepresentationView.hxx>
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
// Removed from Rev2 to Rev4 : <StepVisual_SymbolColour.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_SymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_SymbolRepresentationMap.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_SymbolStyle.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_SymbolTarget.hxx>
#include <StepVisual_Template.hxx>
#include <StepVisual_TemplateInstance.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_TextLiteral.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TextLiteralWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TextLiteralWithBlankingBox.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <StepVisual_TextStyle.hxx>
#include <StepVisual_TextStyleForDefinedFont.hxx>
#include <StepVisual_TextStyleWithBoxCharacteristics.hxx>
// Removed from Rev2 to Rev4 : <StepVisual_TextStyleWithMirror.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepShape_Torus.hxx>
#include <StepShape_TransitionalShapeRepresentation.hxx>
#include <StepGeom_TrimmedCurve.hxx>
// Removed from Rev2 to Rev4 : <StepAP214_TwoDirectionRepeatFactor.hxx>
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

// Added by FMA
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepBasic_MechanicalContext.hxx>

// full Rev4
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

#include <StepVisual_SurfaceStyleTransparent.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>
#include <StepVisual_SurfaceStyleRendering.hxx>
#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>

#include <StepVisual_TessellatedConnectingEdge.hxx>
#include <StepVisual_TessellatedEdge.hxx>
#include <StepVisual_TessellatedPointSet.hxx>
#include <StepVisual_TessellatedShapeRepresentation.hxx>
#include <StepVisual_TessellatedShapeRepresentationWithAccuracyParameters.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>
#include <StepVisual_TessellatedVertex.hxx>
#include <StepVisual_TessellatedWire.hxx>
#include <StepVisual_TriangulatedFace.hxx>
#include <StepVisual_ComplexTriangulatedFace.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
#include <StepVisual_CubicBezierTessellatedEdge.hxx>
#include <StepVisual_CubicBezierTriangulatedFace.hxx>
#include <StepVisual_TriangulatedSurfaceSet.hxx>

#include "../RWStepVisual/RWStepVisual_RWTessellatedConnectingEdge.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedEdge.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedPointSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedShapeRepresentation.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedShell.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedSolid.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedStructuredItem.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedVertex.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedWire.pxx"
#include "../RWStepVisual/RWStepVisual_RWTriangulatedFace.pxx"
#include "../RWStepVisual/RWStepVisual_RWComplexTriangulatedFace.pxx"
#include "../RWStepVisual/RWStepVisual_RWComplexTriangulatedSurfaceSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWCubicBezierTessellatedEdge.pxx"
#include "../RWStepVisual/RWStepVisual_RWCubicBezierTriangulatedFace.pxx"
#include "../RWStepVisual/RWStepVisual_RWTriangulatedSurfaceSet.pxx"

#include "../RWStepBasic/RWStepBasic_RWAddress.pxx"
#include "../RWStepShape/RWStepShape_RWAdvancedBrepShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWAdvancedFace.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationCurveOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationFillArea.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationFillAreaOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSubfigureOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSymbolOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationText.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationTextOccurrence.hxx>
#include "../RWStepBasic/RWStepBasic_RWApplicationContext.pxx"
#include "../RWStepBasic/RWStepBasic_RWApplicationContextElement.pxx"
#include "../RWStepBasic/RWStepBasic_RWApplicationProtocolDefinition.pxx"
#include "../RWStepBasic/RWStepBasic_RWApproval.pxx"
#include "../RWStepBasic/RWStepBasic_RWApprovalPersonOrganization.pxx"
#include "../RWStepBasic/RWStepBasic_RWApprovalRelationship.pxx"
#include "../RWStepBasic/RWStepBasic_RWApprovalRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWApprovalStatus.pxx"
#include "../RWStepVisual/RWStepVisual_RWAreaInSet.pxx"
#include "RWStepAP214_RWAutoDesignActualDateAndTimeAssignment.pxx"
#include "RWStepAP214_RWAutoDesignActualDateAssignment.pxx"
#include "RWStepAP214_RWAutoDesignApprovalAssignment.pxx"
#include "RWStepAP214_RWAutoDesignDateAndPersonAssignment.pxx"
#include "RWStepAP214_RWAutoDesignGroupAssignment.pxx"
#include "RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment.pxx"
#include "RWStepAP214_RWAutoDesignNominalDateAssignment.pxx"
#include "RWStepAP214_RWAutoDesignOrganizationAssignment.pxx"
#include "RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment.pxx"
#include "RWStepAP214_RWAutoDesignPresentedItem.pxx"
#include "RWStepAP214_RWAutoDesignSecurityClassificationAssignment.pxx"
// Removed from Rev2 to Rev4 : <RWStepAP214_RWAutoDesignViewArea.hxx>
#include "../RWStepGeom/RWStepGeom_RWAxis1Placement.pxx"
#include "../RWStepGeom/RWStepGeom_RWAxis2Placement2d.pxx"
#include "../RWStepGeom/RWStepGeom_RWAxis2Placement3d.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineCurveWithKnots.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineSurfaceWithKnots.pxx"
#include "../RWStepVisual/RWStepVisual_RWBackgroundColour.pxx"
#include "../RWStepGeom/RWStepGeom_RWBezierCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBezierSurface.pxx"
#include "../RWStepShape/RWStepShape_RWBlock.pxx"
#include "../RWStepShape/RWStepShape_RWBooleanResult.pxx"
#include "../RWStepGeom/RWStepGeom_RWBoundaryCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBoundedCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBoundedSurface.pxx"
#include "../RWStepShape/RWStepShape_RWBoxDomain.pxx"
#include "../RWStepShape/RWStepShape_RWBoxedHalfSpace.pxx"
#include "../RWStepShape/RWStepShape_RWBrepWithVoids.pxx"
#include "../RWStepBasic/RWStepBasic_RWCalendarDate.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraImage.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraModel.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraModelD2.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraModelD3.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraUsage.pxx"
#include "../RWStepGeom/RWStepGeom_RWCartesianPoint.pxx"
#include "../RWStepGeom/RWStepGeom_RWCartesianTransformationOperator.pxx"
#include "../RWStepGeom/RWStepGeom_RWCartesianTransformationOperator3d.pxx"
#include "../RWStepGeom/RWStepGeom_RWCircle.pxx"
#include "../RWStepShape/RWStepShape_RWClosedShell.pxx"
#include "../RWStepVisual/RWStepVisual_RWColour.pxx"
#include "../RWStepVisual/RWStepVisual_RWColourRgb.pxx"
#include "../RWStepVisual/RWStepVisual_RWColourSpecification.pxx"
#include "../RWStepGeom/RWStepGeom_RWCompositeCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWCompositeCurveOnSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWCompositeCurveSegment.pxx"
#include "../RWStepVisual/RWStepVisual_RWCompositeText.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWCompositeTextWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWCompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include "../RWStepVisual/RWStepVisual_RWCompositeTextWithExtent.pxx"

#include "../RWStepGeom/RWStepGeom_RWConic.pxx"
#include "../RWStepGeom/RWStepGeom_RWConicalSurface.pxx"
#include "../RWStepShape/RWStepShape_RWConnectedFaceSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWContextDependentInvisibility.pxx"
#include "../RWStepVisual/RWStepVisual_RWContextDependentOverRidingStyledItem.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWCoordinatedUniversalTimeOffset.pxx"
// Removed from Rev2 to Rev4 : <RWStepShape_RWCsgRepresentation.hxx>
#include "../RWStepShape/RWStepShape_RWCsgShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWCsgSolid.pxx"
#include "../RWStepGeom/RWStepGeom_RWCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWCurveBoundedSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWCurveReplica.pxx"
#include "../RWStepVisual/RWStepVisual_RWCurveStyle.pxx"
#include "../RWStepVisual/RWStepVisual_RWCurveStyleFont.pxx"
#include "../RWStepVisual/RWStepVisual_RWCurveStyleFontPattern.pxx"
#include "../RWStepGeom/RWStepGeom_RWCylindricalSurface.pxx"
#include "../RWStepBasic/RWStepBasic_RWDate.pxx"
#include "../RWStepBasic/RWStepBasic_RWDateAndTime.pxx"
#include "../RWStepBasic/RWStepBasic_RWDateRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWDateTimeRole.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDefinedSymbol.hxx>
#include "../RWStepRepr/RWStepRepr_RWDefinitionalRepresentation.pxx"
#include "../RWStepGeom/RWStepGeom_RWDegeneratePcurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWDegenerateToroidalSurface.pxx"
#include "../RWStepRepr/RWStepRepr_RWDescriptiveRepresentationItem.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDimensionCurve.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDimensionCurveTerminator.hxx>
#include "../RWStepBasic/RWStepBasic_RWDimensionalExponents.pxx"
#include "../RWStepGeom/RWStepGeom_RWDirection.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingCallout.hxx>
#include "../RWStepVisual/RWStepVisual_RWDraughtingPreDefinedColour.pxx"
#include "../RWStepVisual/RWStepVisual_RWDraughtingPreDefinedCurveFont.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingSubfigureRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDrawingDefinition.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDrawingRevision.hxx>
#include "../RWStepShape/RWStepShape_RWEdge.pxx"
#include "../RWStepShape/RWStepShape_RWEdgeCurve.pxx"
#include "../RWStepShape/RWStepShape_RWEdgeLoop.pxx"
#include "../RWStepGeom/RWStepGeom_RWElementarySurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWEllipse.pxx"
#include "../RWStepGeom/RWStepGeom_RWEvaluatedDegeneratePcurve.pxx"
#include "../RWStepBasic/RWStepBasic_RWExternalSource.pxx"
#include "../RWStepVisual/RWStepVisual_RWExternallyDefinedCurveFont.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedHatchStyle.hxx>
#include "../RWStepBasic/RWStepBasic_RWExternallyDefinedItem.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedTextFont.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedTileStyle.hxx>
#include "../RWStepShape/RWStepShape_RWExtrudedAreaSolid.pxx"
#include "../RWStepShape/RWStepShape_RWFace.pxx"
// Removed from Rev2 to Rev4 : <RWStepShape_RWFaceBasedSurfaceModel.hxx>
#include "../RWStepShape/RWStepShape_RWFaceBound.pxx"
#include "../RWStepShape/RWStepShape_RWFaceOuterBound.pxx"
#include "../RWStepShape/RWStepShape_RWFaceSurface.pxx"
#include "../RWStepShape/RWStepShape_RWFacetedBrep.pxx"
#include "../RWStepShape/RWStepShape_RWFacetedBrepShapeRepresentation.pxx"
#include "../RWStepVisual/RWStepVisual_RWFillAreaStyle.pxx"
#include "../RWStepVisual/RWStepVisual_RWFillAreaStyleColour.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleHatching.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleTileSymbolWithStyle.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleTiles.hxx>
#include "../RWStepRepr/RWStepRepr_RWFunctionallyDefinedTransformation.pxx"
#include "../RWStepShape/RWStepShape_RWGeometricCurveSet.pxx"
#include "../RWStepGeom/RWStepGeom_RWGeometricRepresentationContext.pxx"
#include "../RWStepGeom/RWStepGeom_RWGeometricRepresentationItem.pxx"
#include "../RWStepShape/RWStepShape_RWGeometricSet.pxx"
#include "../RWStepShape/RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWGlobalUncertaintyAssignedContext.pxx"
#include "../RWStepRepr/RWStepRepr_RWGlobalUnitAssignedContext.pxx"
#include "../RWStepBasic/RWStepBasic_RWGeneralPropertyAssociation.pxx"
#include "../RWStepBasic/RWStepBasic_RWGeneralPropertyRelationship.pxx"
#include "../RWStepBasic/RWStepBasic_RWGroup.pxx"
#include "../RWStepBasic/RWStepBasic_RWGroupRelationship.pxx"
#include "../RWStepShape/RWStepShape_RWHalfSpaceSolid.pxx"
#include "../RWStepGeom/RWStepGeom_RWHyperbola.pxx"
#include "../RWStepGeom/RWStepGeom_RWIntersectionCurve.pxx"
#include "../RWStepVisual/RWStepVisual_RWInvisibility.pxx"
#include "../RWStepBasic/RWStepBasic_RWLengthMeasureWithUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWLengthUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWLine.pxx"
#include "../RWStepBasic/RWStepBasic_RWLocalTime.pxx"
#include "../RWStepShape/RWStepShape_RWLoop.pxx"
#include "../RWStepShape/RWStepShape_RWManifoldSolidBrep.pxx"
#include "../RWStepShape/RWStepShape_RWManifoldSurfaceShapeRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWMappedItem.pxx"
#include "../RWStepBasic/RWStepBasic_RWMeasureWithUnit.pxx"
#include "../RWStepRepr/RWStepRepr_RWMechanicalDesignAndDraughtingRelationship.pxx"
#include "../RWStepVisual/RWStepVisual_RWMechanicalDesignGeometricPresentationArea.pxx"
#include "../RWStepVisual/RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWMechanicalDesignPresentationArea.hxx>
#include "../RWStepBasic/RWStepBasic_RWNamedUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWOffsetCurve3d.pxx"
#include "../RWStepGeom/RWStepGeom_RWOffsetSurface.pxx"
// Removed from Rev2 to Rev4 : <RWStepAP214_RWOneDirectionRepeatFactor.hxx>
#include "../RWStepShape/RWStepShape_RWOpenShell.pxx"
#include "../RWStepBasic/RWStepBasic_RWOrdinalDate.pxx"
#include "../RWStepBasic/RWStepBasic_RWOrganization.pxx"
#include "../RWStepBasic/RWStepBasic_RWOrganizationRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWOrganizationalAddress.pxx"
#include "../RWStepShape/RWStepShape_RWOrientedClosedShell.pxx"
#include "../RWStepShape/RWStepShape_RWOrientedEdge.pxx"
#include "../RWStepShape/RWStepShape_RWOrientedFace.pxx"
#include "../RWStepShape/RWStepShape_RWOrientedOpenShell.pxx"
#include "../RWStepShape/RWStepShape_RWOrientedPath.pxx"
#include "../RWStepGeom/RWStepGeom_RWOuterBoundaryCurve.pxx"
#include "../RWStepVisual/RWStepVisual_RWOverRidingStyledItem.pxx"
#include "../RWStepGeom/RWStepGeom_RWParabola.pxx"
#include "../RWStepRepr/RWStepRepr_RWParametricRepresentationContext.pxx"
#include "../RWStepShape/RWStepShape_RWPath.pxx"
#include "../RWStepGeom/RWStepGeom_RWPcurve.pxx"
#include "../RWStepBasic/RWStepBasic_RWPerson.pxx"
#include "../RWStepBasic/RWStepBasic_RWPersonAndOrganization.pxx"
#include "../RWStepBasic/RWStepBasic_RWPersonAndOrganizationRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWPersonalAddress.pxx"
#include "../RWStepGeom/RWStepGeom_RWPlacement.pxx"
#include "../RWStepVisual/RWStepVisual_RWPlanarBox.pxx"
#include "../RWStepVisual/RWStepVisual_RWPlanarExtent.pxx"
#include "../RWStepGeom/RWStepGeom_RWPlane.pxx"
#include "../RWStepBasic/RWStepBasic_RWPlaneAngleMeasureWithUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWPlaneAngleUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWPoint.pxx"
#include "../RWStepGeom/RWStepGeom_RWPointOnCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWPointOnSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWPointReplica.pxx"
#include "../RWStepVisual/RWStepVisual_RWPointStyle.pxx"
#include "../RWStepShape/RWStepShape_RWPolyLoop.pxx"
#include "../RWStepGeom/RWStepGeom_RWPolyline.pxx"
#include "../RWStepVisual/RWStepVisual_RWPreDefinedColour.pxx"
#include "../RWStepVisual/RWStepVisual_RWPreDefinedCurveFont.pxx"
#include "../RWStepVisual/RWStepVisual_RWPreDefinedItem.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWPreDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWPreDefinedTextFont.hxx>
#include "../RWStepVisual/RWStepVisual_RWPresentationArea.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationLayerAssignment.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationRepresentation.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationSize.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationStyleAssignment.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationStyleByContext.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationView.pxx"
#include "../RWStepVisual/RWStepVisual_RWRepositionedTessellatedGeometricSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWRepositionedTessellatedItem.pxx"
#include "../RWStepBasic/RWStepBasic_RWProduct.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductCategory.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductContext.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWProductDataRepresentationView.hxx>
#include "../RWStepBasic/RWStepBasic_RWProductDefinition.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionContext.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionFormation.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource.pxx"
#include "../RWStepRepr/RWStepRepr_RWProductDefinitionShape.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductRelatedProductCategory.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductType.pxx"
#include "../RWStepRepr/RWStepRepr_RWPropertyDefinition.pxx"
#include "../RWStepRepr/RWStepRepr_RWPropertyDefinitionRepresentation.pxx"
#include "../RWStepGeom/RWStepGeom_RWQuasiUniformCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWQuasiUniformSurface.pxx"
#include "../RWStepBasic/RWStepBasic_RWRatioMeasureWithUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWRationalBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWRationalBSplineSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWRectangularCompositeSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWRectangularTrimmedSurface.pxx"
#include "RWStepAP214_RWRepItemGroup.pxx"
#include "../RWStepGeom/RWStepGeom_RWReparametrisedCompositeCurveSegment.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationContext.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationMap.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationRelationship.pxx"
#include "../RWStepShape/RWStepShape_RWRevolvedAreaSolid.pxx"
#include "../RWStepShape/RWStepShape_RWRightAngularWedge.pxx"
#include "../RWStepShape/RWStepShape_RWRightCircularCone.pxx"
#include "../RWStepShape/RWStepShape_RWRightCircularCylinder.pxx"
#include "../RWStepGeom/RWStepGeom_RWSeamCurve.pxx"
#include "../RWStepBasic/RWStepBasic_RWSecurityClassification.pxx"
#include "../RWStepBasic/RWStepBasic_RWSecurityClassificationLevel.pxx"
#include "../RWStepRepr/RWStepRepr_RWShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWShapeAspectRelationship.pxx"
#include "../RWStepRepr/RWStepRepr_RWFeatureForDatumTargetRelationship.pxx"
#include "../RWStepRepr/RWStepRepr_RWShapeAspectTransition.pxx"
#include "../RWStepShape/RWStepShape_RWShapeDefinitionRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWShellBasedSurfaceModel.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSolidAngleMeasureWithUnit.pxx"
#include "../RWStepShape/RWStepShape_RWSolidModel.pxx"
#include "../RWStepShape/RWStepShape_RWSolidReplica.pxx"
#include "../RWStepShape/RWStepShape_RWSphere.pxx"
#include "../RWStepGeom/RWStepGeom_RWSphericalSurface.pxx"
#include "../RWStepVisual/RWStepVisual_RWStyledItem.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfaceCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfaceOfLinearExtrusion.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfaceOfRevolution.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfaceCurveAndBoundedCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfacePatch.pxx"
#include "../RWStepGeom/RWStepGeom_RWSurfaceReplica.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceSideStyle.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleBoundary.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleControlGrid.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleFillArea.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleParameterLine.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleSegmentationCurve.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleSilhouette.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleUsage.pxx"
#include "../RWStepShape/RWStepShape_RWSweptAreaSolid.pxx"
#include "../RWStepGeom/RWStepGeom_RWSweptSurface.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolColour.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolRepresentationMap.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolStyle.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolTarget.hxx>
#include "../RWStepVisual/RWStepVisual_RWTemplate.pxx"
#include "../RWStepVisual/RWStepVisual_RWTemplateInstance.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include "../RWStepVisual/RWStepVisual_RWTextLiteral.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithBlankingBox.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include "../RWStepVisual/RWStepVisual_RWTextStyle.pxx"
#include "../RWStepVisual/RWStepVisual_RWTextStyleForDefinedFont.pxx"
#include "../RWStepVisual/RWStepVisual_RWTextStyleWithBoxCharacteristics.pxx"
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextStyleWithMirror.hxx>
#include "../RWStepShape/RWStepShape_RWTopologicalRepresentationItem.pxx"
#include "../RWStepGeom/RWStepGeom_RWToroidalSurface.pxx"
#include "../RWStepShape/RWStepShape_RWTorus.pxx"
#include "../RWStepShape/RWStepShape_RWTransitionalShapeRepresentation.pxx"
#include "../RWStepGeom/RWStepGeom_RWTrimmedCurve.pxx"
// Removed from Rev2 to Rev4 : <RWStepAP214_RWTwoDirectionRepeatFactor.hxx>
#include "../RWStepBasic/RWStepBasic_RWUncertaintyMeasureWithUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWUniformCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWUniformSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWVector.pxx"
#include "../RWStepShape/RWStepShape_RWVertex.pxx"
#include "../RWStepShape/RWStepShape_RWVertexLoop.pxx"
#include "../RWStepShape/RWStepShape_RWVertexPoint.pxx"
#include "../RWStepVisual/RWStepVisual_RWViewVolume.pxx"
#include "../RWStepBasic/RWStepBasic_RWWeekOfYearAndDayDate.pxx"
#include "../RWStepGeom/RWStepGeom_RWUniformCurveAndRationalBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBezierCurveAndRationalBSplineCurve.pxx"
#include "../RWStepGeom/RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface.pxx"
#include "../RWStepGeom/RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndLengthUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndPlaneAngleUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndLengthUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit.pxx"
#include "../RWStepGeom/RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext.pxx"
#include "../RWStepShape/RWStepShape_RWLoopAndPath.pxx"

// Added by FMA
#include "../RWStepGeom/RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.pxx"
#include "../RWStepGeom/RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndSolidAngleUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSolidAngleUnit.pxx"
#include "../RWStepShape/RWStepShape_RWFacetedBrepAndBrepWithVoids.pxx"
#include "../RWStepBasic/RWStepBasic_RWMechanicalContext.pxx"

// full Rev4
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndRatioUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndTimeUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndRatioUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndTimeUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWApprovalDateTime.pxx"
// not yet #include <StepVisual_CameraImage2dWithScale.hxx> derived
// not yet #include <StepVisual_CameraImage3dWithScale.hxx> derived
// not yet #include <StepVisual_CartesianTransformationOperator2d.hxx> derived
#include "../RWStepBasic/RWStepBasic_RWDerivedUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWDerivedUnitElement.pxx"
#include "../RWStepRepr/RWStepRepr_RWItemDefinedTransformation.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentedItemRepresentation.pxx"
#include "../RWStepVisual/RWStepVisual_RWPresentationLayerUsage.pxx"

//  Added by CKY (JUL-1998) for AP214 CC1 -> CC2

#include "RWStepAP214_RWAutoDesignDocumentReference.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocument.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocumentRelationship.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocumentType.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocumentUsageConstraint.pxx"
#include "../RWStepBasic/RWStepBasic_RWEffectivity.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionEffectivity.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionRelationship.pxx"

#include "../RWStepBasic/RWStepBasic_RWProductDefinitionWithAssociatedDocuments.pxx"

#include "../RWStepRepr/RWStepRepr_RWMakeFromUsageOption.pxx"
#include "../RWStepRepr/RWStepRepr_RWAssemblyComponentUsage.pxx"
#include "../RWStepRepr/RWStepRepr_RWQuantifiedAssemblyComponentUsage.pxx"
#include "../RWStepRepr/RWStepRepr_RWSpecifiedHigherUsageOccurrence.pxx"
#include "../RWStepRepr/RWStepRepr_RWAssemblyComponentUsageSubstitute.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationRelationshipWithTransformation.pxx"
#include "../RWStepRepr/RWStepRepr_RWShapeRepresentationRelationshipWithTransformation.pxx"
#include "../RWStepRepr/RWStepRepr_RWMaterialDesignation.pxx"

#include "../RWStepShape/RWStepShape_RWContextDependentShapeRepresentation.pxx"

// Added from CD to DIS (S4134)
#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_AppliedDateAssignment.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_AppliedDocumentReference.hxx>
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AppliedPresentedItem.hxx>
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepShape_ExtrudedFaceSolid.hxx>
#include <StepShape_RevolvedFaceSolid.hxx>
#include <StepShape_SweptFaceSolid.hxx>

#include "RWStepAP214_RWAppliedDateAndTimeAssignment.pxx"
#include "RWStepAP214_RWAppliedDateAssignment.pxx"
#include "RWStepAP214_RWAppliedApprovalAssignment.pxx"
#include "RWStepAP214_RWAppliedDocumentReference.pxx"
#include "RWStepAP214_RWAppliedGroupAssignment.pxx"
#include "RWStepAP214_RWAppliedOrganizationAssignment.pxx"
#include "RWStepAP214_RWAppliedPersonAndOrganizationAssignment.pxx"
#include "RWStepAP214_RWAppliedPresentedItem.pxx"
#include "RWStepAP214_RWAppliedSecurityClassificationAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocumentFile.pxx"
#include "../RWStepBasic/RWStepBasic_RWCharacterizedObject.pxx"
#include "../RWStepShape/RWStepShape_RWExtrudedFaceSolid.pxx"
#include "../RWStepShape/RWStepShape_RWRevolvedFaceSolid.pxx"
#include "../RWStepShape/RWStepShape_RWSweptFaceSolid.pxx"

// Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
#include <StepRepr_MeasureRepresentationItem.hxx>
#include "../RWStepRepr/RWStepRepr_RWMeasureRepresentationItem.pxx"
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndAreaUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndVolumeUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndAreaUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndVolumeUnit.pxx"

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
#include "../RWStepBasic/RWStepBasic_RWAction.pxx"
#include "../RWStepBasic/RWStepBasic_RWActionAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWActionMethod.pxx"
#include "../RWStepBasic/RWStepBasic_RWActionRequestAssignment.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignApproval.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignCertification.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignContract.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignDateAndTimeAssignment.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignPersonAndOrganizationAssignment.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignSecurityClassification.pxx"
#include "../RWStepAP203/RWStepAP203_RWCcDesignSpecificationReference.pxx"
#include "../RWStepBasic/RWStepBasic_RWCertification.pxx"
#include "../RWStepBasic/RWStepBasic_RWCertificationAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWCertificationType.pxx"
#include "../RWStepAP203/RWStepAP203_RWChange.pxx"
#include "../RWStepAP203/RWStepAP203_RWChangeRequest.pxx"
#include "../RWStepRepr/RWStepRepr_RWConfigurationDesign.pxx"
#include "../RWStepRepr/RWStepRepr_RWConfigurationEffectivity.pxx"
#include "../RWStepBasic/RWStepBasic_RWContract.pxx"
#include "../RWStepBasic/RWStepBasic_RWContractAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWContractType.pxx"
#include "../RWStepRepr/RWStepRepr_RWProductConcept.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductConceptContext.pxx"
#include "../RWStepAP203/RWStepAP203_RWStartRequest.pxx"
#include "../RWStepAP203/RWStepAP203_RWStartWork.pxx"
#include "../RWStepBasic/RWStepBasic_RWVersionedActionRequest.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductCategoryRelationship.pxx"
#include "../RWStepBasic/RWStepBasic_RWActionRequestSolution.pxx"

// Added by ABV 13.01.00 for CAX-IF TRJ3
#include <StepVisual_DraughtingModel.hxx>
#include "../RWStepVisual/RWStepVisual_RWDraughtingModel.pxx"

// Added by ABV 18.04.00 for CAX-IF TRJ4 (dimensions)
#include <StepShape_AngularLocation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include "../RWStepShape/RWStepShape_RWAngularLocation.pxx"
#include "../RWStepShape/RWStepShape_RWAngularSize.pxx"
#include "../RWStepShape/RWStepShape_RWDimensionalCharacteristicRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWDimensionalLocation.pxx"
#include "../RWStepShape/RWStepShape_RWDimensionalLocationWithPath.pxx"
#include "../RWStepShape/RWStepShape_RWDimensionalSize.pxx"
#include "../RWStepShape/RWStepShape_RWDimensionalSizeWithPath.pxx"
#include "../RWStepShape/RWStepShape_RWShapeDimensionRepresentation.pxx"

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
#include "../RWStepBasic/RWStepBasic_RWDocumentRepresentationType.pxx"
#include "../RWStepBasic/RWStepBasic_RWObjectRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWRoleAssociation.pxx"
#include "../RWStepBasic/RWStepBasic_RWIdentificationRole.pxx"
#include "../RWStepBasic/RWStepBasic_RWIdentificationAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWExternalIdentificationAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWEffectivityAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWNameAssignment.pxx"
#include "../RWStepBasic/RWStepBasic_RWGeneralProperty.pxx"
#include "RWStepAP214_RWClass.pxx"
#include "RWStepAP214_RWExternallyDefinedClass.pxx"
#include "RWStepAP214_RWExternallyDefinedGeneralProperty.pxx"
#include "RWStepAP214_RWAppliedExternalIdentificationAssignment.pxx"

// abv 11.07.00: CAX-IF TRJ4: k1_geo-ac.stp
#include <StepShape_DefinitionalRepresentationAndShapeRepresentation.hxx>
#include "../RWStepShape/RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation.pxx"

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
#include "../RWStepShape/RWStepShape_RWLimitsAndFits.pxx"
#include "../RWStepShape/RWStepShape_RWToleranceValue.pxx"
#include "../RWStepShape/RWStepShape_RWMeasureQualification.pxx"
#include "../RWStepShape/RWStepShape_RWPlusMinusTolerance.pxx"
#include "../RWStepShape/RWStepShape_RWPrecisionQualifier.pxx"
#include "../RWStepShape/RWStepShape_RWTypeQualifier.pxx"
#include "../RWStepShape/RWStepShape_RWQualifiedRepresentationItem.pxx"
#include "../RWStepShape/RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWCompoundRepresentationItem.pxx"

// abv 28.12.01: CAX-IF TRJ9: edge_based_wireframe
#include <StepShape_CompoundShapeRepresentation.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_ConnectedFaceShapeRepresentation.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_EdgeBasedWireframeShapeRepresentation.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include "../RWStepShape/RWStepShape_RWCompoundShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWConnectedEdgeSet.pxx"
#include "../RWStepShape/RWStepShape_RWConnectedFaceShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWEdgeBasedWireframeModel.pxx"
#include "../RWStepShape/RWStepShape_RWEdgeBasedWireframeShapeRepresentation.pxx"
#include "../RWStepShape/RWStepShape_RWFaceBasedSurfaceModel.pxx"
#include "../RWStepShape/RWStepShape_RWNonManifoldSurfaceShapeRepresentation.pxx"

// gka 08.01.02 TRJ9
#include <StepGeom_OrientedSurface.hxx>
#include <StepShape_Subface.hxx>
#include <StepShape_Subedge.hxx>
#include <StepShape_SeamEdge.hxx>
#include <StepShape_ConnectedFaceSubSet.hxx>

#include "../RWStepGeom/RWStepGeom_RWOrientedSurface.pxx"
#include "../RWStepShape/RWStepShape_RWSubface.pxx"
#include "../RWStepShape/RWStepShape_RWSubedge.pxx"
#include "../RWStepShape/RWStepShape_RWSeamEdge.pxx"
#include "../RWStepShape/RWStepShape_RWConnectedFaceSubSet.pxx"

// Added for AP209
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
#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
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
#include <StepFEA_SymmetricTensor22d.hxx>
#include <StepFEA_SymmetricTensor42d.hxx>
#include <StepFEA_SymmetricTensor43d.hxx>
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

#include "../RWStepBasic/RWStepBasic_RWEulerAngles.pxx"
#include "../RWStepBasic/RWStepBasic_RWMassUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWThermodynamicTemperatureUnit.pxx"
#include "../RWStepElement/RWStepElement_RWAnalysisItemWithinRepresentation.pxx"
#include "../RWStepElement/RWStepElement_RWCurve3dElementDescriptor.pxx"
#include "../RWStepElement/RWStepElement_RWCurveElementEndReleasePacket.pxx"
#include "../RWStepElement/RWStepElement_RWCurveElementSectionDefinition.pxx"
#include "../RWStepElement/RWStepElement_RWCurveElementSectionDerivedDefinitions.pxx"
#include "../RWStepElement/RWStepElement_RWElementDescriptor.pxx"
#include "../RWStepElement/RWStepElement_RWElementMaterial.pxx"
#include "../RWStepElement/RWStepElement_RWSurface3dElementDescriptor.pxx"
#include "../RWStepElement/RWStepElement_RWSurfaceElementProperty.pxx"
#include "../RWStepElement/RWStepElement_RWSurfaceSection.pxx"
#include "../RWStepElement/RWStepElement_RWSurfaceSectionField.pxx"
#include "../RWStepElement/RWStepElement_RWSurfaceSectionFieldConstant.pxx"
#include "../RWStepElement/RWStepElement_RWSurfaceSectionFieldVarying.pxx"
#include "../RWStepElement/RWStepElement_RWUniformSurfaceSection.pxx"
#include "../RWStepElement/RWStepElement_RWVolume3dElementDescriptor.pxx"
#include "../RWStepFEA/RWStepFEA_RWAlignedCurve3dElementCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurve3dElementProperty.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurve3dElementRepresentation.pxx"
#include "../RWStepFEA/RWStepFEA_RWNode.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurveElementEndOffset.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurveElementEndRelease.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurveElementInterval.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurveElementIntervalConstant.pxx"
#include "../RWStepFEA/RWStepFEA_RWDummyNode.pxx"
#include "../RWStepFEA/RWStepFEA_RWCurveElementLocation.pxx"
#include "../RWStepFEA/RWStepFEA_RWElementGeometricRelationship.pxx"
#include "../RWStepFEA/RWStepFEA_RWElementGroup.pxx"
#include "../RWStepFEA/RWStepFEA_RWElementRepresentation.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaAreaDensity.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaAxis2Placement3d.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaGroup.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaLinearElasticity.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaMassDensity.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaMaterialPropertyRepresentation.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaMaterialPropertyRepresentationItem.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaModel.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaModel3d.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaMoistureAbsorption.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaParametricPoint.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaRepresentationItem.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaShellBendingStiffness.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaShellMembraneStiffness.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaShellShearStiffness.pxx"
#include "../RWStepFEA/RWStepFEA_RWGeometricNode.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeGroup.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeRepresentation.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeSet.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeWithSolutionCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeWithVector.pxx"
#include "../RWStepFEA/RWStepFEA_RWParametricCurve3dElementCoordinateDirection.pxx"
#include "../RWStepFEA/RWStepFEA_RWParametricCurve3dElementCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWParametricSurface3dElementCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWSurface3dElementRepresentation.pxx"
#include "../RWStepFEA/RWStepFEA_RWVolume3dElementRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWDataEnvironment.pxx"
#include "../RWStepRepr/RWStepRepr_RWMaterialPropertyRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWPropertyDefinitionRelationship.pxx"
#include "../RWStepShape/RWStepShape_RWPointRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWMaterialProperty.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaModelDefinition.pxx"
#include "../RWStepFEA/RWStepFEA_RWFreedomAndCoefficient.pxx"
#include "../RWStepFEA/RWStepFEA_RWFreedomsList.pxx"
#include "../RWStepBasic/RWStepBasic_RWProductDefinitionFormationRelationship.pxx"
#include "../RWStepFEA/RWStepFEA_RWNodeDefinition.pxx"
#include "../RWStepRepr/RWStepRepr_RWStructuralResponseProperty.pxx"
#include "../RWStepRepr/RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation.pxx"

#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndMassUnit.pxx"
#include "../RWStepBasic/RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit.pxx"
#include <StepFEA_AlignedSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_ConstantSurface3dElementCoordinateSystem.hxx>
#include "../RWStepFEA/RWStepFEA_RWAlignedSurface3dElementCoordinateSystem.pxx"
#include "../RWStepFEA/RWStepFEA_RWConstantSurface3dElementCoordinateSystem.pxx"

// 23.01.2003
#include <StepFEA_CurveElementIntervalLinearlyVarying.hxx>
#include <StepFEA_FeaCurveSectionGeometricRelationship.hxx>
#include <StepFEA_FeaSurfaceSectionGeometricRelationship.hxx>
#include "../RWStepFEA/RWStepFEA_RWCurveElementIntervalLinearlyVarying.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaCurveSectionGeometricRelationship.pxx"
#include "../RWStepFEA/RWStepFEA_RWFeaSurfaceSectionGeometricRelationship.pxx"

// PTV 28.01.2003 TRJ11 AP214 external references
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include "../RWStepBasic/RWStepBasic_RWDocumentProductAssociation.pxx"
#include "../RWStepBasic/RWStepBasic_RWDocumentProductEquivalence.pxx"

//  TR12J 04.06.2003 G&DT entities GKA
#include <StepShape_ShapeRepresentationWithParameters.hxx>
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
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_CommonDatum.hxx>
#include <StepDimTol_DatumTarget.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_Extension.hxx>
#include "../RWStepRepr/RWStepRepr_RWCompositeShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWDerivedShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWExtension.pxx"
#include "../RWStepShape/RWStepShape_RWShapeRepresentationWithParameters.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWAngularityTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWConcentricityTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWCircularRunoutTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWCoaxialityTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWCylindricityTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWFlatnessTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWLineProfileTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWParallelismTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWPerpendicularityTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWPositionTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWRoundnessTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWStraightnessTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWSurfaceProfileTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWSymmetryTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWTotalRunoutTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceWithDatumReference.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWModifiedGeometricTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatum.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumFeature.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumReference.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWCommonDatum.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumTarget.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWPlacedDatumTargetFeature.pxx"
#include <StepDimTol_GeometricToleranceRelationship.hxx>
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceRelationship.pxx"

#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include "../RWStepRepr/RWStepRepr_RWReprItemAndLengthMeasureWithUnit.pxx"
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.pxx"

// added by skl 10.02.2004 for TRJ13
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include "../RWStepBasic/RWStepBasic_RWConversionBasedUnitAndMassUnit.pxx"
#include <StepBasic_MassMeasureWithUnit.hxx>
#include "../RWStepBasic/RWStepBasic_RWMassMeasureWithUnit.pxx"

// Added by ika for GD&T AP242
#include "../RWStepRepr/RWStepRepr_RWApex.pxx"
#include "../RWStepRepr/RWStepRepr_RWCentreOfSymmetry.pxx"
#include "../RWStepRepr/RWStepRepr_RWGeometricAlignment.pxx"
#include "../RWStepRepr/RWStepRepr_RWParallelOffset.pxx"
#include "../RWStepRepr/RWStepRepr_RWPerpendicularTo.pxx"
#include "../RWStepRepr/RWStepRepr_RWTangent.pxx"
#include "../RWStepAP242/RWStepAP242_RWGeometricItemSpecificUsage.pxx"
#include "../RWStepAP242/RWStepAP242_RWIdAttribute.pxx"
#include "../RWStepAP242/RWStepAP242_RWItemIdentifiedRepresentationUsage.pxx"
#include "../RWStepRepr/RWStepRepr_RWAllAroundShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWBetweenShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWCompositeGroupShapeAspect.pxx"
#include "../RWStepRepr/RWStepRepr_RWContinuosShapeAspect.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceWithDefinedUnit.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceWithMaximumTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeometricToleranceWithModifiers.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWUnequallyDisposedGeometricTolerance.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWNonUniformZoneDefinition.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWProjectedZoneDefinition.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWRunoutZoneDefinition.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWRunoutZoneOrientation.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWToleranceZone.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWToleranceZoneDefinition.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWToleranceZoneForm.pxx"
#include "../RWStepShape/RWStepShape_RWValueFormatTypeQualifier.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumReferenceCompartment.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumReferenceElement.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumReferenceModifierWithValue.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWDatumSystem.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeneralDatumReference.pxx"
#include "../RWStepRepr/RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit.pxx"
#include "../RWStepRepr/RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI.pxx"
#include "../RWStepRepr/RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthDatRef.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthMod.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.pxx"
#include "../RWStepRepr/RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp.pxx"
#include "../RWStepRepr/RWStepRepr_RWCompShAspAndDatumFeatAndShAsp.pxx"
#include "../RWStepRepr/RWStepRepr_RWBooleanRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWIntegerRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWRealRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWValueRepresentationItem.pxx"
#include "../RWStepRepr/RWStepRepr_RWValueRepresentationItem.pxx"
#include "../RWStepAP242/RWStepAP242_RWDraughtingModelItemAssociation.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.pxx"
#include "../RWStepDimTol/RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol.pxx"
#include "../RWStepVisual/RWStepVisual_RWAnnotationCurveOccurrence.pxx"
#include "../RWStepVisual/RWStepVisual_RWAnnotationOccurrence.pxx"
#include "../RWStepVisual/RWStepVisual_RWAnnotationPlane.pxx"
#include "../RWStepVisual/RWStepVisual_RWDraughtingCallout.pxx"

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
#include <StepRepr_BooleanRepresentationItem.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <StepRepr_RealRepresentationItem.hxx>
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
#include <StepRepr_MechanicalDesignAndDraughtingRelationship.hxx>

#include "../RWStepVisual/RWStepVisual_RWTessellatedAnnotationOccurrence.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedItem.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedGeometricSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWTessellatedCurveSet.pxx"
#include "../RWStepVisual/RWStepVisual_RWCoordinatesList.pxx"
#include "../RWStepRepr/RWStepRepr_RWCharacterizedRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWConstructiveGeometryRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWConstructiveGeometryRepresentationRelationship.pxx"
#include <StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel.hxx>
#include "../RWStepVisual/RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel.pxx"
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>
#include "../RWStepVisual/RWStepVisual_RWAnnotationFillArea.pxx"
#include "../RWStepVisual/RWStepVisual_RWAnnotationFillAreaOccurrence.pxx"
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include "../RWStepVisual/RWStepVisual_RWCameraModelD3MultiClipping.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraModelD3MultiClippingIntersection.pxx"
#include "../RWStepVisual/RWStepVisual_RWCameraModelD3MultiClippingUnion.pxx"
#include <StepVisual_AnnotationCurveOccurrenceAndGeomReprItem.hxx>
#include "../RWStepVisual/RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem.pxx"

#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleTransparent.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleReflectanceAmbient.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleRendering.pxx"
#include "../RWStepVisual/RWStepVisual_RWSurfaceStyleRenderingWithProperties.pxx"

// Added for kinematics implementation
#include "../RWStepGeom/RWStepGeom_RWSuParameters.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWActuatedKinPairAndOrderKinPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWActuatedKinematicPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWContextDependentKinematicLinkRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWCylindricalPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWCylindricalPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWCylindricalPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWFullyConstrainedPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWGearPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWGearPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWGearPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWHomokineticPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicJoint.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicLink.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicLinkRepresentationAssociation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicPropertyMechanismRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicTopologyDirectedStructure.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicTopologyNetworkStructure.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWKinematicTopologyStructure.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLinearFlexibleAndPinionPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLinearFlexibleAndPlanarCurvePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLinearFlexibleLinkRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLowOrderKinematicPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLowOrderKinematicPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWLowOrderKinematicPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWMechanismRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWMechanismStateRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWOrientedJoint.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPairRepresentationRelationship.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPlanarCurvePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPlanarCurvePairRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPlanarPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPlanarPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPlanarPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnPlanarCurvePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnPlanarCurvePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnPlanarCurvePairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnSurfacePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnSurfacePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPointOnSurfacePairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPrismaticPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPrismaticPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWPrismaticPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWProductDefinitionKinematics.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWProductDefinitionRelationshipKinematics.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRackAndPinionPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRackAndPinionPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRackAndPinionPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRevolutePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRevolutePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRevolutePairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRigidLinkRepresentation.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRollingCurvePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRollingCurvePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRollingSurfacePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRollingSurfacePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWRotationAboutDirection.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWScrewPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWScrewPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWScrewPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSlidingCurvePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSlidingCurvePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSlidingSurfacePair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSlidingSurfacePairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSphericalPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSphericalPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSphericalPairWithPin.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSphericalPairWithPinAndRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSphericalPairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWSurfacePairWithRange.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWUnconstrainedPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWUnconstrainedPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWUniversalPair.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWUniversalPairValue.pxx"
#include "../RWStepKinematics/RWStepKinematics_RWUniversalPairWithRange.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationContextReference.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentationReference.pxx"

#include <StepGeom_SuParameters.hxx>
#include <StepKinematics_ActuatedKinPairAndOrderKinPair.hxx>
#include <StepKinematics_ActuatedKinematicPair.hxx>
#include <StepKinematics_ContextDependentKinematicLinkRepresentation.hxx>
#include <StepKinematics_CylindricalPair.hxx>
#include <StepKinematics_CylindricalPairValue.hxx>
#include <StepKinematics_CylindricalPairWithRange.hxx>
#include <StepKinematics_FullyConstrainedPair.hxx>
#include <StepKinematics_GearPair.hxx>
#include <StepKinematics_GearPairValue.hxx>
#include <StepKinematics_GearPairWithRange.hxx>
#include <StepKinematics_HomokineticPair.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_KinematicLink.hxx>
#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepKinematics_KinematicPropertyMechanismRepresentation.hxx>
#include <StepKinematics_KinematicTopologyDirectedStructure.hxx>
#include <StepKinematics_KinematicTopologyNetworkStructure.hxx>
#include <StepKinematics_KinematicTopologyStructure.hxx>
#include <StepKinematics_LinearFlexibleAndPinionPair.hxx>
#include <StepKinematics_LinearFlexibleAndPlanarCurvePair.hxx>
#include <StepKinematics_LinearFlexibleLinkRepresentation.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>
#include <StepKinematics_LowOrderKinematicPairValue.hxx>
#include <StepKinematics_LowOrderKinematicPairWithRange.hxx>
#include <StepKinematics_MechanismRepresentation.hxx>
#include <StepKinematics_MechanismStateRepresentation.hxx>
#include <StepKinematics_OrientedJoint.hxx>
#include <StepKinematics_PairRepresentationRelationship.hxx>
#include <StepKinematics_PlanarCurvePair.hxx>
#include <StepKinematics_PlanarCurvePairRange.hxx>
#include <StepKinematics_PlanarPair.hxx>
#include <StepKinematics_PlanarPairValue.hxx>
#include <StepKinematics_PlanarPairWithRange.hxx>
#include <StepKinematics_PointOnPlanarCurvePair.hxx>
#include <StepKinematics_PointOnPlanarCurvePairValue.hxx>
#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>
#include <StepKinematics_PointOnSurfacePair.hxx>
#include <StepKinematics_PointOnSurfacePairValue.hxx>
#include <StepKinematics_PointOnSurfacePairWithRange.hxx>
#include <StepKinematics_PrismaticPair.hxx>
#include <StepKinematics_PrismaticPairValue.hxx>
#include <StepKinematics_PrismaticPairWithRange.hxx>
#include <StepKinematics_ProductDefinitionKinematics.hxx>
#include <StepKinematics_ProductDefinitionRelationshipKinematics.hxx>
#include <StepKinematics_RackAndPinionPair.hxx>
#include <StepKinematics_RackAndPinionPairValue.hxx>
#include <StepKinematics_RackAndPinionPairWithRange.hxx>
#include <StepKinematics_RevolutePair.hxx>
#include <StepKinematics_RevolutePairValue.hxx>
#include <StepKinematics_RevolutePairWithRange.hxx>
#include <StepKinematics_RigidLinkRepresentation.hxx>
#include <StepKinematics_RollingCurvePair.hxx>
#include <StepKinematics_RollingCurvePairValue.hxx>
#include <StepKinematics_RollingSurfacePair.hxx>
#include <StepKinematics_RollingSurfacePairValue.hxx>
#include <StepKinematics_RotationAboutDirection.hxx>
#include <StepKinematics_ScrewPair.hxx>
#include <StepKinematics_ScrewPairValue.hxx>
#include <StepKinematics_ScrewPairWithRange.hxx>
#include <StepKinematics_SlidingCurvePair.hxx>
#include <StepKinematics_SlidingCurvePairValue.hxx>
#include <StepKinematics_SlidingSurfacePair.hxx>
#include <StepKinematics_SlidingSurfacePairValue.hxx>
#include <StepKinematics_SphericalPair.hxx>
#include <StepKinematics_SphericalPairValue.hxx>
#include <StepKinematics_SphericalPairWithPin.hxx>
#include <StepKinematics_SphericalPairWithPinAndRange.hxx>
#include <StepKinematics_SphericalPairWithRange.hxx>
#include <StepKinematics_SurfacePairWithRange.hxx>
#include <StepKinematics_UnconstrainedPair.hxx>
#include <StepKinematics_UnconstrainedPairValue.hxx>
#include <StepKinematics_UniversalPair.hxx>
#include <StepKinematics_UniversalPairValue.hxx>
#include <StepKinematics_UniversalPairWithRange.hxx>
#include <StepRepr_RepresentationContextReference.hxx>
#include <StepRepr_RepresentationReference.hxx>

// -- General Declarations (Recognize, StepType) ---

class Reco_Container
{
public:
  static Reco_Container& Instance()
  {
    static Reco_Container theInstance;
    return theInstance;
  }

  const TCollection_AsciiString PasReco      = "?";
  const TCollection_AsciiString Reco_Address = "ADDRESS";
  const TCollection_AsciiString Reco_AdvancedBrepShapeRepresentation =
    "ADVANCED_BREP_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_AdvancedFace              = "ADVANCED_FACE";
  const TCollection_AsciiString Reco_AnnotationCurveOccurrence = "ANNOTATION_CURVE_OCCURRENCE";
  const TCollection_AsciiString Reco_AnnotationFillArea        = "ANNOTATION_FILL_AREA";
  const TCollection_AsciiString Reco_AnnotationFillAreaOccurrence =
    "ANNOTATION_FILL_AREA_OCCURRENCE";
  const TCollection_AsciiString Reco_AnnotationOccurrence = "ANNOTATION_OCCURRENCE";
  const TCollection_AsciiString Reco_AnnotationSubfigureOccurrence =
    "ANNOTATION_SUBFIGURE_OCCURRENCE";
  const TCollection_AsciiString Reco_AnnotationSymbol           = "ANNOTATION_SYMBOL";
  const TCollection_AsciiString Reco_AnnotationSymbolOccurrence = "ANNOTATION_SYMBOL_OCCURRENCE";
  const TCollection_AsciiString Reco_AnnotationText             = "ANNOTATION_TEXT";
  const TCollection_AsciiString Reco_AnnotationTextOccurrence   = "ANNOTATION_TEXT_OCCURRENCE";
  const TCollection_AsciiString Reco_ApplicationContext         = "APPLICATION_CONTEXT";
  const TCollection_AsciiString Reco_ApplicationContextElement  = "APPLICATION_CONTEXT_ELEMENT";
  const TCollection_AsciiString Reco_ApplicationProtocolDefinition =
    "APPLICATION_PROTOCOL_DEFINITION";
  const TCollection_AsciiString Reco_Approval                   = "APPROVAL";
  const TCollection_AsciiString Reco_ApprovalAssignment         = "APPROVAL_ASSIGNMENT";
  const TCollection_AsciiString Reco_ApprovalPersonOrganization = "APPROVAL_PERSON_ORGANIZATION";
  const TCollection_AsciiString Reco_ApprovalRelationship       = "APPROVAL_RELATIONSHIP";
  const TCollection_AsciiString Reco_ApprovalRole               = "APPROVAL_ROLE";
  const TCollection_AsciiString Reco_ApprovalStatus             = "APPROVAL_STATUS";
  const TCollection_AsciiString Reco_AreaInSet                  = "AREA_IN_SET";
  const TCollection_AsciiString Reco_AutoDesignActualDateAndTimeAssignment =
    "AUTO_DESIGN_ACTUAL_DATE_AND_TIME_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignActualDateAssignment =
    "AUTO_DESIGN_ACTUAL_DATE_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignApprovalAssignment =
    "AUTO_DESIGN_APPROVAL_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignDateAndPersonAssignment =
    "AUTO_DESIGN_DATE_AND_PERSON_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignGroupAssignment = "AUTO_DESIGN_GROUP_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignNominalDateAndTimeAssignment =
    "AUTO_DESIGN_NOMINAL_DATE_AND_TIME_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignNominalDateAssignment =
    "AUTO_DESIGN_NOMINAL_DATE_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignOrganizationAssignment =
    "AUTO_DESIGN_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignPersonAndOrganizationAssignment =
    "AUTO_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignPresentedItem = "AUTO_DESIGN_PRESENTED_ITEM";
  const TCollection_AsciiString Reco_AutoDesignSecurityClassificationAssignment =
    "AUTO_DESIGN_SECURITY_CLASSIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_AutoDesignViewArea      = "AUTO_DESIGN_VIEW_AREA";
  const TCollection_AsciiString Reco_Axis1Placement          = "AXIS1_PLACEMENT";
  const TCollection_AsciiString Reco_Axis2Placement2d        = "AXIS2_PLACEMENT_2D";
  const TCollection_AsciiString Reco_Axis2Placement3d        = "AXIS2_PLACEMENT_3D";
  const TCollection_AsciiString Reco_BSplineCurve            = "B_SPLINE_CURVE";
  const TCollection_AsciiString Reco_BSplineCurveWithKnots   = "B_SPLINE_CURVE_WITH_KNOTS";
  const TCollection_AsciiString Reco_BSplineSurface          = "B_SPLINE_SURFACE";
  const TCollection_AsciiString Reco_BSplineSurfaceWithKnots = "B_SPLINE_SURFACE_WITH_KNOTS";
  const TCollection_AsciiString Reco_BackgroundColour        = "BACKGROUND_COLOUR";
  const TCollection_AsciiString Reco_BezierCurve             = "BEZIER_CURVE";
  const TCollection_AsciiString Reco_BezierSurface           = "BEZIER_SURFACE";
  const TCollection_AsciiString Reco_Block                   = "BLOCK";
  const TCollection_AsciiString Reco_BooleanResult           = "BOOLEAN_RESULT";
  const TCollection_AsciiString Reco_BoundaryCurve           = "BOUNDARY_CURVE";
  const TCollection_AsciiString Reco_BoundedCurve            = "BOUNDED_CURVE";
  const TCollection_AsciiString Reco_BoundedSurface          = "BOUNDED_SURFACE";
  const TCollection_AsciiString Reco_BoxDomain               = "BOX_DOMAIN";
  const TCollection_AsciiString Reco_BoxedHalfSpace          = "BOXED_HALF_SPACE";
  const TCollection_AsciiString Reco_BrepWithVoids           = "BREP_WITH_VOIDS";
  const TCollection_AsciiString Reco_CalendarDate            = "CALENDAR_DATE";
  const TCollection_AsciiString Reco_CameraImage             = "CAMERA_IMAGE";
  const TCollection_AsciiString Reco_CameraModel             = "CAMERA_MODEL";
  const TCollection_AsciiString Reco_CameraModelD2           = "CAMERA_MODEL_D2";
  const TCollection_AsciiString Reco_CameraModelD3           = "CAMERA_MODEL_D3";
  const TCollection_AsciiString Reco_CameraUsage             = "CAMERA_USAGE";
  const TCollection_AsciiString Reco_CartesianPoint          = "CARTESIAN_POINT";
  const TCollection_AsciiString Reco_CartesianTransformationOperator =
    "CARTESIAN_TRANSFORMATION_OPERATOR";
  const TCollection_AsciiString Reco_CartesianTransformationOperator3d =
    "CARTESIAN_TRANSFORMATION_OPERATOR_3D";
  const TCollection_AsciiString Reco_Circle                  = "CIRCLE";
  const TCollection_AsciiString Reco_ClosedShell             = "CLOSED_SHELL";
  const TCollection_AsciiString Reco_Colour                  = "COLOUR";
  const TCollection_AsciiString Reco_ColourRgb               = "COLOUR_RGB";
  const TCollection_AsciiString Reco_ColourSpecification     = "COLOUR_SPECIFICATION";
  const TCollection_AsciiString Reco_CompositeCurve          = "COMPOSITE_CURVE";
  const TCollection_AsciiString Reco_CompositeCurveOnSurface = "COMPOSITE_CURVE_ON_SURFACE";
  const TCollection_AsciiString Reco_CompositeCurveSegment   = "COMPOSITE_CURVE_SEGMENT";
  const TCollection_AsciiString Reco_CompositeText           = "COMPOSITE_TEXT";
  const TCollection_AsciiString Reco_CompositeTextWithAssociatedCurves =
    "COMPOSITE_TEXT_WITH_ASSOCIATED_CURVES";
  const TCollection_AsciiString Reco_CompositeTextWithBlankingBox =
    "COMPOSITE_TEXT_WITH_BLANKING_BOX";
  const TCollection_AsciiString Reco_CompositeTextWithExtent = "COMPOSITE_TEXT_WITH_EXTENT";
  const TCollection_AsciiString Reco_Conic                   = "CONIC";
  const TCollection_AsciiString Reco_ConicalSurface          = "CONICAL_SURFACE";
  const TCollection_AsciiString Reco_ConnectedFaceSet        = "CONNECTED_FACE_SET";
  const TCollection_AsciiString Reco_ContextDependentInvisibility =
    "CONTEXT_DEPENDENT_INVISIBILITY";
  const TCollection_AsciiString Reco_ContextDependentOverRidingStyledItem =
    "CONTEXT_DEPENDENT_OVER_RIDING_STYLED_ITEM";
  const TCollection_AsciiString Reco_ConversionBasedUnit = "CONVERSION_BASED_UNIT";
  const TCollection_AsciiString Reco_CoordinatedUniversalTimeOffset =
    "COORDINATED_UNIVERSAL_TIME_OFFSET";
  const TCollection_AsciiString Reco_CsgRepresentation          = "CSG_REPRESENTATION";
  const TCollection_AsciiString Reco_CsgShapeRepresentation     = "CSG_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_CsgSolid                   = "CSG_SOLID";
  const TCollection_AsciiString Reco_Curve                      = "CURVE";
  const TCollection_AsciiString Reco_CurveBoundedSurface        = "CURVE_BOUNDED_SURFACE";
  const TCollection_AsciiString Reco_CurveReplica               = "CURVE_REPLICA";
  const TCollection_AsciiString Reco_CurveStyle                 = "CURVE_STYLE";
  const TCollection_AsciiString Reco_CurveStyleFont             = "CURVE_STYLE_FONT";
  const TCollection_AsciiString Reco_CurveStyleFontPattern      = "CURVE_STYLE_FONT_PATTERN";
  const TCollection_AsciiString Reco_CylindricalSurface         = "CYLINDRICAL_SURFACE";
  const TCollection_AsciiString Reco_Date                       = "DATE";
  const TCollection_AsciiString Reco_DateAndTime                = "DATE_AND_TIME";
  const TCollection_AsciiString Reco_DateAndTimeAssignment      = "DATE_AND_TIME_ASSIGNMENT";
  const TCollection_AsciiString Reco_DateAssignment             = "DATE_ASSIGNMENT";
  const TCollection_AsciiString Reco_DateRole                   = "DATE_ROLE";
  const TCollection_AsciiString Reco_DateTimeRole               = "DATE_TIME_ROLE";
  const TCollection_AsciiString Reco_DefinedSymbol              = "DEFINED_SYMBOL";
  const TCollection_AsciiString Reco_DefinitionalRepresentation = "DEFINITIONAL_REPRESENTATION";
  const TCollection_AsciiString Reco_DegeneratePcurve           = "DEGENERATE_PCURVE";
  const TCollection_AsciiString Reco_DegenerateToroidalSurface  = "DEGENERATE_TOROIDAL_SURFACE";
  const TCollection_AsciiString Reco_DescriptiveRepresentationItem =
    "DESCRIPTIVE_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_DimensionCurve           = "DIMENSION_CURVE";
  const TCollection_AsciiString Reco_DimensionCurveTerminator = "DIMENSION_CURVE_TERMINATOR";
  const TCollection_AsciiString Reco_DimensionalExponents     = "DIMENSIONAL_EXPONENTS";
  const TCollection_AsciiString Reco_Direction                = "DIRECTION";
  const TCollection_AsciiString Reco_DraughtingAnnotationOccurrence =
    "DRAUGHTING_ANNOTATION_OCCURRENCE";
  const TCollection_AsciiString Reco_DraughtingCallout          = "DRAUGHTING_CALLOUT";
  const TCollection_AsciiString Reco_DraughtingPreDefinedColour = "DRAUGHTING_PRE_DEFINED_COLOUR";
  const TCollection_AsciiString Reco_DraughtingPreDefinedCurveFont =
    "DRAUGHTING_PRE_DEFINED_CURVE_FONT";
  const TCollection_AsciiString Reco_DraughtingSubfigureRepresentation =
    "DRAUGHTING_SUBFIGURE_REPRESENTATION";
  const TCollection_AsciiString Reco_DraughtingSymbolRepresentation =
    "DRAUGHTING_SYMBOL_REPRESENTATION";
  const TCollection_AsciiString Reco_DraughtingTextLiteralWithDelineation =
    "DRAUGHTING_TEXT_LITERAL_WITH_DELINEATION";
  const TCollection_AsciiString Reco_DrawingDefinition           = "DRAWING_DEFINITION";
  const TCollection_AsciiString Reco_DrawingRevision             = "DRAWING_REVISION";
  const TCollection_AsciiString Reco_Edge                        = "EDGE";
  const TCollection_AsciiString Reco_EdgeCurve                   = "EDGE_CURVE";
  const TCollection_AsciiString Reco_EdgeLoop                    = "EDGE_LOOP";
  const TCollection_AsciiString Reco_ElementarySurface           = "ELEMENTARY_SURFACE";
  const TCollection_AsciiString Reco_Ellipse                     = "ELLIPSE";
  const TCollection_AsciiString Reco_EvaluatedDegeneratePcurve   = "EVALUATED_DEGENERATE_PCURVE";
  const TCollection_AsciiString Reco_ExternalSource              = "EXTERNAL_SOURCE";
  const TCollection_AsciiString Reco_ExternallyDefinedCurveFont  = "EXTERNALLY_DEFINED_CURVE_FONT";
  const TCollection_AsciiString Reco_ExternallyDefinedHatchStyle = "EXTERNALLY_DEFINED_HATCH_STYLE";
  const TCollection_AsciiString Reco_ExternallyDefinedItem       = "EXTERNALLY_DEFINED_ITEM";
  const TCollection_AsciiString Reco_ExternallyDefinedSymbol     = "EXTERNALLY_DEFINED_SYMBOL";
  const TCollection_AsciiString Reco_ExternallyDefinedTextFont   = "EXTERNALLY_DEFINED_TEXT_FONT";
  const TCollection_AsciiString Reco_ExternallyDefinedTileStyle  = "EXTERNALLY_DEFINED_TILE_STYLE";
  const TCollection_AsciiString Reco_ExtrudedAreaSolid           = "EXTRUDED_AREA_SOLID";
  const TCollection_AsciiString Reco_Face                        = "FACE";
  const TCollection_AsciiString Reco_FaceBound                   = "FACE_BOUND";
  const TCollection_AsciiString Reco_FaceOuterBound              = "FACE_OUTER_BOUND";
  const TCollection_AsciiString Reco_FaceSurface                 = "FACE_SURFACE";
  const TCollection_AsciiString Reco_FacetedBrep                 = "FACETED_BREP";
  const TCollection_AsciiString Reco_FacetedBrepShapeRepresentation =
    "FACETED_BREP_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_FillAreaStyle         = "FILL_AREA_STYLE";
  const TCollection_AsciiString Reco_FillAreaStyleColour   = "FILL_AREA_STYLE_COLOUR";
  const TCollection_AsciiString Reco_FillAreaStyleHatching = "FILL_AREA_STYLE_HATCHING";
  const TCollection_AsciiString Reco_FillAreaStyleTileSymbolWithStyle =
    "FILL_AREA_STYLE_TILE_SYMBOL_WITH_STYLE";
  const TCollection_AsciiString Reco_FillAreaStyleTiles = "FILL_AREA_STYLE_TILES";
  const TCollection_AsciiString Reco_FunctionallyDefinedTransformation =
    "FUNCTIONALLY_DEFINED_TRANSFORMATION";
  const TCollection_AsciiString Reco_GeometricCurveSet = "GEOMETRIC_CURVE_SET";
  const TCollection_AsciiString Reco_GeometricRepresentationContext =
    "GEOMETRIC_REPRESENTATION_CONTEXT";
  const TCollection_AsciiString Reco_GeometricRepresentationItem = "GEOMETRIC_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_GeometricSet                = "GEOMETRIC_SET";
  const TCollection_AsciiString Reco_GeometricallyBoundedSurfaceShapeRepresentation =
    "GEOMETRICALLY_BOUNDED_SURFACE_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_GeometricallyBoundedWireframeShapeRepresentation =
    "GEOMETRICALLY_BOUNDED_WIREFRAME_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_GlobalUncertaintyAssignedContext =
    "GLOBAL_UNCERTAINTY_ASSIGNED_CONTEXT";
  const TCollection_AsciiString Reco_GlobalUnitAssignedContext = "GLOBAL_UNIT_ASSIGNED_CONTEXT";
  const TCollection_AsciiString Reco_Group                     = "GROUP";
  const TCollection_AsciiString Reco_GroupAssignment           = "GROUP_ASSIGNMENT";
  const TCollection_AsciiString Reco_GroupRelationship         = "GROUP_RELATIONSHIP";
  const TCollection_AsciiString Reco_HalfSpaceSolid            = "HALF_SPACE_SOLID";
  const TCollection_AsciiString Reco_Hyperbola                 = "HYPERBOLA";
  const TCollection_AsciiString Reco_IntersectionCurve         = "INTERSECTION_CURVE";
  const TCollection_AsciiString Reco_Invisibility              = "INVISIBILITY";
  const TCollection_AsciiString Reco_LengthMeasureWithUnit     = "LENGTH_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_LengthUnit                = "LENGTH_UNIT";
  const TCollection_AsciiString Reco_Line                      = "LINE";
  const TCollection_AsciiString Reco_LocalTime                 = "LOCAL_TIME";
  const TCollection_AsciiString Reco_Loop                      = "LOOP";
  const TCollection_AsciiString Reco_ManifoldSolidBrep         = "MANIFOLD_SOLID_BREP";
  const TCollection_AsciiString Reco_ManifoldSurfaceShapeRepresentation =
    "MANIFOLD_SURFACE_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_MappedItem      = "MAPPED_ITEM";
  const TCollection_AsciiString Reco_MeasureWithUnit = "MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_MechanicalDesignGeometricPresentationArea =
    "MECHANICAL_DESIGN_GEOMETRIC_PRESENTATION_AREA";
  const TCollection_AsciiString Reco_MechanicalDesignGeometricPresentationRepresentation =
    "MECHANICAL_DESIGN_GEOMETRIC_PRESENTATION_REPRESENTATION";
  const TCollection_AsciiString Reco_MechanicalDesignPresentationArea =
    "MECHANICAL_DESIGN_PRESENTATION_AREA";
  const TCollection_AsciiString Reco_NamedUnit                = "NAMED_UNIT";
  const TCollection_AsciiString Reco_OffsetCurve3d            = "OFFSET_CURVE_3D";
  const TCollection_AsciiString Reco_OffsetSurface            = "OFFSET_SURFACE";
  const TCollection_AsciiString Reco_OneDirectionRepeatFactor = "ONE_DIRECTION_REPEAT_FACTOR";
  const TCollection_AsciiString Reco_OpenShell                = "OPEN_SHELL";
  const TCollection_AsciiString Reco_OrdinalDate              = "ORDINAL_DATE";
  const TCollection_AsciiString Reco_Organization             = "ORGANIZATION";
  const TCollection_AsciiString Reco_OrganizationAssignment   = "ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_OrganizationRole         = "ORGANIZATION_ROLE";
  const TCollection_AsciiString Reco_OrganizationalAddress    = "ORGANIZATIONAL_ADDRESS";
  const TCollection_AsciiString Reco_OrientedClosedShell      = "ORIENTED_CLOSED_SHELL";
  const TCollection_AsciiString Reco_OrientedEdge             = "ORIENTED_EDGE";
  const TCollection_AsciiString Reco_OrientedFace             = "ORIENTED_FACE";
  const TCollection_AsciiString Reco_OrientedOpenShell        = "ORIENTED_OPEN_SHELL";
  const TCollection_AsciiString Reco_OrientedPath             = "ORIENTED_PATH";
  const TCollection_AsciiString Reco_OuterBoundaryCurve       = "OUTER_BOUNDARY_CURVE";
  const TCollection_AsciiString Reco_OverRidingStyledItem     = "OVER_RIDING_STYLED_ITEM";
  const TCollection_AsciiString Reco_Parabola                 = "PARABOLA";
  const TCollection_AsciiString Reco_ParametricRepresentationContext =
    "PARAMETRIC_REPRESENTATION_CONTEXT";
  const TCollection_AsciiString Reco_Path                  = "PATH";
  const TCollection_AsciiString Reco_Pcurve                = "PCURVE";
  const TCollection_AsciiString Reco_Person                = "PERSON";
  const TCollection_AsciiString Reco_PersonAndOrganization = "PERSON_AND_ORGANIZATION";
  const TCollection_AsciiString Reco_PersonAndOrganizationAssignment =
    "PERSON_AND_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_PersonAndOrganizationRole   = "PERSON_AND_ORGANIZATION_ROLE";
  const TCollection_AsciiString Reco_PersonalAddress             = "PERSONAL_ADDRESS";
  const TCollection_AsciiString Reco_Placement                   = "PLACEMENT";
  const TCollection_AsciiString Reco_PlanarBox                   = "PLANAR_BOX";
  const TCollection_AsciiString Reco_PlanarExtent                = "PLANAR_EXTENT";
  const TCollection_AsciiString Reco_Plane                       = "PLANE";
  const TCollection_AsciiString Reco_PlaneAngleMeasureWithUnit   = "PLANE_ANGLE_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_PlaneAngleUnit              = "PLANE_ANGLE_UNIT";
  const TCollection_AsciiString Reco_Point                       = "POINT";
  const TCollection_AsciiString Reco_PointOnCurve                = "POINT_ON_CURVE";
  const TCollection_AsciiString Reco_PointOnSurface              = "POINT_ON_SURFACE";
  const TCollection_AsciiString Reco_PointReplica                = "POINT_REPLICA";
  const TCollection_AsciiString Reco_PointStyle                  = "POINT_STYLE";
  const TCollection_AsciiString Reco_PolyLoop                    = "POLY_LOOP";
  const TCollection_AsciiString Reco_Polyline                    = "POLYLINE";
  const TCollection_AsciiString Reco_PreDefinedColour            = "PRE_DEFINED_COLOUR";
  const TCollection_AsciiString Reco_PreDefinedCurveFont         = "PRE_DEFINED_CURVE_FONT";
  const TCollection_AsciiString Reco_PreDefinedItem              = "PRE_DEFINED_ITEM";
  const TCollection_AsciiString Reco_PreDefinedSymbol            = "PRE_DEFINED_SYMBOL";
  const TCollection_AsciiString Reco_PreDefinedTextFont          = "PRE_DEFINED_TEXT_FONT";
  const TCollection_AsciiString Reco_PresentationArea            = "PRESENTATION_AREA";
  const TCollection_AsciiString Reco_PresentationLayerAssignment = "PRESENTATION_LAYER_ASSIGNMENT";
  const TCollection_AsciiString Reco_PresentationRepresentation  = "PRESENTATION_REPRESENTATION";
  const TCollection_AsciiString Reco_PresentationSet             = "PRESENTATION_SET";
  const TCollection_AsciiString Reco_PresentationSize            = "PRESENTATION_SIZE";
  const TCollection_AsciiString Reco_PresentationStyleAssignment = "PRESENTATION_STYLE_ASSIGNMENT";
  const TCollection_AsciiString Reco_PresentationStyleByContext  = "PRESENTATION_STYLE_BY_CONTEXT";
  const TCollection_AsciiString Reco_PresentationView            = "PRESENTATION_VIEW";
  const TCollection_AsciiString Reco_PresentedItem               = "PRESENTED_ITEM";
  const TCollection_AsciiString Reco_Product                     = "PRODUCT";
  const TCollection_AsciiString Reco_ProductCategory             = "PRODUCT_CATEGORY";
  const TCollection_AsciiString Reco_ProductContext              = "PRODUCT_CONTEXT";
  const TCollection_AsciiString Reco_ProductDataRepresentationView =
    "PRODUCT_DATA_REPRESENTATION_VIEW";
  const TCollection_AsciiString Reco_ProductDefinition          = "PRODUCT_DEFINITION";
  const TCollection_AsciiString Reco_ProductDefinitionContext   = "PRODUCT_DEFINITION_CONTEXT";
  const TCollection_AsciiString Reco_ProductDefinitionFormation = "PRODUCT_DEFINITION_FORMATION";
  const TCollection_AsciiString Reco_ProductDefinitionFormationWithSpecifiedSource =
    "PRODUCT_DEFINITION_FORMATION_WITH_SPECIFIED_SOURCE";
  const TCollection_AsciiString Reco_ProductDefinitionShape = "PRODUCT_DEFINITION_SHAPE";
  const TCollection_AsciiString Reco_ProductRelatedProductCategory =
    "PRODUCT_RELATED_PRODUCT_CATEGORY";
  const TCollection_AsciiString Reco_ProductType        = "PRODUCT_TYPE";
  const TCollection_AsciiString Reco_PropertyDefinition = "PROPERTY_DEFINITION";
  const TCollection_AsciiString Reco_PropertyDefinitionRepresentation =
    "PROPERTY_DEFINITION_REPRESENTATION";
  const TCollection_AsciiString Reco_QuasiUniformCurve           = "QUASI_UNIFORM_CURVE";
  const TCollection_AsciiString Reco_QuasiUniformSurface         = "QUASI_UNIFORM_SURFACE";
  const TCollection_AsciiString Reco_RatioMeasureWithUnit        = "RATIO_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_RationalBSplineCurve        = "RATIONAL_B_SPLINE_CURVE";
  const TCollection_AsciiString Reco_RationalBSplineSurface      = "RATIONAL_B_SPLINE_SURFACE";
  const TCollection_AsciiString Reco_RectangularCompositeSurface = "RECTANGULAR_COMPOSITE_SURFACE";
  const TCollection_AsciiString Reco_RectangularTrimmedSurface   = "RECTANGULAR_TRIMMED_SURFACE";
  const TCollection_AsciiString Reco_RepItemGroup                = "REP_ITEM_GROUP";
  const TCollection_AsciiString Reco_ReparametrisedCompositeCurveSegment =
    "REPARAMETRISED_COMPOSITE_CURVE_SEGMENT";
  const TCollection_AsciiString Reco_Representation             = "REPRESENTATION";
  const TCollection_AsciiString Reco_RepresentationContext      = "REPRESENTATION_CONTEXT";
  const TCollection_AsciiString Reco_RepresentationItem         = "REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_RepresentationMap          = "REPRESENTATION_MAP";
  const TCollection_AsciiString Reco_RepresentationRelationship = "REPRESENTATION_RELATIONSHIP";
  const TCollection_AsciiString Reco_RevolvedAreaSolid          = "REVOLVED_AREA_SOLID";
  const TCollection_AsciiString Reco_RightAngularWedge          = "RIGHT_ANGULAR_WEDGE";
  const TCollection_AsciiString Reco_RightCircularCone          = "RIGHT_CIRCULAR_CONE";
  const TCollection_AsciiString Reco_RightCircularCylinder      = "RIGHT_CIRCULAR_CYLINDER";
  const TCollection_AsciiString Reco_SeamCurve                  = "SEAM_CURVE";
  const TCollection_AsciiString Reco_SecurityClassification     = "SECURITY_CLASSIFICATION";
  const TCollection_AsciiString Reco_SecurityClassificationAssignment =
    "SECURITY_CLASSIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_SecurityClassificationLevel = "SECURITY_CLASSIFICATION_LEVEL";
  const TCollection_AsciiString Reco_ShapeAspect                 = "SHAPE_ASPECT";
  const TCollection_AsciiString Reco_ShapeAspectRelationship     = "SHAPE_ASPECT_RELATIONSHIP";
  const TCollection_AsciiString Reco_ShapeAspectTransition       = "SHAPE_ASPECT_TRANSITION";
  const TCollection_AsciiString Reco_ShapeDefinitionRepresentation =
    "SHAPE_DEFINITION_REPRESENTATION";
  const TCollection_AsciiString Reco_ShapeRepresentation       = "SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_ShellBasedSurfaceModel    = "SHELL_BASED_SURFACE_MODEL";
  const TCollection_AsciiString Reco_SiUnit                    = "SI_UNIT";
  const TCollection_AsciiString Reco_SolidAngleMeasureWithUnit = "SOLID_ANGLE_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_SolidModel                = "SOLID_MODEL";
  const TCollection_AsciiString Reco_SolidReplica              = "SOLID_REPLICA";
  const TCollection_AsciiString Reco_Sphere                    = "SPHERE";
  const TCollection_AsciiString Reco_SphericalSurface          = "SPHERICAL_SURFACE";
  const TCollection_AsciiString Reco_StyledItem                = "STYLED_ITEM";
  const TCollection_AsciiString Reco_Surface                   = "SURFACE";
  const TCollection_AsciiString Reco_SurfaceCurve              = "SURFACE_CURVE";
  const TCollection_AsciiString Reco_SurfaceOfLinearExtrusion  = "SURFACE_OF_LINEAR_EXTRUSION";
  const TCollection_AsciiString Reco_SurfaceOfRevolution       = "SURFACE_OF_REVOLUTION";
  const TCollection_AsciiString Reco_SurfacePatch              = "SURFACE_PATCH";
  const TCollection_AsciiString Reco_SurfaceReplica            = "SURFACE_REPLICA";
  const TCollection_AsciiString Reco_SurfaceSideStyle          = "SURFACE_SIDE_STYLE";
  const TCollection_AsciiString Reco_SurfaceStyleBoundary      = "SURFACE_STYLE_BOUNDARY";
  const TCollection_AsciiString Reco_SurfaceStyleControlGrid   = "SURFACE_STYLE_CONTROL_GRID";
  const TCollection_AsciiString Reco_SurfaceStyleFillArea      = "SURFACE_STYLE_FILL_AREA";
  const TCollection_AsciiString Reco_SurfaceStyleParameterLine = "SURFACE_STYLE_PARAMETER_LINE";
  const TCollection_AsciiString Reco_SurfaceStyleSegmentationCurve =
    "SURFACE_STYLE_SEGMENTATION_CURVE";
  const TCollection_AsciiString Reco_SurfaceStyleSilhouette  = "SURFACE_STYLE_SILHOUETTE";
  const TCollection_AsciiString Reco_SurfaceStyleUsage       = "SURFACE_STYLE_USAGE";
  const TCollection_AsciiString Reco_SweptAreaSolid          = "SWEPT_AREA_SOLID";
  const TCollection_AsciiString Reco_SweptSurface            = "SWEPT_SURFACE";
  const TCollection_AsciiString Reco_SymbolColour            = "SYMBOL_COLOUR";
  const TCollection_AsciiString Reco_SymbolRepresentation    = "SYMBOL_REPRESENTATION";
  const TCollection_AsciiString Reco_SymbolRepresentationMap = "SYMBOL_REPRESENTATION_MAP";
  const TCollection_AsciiString Reco_SymbolStyle             = "SYMBOL_STYLE";
  const TCollection_AsciiString Reco_SymbolTarget            = "SYMBOL_TARGET";
  const TCollection_AsciiString Reco_Template                = "TEMPLATE";
  const TCollection_AsciiString Reco_TemplateInstance        = "TEMPLATE_INSTANCE";
  const TCollection_AsciiString Reco_TerminatorSymbol        = "TERMINATOR_SYMBOL";
  const TCollection_AsciiString Reco_TextLiteral             = "TEXT_LITERAL";
  const TCollection_AsciiString Reco_TextLiteralWithAssociatedCurves =
    "TEXT_LITERAL_WITH_ASSOCIATED_CURVES";
  const TCollection_AsciiString Reco_TextLiteralWithBlankingBox = "TEXT_LITERAL_WITH_BLANKING_BOX";
  const TCollection_AsciiString Reco_TextLiteralWithDelineation = "TEXT_LITERAL_WITH_DELINEATION";
  const TCollection_AsciiString Reco_TextLiteralWithExtent      = "TEXT_LITERAL_WITH_EXTENT";
  const TCollection_AsciiString Reco_TextStyle                  = "TEXT_STYLE";
  const TCollection_AsciiString Reco_TextStyleForDefinedFont    = "TEXT_STYLE_FOR_DEFINED_FONT";
  const TCollection_AsciiString Reco_TextStyleWithBoxCharacteristics =
    "TEXT_STYLE_WITH_BOX_CHARACTERISTICS";
  const TCollection_AsciiString Reco_TextStyleWithMirror = "TEXT_STYLE_WITH_MIRROR";
  const TCollection_AsciiString Reco_TopologicalRepresentationItem =
    "TOPOLOGICAL_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_ToroidalSurface = "TOROIDAL_SURFACE";
  const TCollection_AsciiString Reco_Torus           = "TORUS";
  const TCollection_AsciiString Reco_TransitionalShapeRepresentation =
    "TRANSITIONAL_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_TrimmedCurve               = "TRIMMED_CURVE";
  const TCollection_AsciiString Reco_TwoDirectionRepeatFactor   = "TWO_DIRECTION_REPEAT_FACTOR";
  const TCollection_AsciiString Reco_UncertaintyMeasureWithUnit = "UNCERTAINTY_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_UniformCurve               = "UNIFORM_CURVE";
  const TCollection_AsciiString Reco_UniformSurface             = "UNIFORM_SURFACE";
  const TCollection_AsciiString Reco_Vector                     = "VECTOR";
  const TCollection_AsciiString Reco_Vertex                     = "VERTEX";
  const TCollection_AsciiString Reco_VertexLoop                 = "VERTEX_LOOP";
  const TCollection_AsciiString Reco_VertexPoint                = "VERTEX_POINT";
  const TCollection_AsciiString Reco_ViewVolume                 = "VIEW_VOLUME";
  const TCollection_AsciiString Reco_WeekOfYearAndDayDate       = "WEEK_OF_YEAR_AND_DAY_DATE";

  // Added by FMA for Rev4

  const TCollection_AsciiString Reco_SolidAngleUnit    = "SOLID_ANGLE_UNIT";
  const TCollection_AsciiString Reco_MechanicalContext = "MECHANICAL_CONTEXT";
  const TCollection_AsciiString Reco_DesignContext     = "DESIGN_CONTEXT";

  // Added for full Rev4
  const TCollection_AsciiString Reco_TimeMeasureWithUnit    = "TIME_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_RatioUnit              = "RATIO_UNIT";
  const TCollection_AsciiString Reco_TimeUnit               = "TIME_UNIT";
  const TCollection_AsciiString Reco_ApprovalDateTime       = "APPROVAL_DATE_TIME";
  const TCollection_AsciiString Reco_CameraImage2dWithScale = "CAMERA_IMAGE_2D_WITH_SCALE";
  const TCollection_AsciiString Reco_CameraImage3dWithScale = "CAMERA_IMAGE_3D_WITH_SCALE";
  const TCollection_AsciiString Reco_CartesianTransformationOperator2d =
    "CARTESIAN_TRANSFORMATION_OPERATOR_2D";
  const TCollection_AsciiString Reco_DerivedUnit                 = "DERIVED_UNIT";
  const TCollection_AsciiString Reco_DerivedUnitElement          = "DERIVED_UNIT_ELEMENT";
  const TCollection_AsciiString Reco_ItemDefinedTransformation   = "ITEM_DEFINED_TRANSFORMATION";
  const TCollection_AsciiString Reco_PresentedItemRepresentation = "PRESENTED_ITEM_REPRESENTATION";
  const TCollection_AsciiString Reco_PresentationLayerUsage      = "PRESENTATION_LAYER_USAGE";

  // Added for AP214 : CC1 -> CC2

  const TCollection_AsciiString Reco_AutoDesignDocumentReference = "AUTO_DESIGN_DOCUMENT_REFERENCE";
  const TCollection_AsciiString Reco_Document                    = "DOCUMENT";
  const TCollection_AsciiString Reco_DigitalDocument             = "DIGITAL_DOCUMENT";
  const TCollection_AsciiString Reco_DocumentRelationship        = "DOCUMENT_RELATIONSHIP";
  const TCollection_AsciiString Reco_DocumentType                = "DOCUMENT_TYPE";
  const TCollection_AsciiString Reco_DocumentUsageConstraint     = "DOCUMENT_USAGE_CONSTRAINT";
  const TCollection_AsciiString Reco_Effectivity                 = "EFFECTIVITY";
  const TCollection_AsciiString Reco_ProductDefinitionEffectivity =
    "PRODUCT_DEFINITION_EFFECTIVITY";
  const TCollection_AsciiString Reco_ProductDefinitionRelationship =
    "PRODUCT_DEFINITION_RELATIONSHIP";
  const TCollection_AsciiString Reco_ProductDefinitionWithAssociatedDocuments =
    "PRODUCT_DEFINITION_WITH_ASSOCIATED_DOCUMENTS";
  const TCollection_AsciiString Reco_PhysicallyModeledProductDefinition =
    "PHYSICALLY_MODELED_PRODUCT_DEFINITION";

  const TCollection_AsciiString Reco_ProductDefinitionUsage      = "PRODUCT_DEFINITION_USAGE";
  const TCollection_AsciiString Reco_MakeFromUsageOption         = "MAKE_FROM_USAGE_OPTION";
  const TCollection_AsciiString Reco_AssemblyComponentUsage      = "ASSEMBLY_COMPONENT_USAGE";
  const TCollection_AsciiString Reco_NextAssemblyUsageOccurrence = "NEXT_ASSEMBLY_USAGE_OCCURRENCE";
  const TCollection_AsciiString Reco_PromissoryUsageOccurrence   = "PROMISSORY_USAGE_OCCURRENCE";
  const TCollection_AsciiString Reco_QuantifiedAssemblyComponentUsage =
    "QUANTIFIED_ASSEMBLY_COMPONENT_USAGE";
  const TCollection_AsciiString Reco_SpecifiedHigherUsageOccurrence =
    "SPECIFIED_HIGHER_USAGE_OCCURRENCE";
  const TCollection_AsciiString Reco_AssemblyComponentUsageSubstitute =
    "ASSEMBLY_COMPONENT_USAGE_SUBSTITUTE";
  const TCollection_AsciiString Reco_SuppliedPartRelationship = "SUPPLIED_PART_RELATIONSHIP";
  const TCollection_AsciiString Reco_ExternallyDefinedRepresentation =
    "EXTERNALLY_DEFINED_REPRESENTATION";
  const TCollection_AsciiString Reco_ShapeRepresentationRelationship =
    "SHAPE_REPRESENTATION_RELATIONSHIP";
  const TCollection_AsciiString Reco_RepresentationRelationshipWithTransformation =
    "REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION";
  const TCollection_AsciiString Reco_MaterialDesignation = "MATERIAL_DESIGNATION";
  const TCollection_AsciiString Reco_ContextDependentShapeRepresentation =
    "CONTEXT_DEPENDENT_SHAPE_REPRESENTATION";

  // Added from CD To DIS (S4134)
  const TCollection_AsciiString Reco_AppliedDateAndTimeAssignment =
    "APPLIED_DATE_AND_TIME_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedDateAssignment     = "APPLIED_DATE_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedApprovalAssignment = "APPLIED_APPROVAL_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedDocumentReference  = "APPLIED_DOCUMENT_REFERENCE";
  const TCollection_AsciiString Reco_AppliedGroupAssignment    = "APPLIED_GROUP_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedOrganizationAssignment =
    "APPLIED_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedPersonAndOrganizationAssignment =
    "APPLIED_PERSON_AND_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_AppliedPresentedItem = "APPLIED_PRESENTED_ITEM";
  const TCollection_AsciiString Reco_AppliedSecurityClassificationAssignment =
    "APPLIED_SECURITY_CLASSIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_DocumentFile        = "DOCUMENT_FILE";
  const TCollection_AsciiString Reco_CharacterizedObject = "CHARACTERIZED_OBJECT";
  const TCollection_AsciiString Reco_ExtrudedFaceSolid   = "EXTRUDED_FACE_SOLID";
  const TCollection_AsciiString Reco_RevolvedFaceSolid   = "REVOLVED_FACE_SOLID";
  const TCollection_AsciiString Reco_SweptFaceSolid      = "SWEPT_FACE_SOLID";

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  const TCollection_AsciiString Reco_MeasureRepresentationItem = "MEASURE_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_AreaUnit                  = "AREA_UNIT";
  const TCollection_AsciiString Reco_VolumeUnit                = "VOLUME_UNIT";

  // Added by ABV 10.11.99 for AP203
  const TCollection_AsciiString Reco_Action                  = "ACTION";
  const TCollection_AsciiString Reco_ActionAssignment        = "ACTION_ASSIGNMENT";
  const TCollection_AsciiString Reco_ActionMethod            = "ACTION_METHOD";
  const TCollection_AsciiString Reco_ActionRequestAssignment = "ACTION_REQUEST_ASSIGNMENT";
  const TCollection_AsciiString Reco_CcDesignApproval        = "CC_DESIGN_APPROVAL";
  const TCollection_AsciiString Reco_CcDesignCertification   = "CC_DESIGN_CERTIFICATION";
  const TCollection_AsciiString Reco_CcDesignContract        = "CC_DESIGN_CONTRACT";
  const TCollection_AsciiString Reco_CcDesignDateAndTimeAssignment =
    "CC_DESIGN_DATE_AND_TIME_ASSIGNMENT";
  const TCollection_AsciiString Reco_CcDesignPersonAndOrganizationAssignment =
    "CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_CcDesignSecurityClassification =
    "CC_DESIGN_SECURITY_CLASSIFICATION";
  const TCollection_AsciiString Reco_CcDesignSpecificationReference =
    "CC_DESIGN_SPECIFICATION_REFERENCE";
  const TCollection_AsciiString Reco_Certification               = "CERTIFICATION";
  const TCollection_AsciiString Reco_CertificationAssignment     = "CERTIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_CertificationType           = "CERTIFICATION_TYPE";
  const TCollection_AsciiString Reco_Change                      = "CHANGE";
  const TCollection_AsciiString Reco_ChangeRequest               = "CHANGE_REQUEST";
  const TCollection_AsciiString Reco_ConfigurationDesign         = "CONFIGURATION_DESIGN";
  const TCollection_AsciiString Reco_ConfigurationEffectivity    = "CONFIGURATION_EFFECTIVITY";
  const TCollection_AsciiString Reco_Contract                    = "CONTRACT";
  const TCollection_AsciiString Reco_ContractAssignment          = "CONTRACT_ASSIGNMENT";
  const TCollection_AsciiString Reco_ContractType                = "CONTRACT_TYPE";
  const TCollection_AsciiString Reco_ProductConcept              = "PRODUCT_CONCEPT";
  const TCollection_AsciiString Reco_ProductConceptContext       = "PRODUCT_CONCEPT_CONTEXT";
  const TCollection_AsciiString Reco_StartRequest                = "START_REQUEST";
  const TCollection_AsciiString Reco_StartWork                   = "START_WORK";
  const TCollection_AsciiString Reco_VersionedActionRequest      = "VERSIONED_ACTION_REQUEST";
  const TCollection_AsciiString Reco_ProductCategoryRelationship = "PRODUCT_CATEGORY_RELATIONSHIP";
  const TCollection_AsciiString Reco_ActionRequestSolution       = "ACTION_REQUEST_SOLUTION";
  const TCollection_AsciiString Reco_DraughtingModel             = "DRAUGHTING_MODEL";
  // Added by ABV 18.04.00 for CAX-IF TRJ4
  const TCollection_AsciiString Reco_AngularLocation = "ANGULAR_LOCATION";
  const TCollection_AsciiString Reco_AngularSize     = "ANGULAR_SIZE";
  const TCollection_AsciiString Reco_DimensionalCharacteristicRepresentation =
    "DIMENSIONAL_CHARACTERISTIC_REPRESENTATION";
  const TCollection_AsciiString Reco_DimensionalLocation         = "DIMENSIONAL_LOCATION";
  const TCollection_AsciiString Reco_DimensionalLocationWithPath = "DIMENSIONAL_LOCATION_WITH_PATH";
  const TCollection_AsciiString Reco_DimensionalSize             = "DIMENSIONAL_SIZE";
  const TCollection_AsciiString Reco_DimensionalSizeWithPath     = "DIMENSIONAL_SIZE_WITH_PATH";
  const TCollection_AsciiString Reco_ShapeDimensionRepresentation =
    "SHAPE_DIMENSION_REPRESENTATION";
  // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  const TCollection_AsciiString Reco_DocumentRepresentationType = "DOCUMENT_REPRESENTATION_TYPE";
  const TCollection_AsciiString Reco_ObjectRole                 = "OBJECT_ROLE";
  const TCollection_AsciiString Reco_RoleAssociation            = "ROLE_ASSOCIATION";
  const TCollection_AsciiString Reco_IdentificationRole         = "IDENTIFICATION_ROLE";
  const TCollection_AsciiString Reco_IdentificationAssignment   = "IDENTIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_ExternalIdentificationAssignment =
    "EXTERNAL_IDENTIFICATION_ASSIGNMENT";
  const TCollection_AsciiString Reco_EffectivityAssignment  = "EFFECTIVITY_ASSIGNMENT";
  const TCollection_AsciiString Reco_NameAssignment         = "NAME_ASSIGNMENT";
  const TCollection_AsciiString Reco_GeneralProperty        = "GENERAL_PROPERTY";
  const TCollection_AsciiString Reco_Class                  = "CLASS";
  const TCollection_AsciiString Reco_ExternallyDefinedClass = "EXTERNALLY_DEFINED_Class";
  const TCollection_AsciiString Reco_ExternallyDefinedGeneralProperty =
    "EXTERNALLY_DEFINED_GENERAL_PROPERTY";
  const TCollection_AsciiString Reco_AppliedExternalIdentificationAssignment =
    "APPLIED_EXTERNAL_IDENTIFICATION_ASSIGNMENT";
  // Added by CKY , 25 APR 2001 for Dimensional Tolerances (CAX-IF TRJ7)
  const TCollection_AsciiString Reco_CompositeShapeAspect        = "COMPOSITE_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_DerivedShapeAspect          = "DERIVED_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_Extension                   = "EXTENSION";
  const TCollection_AsciiString Reco_DirectedDimensionalLocation = "DIRECTED_DIMENSIONAL_LOCATION";
  const TCollection_AsciiString Reco_LimitsAndFits               = "LIMITS_AND_FITS";
  const TCollection_AsciiString Reco_ToleranceValue              = "TOLERANCE_VALUE";
  const TCollection_AsciiString Reco_MeasureQualification        = "MEASURE_QUALIFICATION";
  const TCollection_AsciiString Reco_PlusMinusTolerance          = "PLUS_MINUS_TOLERANCE";
  const TCollection_AsciiString Reco_PrecisionQualifier          = "PRECISION_QUALIFIER";
  const TCollection_AsciiString Reco_TypeQualifier               = "TYPE_QUALIFIER";
  const TCollection_AsciiString Reco_QualifiedRepresentationItem = "QUALIFIED_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_CompoundRepresentationItem  = "COMPOUND_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_ValueRange                  = "VALUE_RANGE";
  const TCollection_AsciiString Reco_ShapeAspectDerivingRelationship =
    "SHAPE_ASPECT_DERIVING_RELATIONSHIP";

  const TCollection_AsciiString Reco_CompoundShapeRepresentation = "COMPOUND_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_ConnectedEdgeSet            = "CONNECTED_EDGE_SET";
  const TCollection_AsciiString Reco_ConnectedFaceShapeRepresentation =
    "CONNECTED_FACE_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_EdgeBasedWireframeModel = "EDGE_BASED_WIREFRAME_MODEL";
  const TCollection_AsciiString Reco_EdgeBasedWireframeShapeRepresentation =
    "EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_FaceBasedSurfaceModel = "FACE_BASED_SURFACE_MODEL";
  const TCollection_AsciiString Reco_NonManifoldSurfaceShapeRepresentation =
    "NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION";

  // gka 08.01.02
  const TCollection_AsciiString Reco_OrientedSurface     = "ORIENTED_SURFACE";
  const TCollection_AsciiString Reco_Subface             = "SUBFACE";
  const TCollection_AsciiString Reco_Subedge             = "SUBEDGE";
  const TCollection_AsciiString Reco_SeamEdge            = "SEAM_EDGE";
  const TCollection_AsciiString Reco_ConnectedFaceSubSet = "CONNECTED_FACE_SUB_SET";

  // Added for AP209
  const TCollection_AsciiString Reco_EulerAngles         = "EULER_ANGLES";
  const TCollection_AsciiString Reco_MassUnit            = "MASS_UNIT";
  const TCollection_AsciiString Reco_MassMeasureWithUnit = "MASS_MEASURE_WITH_UNIT";
  const TCollection_AsciiString Reco_ThermodynamicTemperatureUnit =
    "THERMODYNAMIC_TEMPERATURE_UNIT";
  const TCollection_AsciiString Reco_AnalysisItemWithinRepresentation =
    "ANALYSIS_ITEM_WITHIN_REPRESENTATION";
  const TCollection_AsciiString Reco_Curve3dElementDescriptor = "CURVE_3D_ELEMENT_DESCRIPTOR";
  const TCollection_AsciiString Reco_CurveElementEndReleasePacket =
    "CURVE_ELEMENT_END_RELEASE_PACKET";
  const TCollection_AsciiString Reco_CurveElementSectionDefinition =
    "CURVE_ELEMENT_SECTION_DEFINITION";
  const TCollection_AsciiString Reco_CurveElementSectionDerivedDefinitions =
    "CURVE_ELEMENT_SECTION_DERIVED_DEFINITIONS";
  const TCollection_AsciiString Reco_ElementDescriptor           = "ELEMENT_DESCRIPTOR";
  const TCollection_AsciiString Reco_ElementMaterial             = "ELEMENT_MATERIAL";
  const TCollection_AsciiString Reco_Surface3dElementDescriptor  = "SURFACE_3D_ELEMENT_DESCRIPTOR";
  const TCollection_AsciiString Reco_SurfaceElementProperty      = "SURFACE_ELEMENT_PROPERTY";
  const TCollection_AsciiString Reco_SurfaceSection              = "SURFACE_SECTION";
  const TCollection_AsciiString Reco_SurfaceSectionField         = "SURFACE_SECTION_FIELD";
  const TCollection_AsciiString Reco_SurfaceSectionFieldConstant = "SURFACE_SECTION_FIELD_CONSTANT";
  const TCollection_AsciiString Reco_SurfaceSectionFieldVarying  = "SURFACE_SECTION_FIELD_VARYING";
  const TCollection_AsciiString Reco_UniformSurfaceSection       = "UNIFORM_SURFACE_SECTION";
  const TCollection_AsciiString Reco_Volume3dElementDescriptor   = "VOLUME_3D_ELEMENT_DESCRIPTOR";
  const TCollection_AsciiString Reco_AlignedCurve3dElementCoordinateSystem =
    "ALIGNED_CURVE_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_ArbitraryVolume3dElementCoordinateSystem =
    "ARBITRARY_VOLUME_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_Curve3dElementProperty = "CURVE_3D_ELEMENT_PROPERTY";
  const TCollection_AsciiString Reco_Curve3dElementRepresentation =
    "CURVE_3D_ELEMENT_REPRESENTATION";
  const TCollection_AsciiString Reco_Node = "NODE";
  // const TCollection_AsciiString Reco_CurveElementEndCoordinateSystem(" ");
  const TCollection_AsciiString Reco_CurveElementEndOffset  = "CURVE_ELEMENT_END_OFFSET";
  const TCollection_AsciiString Reco_CurveElementEndRelease = "CURVE_ELEMENT_END_RELEASE";
  const TCollection_AsciiString Reco_CurveElementInterval   = "CURVE_ELEMENT_INTERVAL";
  const TCollection_AsciiString Reco_CurveElementIntervalConstant =
    "CURVE_ELEMENT_INTERVAL_CONSTANT";
  const TCollection_AsciiString Reco_DummyNode            = "DUMMY_NODE";
  const TCollection_AsciiString Reco_CurveElementLocation = "CURVE_ELEMENT_LOCATION";
  const TCollection_AsciiString Reco_ElementGeometricRelationship =
    "ELEMENT_GEOMETRIC_RELATIONSHIP";
  const TCollection_AsciiString Reco_ElementGroup          = "ELEMENT_GROUP";
  const TCollection_AsciiString Reco_ElementRepresentation = "ELEMENT_REPRESENTATION";
  const TCollection_AsciiString Reco_FeaAreaDensity        = "FEA_AREA_DENSITY";
  const TCollection_AsciiString Reco_FeaAxis2Placement3d   = "FEA_AXIS2_PLACEMENT_3D";
  const TCollection_AsciiString Reco_FeaGroup              = "FEA_GROUP";
  const TCollection_AsciiString Reco_FeaLinearElasticity   = "FEA_LINEAR_ELASTICITY";
  const TCollection_AsciiString Reco_FeaMassDensity        = "FEA_MASS_DENSITY";
  const TCollection_AsciiString Reco_FeaMaterialPropertyRepresentation =
    "FEA_MATERIAL_PROPERTY_REPRESENTATION";
  const TCollection_AsciiString Reco_FeaMaterialPropertyRepresentationItem =
    "FEA_MATERIAL_PROPERTY_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_FeaModel              = "FEA_MODEL";
  const TCollection_AsciiString Reco_FeaModel3d            = "FEA_MODEL_3D";
  const TCollection_AsciiString Reco_FeaMoistureAbsorption = "FEA_MOISTURE_ABSORPTION";
  const TCollection_AsciiString Reco_FeaParametricPoint    = "FEA_PARAMETRIC_POINT";
  const TCollection_AsciiString Reco_FeaRepresentationItem = "FEA_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_FeaSecantCoefficientOfLinearThermalExpansion =
    "FEA_SECANT_COEFFICIENT_OF_LINEAR_THERMAL_EXPANSION";
  const TCollection_AsciiString Reco_FeaShellBendingStiffness = "FEA_SHELL_BENDING_STIFFNESS";
  const TCollection_AsciiString Reco_FeaShellMembraneBendingCouplingStiffness =
    "FEA_SHELL_MEMBRANE_BENDING_COUPLING_STIFFNESS";
  const TCollection_AsciiString Reco_FeaShellMembraneStiffness = "FEA_SHELL_MEMBRANE_STIFFNESS";
  const TCollection_AsciiString Reco_FeaShellShearStiffness    = "FEA_SHELL_SHEAR_STIFFNESS";
  const TCollection_AsciiString Reco_GeometricNode             = "GEOMETRIC_NODE";
  const TCollection_AsciiString Reco_FeaTangentialCoefficientOfLinearThermalExpansion =
    "FEA_TANGENTIAL_COEFFICIENT_OF_LINEAR_THERMAL_EXPANSION";
  const TCollection_AsciiString Reco_NodeGroup          = "NODE_GROUP";
  const TCollection_AsciiString Reco_NodeRepresentation = "NODE_REPRESENTATION";
  const TCollection_AsciiString Reco_NodeSet            = "NODE_SET";
  const TCollection_AsciiString Reco_NodeWithSolutionCoordinateSystem =
    "NODE_WITH_SOLUTION_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_NodeWithVector = "NODE_WITH_VECTOR";
  const TCollection_AsciiString Reco_ParametricCurve3dElementCoordinateDirection =
    "PARAMETRIC_CURVE_3D_ELEMENT_COORDINATE_DIRECTION";
  const TCollection_AsciiString Reco_ParametricCurve3dElementCoordinateSystem =
    "PARAMETRIC_CURVE_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_ParametricSurface3dElementCoordinateSystem =
    "PARAMETRIC_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_Surface3dElementRepresentation =
    "SURFACE_3D_ELEMENT_REPRESENTATION";
  // const TCollection_AsciiString Reco_SymmetricTensor22d(" ");
  // const TCollection_AsciiString Reco_SymmetricTensor42d(" ");
  // const TCollection_AsciiString Reco_SymmetricTensor43d(" ");
  const TCollection_AsciiString Reco_Volume3dElementRepresentation =
    "VOLUME_3D_ELEMENT_REPRESENTATION";
  const TCollection_AsciiString Reco_DataEnvironment = "DATA_ENVIRONMENT";
  const TCollection_AsciiString Reco_MaterialPropertyRepresentation =
    "MATERIAL_PROPERTY_REPRESENTATION";
  const TCollection_AsciiString Reco_PropertyDefinitionRelationship =
    "PROPERTY_DEFINITION_RELATIONSHIP";
  const TCollection_AsciiString Reco_PointRepresentation   = "POINT_REPRESENTATION";
  const TCollection_AsciiString Reco_MaterialProperty      = "MATERIAL_PROPERTY";
  const TCollection_AsciiString Reco_FeaModelDefinition    = "FEA_MODEL_DEFINITION";
  const TCollection_AsciiString Reco_FreedomAndCoefficient = "FREEDOM_AND_COEFFICIENT";
  const TCollection_AsciiString Reco_FreedomsList          = "FREEDOMS_LIST";
  const TCollection_AsciiString Reco_ProductDefinitionFormationRelationship =
    "PRODUCT_DEFINITION_FORMATION_RELATIONSHIP";
  // const TCollection_AsciiString Reco_FeaModelDefinition= "FEA_MODEL_DEFINITION";
  const TCollection_AsciiString Reco_NodeDefinition             = "NODE_DEFINITION";
  const TCollection_AsciiString Reco_StructuralResponseProperty = "STRUCTURAL_RESPONSE_PROPERTY";
  const TCollection_AsciiString Reco_StructuralResponsePropertyDefinitionRepresentation =
    "STRUCTURAL_RESPONSE_PROPERTY_DEFINITION_REPRESENTATION";
  const TCollection_AsciiString Reco_AlignedSurface3dElementCoordinateSystem =
    "ALIGNED_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_ConstantSurface3dElementCoordinateSystem =
    "CONSTANT_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM";
  const TCollection_AsciiString Reco_CurveElementIntervalLinearlyVarying =
    "CURVE_ELEMENT_INTERVAL_LINEARLY_VARYING";
  const TCollection_AsciiString Reco_FeaCurveSectionGeometricRelationship =
    "FEA_CURVE_SECTION_GEOMETRIC_RELATIONSHIP";
  const TCollection_AsciiString Reco_FeaSurfaceSectionGeometricRelationship =
    "FEA_SURFACE_SECTION_GEOMETRIC_RELATIONSHIP";

  // PTV 28.01.2003 TRJ11 AP214 external references
  const TCollection_AsciiString Reco_DocumentProductAssociation = "DOCUMENT_PRODUCT_ASSOCIATION";
  const TCollection_AsciiString Reco_DocumentProductEquivalence = "DOCUMENT_PRODUCT_EQUIVALENCE";

  // Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
  const TCollection_AsciiString Reco_ShapeRepresentationWithParameters =
    "SHAPE_REPRESENTATION_WITH_PARAMETERS";
  const TCollection_AsciiString Reco_AngularityTolerance       = "ANGULARITY_TOLERANCE";
  const TCollection_AsciiString Reco_ConcentricityTolerance    = "CONCENTRICITY_TOLERANCE";
  const TCollection_AsciiString Reco_CircularRunoutTolerance   = "CIRCULAR_RUNOUT_TOLERANCE";
  const TCollection_AsciiString Reco_CoaxialityTolerance       = "COAXIALITY_TOLERANCE";
  const TCollection_AsciiString Reco_CylindricityTolerance     = "CYLINDRICITY_TOLERANCE";
  const TCollection_AsciiString Reco_FlatnessTolerance         = "FLATNESS_TOLERANCE";
  const TCollection_AsciiString Reco_LineProfileTolerance      = "LINE_PROFILE_TOLERANCE";
  const TCollection_AsciiString Reco_ParallelismTolerance      = "PARALLELISM_TOLERANCE";
  const TCollection_AsciiString Reco_PerpendicularityTolerance = "PERPENDICULARITY_TOLERANCE";
  const TCollection_AsciiString Reco_PositionTolerance         = "POSITION_TOLERANCE";
  const TCollection_AsciiString Reco_RoundnessTolerance        = "ROUNDNESS_TOLERANCE";
  const TCollection_AsciiString Reco_StraightnessTolerance     = "STRAIGHTNESS_TOLERANCE";
  const TCollection_AsciiString Reco_SurfaceProfileTolerance   = "SURFACE_PROFILE_TOLERANCE";
  const TCollection_AsciiString Reco_SymmetryTolerance         = "SYMMETRY_TOLERANCE";
  const TCollection_AsciiString Reco_TotalRunoutTolerance      = "TOTAL_RUNOUT_TOLERANCE";
  const TCollection_AsciiString Reco_GeometricTolerance        = "GEOMETRIC_TOLERANCE";
  const TCollection_AsciiString Reco_GeometricToleranceRelationship =
    "GEOMETRIC_TOLERANCE_RELATIONSHIP";
  const TCollection_AsciiString Reco_GeometricToleranceWithDatumReference =
    "GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE";
  const TCollection_AsciiString Reco_ModifiedGeometricTolerance = "MODIFIED_GEOMETRIC_TOLERANCE";
  const TCollection_AsciiString Reco_Datum                      = "DATUM";
  const TCollection_AsciiString Reco_DatumFeature               = "DATUM_FEATURE";
  const TCollection_AsciiString Reco_DatumReference             = "DATUM_REFERENCE";
  const TCollection_AsciiString Reco_CommonDatum                = "COMMON_DATUM";
  const TCollection_AsciiString Reco_DatumTarget                = "DATUM_TARGET";
  const TCollection_AsciiString Reco_PlacedDatumTargetFeature   = "PLACED_DATUM_TARGET_FEATURE";

  // Added by ika for GD&T AP242
  const TCollection_AsciiString Reco_Apex                       = "APEX";
  const TCollection_AsciiString Reco_CentreOfSymmetry           = "CENTRE_OF_SYMMETRY";
  const TCollection_AsciiString Reco_GeometricAlignment         = "GEOMETRIC_ALIGNMENT";
  const TCollection_AsciiString Reco_PerpendicularTo            = "PERPENDICULAR_TO";
  const TCollection_AsciiString Reco_Tangent                    = "TANGENT";
  const TCollection_AsciiString Reco_ParallelOffset             = "PARALLEL_OFFSET";
  const TCollection_AsciiString Reco_GeometricItemSpecificUsage = "GEOMETRIC_ITEM_SPECIFIC_USAGE";
  const TCollection_AsciiString Reco_IdAttribute                = "ID_ATTRIBUTE";
  const TCollection_AsciiString Reco_ItemIdentifiedRepresentationUsage =
    "ITEM_IDENTIFIED_REPRESENTATION_USAGE";
  const TCollection_AsciiString Reco_AllAroundShapeAspect      = "ALL_AROUND_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_BetweenShapeAspect        = "BETWEEN_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_CompositeGroupShapeAspect = "COMPOSITE_GROUP_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_ContinuosShapeAspect      = "CONTINUOUS_SHAPE_ASPECT";
  const TCollection_AsciiString Reco_GeometricToleranceWithDefinedAreaUnit =
    "GEOMETRIC_TOLERANCE_WITH_DEFINED_AREA_UNIT";
  const TCollection_AsciiString Reco_GeometricToleranceWithDefinedUnit =
    "GEOMETRIC_TOLERANCE_WITH_DEFINED_UNIT";
  const TCollection_AsciiString Reco_GeometricToleranceWithMaximumTolerance =
    "GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE";
  const TCollection_AsciiString Reco_GeometricToleranceWithModifiers =
    "GEOMETRIC_TOLERANCE_WITH_MODIFIERS";
  const TCollection_AsciiString Reco_UnequallyDisposedGeometricTolerance =
    "UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE";
  const TCollection_AsciiString Reco_NonUniformZoneDefinition  = "NON_UNIFORM_ZONE_DEFINITION";
  const TCollection_AsciiString Reco_ProjectedZoneDefinition   = "PROJECTED_ZONE_DEFINITION";
  const TCollection_AsciiString Reco_RunoutZoneDefinition      = "RUNOUT_ZONE_DEFINITION";
  const TCollection_AsciiString Reco_RunoutZoneOrientation     = "RUNOUT_ZONE_ORIENTATION";
  const TCollection_AsciiString Reco_ToleranceZone             = "TOLERANCE_ZONE";
  const TCollection_AsciiString Reco_ToleranceZoneDefinition   = "TOLERANCE_ZONE_DEFINITION";
  const TCollection_AsciiString Reco_ToleranceZoneForm         = "TOLERANCE_ZONE_FORM";
  const TCollection_AsciiString Reco_ValueFormatTypeQualifier  = "VALUE_FORMAT_TYPE_QUALIFIER";
  const TCollection_AsciiString Reco_DatumReferenceCompartment = "DATUM_REFERENCE_COMPARTMENT";
  const TCollection_AsciiString Reco_DatumReferenceElement     = "DATUM_REFERENCE_ELEMENT";
  const TCollection_AsciiString Reco_DatumReferenceModifierWithValue =
    "DATUM_REFERENCE_MODIFIER_WITH_VALUE";
  const TCollection_AsciiString Reco_DatumSystem               = "DATUM_SYSTEM";
  const TCollection_AsciiString Reco_GeneralDatumReference     = "GENERAL_DATUM_REFERENCE";
  const TCollection_AsciiString Reco_IntegerRepresentationItem = "INTEGER_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_ValueRepresentationItem   = "VALUE_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_FeatureForDatumTargetRelationship =
    "FEATURE_FOR_DATUM_TARGET_RELATIONSHIP";
  const TCollection_AsciiString Reco_DraughtingModelItemAssociation =
    "DRAUGHTING_MODEL_ITEM_ASSOCIATION";
  const TCollection_AsciiString Reco_AnnotationPlane = "ANNOTATION_PLANE";

  const TCollection_AsciiString Reco_TessellatedAnnotationOccurrence =
    "TESSELLATED_ANNOTATION_OCCURRENCE";
  const TCollection_AsciiString Reco_TessellatedGeometricSet     = "TESSELLATED_GEOMETRIC_SET";
  const TCollection_AsciiString Reco_TessellatedCurveSet         = "TESSELLATED_CURVE_SET";
  const TCollection_AsciiString Reco_TessellatedItem             = "TESSELLATED_ITEM";
  const TCollection_AsciiString Reco_RepositionedTessellatedItem = "REPOSITIONED_TESSELLATED_ITEM";
  const TCollection_AsciiString Reco_CoordinatesList             = "COORDINATES_LIST";
  const TCollection_AsciiString Reco_ConstructiveGeometryRepresentation =
    "CONSTRUCTIVE_GEOMETRY_REPRESENTATION";
  const TCollection_AsciiString Reco_ConstructiveGeometryRepresentationRelationship =
    "CONSTRUCTIVE_GEOMETRY_REPRESENTATION_RELATIONSHIP";
  const TCollection_AsciiString Reco_CharacterizedRepresentation = "CHARACTERIZED_REPRESENTATION";
  const TCollection_AsciiString Reco_CameraModelD3MultiClipping  = "CAMERA_MODEL_D3_MULTI_CLIPPING";
  const TCollection_AsciiString Reco_CameraModelD3MultiClippingIntersection =
    "CAMERA_MODEL_D3_MULTI_CLIPPING_INTERSECTION";
  const TCollection_AsciiString Reco_CameraModelD3MultiClippingUnion =
    "CAMERA_MODEL_D3_MULTI_CLIPPING_UNION";

  const TCollection_AsciiString Reco_SurfaceStyleTransparent = "SURFACE_STYLE_TRANSPARENT";
  const TCollection_AsciiString Reco_SurfaceStyleReflectanceAmbient =
    "SURFACE_STYLE_REFLECTANCE_AMBIENT";
  const TCollection_AsciiString Reco_SurfaceStyleRendering = "SURFACE_STYLE_RENDERING";
  const TCollection_AsciiString Reco_SurfaceStyleRenderingWithProperties =
    "SURFACE_STYLE_RENDERING_WITH_PROPERTIES";

  const TCollection_AsciiString Reco_RepresentationContextReference =
    "REPRESENTATION_CONTEXT_REFERENCE";
  const TCollection_AsciiString Reco_RepresentationReference = "REPRESENTATION_REFERENCE";
  const TCollection_AsciiString Reco_SuParameters            = "SU_PARAMETERS";
  const TCollection_AsciiString Reco_RotationAboutDirection  = "ROTATION_ABOUT_DIRECTION";
  const TCollection_AsciiString Reco_KinematicJoint          = "KINEMATIC_JOINT";
  const TCollection_AsciiString Reco_ActuatedKinematicPair   = "ACTUATED_KINEMATIC_PAIR";
  const TCollection_AsciiString Reco_ContextDependentKinematicLinkRepresentation =
    "CONTEXT_DEPENDENT_KINEMATIC_LINK_REPRESENTATION";
  const TCollection_AsciiString Reco_CylindricalPair          = "CYLINDRICAL_PAIR";
  const TCollection_AsciiString Reco_CylindricalPairValue     = "CYLINDRICAL_PAIR_VALUE";
  const TCollection_AsciiString Reco_CylindricalPairWithRange = "CYLINDRICAL_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_FullyConstrainedPair     = "FULLY_CONSTRAINED_PAIR";
  const TCollection_AsciiString Reco_GearPair                 = "GEAR_PAIR";
  const TCollection_AsciiString Reco_GearPairValue            = "GEAR_PAIR_VALUE";
  const TCollection_AsciiString Reco_GearPairWithRange        = "GEAR_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_HomokineticPair          = "HOMOKINETIC_PAIR";
  const TCollection_AsciiString Reco_KinematicLink            = "KINEMATIC_LINK";
  const TCollection_AsciiString Reco_KinematicLinkRepresentationAssociation =
    "KINEMATIC_LINK_REPRESENTATION_ASSOCIATION";
  const TCollection_AsciiString Reco_KinematicPropertyMechanismRepresentation =
    "KINEMATIC_PROPERTY_MECHANISM_REPRESENTATION";
  const TCollection_AsciiString Reco_KinematicTopologyDirectedStructure =
    "KINEMATIC_TOPOLOGY_DIRECTED_STRUCTURE";
  const TCollection_AsciiString Reco_KinematicTopologyNetworkStructure =
    "KINEMATIC_TOPOLOGY_NETWORK_STRUCTURE";
  const TCollection_AsciiString Reco_KinematicTopologyStructure = "KINEMATIC_TOPOLOGY_STRUCTURE";
  const TCollection_AsciiString Reco_LinearFlexibleAndPinionPair =
    "LINEAR_FLEXIBLE_AND_PINION_PAIR";
  const TCollection_AsciiString Reco_LinearFlexibleAndPlanarCurvePair =
    "LINEAR_FLEXIBLE_AND_PLANAR_CURVE_PAIR";
  const TCollection_AsciiString Reco_LinearFlexibleLinkRepresentation =
    "LINEAR_FLEXIBLE_LINK_REPRESENTATION";
  const TCollection_AsciiString Reco_LowOrderKinematicPair      = "LOW_ORDER_KINEMATIC_PAIR";
  const TCollection_AsciiString Reco_LowOrderKinematicPairValue = "LOW_ORDER_KINEMATIC_PAIR_VALUE";
  const TCollection_AsciiString Reco_LowOrderKinematicPairWithRange =
    "LOW_ORDER_KINEMATIC_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_MechanismRepresentation = "MECHANISM_REPRESENTATION";
  const TCollection_AsciiString Reco_OrientedJoint           = "ORIENTED_JOINT";
  const TCollection_AsciiString Reco_PairRepresentationRelationship =
    "PAIR_REPRESENTATION_RELATIONSHIP";
  const TCollection_AsciiString Reco_PlanarCurvePair        = "PLANAR_CURVE_PAIR";
  const TCollection_AsciiString Reco_PlanarCurvePairRange   = "PLANAR_CURVE_PAIR_RANGE";
  const TCollection_AsciiString Reco_PlanarPair             = "PLANAR_PAIR";
  const TCollection_AsciiString Reco_PlanarPairValue        = "PLANAR_PAIR_VALUE";
  const TCollection_AsciiString Reco_PlanarPairWithRange    = "PLANAR_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_PointOnPlanarCurvePair = "POINT_ON_PLANAR_CURVE_PAIR";
  const TCollection_AsciiString Reco_PointOnPlanarCurvePairValue =
    "POINT_ON_PLANAR_CURVE_PAIR_VALUE";
  const TCollection_AsciiString Reco_PointOnPlanarCurvePairWithRange =
    "POINT_ON_PLANAR_CURVE_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_PointOnSurfacePair      = "POINT_ON_SURFACE_PAIR";
  const TCollection_AsciiString Reco_PointOnSurfacePairValue = "POINT_ON_SURFACE_PAIR_VALUE";
  const TCollection_AsciiString Reco_PointOnSurfacePairWithRange =
    "POINT_ON_SURFACE_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_PrismaticPair               = "PRISMATIC_PAIR";
  const TCollection_AsciiString Reco_PrismaticPairValue          = "PRISMATIC_PAIR_VALUE";
  const TCollection_AsciiString Reco_PrismaticPairWithRange      = "PRISMATIC_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_ProductDefinitionKinematics = "PRODUCT_DEFINITION_KINEMATICS";
  const TCollection_AsciiString Reco_ProductDefinitionRelationshipKinematics =
    "PRODUCT_DEFINITION_RELATIONSHIP_KINEMATICS";
  const TCollection_AsciiString Reco_RackAndPinionPair          = "RACK_AND_PINION_PAIR";
  const TCollection_AsciiString Reco_RackAndPinionPairValue     = "RACK_AND_PINION_PAIR_VALUE";
  const TCollection_AsciiString Reco_RackAndPinionPairWithRange = "RACK_AND_PINION_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_RevolutePair               = "REVOLUTE_PAIR";
  const TCollection_AsciiString Reco_RevolutePairValue          = "REVOLUTE_PAIR_VALUE";
  const TCollection_AsciiString Reco_RevolutePairWithRange      = "REVOLUTE_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_RigidLinkRepresentation    = "RIGID_LINK_REPRESENTATION";
  const TCollection_AsciiString Reco_RollingCurvePair           = "ROLLING_CURVE_PAIR";
  const TCollection_AsciiString Reco_RollingCurvePairValue      = "ROLLING_CURVE_PAIR_VALUE";
  const TCollection_AsciiString Reco_RollingSurfacePair         = "ROLLING_SURFACE_PAIR";
  const TCollection_AsciiString Reco_RollingSurfacePairValue    = "ROLLING_SURFACE_PAIR_VALUE";
  const TCollection_AsciiString Reco_ScrewPair                  = "SCREW_PAIR";
  const TCollection_AsciiString Reco_ScrewPairValue             = "SCREW_PAIR_VALUE";
  const TCollection_AsciiString Reco_ScrewPairWithRange         = "SCREW_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_SlidingCurvePair           = "SLIDING_CURVE_PAIR";
  const TCollection_AsciiString Reco_SlidingCurvePairValue      = "SLIDING_CURVE_PAIR_VALUE";
  const TCollection_AsciiString Reco_SlidingSurfacePair         = "SLIDING_SURFACE_PAIR";
  const TCollection_AsciiString Reco_SlidingSurfacePairValue    = "SLIDING_SURFACE_PAIR_VALUE";
  const TCollection_AsciiString Reco_SphericalPair              = "SPHERICAL_PAIR";
  const TCollection_AsciiString Reco_SphericalPairValue         = "SPHERICAL_PAIR_VALUE";
  const TCollection_AsciiString Reco_SphericalPairWithPin       = "SPHERICAL_PAIR_WITH_PIN";
  const TCollection_AsciiString Reco_SphericalPairWithPinAndRange =
    "SPHERICAL_PAIR_WITH_PIN_AND_RANGE";
  const TCollection_AsciiString Reco_SphericalPairWithRange = "SPHERICAL_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_SurfacePairWithRange   = "SURFACE_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_UnconstrainedPair      = "UNCONSTRAINED_PAIR";
  const TCollection_AsciiString Reco_UnconstrainedPairValue = "UNCONSTRAINED_PAIR_VALUE";
  const TCollection_AsciiString Reco_UniversalPair          = "UNIVERSAL_PAIR";
  const TCollection_AsciiString Reco_UniversalPairValue     = "UNIVERSAL_PAIR_VALUE";
  const TCollection_AsciiString Reco_UniversalPairWithRange = "UNIVERSAL_PAIR_WITH_RANGE";
  const TCollection_AsciiString Reco_KinematicPair          = "KINEMATIC_PAIR";
  const TCollection_AsciiString Reco_MechanismStateRepresentation =
    "MECHANISM_STATE_REPRESENTATION";

  const TCollection_AsciiString Reco_TessellatedConnectingEdge = "TESSELLATED_CONNECTING_EDGE";
  const TCollection_AsciiString Reco_TessellatedEdge           = "TESSELLATED_EDGE";
  const TCollection_AsciiString Reco_TessellatedPointSet       = "TESSELLATED_POINT_SET";
  const TCollection_AsciiString Reco_TessellatedShapeRepresentation =
    "TESSELLATED_SHAPE_REPRESENTATION";
  const TCollection_AsciiString Reco_TessellatedShapeRepresentationWithAccuracyParameters =
    "TESSELLATED_SHAPE_REPRESENTATION_WITH_ACCURACY_PARAMETERS";
  const TCollection_AsciiString Reco_TessellatedShell          = "TESSELLATED_SHELL";
  const TCollection_AsciiString Reco_TessellatedSolid          = "TESSELLATED_SOLID";
  const TCollection_AsciiString Reco_TessellatedStructuredItem = "TESSELLATED_STRUCTURED_ITEM";
  const TCollection_AsciiString Reco_TessellatedVertex         = "TESSELLATED_VERTEX";
  const TCollection_AsciiString Reco_TessellatedWire           = "TESSELLATED_WIRE";
  const TCollection_AsciiString Reco_TriangulatedFace          = "TRIANGULATED_FACE";
  const TCollection_AsciiString Reco_ComplexTriangulatedFace   = "COMPLEX_TRIANGULATED_FACE";
  const TCollection_AsciiString Reco_ComplexTriangulatedSurfaceSet =
    "COMPLEX_TRIANGULATED_SURFACE_SET";
  const TCollection_AsciiString Reco_CubicBezierTessellatedEdge  = "CUBIC_BEZIER_TESSELLATED_EDGE";
  const TCollection_AsciiString Reco_CubicBezierTriangulatedFace = "CUBIC_BEZIER_TRIANGULATED_FACE";
  const TCollection_AsciiString Reco_TriangulatedSurfaceSet      = "TRIANGULATED_SURFACE_SET";
  const TCollection_AsciiString Reco_GeneralPropertyAssociation  = "GENERAL_PROPERTY_ASSOCIATION";
  const TCollection_AsciiString Reco_GeneralPropertyRelationship = "GENERAL_PROPERTY_RELATIONSHIP";
  const TCollection_AsciiString Reco_BooleanRepresentationItem   = "BOOLEAN_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_RealRepresentationItem      = "REAL_REPRESENTATION_ITEM";
  const TCollection_AsciiString Reco_MechanicalDesignAndDraughtingRelationship =
    "MECHANICAL_DESIGN_AND_DRAUGHTING_RELATIONSHIP";

private:
  Reco_Container()                                 = default;
  Reco_Container(const Reco_Container&)            = default;
  Reco_Container& operator=(const Reco_Container&) = delete;
};

// -- Definition of the libraries --

namespace
{
static NCollection_DataMap<TCollection_AsciiString, Standard_Integer>& GetTypenums()
{
  static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> anInstance;
  return anInstance;
}

static NCollection_DataMap<TCollection_AsciiString, Standard_Integer>& GetTypeshor()
{
  static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> anInstance;
  return anInstance;
}
} // namespace

RWStepAP214_ReadWriteModule::RWStepAP214_ReadWriteModule()
{
  if (!GetTypenums().IsEmpty())
    return;

  const Reco_Container& aContainer = Reco_Container::Instance();

  GetTypenums().Bind(aContainer.Reco_CartesianPoint, 59);
  GetTypenums().Bind(aContainer.Reco_VertexPoint, 316);
  GetTypenums().Bind(aContainer.Reco_OrientedEdge, 181);
  GetTypenums().Bind(aContainer.Reco_EdgeCurve, 116);
  GetTypenums().Bind(aContainer.Reco_SurfaceCurve, 272);
  GetTypenums().Bind(aContainer.Reco_EdgeLoop, 117);
  GetTypenums().Bind(aContainer.Reco_AdvancedFace, 3);
  GetTypenums().Bind(aContainer.Reco_FaceBound, 131);
  GetTypenums().Bind(aContainer.Reco_FaceOuterBound, 132);
  GetTypenums().Bind(aContainer.Reco_Direction, 105);
  GetTypenums().Bind(aContainer.Reco_Vector, 313);
  GetTypenums().Bind(aContainer.Reco_BSplineCurve, 38);
  GetTypenums().Bind(aContainer.Reco_BSplineCurveWithKnots, 39);
  GetTypenums().Bind(aContainer.Reco_BezierCurve, 43);
  GetTypenums().Bind(aContainer.Reco_Pcurve, 190);
  GetTypenums().Bind(aContainer.Reco_QuasiUniformCurve, 236);
  GetTypenums().Bind(aContainer.Reco_RationalBSplineCurve, 239);
  GetTypenums().Bind(aContainer.Reco_TrimmedCurve, 308);
  GetTypenums().Bind(aContainer.Reco_UniformCurve, 311);

  GetTypenums().Bind(aContainer.Reco_BSplineSurface, 40);
  GetTypenums().Bind(aContainer.Reco_BSplineSurfaceWithKnots, 41);
  GetTypenums().Bind(aContainer.Reco_QuasiUniformSurface, 237);
  GetTypenums().Bind(aContainer.Reco_RationalBSplineSurface, 240);
  GetTypenums().Bind(aContainer.Reco_UniformSurface, 312);
  GetTypenums().Bind(aContainer.Reco_BezierSurface, 44);
  GetTypenums().Bind(aContainer.Reco_ConicalSurface, 75);
  GetTypenums().Bind(aContainer.Reco_CylindricalSurface, 90);
  GetTypenums().Bind(aContainer.Reco_SphericalSurface, 269);
  GetTypenums().Bind(aContainer.Reco_ToroidalSurface, 305);
  GetTypenums().Bind(aContainer.Reco_SurfaceOfLinearExtrusion, 273);
  GetTypenums().Bind(aContainer.Reco_SurfaceOfRevolution, 274);

  GetTypenums().Bind(aContainer.Reco_Address, 1);
  GetTypenums().Bind(aContainer.Reco_AdvancedBrepShapeRepresentation, 2);
  GetTypenums().Bind(aContainer.Reco_AnnotationCurveOccurrence, 4);
  GetTypenums().Bind(aContainer.Reco_AnnotationFillArea, 5);
  GetTypenums().Bind(aContainer.Reco_AnnotationFillAreaOccurrence, 6);
  GetTypenums().Bind(aContainer.Reco_AnnotationOccurrence, 7);
  GetTypenums().Bind(aContainer.Reco_AnnotationSubfigureOccurrence, 8);
  GetTypenums().Bind(aContainer.Reco_AnnotationSymbol, 9);
  GetTypenums().Bind(aContainer.Reco_AnnotationSymbolOccurrence, 10);
  GetTypenums().Bind(aContainer.Reco_AnnotationText, 11);
  GetTypenums().Bind(aContainer.Reco_AnnotationTextOccurrence, 12);
  GetTypenums().Bind(aContainer.Reco_ApplicationContext, 13);
  GetTypenums().Bind(aContainer.Reco_ApplicationContextElement, 14);
  GetTypenums().Bind(aContainer.Reco_ApplicationProtocolDefinition, 15);
  GetTypenums().Bind(aContainer.Reco_Approval, 16);
  GetTypenums().Bind(aContainer.Reco_ApprovalAssignment, 17);
  GetTypenums().Bind(aContainer.Reco_ApprovalPersonOrganization, 18);
  GetTypenums().Bind(aContainer.Reco_ApprovalRelationship, 19);
  GetTypenums().Bind(aContainer.Reco_ApprovalRole, 20);
  GetTypenums().Bind(aContainer.Reco_ApprovalStatus, 21);
  GetTypenums().Bind(aContainer.Reco_AreaInSet, 22);
  GetTypenums().Bind(aContainer.Reco_AutoDesignActualDateAndTimeAssignment, 23);
  GetTypenums().Bind(aContainer.Reco_AutoDesignActualDateAssignment, 24);
  GetTypenums().Bind(aContainer.Reco_AutoDesignApprovalAssignment, 25);
  GetTypenums().Bind(aContainer.Reco_AutoDesignDateAndPersonAssignment, 26);
  GetTypenums().Bind(aContainer.Reco_AutoDesignGroupAssignment, 27);
  GetTypenums().Bind(aContainer.Reco_AutoDesignNominalDateAndTimeAssignment, 28);
  GetTypenums().Bind(aContainer.Reco_AutoDesignNominalDateAssignment, 29);
  GetTypenums().Bind(aContainer.Reco_AutoDesignOrganizationAssignment, 30);
  GetTypenums().Bind(aContainer.Reco_AutoDesignPersonAndOrganizationAssignment, 31);
  GetTypenums().Bind(aContainer.Reco_AutoDesignPresentedItem, 32);
  GetTypenums().Bind(aContainer.Reco_AutoDesignSecurityClassificationAssignment, 33);
  GetTypenums().Bind(aContainer.Reco_AutoDesignViewArea, 34);
  GetTypenums().Bind(aContainer.Reco_Axis1Placement, 35);
  GetTypenums().Bind(aContainer.Reco_Axis2Placement2d, 36);
  GetTypenums().Bind(aContainer.Reco_Axis2Placement3d, 37);
  GetTypenums().Bind(aContainer.Reco_BackgroundColour, 42);
  GetTypenums().Bind(aContainer.Reco_Block, 45);
  GetTypenums().Bind(aContainer.Reco_BooleanResult, 46);
  GetTypenums().Bind(aContainer.Reco_BoundaryCurve, 47);
  GetTypenums().Bind(aContainer.Reco_BoundedCurve, 48);
  GetTypenums().Bind(aContainer.Reco_BoundedSurface, 49);
  GetTypenums().Bind(aContainer.Reco_BoxDomain, 50);
  GetTypenums().Bind(aContainer.Reco_BoxedHalfSpace, 51);
  GetTypenums().Bind(aContainer.Reco_BrepWithVoids, 52);
  GetTypenums().Bind(aContainer.Reco_CalendarDate, 53);
  GetTypenums().Bind(aContainer.Reco_CameraImage, 54);
  GetTypenums().Bind(aContainer.Reco_CameraModel, 55);
  GetTypenums().Bind(aContainer.Reco_CameraModelD2, 56);
  GetTypenums().Bind(aContainer.Reco_CameraModelD3, 57);
  GetTypenums().Bind(aContainer.Reco_CameraUsage, 58);
  GetTypenums().Bind(aContainer.Reco_CartesianTransformationOperator, 60);
  GetTypenums().Bind(aContainer.Reco_CartesianTransformationOperator3d, 61);
  GetTypenums().Bind(aContainer.Reco_Circle, 62);
  GetTypenums().Bind(aContainer.Reco_ClosedShell, 63);
  GetTypenums().Bind(aContainer.Reco_Colour, 64);
  GetTypenums().Bind(aContainer.Reco_ColourRgb, 65);
  GetTypenums().Bind(aContainer.Reco_ColourSpecification, 66);
  GetTypenums().Bind(aContainer.Reco_CompositeCurve, 67);
  GetTypenums().Bind(aContainer.Reco_CompositeCurveOnSurface, 68);
  GetTypenums().Bind(aContainer.Reco_CompositeCurveSegment, 69);
  GetTypenums().Bind(aContainer.Reco_CompositeText, 70);
  GetTypenums().Bind(aContainer.Reco_CompositeTextWithAssociatedCurves, 71);
  GetTypenums().Bind(aContainer.Reco_CompositeTextWithBlankingBox, 72);
  GetTypenums().Bind(aContainer.Reco_CompositeTextWithExtent, 73);
  GetTypenums().Bind(aContainer.Reco_Conic, 74);
  GetTypenums().Bind(aContainer.Reco_ConnectedFaceSet, 76);
  GetTypenums().Bind(aContainer.Reco_ContextDependentInvisibility, 77);
  GetTypenums().Bind(aContainer.Reco_ContextDependentOverRidingStyledItem, 78);
  GetTypenums().Bind(aContainer.Reco_ConversionBasedUnit, 79);
  GetTypenums().Bind(aContainer.Reco_CoordinatedUniversalTimeOffset, 80);
  GetTypenums().Bind(aContainer.Reco_CsgRepresentation, 81);
  GetTypenums().Bind(aContainer.Reco_CsgShapeRepresentation, 82);
  GetTypenums().Bind(aContainer.Reco_CsgSolid, 83);
  GetTypenums().Bind(aContainer.Reco_Curve, 84);
  GetTypenums().Bind(aContainer.Reco_CurveBoundedSurface, 85);
  GetTypenums().Bind(aContainer.Reco_CurveReplica, 86);
  GetTypenums().Bind(aContainer.Reco_CurveStyle, 87);
  GetTypenums().Bind(aContainer.Reco_CurveStyleFont, 88);
  GetTypenums().Bind(aContainer.Reco_CurveStyleFontPattern, 89);
  GetTypenums().Bind(aContainer.Reco_Date, 91);
  GetTypenums().Bind(aContainer.Reco_DateAndTime, 92);
  GetTypenums().Bind(aContainer.Reco_DateAndTimeAssignment, 93);
  GetTypenums().Bind(aContainer.Reco_DateAssignment, 94);
  GetTypenums().Bind(aContainer.Reco_DateRole, 95);
  GetTypenums().Bind(aContainer.Reco_DateTimeRole, 96);
  GetTypenums().Bind(aContainer.Reco_DefinedSymbol, 97);
  GetTypenums().Bind(aContainer.Reco_DefinitionalRepresentation, 98);
  GetTypenums().Bind(aContainer.Reco_DegeneratePcurve, 99);
  GetTypenums().Bind(aContainer.Reco_DegenerateToroidalSurface, 100);
  GetTypenums().Bind(aContainer.Reco_DescriptiveRepresentationItem, 101);
  GetTypenums().Bind(aContainer.Reco_DimensionCurve, 102);
  GetTypenums().Bind(aContainer.Reco_DimensionCurveTerminator, 103);
  GetTypenums().Bind(aContainer.Reco_DimensionalExponents, 104);
  GetTypenums().Bind(aContainer.Reco_DraughtingAnnotationOccurrence, 106);
  GetTypenums().Bind(aContainer.Reco_DraughtingCallout, 107);
  GetTypenums().Bind(aContainer.Reco_DraughtingPreDefinedColour, 108);
  GetTypenums().Bind(aContainer.Reco_DraughtingPreDefinedCurveFont, 109);
  GetTypenums().Bind(aContainer.Reco_DraughtingSubfigureRepresentation, 110);
  GetTypenums().Bind(aContainer.Reco_DraughtingSymbolRepresentation, 111);
  GetTypenums().Bind(aContainer.Reco_DraughtingTextLiteralWithDelineation, 112);
  GetTypenums().Bind(aContainer.Reco_DrawingDefinition, 113);
  GetTypenums().Bind(aContainer.Reco_DrawingRevision, 114);
  GetTypenums().Bind(aContainer.Reco_Edge, 115);
  GetTypenums().Bind(aContainer.Reco_ElementarySurface, 118);
  GetTypenums().Bind(aContainer.Reco_Ellipse, 119);
  GetTypenums().Bind(aContainer.Reco_EvaluatedDegeneratePcurve, 120);
  GetTypenums().Bind(aContainer.Reco_ExternalSource, 121);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedCurveFont, 122);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedHatchStyle, 123);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedItem, 124);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedSymbol, 125);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedTextFont, 126);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedTileStyle, 127);
  GetTypenums().Bind(aContainer.Reco_ExtrudedAreaSolid, 128);
  GetTypenums().Bind(aContainer.Reco_Face, 129);
  GetTypenums().Bind(aContainer.Reco_FaceSurface, 133);
  GetTypenums().Bind(aContainer.Reco_FacetedBrep, 134);
  GetTypenums().Bind(aContainer.Reco_FacetedBrepShapeRepresentation, 135);
  GetTypenums().Bind(aContainer.Reco_FillAreaStyle, 136);
  GetTypenums().Bind(aContainer.Reco_FillAreaStyleColour, 137);
  GetTypenums().Bind(aContainer.Reco_FillAreaStyleHatching, 138);
  GetTypenums().Bind(aContainer.Reco_FillAreaStyleTileSymbolWithStyle, 139);
  GetTypenums().Bind(aContainer.Reco_FillAreaStyleTiles, 140);
  GetTypenums().Bind(aContainer.Reco_FunctionallyDefinedTransformation, 141);
  GetTypenums().Bind(aContainer.Reco_GeometricCurveSet, 142);
  GetTypenums().Bind(aContainer.Reco_GeometricRepresentationContext, 143);
  GetTypenums().Bind(aContainer.Reco_GeometricRepresentationItem, 144);
  GetTypenums().Bind(aContainer.Reco_GeometricSet, 145);
  GetTypenums().Bind(aContainer.Reco_GeometricallyBoundedSurfaceShapeRepresentation, 146);
  GetTypenums().Bind(aContainer.Reco_GeometricallyBoundedWireframeShapeRepresentation, 147);
  GetTypenums().Bind(aContainer.Reco_GlobalUncertaintyAssignedContext, 148);
  GetTypenums().Bind(aContainer.Reco_GlobalUnitAssignedContext, 149);
  GetTypenums().Bind(aContainer.Reco_Group, 150);
  GetTypenums().Bind(aContainer.Reco_GroupAssignment, 151);
  GetTypenums().Bind(aContainer.Reco_GroupRelationship, 152);
  GetTypenums().Bind(aContainer.Reco_HalfSpaceSolid, 153);
  GetTypenums().Bind(aContainer.Reco_Hyperbola, 154);
  GetTypenums().Bind(aContainer.Reco_IntersectionCurve, 155);
  GetTypenums().Bind(aContainer.Reco_Invisibility, 156);
  GetTypenums().Bind(aContainer.Reco_LengthMeasureWithUnit, 157);
  GetTypenums().Bind(aContainer.Reco_LengthUnit, 158);
  GetTypenums().Bind(aContainer.Reco_Line, 159);
  GetTypenums().Bind(aContainer.Reco_LocalTime, 160);
  GetTypenums().Bind(aContainer.Reco_Loop, 161);
  GetTypenums().Bind(aContainer.Reco_ManifoldSolidBrep, 162);
  GetTypenums().Bind(aContainer.Reco_ManifoldSurfaceShapeRepresentation, 163);
  GetTypenums().Bind(aContainer.Reco_MappedItem, 164);
  GetTypenums().Bind(aContainer.Reco_MeasureWithUnit, 165);
  GetTypenums().Bind(aContainer.Reco_MechanicalDesignGeometricPresentationArea, 166);
  GetTypenums().Bind(aContainer.Reco_MechanicalDesignGeometricPresentationRepresentation, 167);
  GetTypenums().Bind(aContainer.Reco_MechanicalDesignPresentationArea, 168);
  GetTypenums().Bind(aContainer.Reco_NamedUnit, 169);
  GetTypenums().Bind(aContainer.Reco_OffsetCurve3d, 171);
  GetTypenums().Bind(aContainer.Reco_OffsetSurface, 172);
  GetTypenums().Bind(aContainer.Reco_OneDirectionRepeatFactor, 173);
  GetTypenums().Bind(aContainer.Reco_OpenShell, 174);
  GetTypenums().Bind(aContainer.Reco_OrdinalDate, 175);
  GetTypenums().Bind(aContainer.Reco_Organization, 176);
  GetTypenums().Bind(aContainer.Reco_OrganizationAssignment, 177);
  GetTypenums().Bind(aContainer.Reco_OrganizationRole, 178);
  GetTypenums().Bind(aContainer.Reco_OrganizationalAddress, 179);
  GetTypenums().Bind(aContainer.Reco_OrientedClosedShell, 180);
  GetTypenums().Bind(aContainer.Reco_OrientedFace, 182);
  GetTypenums().Bind(aContainer.Reco_OrientedOpenShell, 183);
  GetTypenums().Bind(aContainer.Reco_OrientedPath, 184);
  GetTypenums().Bind(aContainer.Reco_OuterBoundaryCurve, 185);
  GetTypenums().Bind(aContainer.Reco_OverRidingStyledItem, 186);
  GetTypenums().Bind(aContainer.Reco_Parabola, 187);
  GetTypenums().Bind(aContainer.Reco_ParametricRepresentationContext, 188);
  GetTypenums().Bind(aContainer.Reco_Path, 189);
  GetTypenums().Bind(aContainer.Reco_Person, 191);
  GetTypenums().Bind(aContainer.Reco_PersonAndOrganization, 192);
  GetTypenums().Bind(aContainer.Reco_PersonAndOrganizationAssignment, 193);
  GetTypenums().Bind(aContainer.Reco_PersonAndOrganizationRole, 194);
  GetTypenums().Bind(aContainer.Reco_PersonalAddress, 195);
  GetTypenums().Bind(aContainer.Reco_Placement, 196);
  GetTypenums().Bind(aContainer.Reco_PlanarBox, 197);
  GetTypenums().Bind(aContainer.Reco_PlanarExtent, 198);
  GetTypenums().Bind(aContainer.Reco_Plane, 199);
  GetTypenums().Bind(aContainer.Reco_PlaneAngleMeasureWithUnit, 200);
  GetTypenums().Bind(aContainer.Reco_PlaneAngleUnit, 201);
  GetTypenums().Bind(aContainer.Reco_Point, 202);
  GetTypenums().Bind(aContainer.Reco_PointOnCurve, 203);
  GetTypenums().Bind(aContainer.Reco_PointOnSurface, 204);
  GetTypenums().Bind(aContainer.Reco_PointReplica, 205);
  GetTypenums().Bind(aContainer.Reco_PointStyle, 206);
  GetTypenums().Bind(aContainer.Reco_PolyLoop, 207);
  GetTypenums().Bind(aContainer.Reco_Polyline, 208);
  GetTypenums().Bind(aContainer.Reco_PreDefinedColour, 209);
  GetTypenums().Bind(aContainer.Reco_PreDefinedCurveFont, 210);
  GetTypenums().Bind(aContainer.Reco_PreDefinedItem, 211);
  GetTypenums().Bind(aContainer.Reco_PreDefinedSymbol, 212);
  GetTypenums().Bind(aContainer.Reco_PreDefinedTextFont, 213);
  GetTypenums().Bind(aContainer.Reco_PresentationArea, 214);
  GetTypenums().Bind(aContainer.Reco_PresentationLayerAssignment, 215);
  GetTypenums().Bind(aContainer.Reco_PresentationRepresentation, 216);
  GetTypenums().Bind(aContainer.Reco_PresentationSet, 217);
  GetTypenums().Bind(aContainer.Reco_PresentationSize, 218);
  GetTypenums().Bind(aContainer.Reco_PresentationStyleAssignment, 219);
  GetTypenums().Bind(aContainer.Reco_PresentationStyleByContext, 220);
  GetTypenums().Bind(aContainer.Reco_PresentationView, 221);
  GetTypenums().Bind(aContainer.Reco_PresentedItem, 222);
  GetTypenums().Bind(aContainer.Reco_Product, 223);
  GetTypenums().Bind(aContainer.Reco_ProductCategory, 224);
  GetTypenums().Bind(aContainer.Reco_ProductContext, 225);
  GetTypenums().Bind(aContainer.Reco_ProductDataRepresentationView, 226);
  GetTypenums().Bind(aContainer.Reco_ProductDefinition, 227);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionContext, 228);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionFormation, 229);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionFormationWithSpecifiedSource, 230);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionShape, 231);
  GetTypenums().Bind(aContainer.Reco_ProductRelatedProductCategory, 232);
  GetTypenums().Bind(aContainer.Reco_ProductType, 233);
  GetTypenums().Bind(aContainer.Reco_PropertyDefinition, 234);
  GetTypenums().Bind(aContainer.Reco_PropertyDefinitionRepresentation, 235);
  GetTypenums().Bind(aContainer.Reco_RatioMeasureWithUnit, 238);
  GetTypenums().Bind(aContainer.Reco_RectangularCompositeSurface, 241);
  GetTypenums().Bind(aContainer.Reco_RectangularTrimmedSurface, 242);
  GetTypenums().Bind(aContainer.Reco_RepItemGroup, 243);
  GetTypenums().Bind(aContainer.Reco_ReparametrisedCompositeCurveSegment, 244);
  GetTypenums().Bind(aContainer.Reco_Representation, 245);
  GetTypenums().Bind(aContainer.Reco_RepresentationContext, 246);
  GetTypenums().Bind(aContainer.Reco_RepresentationItem, 247);
  GetTypenums().Bind(aContainer.Reco_RepresentationMap, 248);
  GetTypenums().Bind(aContainer.Reco_RepresentationRelationship, 249);
  GetTypenums().Bind(aContainer.Reco_RevolvedAreaSolid, 250);
  GetTypenums().Bind(aContainer.Reco_RightAngularWedge, 251);
  GetTypenums().Bind(aContainer.Reco_RightCircularCone, 252);
  GetTypenums().Bind(aContainer.Reco_RightCircularCylinder, 253);
  GetTypenums().Bind(aContainer.Reco_SeamCurve, 254);
  GetTypenums().Bind(aContainer.Reco_SecurityClassification, 255);
  GetTypenums().Bind(aContainer.Reco_SecurityClassificationAssignment, 256);
  GetTypenums().Bind(aContainer.Reco_SecurityClassificationLevel, 257);
  GetTypenums().Bind(aContainer.Reco_ShapeAspect, 258);
  GetTypenums().Bind(aContainer.Reco_ShapeAspectRelationship, 259);
  GetTypenums().Bind(aContainer.Reco_ShapeAspectTransition, 260);
  GetTypenums().Bind(aContainer.Reco_ShapeDefinitionRepresentation, 261);
  GetTypenums().Bind(aContainer.Reco_ShapeRepresentation, 262);
  GetTypenums().Bind(aContainer.Reco_ShellBasedSurfaceModel, 263);
  GetTypenums().Bind(aContainer.Reco_SiUnit, 264);
  GetTypenums().Bind(aContainer.Reco_SolidAngleMeasureWithUnit, 265);
  GetTypenums().Bind(aContainer.Reco_SolidModel, 266);
  GetTypenums().Bind(aContainer.Reco_SolidReplica, 267);
  GetTypenums().Bind(aContainer.Reco_Sphere, 268);
  GetTypenums().Bind(aContainer.Reco_StyledItem, 270);
  GetTypenums().Bind(aContainer.Reco_Surface, 271);
  GetTypenums().Bind(aContainer.Reco_SurfacePatch, 275);
  GetTypenums().Bind(aContainer.Reco_SurfaceReplica, 276);
  GetTypenums().Bind(aContainer.Reco_SurfaceSideStyle, 277);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleBoundary, 278);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleControlGrid, 279);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleFillArea, 280);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleParameterLine, 281);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleSegmentationCurve, 282);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleSilhouette, 283);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleUsage, 284);
  GetTypenums().Bind(aContainer.Reco_SweptAreaSolid, 285);
  GetTypenums().Bind(aContainer.Reco_SweptSurface, 286);
  GetTypenums().Bind(aContainer.Reco_SymbolColour, 287);
  GetTypenums().Bind(aContainer.Reco_SymbolRepresentation, 288);
  GetTypenums().Bind(aContainer.Reco_SymbolRepresentationMap, 289);
  GetTypenums().Bind(aContainer.Reco_SymbolStyle, 290);
  GetTypenums().Bind(aContainer.Reco_SymbolTarget, 291);
  GetTypenums().Bind(aContainer.Reco_Template, 292);
  GetTypenums().Bind(aContainer.Reco_TemplateInstance, 293);
  GetTypenums().Bind(aContainer.Reco_TerminatorSymbol, 294);
  GetTypenums().Bind(aContainer.Reco_TextLiteral, 295);
  GetTypenums().Bind(aContainer.Reco_TextLiteralWithAssociatedCurves, 296);
  GetTypenums().Bind(aContainer.Reco_TextLiteralWithBlankingBox, 297);
  GetTypenums().Bind(aContainer.Reco_TextLiteralWithDelineation, 298);
  GetTypenums().Bind(aContainer.Reco_TextLiteralWithExtent, 299);
  GetTypenums().Bind(aContainer.Reco_TextStyle, 300);
  GetTypenums().Bind(aContainer.Reco_TextStyleForDefinedFont, 301);
  GetTypenums().Bind(aContainer.Reco_TextStyleWithBoxCharacteristics, 302);
  GetTypenums().Bind(aContainer.Reco_TextStyleWithMirror, 303);
  GetTypenums().Bind(aContainer.Reco_TopologicalRepresentationItem, 304);
  GetTypenums().Bind(aContainer.Reco_Torus, 306);
  GetTypenums().Bind(aContainer.Reco_TransitionalShapeRepresentation, 307);
  GetTypenums().Bind(aContainer.Reco_TwoDirectionRepeatFactor, 309);
  GetTypenums().Bind(aContainer.Reco_UncertaintyMeasureWithUnit, 310);
  GetTypenums().Bind(aContainer.Reco_Vertex, 314);
  GetTypenums().Bind(aContainer.Reco_VertexLoop, 315);
  GetTypenums().Bind(aContainer.Reco_ViewVolume, 317);
  GetTypenums().Bind(aContainer.Reco_WeekOfYearAndDayDate, 318);
  // Added by FMA  for Rev4
  GetTypenums().Bind(aContainer.Reco_SolidAngleUnit, 336);
  GetTypenums().Bind(aContainer.Reco_MechanicalContext, 339);
  GetTypenums().Bind(aContainer.Reco_DesignContext, 340); // by CKY
  // full Rev4 (simple types)
  GetTypenums().Bind(aContainer.Reco_TimeMeasureWithUnit, 341);
  GetTypenums().Bind(aContainer.Reco_RatioUnit, 342);
  GetTypenums().Bind(aContainer.Reco_TimeUnit, 343);
  GetTypenums().Bind(aContainer.Reco_ApprovalDateTime, 348);
  GetTypenums().Bind(aContainer.Reco_CameraImage2dWithScale, 349);
  GetTypenums().Bind(aContainer.Reco_CameraImage3dWithScale, 350);
  GetTypenums().Bind(aContainer.Reco_CartesianTransformationOperator2d, 351);
  GetTypenums().Bind(aContainer.Reco_DerivedUnit, 352);
  GetTypenums().Bind(aContainer.Reco_DerivedUnitElement, 353);
  GetTypenums().Bind(aContainer.Reco_ItemDefinedTransformation, 354);
  GetTypenums().Bind(aContainer.Reco_PresentedItemRepresentation, 355);
  GetTypenums().Bind(aContainer.Reco_PresentationLayerUsage, 356);

  //  AP214 : CC1 -> CC2

  GetTypenums().Bind(aContainer.Reco_AutoDesignDocumentReference, 366);

  GetTypenums().Bind(aContainer.Reco_Document, 367);
  GetTypenums().Bind(aContainer.Reco_DigitalDocument, 368);
  GetTypenums().Bind(aContainer.Reco_DocumentRelationship, 369);
  GetTypenums().Bind(aContainer.Reco_DocumentType, 370);
  GetTypenums().Bind(aContainer.Reco_DocumentUsageConstraint, 371);
  GetTypenums().Bind(aContainer.Reco_Effectivity, 372);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionEffectivity, 373);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionRelationship, 374);

  GetTypenums().Bind(aContainer.Reco_ProductDefinitionWithAssociatedDocuments, 375);
  GetTypenums().Bind(aContainer.Reco_PhysicallyModeledProductDefinition, 376);

  GetTypenums().Bind(aContainer.Reco_ProductDefinitionUsage, 377);
  GetTypenums().Bind(aContainer.Reco_MakeFromUsageOption, 378);
  GetTypenums().Bind(aContainer.Reco_AssemblyComponentUsage, 379);
  GetTypenums().Bind(aContainer.Reco_NextAssemblyUsageOccurrence, 380);
  GetTypenums().Bind(aContainer.Reco_PromissoryUsageOccurrence, 381);
  GetTypenums().Bind(aContainer.Reco_QuantifiedAssemblyComponentUsage, 382);
  GetTypenums().Bind(aContainer.Reco_SpecifiedHigherUsageOccurrence, 383);
  GetTypenums().Bind(aContainer.Reco_AssemblyComponentUsageSubstitute, 384);
  GetTypenums().Bind(aContainer.Reco_SuppliedPartRelationship, 385);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedRepresentation, 386);
  GetTypenums().Bind(aContainer.Reco_ShapeRepresentationRelationship, 387);
  GetTypenums().Bind(aContainer.Reco_RepresentationRelationshipWithTransformation, 388);
  GetTypenums().Bind(aContainer.Reco_MaterialDesignation, 390);

  GetTypenums().Bind(aContainer.Reco_ContextDependentShapeRepresentation, 391);
  // Added from CD to DIS (S4134)
  GetTypenums().Bind(aContainer.Reco_AppliedDateAndTimeAssignment, 392);
  GetTypenums().Bind(aContainer.Reco_AppliedDateAssignment, 393);
  GetTypenums().Bind(aContainer.Reco_AppliedApprovalAssignment, 394);
  GetTypenums().Bind(aContainer.Reco_AppliedGroupAssignment, 395);
  GetTypenums().Bind(aContainer.Reco_AppliedOrganizationAssignment, 396);
  GetTypenums().Bind(aContainer.Reco_AppliedPersonAndOrganizationAssignment, 397);
  GetTypenums().Bind(aContainer.Reco_AppliedPresentedItem, 398);
  GetTypenums().Bind(aContainer.Reco_AppliedSecurityClassificationAssignment, 399);
  GetTypenums().Bind(aContainer.Reco_AppliedDocumentReference, 400);
  GetTypenums().Bind(aContainer.Reco_DocumentFile, 401);
  GetTypenums().Bind(aContainer.Reco_CharacterizedObject, 402);
  GetTypenums().Bind(aContainer.Reco_ExtrudedFaceSolid, 403);
  GetTypenums().Bind(aContainer.Reco_RevolvedFaceSolid, 404);
  GetTypenums().Bind(aContainer.Reco_SweptFaceSolid, 405);

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  GetTypenums().Bind(aContainer.Reco_MeasureRepresentationItem, 406);
  GetTypenums().Bind(aContainer.Reco_AreaUnit, 407);
  GetTypenums().Bind(aContainer.Reco_VolumeUnit, 408);

  // Added by ABV 10.11.99 for AP203
  GetTypenums().Bind(aContainer.Reco_Action, 413);
  GetTypenums().Bind(aContainer.Reco_ActionAssignment, 414);
  GetTypenums().Bind(aContainer.Reco_ActionMethod, 415);
  GetTypenums().Bind(aContainer.Reco_ActionRequestAssignment, 416);
  GetTypenums().Bind(aContainer.Reco_CcDesignApproval, 417);
  GetTypenums().Bind(aContainer.Reco_CcDesignCertification, 418);
  GetTypenums().Bind(aContainer.Reco_CcDesignContract, 419);
  GetTypenums().Bind(aContainer.Reco_CcDesignDateAndTimeAssignment, 420);
  GetTypenums().Bind(aContainer.Reco_CcDesignPersonAndOrganizationAssignment, 421);
  GetTypenums().Bind(aContainer.Reco_CcDesignSecurityClassification, 422);
  GetTypenums().Bind(aContainer.Reco_CcDesignSpecificationReference, 423);
  GetTypenums().Bind(aContainer.Reco_Certification, 424);
  GetTypenums().Bind(aContainer.Reco_CertificationAssignment, 425);
  GetTypenums().Bind(aContainer.Reco_CertificationType, 426);
  GetTypenums().Bind(aContainer.Reco_Change, 427);
  GetTypenums().Bind(aContainer.Reco_ChangeRequest, 428);
  GetTypenums().Bind(aContainer.Reco_ConfigurationDesign, 429);
  GetTypenums().Bind(aContainer.Reco_ConfigurationEffectivity, 430);
  GetTypenums().Bind(aContainer.Reco_Contract, 431);
  GetTypenums().Bind(aContainer.Reco_ContractAssignment, 432);
  GetTypenums().Bind(aContainer.Reco_ContractType, 433);
  GetTypenums().Bind(aContainer.Reco_ProductConcept, 434);
  GetTypenums().Bind(aContainer.Reco_ProductConceptContext, 435);
  GetTypenums().Bind(aContainer.Reco_StartRequest, 436);
  GetTypenums().Bind(aContainer.Reco_StartWork, 437);
  GetTypenums().Bind(aContainer.Reco_VersionedActionRequest, 438);
  GetTypenums().Bind(aContainer.Reco_ProductCategoryRelationship, 439);
  GetTypenums().Bind(aContainer.Reco_ActionRequestSolution, 440);
  GetTypenums().Bind(aContainer.Reco_DraughtingModel, 441);

  GetTypenums().Bind(aContainer.Reco_AngularLocation, 442);
  GetTypenums().Bind(aContainer.Reco_AngularSize, 443);
  GetTypenums().Bind(aContainer.Reco_DimensionalCharacteristicRepresentation, 444);
  GetTypenums().Bind(aContainer.Reco_DimensionalLocation, 445);
  GetTypenums().Bind(aContainer.Reco_DimensionalLocationWithPath, 446);
  GetTypenums().Bind(aContainer.Reco_DimensionalSize, 447);
  GetTypenums().Bind(aContainer.Reco_DimensionalSizeWithPath, 448);
  GetTypenums().Bind(aContainer.Reco_ShapeDimensionRepresentation, 449);

  // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  GetTypenums().Bind(aContainer.Reco_DocumentRepresentationType, 450);
  GetTypenums().Bind(aContainer.Reco_ObjectRole, 451);
  GetTypenums().Bind(aContainer.Reco_RoleAssociation, 452);
  GetTypenums().Bind(aContainer.Reco_IdentificationRole, 453);
  GetTypenums().Bind(aContainer.Reco_IdentificationAssignment, 454);
  GetTypenums().Bind(aContainer.Reco_ExternalIdentificationAssignment, 455);
  GetTypenums().Bind(aContainer.Reco_EffectivityAssignment, 456);
  GetTypenums().Bind(aContainer.Reco_NameAssignment, 457);
  GetTypenums().Bind(aContainer.Reco_GeneralProperty, 458);
  GetTypenums().Bind(aContainer.Reco_Class, 459);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedClass, 460);
  GetTypenums().Bind(aContainer.Reco_ExternallyDefinedGeneralProperty, 461);
  GetTypenums().Bind(aContainer.Reco_AppliedExternalIdentificationAssignment, 462);

  // Added by CKY, 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
  GetTypenums().Bind(aContainer.Reco_CompositeShapeAspect, 470);
  GetTypenums().Bind(aContainer.Reco_DerivedShapeAspect, 471);
  GetTypenums().Bind(aContainer.Reco_Extension, 472);
  GetTypenums().Bind(aContainer.Reco_DirectedDimensionalLocation, 473);
  GetTypenums().Bind(aContainer.Reco_LimitsAndFits, 474);
  GetTypenums().Bind(aContainer.Reco_ToleranceValue, 475);
  GetTypenums().Bind(aContainer.Reco_MeasureQualification, 476);
  GetTypenums().Bind(aContainer.Reco_PlusMinusTolerance, 477);
  GetTypenums().Bind(aContainer.Reco_PrecisionQualifier, 478);
  GetTypenums().Bind(aContainer.Reco_TypeQualifier, 479);
  GetTypenums().Bind(aContainer.Reco_QualifiedRepresentationItem, 480);

  GetTypenums().Bind(aContainer.Reco_CompoundRepresentationItem, 482);
  GetTypenums().Bind(aContainer.Reco_ValueRange, 483);
  GetTypenums().Bind(aContainer.Reco_ShapeAspectDerivingRelationship, 484);

  // abv 27.12.01
  GetTypenums().Bind(aContainer.Reco_CompoundShapeRepresentation, 485);
  GetTypenums().Bind(aContainer.Reco_ConnectedEdgeSet, 486);
  GetTypenums().Bind(aContainer.Reco_ConnectedFaceShapeRepresentation, 487);
  GetTypenums().Bind(aContainer.Reco_EdgeBasedWireframeModel, 488);
  GetTypenums().Bind(aContainer.Reco_EdgeBasedWireframeShapeRepresentation, 489);
  GetTypenums().Bind(aContainer.Reco_FaceBasedSurfaceModel, 490);
  GetTypenums().Bind(aContainer.Reco_NonManifoldSurfaceShapeRepresentation, 491);

  // gka 08.01.02
  GetTypenums().Bind(aContainer.Reco_OrientedSurface, 492);
  GetTypenums().Bind(aContainer.Reco_Subface, 493);
  GetTypenums().Bind(aContainer.Reco_Subedge, 494);
  GetTypenums().Bind(aContainer.Reco_SeamEdge, 495);
  GetTypenums().Bind(aContainer.Reco_ConnectedFaceSubSet, 496);

  // AP209
  GetTypenums().Bind(aContainer.Reco_EulerAngles, 500);
  GetTypenums().Bind(aContainer.Reco_MassUnit, 501);
  GetTypenums().Bind(aContainer.Reco_ThermodynamicTemperatureUnit, 502);
  GetTypenums().Bind(aContainer.Reco_AnalysisItemWithinRepresentation, 503);
  GetTypenums().Bind(aContainer.Reco_Curve3dElementDescriptor, 504);
  GetTypenums().Bind(aContainer.Reco_CurveElementEndReleasePacket, 505);
  GetTypenums().Bind(aContainer.Reco_CurveElementSectionDefinition, 506);
  GetTypenums().Bind(aContainer.Reco_CurveElementSectionDerivedDefinitions, 507);
  GetTypenums().Bind(aContainer.Reco_ElementDescriptor, 508);
  GetTypenums().Bind(aContainer.Reco_ElementMaterial, 509);
  GetTypenums().Bind(aContainer.Reco_Surface3dElementDescriptor, 510);
  GetTypenums().Bind(aContainer.Reco_SurfaceElementProperty, 511);
  GetTypenums().Bind(aContainer.Reco_SurfaceSection, 512);
  GetTypenums().Bind(aContainer.Reco_SurfaceSectionField, 513);
  GetTypenums().Bind(aContainer.Reco_SurfaceSectionFieldConstant, 514);
  GetTypenums().Bind(aContainer.Reco_SurfaceSectionFieldVarying, 515);
  GetTypenums().Bind(aContainer.Reco_UniformSurfaceSection, 516);
  GetTypenums().Bind(aContainer.Reco_Volume3dElementDescriptor, 517);
  GetTypenums().Bind(aContainer.Reco_AlignedCurve3dElementCoordinateSystem, 518);
  GetTypenums().Bind(aContainer.Reco_ArbitraryVolume3dElementCoordinateSystem, 519);
  GetTypenums().Bind(aContainer.Reco_Curve3dElementProperty, 520);
  GetTypenums().Bind(aContainer.Reco_Curve3dElementRepresentation, 521);
  GetTypenums().Bind(aContainer.Reco_Node, 522);
  //  GetTypenums().Bind (Reco_CurveElementEndCoordinateSystem,523);
  GetTypenums().Bind(aContainer.Reco_CurveElementEndOffset, 524);
  GetTypenums().Bind(aContainer.Reco_CurveElementEndRelease, 525);
  GetTypenums().Bind(aContainer.Reco_CurveElementInterval, 526);
  GetTypenums().Bind(aContainer.Reco_CurveElementIntervalConstant, 527);
  GetTypenums().Bind(aContainer.Reco_DummyNode, 528);
  GetTypenums().Bind(aContainer.Reco_CurveElementLocation, 529);
  GetTypenums().Bind(aContainer.Reco_ElementGeometricRelationship, 530);
  GetTypenums().Bind(aContainer.Reco_ElementGroup, 531);
  GetTypenums().Bind(aContainer.Reco_ElementRepresentation, 532);
  GetTypenums().Bind(aContainer.Reco_FeaAreaDensity, 533);
  GetTypenums().Bind(aContainer.Reco_FeaAxis2Placement3d, 534);
  GetTypenums().Bind(aContainer.Reco_FeaGroup, 535);
  GetTypenums().Bind(aContainer.Reco_FeaLinearElasticity, 536);
  GetTypenums().Bind(aContainer.Reco_FeaMassDensity, 537);
  GetTypenums().Bind(aContainer.Reco_FeaMaterialPropertyRepresentation, 538);
  GetTypenums().Bind(aContainer.Reco_FeaMaterialPropertyRepresentationItem, 539);
  GetTypenums().Bind(aContainer.Reco_FeaModel, 540);
  GetTypenums().Bind(aContainer.Reco_FeaModel3d, 541);
  GetTypenums().Bind(aContainer.Reco_FeaMoistureAbsorption, 542);
  GetTypenums().Bind(aContainer.Reco_FeaParametricPoint, 543);
  GetTypenums().Bind(aContainer.Reco_FeaRepresentationItem, 544);
  GetTypenums().Bind(aContainer.Reco_FeaSecantCoefficientOfLinearThermalExpansion, 545);
  GetTypenums().Bind(aContainer.Reco_FeaShellBendingStiffness, 546);
  GetTypenums().Bind(aContainer.Reco_FeaShellMembraneBendingCouplingStiffness, 547);
  GetTypenums().Bind(aContainer.Reco_FeaShellMembraneStiffness, 548);
  GetTypenums().Bind(aContainer.Reco_FeaShellShearStiffness, 549);
  GetTypenums().Bind(aContainer.Reco_GeometricNode, 550);
  GetTypenums().Bind(aContainer.Reco_FeaTangentialCoefficientOfLinearThermalExpansion, 551);
  GetTypenums().Bind(aContainer.Reco_NodeGroup, 552);
  GetTypenums().Bind(aContainer.Reco_NodeRepresentation, 553);
  GetTypenums().Bind(aContainer.Reco_NodeSet, 554);
  GetTypenums().Bind(aContainer.Reco_NodeWithSolutionCoordinateSystem, 555);
  GetTypenums().Bind(aContainer.Reco_NodeWithVector, 556);
  GetTypenums().Bind(aContainer.Reco_ParametricCurve3dElementCoordinateDirection, 557);
  GetTypenums().Bind(aContainer.Reco_ParametricCurve3dElementCoordinateSystem, 558);
  GetTypenums().Bind(aContainer.Reco_ParametricSurface3dElementCoordinateSystem, 559);
  GetTypenums().Bind(aContainer.Reco_Surface3dElementRepresentation, 560);
  //  GetTypenums().Bind (Reco_SymmetricTensor22d,561);
  //  GetTypenums().Bind (Reco_SymmetricTensor42d,562);
  //  GetTypenums().Bind (Reco_SymmetricTensor43d,563);
  GetTypenums().Bind(aContainer.Reco_Volume3dElementRepresentation, 564);
  GetTypenums().Bind(aContainer.Reco_DataEnvironment, 565);
  GetTypenums().Bind(aContainer.Reco_MaterialPropertyRepresentation, 566);
  GetTypenums().Bind(aContainer.Reco_PropertyDefinitionRelationship, 567);
  GetTypenums().Bind(aContainer.Reco_PointRepresentation, 568);
  GetTypenums().Bind(aContainer.Reco_MaterialProperty, 569);
  GetTypenums().Bind(aContainer.Reco_FeaModelDefinition, 570);
  GetTypenums().Bind(aContainer.Reco_FreedomAndCoefficient, 571);
  GetTypenums().Bind(aContainer.Reco_FreedomsList, 572);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionFormationRelationship, 573);
  //  GetTypenums().Bind (Reco_FeaModelDefinition,574);
  GetTypenums().Bind(aContainer.Reco_NodeDefinition, 575);
  GetTypenums().Bind(aContainer.Reco_StructuralResponseProperty, 576);
  GetTypenums().Bind(aContainer.Reco_StructuralResponsePropertyDefinitionRepresentation, 577);
  GetTypenums().Bind(aContainer.Reco_AlignedSurface3dElementCoordinateSystem, 579);
  GetTypenums().Bind(aContainer.Reco_ConstantSurface3dElementCoordinateSystem, 580);
  GetTypenums().Bind(aContainer.Reco_CurveElementIntervalLinearlyVarying, 581);
  GetTypenums().Bind(aContainer.Reco_FeaCurveSectionGeometricRelationship, 582);
  GetTypenums().Bind(aContainer.Reco_FeaSurfaceSectionGeometricRelationship, 583);

  // ptv 28.01.2003
  GetTypenums().Bind(aContainer.Reco_DocumentProductAssociation, 600);
  GetTypenums().Bind(aContainer.Reco_DocumentProductEquivalence, 601);

  // Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
  GetTypenums().Bind(aContainer.Reco_CylindricityTolerance, 609);
  GetTypenums().Bind(aContainer.Reco_ShapeRepresentationWithParameters, 610);
  GetTypenums().Bind(aContainer.Reco_AngularityTolerance, 611);
  GetTypenums().Bind(aContainer.Reco_ConcentricityTolerance, 612);
  GetTypenums().Bind(aContainer.Reco_CircularRunoutTolerance, 613);
  GetTypenums().Bind(aContainer.Reco_CoaxialityTolerance, 614);
  GetTypenums().Bind(aContainer.Reco_FlatnessTolerance, 615);
  GetTypenums().Bind(aContainer.Reco_LineProfileTolerance, 616);
  GetTypenums().Bind(aContainer.Reco_ParallelismTolerance, 617);
  GetTypenums().Bind(aContainer.Reco_PerpendicularityTolerance, 618);
  GetTypenums().Bind(aContainer.Reco_PositionTolerance, 619);
  GetTypenums().Bind(aContainer.Reco_RoundnessTolerance, 620);
  GetTypenums().Bind(aContainer.Reco_StraightnessTolerance, 621);
  GetTypenums().Bind(aContainer.Reco_SurfaceProfileTolerance, 622);
  GetTypenums().Bind(aContainer.Reco_SymmetryTolerance, 623);
  GetTypenums().Bind(aContainer.Reco_TotalRunoutTolerance, 624);
  GetTypenums().Bind(aContainer.Reco_GeometricTolerance, 625);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceRelationship, 626);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceWithDatumReference, 627);
  GetTypenums().Bind(aContainer.Reco_ModifiedGeometricTolerance, 628);
  GetTypenums().Bind(aContainer.Reco_Datum, 629);
  GetTypenums().Bind(aContainer.Reco_DatumFeature, 630);
  GetTypenums().Bind(aContainer.Reco_DatumReference, 631);
  GetTypenums().Bind(aContainer.Reco_CommonDatum, 632);
  GetTypenums().Bind(aContainer.Reco_DatumTarget, 633);
  GetTypenums().Bind(aContainer.Reco_PlacedDatumTargetFeature, 634);

  GetTypenums().Bind(aContainer.Reco_MassMeasureWithUnit, 651);

  // Added by ika for GD&T AP242
  GetTypenums().Bind(aContainer.Reco_Apex, 660);
  GetTypenums().Bind(aContainer.Reco_CentreOfSymmetry, 661);
  GetTypenums().Bind(aContainer.Reco_GeometricAlignment, 662);
  GetTypenums().Bind(aContainer.Reco_PerpendicularTo, 663);
  GetTypenums().Bind(aContainer.Reco_Tangent, 664);
  GetTypenums().Bind(aContainer.Reco_ParallelOffset, 665);
  GetTypenums().Bind(aContainer.Reco_GeometricItemSpecificUsage, 666);
  GetTypenums().Bind(aContainer.Reco_IdAttribute, 667);
  GetTypenums().Bind(aContainer.Reco_ItemIdentifiedRepresentationUsage, 668);
  GetTypenums().Bind(aContainer.Reco_AllAroundShapeAspect, 669);
  GetTypenums().Bind(aContainer.Reco_BetweenShapeAspect, 670);
  GetTypenums().Bind(aContainer.Reco_CompositeGroupShapeAspect, 671);
  GetTypenums().Bind(aContainer.Reco_ContinuosShapeAspect, 672);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceWithDefinedAreaUnit, 673);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceWithDefinedUnit, 674);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceWithMaximumTolerance, 675);
  GetTypenums().Bind(aContainer.Reco_GeometricToleranceWithModifiers, 676);
  GetTypenums().Bind(aContainer.Reco_UnequallyDisposedGeometricTolerance, 677);
  GetTypenums().Bind(aContainer.Reco_NonUniformZoneDefinition, 678);
  GetTypenums().Bind(aContainer.Reco_ProjectedZoneDefinition, 679);
  GetTypenums().Bind(aContainer.Reco_RunoutZoneDefinition, 680);
  GetTypenums().Bind(aContainer.Reco_RunoutZoneOrientation, 681);
  GetTypenums().Bind(aContainer.Reco_ToleranceZone, 682);
  GetTypenums().Bind(aContainer.Reco_ToleranceZoneDefinition, 683);
  GetTypenums().Bind(aContainer.Reco_ToleranceZoneForm, 684);
  GetTypenums().Bind(aContainer.Reco_ValueFormatTypeQualifier, 685);
  GetTypenums().Bind(aContainer.Reco_DatumReferenceCompartment, 686);
  GetTypenums().Bind(aContainer.Reco_DatumReferenceElement, 687);
  GetTypenums().Bind(aContainer.Reco_DatumReferenceModifierWithValue, 688);
  GetTypenums().Bind(aContainer.Reco_DatumSystem, 689);
  GetTypenums().Bind(aContainer.Reco_GeneralDatumReference, 690);
  GetTypenums().Bind(aContainer.Reco_IntegerRepresentationItem, 700);
  GetTypenums().Bind(aContainer.Reco_ValueRepresentationItem, 701);
  GetTypenums().Bind(aContainer.Reco_FeatureForDatumTargetRelationship, 702);
  GetTypenums().Bind(aContainer.Reco_DraughtingModelItemAssociation, 703);
  GetTypenums().Bind(aContainer.Reco_AnnotationPlane, 704);

  GetTypenums().Bind(aContainer.Reco_TessellatedAnnotationOccurrence, 707);
  GetTypenums().Bind(aContainer.Reco_TessellatedGeometricSet, 709);

  GetTypenums().Bind(aContainer.Reco_TessellatedCurveSet, 710);
  GetTypenums().Bind(aContainer.Reco_CoordinatesList, 711);
  GetTypenums().Bind(aContainer.Reco_ConstructiveGeometryRepresentation, 712);
  GetTypenums().Bind(aContainer.Reco_ConstructiveGeometryRepresentationRelationship, 713);
  GetTypenums().Bind(aContainer.Reco_CharacterizedRepresentation, 714);
  GetTypenums().Bind(aContainer.Reco_CameraModelD3MultiClipping, 716);
  GetTypenums().Bind(aContainer.Reco_CameraModelD3MultiClippingIntersection, 717);
  GetTypenums().Bind(aContainer.Reco_CameraModelD3MultiClippingUnion, 718);

  GetTypenums().Bind(aContainer.Reco_SurfaceStyleTransparent, 720);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleReflectanceAmbient, 721);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleRendering, 722);
  GetTypenums().Bind(aContainer.Reco_SurfaceStyleRenderingWithProperties, 723);

  GetTypenums().Bind(aContainer.Reco_RepresentationContextReference, 724);
  GetTypenums().Bind(aContainer.Reco_RepresentationReference, 725);
  GetTypenums().Bind(aContainer.Reco_SuParameters, 726);
  GetTypenums().Bind(aContainer.Reco_RotationAboutDirection, 727);
  GetTypenums().Bind(aContainer.Reco_KinematicJoint, 728);
  GetTypenums().Bind(aContainer.Reco_ActuatedKinematicPair, 729);
  GetTypenums().Bind(aContainer.Reco_ContextDependentKinematicLinkRepresentation, 730);
  GetTypenums().Bind(aContainer.Reco_CylindricalPair, 731);
  GetTypenums().Bind(aContainer.Reco_CylindricalPairValue, 732);
  GetTypenums().Bind(aContainer.Reco_CylindricalPairWithRange, 733);
  GetTypenums().Bind(aContainer.Reco_FullyConstrainedPair, 734);
  GetTypenums().Bind(aContainer.Reco_GearPair, 735);
  GetTypenums().Bind(aContainer.Reco_GearPairValue, 736);
  GetTypenums().Bind(aContainer.Reco_GearPairWithRange, 737);
  GetTypenums().Bind(aContainer.Reco_HomokineticPair, 738);
  GetTypenums().Bind(aContainer.Reco_KinematicLink, 739);
  GetTypenums().Bind(aContainer.Reco_KinematicLinkRepresentationAssociation, 740);
  GetTypenums().Bind(aContainer.Reco_KinematicPropertyMechanismRepresentation, 741);
  GetTypenums().Bind(aContainer.Reco_KinematicTopologyStructure, 742);
  GetTypenums().Bind(aContainer.Reco_LowOrderKinematicPair, 743);
  GetTypenums().Bind(aContainer.Reco_LowOrderKinematicPairValue, 744);
  GetTypenums().Bind(aContainer.Reco_LowOrderKinematicPairWithRange, 745);
  GetTypenums().Bind(aContainer.Reco_MechanismRepresentation, 746);
  GetTypenums().Bind(aContainer.Reco_OrientedJoint, 747);
  GetTypenums().Bind(aContainer.Reco_PlanarCurvePair, 748);
  GetTypenums().Bind(aContainer.Reco_PlanarCurvePairRange, 749);
  GetTypenums().Bind(aContainer.Reco_PlanarPair, 750);
  GetTypenums().Bind(aContainer.Reco_PlanarPairValue, 751);
  GetTypenums().Bind(aContainer.Reco_PlanarPairWithRange, 752);
  GetTypenums().Bind(aContainer.Reco_PointOnPlanarCurvePair, 753);
  GetTypenums().Bind(aContainer.Reco_PointOnPlanarCurvePairValue, 754);
  GetTypenums().Bind(aContainer.Reco_PointOnPlanarCurvePairWithRange, 755);
  GetTypenums().Bind(aContainer.Reco_PointOnSurfacePair, 756);
  GetTypenums().Bind(aContainer.Reco_PointOnSurfacePairValue, 757);
  GetTypenums().Bind(aContainer.Reco_PointOnSurfacePairWithRange, 758);
  GetTypenums().Bind(aContainer.Reco_PrismaticPair, 759);
  GetTypenums().Bind(aContainer.Reco_PrismaticPairValue, 760);
  GetTypenums().Bind(aContainer.Reco_PrismaticPairWithRange, 761);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionKinematics, 762);
  GetTypenums().Bind(aContainer.Reco_ProductDefinitionRelationshipKinematics, 763);
  GetTypenums().Bind(aContainer.Reco_RackAndPinionPair, 764);
  GetTypenums().Bind(aContainer.Reco_RackAndPinionPairValue, 765);
  GetTypenums().Bind(aContainer.Reco_RackAndPinionPairWithRange, 766);
  GetTypenums().Bind(aContainer.Reco_RevolutePair, 767);
  GetTypenums().Bind(aContainer.Reco_RevolutePairValue, 768);
  GetTypenums().Bind(aContainer.Reco_RevolutePairWithRange, 769);
  GetTypenums().Bind(aContainer.Reco_RollingCurvePair, 770);
  GetTypenums().Bind(aContainer.Reco_RollingCurvePairValue, 771);
  GetTypenums().Bind(aContainer.Reco_RollingSurfacePair, 772);
  GetTypenums().Bind(aContainer.Reco_RollingSurfacePairValue, 773);
  GetTypenums().Bind(aContainer.Reco_ScrewPair, 774);
  GetTypenums().Bind(aContainer.Reco_ScrewPairValue, 775);
  GetTypenums().Bind(aContainer.Reco_ScrewPairWithRange, 776);
  GetTypenums().Bind(aContainer.Reco_SlidingCurvePair, 777);
  GetTypenums().Bind(aContainer.Reco_SlidingCurvePairValue, 778);
  GetTypenums().Bind(aContainer.Reco_SlidingSurfacePair, 779);
  GetTypenums().Bind(aContainer.Reco_SlidingSurfacePairValue, 780);
  GetTypenums().Bind(aContainer.Reco_SphericalPair, 781);
  GetTypenums().Bind(aContainer.Reco_SphericalPairValue, 782);
  GetTypenums().Bind(aContainer.Reco_SphericalPairWithPin, 783);
  GetTypenums().Bind(aContainer.Reco_SphericalPairWithPinAndRange, 784);
  GetTypenums().Bind(aContainer.Reco_SphericalPairWithRange, 785);
  GetTypenums().Bind(aContainer.Reco_SurfacePairWithRange, 786);
  GetTypenums().Bind(aContainer.Reco_UnconstrainedPair, 787);
  GetTypenums().Bind(aContainer.Reco_UnconstrainedPairValue, 788);
  GetTypenums().Bind(aContainer.Reco_UniversalPair, 789);
  GetTypenums().Bind(aContainer.Reco_UniversalPairValue, 790);
  GetTypenums().Bind(aContainer.Reco_UniversalPairWithRange, 791);
  GetTypenums().Bind(aContainer.Reco_PairRepresentationRelationship, 792);
  GetTypenums().Bind(aContainer.Reco_RigidLinkRepresentation, 793);
  GetTypenums().Bind(aContainer.Reco_KinematicTopologyDirectedStructure, 794);
  GetTypenums().Bind(aContainer.Reco_KinematicTopologyNetworkStructure, 795);
  GetTypenums().Bind(aContainer.Reco_LinearFlexibleAndPinionPair, 796);
  GetTypenums().Bind(aContainer.Reco_LinearFlexibleAndPlanarCurvePair, 797);
  GetTypenums().Bind(aContainer.Reco_LinearFlexibleLinkRepresentation, 798);
  GetTypenums().Bind(aContainer.Reco_KinematicPair, 799);
  GetTypenums().Bind(aContainer.Reco_MechanismStateRepresentation, 801);
  GetTypenums().Bind(aContainer.Reco_RepositionedTessellatedItem, 803);
  GetTypenums().Bind(aContainer.Reco_TessellatedConnectingEdge, 804);
  GetTypenums().Bind(aContainer.Reco_TessellatedEdge, 805);
  GetTypenums().Bind(aContainer.Reco_TessellatedPointSet, 806);
  GetTypenums().Bind(aContainer.Reco_TessellatedShapeRepresentation, 807);
  GetTypenums().Bind(aContainer.Reco_TessellatedShapeRepresentationWithAccuracyParameters, 808);
  GetTypenums().Bind(aContainer.Reco_TessellatedShell, 809);
  GetTypenums().Bind(aContainer.Reco_TessellatedSolid, 810);
  GetTypenums().Bind(aContainer.Reco_TessellatedStructuredItem, 811);
  GetTypenums().Bind(aContainer.Reco_TessellatedVertex, 812);
  GetTypenums().Bind(aContainer.Reco_TessellatedWire, 813);
  GetTypenums().Bind(aContainer.Reco_TriangulatedFace, 814);
  GetTypenums().Bind(aContainer.Reco_ComplexTriangulatedFace, 815);
  GetTypenums().Bind(aContainer.Reco_ComplexTriangulatedSurfaceSet, 816);
  GetTypenums().Bind(aContainer.Reco_CubicBezierTessellatedEdge, 817);
  GetTypenums().Bind(aContainer.Reco_CubicBezierTriangulatedFace, 818);
  GetTypenums().Bind(aContainer.Reco_TriangulatedSurfaceSet, 819);
  GetTypenums().Bind(aContainer.Reco_GeneralPropertyAssociation, 820);
  GetTypenums().Bind(aContainer.Reco_GeneralPropertyRelationship, 821);
  GetTypenums().Bind(aContainer.Reco_BooleanRepresentationItem, 822);
  GetTypenums().Bind(aContainer.Reco_RealRepresentationItem, 823);
  GetTypenums().Bind(aContainer.Reco_MechanicalDesignAndDraughtingRelationship, 824);

  //    SHORT NAMES
  //    NB : la liste est celle de AP203
  //    Directement exploite pour les types simples
  //    Pour les types complexes, l option prise est de convertir les noms courts
  //    en noms longs et de refaire l essai

  // 203  GetTypeshor().Bind ("ACTASS",ACTION_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("ACTDRC",ACTION_DIRECTIVE);
  // 203  GetTypeshor().Bind ("ACTMTH",ACTION_METHOD);
  // 203  GetTypeshor().Bind ("ACRQAS",ACTION_REQUEST_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("ACRQSL",ACTION_REQUEST_SOLUTION);
  // 203  GetTypeshor().Bind ("ACRQST",ACTION_REQUEST_STATUS);
  // 203  GetTypeshor().Bind ("ACTSTT",ACTION_STATUS);
  GetTypeshor().Bind("ADDRSS", 1);
  GetTypeshor().Bind("ABSR", 2);
  GetTypeshor().Bind("ADVFC", 3);
  GetTypeshor().Bind("ANCROC", 4);
  GetTypeshor().Bind("ANFLAR", 5);
  GetTypeshor().Bind("AFAO", 6);
  GetTypeshor().Bind("ANNOCC", 7);
  GetTypeshor().Bind("ANSBOC", 8);
  GetTypeshor().Bind("ANNSYM", 9);
  GetTypeshor().Bind("ANSYOC", 10);
  GetTypeshor().Bind("ANNTXT", 11);
  GetTypeshor().Bind("ANTXOC", 12);
  // 203  GetTypeshor().Bind ("ALPRRL",ALTERNATE_PRODUCT_RELATIONSHIP);
  GetTypeshor().Bind("APPCNT", 13);
  GetTypeshor().Bind("APCNEL", 14);
  GetTypeshor().Bind("APPRDF", 15);
  GetTypeshor().Bind("APPRVL", 16);
  GetTypeshor().Bind("APPASS", 17);
  GetTypeshor().Bind("APDTTM", 348);
  GetTypeshor().Bind("APPROR", 18);
  GetTypeshor().Bind("APPRLT", 19);
  GetTypeshor().Bind("APPRL", 20);
  GetTypeshor().Bind("APPSTT", 21);
  GetTypeshor().Bind("ARINST", 22);
  // 203  GetTypeshor().Bind ("AMWU",AREA_MEASURE_WITH_UNIT);
  // 203  GetTypeshor().Bind ("ARUNT",AREA_UNIT);
  GetTypeshor().Bind("ASCMUS", 379);
  GetTypeshor().Bind("ACUS", 384);
  GetTypeshor().Bind("AX1PLC", 35);
  GetTypeshor().Bind("A2PL2D", 36);
  GetTypeshor().Bind("A2PL3D", 37);
  GetTypeshor().Bind("BZRCRV", 43);
  GetTypeshor().Bind("BZRSRF", 44);
  GetTypeshor().Bind("BLNRSL", 46);
  GetTypeshor().Bind("BNDCR", 47);
  GetTypeshor().Bind("BNDCRV", 48);
  GetTypeshor().Bind("BNDSRF", 49);
  GetTypeshor().Bind("BRWTVD", 52);
  GetTypeshor().Bind("BSPCR", 38);
  GetTypeshor().Bind("BSCWK", 39);
  GetTypeshor().Bind("BSPSR", 40);
  GetTypeshor().Bind("BSSWK", 41);
  GetTypeshor().Bind("BXDMN", 50);
  GetTypeshor().Bind("BXHLSP", 51);
  GetTypeshor().Bind("CLNDT", 53);
  GetTypeshor().Bind("CMRIMG", 54);
  GetTypeshor().Bind("CMRMDL", 55);
  GetTypeshor().Bind("CMMDD3", 57);
  GetTypeshor().Bind("CMRUSG", 58);
  GetTypeshor().Bind("CRTPNT", 59);
  GetTypeshor().Bind("CRTROP", 60);
  // 203  GetTypeshor().Bind ("CTO2",CARTESIAN_TRANSFORMATION_OPERATOR_2D);
  GetTypeshor().Bind("CTO3", 61);
  // 203  GetTypeshor().Bind ("CCDSAP",CC_DESIGN_APPROVAL);
  // 203  GetTypeshor().Bind ("CCDSCR",CC_DESIGN_CERTIFICATION);
  // 203  GetTypeshor().Bind ("CCDSCN",CC_DESIGN_CONTRACT);
  // 203  GetTypeshor().Bind ("CDDATA",CC_DESIGN_DATE_AND_TIME_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("CDPAOA",CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("CDSC",CC_DESIGN_SECURITY_CLASSIFICATION);
  // 203  GetTypeshor().Bind ("CDS",CC_DESIGN_SPECIFICATION_REFERENCE);
  // 203  GetTypeshor().Bind ("CRTFCT",CERTIFICATION);
  // 203  GetTypeshor().Bind ("CRTASS",CERTIFICATION_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("CRTTYP",CERTIFICATION_TYPE);

  // 203  GetTypeshor().Bind ("CHNRQS",CHANGE_REQUEST);

  GetTypeshor().Bind("CLSSHL", 63);
  GetTypeshor().Bind("CLRRGB", 65);
  GetTypeshor().Bind("CLRSPC", 66);
  GetTypeshor().Bind("CMPCRV", 67);
  GetTypeshor().Bind("CCOS", 68);
  GetTypeshor().Bind("CMCRSG", 69);
  // 203  GetTypeshor().Bind ("CNFDSG",CONFIGURATION_DESIGN);
  // 203  GetTypeshor().Bind ("CNFEFF",CONFIGURATION_EFFECTIVITY);
  // 203  GetTypeshor().Bind ("CNFITM",CONFIGURATION_ITEM);

  GetTypeshor().Bind("CMPTXT", 70);
  GetTypeshor().Bind("CTWAC", 71);
  GetTypeshor().Bind("CTWBB", 72);
  GetTypeshor().Bind("CTWE", 73);
  GetTypeshor().Bind("CNCSRF", 75);
  // 203  GetTypeshor().Bind ("CNEDST",CONNECTED_EDGE_SET);
  GetTypeshor().Bind("CNFCST", 76);
  GetTypeshor().Bind("CNDPIN", 77);
  GetTypeshor().Bind("CDORSI", 78);
  GetTypeshor().Bind("CDSR", 391);
  // 203  GetTypeshor().Bind ("CNDPUN",CONTEXT_DEPENDENT_UNIT);
  // 203  GetTypeshor().Bind ("CNTRCT",CONTRACT);
  // 203  GetTypeshor().Bind ("CNTASS",CONTRACT_ASSIGNMENT);
  // 203  GetTypeshor().Bind ("CNTTYP",CONTRACT_TYPE);
  GetTypeshor().Bind("CNBSUN", 79);
  GetTypeshor().Bind("CUTO", 80);
  GetTypeshor().Bind("CSSHRP", 82);
  GetTypeshor().Bind("CSGSLD", 83);

  GetTypeshor().Bind("CRBNSR", 85);
  GetTypeshor().Bind("CRVRPL", 86);
  GetTypeshor().Bind("CRVSTY", 87);
  GetTypeshor().Bind("CRSTFN", 88);
  GetTypeshor().Bind("CSFP", 89);
  GetTypeshor().Bind("CYLSRF", 90);

  // 203  GetTypeshor().Bind ("DTDEFF",DATED_EFFECTIVITY);
  GetTypeshor().Bind("DTANTM", 92);
  GetTypeshor().Bind("DATA", 93);
  GetTypeshor().Bind("DTASS", 94);
  GetTypeshor().Bind("DTRL", 95);
  GetTypeshor().Bind("DTTMRL", 96);
  GetTypeshor().Bind("DFNSYM", 97);
  GetTypeshor().Bind("DFNRPR", 98);
  GetTypeshor().Bind("DGNPCR", 99);
  GetTypeshor().Bind("DGTRSR", 100);
  GetTypeshor().Bind("DSRPIT", 101);
  GetTypeshor().Bind("DMNCRV", 102);
  GetTypeshor().Bind("DMCRTR", 103);
  GetTypeshor().Bind("DSGCNT", 340);
  // 203  GetTypeshor().Bind ("DMFR",DESIGN_MAKE_FROM_RELATIONSHIP);
  GetTypeshor().Bind("DMNEXP", 104);
  // 203  GetTypeshor().Bind ("DRCACT",DIRECTED_ACTION);
  GetTypeshor().Bind("DRCTN", 105);
  GetTypeshor().Bind("DRANOC", 106);
  GetTypeshor().Bind("DRGCLL", 107);
  GetTypeshor().Bind("DPDC", 108);
  GetTypeshor().Bind("DPDCF", 109);
  GetTypeshor().Bind("DRSBRP", 110);
  GetTypeshor().Bind("DRSYRP", 111);
  GetTypeshor().Bind("DTLWD", 112);
  GetTypeshor().Bind("DRWDFN", 113);
  GetTypeshor().Bind("DRWRVS", 114);
  GetTypeshor().Bind("DCMNT", 367);
  // 203  GetTypeshor().Bind ("DCMRFR",DOCUMENT_REFERENCE);
  GetTypeshor().Bind("DCMRLT", 369);
  GetTypeshor().Bind("DCMTYP", 370);
  GetTypeshor().Bind("DCUSCN", 371);
  // 203  GetTypeshor().Bind ("DCWTCL",DOCUMENT_WITH_CLASS);

  // 203  GetTypeshor().Bind ("EBWM",EDGE_BASED_WIREFRAME_MODEL);
  // 203  GetTypeshor().Bind ("EBWSR",EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION);
  GetTypeshor().Bind("EDGCRV", 116);
  GetTypeshor().Bind("EDGLP", 117);
  GetTypeshor().Bind("EFFCTV", 372);
  GetTypeshor().Bind("ELMSRF", 118);
  GetTypeshor().Bind("ELLPS", 119);
  GetTypeshor().Bind("EVDGPC", 120);
  GetTypeshor().Bind("EXTSRC", 121);
  GetTypeshor().Bind("EDCF", 122);
  GetTypeshor().Bind("EDHS", 123);
  GetTypeshor().Bind("EXDFIT", 124);
  GetTypeshor().Bind("EXDFSY", 125);
  GetTypeshor().Bind("EDTF", 126);
  GetTypeshor().Bind("EDTS", 127);
  GetTypeshor().Bind("EXARSL", 128);
  // 203  GetTypeshor().Bind ("EXCACT",EXECUTED_ACTION);

  GetTypeshor().Bind("FCBND", 131);
  GetTypeshor().Bind("FCOTBN", 132);
  GetTypeshor().Bind("FCSRF", 133);
  GetTypeshor().Bind("FCTBR", 134);
  GetTypeshor().Bind("FBSR", 135);
  GetTypeshor().Bind("FLARST", 136);
  GetTypeshor().Bind("FASC", 137);
  GetTypeshor().Bind("FASH", 138);
  GetTypeshor().Bind("FASTSW", 139);
  GetTypeshor().Bind("FAST", 140);
  GetTypeshor().Bind("FNDFTR", 141);
  GetTypeshor().Bind("GBSSR", 146);
  GetTypeshor().Bind("GBWSR", 147);
  GetTypeshor().Bind("GMCRST", 142);
  GetTypeshor().Bind("GMRPCN", 143);
  GetTypeshor().Bind("GMRPIT", 144);
  GetTypeshor().Bind("GMTST", 145);
  GetTypeshor().Bind("GC", 148);
  GetTypeshor().Bind("GUAC", 149);
  GetTypeshor().Bind("GRPASS", 151);
  GetTypeshor().Bind("GRPRLT", 152);
  GetTypeshor().Bind("HLSPSL", 153);
  GetTypeshor().Bind("HYPRBL", 154);
  GetTypeshor().Bind("INTCRV", 155);
  GetTypeshor().Bind("INVSBL", 156);
  GetTypeshor().Bind("ITDFTR", 354);
  GetTypeshor().Bind("LMWU", 157);
  GetTypeshor().Bind("LNGUNT", 158);

  GetTypeshor().Bind("LCLTM", 160);

  // 203  GetTypeshor().Bind ("LTEFF",LOT_EFFECTIVITY);
  GetTypeshor().Bind("MNSLBR", 162);
  GetTypeshor().Bind("MSSR", 163);
  GetTypeshor().Bind("MPPITM", 164);
  GetTypeshor().Bind("MDGPA", 166);
  GetTypeshor().Bind("MDGPR", 167);
  // 203  GetTypeshor().Bind ("MMWU",MASS_MEASURE_WITH_UNIT);
  // 203  GetTypeshor().Bind ("MSSUNT",MASS_UNIT);
  GetTypeshor().Bind("MSWTUN", 165);
  GetTypeshor().Bind("MCHCNT", 339);
  GetTypeshor().Bind("NMDUNT", 169);
  GetTypeshor().Bind("NAUO", 380);
  // 203  GetTypeshor().Bind ("OFCR2D",OFFSET_CURVE_2D);
  GetTypeshor().Bind("OFCR3D", 171);
  GetTypeshor().Bind("OFFSRF", 172);
  GetTypeshor().Bind("ODRF", 173);
  GetTypeshor().Bind("OPNSHL", 174);
  GetTypeshor().Bind("ORDDT", 175);
  GetTypeshor().Bind("ORGNZT", 176);
  GetTypeshor().Bind("ORGASS", 177);
  GetTypeshor().Bind("ORGRL", 178);
  GetTypeshor().Bind("ORGADD", 179);
  // 203  GetTypeshor().Bind ("ORGPRJ",ORGANIZATIONAL_PROJECT);
  // 203  GetTypeshor().Bind ("ORGRLT",ORGANIZATION_RELATIONSHIP);
  GetTypeshor().Bind("ORCLSH", 180);
  GetTypeshor().Bind("ORNEDG", 181);
  GetTypeshor().Bind("ORNFC", 182);
  GetTypeshor().Bind("OROPSH", 183);
  GetTypeshor().Bind("ORNPTH", 184);
  GetTypeshor().Bind("OTBNCR", 185);
  GetTypeshor().Bind("ORSI", 186);
  GetTypeshor().Bind("PRBL", 187);
  GetTypeshor().Bind("PRRPCN", 188);

  GetTypeshor().Bind("PRANOR", 192);
  GetTypeshor().Bind("PAOA", 193);
  GetTypeshor().Bind("PAOR", 194);
  GetTypeshor().Bind("PRSADD", 195);
  GetTypeshor().Bind("PLCMNT", 196);
  GetTypeshor().Bind("PLNBX", 197);
  GetTypeshor().Bind("PLNEXT", 198);

  GetTypeshor().Bind("PAMWU", 200);
  GetTypeshor().Bind("PLANUN", 201);

  GetTypeshor().Bind("PNONCR", 203);
  GetTypeshor().Bind("PNONSR", 204);
  GetTypeshor().Bind("PNTRPL", 205);
  GetTypeshor().Bind("PNTSTY", 206);
  GetTypeshor().Bind("PLYLP", 207);
  GetTypeshor().Bind("PLYLN", 208);
  GetTypeshor().Bind("PRDFCL", 209);
  GetTypeshor().Bind("PDCF", 210);
  GetTypeshor().Bind("PRDFIT", 211);
  GetTypeshor().Bind("PRDFSY", 212);
  GetTypeshor().Bind("PDTF", 213);
  GetTypeshor().Bind("PRSAR", 214);
  GetTypeshor().Bind("PRLYAS", 215);
  GetTypeshor().Bind("PRSRPR", 216);
  GetTypeshor().Bind("PRSST", 217);
  GetTypeshor().Bind("PRSSZ", 218);
  GetTypeshor().Bind("PRSTAS", 219);
  GetTypeshor().Bind("PSBC", 220);
  GetTypeshor().Bind("PRSVW", 221);
  GetTypeshor().Bind("PRSITM", 222);
  GetTypeshor().Bind("PRDCT", 223);
  GetTypeshor().Bind("PRDCTG", 224);
  // 203  GetTypeshor().Bind ("PRCTRL",PRODUCT_CATEGORY_RELATIONSHIP);
  // 203  GetTypeshor().Bind ("PRDCNC",PRODUCT_CONCEPT);
  // 203  GetTypeshor().Bind ("PRCNCN",PRODUCT_CONCEPT_CONTEXT);
  GetTypeshor().Bind("PRDCNT", 225);
  GetTypeshor().Bind("PRDDFN", 227);
  GetTypeshor().Bind("PRDFCN", 228);
  GetTypeshor().Bind("PRDFEF", 373);
  GetTypeshor().Bind("PRDFFR", 229);
  GetTypeshor().Bind("PDFWSS", 230);
  GetTypeshor().Bind("PRDFRL", 374);
  GetTypeshor().Bind("PRDFSH", 231);
  GetTypeshor().Bind("PRDFUS", 377);
  GetTypeshor().Bind("PDWAD", 375);
  GetTypeshor().Bind("PRPC", 232);
  GetTypeshor().Bind("PRUSOC", 381);
  GetTypeshor().Bind("PRPDFN", 234);
  GetTypeshor().Bind("PRDFRP", 235);
  GetTypeshor().Bind("QACU", 382);
  GetTypeshor().Bind("QSUNCR", 236);
  GetTypeshor().Bind("QSUNSR", 237);
  GetTypeshor().Bind("RMWU", 238);
  GetTypeshor().Bind("RBSC", 239);
  GetTypeshor().Bind("RBSS", 240);
  GetTypeshor().Bind("RCCMSR", 241);
  GetTypeshor().Bind("RCTRSR", 242);
  GetTypeshor().Bind("RPITGR", 243);
  GetTypeshor().Bind("RCCS", 244);
  GetTypeshor().Bind("RPRSNT", 245);
  GetTypeshor().Bind("RPRCNT", 246);
  GetTypeshor().Bind("RPRITM", 247);
  GetTypeshor().Bind("RPRMP", 248);
  GetTypeshor().Bind("RPRRLT", 249);
  GetTypeshor().Bind("RVARSL", 250);
  GetTypeshor().Bind("RGANWD", 251);
  GetTypeshor().Bind("RGCRCN", 252);
  GetTypeshor().Bind("RGCRCY", 253);
  GetTypeshor().Bind("RRWT", 388);
  GetTypeshor().Bind("SMCRV", 254);
  GetTypeshor().Bind("SCRCLS", 255);
  GetTypeshor().Bind("SCCLAS", 256);
  GetTypeshor().Bind("SCCLLV", 257);
  // 203  GetTypeshor().Bind ("SRNMEF",SERIAL_NUMBERED_EFFECTIVITY);
  GetTypeshor().Bind("SHPASP", 258);
  GetTypeshor().Bind("SHASRL", 259);
  GetTypeshor().Bind("SHDFRP", 261);
  GetTypeshor().Bind("SHPRPR", 262);
  GetTypeshor().Bind("SHRPRL", 387);
  GetTypeshor().Bind("SBSM", 263);
  // 203  GetTypeshor().Bind ("SBWM",SHELL_BASED_WIREFRAME_MODEL);
  // 203  GetTypeshor().Bind ("SBWSR",SHELL_BASED_WIREFRAME_SHAPE_REPRESENTATION);
  GetTypeshor().Bind("SUNT", 264);
  GetTypeshor().Bind("SAMWU", 265);
  GetTypeshor().Bind("SLANUN", 336);
  GetTypeshor().Bind("SLDMDL", 266);
  GetTypeshor().Bind("SLDRPL", 267);
  GetTypeshor().Bind("SHUO", 383);
  GetTypeshor().Bind("SPHSRF", 269);
  GetTypeshor().Bind("STYITM", 270);
  // 203  GetTypeshor().Bind ("STRRQS",START_REQUEST);
  // 203  GetTypeshor().Bind ("STRWRK",START_WORK);
  GetTypeshor().Bind("SPPRRL", 385);
  GetTypeshor().Bind("SRFC", 271);
  GetTypeshor().Bind("SRFCRV", 272);
  GetTypeshor().Bind("SL", 273);
  GetTypeshor().Bind("SROFRV", 274);
  GetTypeshor().Bind("SRFPTC", 275);
  GetTypeshor().Bind("SRFRPL", 276);
  GetTypeshor().Bind("SRSDST", 277);
  GetTypeshor().Bind("SRSTBN", 278);
  GetTypeshor().Bind("SSCG", 279);
  GetTypeshor().Bind("SSFA", 280);
  GetTypeshor().Bind("SSPL", 281);
  GetTypeshor().Bind("SSSC", 282);
  GetTypeshor().Bind("SRSTSL", 283);
  GetTypeshor().Bind("SRSTUS", 284);
  GetTypeshor().Bind("SWARSL", 285);
  GetTypeshor().Bind("SWPSRF", 286);
  GetTypeshor().Bind("SYMCLR", 287);
  GetTypeshor().Bind("SYMRPR", 288);
  GetTypeshor().Bind("SYRPMP", 289);
  GetTypeshor().Bind("SYMSTY", 290);
  GetTypeshor().Bind("SYMTRG", 291);
  GetTypeshor().Bind("TRMSYM", 294);
  GetTypeshor().Bind("TXTLTR", 295);
  GetTypeshor().Bind("TLWAC", 296);
  GetTypeshor().Bind("TLWBB", 297);
  GetTypeshor().Bind("TLWD", 298);
  GetTypeshor().Bind("TLWE", 299);
  GetTypeshor().Bind("TXTSTY", 300);
  GetTypeshor().Bind("TSFDF", 301);
  GetTypeshor().Bind("TSWBC", 302);
  GetTypeshor().Bind("TSWM", 303);
  GetTypeshor().Bind("TPRPIT", 304);
  GetTypeshor().Bind("TRDSRF", 305);
  GetTypeshor().Bind("TRMCRV", 308);
  GetTypeshor().Bind("TDRF", 309);
  GetTypeshor().Bind("UMWU", 310);
  GetTypeshor().Bind("UNFCRV", 311);
  GetTypeshor().Bind("UNFSRF", 312);

  // 203  GetTypeshor().Bind ("VRACRQ",VERSIONED_ACTION_REQUEST);

  GetTypeshor().Bind("VRTLP", 315);
  GetTypeshor().Bind("VRTPNT", 316);
  // 203  GetTypeshor().Bind ("VRTSHL",VERTEX_SHELL);
  // 203  GetTypeshor().Bind ("VMWU",VOLUME_MEASURE_WITH_UNIT);
  // 203  GetTypeshor().Bind ("VLMUNT",VOLUME_UNIT);
  GetTypeshor().Bind("VWVLM", 317);
  GetTypeshor().Bind("WOYADD", 318);
  GetTypeshor().Bind("TMWU", 341);
  GetTypeshor().Bind("RTUNT", 342);
  GetTypeshor().Bind("TMUNT", 343);
  GetTypeshor().Bind("CI3WS", 350);
  GetTypeshor().Bind("CTO2", 351);
  GetTypeshor().Bind("DRVUNT", 352);
  GetTypeshor().Bind("DRUNEL", 353);
  GetTypeshor().Bind("PRITRP", 355);
  GetTypeshor().Bind("MFUO", 378);
  // 203  GetTypeshor().Bind ("WRSHL",WIRE_SHELL);
  GetTypeshor().Bind("MTRDSG", 390);
  GetTypeshor().Bind("ADATA", 392);
  GetTypeshor().Bind("APDTAS", 393);
  GetTypeshor().Bind("APGRAS", 395);
  GetTypeshor().Bind("APORAS", 396);
  GetTypeshor().Bind("APAOA", 397);
  GetTypeshor().Bind("APPRIT", 398);
  GetTypeshor().Bind("ASCA", 399);
  GetTypeshor().Bind("APDCRF", 400);
  GetTypeshor().Bind("DCMFL", 401);
  GetTypeshor().Bind("CHROBJ", 402);
  GetTypeshor().Bind("EXFCSL", 403);
  GetTypeshor().Bind("RVFCSL", 404);
  GetTypeshor().Bind("SWFCSL", 405);

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  GetTypeshor().Bind("MSRPIT", 406);
  GetTypeshor().Bind("ARUNT", 407);
  GetTypeshor().Bind("VLMUNT", 408);

  // Added by ABV 10.11.99 for AP203
  GetTypeshor().Bind("ACTION", 413);
  GetTypeshor().Bind("ACTASS", 414);
  GetTypeshor().Bind("ACTMTH", 415);
  GetTypeshor().Bind("ACRQAS", 416);
  GetTypeshor().Bind("CCDSAP", 417);
  GetTypeshor().Bind("CCDSCR", 418);
  GetTypeshor().Bind("CCDSCN", 419);
  GetTypeshor().Bind("CDDATA", 420);
  GetTypeshor().Bind("CDPAOA", 421);
  GetTypeshor().Bind("CDSC", 422);
  GetTypeshor().Bind("CDS", 423);
  GetTypeshor().Bind("CRTFCT", 424);
  GetTypeshor().Bind("CRTASS", 425);
  GetTypeshor().Bind("CRTTYP", 426);
  GetTypeshor().Bind("CHANGE", 427);
  GetTypeshor().Bind("CHNRQS", 428);
  GetTypeshor().Bind("CNFDSG", 429);
  GetTypeshor().Bind("CNFEFF", 430);
  GetTypeshor().Bind("CNTRCT", 431);
  GetTypeshor().Bind("CNTASS", 432);
  GetTypeshor().Bind("CNTTYP", 433);
  GetTypeshor().Bind("PRDCNC", 434);
  GetTypeshor().Bind("PRCNCN", 435);
  GetTypeshor().Bind("STRRQS", 436);
  GetTypeshor().Bind("STRWRK", 437);
  GetTypeshor().Bind("VRACRQ", 438);
  GetTypeshor().Bind("PRCTRL", 439);
  GetTypeshor().Bind("ACRQSL", 440);
  GetTypeshor().Bind("DRGMDL", 441);
  GetTypeshor().Bind("ANGLCT", 442);
  GetTypeshor().Bind("ANGSZ", 443);
  GetTypeshor().Bind("DMCHRP", 444);
  GetTypeshor().Bind("DMNLCT", 445);
  GetTypeshor().Bind("DLWP", 446);
  GetTypeshor().Bind("DMNSZ", 447);
  GetTypeshor().Bind("DSWP", 448);
  GetTypeshor().Bind("SHDMRP", 449);
  GetTypeshor().Bind("DCRPTY", 450);
  GetTypeshor().Bind("OBJRL", 451);
  GetTypeshor().Bind("RLASS", 452);
  GetTypeshor().Bind("IDNRL", 453);
  GetTypeshor().Bind("IDNASS", 454);
  GetTypeshor().Bind("EXIDAS", 455);
  GetTypeshor().Bind("EFFASS", 456);
  GetTypeshor().Bind("NMASS", 457);
  GetTypeshor().Bind("GNRPRP", 458);
  GetTypeshor().Bind("EDGP", 461);
  GetTypeshor().Bind("AEIA", 462);
  GetTypeshor().Bind("CMSHAS", 470);
  GetTypeshor().Bind("DRSHAS", 471);
  GetTypeshor().Bind("EXTNSN", 472);
  GetTypeshor().Bind("DRDMLC", 473);
  GetTypeshor().Bind("LMANFT", 474);
  GetTypeshor().Bind("TLRVL", 475);
  GetTypeshor().Bind("MSRQLF", 476);
  GetTypeshor().Bind("PLMNTL", 477);
  GetTypeshor().Bind("PRCQLF", 478);
  GetTypeshor().Bind("TYPQLF", 479);
  GetTypeshor().Bind("QLRPIT", 480);
  GetTypeshor().Bind("CMRPIT", 482);
  GetTypeshor().Bind("CMRPIT", 483);
  GetTypeshor().Bind("CMS0", 485);
  GetTypeshor().Bind("CNEDST", 486);
  GetTypeshor().Bind("EBWM", 488);
  GetTypeshor().Bind("EBWSR", 489);
  GetTypeshor().Bind("NMSSR", 491);
  GetTypeshor().Bind("ORNSRF", 492);
  GetTypeshor().Bind("SBFC", 493);
  GetTypeshor().Bind("SBDG", 494);
  GetTypeshor().Bind("CFSS", 496);
  GetTypeshor().Bind("MSSUNT", 501);
  GetTypeshor().Bind("THTMUN", 502);
  GetTypeshor().Bind("DTENV", 565);
  GetTypeshor().Bind("MTPRRP", 566);
  GetTypeshor().Bind("PRDFR", 567);
  GetTypeshor().Bind("MTRPRP", 569);
  GetTypeshor().Bind("PDFR", 573);
  GetTypeshor().Bind("DCP1", 600);
  GetTypeshor().Bind("DCPREQ", 601);
  //  GetTypeshor().Bind (AngularLocation);
  //  GetTypeshor().Bind (AngularSize);
  //  GetTypeshor().Bind (DimensionalCharacteristicRepresentation);
  //  GetTypeshor().Bind (DimensionalLocation);
  //  GetTypeshor().Bind (DimensionalLocationWithPath);
  //  GetTypeshor().Bind (DimensionalSize);
  //  GetTypeshor().Bind (DimensionalSizeWithPath);
  //  GetTypeshor().Bind (ShapeDimensionRepresentation);
  GetTypeshor().Bind("CYLTLR", 609);
  GetTypeshor().Bind("SRWP", 610);
  GetTypeshor().Bind("ANGTLR", 611);
  GetTypeshor().Bind("CNCTLR", 612);
  GetTypeshor().Bind("CRRNTL", 613);
  GetTypeshor().Bind("CXLTLR", 614);
  GetTypeshor().Bind("FLTTLR", 615);
  GetTypeshor().Bind("LNP0", 616);
  GetTypeshor().Bind("PRLTLR", 617);
  GetTypeshor().Bind("PRPTLR", 618);
  GetTypeshor().Bind("PSTTLR", 619);
  GetTypeshor().Bind("RNDTLR", 620);
  GetTypeshor().Bind("STRTLR", 621);
  GetTypeshor().Bind("SRPRTL", 622);
  GetTypeshor().Bind("SYMTLR", 623);
  GetTypeshor().Bind("TTRNTL", 624);
  GetTypeshor().Bind("GMTTLR", 625);
  GetTypeshor().Bind("GMTLRL", 626);
  GetTypeshor().Bind("GTWDR", 627);
  GetTypeshor().Bind("MDGMTL", 628);
  GetTypeshor().Bind("DTMFTR", 630);
  GetTypeshor().Bind("DTMRFR", 631);
  GetTypeshor().Bind("CMMDTM", 632);
  GetTypeshor().Bind("DTMTRG", 633);
  GetTypeshor().Bind("PDT0", 634);
  GetTypeshor().Bind("MMWU", 651);
  GetTypeshor().Bind("CNOFSY", 661);
  GetTypeshor().Bind("GMTALG", 662);
  GetTypeshor().Bind("PRPT", 663);
  GetTypeshor().Bind("TNGNT", 664);
  GetTypeshor().Bind("PRLOFF", 665);
  GetTypeshor().Bind("GISU", 666);
  GetTypeshor().Bind("IDATT", 667);
  GetTypeshor().Bind("IIRU", 668);
  GetTypeshor().Bind("GTWDU", 674);
  GetTypeshor().Bind("PRZNDF", 679);
  GetTypeshor().Bind("RNZNDF", 680);
  GetTypeshor().Bind("RNZNOR", 681);
  GetTypeshor().Bind("TLRZN", 682);
  GetTypeshor().Bind("TLZNDF", 683);
  GetTypeshor().Bind("TLZNFR", 684);
  GetTypeshor().Bind("INRPIT", 700);
  GetTypeshor().Bind("VLRPIT", 701);
  GetTypeshor().Bind("DMIA", 703);
  GetTypeshor().Bind("ANNPLN", 704);
  GetTypeshor().Bind("CNGMRP", 712);
  GetTypeshor().Bind("CGRR", 713);
}

// --- Case Recognition ---

Standard_Integer RWStepAP214_ReadWriteModule::CaseStep(const TCollection_AsciiString& key) const
{
  // FMA - le 25-07-96 : Optimisation -> on teste en premier les types les plus
  //                     frequents dans le fichier cad geometry/topology
  Standard_Integer num;
  if (key.IsEqual("CARTESIAN_POINT"))
    return 59; // car tres courant
  if (GetTypenums().Find(key, num))
    return num;
  if (GetTypeshor().Find(key, num))
    return num; // AJOUT DES TYPES COURTS
  return 0;
}

// --- External Mapping Case Recognition ---

//=================================================================================================

Standard_Integer RWStepAP214_ReadWriteModule::CaseStep(
  const TColStd_SequenceOfAsciiString& theTypes) const
{

  // Optimized by FMA : le test sur le nombre de composant est repete meme
  //                    si la valeur testee est la meme.

  Standard_Integer NbComp = theTypes.Length();
  if (NbComp < 2)
  {
#ifdef OCCT_DEBUG
    std::cout << "Not a Plex" << std::endl;
#endif
  }
  else
  {
    // SHORT TYPES
    //  Pas tres elegant : on convertit d abord
    //  Sinon, il faudrait sortir des routines
    Standard_Integer i, num = 0;
    for (i = 1; i <= NbComp; i++)
    {
      if (GetTypeshor().IsBound(theTypes(i)))
      {
        num = 1;
        break;
      }
    }
    if (num > 0)
    {
      TColStd_SequenceOfAsciiString longs;
      for (i = 1; i <= NbComp; i++)
      {
        if (GetTypeshor().Find(theTypes(i), num))
          longs.Append(StepType(num));
        else
          longs.Append(theTypes(i));
      }
      return CaseStep(longs);
    }

    // sln 03.10.2001. BUC61003. Correction of alphabetic order of complex entity's items (ascending
    // sorting)
    TColStd_SequenceOfAsciiString types;
    for (i = 1; i <= theTypes.Length(); i++)
      types.Append(theTypes(i));
    // do ascending sorting
    Standard_Boolean        isOK = Standard_False;
    TCollection_AsciiString tmpStr;
    Standard_Integer        aLen = types.Length() - 1;
    while (!isOK)
    {
      isOK = Standard_True;
      for (i = 1; i <= aLen; i++)
        if (types(i) > types(i + 1))
        {
          tmpStr       = types(i);
          types(i)     = types(i + 1);
          types(i + 1) = tmpStr;
          isOK         = Standard_False;
        }
    }

    if (NbComp == 8)
    {
      if ((types(1).IsEqual(StepType(729))) && (types(2).IsEqual(StepType(144)))
          && (types(3).IsEqual(StepType(354))) && (types(4).IsEqual(StepType(799)))
          && (types(5).IsEqual(StepType(743)))
          && (((types(6).IsEqual(StepType(759))) && (types(7).IsEqual(StepType(761))))
              || ((types(6).IsEqual(StepType(731))) && (types(7).IsEqual(StepType(733))))
              || ((types(6).IsEqual(StepType(767))) && (types(7).IsEqual(StepType(769))))
              || ((types(6).IsEqual(StepType(789))) && (types(7).IsEqual(StepType(791))))
              || ((types(6).IsEqual(StepType(781))) && (types(7).IsEqual(StepType(785))))
              || ((types(6).IsEqual(StepType(783))) && (types(7).IsEqual(StepType(784)))))
          && (types(8).IsEqual(StepType(247))))
      {
        return 800;
      }
    }
    else if (NbComp == 7)
    {
      if ((types(1).IsEqual(StepType(48))) && (types(2).IsEqual(StepType(38)))
          && (types(3).IsEqual(StepType(84))) && (types(4).IsEqual(StepType(144)))
          && (types(5).IsEqual(StepType(239))) && (types(6).IsEqual(StepType(247)))
          && (types(7).IsEqual(StepType(311))))
      {
        return 319;
      }
      else if ((types(1).IsEqual(StepType(48))) && (types(2).IsEqual(StepType(38)))
               && (types(3).IsEqual(StepType(39))) && (types(4).IsEqual(StepType(84)))
               && (types(5).IsEqual(StepType(144))) && (types(6).IsEqual(StepType(239)))
               && (types(7).IsEqual(StepType(247))))
      {
        return 320;
      }
      else if ((types(1).IsEqual(StepType(48))) && (types(2).IsEqual(StepType(38)))
               && (types(3).IsEqual(StepType(84))) && (types(4).IsEqual(StepType(144)))
               && (types(5).IsEqual(StepType(236))) && (types(6).IsEqual(StepType(239)))
               && (types(7).IsEqual(StepType(247))))
      {
        return 321;
      }
      else if ((types(1).IsEqual(StepType(43))) && (types(2).IsEqual(StepType(48)))
               && (types(3).IsEqual(StepType(38))) && (types(4).IsEqual(StepType(84)))
               && (types(5).IsEqual(StepType(144))) && (types(6).IsEqual(StepType(239)))
               && (types(7).IsEqual(StepType(247))))
      {
        return 322;
      }
      else if ((types(1).IsEqual(StepType(49))) && (types(2).IsEqual(StepType(40)))
               && (types(3).IsEqual(StepType(41))) && (types(4).IsEqual(StepType(144)))
               && (types(5).IsEqual(StepType(240))) && (types(6).IsEqual(StepType(247)))
               && (types(7).IsEqual(StepType(271))))
      {
        return 323;
      }
      else if ((types(1).IsEqual(StepType(49))) && (types(2).IsEqual(StepType(40)))
               && (types(3).IsEqual(StepType(144))) && (types(4).IsEqual(StepType(240)))
               && (types(5).IsEqual(StepType(247))) && (types(6).IsEqual(StepType(271)))
               && (types(7).IsEqual(StepType(312))))
      {
        return 324;
      }
      else if ((types(1).IsEqual(StepType(49))) && (types(2).IsEqual(StepType(40)))
               && (types(3).IsEqual(StepType(144))) && (types(4).IsEqual(StepType(237)))
               && (types(5).IsEqual(StepType(240))) && (types(6).IsEqual(StepType(247)))
               && (types(7).IsEqual(StepType(271))))
      {
        return 325;
      }
      else if ((types(1).IsEqual(StepType(44))) && (types(2).IsEqual(StepType(49)))
               && (types(3).IsEqual(StepType(40))) && (types(4).IsEqual(StepType(144)))
               && (types(5).IsEqual(StepType(240))) && (types(6).IsEqual(StepType(247)))
               && (types(7).IsEqual(StepType(271))))
      {
        return 326;
      }
      else if ((types(1).IsEqual(StepType(40))) && (types(2).IsEqual(StepType(41)))
               && (types(3).IsEqual(StepType(49))) && (types(4).IsEqual(StepType(144)))
               && (types(5).IsEqual(StepType(240))) && (types(6).IsEqual(StepType(247)))
               && (types(7).IsEqual(StepType(271))))
      {
        return 323;
      }
      else if ((types(1).IsEqual(StepType(40))) && (types(2).IsEqual(StepType(41)))
               && (types(3).IsEqual(StepType(240))) && (types(4).IsEqual(StepType(49)))
               && (types(5).IsEqual(StepType(247))) && (types(6).IsEqual(StepType(144)))
               && (types(7).IsEqual(StepType(271))))
      {
        return 323;
      }
    }
    // Added by FMA
    else if (NbComp == 6)
    {
      if (types(1).IsEqual(StepType(52)) && types(2).IsEqual(StepType(134))
          && types(3).IsEqual(StepType(144)) && types(4).IsEqual(StepType(162))
          && types(5).IsEqual(StepType(247)) && types(6).IsEqual(StepType(266)))
      {
        return 337;
      }
    }
    else if (NbComp == 5)
    {
      //: n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
      if ((types(1).IsEqual(StepType(48))) && (types(2).IsEqual(StepType(84)))
          && (types(3).IsEqual(StepType(144))) && (types(4).IsEqual(StepType(247)))
          && (types(5).IsEqual(StepType(272))))
      {
        return 358;
      }
      else if ((types(1).IsEqual(StepType(157))) && (types(2).IsEqual(StepType(406)))
               && (types(3).IsEqual(StepType(165))) && (types(4).IsEqual(StepType(480)))
               && (types(5).IsEqual(StepType(247))))
      {
        return 692;
      }
      else if ((types(1).IsEqual(StepType(406))) && (types(2).IsEqual(StepType(165)))
               && (types(3).IsEqual(StepType(200))) && (types(4).IsEqual(StepType(480)))
               && (types(5).IsEqual(StepType(247))))
      {
        return 693;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(627))
                && types(4).IsEqual(StepType(675)) && types(5).IsEqual(StepType(676)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(627))
                   && types(3).IsEqual(StepType(675)) && types(4).IsEqual(StepType(676))
                   && (types(5).IsEqual(StepType(616)) || types(5).IsEqual(StepType(617))
                       || types(5).IsEqual(StepType(618)) || types(5).IsEqual(StepType(619))
                       || types(5).IsEqual(StepType(620)) || types(5).IsEqual(StepType(621))
                       || types(5).IsEqual(StepType(622)) || types(5).IsEqual(StepType(623))
                       || types(5).IsEqual(StepType(624)))))
      {
        return 705;
      }
      else if ((types(1).IsEqual(StepType(4))) && (types(2).IsEqual(StepType(7)))
               && (types(3).IsEqual(StepType(144))) && (types(4).IsEqual(StepType(247)))
               && (types(5).IsEqual(StepType(270))))
      {
        return 719;
      }
      else if ((types(1).IsEqual(StepType(144))) && (types(2).IsEqual(StepType(803)))
               && (types(3).IsEqual(StepType(247))) && (types(4).IsEqual(StepType(709)))
               && (types(5).IsEqual(StepType(708))))
      {
        return 802;
      }
    }
    else if (NbComp == 4)
    {
      if ((types(1).IsEqual(StepType(161))) && (types(2).IsEqual(StepType(189)))
          && (types(3).IsEqual(StepType(247))) && (types(4).IsEqual(StepType(304))))
      {
        return 332;
      }
      // Added by FMA
      else if ((types(1).IsEqual(StepType(143)) && types(2).IsEqual(StepType(148))
                && types(3).IsEqual(StepType(149)) && types(4).IsEqual(StepType(246))))
      {
        return 333;
      }

      else if ((types(1).IsEqual(StepType(157))) && (types(2).IsEqual(StepType(406)))
               && (types(3).IsEqual(StepType(165))) && (types(4).IsEqual(StepType(247))))
      {
        return 635;
      }
      else if ((types(1).IsEqual(StepType(625))) && (types(2).IsEqual(StepType(627)))
               && (types(3).IsEqual(StepType(628))) && (types(4).IsEqual(StepType(619))))
      {
        return 636;
      }
      else if ((types(1).IsEqual(StepType(406))) && (types(2).IsEqual(StepType(165)))
               && (types(3).IsEqual(StepType(200))) && (types(4).IsEqual(StepType(247))))
      {
        return 691;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(627))
                && types(4).IsEqual(StepType(676)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(627))
                   && types(3).IsEqual(StepType(676))
                   && (types(4).IsEqual(StepType(616)) || types(4).IsEqual(StepType(617))
                       || types(4).IsEqual(StepType(618)) || types(4).IsEqual(StepType(619))
                       || types(4).IsEqual(StepType(620)) || types(4).IsEqual(StepType(621))
                       || types(4).IsEqual(StepType(622)) || types(4).IsEqual(StepType(623))
                       || types(4).IsEqual(StepType(624)))))
      {
        return 695;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(627))
                && types(4).IsEqual(StepType(677)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(627))
                   && types(4).IsEqual(StepType(677))
                   && (types(3).IsEqual(StepType(616)) || types(3).IsEqual(StepType(617))
                       || types(3).IsEqual(StepType(618)) || types(3).IsEqual(StepType(619))
                       || types(3).IsEqual(StepType(620)) || types(3).IsEqual(StepType(621))
                       || types(3).IsEqual(StepType(622)) || types(3).IsEqual(StepType(623))
                       || types(3).IsEqual(StepType(624)))))
      {
        return 697;
      }
      else if (types(1).IsEqual(StepType(671)) && types(2).IsEqual(StepType(470))
               && types(3).IsEqual(StepType(630)) && types(4).IsEqual(StepType(258)))
      {
        return 698;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(675))
                && types(4).IsEqual(StepType(676)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(675))
                   && types(3).IsEqual(StepType(676))
                   && (types(4).IsEqual(StepType(616)) || types(4).IsEqual(StepType(617))
                       || types(4).IsEqual(StepType(618)) || types(4).IsEqual(StepType(619))
                       || types(4).IsEqual(StepType(620)) || types(4).IsEqual(StepType(621))
                       || types(4).IsEqual(StepType(622)) || types(4).IsEqual(StepType(623))
                       || types(4).IsEqual(StepType(624)))))
      {
        return 706;
      }
      else if (types(1).IsEqual(StepType(402)) && types(2).IsEqual(StepType(714))
               && types(3).IsEqual(StepType(441)) && types(4).IsEqual(StepType(245)))
      {
        return 715;
      }
    }
    else if (NbComp == 3)
    {
      if ((types(1).IsEqual(StepType(158))) && (types(2).IsEqual(StepType(169)))
          && (types(3).IsEqual(StepType(264))))
      {
        return 327;
      } // pdn t3d_opt
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(264)))
               && (types(3).IsEqual(StepType(158))))
      {
        return 327;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(201)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 328;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(158)))
               && (types(3).IsEqual(StepType(169))))
      {
        return 329;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(201))))
      {
        return 330;
      }
      else if ((types(1).IsEqual(StepType(143))) && (types(2).IsEqual(StepType(149)))
               && (types(3).IsEqual(StepType(246))))
      {
        return 331;
      }
      // Added by FMA
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(336))))
      {
        return 334;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(264)))
               && (types(3).IsEqual(StepType(336))))
      {
        return 335;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(336)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 335;
      }
      else if ((types(1).IsEqual(StepType(143))) && (types(2).IsEqual(StepType(188)))
               && (types(3).IsEqual(StepType(246))))
      {
        return 338;
      }
      // full Rev4 (CKY 30-MARS-1997)
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(342)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 344;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(264)))
               && (types(3).IsEqual(StepType(343))))
      {
        return 345;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(342))))
      {
        return 346;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(343))))
      {
        return 347;
      }
      else if ((types(1).IsEqual(StepType(157))) && (types(2).IsEqual(StepType(165)))
               && (types(3).IsEqual(StepType(310))))
      {
        return 357; // LECTURE SEULEMENT (READ ONLY), origine CATIA. CKY 2-SEP-1997
      }
      //      Additional non-alphabetic (CKY 5 MAI 1998)
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(158)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 327;
      }
      //      CC1 -> CC2 (CKY 1-JUL-1998)
      else if ((types(1).IsEqual(StepType(249))) && (types(2).IsEqual(StepType(388)))
               && (types(3).IsEqual(StepType(387))))
      {
        return 389;
      }
      else if ((types(1).IsEqual(StepType(407))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 409;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(264)))
               && (types(3).IsEqual(StepType(408))))
      {
        return 410;
      }
      else if ((types(1).IsEqual(StepType(407))) && (types(2).IsEqual(StepType(79)))
               && (types(3).IsEqual(StepType(169))))
      {
        return 411;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(408))))
      {
        return 412;
      }
      // abv 11.07.00: CAX-IF TRJ4: k1_geo-ac.stp
      else if ((types(1).IsEqual(StepType(98))) && (types(2).IsEqual(StepType(245)))
               && (types(3).IsEqual(StepType(262))))
      {
        return 463;
      }
      // CKY 25 APR 2001; CAX-IF TR7J (dim.tol.)
      else if ((types(1).IsEqual(StepType(406))) && (types(2).IsEqual(StepType(480)))
               && (types(3).IsEqual(StepType(247))))
      {
        return 481;
      }
      else if ((types(1).IsEqual(StepType(501))) && (types(2).IsEqual(StepType(169)))
               && (types(3).IsEqual(StepType(264))))
      {
        return 574;
      }
      else if ((types(1).IsEqual(StepType(169))) && (types(2).IsEqual(StepType(264)))
               && (types(3).IsEqual(StepType(502))))
      {
        return 578;
      }
      else if ((types(1).IsEqual(StepType(79))) && (types(2).IsEqual(StepType(501)))
               && (types(3).IsEqual(StepType(169))))
      {
        return 650;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(627)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(627))
                   && (types(3).IsEqual(StepType(616)) || types(3).IsEqual(StepType(617))
                       || types(3).IsEqual(StepType(618)) || types(3).IsEqual(StepType(619))
                       || types(3).IsEqual(StepType(620)) || types(3).IsEqual(StepType(621))
                       || types(3).IsEqual(StepType(622)) || types(3).IsEqual(StepType(623))
                       || types(3).IsEqual(StepType(624)))))
      {
        return 694;
      }
      else if (((types(1).IsEqual(StepType(609)) || types(1).IsEqual(StepType(611))
                 || types(1).IsEqual(StepType(612)) || types(1).IsEqual(StepType(613))
                 || types(1).IsEqual(StepType(614)) || types(1).IsEqual(StepType(615)))
                && types(2).IsEqual(StepType(625)) && types(3).IsEqual(StepType(676)))
               || (types(1).IsEqual(StepType(625)) && types(2).IsEqual(StepType(676))
                   && (types(3).IsEqual(StepType(616)) || types(3).IsEqual(StepType(617))
                       || types(3).IsEqual(StepType(618)) || types(3).IsEqual(StepType(619))
                       || types(3).IsEqual(StepType(620)) || types(3).IsEqual(StepType(621))
                       || types(3).IsEqual(StepType(622)) || types(3).IsEqual(StepType(623))
                       || types(3).IsEqual(StepType(624)))))
      {
        return 696;
      }
      else if (types(1).IsEqual(StepType(470)) && types(2).IsEqual(StepType(630))
               && types(3).IsEqual(StepType(258)))
      {
        return 699;
      }
    }
    return 0;
  }
  return 0;
}

//=======================================================================
// function : IsComplex
// purpose  : External Mapping Recognition
//=======================================================================

Standard_Boolean RWStepAP214_ReadWriteModule::IsComplex(const Standard_Integer CN) const
{
  switch (CN)
  {
    case 319:
      return Standard_True;
    case 320:
      return Standard_True;
    case 321:
      return Standard_True;
    case 322:
      return Standard_True;
    case 323:
      return Standard_True;
    case 324:
      return Standard_True;
    case 325:
      return Standard_True;
    case 326:
      return Standard_True;
    case 327:
      return Standard_True;
    case 328:
      return Standard_True;
    case 329:
      return Standard_True;
    case 330:
      return Standard_True;
    case 331:
      return Standard_True;
    case 332:
      return Standard_True;
      // Added by FMA
    case 333:
      return Standard_True;
    case 334:
      return Standard_True;
    case 335:
      return Standard_True;
    case 337:
      return Standard_True;
    case 338:
      return Standard_True;
    case 344:
      return Standard_True;
    case 345:
      return Standard_True;
    case 346:
      return Standard_True;
    case 347:
      return Standard_True;
    case 357:
      return Standard_True;
    case 358: //: n5
      return Standard_True;
      //  AP214 CC1 -> CC2
    case 389:
      return Standard_True;
    case 409:
    case 410:
    case 411:
    case 412:
      return Standard_True;
    case 463:
      return Standard_True;
    case 481:
      return Standard_True;
    case 574:
      return Standard_True;
    case 578:
      return Standard_True;
    case 635:
      return Standard_True;
    case 636:
      return Standard_True;
    case 650:
      return Standard_True;
    case 691:
      return Standard_True;
    case 692:
      return Standard_True;
    case 693:
      return Standard_True;
    case 694:
      return Standard_True;
    case 695:
      return Standard_True;
    case 696:
      return Standard_True;
    case 697:
      return Standard_True;
    case 698:
      return Standard_True;
    case 699:
      return Standard_True;
    case 705:
      return Standard_True;
    case 706:
      return Standard_True;
    case 715:
      return Standard_True;
    case 719:
      return Standard_True;
    default:
      return Standard_False;
  }
}

//=================================================================================================

const TCollection_AsciiString& RWStepAP214_ReadWriteModule::StepType(
  const Standard_Integer CN) const
{
  const Reco_Container& aContainer = Reco_Container::Instance();
  switch (CN)
  {
    case 1:
      return aContainer.Reco_Address;
    case 2:
      return aContainer.Reco_AdvancedBrepShapeRepresentation;
    case 3:
      return aContainer.Reco_AdvancedFace;
    case 4:
      return aContainer.Reco_AnnotationCurveOccurrence;
    case 5:
      return aContainer.Reco_AnnotationFillArea;
    case 6:
      return aContainer.Reco_AnnotationFillAreaOccurrence;
    case 7:
      return aContainer.Reco_AnnotationOccurrence;
    case 8:
      return aContainer.Reco_AnnotationSubfigureOccurrence;
    case 9:
      return aContainer.Reco_AnnotationSymbol;
    case 10:
      return aContainer.Reco_AnnotationSymbolOccurrence;
    case 11:
      return aContainer.Reco_AnnotationText;
    case 12:
      return aContainer.Reco_AnnotationTextOccurrence;
    case 13:
      return aContainer.Reco_ApplicationContext;
    case 14:
      return aContainer.Reco_ApplicationContextElement;
    case 15:
      return aContainer.Reco_ApplicationProtocolDefinition;
    case 16:
      return aContainer.Reco_Approval;
    case 17:
      return aContainer.Reco_ApprovalAssignment;
    case 18:
      return aContainer.Reco_ApprovalPersonOrganization;
    case 19:
      return aContainer.Reco_ApprovalRelationship;
    case 20:
      return aContainer.Reco_ApprovalRole;
    case 21:
      return aContainer.Reco_ApprovalStatus;
    case 22:
      return aContainer.Reco_AreaInSet;
    case 23:
      return aContainer.Reco_AutoDesignActualDateAndTimeAssignment;
    case 24:
      return aContainer.Reco_AutoDesignActualDateAssignment;
    case 25:
      return aContainer.Reco_AutoDesignApprovalAssignment;
    case 26:
      return aContainer.Reco_AutoDesignDateAndPersonAssignment;
    case 27:
      return aContainer.Reco_AutoDesignGroupAssignment;
    case 28:
      return aContainer.Reco_AutoDesignNominalDateAndTimeAssignment;
    case 29:
      return aContainer.Reco_AutoDesignNominalDateAssignment;
    case 30:
      return aContainer.Reco_AutoDesignOrganizationAssignment;
    case 31:
      return aContainer.Reco_AutoDesignPersonAndOrganizationAssignment;
    case 32:
      return aContainer.Reco_AutoDesignPresentedItem;
    case 33:
      return aContainer.Reco_AutoDesignSecurityClassificationAssignment;
    case 34:
      return aContainer.Reco_AutoDesignViewArea;
    case 35:
      return aContainer.Reco_Axis1Placement;
    case 36:
      return aContainer.Reco_Axis2Placement2d;
    case 37:
      return aContainer.Reco_Axis2Placement3d;
    case 38:
      return aContainer.Reco_BSplineCurve;
    case 39:
      return aContainer.Reco_BSplineCurveWithKnots;
    case 40:
      return aContainer.Reco_BSplineSurface;
    case 41:
      return aContainer.Reco_BSplineSurfaceWithKnots;
    case 42:
      return aContainer.Reco_BackgroundColour;
    case 43:
      return aContainer.Reco_BezierCurve;
    case 44:
      return aContainer.Reco_BezierSurface;
    case 45:
      return aContainer.Reco_Block;
    case 46:
      return aContainer.Reco_BooleanResult;
    case 47:
      return aContainer.Reco_BoundaryCurve;
    case 48:
      return aContainer.Reco_BoundedCurve;
    case 49:
      return aContainer.Reco_BoundedSurface;
    case 50:
      return aContainer.Reco_BoxDomain;
    case 51:
      return aContainer.Reco_BoxedHalfSpace;
    case 52:
      return aContainer.Reco_BrepWithVoids;
    case 53:
      return aContainer.Reco_CalendarDate;
    case 54:
      return aContainer.Reco_CameraImage;
    case 55:
      return aContainer.Reco_CameraModel;
    case 56:
      return aContainer.Reco_CameraModelD2;
    case 57:
      return aContainer.Reco_CameraModelD3;
    case 58:
      return aContainer.Reco_CameraUsage;
    case 59:
      return aContainer.Reco_CartesianPoint;
    case 60:
      return aContainer.Reco_CartesianTransformationOperator;
    case 61:
      return aContainer.Reco_CartesianTransformationOperator3d;
    case 62:
      return aContainer.Reco_Circle;
    case 63:
      return aContainer.Reco_ClosedShell;
    case 64:
      return aContainer.Reco_Colour;
    case 65:
      return aContainer.Reco_ColourRgb;
    case 66:
      return aContainer.Reco_ColourSpecification;
    case 67:
      return aContainer.Reco_CompositeCurve;
    case 68:
      return aContainer.Reco_CompositeCurveOnSurface;
    case 69:
      return aContainer.Reco_CompositeCurveSegment;
    case 70:
      return aContainer.Reco_CompositeText;
    case 71:
      return aContainer.Reco_CompositeTextWithAssociatedCurves;
    case 72:
      return aContainer.Reco_CompositeTextWithBlankingBox;
    case 73:
      return aContainer.Reco_CompositeTextWithExtent;
    case 74:
      return aContainer.Reco_Conic;
    case 75:
      return aContainer.Reco_ConicalSurface;
    case 76:
      return aContainer.Reco_ConnectedFaceSet;
    case 77:
      return aContainer.Reco_ContextDependentInvisibility;
    case 78:
      return aContainer.Reco_ContextDependentOverRidingStyledItem;
    case 79:
      return aContainer.Reco_ConversionBasedUnit;
    case 80:
      return aContainer.Reco_CoordinatedUniversalTimeOffset;
    case 81:
      return aContainer.Reco_CsgRepresentation;
    case 82:
      return aContainer.Reco_CsgShapeRepresentation;
    case 83:
      return aContainer.Reco_CsgSolid;
    case 84:
      return aContainer.Reco_Curve;
    case 85:
      return aContainer.Reco_CurveBoundedSurface;
    case 86:
      return aContainer.Reco_CurveReplica;
    case 87:
      return aContainer.Reco_CurveStyle;
    case 88:
      return aContainer.Reco_CurveStyleFont;
    case 89:
      return aContainer.Reco_CurveStyleFontPattern;
    case 90:
      return aContainer.Reco_CylindricalSurface;
    case 91:
      return aContainer.Reco_Date;
    case 92:
      return aContainer.Reco_DateAndTime;
    case 93:
      return aContainer.Reco_DateAndTimeAssignment;
    case 94:
      return aContainer.Reco_DateAssignment;
    case 95:
      return aContainer.Reco_DateRole;
    case 96:
      return aContainer.Reco_DateTimeRole;
    case 97:
      return aContainer.Reco_DefinedSymbol;
    case 98:
      return aContainer.Reco_DefinitionalRepresentation;
    case 99:
      return aContainer.Reco_DegeneratePcurve;
    case 100:
      return aContainer.Reco_DegenerateToroidalSurface;
    case 101:
      return aContainer.Reco_DescriptiveRepresentationItem;
    case 102:
      return aContainer.Reco_DimensionCurve;
    case 103:
      return aContainer.Reco_DimensionCurveTerminator;
    case 104:
      return aContainer.Reco_DimensionalExponents;
    case 105:
      return aContainer.Reco_Direction;
    case 106:
      return aContainer.Reco_DraughtingAnnotationOccurrence;
    case 107:
      return aContainer.Reco_DraughtingCallout;
    case 108:
      return aContainer.Reco_DraughtingPreDefinedColour;
    case 109:
      return aContainer.Reco_DraughtingPreDefinedCurveFont;
    case 110:
      return aContainer.Reco_DraughtingSubfigureRepresentation;
    case 111:
      return aContainer.Reco_DraughtingSymbolRepresentation;
    case 112:
      return aContainer.Reco_DraughtingTextLiteralWithDelineation;
    case 113:
      return aContainer.Reco_DrawingDefinition;
    case 114:
      return aContainer.Reco_DrawingRevision;
    case 115:
      return aContainer.Reco_Edge;
    case 116:
      return aContainer.Reco_EdgeCurve;
    case 117:
      return aContainer.Reco_EdgeLoop;
    case 118:
      return aContainer.Reco_ElementarySurface;
    case 119:
      return aContainer.Reco_Ellipse;
    case 120:
      return aContainer.Reco_EvaluatedDegeneratePcurve;
    case 121:
      return aContainer.Reco_ExternalSource;
    case 122:
      return aContainer.Reco_ExternallyDefinedCurveFont;
    case 123:
      return aContainer.Reco_ExternallyDefinedHatchStyle;
    case 124:
      return aContainer.Reco_ExternallyDefinedItem;
    case 125:
      return aContainer.Reco_ExternallyDefinedSymbol;
    case 126:
      return aContainer.Reco_ExternallyDefinedTextFont;
    case 127:
      return aContainer.Reco_ExternallyDefinedTileStyle;
    case 128:
      return aContainer.Reco_ExtrudedAreaSolid;
    case 129:
      return aContainer.Reco_Face;
    case 131:
      return aContainer.Reco_FaceBound;
    case 132:
      return aContainer.Reco_FaceOuterBound;
    case 133:
      return aContainer.Reco_FaceSurface;
    case 134:
      return aContainer.Reco_FacetedBrep;
    case 135:
      return aContainer.Reco_FacetedBrepShapeRepresentation;
    case 136:
      return aContainer.Reco_FillAreaStyle;
    case 137:
      return aContainer.Reco_FillAreaStyleColour;
    case 138:
      return aContainer.Reco_FillAreaStyleHatching;
    case 139:
      return aContainer.Reco_FillAreaStyleTileSymbolWithStyle;
    case 140:
      return aContainer.Reco_FillAreaStyleTiles;
    case 141:
      return aContainer.Reco_FunctionallyDefinedTransformation;
    case 142:
      return aContainer.Reco_GeometricCurveSet;
    case 143:
      return aContainer.Reco_GeometricRepresentationContext;
    case 144:
      return aContainer.Reco_GeometricRepresentationItem;
    case 145:
      return aContainer.Reco_GeometricSet;
    case 146:
      return aContainer.Reco_GeometricallyBoundedSurfaceShapeRepresentation;
    case 147:
      return aContainer.Reco_GeometricallyBoundedWireframeShapeRepresentation;
    case 148:
      return aContainer.Reco_GlobalUncertaintyAssignedContext;
    case 149:
      return aContainer.Reco_GlobalUnitAssignedContext;
    case 150:
      return aContainer.Reco_Group;
    case 151:
      return aContainer.Reco_GroupAssignment;
    case 152:
      return aContainer.Reco_GroupRelationship;
    case 153:
      return aContainer.Reco_HalfSpaceSolid;
    case 154:
      return aContainer.Reco_Hyperbola;
    case 155:
      return aContainer.Reco_IntersectionCurve;
    case 156:
      return aContainer.Reco_Invisibility;
    case 157:
      return aContainer.Reco_LengthMeasureWithUnit;
    case 158:
      return aContainer.Reco_LengthUnit;
    case 159:
      return aContainer.Reco_Line;
    case 160:
      return aContainer.Reco_LocalTime;
    case 161:
      return aContainer.Reco_Loop;
    case 162:
      return aContainer.Reco_ManifoldSolidBrep;
    case 163:
      return aContainer.Reco_ManifoldSurfaceShapeRepresentation;
    case 164:
      return aContainer.Reco_MappedItem;
    case 165:
      return aContainer.Reco_MeasureWithUnit;
    case 166:
      return aContainer.Reco_MechanicalDesignGeometricPresentationArea;
    case 167:
      return aContainer.Reco_MechanicalDesignGeometricPresentationRepresentation;
    case 168:
      return aContainer.Reco_MechanicalDesignPresentationArea;
    case 169:
      return aContainer.Reco_NamedUnit;
    case 171:
      return aContainer.Reco_OffsetCurve3d;
    case 172:
      return aContainer.Reco_OffsetSurface;
    case 173:
      return aContainer.Reco_OneDirectionRepeatFactor;
    case 174:
      return aContainer.Reco_OpenShell;
    case 175:
      return aContainer.Reco_OrdinalDate;
    case 176:
      return aContainer.Reco_Organization;
    case 177:
      return aContainer.Reco_OrganizationAssignment;
    case 178:
      return aContainer.Reco_OrganizationRole;
    case 179:
      return aContainer.Reco_OrganizationalAddress;
    case 180:
      return aContainer.Reco_OrientedClosedShell;
    case 181:
      return aContainer.Reco_OrientedEdge;
    case 182:
      return aContainer.Reco_OrientedFace;
    case 183:
      return aContainer.Reco_OrientedOpenShell;
    case 184:
      return aContainer.Reco_OrientedPath;
    case 185:
      return aContainer.Reco_OuterBoundaryCurve;
    case 186:
      return aContainer.Reco_OverRidingStyledItem;
    case 187:
      return aContainer.Reco_Parabola;
    case 188:
      return aContainer.Reco_ParametricRepresentationContext;
    case 189:
      return aContainer.Reco_Path;
    case 190:
      return aContainer.Reco_Pcurve;
    case 191:
      return aContainer.Reco_Person;
    case 192:
      return aContainer.Reco_PersonAndOrganization;
    case 193:
      return aContainer.Reco_PersonAndOrganizationAssignment;
    case 194:
      return aContainer.Reco_PersonAndOrganizationRole;
    case 195:
      return aContainer.Reco_PersonalAddress;
    case 196:
      return aContainer.Reco_Placement;
    case 197:
      return aContainer.Reco_PlanarBox;
    case 198:
      return aContainer.Reco_PlanarExtent;
    case 199:
      return aContainer.Reco_Plane;
    case 200:
      return aContainer.Reco_PlaneAngleMeasureWithUnit;
    case 201:
      return aContainer.Reco_PlaneAngleUnit;
    case 202:
      return aContainer.Reco_Point;
    case 203:
      return aContainer.Reco_PointOnCurve;
    case 204:
      return aContainer.Reco_PointOnSurface;
    case 205:
      return aContainer.Reco_PointReplica;
    case 206:
      return aContainer.Reco_PointStyle;
    case 207:
      return aContainer.Reco_PolyLoop;
    case 208:
      return aContainer.Reco_Polyline;
    case 209:
      return aContainer.Reco_PreDefinedColour;
    case 210:
      return aContainer.Reco_PreDefinedCurveFont;
    case 211:
      return aContainer.Reco_PreDefinedItem;
    case 212:
      return aContainer.Reco_PreDefinedSymbol;
    case 213:
      return aContainer.Reco_PreDefinedTextFont;
    case 214:
      return aContainer.Reco_PresentationArea;
    case 215:
      return aContainer.Reco_PresentationLayerAssignment;
    case 216:
      return aContainer.Reco_PresentationRepresentation;
    case 217:
      return aContainer.Reco_PresentationSet;
    case 218:
      return aContainer.Reco_PresentationSize;
    case 219:
      return aContainer.Reco_PresentationStyleAssignment;
    case 220:
      return aContainer.Reco_PresentationStyleByContext;
    case 221:
      return aContainer.Reco_PresentationView;
    case 222:
      return aContainer.Reco_PresentedItem;
    case 223:
      return aContainer.Reco_Product;
    case 224:
      return aContainer.Reco_ProductCategory;
    case 225:
      return aContainer.Reco_ProductContext;
    case 226:
      return aContainer.Reco_ProductDataRepresentationView;
    case 227:
      return aContainer.Reco_ProductDefinition;
    case 228:
      return aContainer.Reco_ProductDefinitionContext;
    case 229:
      return aContainer.Reco_ProductDefinitionFormation;
    case 230:
      return aContainer.Reco_ProductDefinitionFormationWithSpecifiedSource;
    case 231:
      return aContainer.Reco_ProductDefinitionShape;
    case 232:
      return aContainer.Reco_ProductRelatedProductCategory;
    case 233:
      return aContainer.Reco_ProductType;
    case 234:
      return aContainer.Reco_PropertyDefinition;
    case 235:
      return aContainer.Reco_PropertyDefinitionRepresentation;
    case 236:
      return aContainer.Reco_QuasiUniformCurve;
    case 237:
      return aContainer.Reco_QuasiUniformSurface;
    case 238:
      return aContainer.Reco_RatioMeasureWithUnit;
    case 239:
      return aContainer.Reco_RationalBSplineCurve;
    case 240:
      return aContainer.Reco_RationalBSplineSurface;
    case 241:
      return aContainer.Reco_RectangularCompositeSurface;
    case 242:
      return aContainer.Reco_RectangularTrimmedSurface;
    case 243:
      return aContainer.Reco_RepItemGroup;
    case 244:
      return aContainer.Reco_ReparametrisedCompositeCurveSegment;
    case 245:
      return aContainer.Reco_Representation;
    case 246:
      return aContainer.Reco_RepresentationContext;
    case 247:
      return aContainer.Reco_RepresentationItem;
    case 248:
      return aContainer.Reco_RepresentationMap;
    case 249:
      return aContainer.Reco_RepresentationRelationship;
    case 250:
      return aContainer.Reco_RevolvedAreaSolid;
    case 251:
      return aContainer.Reco_RightAngularWedge;
    case 252:
      return aContainer.Reco_RightCircularCone;
    case 253:
      return aContainer.Reco_RightCircularCylinder;
    case 254:
      return aContainer.Reco_SeamCurve;
    case 255:
      return aContainer.Reco_SecurityClassification;
    case 256:
      return aContainer.Reco_SecurityClassificationAssignment;
    case 257:
      return aContainer.Reco_SecurityClassificationLevel;
    case 258:
      return aContainer.Reco_ShapeAspect;
    case 259:
      return aContainer.Reco_ShapeAspectRelationship;
    case 260:
      return aContainer.Reco_ShapeAspectTransition;
    case 261:
      return aContainer.Reco_ShapeDefinitionRepresentation;
    case 262:
      return aContainer.Reco_ShapeRepresentation;
    case 263:
      return aContainer.Reco_ShellBasedSurfaceModel;
    case 264:
      return aContainer.Reco_SiUnit;
    case 265:
      return aContainer.Reco_SolidAngleMeasureWithUnit;
    case 266:
      return aContainer.Reco_SolidModel;
    case 267:
      return aContainer.Reco_SolidReplica;
    case 268:
      return aContainer.Reco_Sphere;
    case 269:
      return aContainer.Reco_SphericalSurface;
    case 270:
      return aContainer.Reco_StyledItem;
    case 271:
      return aContainer.Reco_Surface;
    case 272:
      return aContainer.Reco_SurfaceCurve;
    case 273:
      return aContainer.Reco_SurfaceOfLinearExtrusion;
    case 274:
      return aContainer.Reco_SurfaceOfRevolution;
    case 275:
      return aContainer.Reco_SurfacePatch;
    case 276:
      return aContainer.Reco_SurfaceReplica;
    case 277:
      return aContainer.Reco_SurfaceSideStyle;
    case 278:
      return aContainer.Reco_SurfaceStyleBoundary;
    case 279:
      return aContainer.Reco_SurfaceStyleControlGrid;
    case 280:
      return aContainer.Reco_SurfaceStyleFillArea;
    case 281:
      return aContainer.Reco_SurfaceStyleParameterLine;
    case 282:
      return aContainer.Reco_SurfaceStyleSegmentationCurve;
    case 283:
      return aContainer.Reco_SurfaceStyleSilhouette;
    case 284:
      return aContainer.Reco_SurfaceStyleUsage;
    case 285:
      return aContainer.Reco_SweptAreaSolid;
    case 286:
      return aContainer.Reco_SweptSurface;
    case 287:
      return aContainer.Reco_SymbolColour;
    case 288:
      return aContainer.Reco_SymbolRepresentation;
    case 289:
      return aContainer.Reco_SymbolRepresentationMap;
    case 290:
      return aContainer.Reco_SymbolStyle;
    case 291:
      return aContainer.Reco_SymbolTarget;
    case 292:
      return aContainer.Reco_Template;
    case 293:
      return aContainer.Reco_TemplateInstance;
    case 294:
      return aContainer.Reco_TerminatorSymbol;
    case 295:
      return aContainer.Reco_TextLiteral;
    case 296:
      return aContainer.Reco_TextLiteralWithAssociatedCurves;
    case 297:
      return aContainer.Reco_TextLiteralWithBlankingBox;
    case 298:
      return aContainer.Reco_TextLiteralWithDelineation;
    case 299:
      return aContainer.Reco_TextLiteralWithExtent;
    case 300:
      return aContainer.Reco_TextStyle;
    case 301:
      return aContainer.Reco_TextStyleForDefinedFont;
    case 302:
      return aContainer.Reco_TextStyleWithBoxCharacteristics;
    case 303:
      return aContainer.Reco_TextStyleWithMirror;
    case 304:
      return aContainer.Reco_TopologicalRepresentationItem;
    case 305:
      return aContainer.Reco_ToroidalSurface;
    case 306:
      return aContainer.Reco_Torus;
    case 307:
      return aContainer.Reco_TransitionalShapeRepresentation;
    case 308:
      return aContainer.Reco_TrimmedCurve;
    case 309:
      return aContainer.Reco_TwoDirectionRepeatFactor;
    case 310:
      return aContainer.Reco_UncertaintyMeasureWithUnit;
    case 311:
      return aContainer.Reco_UniformCurve;
    case 312:
      return aContainer.Reco_UniformSurface;
    case 313:
      return aContainer.Reco_Vector;
    case 314:
      return aContainer.Reco_Vertex;
    case 315:
      return aContainer.Reco_VertexLoop;
    case 316:
      return aContainer.Reco_VertexPoint;
    case 317:
      return aContainer.Reco_ViewVolume;
    case 318:
      return aContainer.Reco_WeekOfYearAndDayDate;
      // Added by FMA
    case 336:
      return aContainer.Reco_SolidAngleUnit;
    case 339:
      return aContainer.Reco_MechanicalContext;
    case 340:
      return aContainer.Reco_DesignContext;
      // Added for full Rev4
    case 341:
      return aContainer.Reco_TimeMeasureWithUnit;
    case 342:
      return aContainer.Reco_RatioUnit;
    case 343:
      return aContainer.Reco_TimeUnit;
    case 348:
      return aContainer.Reco_ApprovalDateTime;
    case 349:
      return aContainer.Reco_CameraImage2dWithScale;
    case 350:
      return aContainer.Reco_CameraImage3dWithScale;
    case 351:
      return aContainer.Reco_CartesianTransformationOperator2d;
    case 352:
      return aContainer.Reco_DerivedUnit;
    case 353:
      return aContainer.Reco_DerivedUnitElement;
    case 354:
      return aContainer.Reco_ItemDefinedTransformation;
    case 355:
      return aContainer.Reco_PresentedItemRepresentation;
    case 356:
      return aContainer.Reco_PresentationLayerUsage;

      //  AP214 : CC1 -> CC2

    case 366:
      return aContainer.Reco_AutoDesignDocumentReference;
    case 367:
      return aContainer.Reco_Document;
    case 368:
      return aContainer.Reco_DigitalDocument;
    case 369:
      return aContainer.Reco_DocumentRelationship;
    case 370:
      return aContainer.Reco_DocumentType;
    case 371:
      return aContainer.Reco_DocumentUsageConstraint;
    case 372:
      return aContainer.Reco_Effectivity;
    case 373:
      return aContainer.Reco_ProductDefinitionEffectivity;
    case 374:
      return aContainer.Reco_ProductDefinitionRelationship;

    case 375:
      return aContainer.Reco_ProductDefinitionWithAssociatedDocuments;
    case 376:
      return aContainer.Reco_PhysicallyModeledProductDefinition;

    case 377:
      return aContainer.Reco_ProductDefinitionUsage;
    case 378:
      return aContainer.Reco_MakeFromUsageOption;
    case 379:
      return aContainer.Reco_AssemblyComponentUsage;
    case 380:
      return aContainer.Reco_NextAssemblyUsageOccurrence;
    case 381:
      return aContainer.Reco_PromissoryUsageOccurrence;
    case 382:
      return aContainer.Reco_QuantifiedAssemblyComponentUsage;
    case 383:
      return aContainer.Reco_SpecifiedHigherUsageOccurrence;
    case 384:
      return aContainer.Reco_AssemblyComponentUsageSubstitute;
    case 385:
      return aContainer.Reco_SuppliedPartRelationship;
    case 386:
      return aContainer.Reco_ExternallyDefinedRepresentation;
    case 387:
      return aContainer.Reco_ShapeRepresentationRelationship;
    case 388:
      return aContainer.Reco_RepresentationRelationshipWithTransformation;

    case 390:
      return aContainer.Reco_MaterialDesignation;
    case 391:
      return aContainer.Reco_ContextDependentShapeRepresentation;
    //: S4134: Added from CD to DIS
    case 392:
      return aContainer.Reco_AppliedDateAndTimeAssignment;
    case 393:
      return aContainer.Reco_AppliedDateAssignment;
    case 394:
      return aContainer.Reco_AppliedApprovalAssignment;
    case 395:
      return aContainer.Reco_AppliedGroupAssignment;
    case 396:
      return aContainer.Reco_AppliedOrganizationAssignment;
    case 397:
      return aContainer.Reco_AppliedPersonAndOrganizationAssignment;
    case 398:
      return aContainer.Reco_AppliedPresentedItem;
    case 399:
      return aContainer.Reco_AppliedSecurityClassificationAssignment;
    case 400:
      return aContainer.Reco_AppliedDocumentReference;
    case 401:
      return aContainer.Reco_DocumentFile;
    case 402:
      return aContainer.Reco_CharacterizedObject;
    case 403:
      return aContainer.Reco_ExtrudedFaceSolid;
    case 404:
      return aContainer.Reco_RevolvedFaceSolid;
    case 405:
      return aContainer.Reco_SweptFaceSolid;

    // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
    case 406:
      return aContainer.Reco_MeasureRepresentationItem;
    case 407:
      return aContainer.Reco_AreaUnit;
    case 408:
      return aContainer.Reco_VolumeUnit;

    // Added by ABV 10.11.99 for AP203
    case 413:
      return aContainer.Reco_Action;
    case 414:
      return aContainer.Reco_ActionAssignment;
    case 415:
      return aContainer.Reco_ActionMethod;
    case 416:
      return aContainer.Reco_ActionRequestAssignment;
    case 417:
      return aContainer.Reco_CcDesignApproval;
    case 418:
      return aContainer.Reco_CcDesignCertification;
    case 419:
      return aContainer.Reco_CcDesignContract;
    case 420:
      return aContainer.Reco_CcDesignDateAndTimeAssignment;
    case 421:
      return aContainer.Reco_CcDesignPersonAndOrganizationAssignment;
    case 422:
      return aContainer.Reco_CcDesignSecurityClassification;
    case 423:
      return aContainer.Reco_CcDesignSpecificationReference;
    case 424:
      return aContainer.Reco_Certification;
    case 425:
      return aContainer.Reco_CertificationAssignment;
    case 426:
      return aContainer.Reco_CertificationType;
    case 427:
      return aContainer.Reco_Change;
    case 428:
      return aContainer.Reco_ChangeRequest;
    case 429:
      return aContainer.Reco_ConfigurationDesign;
    case 430:
      return aContainer.Reco_ConfigurationEffectivity;
    case 431:
      return aContainer.Reco_Contract;
    case 432:
      return aContainer.Reco_ContractAssignment;
    case 433:
      return aContainer.Reco_ContractType;
    case 434:
      return aContainer.Reco_ProductConcept;
    case 435:
      return aContainer.Reco_ProductConceptContext;
    case 436:
      return aContainer.Reco_StartRequest;
    case 437:
      return aContainer.Reco_StartWork;
    case 438:
      return aContainer.Reco_VersionedActionRequest;
    case 439:
      return aContainer.Reco_ProductCategoryRelationship;
    case 440:
      return aContainer.Reco_ActionRequestSolution;
    case 441:
      return aContainer.Reco_DraughtingModel;

    // Added by ABV 18.04.00 for CAX-IF TRJ4
    case 442:
      return aContainer.Reco_AngularLocation;
    case 443:
      return aContainer.Reco_AngularSize;
    case 444:
      return aContainer.Reco_DimensionalCharacteristicRepresentation;
    case 445:
      return aContainer.Reco_DimensionalLocation;
    case 446:
      return aContainer.Reco_DimensionalLocationWithPath;
    case 447:
      return aContainer.Reco_DimensionalSize;
    case 448:
      return aContainer.Reco_DimensionalSizeWithPath;
    case 449:
      return aContainer.Reco_ShapeDimensionRepresentation;

    // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
    case 450:
      return aContainer.Reco_DocumentRepresentationType;
    case 451:
      return aContainer.Reco_ObjectRole;
    case 452:
      return aContainer.Reco_RoleAssociation;
    case 453:
      return aContainer.Reco_IdentificationRole;
    case 454:
      return aContainer.Reco_IdentificationAssignment;
    case 455:
      return aContainer.Reco_ExternalIdentificationAssignment;
    case 456:
      return aContainer.Reco_EffectivityAssignment;
    case 457:
      return aContainer.Reco_NameAssignment;
    case 458:
      return aContainer.Reco_GeneralProperty;
    case 459:
      return aContainer.Reco_Class;
    case 460:
      return aContainer.Reco_ExternallyDefinedClass;
    case 461:
      return aContainer.Reco_ExternallyDefinedGeneralProperty;
    case 462:
      return aContainer.Reco_AppliedExternalIdentificationAssignment;

    // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dim.tol.)
    case 470:
      return aContainer.Reco_CompositeShapeAspect;
    case 471:
      return aContainer.Reco_DerivedShapeAspect;
    case 472:
      return aContainer.Reco_Extension;
    case 473:
      return aContainer.Reco_DirectedDimensionalLocation;
    case 474:
      return aContainer.Reco_LimitsAndFits;
    case 475:
      return aContainer.Reco_ToleranceValue;
    case 476:
      return aContainer.Reco_MeasureQualification;
    case 477:
      return aContainer.Reco_PlusMinusTolerance;
    case 478:
      return aContainer.Reco_PrecisionQualifier;
    case 479:
      return aContainer.Reco_TypeQualifier;
    case 480:
      return aContainer.Reco_QualifiedRepresentationItem;

    case 482:
      return aContainer.Reco_CompoundRepresentationItem;
    case 483:
      return aContainer.Reco_ValueRange;
    case 484:
      return aContainer.Reco_ShapeAspectDerivingRelationship;

    case 485:
      return aContainer.Reco_CompoundShapeRepresentation;
    case 486:
      return aContainer.Reco_ConnectedEdgeSet;
    case 487:
      return aContainer.Reco_ConnectedFaceShapeRepresentation;
    case 488:
      return aContainer.Reco_EdgeBasedWireframeModel;
    case 489:
      return aContainer.Reco_EdgeBasedWireframeShapeRepresentation;
    case 490:
      return aContainer.Reco_FaceBasedSurfaceModel;
    case 491:
      return aContainer.Reco_NonManifoldSurfaceShapeRepresentation;

    // gka 08.01.02
    case 492:
      return aContainer.Reco_OrientedSurface;
    case 493:
      return aContainer.Reco_Subface;
    case 494:
      return aContainer.Reco_Subedge;
    case 495:
      return aContainer.Reco_SeamEdge;
    case 496:
      return aContainer.Reco_ConnectedFaceSubSet;

    // AP209
    case 500:
      return aContainer.Reco_EulerAngles;
    case 501:
      return aContainer.Reco_MassUnit;
    case 502:
      return aContainer.Reco_ThermodynamicTemperatureUnit;
    case 503:
      return aContainer.Reco_AnalysisItemWithinRepresentation;
    case 504:
      return aContainer.Reco_Curve3dElementDescriptor;
    case 505:
      return aContainer.Reco_CurveElementEndReleasePacket;
    case 506:
      return aContainer.Reco_CurveElementSectionDefinition;
    case 507:
      return aContainer.Reco_CurveElementSectionDerivedDefinitions;
    case 508:
      return aContainer.Reco_ElementDescriptor;
    case 509:
      return aContainer.Reco_ElementMaterial;
    case 510:
      return aContainer.Reco_Surface3dElementDescriptor;
    case 511:
      return aContainer.Reco_SurfaceElementProperty;
    case 512:
      return aContainer.Reco_SurfaceSection;
    case 513:
      return aContainer.Reco_SurfaceSectionField;
    case 514:
      return aContainer.Reco_SurfaceSectionFieldConstant;
    case 515:
      return aContainer.Reco_SurfaceSectionFieldVarying;
    case 516:
      return aContainer.Reco_UniformSurfaceSection;
    case 517:
      return aContainer.Reco_Volume3dElementDescriptor;
    case 518:
      return aContainer.Reco_AlignedCurve3dElementCoordinateSystem;
    case 519:
      return aContainer.Reco_ArbitraryVolume3dElementCoordinateSystem;
    case 520:
      return aContainer.Reco_Curve3dElementProperty;
    case 521:
      return aContainer.Reco_Curve3dElementRepresentation;
    case 522:
      return aContainer.Reco_Node;
      //  case 523: return aContainer.Reco_CurveElementEndCoordinateSystem;
    case 524:
      return aContainer.Reco_CurveElementEndOffset;
    case 525:
      return aContainer.Reco_CurveElementEndRelease;
    case 526:
      return aContainer.Reco_CurveElementInterval;
    case 527:
      return aContainer.Reco_CurveElementIntervalConstant;
    case 528:
      return aContainer.Reco_DummyNode;
    case 529:
      return aContainer.Reco_CurveElementLocation;
    case 530:
      return aContainer.Reco_ElementGeometricRelationship;
    case 531:
      return aContainer.Reco_ElementGroup;
    case 532:
      return aContainer.Reco_ElementRepresentation;
    case 533:
      return aContainer.Reco_FeaAreaDensity;
    case 534:
      return aContainer.Reco_FeaAxis2Placement3d;
    case 535:
      return aContainer.Reco_FeaGroup;
    case 536:
      return aContainer.Reco_FeaLinearElasticity;
    case 537:
      return aContainer.Reco_FeaMassDensity;
    case 538:
      return aContainer.Reco_FeaMaterialPropertyRepresentation;
    case 539:
      return aContainer.Reco_FeaMaterialPropertyRepresentationItem;
    case 540:
      return aContainer.Reco_FeaModel;
    case 541:
      return aContainer.Reco_FeaModel3d;
    case 542:
      return aContainer.Reco_FeaMoistureAbsorption;
    case 543:
      return aContainer.Reco_FeaParametricPoint;
    case 544:
      return aContainer.Reco_FeaRepresentationItem;
    case 545:
      return aContainer.Reco_FeaSecantCoefficientOfLinearThermalExpansion;
    case 546:
      return aContainer.Reco_FeaShellBendingStiffness;
    case 547:
      return aContainer.Reco_FeaShellMembraneBendingCouplingStiffness;
    case 548:
      return aContainer.Reco_FeaShellMembraneStiffness;
    case 549:
      return aContainer.Reco_FeaShellShearStiffness;
    case 550:
      return aContainer.Reco_GeometricNode;
    case 551:
      return aContainer.Reco_FeaTangentialCoefficientOfLinearThermalExpansion;
    case 552:
      return aContainer.Reco_NodeGroup;
    case 553:
      return aContainer.Reco_NodeRepresentation;
    case 554:
      return aContainer.Reco_NodeSet;
    case 555:
      return aContainer.Reco_NodeWithSolutionCoordinateSystem;
    case 556:
      return aContainer.Reco_NodeWithVector;
    case 557:
      return aContainer.Reco_ParametricCurve3dElementCoordinateDirection;
    case 558:
      return aContainer.Reco_ParametricCurve3dElementCoordinateSystem;
    case 559:
      return aContainer.Reco_ParametricSurface3dElementCoordinateSystem;
    case 560:
      return aContainer.Reco_Surface3dElementRepresentation;
      //  case 561: return aContainer.Reco_SymmetricTensor22d;
      //  case 562: return aContainer.Reco_SymmetricTensor42d;
      //  case 563: return aContainer.Reco_SymmetricTensor43d;
    case 564:
      return aContainer.Reco_Volume3dElementRepresentation;
    case 565:
      return aContainer.Reco_DataEnvironment;
    case 566:
      return aContainer.Reco_MaterialPropertyRepresentation;
    case 567:
      return aContainer.Reco_PropertyDefinitionRelationship;
    case 568:
      return aContainer.Reco_PointRepresentation;
    case 569:
      return aContainer.Reco_MaterialProperty;
    case 570:
      return aContainer.Reco_FeaModelDefinition;
    case 571:
      return aContainer.Reco_FreedomAndCoefficient;
    case 572:
      return aContainer.Reco_FreedomsList;
    case 573:
      return aContainer.Reco_ProductDefinitionFormationRelationship;
      //  case 574: return aContainer.Reco_FeaModelDefinition;
    case 575:
      return aContainer.Reco_NodeDefinition;
    case 576:
      return aContainer.Reco_StructuralResponseProperty;
    case 577:
      return aContainer.Reco_StructuralResponsePropertyDefinitionRepresentation;
    case 579:
      return aContainer.Reco_AlignedSurface3dElementCoordinateSystem;
    case 580:
      return aContainer.Reco_ConstantSurface3dElementCoordinateSystem;
    case 581:
      return aContainer.Reco_CurveElementIntervalLinearlyVarying;
    case 582:
      return aContainer.Reco_FeaCurveSectionGeometricRelationship;
    case 583:
      return aContainer.Reco_FeaSurfaceSectionGeometricRelationship;

    // ptv 28.01.2003
    case 600:
      return aContainer.Reco_DocumentProductAssociation;
    case 601:
      return aContainer.Reco_DocumentProductEquivalence;

    // Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
    case 609:
      return aContainer.Reco_CylindricityTolerance;
    case 610:
      return aContainer.Reco_ShapeRepresentationWithParameters;
    case 611:
      return aContainer.Reco_AngularityTolerance;
    case 612:
      return aContainer.Reco_ConcentricityTolerance;
    case 613:
      return aContainer.Reco_CircularRunoutTolerance;
    case 614:
      return aContainer.Reco_CoaxialityTolerance;
    case 615:
      return aContainer.Reco_FlatnessTolerance;
    case 616:
      return aContainer.Reco_LineProfileTolerance;
    case 617:
      return aContainer.Reco_ParallelismTolerance;
    case 618:
      return aContainer.Reco_PerpendicularityTolerance;
    case 619:
      return aContainer.Reco_PositionTolerance;
    case 620:
      return aContainer.Reco_RoundnessTolerance;
    case 621:
      return aContainer.Reco_StraightnessTolerance;
    case 622:
      return aContainer.Reco_SurfaceProfileTolerance;
    case 623:
      return aContainer.Reco_SymmetryTolerance;
    case 624:
      return aContainer.Reco_TotalRunoutTolerance;
    case 625:
      return aContainer.Reco_GeometricTolerance;
    case 626:
      return aContainer.Reco_GeometricToleranceRelationship;
    case 627:
      return aContainer.Reco_GeometricToleranceWithDatumReference;
    case 628:
      return aContainer.Reco_ModifiedGeometricTolerance;
    case 629:
      return aContainer.Reco_Datum;
    case 630:
      return aContainer.Reco_DatumFeature;
    case 631:
      return aContainer.Reco_DatumReference;
    case 632:
      return aContainer.Reco_CommonDatum;
    case 633:
      return aContainer.Reco_DatumTarget;
    case 634:
      return aContainer.Reco_PlacedDatumTargetFeature;

    case 651:
      return aContainer.Reco_MassMeasureWithUnit;

    // Added by ika for GD&T AP242
    case 660:
      return aContainer.Reco_Apex;
    case 661:
      return aContainer.Reco_CentreOfSymmetry;
    case 662:
      return aContainer.Reco_GeometricAlignment;
    case 663:
      return aContainer.Reco_PerpendicularTo;
    case 664:
      return aContainer.Reco_Tangent;
    case 665:
      return aContainer.Reco_ParallelOffset;
    case 666:
      return aContainer.Reco_GeometricItemSpecificUsage;
    case 667:
      return aContainer.Reco_IdAttribute;
    case 668:
      return aContainer.Reco_ItemIdentifiedRepresentationUsage;
    case 669:
      return aContainer.Reco_AllAroundShapeAspect;
    case 670:
      return aContainer.Reco_BetweenShapeAspect;
    case 671:
      return aContainer.Reco_CompositeGroupShapeAspect;
    case 672:
      return aContainer.Reco_ContinuosShapeAspect;
    case 673:
      return aContainer.Reco_GeometricToleranceWithDefinedAreaUnit;
    case 674:
      return aContainer.Reco_GeometricToleranceWithDefinedUnit;
    case 675:
      return aContainer.Reco_GeometricToleranceWithMaximumTolerance;
    case 676:
      return aContainer.Reco_GeometricToleranceWithModifiers;
    case 677:
      return aContainer.Reco_UnequallyDisposedGeometricTolerance;
    case 678:
      return aContainer.Reco_NonUniformZoneDefinition;
    case 679:
      return aContainer.Reco_ProjectedZoneDefinition;
    case 680:
      return aContainer.Reco_RunoutZoneDefinition;
    case 681:
      return aContainer.Reco_RunoutZoneOrientation;
    case 682:
      return aContainer.Reco_ToleranceZone;
    case 683:
      return aContainer.Reco_ToleranceZoneDefinition;
    case 684:
      return aContainer.Reco_ToleranceZoneForm;
    case 685:
      return aContainer.Reco_ValueFormatTypeQualifier;
    case 686:
      return aContainer.Reco_DatumReferenceCompartment;
    case 687:
      return aContainer.Reco_DatumReferenceElement;
    case 688:
      return aContainer.Reco_DatumReferenceModifierWithValue;
    case 689:
      return aContainer.Reco_DatumSystem;
    case 690:
      return aContainer.Reco_GeneralDatumReference;
    case 700:
      return aContainer.Reco_IntegerRepresentationItem;
    case 701:
      return aContainer.Reco_ValueRepresentationItem;
    case 702:
      return aContainer.Reco_FeatureForDatumTargetRelationship;
    case 703:
      return aContainer.Reco_DraughtingModelItemAssociation;
    case 704:
      return aContainer.Reco_AnnotationPlane;

    case 707:
      return aContainer.Reco_TessellatedAnnotationOccurrence;
    case 708:
      return aContainer.Reco_TessellatedItem;
    case 709:
      return aContainer.Reco_TessellatedGeometricSet;

    case 710:
      return aContainer.Reco_TessellatedCurveSet;
    case 711:
      return aContainer.Reco_CoordinatesList;
    case 712:
      return aContainer.Reco_ConstructiveGeometryRepresentation;
    case 713:
      return aContainer.Reco_ConstructiveGeometryRepresentationRelationship;
    case 714:
      return aContainer.Reco_CharacterizedRepresentation;
    case 716:
      return aContainer.Reco_CameraModelD3MultiClipping;
    case 717:
      return aContainer.Reco_CameraModelD3MultiClippingIntersection;
    case 718:
      return aContainer.Reco_CameraModelD3MultiClippingUnion;

    case 720:
      return aContainer.Reco_SurfaceStyleTransparent;
    case 721:
      return aContainer.Reco_SurfaceStyleReflectanceAmbient;
    case 722:
      return aContainer.Reco_SurfaceStyleRendering;
    case 723:
      return aContainer.Reco_SurfaceStyleRenderingWithProperties;

    case 724:
      return aContainer.Reco_RepresentationContextReference;
    case 725:
      return aContainer.Reco_RepresentationReference;
    case 726:
      return aContainer.Reco_SuParameters;
    case 727:
      return aContainer.Reco_RotationAboutDirection;
    case 728:
      return aContainer.Reco_KinematicJoint;
    case 729:
      return aContainer.Reco_ActuatedKinematicPair;
    case 730:
      return aContainer.Reco_ContextDependentKinematicLinkRepresentation;
    case 731:
      return aContainer.Reco_CylindricalPair;
    case 732:
      return aContainer.Reco_CylindricalPairValue;
    case 733:
      return aContainer.Reco_CylindricalPairWithRange;
    case 734:
      return aContainer.Reco_FullyConstrainedPair;
    case 735:
      return aContainer.Reco_GearPair;
    case 736:
      return aContainer.Reco_GearPairValue;
    case 737:
      return aContainer.Reco_GearPairWithRange;
    case 738:
      return aContainer.Reco_HomokineticPair;
    case 739:
      return aContainer.Reco_KinematicLink;
    case 740:
      return aContainer.Reco_KinematicLinkRepresentationAssociation;
    case 741:
      return aContainer.Reco_KinematicPropertyMechanismRepresentation;
    case 742:
      return aContainer.Reco_KinematicTopologyStructure;
    case 743:
      return aContainer.Reco_LowOrderKinematicPair;
    case 744:
      return aContainer.Reco_LowOrderKinematicPairValue;
    case 745:
      return aContainer.Reco_LowOrderKinematicPairWithRange;
    case 746:
      return aContainer.Reco_MechanismRepresentation;
    case 747:
      return aContainer.Reco_OrientedJoint;
    case 748:
      return aContainer.Reco_PlanarCurvePair;
    case 749:
      return aContainer.Reco_PlanarCurvePairRange;
    case 750:
      return aContainer.Reco_PlanarPair;
    case 751:
      return aContainer.Reco_PlanarPairValue;
    case 752:
      return aContainer.Reco_PlanarPairWithRange;
    case 753:
      return aContainer.Reco_PointOnPlanarCurvePair;
    case 754:
      return aContainer.Reco_PointOnPlanarCurvePairValue;
    case 755:
      return aContainer.Reco_PointOnPlanarCurvePairWithRange;
    case 756:
      return aContainer.Reco_PointOnSurfacePair;
    case 757:
      return aContainer.Reco_PointOnSurfacePairValue;
    case 758:
      return aContainer.Reco_PointOnSurfacePairWithRange;
    case 759:
      return aContainer.Reco_PrismaticPair;
    case 760:
      return aContainer.Reco_PrismaticPairValue;
    case 761:
      return aContainer.Reco_PrismaticPairWithRange;
    case 762:
      return aContainer.Reco_ProductDefinitionKinematics;
    case 763:
      return aContainer.Reco_ProductDefinitionRelationshipKinematics;
    case 764:
      return aContainer.Reco_RackAndPinionPair;
    case 765:
      return aContainer.Reco_RackAndPinionPairValue;
    case 766:
      return aContainer.Reco_RackAndPinionPairWithRange;
    case 767:
      return aContainer.Reco_RevolutePair;
    case 768:
      return aContainer.Reco_RevolutePairValue;
    case 769:
      return aContainer.Reco_RevolutePairWithRange;
    case 770:
      return aContainer.Reco_RollingCurvePair;
    case 771:
      return aContainer.Reco_RollingCurvePairValue;
    case 772:
      return aContainer.Reco_RollingSurfacePair;
    case 773:
      return aContainer.Reco_RollingSurfacePairValue;
    case 774:
      return aContainer.Reco_ScrewPair;
    case 775:
      return aContainer.Reco_ScrewPairValue;
    case 776:
      return aContainer.Reco_ScrewPairWithRange;
    case 777:
      return aContainer.Reco_SlidingCurvePair;
    case 778:
      return aContainer.Reco_SlidingCurvePairValue;
    case 779:
      return aContainer.Reco_SlidingSurfacePair;
    case 780:
      return aContainer.Reco_SlidingSurfacePairValue;
    case 781:
      return aContainer.Reco_SphericalPair;
    case 782:
      return aContainer.Reco_SphericalPairValue;
    case 783:
      return aContainer.Reco_SphericalPairWithPin;
    case 784:
      return aContainer.Reco_SphericalPairWithPinAndRange;
    case 785:
      return aContainer.Reco_SphericalPairWithRange;
    case 786:
      return aContainer.Reco_SurfacePairWithRange;
    case 787:
      return aContainer.Reco_UnconstrainedPair;
    case 788:
      return aContainer.Reco_UnconstrainedPairValue;
    case 789:
      return aContainer.Reco_UniversalPair;
    case 790:
      return aContainer.Reco_UniversalPairValue;
    case 791:
      return aContainer.Reco_UniversalPairWithRange;
    case 792:
      return aContainer.Reco_PairRepresentationRelationship;
    case 793:
      return aContainer.Reco_RigidLinkRepresentation;
    case 794:
      return aContainer.Reco_KinematicTopologyDirectedStructure;
    case 795:
      return aContainer.Reco_KinematicTopologyNetworkStructure;
    case 796:
      return aContainer.Reco_LinearFlexibleAndPinionPair;
    case 797:
      return aContainer.Reco_LinearFlexibleAndPlanarCurvePair;
    case 798:
      return aContainer.Reco_LinearFlexibleLinkRepresentation;
    case 799:
      return aContainer.Reco_KinematicPair;
    case 801:
      return aContainer.Reco_MechanismStateRepresentation;
    case 803:
      return aContainer.Reco_RepositionedTessellatedItem;
    case 804:
      return aContainer.Reco_TessellatedConnectingEdge;
    case 805:
      return aContainer.Reco_TessellatedEdge;
    case 806:
      return aContainer.Reco_TessellatedPointSet;
    case 807:
      return aContainer.Reco_TessellatedShapeRepresentation;
    case 808:
      return aContainer.Reco_TessellatedShapeRepresentationWithAccuracyParameters;
    case 809:
      return aContainer.Reco_TessellatedShell;
    case 810:
      return aContainer.Reco_TessellatedSolid;
    case 811:
      return aContainer.Reco_TessellatedStructuredItem;
    case 812:
      return aContainer.Reco_TessellatedVertex;
    case 813:
      return aContainer.Reco_TessellatedWire;
    case 814:
      return aContainer.Reco_TriangulatedFace;
    case 815:
      return aContainer.Reco_ComplexTriangulatedFace;
    case 816:
      return aContainer.Reco_ComplexTriangulatedSurfaceSet;
    case 817:
      return aContainer.Reco_CubicBezierTessellatedEdge;
    case 818:
      return aContainer.Reco_CubicBezierTriangulatedFace;
    case 819:
      return aContainer.Reco_TriangulatedSurfaceSet;
    case 820:
      return aContainer.Reco_GeneralPropertyAssociation;
    case 821:
      return aContainer.Reco_GeneralPropertyRelationship;
    case 822:
      return aContainer.Reco_BooleanRepresentationItem;
    case 823:
      return aContainer.Reco_RealRepresentationItem;
    case 824:
      return aContainer.Reco_MechanicalDesignAndDraughtingRelationship;
    default:
      return aContainer.PasReco;
  }
}

//=======================================================================
// function : ComplexType
// purpose  : Complex Type (list of types)
//=======================================================================

Standard_Boolean RWStepAP214_ReadWriteModule::ComplexType(
  const Standard_Integer         CN,
  TColStd_SequenceOfAsciiString& types) const
{
  switch (CN)
  {
    case 319:
      types.Append(StepType(48));
      types.Append(StepType(38));
      types.Append(StepType(84));
      types.Append(StepType(144));
      types.Append(StepType(239));
      types.Append(StepType(247));
      types.Append(StepType(311));
      break;
    case 320:
      types.Append(StepType(48));
      types.Append(StepType(38));
      types.Append(StepType(39));
      types.Append(StepType(84));
      types.Append(StepType(144));
      types.Append(StepType(239));
      types.Append(StepType(247));
      break;
    case 321:
      types.Append(StepType(48));
      types.Append(StepType(38));
      types.Append(StepType(84));
      types.Append(StepType(144));
      types.Append(StepType(236));
      types.Append(StepType(239));
      types.Append(StepType(247));
      break;
    case 322:
      types.Append(StepType(43));
      types.Append(StepType(48));
      types.Append(StepType(38));
      types.Append(StepType(84));
      types.Append(StepType(144));
      types.Append(StepType(239));
      types.Append(StepType(247));
      break;
    case 323:
      types.Append(StepType(49));
      types.Append(StepType(40));
      types.Append(StepType(41));
      types.Append(StepType(144));
      types.Append(StepType(240));
      types.Append(StepType(247));
      types.Append(StepType(271));
      break;
    case 324:
      types.Append(StepType(49));
      types.Append(StepType(40));
      types.Append(StepType(144));
      types.Append(StepType(240));
      types.Append(StepType(247));
      types.Append(StepType(271));
      types.Append(StepType(312));
      break;
    case 325:
      types.Append(StepType(49));
      types.Append(StepType(40));
      types.Append(StepType(144));
      types.Append(StepType(237));
      types.Append(StepType(240));
      types.Append(StepType(247));
      types.Append(StepType(271));
      break;
    case 326:
      types.Append(StepType(44));
      types.Append(StepType(49));
      types.Append(StepType(40));
      types.Append(StepType(144));
      types.Append(StepType(240));
      types.Append(StepType(247));
      types.Append(StepType(271));
      break;
    case 327:
      types.Append(StepType(158));
      types.Append(StepType(169));
      types.Append(StepType(264));
      break;
    case 328:
      types.Append(StepType(169));
      types.Append(StepType(201));
      types.Append(StepType(264));
      break;
    case 329:
      types.Append(StepType(79));
      types.Append(StepType(158));
      types.Append(StepType(169));
      break;
    case 330:
      types.Append(StepType(79));
      types.Append(StepType(169));
      types.Append(StepType(201));
      break;
    case 331:
      types.Append(StepType(143));
      types.Append(StepType(149));
      types.Append(StepType(246));
      break;
    case 332:
      types.Append(StepType(161));
      types.Append(StepType(189));
      types.Append(StepType(247));
      types.Append(StepType(304));
      break;
    case 333:
      types.Append(StepType(143));
      types.Append(StepType(148));
      types.Append(StepType(149));
      types.Append(StepType(246));
      break;
    case 334:
      types.Append(StepType(79));
      types.Append(StepType(169));
      types.Append(StepType(336));
      break;
    case 335:
      types.Append(StepType(169));
      types.Append(StepType(264));
      types.Append(StepType(336));
      break;
    case 337:
      types.Append(StepType(52));
      types.Append(StepType(134));
      types.Append(StepType(144));
      types.Append(StepType(162));
      types.Append(StepType(247));
      types.Append(StepType(266));
      break;
    case 338:
      types.Append(StepType(143));
      types.Append(StepType(188));
      types.Append(StepType(246));
      break;
    case 344:
      types.Append(StepType(169));
      types.Append(StepType(342));
      types.Append(StepType(264));
      break;
    case 345:
      types.Append(StepType(169));
      types.Append(StepType(264));
      types.Append(StepType(343));
      break;
    case 346:
      types.Append(StepType(79));
      types.Append(StepType(169));
      types.Append(StepType(342));
      break;
    case 347:
      types.Append(StepType(79));
      types.Append(StepType(169));
      types.Append(StepType(343));
      break;
    case 357:
      types.Append(StepType(157));
      types.Append(StepType(165));
      types.Append(StepType(310));
      break;
    case 358: //: n5
      types.Append(StepType(48));
      types.Append(StepType(84));
      types.Append(StepType(144));
      types.Append(StepType(247));
      types.Append(StepType(272));
      break;
    case 389:
      types.Append(StepType(249));
      types.Append(StepType(388));
      types.Append(StepType(387));
      break;
    case 409:
      types.Append(StepType(407));
      types.Append(StepType(169));
      types.Append(StepType(264));
      break;
    case 410:
      types.Append(StepType(169));
      types.Append(StepType(264));
      types.Append(StepType(408));
      break;
    case 411:
      types.Append(StepType(407));
      types.Append(StepType(79));
      types.Append(StepType(169));
      break;
    case 412:
      types.Append(StepType(79));
      types.Append(StepType(169));
      types.Append(StepType(408));
      break;
    case 463:
      types.Append(StepType(98));
      types.Append(StepType(245));
      types.Append(StepType(262));
      break;
    case 481:
      types.Append(StepType(406));
      types.Append(StepType(480));
      types.Append(StepType(247));
      break;
    case 574:
      types.Append(StepType(501));
      types.Append(StepType(169));
      types.Append(StepType(264));
      break;
    case 578:
      types.Append(StepType(169));
      types.Append(StepType(264));
      types.Append(StepType(502));
      break;
    case 635:
      types.Append(StepType(157));
      types.Append(StepType(406));
      types.Append(StepType(165));
      types.Append(StepType(247));
      break;
    case 636:
      types.Append(StepType(625));
      types.Append(StepType(627));
      types.Append(StepType(628));
      types.Append(StepType(619));
      break;
    case 650:
      types.Append(StepType(79));
      types.Append(StepType(501));
      types.Append(StepType(169));
      break;
    case 691:
      types.Append(StepType(406));
      types.Append(StepType(165));
      types.Append(StepType(200));
      types.Append(StepType(247));
      break;
    case 692:
      types.Append(StepType(157));
      types.Append(StepType(406));
      types.Append(StepType(165));
      types.Append(StepType(480));
      types.Append(StepType(247));
      break;
    case 693:
      types.Append(StepType(406));
      types.Append(StepType(165));
      types.Append(StepType(200));
      types.Append(StepType(480));
      types.Append(StepType(247));
      break;
    case 694:
      types.Append(StepType(625));
      types.Append(StepType(627));
      types.Append(StepType(625));
      break;
    case 695:
      types.Append(StepType(625));
      types.Append(StepType(627));
      types.Append(StepType(676));
      types.Append(StepType(625));
      break;
    case 696:
      types.Append(StepType(625));
      types.Append(StepType(676));
      types.Append(StepType(625));
      break;
    case 697:
      types.Append(StepType(625));
      types.Append(StepType(627));
      types.Append(StepType(625));
      types.Append(StepType(677));
      break;
    case 698:
      types.Append(StepType(671));
      types.Append(StepType(470));
      types.Append(StepType(630));
      types.Append(StepType(258));
      break;
    case 699:
      types.Append(StepType(470));
      types.Append(StepType(630));
      types.Append(StepType(258));
      break;
    case 705:
      types.Append(StepType(625));
      types.Append(StepType(627));
      types.Append(StepType(675));
      types.Append(StepType(676));
      types.Append(StepType(625));
      break;
    case 706:
      types.Append(StepType(625));
      types.Append(StepType(675));
      types.Append(StepType(676));
      types.Append(StepType(625));
      break;
    case 715:
      types.Append(StepType(402));
      types.Append(StepType(714));
      types.Append(StepType(441));
      types.Append(StepType(245));
      break;
    case 719:
      types.Append(StepType(4));
      types.Append(StepType(7));
      types.Append(StepType(144));
      types.Append(StepType(247));
      types.Append(StepType(270));
      break;
    case 800:
      types.Append(StepType(729));
      types.Append(StepType(144));
      types.Append(StepType(354));
      types.Append(StepType(799));
      types.Append(StepType(743));
      types.Append(StepType(757));
      types.Append(StepType(759));
      types.Append(StepType(247));
      break;
    case 802:
      types.Append(StepType(144));
      types.Append(StepType(803));
      types.Append(StepType(247));
      types.Append(StepType(709));
      types.Append(StepType(708));
      break;
    default:
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
// function : ReadStep
// purpose  : Reading of a file
//=======================================================================

void RWStepAP214_ReadWriteModule::ReadStep(const Standard_Integer                 CN,
                                           const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer                 num,
                                           Handle(Interface_Check)&               ach,
                                           const Handle(Standard_Transient)&      ent) const
{
  if (CN == 0)
  {
#ifdef OCCT_DEBUG
    std::cout << "CN = 0 for num = " << num << std::endl;
#endif
    return;
  }
  switch (CN)
  {
    case 1: {
      DeclareAndCast(StepBasic_Address, anent, ent);
      RWStepBasic_RWAddress tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 2: {
      DeclareAndCast(StepShape_AdvancedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWAdvancedBrepShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 3: {
      DeclareAndCast(StepShape_AdvancedFace, anent, ent);
      RWStepShape_RWAdvancedFace tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 4: {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrence, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrence tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 5: {
      DeclareAndCast(StepVisual_AnnotationFillArea, anent, ent);
      RWStepVisual_RWAnnotationFillArea tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 6: {
      DeclareAndCast(StepVisual_AnnotationFillAreaOccurrence, anent, ent);
      RWStepVisual_RWAnnotationFillAreaOccurrence tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 7: {
      DeclareAndCast(StepVisual_AnnotationOccurrence, anent, ent);
      RWStepVisual_RWAnnotationOccurrence tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 11: {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 12: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 13: {
      DeclareAndCast(StepBasic_ApplicationContext, anent, ent);
      RWStepBasic_RWApplicationContext tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 14: {
      DeclareAndCast(StepBasic_ApplicationContextElement, anent, ent);
      RWStepBasic_RWApplicationContextElement tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 15: {
      DeclareAndCast(StepBasic_ApplicationProtocolDefinition, anent, ent);
      RWStepBasic_RWApplicationProtocolDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 16: {
      DeclareAndCast(StepBasic_Approval, anent, ent);
      RWStepBasic_RWApproval tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 18: {
      DeclareAndCast(StepBasic_ApprovalPersonOrganization, anent, ent);
      RWStepBasic_RWApprovalPersonOrganization tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 19: {
      DeclareAndCast(StepBasic_ApprovalRelationship, anent, ent);
      RWStepBasic_RWApprovalRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 20: {
      DeclareAndCast(StepBasic_ApprovalRole, anent, ent);
      RWStepBasic_RWApprovalRole tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 21: {
      DeclareAndCast(StepBasic_ApprovalStatus, anent, ent);
      RWStepBasic_RWApprovalStatus tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 22: {
      DeclareAndCast(StepVisual_AreaInSet, anent, ent);
      RWStepVisual_RWAreaInSet tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 23: {
      DeclareAndCast(StepAP214_AutoDesignActualDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAndTimeAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 24: {
      DeclareAndCast(StepAP214_AutoDesignActualDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 25: {
      DeclareAndCast(StepAP214_AutoDesignApprovalAssignment, anent, ent);
      RWStepAP214_RWAutoDesignApprovalAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 26: {
      DeclareAndCast(StepAP214_AutoDesignDateAndPersonAssignment, anent, ent);
      RWStepAP214_RWAutoDesignDateAndPersonAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 27: {
      DeclareAndCast(StepAP214_AutoDesignGroupAssignment, anent, ent);
      RWStepAP214_RWAutoDesignGroupAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 28: {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 29: {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 30: {
      DeclareAndCast(StepAP214_AutoDesignOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignOrganizationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 31: {
      DeclareAndCast(StepAP214_AutoDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 32: {
      DeclareAndCast(StepAP214_AutoDesignPresentedItem, anent, ent);
      RWStepAP214_RWAutoDesignPresentedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 33: {
      DeclareAndCast(StepAP214_AutoDesignSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignSecurityClassificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 35: {
      DeclareAndCast(StepGeom_Axis1Placement, anent, ent);
      RWStepGeom_RWAxis1Placement tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 36: {
      DeclareAndCast(StepGeom_Axis2Placement2d, anent, ent);
      RWStepGeom_RWAxis2Placement2d tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 37: {
      DeclareAndCast(StepGeom_Axis2Placement3d, anent, ent);
      RWStepGeom_RWAxis2Placement3d tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 38: {
      DeclareAndCast(StepGeom_BSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 39: {
      DeclareAndCast(StepGeom_BSplineCurveWithKnots, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnots tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 40: {
      DeclareAndCast(StepGeom_BSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 41: {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnots, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnots tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 42: {
      DeclareAndCast(StepVisual_BackgroundColour, anent, ent);
      RWStepVisual_RWBackgroundColour tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 43: {
      DeclareAndCast(StepGeom_BezierCurve, anent, ent);
      RWStepGeom_RWBezierCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 44: {
      DeclareAndCast(StepGeom_BezierSurface, anent, ent);
      RWStepGeom_RWBezierSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 45: {
      DeclareAndCast(StepShape_Block, anent, ent);
      RWStepShape_RWBlock tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 46: {
      DeclareAndCast(StepShape_BooleanResult, anent, ent);
      RWStepShape_RWBooleanResult tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 47: {
      DeclareAndCast(StepGeom_BoundaryCurve, anent, ent);
      RWStepGeom_RWBoundaryCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 48: {
      DeclareAndCast(StepGeom_BoundedCurve, anent, ent);
      RWStepGeom_RWBoundedCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 49: {
      DeclareAndCast(StepGeom_BoundedSurface, anent, ent);
      RWStepGeom_RWBoundedSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 50: {
      DeclareAndCast(StepShape_BoxDomain, anent, ent);
      RWStepShape_RWBoxDomain tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 51: {
      DeclareAndCast(StepShape_BoxedHalfSpace, anent, ent);
      RWStepShape_RWBoxedHalfSpace tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 52: {
      DeclareAndCast(StepShape_BrepWithVoids, anent, ent);
      RWStepShape_RWBrepWithVoids tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 53: {
      DeclareAndCast(StepBasic_CalendarDate, anent, ent);
      RWStepBasic_RWCalendarDate tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 54: {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 55: {
      DeclareAndCast(StepVisual_CameraModel, anent, ent);
      RWStepVisual_RWCameraModel tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 56: {
      DeclareAndCast(StepVisual_CameraModelD2, anent, ent);
      RWStepVisual_RWCameraModelD2 tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 57: {
      DeclareAndCast(StepVisual_CameraModelD3, anent, ent);
      RWStepVisual_RWCameraModelD3 tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 58: {
      DeclareAndCast(StepVisual_CameraUsage, anent, ent);
      RWStepVisual_RWCameraUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 59: {
      DeclareAndCast(StepGeom_CartesianPoint, anent, ent);
      RWStepGeom_RWCartesianPoint tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 60: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 61: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator3d, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator3d tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 62: {
      DeclareAndCast(StepGeom_Circle, anent, ent);
      RWStepGeom_RWCircle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 63: {
      DeclareAndCast(StepShape_ClosedShell, anent, ent);
      RWStepShape_RWClosedShell tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 64: {
      DeclareAndCast(StepVisual_Colour, anent, ent);
      RWStepVisual_RWColour tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 65: {
      DeclareAndCast(StepVisual_ColourRgb, anent, ent);
      RWStepVisual_RWColourRgb tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 66: {
      DeclareAndCast(StepVisual_ColourSpecification, anent, ent);
      RWStepVisual_RWColourSpecification tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 67: {
      DeclareAndCast(StepGeom_CompositeCurve, anent, ent);
      RWStepGeom_RWCompositeCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 68: {
      DeclareAndCast(StepGeom_CompositeCurveOnSurface, anent, ent);
      RWStepGeom_RWCompositeCurveOnSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 69: {
      DeclareAndCast(StepGeom_CompositeCurveSegment, anent, ent);
      RWStepGeom_RWCompositeCurveSegment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 70: {
      DeclareAndCast(StepVisual_CompositeText, anent, ent);
      RWStepVisual_RWCompositeText tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 73: {
      DeclareAndCast(StepVisual_CompositeTextWithExtent, anent, ent);
      RWStepVisual_RWCompositeTextWithExtent tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 74: {
      DeclareAndCast(StepGeom_Conic, anent, ent);
      RWStepGeom_RWConic tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 75: {
      DeclareAndCast(StepGeom_ConicalSurface, anent, ent);
      RWStepGeom_RWConicalSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 76: {
      DeclareAndCast(StepShape_ConnectedFaceSet, anent, ent);
      RWStepShape_RWConnectedFaceSet tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 77: {
      DeclareAndCast(StepVisual_ContextDependentInvisibility, anent, ent);
      RWStepVisual_RWContextDependentInvisibility tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 78: {
      DeclareAndCast(StepVisual_ContextDependentOverRidingStyledItem, anent, ent);
      RWStepVisual_RWContextDependentOverRidingStyledItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 79: {
      DeclareAndCast(StepBasic_ConversionBasedUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 80: {
      DeclareAndCast(StepBasic_CoordinatedUniversalTimeOffset, anent, ent);
      RWStepBasic_RWCoordinatedUniversalTimeOffset tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 82: {
      DeclareAndCast(StepShape_CsgShapeRepresentation, anent, ent);
      RWStepShape_RWCsgShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 83: {
      DeclareAndCast(StepShape_CsgSolid, anent, ent);
      RWStepShape_RWCsgSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 84: {
      DeclareAndCast(StepGeom_Curve, anent, ent);
      RWStepGeom_RWCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 85: {
      DeclareAndCast(StepGeom_CurveBoundedSurface, anent, ent);
      RWStepGeom_RWCurveBoundedSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 86: {
      DeclareAndCast(StepGeom_CurveReplica, anent, ent);
      RWStepGeom_RWCurveReplica tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 87: {
      DeclareAndCast(StepVisual_CurveStyle, anent, ent);
      RWStepVisual_RWCurveStyle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 88: {
      DeclareAndCast(StepVisual_CurveStyleFont, anent, ent);
      RWStepVisual_RWCurveStyleFont tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 89: {
      DeclareAndCast(StepVisual_CurveStyleFontPattern, anent, ent);
      RWStepVisual_RWCurveStyleFontPattern tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 90: {
      DeclareAndCast(StepGeom_CylindricalSurface, anent, ent);
      RWStepGeom_RWCylindricalSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 91: {
      DeclareAndCast(StepBasic_Date, anent, ent);
      RWStepBasic_RWDate tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 92: {
      DeclareAndCast(StepBasic_DateAndTime, anent, ent);
      RWStepBasic_RWDateAndTime tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 95: {
      DeclareAndCast(StepBasic_DateRole, anent, ent);
      RWStepBasic_RWDateRole tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 96: {
      DeclareAndCast(StepBasic_DateTimeRole, anent, ent);
      RWStepBasic_RWDateTimeRole tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 98: {
      DeclareAndCast(StepRepr_DefinitionalRepresentation, anent, ent);
      RWStepRepr_RWDefinitionalRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 99: {
      DeclareAndCast(StepGeom_DegeneratePcurve, anent, ent);
      RWStepGeom_RWDegeneratePcurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 100: {
      DeclareAndCast(StepGeom_DegenerateToroidalSurface, anent, ent);
      RWStepGeom_RWDegenerateToroidalSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 101: {
      DeclareAndCast(StepRepr_DescriptiveRepresentationItem, anent, ent);
      RWStepRepr_RWDescriptiveRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 104: {
      DeclareAndCast(StepBasic_DimensionalExponents, anent, ent);
      RWStepBasic_RWDimensionalExponents tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 105: {
      DeclareAndCast(StepGeom_Direction, anent, ent);
      RWStepGeom_RWDirection tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 106: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 107: {
      DeclareAndCast(StepVisual_DraughtingCallout, anent, ent);
      RWStepVisual_RWDraughtingCallout tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 108: {
      DeclareAndCast(StepVisual_DraughtingPreDefinedColour, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedColour tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 109: {
      DeclareAndCast(StepVisual_DraughtingPreDefinedCurveFont, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedCurveFont tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 115: {
      DeclareAndCast(StepShape_Edge, anent, ent);
      RWStepShape_RWEdge tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 116: {
      DeclareAndCast(StepShape_EdgeCurve, anent, ent);
      RWStepShape_RWEdgeCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 117: {
      DeclareAndCast(StepShape_EdgeLoop, anent, ent);
      RWStepShape_RWEdgeLoop tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 118: {
      DeclareAndCast(StepGeom_ElementarySurface, anent, ent);
      RWStepGeom_RWElementarySurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 119: {
      DeclareAndCast(StepGeom_Ellipse, anent, ent);
      RWStepGeom_RWEllipse tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 120: {
      DeclareAndCast(StepGeom_EvaluatedDegeneratePcurve, anent, ent);
      RWStepGeom_RWEvaluatedDegeneratePcurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 121: {
      DeclareAndCast(StepBasic_ExternalSource, anent, ent);
      RWStepBasic_RWExternalSource tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 122: {
      DeclareAndCast(StepVisual_ExternallyDefinedCurveFont, anent, ent);
      RWStepVisual_RWExternallyDefinedCurveFont tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 124:
    case 126: {
      DeclareAndCast(StepBasic_ExternallyDefinedItem, anent, ent);
      RWStepBasic_RWExternallyDefinedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 128: {
      DeclareAndCast(StepShape_ExtrudedAreaSolid, anent, ent);
      RWStepShape_RWExtrudedAreaSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 129: {
      DeclareAndCast(StepShape_Face, anent, ent);
      RWStepShape_RWFace tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 131: {
      DeclareAndCast(StepShape_FaceBound, anent, ent);
      RWStepShape_RWFaceBound tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 132: {
      DeclareAndCast(StepShape_FaceOuterBound, anent, ent);
      RWStepShape_RWFaceOuterBound tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 133: {
      DeclareAndCast(StepShape_FaceSurface, anent, ent);
      RWStepShape_RWFaceSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 134: {
      DeclareAndCast(StepShape_FacetedBrep, anent, ent);
      RWStepShape_RWFacetedBrep tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 135: {
      DeclareAndCast(StepShape_FacetedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWFacetedBrepShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 136: {
      DeclareAndCast(StepVisual_FillAreaStyle, anent, ent);
      RWStepVisual_RWFillAreaStyle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 137: {
      DeclareAndCast(StepVisual_FillAreaStyleColour, anent, ent);
      RWStepVisual_RWFillAreaStyleColour tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 141: {
      DeclareAndCast(StepRepr_FunctionallyDefinedTransformation, anent, ent);
      RWStepRepr_RWFunctionallyDefinedTransformation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 142: {
      DeclareAndCast(StepShape_GeometricCurveSet, anent, ent);
      RWStepShape_RWGeometricCurveSet tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 143: {
      DeclareAndCast(StepGeom_GeometricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 144: {
      DeclareAndCast(StepGeom_GeometricRepresentationItem, anent, ent);
      RWStepGeom_RWGeometricRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 145: {
      DeclareAndCast(StepShape_GeometricSet, anent, ent);
      RWStepShape_RWGeometricSet tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 146: {
      DeclareAndCast(StepShape_GeometricallyBoundedSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 147: {
      DeclareAndCast(StepShape_GeometricallyBoundedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 148: {
      DeclareAndCast(StepRepr_GlobalUncertaintyAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUncertaintyAssignedContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 149: {
      DeclareAndCast(StepRepr_GlobalUnitAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUnitAssignedContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 150: {
      DeclareAndCast(StepBasic_Group, anent, ent);
      RWStepBasic_RWGroup tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 152: {
      DeclareAndCast(StepBasic_GroupRelationship, anent, ent);
      RWStepBasic_RWGroupRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 153: {
      DeclareAndCast(StepShape_HalfSpaceSolid, anent, ent);
      RWStepShape_RWHalfSpaceSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 154: {
      DeclareAndCast(StepGeom_Hyperbola, anent, ent);
      RWStepGeom_RWHyperbola tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 155: {
      DeclareAndCast(StepGeom_IntersectionCurve, anent, ent);
      RWStepGeom_RWIntersectionCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 156: {
      DeclareAndCast(StepVisual_Invisibility, anent, ent);
      RWStepVisual_RWInvisibility tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 157: {
      DeclareAndCast(StepBasic_LengthMeasureWithUnit, anent, ent);
      RWStepBasic_RWLengthMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 158: {
      DeclareAndCast(StepBasic_LengthUnit, anent, ent);
      RWStepBasic_RWLengthUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 159: {
      DeclareAndCast(StepGeom_Line, anent, ent);
      RWStepGeom_RWLine tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 160: {
      DeclareAndCast(StepBasic_LocalTime, anent, ent);
      RWStepBasic_RWLocalTime tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 161: {
      DeclareAndCast(StepShape_Loop, anent, ent);
      RWStepShape_RWLoop tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 162: {
      DeclareAndCast(StepShape_ManifoldSolidBrep, anent, ent);
      RWStepShape_RWManifoldSolidBrep tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 163: {
      DeclareAndCast(StepShape_ManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWManifoldSurfaceShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 164: {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 165: {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 166: {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationArea, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationArea tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 167: {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationRepresentation, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 169: {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 171: {
      DeclareAndCast(StepGeom_OffsetCurve3d, anent, ent);
      RWStepGeom_RWOffsetCurve3d tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 172: {
      DeclareAndCast(StepGeom_OffsetSurface, anent, ent);
      RWStepGeom_RWOffsetSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 174: {
      DeclareAndCast(StepShape_OpenShell, anent, ent);
      RWStepShape_RWOpenShell tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 175: {
      DeclareAndCast(StepBasic_OrdinalDate, anent, ent);
      RWStepBasic_RWOrdinalDate tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 176: {
      DeclareAndCast(StepBasic_Organization, anent, ent);
      RWStepBasic_RWOrganization tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 178: {
      DeclareAndCast(StepBasic_OrganizationRole, anent, ent);
      RWStepBasic_RWOrganizationRole tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 179: {
      DeclareAndCast(StepBasic_OrganizationalAddress, anent, ent);
      RWStepBasic_RWOrganizationalAddress tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 180: {
      DeclareAndCast(StepShape_OrientedClosedShell, anent, ent);
      RWStepShape_RWOrientedClosedShell tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 181: {
      DeclareAndCast(StepShape_OrientedEdge, anent, ent);
      RWStepShape_RWOrientedEdge tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 182: {
      DeclareAndCast(StepShape_OrientedFace, anent, ent);
      RWStepShape_RWOrientedFace tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 183: {
      DeclareAndCast(StepShape_OrientedOpenShell, anent, ent);
      RWStepShape_RWOrientedOpenShell tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 184: {
      DeclareAndCast(StepShape_OrientedPath, anent, ent);
      RWStepShape_RWOrientedPath tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 185: {
      DeclareAndCast(StepGeom_OuterBoundaryCurve, anent, ent);
      RWStepGeom_RWOuterBoundaryCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 186: {
      DeclareAndCast(StepVisual_OverRidingStyledItem, anent, ent);
      RWStepVisual_RWOverRidingStyledItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 187: {
      DeclareAndCast(StepGeom_Parabola, anent, ent);
      RWStepGeom_RWParabola tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 188: {
      DeclareAndCast(StepRepr_ParametricRepresentationContext, anent, ent);
      RWStepRepr_RWParametricRepresentationContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 189: {
      DeclareAndCast(StepShape_Path, anent, ent);
      RWStepShape_RWPath tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 190: {
      DeclareAndCast(StepGeom_Pcurve, anent, ent);
      RWStepGeom_RWPcurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 191: {
      DeclareAndCast(StepBasic_Person, anent, ent);
      RWStepBasic_RWPerson tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 192: {
      DeclareAndCast(StepBasic_PersonAndOrganization, anent, ent);
      RWStepBasic_RWPersonAndOrganization tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 194: {
      DeclareAndCast(StepBasic_PersonAndOrganizationRole, anent, ent);
      RWStepBasic_RWPersonAndOrganizationRole tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 195: {
      DeclareAndCast(StepBasic_PersonalAddress, anent, ent);
      RWStepBasic_RWPersonalAddress tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 196: {
      DeclareAndCast(StepGeom_Placement, anent, ent);
      RWStepGeom_RWPlacement tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 197: {
      DeclareAndCast(StepVisual_PlanarBox, anent, ent);
      RWStepVisual_RWPlanarBox tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 198: {
      DeclareAndCast(StepVisual_PlanarExtent, anent, ent);
      RWStepVisual_RWPlanarExtent tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 199: {
      DeclareAndCast(StepGeom_Plane, anent, ent);
      RWStepGeom_RWPlane tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 200: {
      DeclareAndCast(StepBasic_PlaneAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWPlaneAngleMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 201: {
      DeclareAndCast(StepBasic_PlaneAngleUnit, anent, ent);
      RWStepBasic_RWPlaneAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 202: {
      DeclareAndCast(StepGeom_Point, anent, ent);
      RWStepGeom_RWPoint tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 203: {
      DeclareAndCast(StepGeom_PointOnCurve, anent, ent);
      RWStepGeom_RWPointOnCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 204: {
      DeclareAndCast(StepGeom_PointOnSurface, anent, ent);
      RWStepGeom_RWPointOnSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 205: {
      DeclareAndCast(StepGeom_PointReplica, anent, ent);
      RWStepGeom_RWPointReplica tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 206: {
      DeclareAndCast(StepVisual_PointStyle, anent, ent);
      RWStepVisual_RWPointStyle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 207: {
      DeclareAndCast(StepShape_PolyLoop, anent, ent);
      RWStepShape_RWPolyLoop tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 208: {
      DeclareAndCast(StepGeom_Polyline, anent, ent);
      RWStepGeom_RWPolyline tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 209: {
      DeclareAndCast(StepVisual_PreDefinedColour, anent, ent);
      RWStepVisual_RWPreDefinedColour tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 210: {
      DeclareAndCast(StepVisual_PreDefinedCurveFont, anent, ent);
      RWStepVisual_RWPreDefinedCurveFont tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 211:
    case 213: {
      DeclareAndCast(StepVisual_PreDefinedItem, anent, ent);
      RWStepVisual_RWPreDefinedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 214: {
      DeclareAndCast(StepVisual_PresentationArea, anent, ent);
      RWStepVisual_RWPresentationArea tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 215: {
      DeclareAndCast(StepVisual_PresentationLayerAssignment, anent, ent);
      RWStepVisual_RWPresentationLayerAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 216: {
      DeclareAndCast(StepVisual_PresentationRepresentation, anent, ent);
      RWStepVisual_RWPresentationRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 217: {
      DeclareAndCast(StepVisual_PresentationSet, anent, ent);
      RWStepVisual_RWPresentationSet tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 218: {
      DeclareAndCast(StepVisual_PresentationSize, anent, ent);
      RWStepVisual_RWPresentationSize tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 219: {
      DeclareAndCast(StepVisual_PresentationStyleAssignment, anent, ent);
      RWStepVisual_RWPresentationStyleAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 220: {
      DeclareAndCast(StepVisual_PresentationStyleByContext, anent, ent);
      RWStepVisual_RWPresentationStyleByContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 221: {
      DeclareAndCast(StepVisual_PresentationView, anent, ent);
      RWStepVisual_RWPresentationView tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 223: {
      DeclareAndCast(StepBasic_Product, anent, ent);
      RWStepBasic_RWProduct tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 224: {
      DeclareAndCast(StepBasic_ProductCategory, anent, ent);
      RWStepBasic_RWProductCategory tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 225: {
      DeclareAndCast(StepBasic_ProductContext, anent, ent);
      RWStepBasic_RWProductContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 227: {
      DeclareAndCast(StepBasic_ProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 228: {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 229: {
      DeclareAndCast(StepBasic_ProductDefinitionFormation, anent, ent);
      RWStepBasic_RWProductDefinitionFormation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 230: {
      DeclareAndCast(StepBasic_ProductDefinitionFormationWithSpecifiedSource, anent, ent);
      RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 231: {
      DeclareAndCast(StepRepr_ProductDefinitionShape, anent, ent);
      RWStepRepr_RWProductDefinitionShape tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 232: {
      DeclareAndCast(StepBasic_ProductRelatedProductCategory, anent, ent);
      RWStepBasic_RWProductRelatedProductCategory tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 233: {
      DeclareAndCast(StepBasic_ProductType, anent, ent);
      RWStepBasic_RWProductType tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 234: {
      DeclareAndCast(StepRepr_PropertyDefinition, anent, ent);
      RWStepRepr_RWPropertyDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 235: {
      DeclareAndCast(StepRepr_PropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWPropertyDefinitionRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 236: {
      DeclareAndCast(StepGeom_QuasiUniformCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 237: {
      DeclareAndCast(StepGeom_QuasiUniformSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 238: {
      DeclareAndCast(StepBasic_RatioMeasureWithUnit, anent, ent);
      RWStepBasic_RWRatioMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 239: {
      DeclareAndCast(StepGeom_RationalBSplineCurve, anent, ent);
      RWStepGeom_RWRationalBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 240: {
      DeclareAndCast(StepGeom_RationalBSplineSurface, anent, ent);
      RWStepGeom_RWRationalBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 241: {
      DeclareAndCast(StepGeom_RectangularCompositeSurface, anent, ent);
      RWStepGeom_RWRectangularCompositeSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 242: {
      DeclareAndCast(StepGeom_RectangularTrimmedSurface, anent, ent);
      RWStepGeom_RWRectangularTrimmedSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 243: {
      DeclareAndCast(StepAP214_RepItemGroup, anent, ent);
      RWStepAP214_RWRepItemGroup tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 244: {
      DeclareAndCast(StepGeom_ReparametrisedCompositeCurveSegment, anent, ent);
      RWStepGeom_RWReparametrisedCompositeCurveSegment tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 245: {
      DeclareAndCast(StepRepr_Representation, anent, ent);
      RWStepRepr_RWRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 246: {
      DeclareAndCast(StepRepr_RepresentationContext, anent, ent);
      RWStepRepr_RWRepresentationContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 247: {
      DeclareAndCast(StepRepr_RepresentationItem, anent, ent);
      RWStepRepr_RWRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 248: {
      DeclareAndCast(StepRepr_RepresentationMap, anent, ent);
      RWStepRepr_RWRepresentationMap tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 249: {
      DeclareAndCast(StepRepr_RepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 250: {
      DeclareAndCast(StepShape_RevolvedAreaSolid, anent, ent);
      RWStepShape_RWRevolvedAreaSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 251: {
      DeclareAndCast(StepShape_RightAngularWedge, anent, ent);
      RWStepShape_RWRightAngularWedge tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 252: {
      DeclareAndCast(StepShape_RightCircularCone, anent, ent);
      RWStepShape_RWRightCircularCone tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 253: {
      DeclareAndCast(StepShape_RightCircularCylinder, anent, ent);
      RWStepShape_RWRightCircularCylinder tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 254: {
      DeclareAndCast(StepGeom_SeamCurve, anent, ent);
      RWStepGeom_RWSeamCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 255: {
      DeclareAndCast(StepBasic_SecurityClassification, anent, ent);
      RWStepBasic_RWSecurityClassification tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 257: {
      DeclareAndCast(StepBasic_SecurityClassificationLevel, anent, ent);
      RWStepBasic_RWSecurityClassificationLevel tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 258: {
      DeclareAndCast(StepRepr_ShapeAspect, anent, ent);
      RWStepRepr_RWShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 259: {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 260: {
      DeclareAndCast(StepRepr_ShapeAspectTransition, anent, ent);
      RWStepRepr_RWShapeAspectTransition tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 261: {
      DeclareAndCast(StepShape_ShapeDefinitionRepresentation, anent, ent);
      RWStepShape_RWShapeDefinitionRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 262: {
      DeclareAndCast(StepShape_ShapeRepresentation, anent, ent);
      RWStepShape_RWShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 263: {
      DeclareAndCast(StepShape_ShellBasedSurfaceModel, anent, ent);
      RWStepShape_RWShellBasedSurfaceModel tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 264: {
      DeclareAndCast(StepBasic_SiUnit, anent, ent);
      RWStepBasic_RWSiUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 265: {
      DeclareAndCast(StepBasic_SolidAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWSolidAngleMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 266: {
      DeclareAndCast(StepShape_SolidModel, anent, ent);
      RWStepShape_RWSolidModel tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 267: {
      DeclareAndCast(StepShape_SolidReplica, anent, ent);
      RWStepShape_RWSolidReplica tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 268: {
      DeclareAndCast(StepShape_Sphere, anent, ent);
      RWStepShape_RWSphere tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 269: {
      DeclareAndCast(StepGeom_SphericalSurface, anent, ent);
      RWStepGeom_RWSphericalSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 270: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 271: {
      DeclareAndCast(StepGeom_Surface, anent, ent);
      RWStepGeom_RWSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 272: {
      DeclareAndCast(StepGeom_SurfaceCurve, anent, ent);
      RWStepGeom_RWSurfaceCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 273: {
      DeclareAndCast(StepGeom_SurfaceOfLinearExtrusion, anent, ent);
      RWStepGeom_RWSurfaceOfLinearExtrusion tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 274: {
      DeclareAndCast(StepGeom_SurfaceOfRevolution, anent, ent);
      RWStepGeom_RWSurfaceOfRevolution tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 275: {
      DeclareAndCast(StepGeom_SurfacePatch, anent, ent);
      RWStepGeom_RWSurfacePatch tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 276: {
      DeclareAndCast(StepGeom_SurfaceReplica, anent, ent);
      RWStepGeom_RWSurfaceReplica tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 277: {
      DeclareAndCast(StepVisual_SurfaceSideStyle, anent, ent);
      RWStepVisual_RWSurfaceSideStyle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 278: {
      DeclareAndCast(StepVisual_SurfaceStyleBoundary, anent, ent);
      RWStepVisual_RWSurfaceStyleBoundary tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 279: {
      DeclareAndCast(StepVisual_SurfaceStyleControlGrid, anent, ent);
      RWStepVisual_RWSurfaceStyleControlGrid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 280: {
      DeclareAndCast(StepVisual_SurfaceStyleFillArea, anent, ent);
      RWStepVisual_RWSurfaceStyleFillArea tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 281: {
      DeclareAndCast(StepVisual_SurfaceStyleParameterLine, anent, ent);
      RWStepVisual_RWSurfaceStyleParameterLine tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 282: {
      DeclareAndCast(StepVisual_SurfaceStyleSegmentationCurve, anent, ent);
      RWStepVisual_RWSurfaceStyleSegmentationCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 283: {
      DeclareAndCast(StepVisual_SurfaceStyleSilhouette, anent, ent);
      RWStepVisual_RWSurfaceStyleSilhouette tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 284: {
      DeclareAndCast(StepVisual_SurfaceStyleUsage, anent, ent);
      RWStepVisual_RWSurfaceStyleUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 285: {
      DeclareAndCast(StepShape_SweptAreaSolid, anent, ent);
      RWStepShape_RWSweptAreaSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 286: {
      DeclareAndCast(StepGeom_SweptSurface, anent, ent);
      RWStepGeom_RWSweptSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 292: {
      DeclareAndCast(StepVisual_Template, anent, ent);
      RWStepVisual_RWTemplate tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 293: {
      DeclareAndCast(StepVisual_TemplateInstance, anent, ent);
      RWStepVisual_RWTemplateInstance tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 295: {
      DeclareAndCast(StepVisual_TextLiteral, anent, ent);
      RWStepVisual_RWTextLiteral tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 300: {
      DeclareAndCast(StepVisual_TextStyle, anent, ent);
      RWStepVisual_RWTextStyle tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 301: {
      DeclareAndCast(StepVisual_TextStyleForDefinedFont, anent, ent);
      RWStepVisual_RWTextStyleForDefinedFont tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 302: {
      DeclareAndCast(StepVisual_TextStyleWithBoxCharacteristics, anent, ent);
      RWStepVisual_RWTextStyleWithBoxCharacteristics tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 304: {
      DeclareAndCast(StepShape_TopologicalRepresentationItem, anent, ent);
      RWStepShape_RWTopologicalRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 305: {
      DeclareAndCast(StepGeom_ToroidalSurface, anent, ent);
      RWStepGeom_RWToroidalSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 306: {
      DeclareAndCast(StepShape_Torus, anent, ent);
      RWStepShape_RWTorus tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 307: {
      DeclareAndCast(StepShape_TransitionalShapeRepresentation, anent, ent);
      RWStepShape_RWTransitionalShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 308: {
      DeclareAndCast(StepGeom_TrimmedCurve, anent, ent);
      RWStepGeom_RWTrimmedCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 310: {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 311: {
      DeclareAndCast(StepGeom_UniformCurve, anent, ent);
      RWStepGeom_RWUniformCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 312: {
      DeclareAndCast(StepGeom_UniformSurface, anent, ent);
      RWStepGeom_RWUniformSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 313: {
      DeclareAndCast(StepGeom_Vector, anent, ent);
      RWStepGeom_RWVector tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 314: {
      DeclareAndCast(StepShape_Vertex, anent, ent);
      RWStepShape_RWVertex tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 315: {
      DeclareAndCast(StepShape_VertexLoop, anent, ent);
      RWStepShape_RWVertexLoop tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 316: {
      DeclareAndCast(StepShape_VertexPoint, anent, ent);
      RWStepShape_RWVertexPoint tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 317: {
      DeclareAndCast(StepVisual_ViewVolume, anent, ent);
      RWStepVisual_RWViewVolume tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 318: {
      DeclareAndCast(StepBasic_WeekOfYearAndDayDate, anent, ent);
      RWStepBasic_RWWeekOfYearAndDayDate tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 319: {
      DeclareAndCast(StepGeom_UniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWUniformCurveAndRationalBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 320: {
      DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 321: {
      DeclareAndCast(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 322: {
      DeclareAndCast(StepGeom_BezierCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBezierCurveAndRationalBSplineCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 323: {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 324: {
      DeclareAndCast(StepGeom_UniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 325: {
      DeclareAndCast(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 326: {
      DeclareAndCast(StepGeom_BezierSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 327: {
      DeclareAndCast(StepBasic_SiUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWSiUnitAndLengthUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 328: {
      DeclareAndCast(StepBasic_SiUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndPlaneAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 329: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndLengthUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 330: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 331: {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext,
                     anent,
                     ent);
      RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 332: {
      DeclareAndCast(StepShape_LoopAndPath, anent, ent);
      RWStepShape_RWLoopAndPath tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      // ------------
      // Added by FMA
      // ------------

    case 333: {
      DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, anent, ent);
      RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 334: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 335: {
      DeclareAndCast(StepBasic_SiUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndSolidAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 336: {
      DeclareAndCast(StepBasic_SolidAngleUnit, anent, ent);
      RWStepBasic_RWSolidAngleUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 337: {
      DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids, anent, ent);
      RWStepShape_RWFacetedBrepAndBrepWithVoids tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 338: {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,
                     anent,
                     ent);
      RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 339: {
      DeclareAndCast(StepBasic_MechanicalContext, anent, ent);
      RWStepBasic_RWMechanicalContext tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 340: // added by CKY : DesignContext cf ProductDefinitionContext
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      tool.ReadStep(data, num, ach, anent);
      break;
    }

      // -----------
      // Added for Rev4
      // -----------

    case 341: // TimeMeasureWithUnit
    {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 342:
    case 343: // RatioUnit, TimeUnit
    {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 344: {
      DeclareAndCast(StepBasic_SiUnitAndRatioUnit, anent, ent);
      RWStepBasic_RWSiUnitAndRatioUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 345: {
      DeclareAndCast(StepBasic_SiUnitAndTimeUnit, anent, ent);
      RWStepBasic_RWSiUnitAndTimeUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 346: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndRatioUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndRatioUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 347: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndTimeUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndTimeUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 348: // ApprovalDateTime
    {
      DeclareAndCast(StepBasic_ApprovalDateTime, anent, ent);
      RWStepBasic_RWApprovalDateTime tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 349: // CameraImage 2d and 3d
    case 350: {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 351: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 352: {
      DeclareAndCast(StepBasic_DerivedUnit, anent, ent);
      RWStepBasic_RWDerivedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 353: {
      DeclareAndCast(StepBasic_DerivedUnitElement, anent, ent);
      RWStepBasic_RWDerivedUnitElement tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 354: {
      DeclareAndCast(StepRepr_ItemDefinedTransformation, anent, ent);
      RWStepRepr_RWItemDefinedTransformation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 355: {
      DeclareAndCast(StepVisual_PresentedItemRepresentation, anent, ent);
      RWStepVisual_RWPresentedItemRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 356: {
      DeclareAndCast(StepVisual_PresentationLayerUsage, anent, ent);
      RWStepVisual_RWPresentationLayerUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      // Added for CATIA -  CKY 15-SEP-1997
      // RWUncertaintyMeasureWithUnit sait lire cette variante
    case 357: {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      //: n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
    case 358: {
      DeclareAndCast(StepGeom_SurfaceCurveAndBoundedCurve, anent, ent);
      RWStepGeom_RWSurfaceCurveAndBoundedCurve tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      //  AP214 : CC1 -> CC2
    case 366: {
      DeclareAndCast(StepAP214_AutoDesignDocumentReference, anent, ent);
      RWStepAP214_RWAutoDesignDocumentReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 367:
    case 368: {
      DeclareAndCast(StepBasic_Document, anent, ent);
      RWStepBasic_RWDocument tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 369: {
      DeclareAndCast(StepBasic_DocumentRelationship, anent, ent);
      RWStepBasic_RWDocumentRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 370: {
      DeclareAndCast(StepBasic_DocumentType, anent, ent);
      RWStepBasic_RWDocumentType tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 371: {
      DeclareAndCast(StepBasic_DocumentUsageConstraint, anent, ent);
      RWStepBasic_RWDocumentUsageConstraint tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 372: {
      DeclareAndCast(StepBasic_Effectivity, anent, ent);
      RWStepBasic_RWEffectivity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 373: {
      DeclareAndCast(StepBasic_ProductDefinitionEffectivity, anent, ent);
      RWStepBasic_RWProductDefinitionEffectivity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 374: {
      DeclareAndCast(StepBasic_ProductDefinitionRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 375: {
      DeclareAndCast(StepBasic_ProductDefinitionWithAssociatedDocuments, anent, ent);
      RWStepBasic_RWProductDefinitionWithAssociatedDocuments tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 376: {
      DeclareAndCast(StepBasic_PhysicallyModeledProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 377: {
      DeclareAndCast(StepRepr_ProductDefinitionUsage, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 378: {
      DeclareAndCast(StepRepr_MakeFromUsageOption, anent, ent);
      RWStepRepr_RWMakeFromUsageOption tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 379:
    case 380:
    case 381: {
      DeclareAndCast(StepRepr_AssemblyComponentUsage, anent, ent);
      RWStepRepr_RWAssemblyComponentUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 382: {
      DeclareAndCast(StepRepr_QuantifiedAssemblyComponentUsage, anent, ent);
      RWStepRepr_RWQuantifiedAssemblyComponentUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 383: {
      DeclareAndCast(StepRepr_SpecifiedHigherUsageOccurrence, anent, ent);
      RWStepRepr_RWSpecifiedHigherUsageOccurrence tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 384: {
      DeclareAndCast(StepRepr_AssemblyComponentUsageSubstitute, anent, ent);
      RWStepRepr_RWAssemblyComponentUsageSubstitute tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 385: {
      DeclareAndCast(StepRepr_SuppliedPartRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 386: {
      DeclareAndCast(StepRepr_ExternallyDefinedRepresentation, anent, ent);
      RWStepRepr_RWRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 387: {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 388: {
      DeclareAndCast(StepRepr_RepresentationRelationshipWithTransformation, anent, ent);
      RWStepRepr_RWRepresentationRelationshipWithTransformation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 389: {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation, anent, ent);
      RWStepRepr_RWShapeRepresentationRelationshipWithTransformation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 390: {
      DeclareAndCast(StepRepr_MaterialDesignation, anent, ent);
      RWStepRepr_RWMaterialDesignation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 391: {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation, anent, ent);
      RWStepShape_RWContextDependentShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      //: S4134: Added from CD to DIS
    case 392: {
      DeclareAndCast(StepAP214_AppliedDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAppliedDateAndTimeAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 393: {
      DeclareAndCast(StepAP214_AppliedDateAssignment, anent, ent);
      RWStepAP214_RWAppliedDateAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 394: {
      DeclareAndCast(StepAP214_AppliedApprovalAssignment, anent, ent);
      RWStepAP214_RWAppliedApprovalAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 395: {
      DeclareAndCast(StepAP214_AppliedGroupAssignment, anent, ent);
      RWStepAP214_RWAppliedGroupAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 396: {
      DeclareAndCast(StepAP214_AppliedOrganizationAssignment, anent, ent);
      RWStepAP214_RWAppliedOrganizationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 397: {
      DeclareAndCast(StepAP214_AppliedPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAppliedPersonAndOrganizationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 398: {
      DeclareAndCast(StepAP214_AppliedPresentedItem, anent, ent);
      RWStepAP214_RWAppliedPresentedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 399: {
      DeclareAndCast(StepAP214_AppliedSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAppliedSecurityClassificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 400: {
      DeclareAndCast(StepAP214_AppliedDocumentReference, anent, ent);
      RWStepAP214_RWAppliedDocumentReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 401: {
      DeclareAndCast(StepBasic_DocumentFile, anent, ent);
      RWStepBasic_RWDocumentFile tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 402: {
      DeclareAndCast(StepBasic_CharacterizedObject, anent, ent);
      RWStepBasic_RWCharacterizedObject tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 403: {
      DeclareAndCast(StepShape_ExtrudedFaceSolid, anent, ent);
      RWStepShape_RWExtrudedFaceSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 404: {
      DeclareAndCast(StepShape_RevolvedFaceSolid, anent, ent);
      RWStepShape_RWRevolvedFaceSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 405: {
      DeclareAndCast(StepShape_SweptFaceSolid, anent, ent);
      RWStepShape_RWSweptFaceSolid tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
    case 406: {
      DeclareAndCast(StepRepr_MeasureRepresentationItem, anent, ent);
      RWStepRepr_RWMeasureRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 407: {
      DeclareAndCast(StepBasic_AreaUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 408: {
      DeclareAndCast(StepBasic_VolumeUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 409: {
      DeclareAndCast(StepBasic_SiUnitAndAreaUnit, anent, ent);
      RWStepBasic_RWSiUnitAndAreaUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 410: {
      DeclareAndCast(StepBasic_SiUnitAndVolumeUnit, anent, ent);
      RWStepBasic_RWSiUnitAndVolumeUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 411: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndAreaUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndAreaUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 412: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndVolumeUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndVolumeUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    // Added by ABV 10.11.99 for AP203
    case 413: {
      DeclareAndCast(StepBasic_Action, anent, ent);
      RWStepBasic_RWAction tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 414: {
      DeclareAndCast(StepBasic_ActionAssignment, anent, ent);
      RWStepBasic_RWActionAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 415: {
      DeclareAndCast(StepBasic_ActionMethod, anent, ent);
      RWStepBasic_RWActionMethod tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 416: {
      DeclareAndCast(StepBasic_ActionRequestAssignment, anent, ent);
      RWStepBasic_RWActionRequestAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 417: {
      DeclareAndCast(StepAP203_CcDesignApproval, anent, ent);
      RWStepAP203_RWCcDesignApproval tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 418: {
      DeclareAndCast(StepAP203_CcDesignCertification, anent, ent);
      RWStepAP203_RWCcDesignCertification tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 419: {
      DeclareAndCast(StepAP203_CcDesignContract, anent, ent);
      RWStepAP203_RWCcDesignContract tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 420: {
      DeclareAndCast(StepAP203_CcDesignDateAndTimeAssignment, anent, ent);
      RWStepAP203_RWCcDesignDateAndTimeAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 421: {
      DeclareAndCast(StepAP203_CcDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP203_RWCcDesignPersonAndOrganizationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 422: {
      DeclareAndCast(StepAP203_CcDesignSecurityClassification, anent, ent);
      RWStepAP203_RWCcDesignSecurityClassification tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 423: {
      DeclareAndCast(StepAP203_CcDesignSpecificationReference, anent, ent);
      RWStepAP203_RWCcDesignSpecificationReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 424: {
      DeclareAndCast(StepBasic_Certification, anent, ent);
      RWStepBasic_RWCertification tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 425: {
      DeclareAndCast(StepBasic_CertificationAssignment, anent, ent);
      RWStepBasic_RWCertificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 426: {
      DeclareAndCast(StepBasic_CertificationType, anent, ent);
      RWStepBasic_RWCertificationType tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 427: {
      DeclareAndCast(StepAP203_Change, anent, ent);
      RWStepAP203_RWChange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 428: {
      DeclareAndCast(StepAP203_ChangeRequest, anent, ent);
      RWStepAP203_RWChangeRequest tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 429: {
      DeclareAndCast(StepRepr_ConfigurationDesign, anent, ent);
      RWStepRepr_RWConfigurationDesign tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 430: {
      DeclareAndCast(StepRepr_ConfigurationEffectivity, anent, ent);
      RWStepRepr_RWConfigurationEffectivity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 431: {
      DeclareAndCast(StepBasic_Contract, anent, ent);
      RWStepBasic_RWContract tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 432: {
      DeclareAndCast(StepBasic_ContractAssignment, anent, ent);
      RWStepBasic_RWContractAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 433: {
      DeclareAndCast(StepBasic_ContractType, anent, ent);
      RWStepBasic_RWContractType tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 434: {
      DeclareAndCast(StepRepr_ProductConcept, anent, ent);
      RWStepRepr_RWProductConcept tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 435: {
      DeclareAndCast(StepBasic_ProductConceptContext, anent, ent);
      RWStepBasic_RWProductConceptContext tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 436: {
      DeclareAndCast(StepAP203_StartRequest, anent, ent);
      RWStepAP203_RWStartRequest tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 437: {
      DeclareAndCast(StepAP203_StartWork, anent, ent);
      RWStepAP203_RWStartWork tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 438: {
      DeclareAndCast(StepBasic_VersionedActionRequest, anent, ent);
      RWStepBasic_RWVersionedActionRequest tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 439: {
      DeclareAndCast(StepBasic_ProductCategoryRelationship, anent, ent);
      RWStepBasic_RWProductCategoryRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 440: {
      DeclareAndCast(StepBasic_ActionRequestSolution, anent, ent);
      RWStepBasic_RWActionRequestSolution tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 441: {
      DeclareAndCast(StepVisual_DraughtingModel, anent, ent);
      RWStepVisual_RWDraughtingModel tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    // Added by ABV 18.04.00 for CAX-IF TRJ4
    case 442: {
      DeclareAndCast(StepShape_AngularLocation, anent, ent);
      RWStepShape_RWAngularLocation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 443: {
      DeclareAndCast(StepShape_AngularSize, anent, ent);
      RWStepShape_RWAngularSize tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 444: {
      DeclareAndCast(StepShape_DimensionalCharacteristicRepresentation, anent, ent);
      RWStepShape_RWDimensionalCharacteristicRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 445: {
      DeclareAndCast(StepShape_DimensionalLocation, anent, ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 446: {
      DeclareAndCast(StepShape_DimensionalLocationWithPath, anent, ent);
      RWStepShape_RWDimensionalLocationWithPath tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 447: {
      DeclareAndCast(StepShape_DimensionalSize, anent, ent);
      RWStepShape_RWDimensionalSize tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 448: {
      DeclareAndCast(StepShape_DimensionalSizeWithPath, anent, ent);
      RWStepShape_RWDimensionalSizeWithPath tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 449: {
      DeclareAndCast(StepShape_ShapeDimensionRepresentation, anent, ent);
      RWStepShape_RWShapeDimensionRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
    case 450: {
      DeclareAndCast(StepBasic_DocumentRepresentationType, anent, ent);
      RWStepBasic_RWDocumentRepresentationType tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 451: {
      DeclareAndCast(StepBasic_ObjectRole, anent, ent);
      RWStepBasic_RWObjectRole tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 452: {
      DeclareAndCast(StepBasic_RoleAssociation, anent, ent);
      RWStepBasic_RWRoleAssociation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 453: {
      DeclareAndCast(StepBasic_IdentificationRole, anent, ent);
      RWStepBasic_RWIdentificationRole tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 454: {
      DeclareAndCast(StepBasic_IdentificationAssignment, anent, ent);
      RWStepBasic_RWIdentificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 455: {
      DeclareAndCast(StepBasic_ExternalIdentificationAssignment, anent, ent);
      RWStepBasic_RWExternalIdentificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 456: {
      DeclareAndCast(StepBasic_EffectivityAssignment, anent, ent);
      RWStepBasic_RWEffectivityAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 457: {
      DeclareAndCast(StepBasic_NameAssignment, anent, ent);
      RWStepBasic_RWNameAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 458: {
      DeclareAndCast(StepBasic_GeneralProperty, anent, ent);
      RWStepBasic_RWGeneralProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 459: {
      DeclareAndCast(StepAP214_Class, anent, ent);
      RWStepAP214_RWClass tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 460: {
      DeclareAndCast(StepAP214_ExternallyDefinedClass, anent, ent);
      RWStepAP214_RWExternallyDefinedClass tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 461: {
      DeclareAndCast(StepAP214_ExternallyDefinedGeneralProperty, anent, ent);
      RWStepAP214_RWExternallyDefinedGeneralProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 462: {
      DeclareAndCast(StepAP214_AppliedExternalIdentificationAssignment, anent, ent);
      RWStepAP214_RWAppliedExternalIdentificationAssignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 463: {
      DeclareAndCast(StepShape_DefinitionalRepresentationAndShapeRepresentation, anent, ent);
      RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

      // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
    case 470: {
      DeclareAndCast(StepRepr_CompositeShapeAspect, anent, ent);
      RWStepRepr_RWCompositeShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 471: {
      DeclareAndCast(StepRepr_DerivedShapeAspect, anent, ent);
      RWStepRepr_RWDerivedShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 472: {
      DeclareAndCast(StepRepr_Extension, anent, ent);
      RWStepRepr_RWExtension tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 473: // same as DimensionalLocation
    {
      DeclareAndCast(StepShape_DirectedDimensionalLocation, anent, ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 474: {
      DeclareAndCast(StepShape_LimitsAndFits, anent, ent);
      RWStepShape_RWLimitsAndFits tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 475: {
      DeclareAndCast(StepShape_ToleranceValue, anent, ent);
      RWStepShape_RWToleranceValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 476: {
      DeclareAndCast(StepShape_MeasureQualification, anent, ent);
      RWStepShape_RWMeasureQualification tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 477: {
      DeclareAndCast(StepShape_PlusMinusTolerance, anent, ent);
      RWStepShape_RWPlusMinusTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 478: {
      DeclareAndCast(StepShape_PrecisionQualifier, anent, ent);
      RWStepShape_RWPrecisionQualifier tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 479: {
      DeclareAndCast(StepShape_TypeQualifier, anent, ent);
      RWStepShape_RWTypeQualifier tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 480: {
      DeclareAndCast(StepShape_QualifiedRepresentationItem, anent, ent);
      RWStepShape_RWQualifiedRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 481: {
      DeclareAndCast(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem, anent, ent);
      RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 482: // the same types
    case 483: {
      DeclareAndCast(StepRepr_CompoundRepresentationItem, anent, ent);
      RWStepRepr_RWCompoundRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 484: // same as ShapeAspectRelationship
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    // Added by ABV 27.12.01 for CAX-IF TRJ9
    case 485: {
      DeclareAndCast(StepShape_CompoundShapeRepresentation, anent, ent);
      RWStepShape_RWCompoundShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 486: {
      DeclareAndCast(StepShape_ConnectedEdgeSet, anent, ent);
      RWStepShape_RWConnectedEdgeSet tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 487: {
      DeclareAndCast(StepShape_ConnectedFaceShapeRepresentation, anent, ent);
      RWStepShape_RWConnectedFaceShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 488: {
      DeclareAndCast(StepShape_EdgeBasedWireframeModel, anent, ent);
      RWStepShape_RWEdgeBasedWireframeModel tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 489: {
      DeclareAndCast(StepShape_EdgeBasedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWEdgeBasedWireframeShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 490: {
      DeclareAndCast(StepShape_FaceBasedSurfaceModel, anent, ent);
      RWStepShape_RWFaceBasedSurfaceModel tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 491: {
      DeclareAndCast(StepShape_NonManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWNonManifoldSurfaceShapeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 492: {
      DeclareAndCast(StepGeom_OrientedSurface, anent, ent);
      RWStepGeom_RWOrientedSurface tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 493: {
      DeclareAndCast(StepShape_Subface, anent, ent);
      RWStepShape_RWSubface tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 494: {
      DeclareAndCast(StepShape_Subedge, anent, ent);
      RWStepShape_RWSubedge tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 495: {
      DeclareAndCast(StepShape_SeamEdge, anent, ent);
      RWStepShape_RWSeamEdge tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 496: {
      DeclareAndCast(StepShape_ConnectedFaceSubSet, anent, ent);
      RWStepShape_RWConnectedFaceSubSet tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 500: {
      DeclareAndCast(StepBasic_EulerAngles, anent, ent);
      RWStepBasic_RWEulerAngles tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 501: {
      DeclareAndCast(StepBasic_MassUnit, anent, ent);
      RWStepBasic_RWMassUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 502: {
      DeclareAndCast(StepBasic_ThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWThermodynamicTemperatureUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 503: {
      DeclareAndCast(StepElement_AnalysisItemWithinRepresentation, anent, ent);
      RWStepElement_RWAnalysisItemWithinRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 504: {
      DeclareAndCast(StepElement_Curve3dElementDescriptor, anent, ent);
      RWStepElement_RWCurve3dElementDescriptor tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 505: {
      DeclareAndCast(StepElement_CurveElementEndReleasePacket, anent, ent);
      RWStepElement_RWCurveElementEndReleasePacket tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 506: {
      DeclareAndCast(StepElement_CurveElementSectionDefinition, anent, ent);
      RWStepElement_RWCurveElementSectionDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 507: {
      DeclareAndCast(StepElement_CurveElementSectionDerivedDefinitions, anent, ent);
      RWStepElement_RWCurveElementSectionDerivedDefinitions tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 508: {
      DeclareAndCast(StepElement_ElementDescriptor, anent, ent);
      RWStepElement_RWElementDescriptor tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 509: {
      DeclareAndCast(StepElement_ElementMaterial, anent, ent);
      RWStepElement_RWElementMaterial tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 510: {
      DeclareAndCast(StepElement_Surface3dElementDescriptor, anent, ent);
      RWStepElement_RWSurface3dElementDescriptor tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 511: {
      DeclareAndCast(StepElement_SurfaceElementProperty, anent, ent);
      RWStepElement_RWSurfaceElementProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 512: {
      DeclareAndCast(StepElement_SurfaceSection, anent, ent);
      RWStepElement_RWSurfaceSection tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 513: {
      DeclareAndCast(StepElement_SurfaceSectionField, anent, ent);
      RWStepElement_RWSurfaceSectionField tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 514: {
      DeclareAndCast(StepElement_SurfaceSectionFieldConstant, anent, ent);
      RWStepElement_RWSurfaceSectionFieldConstant tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 515: {
      DeclareAndCast(StepElement_SurfaceSectionFieldVarying, anent, ent);
      RWStepElement_RWSurfaceSectionFieldVarying tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 516: {
      DeclareAndCast(StepElement_UniformSurfaceSection, anent, ent);
      RWStepElement_RWUniformSurfaceSection tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 517: {
      DeclareAndCast(StepElement_Volume3dElementDescriptor, anent, ent);
      RWStepElement_RWVolume3dElementDescriptor tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 518: {
      DeclareAndCast(StepFEA_AlignedCurve3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWAlignedCurve3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 519: {
      DeclareAndCast(StepFEA_ArbitraryVolume3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 520: {
      DeclareAndCast(StepFEA_Curve3dElementProperty, anent, ent);
      RWStepFEA_RWCurve3dElementProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 521: {
      DeclareAndCast(StepFEA_Curve3dElementRepresentation, anent, ent);
      RWStepFEA_RWCurve3dElementRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 522: {
      DeclareAndCast(StepFEA_Node, anent, ent);
      RWStepFEA_RWNode tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
      // case 523:
      //     {
      //       DeclareAndCast(StepFEA_CurveElementEndCoordinateSystem,anent,ent);
      //       RWStepFEA_RWCurveElementEndCoordinateSystem tool;
      //       tool.ReadStep (data,num,ach,anent);
      //     }
      //     break;
    case 524: {
      DeclareAndCast(StepFEA_CurveElementEndOffset, anent, ent);
      RWStepFEA_RWCurveElementEndOffset tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 525: {
      DeclareAndCast(StepFEA_CurveElementEndRelease, anent, ent);
      RWStepFEA_RWCurveElementEndRelease tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 526: {
      DeclareAndCast(StepFEA_CurveElementInterval, anent, ent);
      RWStepFEA_RWCurveElementInterval tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 527: {
      DeclareAndCast(StepFEA_CurveElementIntervalConstant, anent, ent);
      RWStepFEA_RWCurveElementIntervalConstant tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 528: {
      DeclareAndCast(StepFEA_DummyNode, anent, ent);
      RWStepFEA_RWDummyNode tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 529: {
      DeclareAndCast(StepFEA_CurveElementLocation, anent, ent);
      RWStepFEA_RWCurveElementLocation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 530: {
      DeclareAndCast(StepFEA_ElementGeometricRelationship, anent, ent);
      RWStepFEA_RWElementGeometricRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 531: {
      DeclareAndCast(StepFEA_ElementGroup, anent, ent);
      RWStepFEA_RWElementGroup tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 532: {
      DeclareAndCast(StepFEA_ElementRepresentation, anent, ent);
      RWStepFEA_RWElementRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 533: {
      DeclareAndCast(StepFEA_FeaAreaDensity, anent, ent);
      RWStepFEA_RWFeaAreaDensity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 534: {
      DeclareAndCast(StepFEA_FeaAxis2Placement3d, anent, ent);
      RWStepFEA_RWFeaAxis2Placement3d tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 535: {
      DeclareAndCast(StepFEA_FeaGroup, anent, ent);
      RWStepFEA_RWFeaGroup tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 536: {
      DeclareAndCast(StepFEA_FeaLinearElasticity, anent, ent);
      RWStepFEA_RWFeaLinearElasticity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 537: {
      DeclareAndCast(StepFEA_FeaMassDensity, anent, ent);
      RWStepFEA_RWFeaMassDensity tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 538: {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentation, anent, ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 539: {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentationItem, anent, ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 540: {
      DeclareAndCast(StepFEA_FeaModel, anent, ent);
      RWStepFEA_RWFeaModel tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 541: {
      DeclareAndCast(StepFEA_FeaModel3d, anent, ent);
      RWStepFEA_RWFeaModel3d tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 542: {
      DeclareAndCast(StepFEA_FeaMoistureAbsorption, anent, ent);
      RWStepFEA_RWFeaMoistureAbsorption tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 543: {
      DeclareAndCast(StepFEA_FeaParametricPoint, anent, ent);
      RWStepFEA_RWFeaParametricPoint tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 544: {
      DeclareAndCast(StepFEA_FeaRepresentationItem, anent, ent);
      RWStepFEA_RWFeaRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 545: {
      DeclareAndCast(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion, anent, ent);
      RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 546: {
      DeclareAndCast(StepFEA_FeaShellBendingStiffness, anent, ent);
      RWStepFEA_RWFeaShellBendingStiffness tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 547: {
      DeclareAndCast(StepFEA_FeaShellMembraneBendingCouplingStiffness, anent, ent);
      RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 548: {
      DeclareAndCast(StepFEA_FeaShellMembraneStiffness, anent, ent);
      RWStepFEA_RWFeaShellMembraneStiffness tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 549: {
      DeclareAndCast(StepFEA_FeaShellShearStiffness, anent, ent);
      RWStepFEA_RWFeaShellShearStiffness tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 550: {
      DeclareAndCast(StepFEA_GeometricNode, anent, ent);
      RWStepFEA_RWGeometricNode tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 551: {
      DeclareAndCast(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion, anent, ent);
      RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 552: {
      DeclareAndCast(StepFEA_NodeGroup, anent, ent);
      RWStepFEA_RWNodeGroup tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 553: {
      DeclareAndCast(StepFEA_NodeRepresentation, anent, ent);
      RWStepFEA_RWNodeRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 554: {
      DeclareAndCast(StepFEA_NodeSet, anent, ent);
      RWStepFEA_RWNodeSet tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 555: {
      DeclareAndCast(StepFEA_NodeWithSolutionCoordinateSystem, anent, ent);
      RWStepFEA_RWNodeWithSolutionCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 556: {
      DeclareAndCast(StepFEA_NodeWithVector, anent, ent);
      RWStepFEA_RWNodeWithVector tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 557: {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateDirection, anent, ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateDirection tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 558: {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 559: {
      DeclareAndCast(StepFEA_ParametricSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWParametricSurface3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 560: {
      DeclareAndCast(StepFEA_Surface3dElementRepresentation, anent, ent);
      RWStepFEA_RWSurface3dElementRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
      // case 561:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor22d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor22d tool;
      //       tool.ReadStep (data,num,ach,anent);
      //     }
      //     break;
      // case 562:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor42d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor42d tool;
      //       tool.ReadStep (data,num,ach,anent);
      //     }
      //     break;
      // case 563:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor43d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor43d tool;
      //       tool.ReadStep (data,num,ach,anent);
      //     }
      //     break;
    case 564: {
      DeclareAndCast(StepFEA_Volume3dElementRepresentation, anent, ent);
      RWStepFEA_RWVolume3dElementRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 565: {
      DeclareAndCast(StepRepr_DataEnvironment, anent, ent);
      RWStepRepr_RWDataEnvironment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 566: {
      DeclareAndCast(StepRepr_MaterialPropertyRepresentation, anent, ent);
      RWStepRepr_RWMaterialPropertyRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 567: {
      DeclareAndCast(StepRepr_PropertyDefinitionRelationship, anent, ent);
      RWStepRepr_RWPropertyDefinitionRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 568: {
      DeclareAndCast(StepShape_PointRepresentation, anent, ent);
      RWStepShape_RWPointRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 569: {
      DeclareAndCast(StepRepr_MaterialProperty, anent, ent);
      RWStepRepr_RWMaterialProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 570: {
      DeclareAndCast(StepFEA_FeaModelDefinition, anent, ent);
      RWStepFEA_RWFeaModelDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 571: {
      DeclareAndCast(StepFEA_FreedomAndCoefficient, anent, ent);
      RWStepFEA_RWFreedomAndCoefficient tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 572: {
      DeclareAndCast(StepFEA_FreedomsList, anent, ent);
      RWStepFEA_RWFreedomsList tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 573: {
      DeclareAndCast(StepBasic_ProductDefinitionFormationRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionFormationRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 574: {
      DeclareAndCast(StepBasic_SiUnitAndMassUnit, anent, ent);
      RWStepBasic_RWSiUnitAndMassUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 575: {
      DeclareAndCast(StepFEA_NodeDefinition, anent, ent);
      RWStepFEA_RWNodeDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 576: {
      DeclareAndCast(StepRepr_StructuralResponseProperty, anent, ent);
      RWStepRepr_RWStructuralResponseProperty tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 577: {
      DeclareAndCast(StepRepr_StructuralResponsePropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 578: {
      DeclareAndCast(StepBasic_SiUnitAndThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }

    break;
    case 579: {
      DeclareAndCast(StepFEA_AlignedSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWAlignedSurface3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 580: {
      DeclareAndCast(StepFEA_ConstantSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWConstantSurface3dElementCoordinateSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 581: {
      DeclareAndCast(StepFEA_CurveElementIntervalLinearlyVarying, anent, ent);
      RWStepFEA_RWCurveElementIntervalLinearlyVarying tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 582: {
      DeclareAndCast(StepFEA_FeaCurveSectionGeometricRelationship, anent, ent);
      RWStepFEA_RWFeaCurveSectionGeometricRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 583: {
      DeclareAndCast(StepFEA_FeaSurfaceSectionGeometricRelationship, anent, ent);
      RWStepFEA_RWFeaSurfaceSectionGeometricRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 600: {
      DeclareAndCast(StepBasic_DocumentProductAssociation, anent, ent);
      RWStepBasic_RWDocumentProductAssociation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 601: {
      DeclareAndCast(StepBasic_DocumentProductEquivalence, anent, ent);
      RWStepBasic_RWDocumentProductEquivalence tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 609: {
      DeclareAndCast(StepDimTol_CylindricityTolerance, anent, ent);
      RWStepDimTol_RWCylindricityTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 610: {
      DeclareAndCast(StepShape_ShapeRepresentationWithParameters, anent, ent);
      RWStepShape_RWShapeRepresentationWithParameters tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 611: {
      DeclareAndCast(StepDimTol_AngularityTolerance, anent, ent);
      RWStepDimTol_RWAngularityTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 612: {
      DeclareAndCast(StepDimTol_ConcentricityTolerance, anent, ent);
      RWStepDimTol_RWConcentricityTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 613: {
      DeclareAndCast(StepDimTol_CircularRunoutTolerance, anent, ent);
      RWStepDimTol_RWCircularRunoutTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 614: {
      DeclareAndCast(StepDimTol_CoaxialityTolerance, anent, ent);
      RWStepDimTol_RWCoaxialityTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 615: {
      DeclareAndCast(StepDimTol_FlatnessTolerance, anent, ent);
      RWStepDimTol_RWFlatnessTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 616: {
      DeclareAndCast(StepDimTol_LineProfileTolerance, anent, ent);
      RWStepDimTol_RWLineProfileTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 617: {
      DeclareAndCast(StepDimTol_ParallelismTolerance, anent, ent);
      RWStepDimTol_RWParallelismTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 618: {
      DeclareAndCast(StepDimTol_PerpendicularityTolerance, anent, ent);
      RWStepDimTol_RWPerpendicularityTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 619: {
      DeclareAndCast(StepDimTol_PositionTolerance, anent, ent);
      RWStepDimTol_RWPositionTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 620: {
      DeclareAndCast(StepDimTol_RoundnessTolerance, anent, ent);
      RWStepDimTol_RWRoundnessTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 621: {
      DeclareAndCast(StepDimTol_StraightnessTolerance, anent, ent);
      RWStepDimTol_RWStraightnessTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 622: {
      DeclareAndCast(StepDimTol_SurfaceProfileTolerance, anent, ent);
      RWStepDimTol_RWSurfaceProfileTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 623: {
      DeclareAndCast(StepDimTol_SymmetryTolerance, anent, ent);
      RWStepDimTol_RWSymmetryTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 624: {
      DeclareAndCast(StepDimTol_TotalRunoutTolerance, anent, ent);
      RWStepDimTol_RWTotalRunoutTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 625: {
      DeclareAndCast(StepDimTol_GeometricTolerance, anent, ent);
      RWStepDimTol_RWGeometricTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 626: {
      DeclareAndCast(StepDimTol_GeometricToleranceRelationship, anent, ent);
      RWStepDimTol_RWGeometricToleranceRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 627: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDatumReference, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDatumReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 628: {
      DeclareAndCast(StepDimTol_ModifiedGeometricTolerance, anent, ent);
      RWStepDimTol_RWModifiedGeometricTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 629: {
      DeclareAndCast(StepDimTol_Datum, anent, ent);
      RWStepDimTol_RWDatum tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 630: {
      DeclareAndCast(StepDimTol_DatumFeature, anent, ent);
      RWStepDimTol_RWDatumFeature tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 631: {
      DeclareAndCast(StepDimTol_DatumReference, anent, ent);
      RWStepDimTol_RWDatumReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 632: {
      DeclareAndCast(StepDimTol_CommonDatum, anent, ent);
      RWStepDimTol_RWCommonDatum tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 633: {
      DeclareAndCast(StepDimTol_DatumTarget, anent, ent);
      RWStepDimTol_RWDatumTarget tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 634: {
      DeclareAndCast(StepDimTol_PlacedDatumTargetFeature, anent, ent);
      RWStepDimTol_RWPlacedDatumTargetFeature tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 635: {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnit, anent, ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 636: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 650: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndMassUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndMassUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 651: {
      DeclareAndCast(StepBasic_MassMeasureWithUnit, anent, ent);
      RWStepBasic_RWMassMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 660: {
      DeclareAndCast(StepRepr_Apex, anent, ent);
      RWStepRepr_RWApex tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 661: {
      DeclareAndCast(StepRepr_CentreOfSymmetry, anent, ent);
      RWStepRepr_RWCentreOfSymmetry tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 662: {
      DeclareAndCast(StepRepr_GeometricAlignment, anent, ent);
      RWStepRepr_RWGeometricAlignment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 663: {
      DeclareAndCast(StepRepr_PerpendicularTo, anent, ent);
      RWStepRepr_RWPerpendicularTo tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 664: {
      DeclareAndCast(StepRepr_Tangent, anent, ent);
      RWStepRepr_RWTangent tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 665: {
      DeclareAndCast(StepRepr_ParallelOffset, anent, ent);
      RWStepRepr_RWParallelOffset tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 666: {
      DeclareAndCast(StepAP242_GeometricItemSpecificUsage, anent, ent);
      RWStepAP242_RWGeometricItemSpecificUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 667: {
      DeclareAndCast(StepAP242_IdAttribute, anent, ent);
      RWStepAP242_RWIdAttribute tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 668: {
      DeclareAndCast(StepAP242_ItemIdentifiedRepresentationUsage, anent, ent);
      RWStepAP242_RWItemIdentifiedRepresentationUsage tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 669: {
      DeclareAndCast(StepRepr_AllAroundShapeAspect, anent, ent);
      RWStepRepr_RWAllAroundShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 670: {
      DeclareAndCast(StepRepr_BetweenShapeAspect, anent, ent);
      RWStepRepr_RWBetweenShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 671: {
      DeclareAndCast(StepRepr_CompositeGroupShapeAspect, anent, ent);
      RWStepRepr_RWCompositeGroupShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 672: {
      DeclareAndCast(StepRepr_ContinuosShapeAspect, anent, ent);
      RWStepRepr_RWContinuosShapeAspect tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 673: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedAreaUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 674: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 675: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithMaximumTolerance, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithMaximumTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 676: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithModifiers, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithModifiers tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 677: {
      DeclareAndCast(StepDimTol_UnequallyDisposedGeometricTolerance, anent, ent);
      RWStepDimTol_RWUnequallyDisposedGeometricTolerance tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 678: {
      DeclareAndCast(StepDimTol_NonUniformZoneDefinition, anent, ent);
      RWStepDimTol_RWNonUniformZoneDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 679: {
      DeclareAndCast(StepDimTol_ProjectedZoneDefinition, anent, ent);
      RWStepDimTol_RWProjectedZoneDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 680: {
      DeclareAndCast(StepDimTol_RunoutZoneDefinition, anent, ent);
      RWStepDimTol_RWRunoutZoneDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 681: {
      DeclareAndCast(StepDimTol_RunoutZoneOrientation, anent, ent);
      RWStepDimTol_RWRunoutZoneOrientation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 682: {
      DeclareAndCast(StepDimTol_ToleranceZone, anent, ent);
      RWStepDimTol_RWToleranceZone tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 683: {
      DeclareAndCast(StepDimTol_ToleranceZoneDefinition, anent, ent);
      RWStepDimTol_RWToleranceZoneDefinition tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 684: {
      DeclareAndCast(StepDimTol_ToleranceZoneForm, anent, ent);
      RWStepDimTol_RWToleranceZoneForm tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 685: {
      DeclareAndCast(StepShape_ValueFormatTypeQualifier, anent, ent);
      RWStepShape_RWValueFormatTypeQualifier tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 686: {
      DeclareAndCast(StepDimTol_DatumReferenceCompartment, anent, ent);
      RWStepDimTol_RWDatumReferenceCompartment tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 687: {
      DeclareAndCast(StepDimTol_DatumReferenceElement, anent, ent);
      RWStepDimTol_RWDatumReferenceElement tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 688: {
      DeclareAndCast(StepDimTol_DatumReferenceModifierWithValue, anent, ent);
      RWStepDimTol_RWDatumReferenceModifierWithValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 689: {
      DeclareAndCast(StepDimTol_DatumSystem, anent, ent);
      RWStepDimTol_RWDatumSystem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 690: {
      DeclareAndCast(StepDimTol_GeneralDatumReference, anent, ent);
      RWStepDimTol_RWGeneralDatumReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 691: {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit, anent, ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 692: {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI, anent, ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 693: {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI, anent, ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 694: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRef, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRef tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 695: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 696: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMod, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMod tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 697: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 698: {
      DeclareAndCast(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp, anent, ent);
      RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 699: {
      DeclareAndCast(StepRepr_CompShAspAndDatumFeatAndShAsp, anent, ent);
      RWStepRepr_RWCompShAspAndDatumFeatAndShAsp tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 700: {
      DeclareAndCast(StepRepr_IntegerRepresentationItem, anent, ent);
      RWStepRepr_RWIntegerRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 701: {
      DeclareAndCast(StepRepr_ValueRepresentationItem, anent, ent);
      RWStepRepr_RWValueRepresentationItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 702: {
      DeclareAndCast(StepRepr_FeatureForDatumTargetRelationship, anent, ent);
      RWStepRepr_RWFeatureForDatumTargetRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 703: {
      DeclareAndCast(StepAP242_DraughtingModelItemAssociation, anent, ent);
      RWStepAP242_RWDraughtingModelItemAssociation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 704: {
      DeclareAndCast(StepVisual_AnnotationPlane, anent, ent);
      RWStepVisual_RWAnnotationPlane tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 705: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 706: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMaxTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 707: {
      DeclareAndCast(StepVisual_TessellatedAnnotationOccurrence, anent, ent);
      RWStepVisual_RWTessellatedAnnotationOccurrence tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 708: {
      DeclareAndCast(StepVisual_TessellatedItem, anent, ent);
      RWStepVisual_RWTessellatedItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;

    case 709: {
      DeclareAndCast(StepVisual_TessellatedGeometricSet, anent, ent);
      RWStepVisual_RWTessellatedGeometricSet tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 710: {
      DeclareAndCast(StepVisual_TessellatedCurveSet, anent, ent);
      RWStepVisual_RWTessellatedCurveSet tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 711: {
      DeclareAndCast(StepVisual_CoordinatesList, anent, ent);
      RWStepVisual_RWCoordinatesList tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 712: {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentation, anent, ent);
      RWStepRepr_RWConstructiveGeometryRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 713: {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentationRelationship, anent, ent);
      RWStepRepr_RWConstructiveGeometryRepresentationRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 714: {
      DeclareAndCast(StepRepr_CharacterizedRepresentation, anent, ent);
      RWStepRepr_RWCharacterizedRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 715: {
      DeclareAndCast(StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel, anent, ent);
      RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 716: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClipping, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClipping tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 717: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingIntersection, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingIntersection tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 718: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingUnion, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingUnion tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 719: {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 720: {
      DeclareAndCast(StepVisual_SurfaceStyleTransparent, anent, ent);
      RWStepVisual_RWSurfaceStyleTransparent tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 721: {
      DeclareAndCast(StepVisual_SurfaceStyleReflectanceAmbient, anent, ent);
      RWStepVisual_RWSurfaceStyleReflectanceAmbient tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 722: {
      DeclareAndCast(StepVisual_SurfaceStyleRendering, anent, ent);
      RWStepVisual_RWSurfaceStyleRendering tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 723: {
      DeclareAndCast(StepVisual_SurfaceStyleRenderingWithProperties, anent, ent);
      RWStepVisual_RWSurfaceStyleRenderingWithProperties tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 724: {
      DeclareAndCast(StepRepr_RepresentationContextReference, anent, ent);
      RWStepRepr_RWRepresentationContextReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 725: {
      DeclareAndCast(StepRepr_RepresentationReference, anent, ent);
      RWStepRepr_RWRepresentationReference tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 726: {
      DeclareAndCast(StepGeom_SuParameters, anent, ent);
      RWStepGeom_RWSuParameters tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 727: {
      DeclareAndCast(StepKinematics_RotationAboutDirection, anent, ent);
      RWStepKinematics_RWRotationAboutDirection tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 728: {
      DeclareAndCast(StepKinematics_KinematicJoint, anent, ent);
      RWStepKinematics_RWKinematicJoint tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 729: {
      DeclareAndCast(StepKinematics_ActuatedKinematicPair, anent, ent);
      RWStepKinematics_RWActuatedKinematicPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 730: {
      DeclareAndCast(StepKinematics_ContextDependentKinematicLinkRepresentation, anent, ent);
      RWStepKinematics_RWContextDependentKinematicLinkRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 731: {
      DeclareAndCast(StepKinematics_CylindricalPair, anent, ent);
      RWStepKinematics_RWCylindricalPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 732: {
      DeclareAndCast(StepKinematics_CylindricalPairValue, anent, ent);
      RWStepKinematics_RWCylindricalPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 733: {
      DeclareAndCast(StepKinematics_CylindricalPairWithRange, anent, ent);
      RWStepKinematics_RWCylindricalPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 734: {
      DeclareAndCast(StepKinematics_FullyConstrainedPair, anent, ent);
      RWStepKinematics_RWFullyConstrainedPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 735: {
      DeclareAndCast(StepKinematics_GearPair, anent, ent);
      RWStepKinematics_RWGearPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 736: {
      DeclareAndCast(StepKinematics_GearPairValue, anent, ent);
      RWStepKinematics_RWGearPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 737: {
      DeclareAndCast(StepKinematics_GearPairWithRange, anent, ent);
      RWStepKinematics_RWGearPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 738: {
      DeclareAndCast(StepKinematics_HomokineticPair, anent, ent);
      RWStepKinematics_RWHomokineticPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 739: {
      DeclareAndCast(StepKinematics_KinematicLink, anent, ent);
      RWStepKinematics_RWKinematicLink tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 740: {
      DeclareAndCast(StepKinematics_KinematicLinkRepresentationAssociation, anent, ent);
      RWStepKinematics_RWKinematicLinkRepresentationAssociation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 741: {
      DeclareAndCast(StepKinematics_KinematicPropertyMechanismRepresentation, anent, ent);
      RWStepKinematics_RWKinematicPropertyMechanismRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 742: {
      DeclareAndCast(StepKinematics_KinematicTopologyStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyStructure tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 743: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPair, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 744: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPairValue, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 745: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPairWithRange, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 746: {
      DeclareAndCast(StepKinematics_MechanismRepresentation, anent, ent);
      RWStepKinematics_RWMechanismRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 747: {
      DeclareAndCast(StepKinematics_OrientedJoint, anent, ent);
      RWStepKinematics_RWOrientedJoint tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 748: {
      DeclareAndCast(StepKinematics_PlanarCurvePair, anent, ent);
      RWStepKinematics_RWPlanarCurvePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 749: {
      DeclareAndCast(StepKinematics_PlanarCurvePairRange, anent, ent);
      RWStepKinematics_RWPlanarCurvePairRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 750: {
      DeclareAndCast(StepKinematics_PlanarPair, anent, ent);
      RWStepKinematics_RWPlanarPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 751: {
      DeclareAndCast(StepKinematics_PlanarPairValue, anent, ent);
      RWStepKinematics_RWPlanarPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 752: {
      DeclareAndCast(StepKinematics_PlanarPairWithRange, anent, ent);
      RWStepKinematics_RWPlanarPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 753: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePair, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 754: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePairValue, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 755: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePairWithRange, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 756: {
      DeclareAndCast(StepKinematics_PointOnSurfacePair, anent, ent);
      RWStepKinematics_RWPointOnSurfacePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 757: {
      DeclareAndCast(StepKinematics_PointOnSurfacePairValue, anent, ent);
      RWStepKinematics_RWPointOnSurfacePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 758: {
      DeclareAndCast(StepKinematics_PointOnSurfacePairWithRange, anent, ent);
      RWStepKinematics_RWPointOnSurfacePairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 759: {
      DeclareAndCast(StepKinematics_PrismaticPair, anent, ent);
      RWStepKinematics_RWPrismaticPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 760: {
      DeclareAndCast(StepKinematics_PrismaticPairValue, anent, ent);
      RWStepKinematics_RWPrismaticPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 761: {
      DeclareAndCast(StepKinematics_PrismaticPairWithRange, anent, ent);
      RWStepKinematics_RWPrismaticPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 762: {
      DeclareAndCast(StepKinematics_ProductDefinitionKinematics, anent, ent);
      RWStepKinematics_RWProductDefinitionKinematics tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 763: {
      DeclareAndCast(StepKinematics_ProductDefinitionRelationshipKinematics, anent, ent);
      RWStepKinematics_RWProductDefinitionRelationshipKinematics tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 764: {
      DeclareAndCast(StepKinematics_RackAndPinionPair, anent, ent);
      RWStepKinematics_RWRackAndPinionPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 765: {
      DeclareAndCast(StepKinematics_RackAndPinionPairValue, anent, ent);
      RWStepKinematics_RWRackAndPinionPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 766: {
      DeclareAndCast(StepKinematics_RackAndPinionPairWithRange, anent, ent);
      RWStepKinematics_RWRackAndPinionPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 767: {
      DeclareAndCast(StepKinematics_RevolutePair, anent, ent);
      RWStepKinematics_RWRevolutePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 768: {
      DeclareAndCast(StepKinematics_RevolutePairValue, anent, ent);
      RWStepKinematics_RWRevolutePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 769: {
      DeclareAndCast(StepKinematics_RevolutePairWithRange, anent, ent);
      RWStepKinematics_RWRevolutePairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 770: {
      DeclareAndCast(StepKinematics_RollingCurvePair, anent, ent);
      RWStepKinematics_RWRollingCurvePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 771: {
      DeclareAndCast(StepKinematics_RollingCurvePairValue, anent, ent);
      RWStepKinematics_RWRollingCurvePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 772: {
      DeclareAndCast(StepKinematics_RollingSurfacePair, anent, ent);
      RWStepKinematics_RWRollingSurfacePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 773: {
      DeclareAndCast(StepKinematics_RollingSurfacePairValue, anent, ent);
      RWStepKinematics_RWRollingSurfacePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 774: {
      DeclareAndCast(StepKinematics_ScrewPair, anent, ent);
      RWStepKinematics_RWScrewPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 775: {
      DeclareAndCast(StepKinematics_ScrewPairValue, anent, ent);
      RWStepKinematics_RWScrewPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 776: {
      DeclareAndCast(StepKinematics_ScrewPairWithRange, anent, ent);
      RWStepKinematics_RWScrewPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 777: {
      DeclareAndCast(StepKinematics_SlidingCurvePair, anent, ent);
      RWStepKinematics_RWSlidingCurvePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 778: {
      DeclareAndCast(StepKinematics_SlidingCurvePairValue, anent, ent);
      RWStepKinematics_RWSlidingCurvePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 779: {
      DeclareAndCast(StepKinematics_SlidingSurfacePair, anent, ent);
      RWStepKinematics_RWSlidingSurfacePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 780: {
      DeclareAndCast(StepKinematics_SlidingSurfacePairValue, anent, ent);
      RWStepKinematics_RWSlidingSurfacePairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 781: {
      DeclareAndCast(StepKinematics_SphericalPair, anent, ent);
      RWStepKinematics_RWSphericalPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 782: {
      DeclareAndCast(StepKinematics_SphericalPairValue, anent, ent);
      RWStepKinematics_RWSphericalPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 783: {
      DeclareAndCast(StepKinematics_SphericalPairWithPin, anent, ent);
      RWStepKinematics_RWSphericalPairWithPin tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 784: {
      DeclareAndCast(StepKinematics_SphericalPairWithPinAndRange, anent, ent);
      RWStepKinematics_RWSphericalPairWithPinAndRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 785: {
      DeclareAndCast(StepKinematics_SphericalPairWithRange, anent, ent);
      RWStepKinematics_RWSphericalPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 786: {
      DeclareAndCast(StepKinematics_SurfacePairWithRange, anent, ent);
      RWStepKinematics_RWSurfacePairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 787: {
      DeclareAndCast(StepKinematics_UnconstrainedPair, anent, ent);
      RWStepKinematics_RWUnconstrainedPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 788: {
      DeclareAndCast(StepKinematics_UnconstrainedPairValue, anent, ent);
      RWStepKinematics_RWUnconstrainedPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 789: {
      DeclareAndCast(StepKinematics_UniversalPair, anent, ent);
      RWStepKinematics_RWUniversalPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 790: {
      DeclareAndCast(StepKinematics_UniversalPairValue, anent, ent);
      RWStepKinematics_RWUniversalPairValue tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 791: {
      DeclareAndCast(StepKinematics_UniversalPairWithRange, anent, ent);
      RWStepKinematics_RWUniversalPairWithRange tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 792: {
      DeclareAndCast(StepKinematics_PairRepresentationRelationship, anent, ent);
      RWStepKinematics_RWPairRepresentationRelationship tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 793: {
      DeclareAndCast(StepKinematics_RigidLinkRepresentation, anent, ent);
      RWStepKinematics_RWRigidLinkRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 794: {
      DeclareAndCast(StepKinematics_KinematicTopologyDirectedStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyDirectedStructure tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 795: {
      DeclareAndCast(StepKinematics_KinematicTopologyNetworkStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyNetworkStructure tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 796: {
      DeclareAndCast(StepKinematics_LinearFlexibleAndPinionPair, anent, ent);
      RWStepKinematics_RWLinearFlexibleAndPinionPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 797: {
      DeclareAndCast(StepKinematics_LinearFlexibleAndPlanarCurvePair, anent, ent);
      RWStepKinematics_RWLinearFlexibleAndPlanarCurvePair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 798: {
      DeclareAndCast(StepKinematics_LinearFlexibleLinkRepresentation, anent, ent);
      RWStepKinematics_RWLinearFlexibleLinkRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 800: {
      DeclareAndCast(StepKinematics_ActuatedKinPairAndOrderKinPair, anent, ent);
      RWStepKinematics_RWActuatedKinPairAndOrderKinPair tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 801: {
      DeclareAndCast(StepKinematics_MechanismStateRepresentation, anent, ent);
      RWStepKinematics_RWMechanismStateRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
    case 802: {
      DeclareAndCast(StepVisual_RepositionedTessellatedGeometricSet, anEnt, ent);
      RWStepVisual_RWRepositionedTessellatedGeometricSet aTool;
      aTool.ReadStep(data, num, ach, anEnt);
      break;
    }
    case 803: {
      DeclareAndCast(StepVisual_RepositionedTessellatedItem, anEnt, ent);
      RWStepVisual_RWRepositionedTessellatedItem aTool;
      aTool.ReadStep(data, num, ach, anEnt);
      break;
    }
    case 804: {
      DeclareAndCast(StepVisual_TessellatedConnectingEdge, anEnt, ent);
      RWStepVisual_RWTessellatedConnectingEdge aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 805: {
      DeclareAndCast(StepVisual_TessellatedEdge, anEnt, ent);
      RWStepVisual_RWTessellatedEdge aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 806: {
      DeclareAndCast(StepVisual_TessellatedPointSet, anEnt, ent);
      RWStepVisual_RWTessellatedPointSet aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 807: {
      DeclareAndCast(StepVisual_TessellatedShapeRepresentation, anEnt, ent);
      RWStepVisual_RWTessellatedShapeRepresentation aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 808: {
      DeclareAndCast(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters, anEnt, ent);
      RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 809: {
      DeclareAndCast(StepVisual_TessellatedShell, anEnt, ent);
      RWStepVisual_RWTessellatedShell aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 810: {
      DeclareAndCast(StepVisual_TessellatedSolid, anEnt, ent);
      RWStepVisual_RWTessellatedSolid aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 811: {
      DeclareAndCast(StepVisual_TessellatedStructuredItem, anEnt, ent);
      RWStepVisual_RWTessellatedStructuredItem aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 812: {
      DeclareAndCast(StepVisual_TessellatedVertex, anEnt, ent);
      RWStepVisual_RWTessellatedVertex aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 813: {
      DeclareAndCast(StepVisual_TessellatedWire, anEnt, ent);
      RWStepVisual_RWTessellatedWire aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 814: {
      DeclareAndCast(StepVisual_TriangulatedFace, anEnt, ent);
      RWStepVisual_RWTriangulatedFace aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 815: {
      DeclareAndCast(StepVisual_ComplexTriangulatedFace, anEnt, ent);
      RWStepVisual_RWComplexTriangulatedFace aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 816: {
      DeclareAndCast(StepVisual_ComplexTriangulatedSurfaceSet, anEnt, ent);
      RWStepVisual_RWComplexTriangulatedSurfaceSet aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 817: {
      DeclareAndCast(StepVisual_CubicBezierTessellatedEdge, anEnt, ent);
      RWStepVisual_RWCubicBezierTessellatedEdge aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 818: {
      DeclareAndCast(StepVisual_CubicBezierTriangulatedFace, anEnt, ent);
      RWStepVisual_RWCubicBezierTriangulatedFace aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 819: {
      DeclareAndCast(StepVisual_TriangulatedSurfaceSet, anEnt, ent);
      RWStepVisual_RWTriangulatedSurfaceSet aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 820: {
      DeclareAndCast(StepBasic_GeneralPropertyAssociation, anEnt, ent);
      RWStepBasic_RWGeneralPropertyAssociation aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 821: {
      DeclareAndCast(StepBasic_GeneralPropertyRelationship, anEnt, ent);
      RWStepBasic_RWGeneralPropertyRelationship aTool;
      aTool.ReadStep(data, num, ach, anEnt);
    }
    break;
    case 822: {
      DeclareAndCast(StepRepr_BooleanRepresentationItem, anent, ent);
      RWStepRepr_RWBooleanRepresentationItem aTool;
      aTool.ReadStep(data, num, ach, anent);
    }
    break;
    case 823: {
      DeclareAndCast(StepRepr_RealRepresentationItem, anent, ent);
      RWStepRepr_RWRealRepresentationItem aTool;
      aTool.ReadStep(data, num, ach, anent);
    }
    break;
    case 824: {
      DeclareAndCast(StepRepr_MechanicalDesignAndDraughtingRelationship, anent, ent);
      RWStepRepr_RWMechanicalDesignAndDraughtingRelationship aTool;
      aTool.ReadStep(data, num, ach, anent);
    }
    break;
    default:
      ach->AddFail("Type Mismatch when reading - Entity");
  }
  return;
}

//=======================================================================
// function : WriteStep
// purpose  : Writing of a file
//=======================================================================

void RWStepAP214_ReadWriteModule::WriteStep(const Standard_Integer            CN,
                                            StepData_StepWriter&              SW,
                                            const Handle(Standard_Transient)& ent) const
{
  if (CN == 0)
    return;
  switch (CN)
  {
    case 1: {
      DeclareAndCast(StepBasic_Address, anent, ent);
      RWStepBasic_RWAddress tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 2: {
      DeclareAndCast(StepShape_AdvancedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWAdvancedBrepShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 3: {
      DeclareAndCast(StepShape_AdvancedFace, anent, ent);
      RWStepShape_RWAdvancedFace tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 4: {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrence, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrence tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 5: {
      DeclareAndCast(StepVisual_AnnotationFillArea, anent, ent);
      RWStepVisual_RWAnnotationFillArea tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 6: {
      DeclareAndCast(StepVisual_AnnotationFillAreaOccurrence, anent, ent);
      RWStepVisual_RWAnnotationFillAreaOccurrence tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 7: {
      DeclareAndCast(StepVisual_AnnotationOccurrence, anent, ent);
      RWStepVisual_RWAnnotationOccurrence tool;
      tool.WriteStep(SW, anent);
    }

    break;
    case 11: {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 12: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 13: {
      DeclareAndCast(StepBasic_ApplicationContext, anent, ent);
      RWStepBasic_RWApplicationContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 14: {
      DeclareAndCast(StepBasic_ApplicationContextElement, anent, ent);
      RWStepBasic_RWApplicationContextElement tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 15: {
      DeclareAndCast(StepBasic_ApplicationProtocolDefinition, anent, ent);
      RWStepBasic_RWApplicationProtocolDefinition tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 16: {
      DeclareAndCast(StepBasic_Approval, anent, ent);
      RWStepBasic_RWApproval tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 18: {
      DeclareAndCast(StepBasic_ApprovalPersonOrganization, anent, ent);
      RWStepBasic_RWApprovalPersonOrganization tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 19: {
      DeclareAndCast(StepBasic_ApprovalRelationship, anent, ent);
      RWStepBasic_RWApprovalRelationship tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 20: {
      DeclareAndCast(StepBasic_ApprovalRole, anent, ent);
      RWStepBasic_RWApprovalRole tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 21: {
      DeclareAndCast(StepBasic_ApprovalStatus, anent, ent);
      RWStepBasic_RWApprovalStatus tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 22: {
      DeclareAndCast(StepVisual_AreaInSet, anent, ent);
      RWStepVisual_RWAreaInSet tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 23: {
      DeclareAndCast(StepAP214_AutoDesignActualDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAndTimeAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 24: {
      DeclareAndCast(StepAP214_AutoDesignActualDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 25: {
      DeclareAndCast(StepAP214_AutoDesignApprovalAssignment, anent, ent);
      RWStepAP214_RWAutoDesignApprovalAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 26: {
      DeclareAndCast(StepAP214_AutoDesignDateAndPersonAssignment, anent, ent);
      RWStepAP214_RWAutoDesignDateAndPersonAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 27: {
      DeclareAndCast(StepAP214_AutoDesignGroupAssignment, anent, ent);
      RWStepAP214_RWAutoDesignGroupAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 28: {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 29: {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 30: {
      DeclareAndCast(StepAP214_AutoDesignOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignOrganizationAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 31: {
      DeclareAndCast(StepAP214_AutoDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 32: {
      DeclareAndCast(StepAP214_AutoDesignPresentedItem, anent, ent);
      RWStepAP214_RWAutoDesignPresentedItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 33: {
      DeclareAndCast(StepAP214_AutoDesignSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignSecurityClassificationAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 35: {
      DeclareAndCast(StepGeom_Axis1Placement, anent, ent);
      RWStepGeom_RWAxis1Placement tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 36: {
      DeclareAndCast(StepGeom_Axis2Placement2d, anent, ent);
      RWStepGeom_RWAxis2Placement2d tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 37: {
      DeclareAndCast(StepGeom_Axis2Placement3d, anent, ent);
      RWStepGeom_RWAxis2Placement3d tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 38: {
      DeclareAndCast(StepGeom_BSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 39: {
      DeclareAndCast(StepGeom_BSplineCurveWithKnots, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnots tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 40: {
      DeclareAndCast(StepGeom_BSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 41: {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnots, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnots tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 42: {
      DeclareAndCast(StepVisual_BackgroundColour, anent, ent);
      RWStepVisual_RWBackgroundColour tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 43: {
      DeclareAndCast(StepGeom_BezierCurve, anent, ent);
      RWStepGeom_RWBezierCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 44: {
      DeclareAndCast(StepGeom_BezierSurface, anent, ent);
      RWStepGeom_RWBezierSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 45: {
      DeclareAndCast(StepShape_Block, anent, ent);
      RWStepShape_RWBlock tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 46: {
      DeclareAndCast(StepShape_BooleanResult, anent, ent);
      RWStepShape_RWBooleanResult tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 47: {
      DeclareAndCast(StepGeom_BoundaryCurve, anent, ent);
      RWStepGeom_RWBoundaryCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 48: {
      DeclareAndCast(StepGeom_BoundedCurve, anent, ent);
      RWStepGeom_RWBoundedCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 49: {
      DeclareAndCast(StepGeom_BoundedSurface, anent, ent);
      RWStepGeom_RWBoundedSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 50: {
      DeclareAndCast(StepShape_BoxDomain, anent, ent);
      RWStepShape_RWBoxDomain tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 51: {
      DeclareAndCast(StepShape_BoxedHalfSpace, anent, ent);
      RWStepShape_RWBoxedHalfSpace tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 52: {
      DeclareAndCast(StepShape_BrepWithVoids, anent, ent);
      RWStepShape_RWBrepWithVoids tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 53: {
      DeclareAndCast(StepBasic_CalendarDate, anent, ent);
      RWStepBasic_RWCalendarDate tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 54: {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 55: {
      DeclareAndCast(StepVisual_CameraModel, anent, ent);
      RWStepVisual_RWCameraModel tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 56: {
      DeclareAndCast(StepVisual_CameraModelD2, anent, ent);
      RWStepVisual_RWCameraModelD2 tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 57: {
      DeclareAndCast(StepVisual_CameraModelD3, anent, ent);
      RWStepVisual_RWCameraModelD3 tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 58: {
      DeclareAndCast(StepVisual_CameraUsage, anent, ent);
      RWStepVisual_RWCameraUsage tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 59: {
      DeclareAndCast(StepGeom_CartesianPoint, anent, ent);
      RWStepGeom_RWCartesianPoint tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 60: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 61: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator3d, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator3d tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 62: {
      DeclareAndCast(StepGeom_Circle, anent, ent);
      RWStepGeom_RWCircle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 63: {
      DeclareAndCast(StepShape_ClosedShell, anent, ent);
      RWStepShape_RWClosedShell tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 64: {
      DeclareAndCast(StepVisual_Colour, anent, ent);
      RWStepVisual_RWColour tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 65: {
      DeclareAndCast(StepVisual_ColourRgb, anent, ent);
      RWStepVisual_RWColourRgb tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 66: {
      DeclareAndCast(StepVisual_ColourSpecification, anent, ent);
      RWStepVisual_RWColourSpecification tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 67: {
      DeclareAndCast(StepGeom_CompositeCurve, anent, ent);
      RWStepGeom_RWCompositeCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 68: {
      DeclareAndCast(StepGeom_CompositeCurveOnSurface, anent, ent);
      RWStepGeom_RWCompositeCurveOnSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 69: {
      DeclareAndCast(StepGeom_CompositeCurveSegment, anent, ent);
      RWStepGeom_RWCompositeCurveSegment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 70: {
      DeclareAndCast(StepVisual_CompositeText, anent, ent);
      RWStepVisual_RWCompositeText tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 73: {
      DeclareAndCast(StepVisual_CompositeTextWithExtent, anent, ent);
      RWStepVisual_RWCompositeTextWithExtent tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 74: {
      DeclareAndCast(StepGeom_Conic, anent, ent);
      RWStepGeom_RWConic tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 75: {
      DeclareAndCast(StepGeom_ConicalSurface, anent, ent);
      RWStepGeom_RWConicalSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 76: {
      DeclareAndCast(StepShape_ConnectedFaceSet, anent, ent);
      RWStepShape_RWConnectedFaceSet tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 77: {
      DeclareAndCast(StepVisual_ContextDependentInvisibility, anent, ent);
      RWStepVisual_RWContextDependentInvisibility tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 78: {
      DeclareAndCast(StepVisual_ContextDependentOverRidingStyledItem, anent, ent);
      RWStepVisual_RWContextDependentOverRidingStyledItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 79: {
      DeclareAndCast(StepBasic_ConversionBasedUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 80: {
      DeclareAndCast(StepBasic_CoordinatedUniversalTimeOffset, anent, ent);
      RWStepBasic_RWCoordinatedUniversalTimeOffset tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 82: {
      DeclareAndCast(StepShape_CsgShapeRepresentation, anent, ent);
      RWStepShape_RWCsgShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 83: {
      DeclareAndCast(StepShape_CsgSolid, anent, ent);
      RWStepShape_RWCsgSolid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 84: {
      DeclareAndCast(StepGeom_Curve, anent, ent);
      RWStepGeom_RWCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 85: {
      DeclareAndCast(StepGeom_CurveBoundedSurface, anent, ent);
      RWStepGeom_RWCurveBoundedSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 86: {
      DeclareAndCast(StepGeom_CurveReplica, anent, ent);
      RWStepGeom_RWCurveReplica tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 87: {
      DeclareAndCast(StepVisual_CurveStyle, anent, ent);
      RWStepVisual_RWCurveStyle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 88: {
      DeclareAndCast(StepVisual_CurveStyleFont, anent, ent);
      RWStepVisual_RWCurveStyleFont tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 89: {
      DeclareAndCast(StepVisual_CurveStyleFontPattern, anent, ent);
      RWStepVisual_RWCurveStyleFontPattern tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 90: {
      DeclareAndCast(StepGeom_CylindricalSurface, anent, ent);
      RWStepGeom_RWCylindricalSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 91: {
      DeclareAndCast(StepBasic_Date, anent, ent);
      RWStepBasic_RWDate tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 92: {
      DeclareAndCast(StepBasic_DateAndTime, anent, ent);
      RWStepBasic_RWDateAndTime tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 95: {
      DeclareAndCast(StepBasic_DateRole, anent, ent);
      RWStepBasic_RWDateRole tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 96: {
      DeclareAndCast(StepBasic_DateTimeRole, anent, ent);
      RWStepBasic_RWDateTimeRole tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 98: {
      DeclareAndCast(StepRepr_DefinitionalRepresentation, anent, ent);
      RWStepRepr_RWDefinitionalRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 99: {
      DeclareAndCast(StepGeom_DegeneratePcurve, anent, ent);
      RWStepGeom_RWDegeneratePcurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 100: {
      DeclareAndCast(StepGeom_DegenerateToroidalSurface, anent, ent);
      RWStepGeom_RWDegenerateToroidalSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 101: {
      DeclareAndCast(StepRepr_DescriptiveRepresentationItem, anent, ent);
      RWStepRepr_RWDescriptiveRepresentationItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 104: {
      DeclareAndCast(StepBasic_DimensionalExponents, anent, ent);
      RWStepBasic_RWDimensionalExponents tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 105: {
      DeclareAndCast(StepGeom_Direction, anent, ent);
      RWStepGeom_RWDirection tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 106: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 107: {
      DeclareAndCast(StepVisual_DraughtingCallout, anent, ent);
      RWStepVisual_RWDraughtingCallout tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 108: {
      DeclareAndCast(StepVisual_DraughtingPreDefinedColour, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedColour tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 109: {
      DeclareAndCast(StepVisual_DraughtingPreDefinedCurveFont, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedCurveFont tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 115: {
      DeclareAndCast(StepShape_Edge, anent, ent);
      RWStepShape_RWEdge tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 116: {
      DeclareAndCast(StepShape_EdgeCurve, anent, ent);
      RWStepShape_RWEdgeCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 117: {
      DeclareAndCast(StepShape_EdgeLoop, anent, ent);
      RWStepShape_RWEdgeLoop tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 118: {
      DeclareAndCast(StepGeom_ElementarySurface, anent, ent);
      RWStepGeom_RWElementarySurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 119: {
      DeclareAndCast(StepGeom_Ellipse, anent, ent);
      RWStepGeom_RWEllipse tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 120: {
      DeclareAndCast(StepGeom_EvaluatedDegeneratePcurve, anent, ent);
      RWStepGeom_RWEvaluatedDegeneratePcurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 121: {
      DeclareAndCast(StepBasic_ExternalSource, anent, ent);
      RWStepBasic_RWExternalSource tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 122: {
      DeclareAndCast(StepVisual_ExternallyDefinedCurveFont, anent, ent);
      RWStepVisual_RWExternallyDefinedCurveFont tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 124:
    case 126: {
      DeclareAndCast(StepBasic_ExternallyDefinedItem, anent, ent);
      RWStepBasic_RWExternallyDefinedItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 128: {
      DeclareAndCast(StepShape_ExtrudedAreaSolid, anent, ent);
      RWStepShape_RWExtrudedAreaSolid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 129: {
      DeclareAndCast(StepShape_Face, anent, ent);
      RWStepShape_RWFace tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 131: {
      DeclareAndCast(StepShape_FaceBound, anent, ent);
      RWStepShape_RWFaceBound tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 132: {
      DeclareAndCast(StepShape_FaceOuterBound, anent, ent);
      RWStepShape_RWFaceOuterBound tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 133: {
      DeclareAndCast(StepShape_FaceSurface, anent, ent);
      RWStepShape_RWFaceSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 134: {
      DeclareAndCast(StepShape_FacetedBrep, anent, ent);
      RWStepShape_RWFacetedBrep tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 135: {
      DeclareAndCast(StepShape_FacetedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWFacetedBrepShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 136: {
      DeclareAndCast(StepVisual_FillAreaStyle, anent, ent);
      RWStepVisual_RWFillAreaStyle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 137: {
      DeclareAndCast(StepVisual_FillAreaStyleColour, anent, ent);
      RWStepVisual_RWFillAreaStyleColour tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 141: {
      DeclareAndCast(StepRepr_FunctionallyDefinedTransformation, anent, ent);
      RWStepRepr_RWFunctionallyDefinedTransformation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 142: {
      DeclareAndCast(StepShape_GeometricCurveSet, anent, ent);
      RWStepShape_RWGeometricCurveSet tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 143: {
      DeclareAndCast(StepGeom_GeometricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 144: {
      DeclareAndCast(StepGeom_GeometricRepresentationItem, anent, ent);
      RWStepGeom_RWGeometricRepresentationItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 145: {
      DeclareAndCast(StepShape_GeometricSet, anent, ent);
      RWStepShape_RWGeometricSet tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 146: {
      DeclareAndCast(StepShape_GeometricallyBoundedSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 147: {
      DeclareAndCast(StepShape_GeometricallyBoundedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 148: {
      DeclareAndCast(StepRepr_GlobalUncertaintyAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUncertaintyAssignedContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 149: {
      DeclareAndCast(StepRepr_GlobalUnitAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUnitAssignedContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 150: {
      DeclareAndCast(StepBasic_Group, anent, ent);
      RWStepBasic_RWGroup tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 152: {
      DeclareAndCast(StepBasic_GroupRelationship, anent, ent);
      RWStepBasic_RWGroupRelationship tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 153: {
      DeclareAndCast(StepShape_HalfSpaceSolid, anent, ent);
      RWStepShape_RWHalfSpaceSolid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 154: {
      DeclareAndCast(StepGeom_Hyperbola, anent, ent);
      RWStepGeom_RWHyperbola tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 155: {
      DeclareAndCast(StepGeom_IntersectionCurve, anent, ent);
      RWStepGeom_RWIntersectionCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 156: {
      DeclareAndCast(StepVisual_Invisibility, anent, ent);
      RWStepVisual_RWInvisibility tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 157: {
      DeclareAndCast(StepBasic_LengthMeasureWithUnit, anent, ent);
      RWStepBasic_RWLengthMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 158: {
      DeclareAndCast(StepBasic_LengthUnit, anent, ent);
      RWStepBasic_RWLengthUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 159: {
      DeclareAndCast(StepGeom_Line, anent, ent);
      RWStepGeom_RWLine tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 160: {
      DeclareAndCast(StepBasic_LocalTime, anent, ent);
      RWStepBasic_RWLocalTime tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 161: {
      DeclareAndCast(StepShape_Loop, anent, ent);
      RWStepShape_RWLoop tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 162: {
      DeclareAndCast(StepShape_ManifoldSolidBrep, anent, ent);
      RWStepShape_RWManifoldSolidBrep tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 163: {
      DeclareAndCast(StepShape_ManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWManifoldSurfaceShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 164: {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 165: {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 166: {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationArea, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationArea tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 167: {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationRepresentation, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 169: {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 171: {
      DeclareAndCast(StepGeom_OffsetCurve3d, anent, ent);
      RWStepGeom_RWOffsetCurve3d tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 172: {
      DeclareAndCast(StepGeom_OffsetSurface, anent, ent);
      RWStepGeom_RWOffsetSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 174: {
      DeclareAndCast(StepShape_OpenShell, anent, ent);
      RWStepShape_RWOpenShell tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 175: {
      DeclareAndCast(StepBasic_OrdinalDate, anent, ent);
      RWStepBasic_RWOrdinalDate tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 176: {
      DeclareAndCast(StepBasic_Organization, anent, ent);
      RWStepBasic_RWOrganization tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 178: {
      DeclareAndCast(StepBasic_OrganizationRole, anent, ent);
      RWStepBasic_RWOrganizationRole tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 179: {
      DeclareAndCast(StepBasic_OrganizationalAddress, anent, ent);
      RWStepBasic_RWOrganizationalAddress tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 180: {
      DeclareAndCast(StepShape_OrientedClosedShell, anent, ent);
      RWStepShape_RWOrientedClosedShell tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 181: {
      DeclareAndCast(StepShape_OrientedEdge, anent, ent);
      RWStepShape_RWOrientedEdge tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 182: {
      DeclareAndCast(StepShape_OrientedFace, anent, ent);
      RWStepShape_RWOrientedFace tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 183: {
      DeclareAndCast(StepShape_OrientedOpenShell, anent, ent);
      RWStepShape_RWOrientedOpenShell tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 184: {
      DeclareAndCast(StepShape_OrientedPath, anent, ent);
      RWStepShape_RWOrientedPath tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 185: {
      DeclareAndCast(StepGeom_OuterBoundaryCurve, anent, ent);
      RWStepGeom_RWOuterBoundaryCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 186: {
      DeclareAndCast(StepVisual_OverRidingStyledItem, anent, ent);
      RWStepVisual_RWOverRidingStyledItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 187: {
      DeclareAndCast(StepGeom_Parabola, anent, ent);
      RWStepGeom_RWParabola tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 188: {
      DeclareAndCast(StepRepr_ParametricRepresentationContext, anent, ent);
      RWStepRepr_RWParametricRepresentationContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 189: {
      DeclareAndCast(StepShape_Path, anent, ent);
      RWStepShape_RWPath tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 190: {
      DeclareAndCast(StepGeom_Pcurve, anent, ent);
      RWStepGeom_RWPcurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 191: {
      DeclareAndCast(StepBasic_Person, anent, ent);
      RWStepBasic_RWPerson tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 192: {
      DeclareAndCast(StepBasic_PersonAndOrganization, anent, ent);
      RWStepBasic_RWPersonAndOrganization tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 194: {
      DeclareAndCast(StepBasic_PersonAndOrganizationRole, anent, ent);
      RWStepBasic_RWPersonAndOrganizationRole tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 195: {
      DeclareAndCast(StepBasic_PersonalAddress, anent, ent);
      RWStepBasic_RWPersonalAddress tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 196: {
      DeclareAndCast(StepGeom_Placement, anent, ent);
      RWStepGeom_RWPlacement tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 197: {
      DeclareAndCast(StepVisual_PlanarBox, anent, ent);
      RWStepVisual_RWPlanarBox tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 198: {
      DeclareAndCast(StepVisual_PlanarExtent, anent, ent);
      RWStepVisual_RWPlanarExtent tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 199: {
      DeclareAndCast(StepGeom_Plane, anent, ent);
      RWStepGeom_RWPlane tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 200: {
      DeclareAndCast(StepBasic_PlaneAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWPlaneAngleMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 201: {
      DeclareAndCast(StepBasic_PlaneAngleUnit, anent, ent);
      RWStepBasic_RWPlaneAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 202: {
      DeclareAndCast(StepGeom_Point, anent, ent);
      RWStepGeom_RWPoint tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 203: {
      DeclareAndCast(StepGeom_PointOnCurve, anent, ent);
      RWStepGeom_RWPointOnCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 204: {
      DeclareAndCast(StepGeom_PointOnSurface, anent, ent);
      RWStepGeom_RWPointOnSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 205: {
      DeclareAndCast(StepGeom_PointReplica, anent, ent);
      RWStepGeom_RWPointReplica tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 206: {
      DeclareAndCast(StepVisual_PointStyle, anent, ent);
      RWStepVisual_RWPointStyle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 207: {
      DeclareAndCast(StepShape_PolyLoop, anent, ent);
      RWStepShape_RWPolyLoop tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 208: {
      DeclareAndCast(StepGeom_Polyline, anent, ent);
      RWStepGeom_RWPolyline tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 209: {
      DeclareAndCast(StepVisual_PreDefinedColour, anent, ent);
      RWStepVisual_RWPreDefinedColour tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 210: {
      DeclareAndCast(StepVisual_PreDefinedCurveFont, anent, ent);
      RWStepVisual_RWPreDefinedCurveFont tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 211:
    case 213: {
      DeclareAndCast(StepVisual_PreDefinedItem, anent, ent);
      RWStepVisual_RWPreDefinedItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 214: {
      DeclareAndCast(StepVisual_PresentationArea, anent, ent);
      RWStepVisual_RWPresentationArea tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 215: {
      DeclareAndCast(StepVisual_PresentationLayerAssignment, anent, ent);
      RWStepVisual_RWPresentationLayerAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 216: {
      DeclareAndCast(StepVisual_PresentationRepresentation, anent, ent);
      RWStepVisual_RWPresentationRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 217: {
      DeclareAndCast(StepVisual_PresentationSet, anent, ent);
      RWStepVisual_RWPresentationSet tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 218: {
      DeclareAndCast(StepVisual_PresentationSize, anent, ent);
      RWStepVisual_RWPresentationSize tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 219: {
      DeclareAndCast(StepVisual_PresentationStyleAssignment, anent, ent);
      RWStepVisual_RWPresentationStyleAssignment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 220: {
      DeclareAndCast(StepVisual_PresentationStyleByContext, anent, ent);
      RWStepVisual_RWPresentationStyleByContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 221: {
      DeclareAndCast(StepVisual_PresentationView, anent, ent);
      RWStepVisual_RWPresentationView tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 223: {
      DeclareAndCast(StepBasic_Product, anent, ent);
      RWStepBasic_RWProduct tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 224: {
      DeclareAndCast(StepBasic_ProductCategory, anent, ent);
      RWStepBasic_RWProductCategory tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 225: {
      DeclareAndCast(StepBasic_ProductContext, anent, ent);
      RWStepBasic_RWProductContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 227: {
      DeclareAndCast(StepBasic_ProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 228: {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 229: {
      DeclareAndCast(StepBasic_ProductDefinitionFormation, anent, ent);
      RWStepBasic_RWProductDefinitionFormation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 230: {
      DeclareAndCast(StepBasic_ProductDefinitionFormationWithSpecifiedSource, anent, ent);
      RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 231: {
      DeclareAndCast(StepRepr_ProductDefinitionShape, anent, ent);
      RWStepRepr_RWProductDefinitionShape tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 232: {
      DeclareAndCast(StepBasic_ProductRelatedProductCategory, anent, ent);
      RWStepBasic_RWProductRelatedProductCategory tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 233: {
      DeclareAndCast(StepBasic_ProductType, anent, ent);
      RWStepBasic_RWProductType tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 234: {
      DeclareAndCast(StepRepr_PropertyDefinition, anent, ent);
      RWStepRepr_RWPropertyDefinition tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 235: {
      DeclareAndCast(StepRepr_PropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWPropertyDefinitionRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 236: {
      DeclareAndCast(StepGeom_QuasiUniformCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 237: {
      DeclareAndCast(StepGeom_QuasiUniformSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 238: {
      DeclareAndCast(StepBasic_RatioMeasureWithUnit, anent, ent);
      RWStepBasic_RWRatioMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 239: {
      DeclareAndCast(StepGeom_RationalBSplineCurve, anent, ent);
      RWStepGeom_RWRationalBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 240: {
      DeclareAndCast(StepGeom_RationalBSplineSurface, anent, ent);
      RWStepGeom_RWRationalBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 241: {
      DeclareAndCast(StepGeom_RectangularCompositeSurface, anent, ent);
      RWStepGeom_RWRectangularCompositeSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 242: {
      DeclareAndCast(StepGeom_RectangularTrimmedSurface, anent, ent);
      RWStepGeom_RWRectangularTrimmedSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 243: {
      DeclareAndCast(StepAP214_RepItemGroup, anent, ent);
      RWStepAP214_RWRepItemGroup tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 244: {
      DeclareAndCast(StepGeom_ReparametrisedCompositeCurveSegment, anent, ent);
      RWStepGeom_RWReparametrisedCompositeCurveSegment tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 245: {
      DeclareAndCast(StepRepr_Representation, anent, ent);
      RWStepRepr_RWRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 246: {
      DeclareAndCast(StepRepr_RepresentationContext, anent, ent);
      RWStepRepr_RWRepresentationContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 247: {
      DeclareAndCast(StepRepr_RepresentationItem, anent, ent);
      RWStepRepr_RWRepresentationItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 248: {
      DeclareAndCast(StepRepr_RepresentationMap, anent, ent);
      RWStepRepr_RWRepresentationMap tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 249: {
      DeclareAndCast(StepRepr_RepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 250: {
      DeclareAndCast(StepShape_RevolvedAreaSolid, anent, ent);
      RWStepShape_RWRevolvedAreaSolid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 251: {
      DeclareAndCast(StepShape_RightAngularWedge, anent, ent);
      RWStepShape_RWRightAngularWedge tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 252: {
      DeclareAndCast(StepShape_RightCircularCone, anent, ent);
      RWStepShape_RWRightCircularCone tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 253: {
      DeclareAndCast(StepShape_RightCircularCylinder, anent, ent);
      RWStepShape_RWRightCircularCylinder tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 254: {
      DeclareAndCast(StepGeom_SeamCurve, anent, ent);
      RWStepGeom_RWSeamCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 255: {
      DeclareAndCast(StepBasic_SecurityClassification, anent, ent);
      RWStepBasic_RWSecurityClassification tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 257: {
      DeclareAndCast(StepBasic_SecurityClassificationLevel, anent, ent);
      RWStepBasic_RWSecurityClassificationLevel tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 258: {
      DeclareAndCast(StepRepr_ShapeAspect, anent, ent);
      RWStepRepr_RWShapeAspect tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 259: {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 260: {
      DeclareAndCast(StepRepr_ShapeAspectTransition, anent, ent);
      RWStepRepr_RWShapeAspectTransition tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 261: {
      DeclareAndCast(StepShape_ShapeDefinitionRepresentation, anent, ent);
      RWStepShape_RWShapeDefinitionRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 262: {
      DeclareAndCast(StepShape_ShapeRepresentation, anent, ent);
      RWStepShape_RWShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 263: {
      DeclareAndCast(StepShape_ShellBasedSurfaceModel, anent, ent);
      RWStepShape_RWShellBasedSurfaceModel tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 264: {
      DeclareAndCast(StepBasic_SiUnit, anent, ent);
      RWStepBasic_RWSiUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 265: {
      DeclareAndCast(StepBasic_SolidAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWSolidAngleMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 266: {
      DeclareAndCast(StepShape_SolidModel, anent, ent);
      RWStepShape_RWSolidModel tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 267: {
      DeclareAndCast(StepShape_SolidReplica, anent, ent);
      RWStepShape_RWSolidReplica tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 268: {
      DeclareAndCast(StepShape_Sphere, anent, ent);
      RWStepShape_RWSphere tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 269: {
      DeclareAndCast(StepGeom_SphericalSurface, anent, ent);
      RWStepGeom_RWSphericalSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 270: {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 271: {
      DeclareAndCast(StepGeom_Surface, anent, ent);
      RWStepGeom_RWSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 272: {
      DeclareAndCast(StepGeom_SurfaceCurve, anent, ent);
      RWStepGeom_RWSurfaceCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 273: {
      DeclareAndCast(StepGeom_SurfaceOfLinearExtrusion, anent, ent);
      RWStepGeom_RWSurfaceOfLinearExtrusion tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 274: {
      DeclareAndCast(StepGeom_SurfaceOfRevolution, anent, ent);
      RWStepGeom_RWSurfaceOfRevolution tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 275: {
      DeclareAndCast(StepGeom_SurfacePatch, anent, ent);
      RWStepGeom_RWSurfacePatch tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 276: {
      DeclareAndCast(StepGeom_SurfaceReplica, anent, ent);
      RWStepGeom_RWSurfaceReplica tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 277: {
      DeclareAndCast(StepVisual_SurfaceSideStyle, anent, ent);
      RWStepVisual_RWSurfaceSideStyle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 278: {
      DeclareAndCast(StepVisual_SurfaceStyleBoundary, anent, ent);
      RWStepVisual_RWSurfaceStyleBoundary tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 279: {
      DeclareAndCast(StepVisual_SurfaceStyleControlGrid, anent, ent);
      RWStepVisual_RWSurfaceStyleControlGrid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 280: {
      DeclareAndCast(StepVisual_SurfaceStyleFillArea, anent, ent);
      RWStepVisual_RWSurfaceStyleFillArea tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 281: {
      DeclareAndCast(StepVisual_SurfaceStyleParameterLine, anent, ent);
      RWStepVisual_RWSurfaceStyleParameterLine tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 282: {
      DeclareAndCast(StepVisual_SurfaceStyleSegmentationCurve, anent, ent);
      RWStepVisual_RWSurfaceStyleSegmentationCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 283: {
      DeclareAndCast(StepVisual_SurfaceStyleSilhouette, anent, ent);
      RWStepVisual_RWSurfaceStyleSilhouette tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 284: {
      DeclareAndCast(StepVisual_SurfaceStyleUsage, anent, ent);
      RWStepVisual_RWSurfaceStyleUsage tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 285: {
      DeclareAndCast(StepShape_SweptAreaSolid, anent, ent);
      RWStepShape_RWSweptAreaSolid tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 286: {
      DeclareAndCast(StepGeom_SweptSurface, anent, ent);
      RWStepGeom_RWSweptSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 292: {
      DeclareAndCast(StepVisual_Template, anent, ent);
      RWStepVisual_RWTemplate tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 293: {
      DeclareAndCast(StepVisual_TemplateInstance, anent, ent);
      RWStepVisual_RWTemplateInstance tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 295: {
      DeclareAndCast(StepVisual_TextLiteral, anent, ent);
      RWStepVisual_RWTextLiteral tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 300: {
      DeclareAndCast(StepVisual_TextStyle, anent, ent);
      RWStepVisual_RWTextStyle tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 301: {
      DeclareAndCast(StepVisual_TextStyleForDefinedFont, anent, ent);
      RWStepVisual_RWTextStyleForDefinedFont tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 302: {
      DeclareAndCast(StepVisual_TextStyleWithBoxCharacteristics, anent, ent);
      RWStepVisual_RWTextStyleWithBoxCharacteristics tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 304: {
      DeclareAndCast(StepShape_TopologicalRepresentationItem, anent, ent);
      RWStepShape_RWTopologicalRepresentationItem tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 305: {
      DeclareAndCast(StepGeom_ToroidalSurface, anent, ent);
      RWStepGeom_RWToroidalSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 306: {
      DeclareAndCast(StepShape_Torus, anent, ent);
      RWStepShape_RWTorus tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 307: {
      DeclareAndCast(StepShape_TransitionalShapeRepresentation, anent, ent);
      RWStepShape_RWTransitionalShapeRepresentation tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 308: {
      DeclareAndCast(StepGeom_TrimmedCurve, anent, ent);
      RWStepGeom_RWTrimmedCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 310: {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 311: {
      DeclareAndCast(StepGeom_UniformCurve, anent, ent);
      RWStepGeom_RWUniformCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 312: {
      DeclareAndCast(StepGeom_UniformSurface, anent, ent);
      RWStepGeom_RWUniformSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 313: {
      DeclareAndCast(StepGeom_Vector, anent, ent);
      RWStepGeom_RWVector tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 314: {
      DeclareAndCast(StepShape_Vertex, anent, ent);
      RWStepShape_RWVertex tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 315: {
      DeclareAndCast(StepShape_VertexLoop, anent, ent);
      RWStepShape_RWVertexLoop tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 316: {
      DeclareAndCast(StepShape_VertexPoint, anent, ent);
      RWStepShape_RWVertexPoint tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 317: {
      DeclareAndCast(StepVisual_ViewVolume, anent, ent);
      RWStepVisual_RWViewVolume tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 318: {
      DeclareAndCast(StepBasic_WeekOfYearAndDayDate, anent, ent);
      RWStepBasic_RWWeekOfYearAndDayDate tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 319: {
      DeclareAndCast(StepGeom_UniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWUniformCurveAndRationalBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 320: {
      DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 321: {
      DeclareAndCast(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 322: {
      DeclareAndCast(StepGeom_BezierCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBezierCurveAndRationalBSplineCurve tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 323: {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 324: {
      DeclareAndCast(StepGeom_UniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 325: {
      DeclareAndCast(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 326: {
      DeclareAndCast(StepGeom_BezierSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 327: {
      DeclareAndCast(StepBasic_SiUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWSiUnitAndLengthUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 328: {
      DeclareAndCast(StepBasic_SiUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndPlaneAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 329: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndLengthUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 330: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 331: {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext,
                     anent,
                     ent);
      RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;
    case 332: {
      DeclareAndCast(StepShape_LoopAndPath, anent, ent);
      RWStepShape_RWLoopAndPath tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }

    break;

      // ------------
      // Added by FMA
      // ------------

    case 333: {
      DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, anent, ent);
      RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;

    case 334: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;

    case 335: {
      DeclareAndCast(StepBasic_SiUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndSolidAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;
    case 336: {
      DeclareAndCast(StepBasic_SolidAngleUnit, anent, ent);
      RWStepBasic_RWSolidAngleUnit tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;
    case 337: {
      DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids, anent, ent);
      RWStepShape_RWFacetedBrepAndBrepWithVoids tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;
    case 338: {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,
                     anent,
                     ent);
      RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;
    case 339: {
      DeclareAndCast(StepBasic_MechanicalContext, anent, ent);
      RWStepBasic_RWMechanicalContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
    }
    break;

    case 340: // added by CKY : DesignContext cf ProductDefinitionContext
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      //      if (anent.IsNull()) return;
      tool.WriteStep(SW, anent);
      break;
    }

      // -----------
      // Added for Rev4
      // -----------

    case 341: // TimeMeasureWithUnit
    {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 342:
    case 343: // RatioUnit, TimeUnit
    {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 344: {
      DeclareAndCast(StepBasic_SiUnitAndRatioUnit, anent, ent);
      RWStepBasic_RWSiUnitAndRatioUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 345: {
      DeclareAndCast(StepBasic_SiUnitAndTimeUnit, anent, ent);
      RWStepBasic_RWSiUnitAndTimeUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 346: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndRatioUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndRatioUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 347: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndTimeUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndTimeUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 348: // ApprovalDateTime
    {
      DeclareAndCast(StepBasic_ApprovalDateTime, anent, ent);
      RWStepBasic_RWApprovalDateTime tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 349: // CameraImage 2d and 3d
    case 350: {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 351: {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 352: {
      DeclareAndCast(StepBasic_DerivedUnit, anent, ent);
      RWStepBasic_RWDerivedUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 353: {
      DeclareAndCast(StepBasic_DerivedUnitElement, anent, ent);
      RWStepBasic_RWDerivedUnitElement tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 354: {
      DeclareAndCast(StepRepr_ItemDefinedTransformation, anent, ent);
      RWStepRepr_RWItemDefinedTransformation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 355: {
      DeclareAndCast(StepVisual_PresentedItemRepresentation, anent, ent);
      RWStepVisual_RWPresentedItemRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 356: {
      DeclareAndCast(StepVisual_PresentationLayerUsage, anent, ent);
      RWStepVisual_RWPresentationLayerUsage tool;
      tool.WriteStep(SW, anent);
    }
    break;

      //: n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
    case 358: {
      DeclareAndCast(StepGeom_SurfaceCurveAndBoundedCurve, anent, ent);
      RWStepGeom_RWSurfaceCurveAndBoundedCurve tool;
      tool.WriteStep(SW, anent);
      break;
    }

      //  AP214 : CC1 -> CC2
    case 366: {
      DeclareAndCast(StepAP214_AutoDesignDocumentReference, anent, ent);
      RWStepAP214_RWAutoDesignDocumentReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 367:
    case 368: {
      DeclareAndCast(StepBasic_Document, anent, ent);
      RWStepBasic_RWDocument tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 369: {
      DeclareAndCast(StepBasic_DocumentRelationship, anent, ent);
      RWStepBasic_RWDocumentRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 370: {
      DeclareAndCast(StepBasic_DocumentType, anent, ent);
      RWStepBasic_RWDocumentType tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 371: {
      DeclareAndCast(StepBasic_DocumentUsageConstraint, anent, ent);
      RWStepBasic_RWDocumentUsageConstraint tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 372: {
      DeclareAndCast(StepBasic_Effectivity, anent, ent);
      RWStepBasic_RWEffectivity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 373: {
      DeclareAndCast(StepBasic_ProductDefinitionEffectivity, anent, ent);
      RWStepBasic_RWProductDefinitionEffectivity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 374: {
      DeclareAndCast(StepBasic_ProductDefinitionRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 375: {
      DeclareAndCast(StepBasic_ProductDefinitionWithAssociatedDocuments, anent, ent);
      RWStepBasic_RWProductDefinitionWithAssociatedDocuments tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 376: {
      DeclareAndCast(StepBasic_PhysicallyModeledProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 377: {
      DeclareAndCast(StepRepr_ProductDefinitionUsage, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 378: {
      DeclareAndCast(StepRepr_MakeFromUsageOption, anent, ent);
      RWStepRepr_RWMakeFromUsageOption tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 379:
    case 380:
    case 381: {
      DeclareAndCast(StepRepr_AssemblyComponentUsage, anent, ent);
      RWStepRepr_RWAssemblyComponentUsage tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 382: {
      DeclareAndCast(StepRepr_QuantifiedAssemblyComponentUsage, anent, ent);
      RWStepRepr_RWQuantifiedAssemblyComponentUsage tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 383: {
      DeclareAndCast(StepRepr_SpecifiedHigherUsageOccurrence, anent, ent);
      RWStepRepr_RWSpecifiedHigherUsageOccurrence tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 384: {
      DeclareAndCast(StepRepr_AssemblyComponentUsageSubstitute, anent, ent);
      RWStepRepr_RWAssemblyComponentUsageSubstitute tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 385: {
      DeclareAndCast(StepRepr_SuppliedPartRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 386: {
      DeclareAndCast(StepRepr_ExternallyDefinedRepresentation, anent, ent);
      RWStepRepr_RWRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 387: {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 388: {
      DeclareAndCast(StepRepr_RepresentationRelationshipWithTransformation, anent, ent);
      RWStepRepr_RWRepresentationRelationshipWithTransformation tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 389: {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation, anent, ent);
      RWStepRepr_RWShapeRepresentationRelationshipWithTransformation tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 390: {
      DeclareAndCast(StepRepr_MaterialDesignation, anent, ent);
      RWStepRepr_RWMaterialDesignation tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 391: {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation, anent, ent);
      RWStepShape_RWContextDependentShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;

      //: S4134: Added from CD to DIS
    case 392: {
      DeclareAndCast(StepAP214_AppliedDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAppliedDateAndTimeAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 393: {
      DeclareAndCast(StepAP214_AppliedDateAssignment, anent, ent);
      RWStepAP214_RWAppliedDateAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 394: {
      DeclareAndCast(StepAP214_AppliedApprovalAssignment, anent, ent);
      RWStepAP214_RWAppliedApprovalAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 395: {
      DeclareAndCast(StepAP214_AppliedGroupAssignment, anent, ent);
      RWStepAP214_RWAppliedGroupAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 396: {
      DeclareAndCast(StepAP214_AppliedOrganizationAssignment, anent, ent);
      RWStepAP214_RWAppliedOrganizationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 397: {
      DeclareAndCast(StepAP214_AppliedPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAppliedPersonAndOrganizationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 398: {
      DeclareAndCast(StepAP214_AppliedPresentedItem, anent, ent);
      RWStepAP214_RWAppliedPresentedItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 399: {
      DeclareAndCast(StepAP214_AppliedSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAppliedSecurityClassificationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 400: {
      DeclareAndCast(StepAP214_AppliedDocumentReference, anent, ent);
      RWStepAP214_RWAppliedDocumentReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 401: {
      DeclareAndCast(StepBasic_DocumentFile, anent, ent);
      RWStepBasic_RWDocumentFile tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 402: {
      DeclareAndCast(StepBasic_CharacterizedObject, anent, ent);
      RWStepBasic_RWCharacterizedObject tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 403: {
      DeclareAndCast(StepShape_ExtrudedFaceSolid, anent, ent);
      RWStepShape_RWExtrudedFaceSolid tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 404: {
      DeclareAndCast(StepShape_RevolvedFaceSolid, anent, ent);
      RWStepShape_RWRevolvedFaceSolid tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 405: {
      DeclareAndCast(StepShape_SweptFaceSolid, anent, ent);
      RWStepShape_RWSweptFaceSolid tool;
      tool.WriteStep(SW, anent);
    }
    break;

    // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
    case 406: {
      DeclareAndCast(StepRepr_MeasureRepresentationItem, anent, ent);
      RWStepRepr_RWMeasureRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 407: {
      DeclareAndCast(StepBasic_AreaUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 408: {
      DeclareAndCast(StepBasic_VolumeUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 409: {
      DeclareAndCast(StepBasic_SiUnitAndAreaUnit, anent, ent);
      RWStepBasic_RWSiUnitAndAreaUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 410: {
      DeclareAndCast(StepBasic_SiUnitAndVolumeUnit, anent, ent);
      RWStepBasic_RWSiUnitAndVolumeUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 411: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndAreaUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndAreaUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 412: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndVolumeUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndVolumeUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;

    // Added by ABV 10.11.99 for AP203
    case 413: {
      DeclareAndCast(StepBasic_Action, anent, ent);
      RWStepBasic_RWAction tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 414: {
      DeclareAndCast(StepBasic_ActionAssignment, anent, ent);
      RWStepBasic_RWActionAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 415: {
      DeclareAndCast(StepBasic_ActionMethod, anent, ent);
      RWStepBasic_RWActionMethod tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 416: {
      DeclareAndCast(StepBasic_ActionRequestAssignment, anent, ent);
      RWStepBasic_RWActionRequestAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 417: {
      DeclareAndCast(StepAP203_CcDesignApproval, anent, ent);
      RWStepAP203_RWCcDesignApproval tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 418: {
      DeclareAndCast(StepAP203_CcDesignCertification, anent, ent);
      RWStepAP203_RWCcDesignCertification tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 419: {
      DeclareAndCast(StepAP203_CcDesignContract, anent, ent);
      RWStepAP203_RWCcDesignContract tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 420: {
      DeclareAndCast(StepAP203_CcDesignDateAndTimeAssignment, anent, ent);
      RWStepAP203_RWCcDesignDateAndTimeAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 421: {
      DeclareAndCast(StepAP203_CcDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP203_RWCcDesignPersonAndOrganizationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 422: {
      DeclareAndCast(StepAP203_CcDesignSecurityClassification, anent, ent);
      RWStepAP203_RWCcDesignSecurityClassification tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 423: {
      DeclareAndCast(StepAP203_CcDesignSpecificationReference, anent, ent);
      RWStepAP203_RWCcDesignSpecificationReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 424: {
      DeclareAndCast(StepBasic_Certification, anent, ent);
      RWStepBasic_RWCertification tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 425: {
      DeclareAndCast(StepBasic_CertificationAssignment, anent, ent);
      RWStepBasic_RWCertificationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 426: {
      DeclareAndCast(StepBasic_CertificationType, anent, ent);
      RWStepBasic_RWCertificationType tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 427: {
      DeclareAndCast(StepAP203_Change, anent, ent);
      RWStepAP203_RWChange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 428: {
      DeclareAndCast(StepAP203_ChangeRequest, anent, ent);
      RWStepAP203_RWChangeRequest tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 429: {
      DeclareAndCast(StepRepr_ConfigurationDesign, anent, ent);
      RWStepRepr_RWConfigurationDesign tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 430: {
      DeclareAndCast(StepRepr_ConfigurationEffectivity, anent, ent);
      RWStepRepr_RWConfigurationEffectivity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 431: {
      DeclareAndCast(StepBasic_Contract, anent, ent);
      RWStepBasic_RWContract tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 432: {
      DeclareAndCast(StepBasic_ContractAssignment, anent, ent);
      RWStepBasic_RWContractAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 433: {
      DeclareAndCast(StepBasic_ContractType, anent, ent);
      RWStepBasic_RWContractType tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 434: {
      DeclareAndCast(StepRepr_ProductConcept, anent, ent);
      RWStepRepr_RWProductConcept tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 435: {
      DeclareAndCast(StepBasic_ProductConceptContext, anent, ent);
      RWStepBasic_RWProductConceptContext tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 436: {
      DeclareAndCast(StepAP203_StartRequest, anent, ent);
      RWStepAP203_RWStartRequest tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 437: {
      DeclareAndCast(StepAP203_StartWork, anent, ent);
      RWStepAP203_RWStartWork tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 438: {
      DeclareAndCast(StepBasic_VersionedActionRequest, anent, ent);
      RWStepBasic_RWVersionedActionRequest tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 439: {
      DeclareAndCast(StepBasic_ProductCategoryRelationship, anent, ent);
      RWStepBasic_RWProductCategoryRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 440: {
      DeclareAndCast(StepBasic_ActionRequestSolution, anent, ent);
      RWStepBasic_RWActionRequestSolution tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 441: {
      DeclareAndCast(StepVisual_DraughtingModel, anent, ent);
      RWStepVisual_RWDraughtingModel tool;
      tool.WriteStep(SW, anent);
    }
    break;

    // Added by ABV 18.04.00 for CAX-IF TRJ4
    case 442: {
      DeclareAndCast(StepShape_AngularLocation, anent, ent);
      RWStepShape_RWAngularLocation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 443: {
      DeclareAndCast(StepShape_AngularSize, anent, ent);
      RWStepShape_RWAngularSize tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 444: {
      DeclareAndCast(StepShape_DimensionalCharacteristicRepresentation, anent, ent);
      RWStepShape_RWDimensionalCharacteristicRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 445: {
      DeclareAndCast(StepShape_DimensionalLocation, anent, ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 446: {
      DeclareAndCast(StepShape_DimensionalLocationWithPath, anent, ent);
      RWStepShape_RWDimensionalLocationWithPath tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 447: {
      DeclareAndCast(StepShape_DimensionalSize, anent, ent);
      RWStepShape_RWDimensionalSize tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 448: {
      DeclareAndCast(StepShape_DimensionalSizeWithPath, anent, ent);
      RWStepShape_RWDimensionalSizeWithPath tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 449: {
      DeclareAndCast(StepShape_ShapeDimensionRepresentation, anent, ent);
      RWStepShape_RWShapeDimensionRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;

      // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
    case 450: {
      DeclareAndCast(StepBasic_DocumentRepresentationType, anent, ent);
      RWStepBasic_RWDocumentRepresentationType tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 451: {
      DeclareAndCast(StepBasic_ObjectRole, anent, ent);
      RWStepBasic_RWObjectRole tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 452: {
      DeclareAndCast(StepBasic_RoleAssociation, anent, ent);
      RWStepBasic_RWRoleAssociation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 453: {
      DeclareAndCast(StepBasic_IdentificationRole, anent, ent);
      RWStepBasic_RWIdentificationRole tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 454: {
      DeclareAndCast(StepBasic_IdentificationAssignment, anent, ent);
      RWStepBasic_RWIdentificationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 455: {
      DeclareAndCast(StepBasic_ExternalIdentificationAssignment, anent, ent);
      RWStepBasic_RWExternalIdentificationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 456: {
      DeclareAndCast(StepBasic_EffectivityAssignment, anent, ent);
      RWStepBasic_RWEffectivityAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 457: {
      DeclareAndCast(StepBasic_NameAssignment, anent, ent);
      RWStepBasic_RWNameAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 458: {
      DeclareAndCast(StepBasic_GeneralProperty, anent, ent);
      RWStepBasic_RWGeneralProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 459: {
      DeclareAndCast(StepAP214_Class, anent, ent);
      RWStepAP214_RWClass tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 460: {
      DeclareAndCast(StepAP214_ExternallyDefinedClass, anent, ent);
      RWStepAP214_RWExternallyDefinedClass tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 461: {
      DeclareAndCast(StepAP214_ExternallyDefinedGeneralProperty, anent, ent);
      RWStepAP214_RWExternallyDefinedGeneralProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 462: {
      DeclareAndCast(StepAP214_AppliedExternalIdentificationAssignment, anent, ent);
      RWStepAP214_RWAppliedExternalIdentificationAssignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 463: {
      DeclareAndCast(StepShape_DefinitionalRepresentationAndShapeRepresentation, anent, ent);
      RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;

      // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
    case 470: {
      DeclareAndCast(StepRepr_CompositeShapeAspect, anent, ent);
      RWStepRepr_RWCompositeShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 471: {
      DeclareAndCast(StepRepr_DerivedShapeAspect, anent, ent);
      RWStepRepr_RWDerivedShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 472: {
      DeclareAndCast(StepRepr_Extension, anent, ent);
      RWStepRepr_RWExtension tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 473: // same as DimensionalLocation
    {
      DeclareAndCast(StepShape_DirectedDimensionalLocation, anent, ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 474: {
      DeclareAndCast(StepShape_LimitsAndFits, anent, ent);
      RWStepShape_RWLimitsAndFits tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 475: {
      DeclareAndCast(StepShape_ToleranceValue, anent, ent);
      RWStepShape_RWToleranceValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 476: {
      DeclareAndCast(StepShape_MeasureQualification, anent, ent);
      RWStepShape_RWMeasureQualification tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 477: {
      DeclareAndCast(StepShape_PlusMinusTolerance, anent, ent);
      RWStepShape_RWPlusMinusTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 478: {
      DeclareAndCast(StepShape_PrecisionQualifier, anent, ent);
      RWStepShape_RWPrecisionQualifier tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 479: {
      DeclareAndCast(StepShape_TypeQualifier, anent, ent);
      RWStepShape_RWTypeQualifier tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 480: {
      DeclareAndCast(StepShape_QualifiedRepresentationItem, anent, ent);
      RWStepShape_RWQualifiedRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 481: {
      DeclareAndCast(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem, anent, ent);
      RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 482:
    case 483: {
      DeclareAndCast(StepRepr_CompoundRepresentationItem, anent, ent);
      RWStepRepr_RWCompoundRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 484: // same as ShapeAspectRelationship
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;

      // abv 28.12.01
    case 485: {
      DeclareAndCast(StepShape_CompoundShapeRepresentation, anent, ent);
      RWStepShape_RWCompoundShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 486: {
      DeclareAndCast(StepShape_ConnectedEdgeSet, anent, ent);
      RWStepShape_RWConnectedEdgeSet tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 487: {
      DeclareAndCast(StepShape_ConnectedFaceShapeRepresentation, anent, ent);
      RWStepShape_RWConnectedFaceShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 488: {
      DeclareAndCast(StepShape_EdgeBasedWireframeModel, anent, ent);
      RWStepShape_RWEdgeBasedWireframeModel tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 489: {
      DeclareAndCast(StepShape_EdgeBasedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWEdgeBasedWireframeShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 490: {
      DeclareAndCast(StepShape_FaceBasedSurfaceModel, anent, ent);
      RWStepShape_RWFaceBasedSurfaceModel tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 491: {
      DeclareAndCast(StepShape_NonManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWNonManifoldSurfaceShapeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 492: {
      DeclareAndCast(StepGeom_OrientedSurface, anent, ent);
      RWStepGeom_RWOrientedSurface tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 493: {
      DeclareAndCast(StepShape_Subface, anent, ent);
      RWStepShape_RWSubface tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 494: {
      DeclareAndCast(StepShape_Subedge, anent, ent);
      RWStepShape_RWSubedge tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 495: {
      DeclareAndCast(StepShape_SeamEdge, anent, ent);
      RWStepShape_RWSeamEdge tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 496: {
      DeclareAndCast(StepShape_ConnectedFaceSubSet, anent, ent);
      RWStepShape_RWConnectedFaceSubSet tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 500: {
      DeclareAndCast(StepBasic_EulerAngles, anent, ent);
      RWStepBasic_RWEulerAngles tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 501: {
      DeclareAndCast(StepBasic_MassUnit, anent, ent);
      RWStepBasic_RWMassUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 502: {
      DeclareAndCast(StepBasic_ThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWThermodynamicTemperatureUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 503: {
      DeclareAndCast(StepElement_AnalysisItemWithinRepresentation, anent, ent);
      RWStepElement_RWAnalysisItemWithinRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 504: {
      DeclareAndCast(StepElement_Curve3dElementDescriptor, anent, ent);
      RWStepElement_RWCurve3dElementDescriptor tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 505: {
      DeclareAndCast(StepElement_CurveElementEndReleasePacket, anent, ent);
      RWStepElement_RWCurveElementEndReleasePacket tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 506: {
      DeclareAndCast(StepElement_CurveElementSectionDefinition, anent, ent);
      RWStepElement_RWCurveElementSectionDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 507: {
      DeclareAndCast(StepElement_CurveElementSectionDerivedDefinitions, anent, ent);
      RWStepElement_RWCurveElementSectionDerivedDefinitions tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 508: {
      DeclareAndCast(StepElement_ElementDescriptor, anent, ent);
      RWStepElement_RWElementDescriptor tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 509: {
      DeclareAndCast(StepElement_ElementMaterial, anent, ent);
      RWStepElement_RWElementMaterial tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 510: {
      DeclareAndCast(StepElement_Surface3dElementDescriptor, anent, ent);
      RWStepElement_RWSurface3dElementDescriptor tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 511: {
      DeclareAndCast(StepElement_SurfaceElementProperty, anent, ent);
      RWStepElement_RWSurfaceElementProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 512: {
      DeclareAndCast(StepElement_SurfaceSection, anent, ent);
      RWStepElement_RWSurfaceSection tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 513: {
      DeclareAndCast(StepElement_SurfaceSectionField, anent, ent);
      RWStepElement_RWSurfaceSectionField tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 514: {
      DeclareAndCast(StepElement_SurfaceSectionFieldConstant, anent, ent);
      RWStepElement_RWSurfaceSectionFieldConstant tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 515: {
      DeclareAndCast(StepElement_SurfaceSectionFieldVarying, anent, ent);
      RWStepElement_RWSurfaceSectionFieldVarying tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 516: {
      DeclareAndCast(StepElement_UniformSurfaceSection, anent, ent);
      RWStepElement_RWUniformSurfaceSection tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 517: {
      DeclareAndCast(StepElement_Volume3dElementDescriptor, anent, ent);
      RWStepElement_RWVolume3dElementDescriptor tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 518: {
      DeclareAndCast(StepFEA_AlignedCurve3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWAlignedCurve3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 519: {
      DeclareAndCast(StepFEA_ArbitraryVolume3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 520: {
      DeclareAndCast(StepFEA_Curve3dElementProperty, anent, ent);
      RWStepFEA_RWCurve3dElementProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 521: {
      DeclareAndCast(StepFEA_Curve3dElementRepresentation, anent, ent);
      RWStepFEA_RWCurve3dElementRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 522: {
      DeclareAndCast(StepFEA_Node, anent, ent);
      RWStepFEA_RWNode tool;
      tool.WriteStep(SW, anent);
    }
    break;
      // case 523:
      //     {
      //       DeclareAndCast(StepFEA_CurveElementEndCoordinateSystem,anent,ent);
      //       RWStepFEA_RWCurveElementEndCoordinateSystem tool;
      //       tool.WriteStep (SW,anent);
      //     }
      //     break;
    case 524: {
      DeclareAndCast(StepFEA_CurveElementEndOffset, anent, ent);
      RWStepFEA_RWCurveElementEndOffset tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 525: {
      DeclareAndCast(StepFEA_CurveElementEndRelease, anent, ent);
      RWStepFEA_RWCurveElementEndRelease tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 526: {
      DeclareAndCast(StepFEA_CurveElementInterval, anent, ent);
      RWStepFEA_RWCurveElementInterval tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 527: {
      DeclareAndCast(StepFEA_CurveElementIntervalConstant, anent, ent);
      RWStepFEA_RWCurveElementIntervalConstant tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 528: {
      DeclareAndCast(StepFEA_DummyNode, anent, ent);
      RWStepFEA_RWDummyNode tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 529: {
      DeclareAndCast(StepFEA_CurveElementLocation, anent, ent);
      RWStepFEA_RWCurveElementLocation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 530: {
      DeclareAndCast(StepFEA_ElementGeometricRelationship, anent, ent);
      RWStepFEA_RWElementGeometricRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 531: {
      DeclareAndCast(StepFEA_ElementGroup, anent, ent);
      RWStepFEA_RWElementGroup tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 532: {
      DeclareAndCast(StepFEA_ElementRepresentation, anent, ent);
      RWStepFEA_RWElementRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 533: {
      DeclareAndCast(StepFEA_FeaAreaDensity, anent, ent);
      RWStepFEA_RWFeaAreaDensity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 534: {
      DeclareAndCast(StepFEA_FeaAxis2Placement3d, anent, ent);
      RWStepFEA_RWFeaAxis2Placement3d tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 535: {
      DeclareAndCast(StepFEA_FeaGroup, anent, ent);
      RWStepFEA_RWFeaGroup tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 536: {
      DeclareAndCast(StepFEA_FeaLinearElasticity, anent, ent);
      RWStepFEA_RWFeaLinearElasticity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 537: {
      DeclareAndCast(StepFEA_FeaMassDensity, anent, ent);
      RWStepFEA_RWFeaMassDensity tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 538: {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentation, anent, ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 539: {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentationItem, anent, ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 540: {
      DeclareAndCast(StepFEA_FeaModel, anent, ent);
      RWStepFEA_RWFeaModel tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 541: {
      DeclareAndCast(StepFEA_FeaModel3d, anent, ent);
      RWStepFEA_RWFeaModel3d tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 542: {
      DeclareAndCast(StepFEA_FeaMoistureAbsorption, anent, ent);
      RWStepFEA_RWFeaMoistureAbsorption tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 543: {
      DeclareAndCast(StepFEA_FeaParametricPoint, anent, ent);
      RWStepFEA_RWFeaParametricPoint tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 544: {
      DeclareAndCast(StepFEA_FeaRepresentationItem, anent, ent);
      RWStepFEA_RWFeaRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 545: {
      DeclareAndCast(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion, anent, ent);
      RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 546: {
      DeclareAndCast(StepFEA_FeaShellBendingStiffness, anent, ent);
      RWStepFEA_RWFeaShellBendingStiffness tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 547: {
      DeclareAndCast(StepFEA_FeaShellMembraneBendingCouplingStiffness, anent, ent);
      RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 548: {
      DeclareAndCast(StepFEA_FeaShellMembraneStiffness, anent, ent);
      RWStepFEA_RWFeaShellMembraneStiffness tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 549: {
      DeclareAndCast(StepFEA_FeaShellShearStiffness, anent, ent);
      RWStepFEA_RWFeaShellShearStiffness tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 550: {
      DeclareAndCast(StepFEA_GeometricNode, anent, ent);
      RWStepFEA_RWGeometricNode tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 551: {
      DeclareAndCast(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion, anent, ent);
      RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 552: {
      DeclareAndCast(StepFEA_NodeGroup, anent, ent);
      RWStepFEA_RWNodeGroup tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 553: {
      DeclareAndCast(StepFEA_NodeRepresentation, anent, ent);
      RWStepFEA_RWNodeRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 554: {
      DeclareAndCast(StepFEA_NodeSet, anent, ent);
      RWStepFEA_RWNodeSet tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 555: {
      DeclareAndCast(StepFEA_NodeWithSolutionCoordinateSystem, anent, ent);
      RWStepFEA_RWNodeWithSolutionCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 556: {
      DeclareAndCast(StepFEA_NodeWithVector, anent, ent);
      RWStepFEA_RWNodeWithVector tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 557: {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateDirection, anent, ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateDirection tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 558: {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 559: {
      DeclareAndCast(StepFEA_ParametricSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWParametricSurface3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 560: {
      DeclareAndCast(StepFEA_Surface3dElementRepresentation, anent, ent);
      RWStepFEA_RWSurface3dElementRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
      // case 561:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor22d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor22d tool;
      //       tool.WriteStep (SW,anent);
      //     }
      //     break;
      // case 562:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor42d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor42d tool;
      //       tool.WriteStep (SW,anent);
      //     }
      //     break;
      // case 563:
      //     {
      //       DeclareAndCast(StepFEA_SymmetricTensor43d,anent,ent);
      //       RWStepFEA_RWSymmetricTensor43d tool;
      //       tool.WriteStep (SW,anent);
      //     }
      //     break;
    case 564: {
      DeclareAndCast(StepFEA_Volume3dElementRepresentation, anent, ent);
      RWStepFEA_RWVolume3dElementRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 565: {
      DeclareAndCast(StepRepr_DataEnvironment, anent, ent);
      RWStepRepr_RWDataEnvironment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 566: {
      DeclareAndCast(StepRepr_MaterialPropertyRepresentation, anent, ent);
      RWStepRepr_RWMaterialPropertyRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 567: {
      DeclareAndCast(StepRepr_PropertyDefinitionRelationship, anent, ent);
      RWStepRepr_RWPropertyDefinitionRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 568: {
      DeclareAndCast(StepShape_PointRepresentation, anent, ent);
      RWStepShape_RWPointRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 569: {
      DeclareAndCast(StepRepr_MaterialProperty, anent, ent);
      RWStepRepr_RWMaterialProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 570: {
      DeclareAndCast(StepFEA_FeaModelDefinition, anent, ent);
      RWStepFEA_RWFeaModelDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 571: {
      DeclareAndCast(StepFEA_FreedomAndCoefficient, anent, ent);
      RWStepFEA_RWFreedomAndCoefficient tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 572: {
      DeclareAndCast(StepFEA_FreedomsList, anent, ent);
      RWStepFEA_RWFreedomsList tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 573: {
      DeclareAndCast(StepBasic_ProductDefinitionFormationRelationship, anent, ent);
      RWStepBasic_RWProductDefinitionFormationRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 574: {
      DeclareAndCast(StepBasic_SiUnitAndMassUnit, anent, ent);
      RWStepBasic_RWSiUnitAndMassUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 575: {
      DeclareAndCast(StepFEA_NodeDefinition, anent, ent);
      RWStepFEA_RWNodeDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 576: {
      DeclareAndCast(StepRepr_StructuralResponseProperty, anent, ent);
      RWStepRepr_RWStructuralResponseProperty tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 577: {
      DeclareAndCast(StepRepr_StructuralResponsePropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 578: {
      DeclareAndCast(StepBasic_SiUnitAndThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 579: {
      DeclareAndCast(StepFEA_AlignedSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWAlignedSurface3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 580: {
      DeclareAndCast(StepFEA_ConstantSurface3dElementCoordinateSystem, anent, ent);
      RWStepFEA_RWConstantSurface3dElementCoordinateSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 581: {
      DeclareAndCast(StepFEA_CurveElementIntervalLinearlyVarying, anent, ent);
      RWStepFEA_RWCurveElementIntervalLinearlyVarying tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 582: {
      DeclareAndCast(StepFEA_FeaCurveSectionGeometricRelationship, anent, ent);
      RWStepFEA_RWFeaCurveSectionGeometricRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 583: {
      DeclareAndCast(StepFEA_FeaSurfaceSectionGeometricRelationship, anent, ent);
      RWStepFEA_RWFeaSurfaceSectionGeometricRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 600: {
      DeclareAndCast(StepBasic_DocumentProductAssociation, anent, ent);
      RWStepBasic_RWDocumentProductAssociation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 601: {
      DeclareAndCast(StepBasic_DocumentProductEquivalence, anent, ent);
      RWStepBasic_RWDocumentProductEquivalence tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 609: {
      DeclareAndCast(StepDimTol_CylindricityTolerance, anent, ent);
      RWStepDimTol_RWCylindricityTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 610: {
      DeclareAndCast(StepShape_ShapeRepresentationWithParameters, anent, ent);
      RWStepShape_RWShapeRepresentationWithParameters tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 611: {
      DeclareAndCast(StepDimTol_AngularityTolerance, anent, ent);
      RWStepDimTol_RWAngularityTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 612: {
      DeclareAndCast(StepDimTol_ConcentricityTolerance, anent, ent);
      RWStepDimTol_RWConcentricityTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 613: {
      DeclareAndCast(StepDimTol_CircularRunoutTolerance, anent, ent);
      RWStepDimTol_RWCircularRunoutTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 614: {
      DeclareAndCast(StepDimTol_CoaxialityTolerance, anent, ent);
      RWStepDimTol_RWCoaxialityTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 615: {
      DeclareAndCast(StepDimTol_FlatnessTolerance, anent, ent);
      RWStepDimTol_RWFlatnessTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 616: {
      DeclareAndCast(StepDimTol_LineProfileTolerance, anent, ent);
      RWStepDimTol_RWLineProfileTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 617: {
      DeclareAndCast(StepDimTol_ParallelismTolerance, anent, ent);
      RWStepDimTol_RWParallelismTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 618: {
      DeclareAndCast(StepDimTol_PerpendicularityTolerance, anent, ent);
      RWStepDimTol_RWPerpendicularityTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 619: {
      DeclareAndCast(StepDimTol_PositionTolerance, anent, ent);
      RWStepDimTol_RWPositionTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 620: {
      DeclareAndCast(StepDimTol_RoundnessTolerance, anent, ent);
      RWStepDimTol_RWRoundnessTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 621: {
      DeclareAndCast(StepDimTol_StraightnessTolerance, anent, ent);
      RWStepDimTol_RWStraightnessTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 622: {
      DeclareAndCast(StepDimTol_SurfaceProfileTolerance, anent, ent);
      RWStepDimTol_RWSurfaceProfileTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 623: {
      DeclareAndCast(StepDimTol_SymmetryTolerance, anent, ent);
      RWStepDimTol_RWSymmetryTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 624: {
      DeclareAndCast(StepDimTol_TotalRunoutTolerance, anent, ent);
      RWStepDimTol_RWTotalRunoutTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 625: {
      DeclareAndCast(StepDimTol_GeometricTolerance, anent, ent);
      RWStepDimTol_RWGeometricTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 626: {
      DeclareAndCast(StepDimTol_GeometricToleranceRelationship, anent, ent);
      RWStepDimTol_RWGeometricToleranceRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 627: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDatumReference, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDatumReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 628: {
      DeclareAndCast(StepDimTol_ModifiedGeometricTolerance, anent, ent);
      RWStepDimTol_RWModifiedGeometricTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 629: {
      DeclareAndCast(StepDimTol_Datum, anent, ent);
      RWStepDimTol_RWDatum tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 630: {
      DeclareAndCast(StepDimTol_DatumFeature, anent, ent);
      RWStepDimTol_RWDatumFeature tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 631: {
      DeclareAndCast(StepDimTol_DatumReference, anent, ent);
      RWStepDimTol_RWDatumReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 632: {
      DeclareAndCast(StepDimTol_CommonDatum, anent, ent);
      RWStepDimTol_RWCommonDatum tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 633: {
      DeclareAndCast(StepDimTol_DatumTarget, anent, ent);
      RWStepDimTol_RWDatumTarget tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 634: {
      DeclareAndCast(StepDimTol_PlacedDatumTargetFeature, anent, ent);
      RWStepDimTol_RWPlacedDatumTargetFeature tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 635: {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnit, anent, ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 636: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 650: {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndMassUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndMassUnit tool;
      tool.WriteStep(SW, anent);
    }

    break;
    case 651: {
      DeclareAndCast(StepBasic_MassMeasureWithUnit, anent, ent);
      RWStepBasic_RWMassMeasureWithUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 660: {
      DeclareAndCast(StepRepr_Apex, anent, ent);
      RWStepRepr_RWApex tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 661: {
      DeclareAndCast(StepRepr_CentreOfSymmetry, anent, ent);
      RWStepRepr_RWCentreOfSymmetry tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 662: {
      DeclareAndCast(StepRepr_GeometricAlignment, anent, ent);
      RWStepRepr_RWGeometricAlignment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 663: {
      DeclareAndCast(StepRepr_PerpendicularTo, anent, ent);
      RWStepRepr_RWPerpendicularTo tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 664: {
      DeclareAndCast(StepRepr_Tangent, anent, ent);
      RWStepRepr_RWTangent tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 665: {
      DeclareAndCast(StepRepr_ParallelOffset, anent, ent);
      RWStepRepr_RWParallelOffset tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 666: {
      DeclareAndCast(StepAP242_GeometricItemSpecificUsage, anent, ent);
      RWStepAP242_RWGeometricItemSpecificUsage tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 667: {
      DeclareAndCast(StepAP242_IdAttribute, anent, ent);
      RWStepAP242_RWIdAttribute tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 668: {
      DeclareAndCast(StepAP242_ItemIdentifiedRepresentationUsage, anent, ent);
      RWStepAP242_RWItemIdentifiedRepresentationUsage tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 669: {
      DeclareAndCast(StepRepr_AllAroundShapeAspect, anent, ent);
      RWStepRepr_RWAllAroundShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 670: {
      DeclareAndCast(StepRepr_BetweenShapeAspect, anent, ent);
      RWStepRepr_RWBetweenShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 671: {
      DeclareAndCast(StepRepr_CompositeGroupShapeAspect, anent, ent);
      RWStepRepr_RWCompositeGroupShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 672: {
      DeclareAndCast(StepRepr_ContinuosShapeAspect, anent, ent);
      RWStepRepr_RWContinuosShapeAspect tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 673: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedAreaUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 674: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 675: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithMaximumTolerance, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithMaximumTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 676: {
      DeclareAndCast(StepDimTol_GeometricToleranceWithModifiers, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithModifiers tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 677: {
      DeclareAndCast(StepDimTol_UnequallyDisposedGeometricTolerance, anent, ent);
      RWStepDimTol_RWUnequallyDisposedGeometricTolerance tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 678: {
      DeclareAndCast(StepDimTol_NonUniformZoneDefinition, anent, ent);
      RWStepDimTol_RWNonUniformZoneDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 679: {
      DeclareAndCast(StepDimTol_ProjectedZoneDefinition, anent, ent);
      RWStepDimTol_RWProjectedZoneDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 680: {
      DeclareAndCast(StepDimTol_RunoutZoneDefinition, anent, ent);
      RWStepDimTol_RWRunoutZoneDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 681: {
      DeclareAndCast(StepDimTol_RunoutZoneOrientation, anent, ent);
      RWStepDimTol_RWRunoutZoneOrientation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 682: {
      DeclareAndCast(StepDimTol_ToleranceZone, anent, ent);
      RWStepDimTol_RWToleranceZone tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 683: {
      DeclareAndCast(StepDimTol_ToleranceZoneDefinition, anent, ent);
      RWStepDimTol_RWToleranceZoneDefinition tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 684: {
      DeclareAndCast(StepDimTol_ToleranceZoneForm, anent, ent);
      RWStepDimTol_RWToleranceZoneForm tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 685: {
      DeclareAndCast(StepShape_ValueFormatTypeQualifier, anent, ent);
      RWStepShape_RWValueFormatTypeQualifier tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 686: {
      DeclareAndCast(StepDimTol_DatumReferenceCompartment, anent, ent);
      RWStepDimTol_RWDatumReferenceCompartment tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 687: {
      DeclareAndCast(StepDimTol_DatumReferenceElement, anent, ent);
      RWStepDimTol_RWDatumReferenceElement tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 688: {
      DeclareAndCast(StepDimTol_DatumReferenceModifierWithValue, anent, ent);
      RWStepDimTol_RWDatumReferenceModifierWithValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 689: {
      DeclareAndCast(StepDimTol_DatumSystem, anent, ent);
      RWStepDimTol_RWDatumSystem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 690: {
      DeclareAndCast(StepDimTol_GeneralDatumReference, anent, ent);
      RWStepDimTol_RWGeneralDatumReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 691: {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit, anent, ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 692: {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI, anent, ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 693: {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI, anent, ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 694: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRef, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRef tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 695: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 696: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMod, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMod tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 697: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 698: {
      DeclareAndCast(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp, anent, ent);
      RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 699: {
      DeclareAndCast(StepRepr_CompShAspAndDatumFeatAndShAsp, anent, ent);
      RWStepRepr_RWCompShAspAndDatumFeatAndShAsp tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 700: {
      DeclareAndCast(StepRepr_IntegerRepresentationItem, anent, ent);
      RWStepRepr_RWIntegerRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 701: {
      DeclareAndCast(StepRepr_ValueRepresentationItem, anent, ent);
      RWStepRepr_RWValueRepresentationItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 702: {
      DeclareAndCast(StepRepr_FeatureForDatumTargetRelationship, anent, ent);
      RWStepRepr_RWFeatureForDatumTargetRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 703: {
      DeclareAndCast(StepAP242_DraughtingModelItemAssociation, anent, ent);
      RWStepAP242_RWDraughtingModelItemAssociation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 704: {
      DeclareAndCast(StepVisual_AnnotationPlane, anent, ent);
      RWStepVisual_RWAnnotationPlane tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 705: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 706: {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMaxTol, anent, ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 707: {
      DeclareAndCast(StepVisual_TessellatedAnnotationOccurrence, anent, ent);
      RWStepVisual_RWTessellatedAnnotationOccurrence tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 708: {
      DeclareAndCast(StepVisual_TessellatedItem, anent, ent);
      RWStepVisual_RWTessellatedItem tool;
      tool.WriteStep(SW, anent);
    }
    break;

    case 709: {
      DeclareAndCast(StepVisual_TessellatedGeometricSet, anent, ent);
      RWStepVisual_RWTessellatedGeometricSet tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 710: {
      DeclareAndCast(StepVisual_TessellatedCurveSet, anent, ent);
      RWStepVisual_RWTessellatedCurveSet tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 711: {
      DeclareAndCast(StepVisual_CoordinatesList, anent, ent);
      RWStepVisual_RWCoordinatesList tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 712: {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentation, anent, ent);
      RWStepRepr_RWConstructiveGeometryRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 713: {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentationRelationship, anent, ent);
      RWStepRepr_RWConstructiveGeometryRepresentationRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 714: {
      DeclareAndCast(StepRepr_CharacterizedRepresentation, anent, ent);
      RWStepRepr_RWCharacterizedRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 715: {
      DeclareAndCast(StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel, anent, ent);
      RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 716: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClipping, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClipping tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 717: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingIntersection, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingIntersection tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 718: {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingUnion, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingUnion tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 719: {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 720: {
      DeclareAndCast(StepVisual_SurfaceStyleTransparent, anent, ent);
      RWStepVisual_RWSurfaceStyleTransparent tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 721: {
      DeclareAndCast(StepVisual_SurfaceStyleReflectanceAmbient, anent, ent);
      RWStepVisual_RWSurfaceStyleReflectanceAmbient tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 722: {
      DeclareAndCast(StepVisual_SurfaceStyleRendering, anent, ent);
      RWStepVisual_RWSurfaceStyleRendering tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 723: {
      DeclareAndCast(StepVisual_SurfaceStyleRenderingWithProperties, anent, ent);
      RWStepVisual_RWSurfaceStyleRenderingWithProperties tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 724: {
      DeclareAndCast(StepRepr_RepresentationContextReference, anent, ent);
      RWStepRepr_RWRepresentationContextReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 725: {
      DeclareAndCast(StepRepr_RepresentationReference, anent, ent);
      RWStepRepr_RWRepresentationReference tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 726: {
      DeclareAndCast(StepGeom_SuParameters, anent, ent);
      RWStepGeom_RWSuParameters tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 727: {
      DeclareAndCast(StepKinematics_RotationAboutDirection, anent, ent);
      RWStepKinematics_RWRotationAboutDirection tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 728: {
      DeclareAndCast(StepKinematics_KinematicJoint, anent, ent);
      RWStepKinematics_RWKinematicJoint tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 729: {
      DeclareAndCast(StepKinematics_ActuatedKinematicPair, anent, ent);
      RWStepKinematics_RWActuatedKinematicPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 730: {
      DeclareAndCast(StepKinematics_ContextDependentKinematicLinkRepresentation, anent, ent);
      RWStepKinematics_RWContextDependentKinematicLinkRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 731: {
      DeclareAndCast(StepKinematics_CylindricalPair, anent, ent);
      RWStepKinematics_RWCylindricalPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 732: {
      DeclareAndCast(StepKinematics_CylindricalPairValue, anent, ent);
      RWStepKinematics_RWCylindricalPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 733: {
      DeclareAndCast(StepKinematics_CylindricalPairWithRange, anent, ent);
      RWStepKinematics_RWCylindricalPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 734: {
      DeclareAndCast(StepKinematics_FullyConstrainedPair, anent, ent);
      RWStepKinematics_RWFullyConstrainedPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 735: {
      DeclareAndCast(StepKinematics_GearPair, anent, ent);
      RWStepKinematics_RWGearPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 736: {
      DeclareAndCast(StepKinematics_GearPairValue, anent, ent);
      RWStepKinematics_RWGearPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 737: {
      DeclareAndCast(StepKinematics_GearPairWithRange, anent, ent);
      RWStepKinematics_RWGearPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 738: {
      DeclareAndCast(StepKinematics_HomokineticPair, anent, ent);
      RWStepKinematics_RWHomokineticPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 739: {
      DeclareAndCast(StepKinematics_KinematicLink, anent, ent);
      RWStepKinematics_RWKinematicLink tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 740: {
      DeclareAndCast(StepKinematics_KinematicLinkRepresentationAssociation, anent, ent);
      RWStepKinematics_RWKinematicLinkRepresentationAssociation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 741: {
      DeclareAndCast(StepKinematics_KinematicPropertyMechanismRepresentation, anent, ent);
      RWStepKinematics_RWKinematicPropertyMechanismRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 742: {
      DeclareAndCast(StepKinematics_KinematicTopologyStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyStructure tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 743: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPair, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 744: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPairValue, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 745: {
      DeclareAndCast(StepKinematics_LowOrderKinematicPairWithRange, anent, ent);
      RWStepKinematics_RWLowOrderKinematicPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 746: {
      DeclareAndCast(StepKinematics_MechanismRepresentation, anent, ent);
      RWStepKinematics_RWMechanismRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 747: {
      DeclareAndCast(StepKinematics_OrientedJoint, anent, ent);
      RWStepKinematics_RWOrientedJoint tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 748: {
      DeclareAndCast(StepKinematics_PlanarCurvePair, anent, ent);
      RWStepKinematics_RWPlanarCurvePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 749: {
      DeclareAndCast(StepKinematics_PlanarCurvePairRange, anent, ent);
      RWStepKinematics_RWPlanarCurvePairRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 750: {
      DeclareAndCast(StepKinematics_PlanarPair, anent, ent);
      RWStepKinematics_RWPlanarPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 751: {
      DeclareAndCast(StepKinematics_PlanarPairValue, anent, ent);
      RWStepKinematics_RWPlanarPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 752: {
      DeclareAndCast(StepKinematics_PlanarPairWithRange, anent, ent);
      RWStepKinematics_RWPlanarPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 753: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePair, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 754: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePairValue, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 755: {
      DeclareAndCast(StepKinematics_PointOnPlanarCurvePairWithRange, anent, ent);
      RWStepKinematics_RWPointOnPlanarCurvePairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 756: {
      DeclareAndCast(StepKinematics_PointOnSurfacePair, anent, ent);
      RWStepKinematics_RWPointOnSurfacePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 757: {
      DeclareAndCast(StepKinematics_PointOnSurfacePairValue, anent, ent);
      RWStepKinematics_RWPointOnSurfacePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 758: {
      DeclareAndCast(StepKinematics_PointOnSurfacePairWithRange, anent, ent);
      RWStepKinematics_RWPointOnSurfacePairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 759: {
      DeclareAndCast(StepKinematics_PrismaticPair, anent, ent);
      RWStepKinematics_RWPrismaticPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 760: {
      DeclareAndCast(StepKinematics_PrismaticPairValue, anent, ent);
      RWStepKinematics_RWPrismaticPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 761: {
      DeclareAndCast(StepKinematics_PrismaticPairWithRange, anent, ent);
      RWStepKinematics_RWPrismaticPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 762: {
      DeclareAndCast(StepKinematics_ProductDefinitionKinematics, anent, ent);
      RWStepKinematics_RWProductDefinitionKinematics tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 763: {
      DeclareAndCast(StepKinematics_ProductDefinitionRelationshipKinematics, anent, ent);
      RWStepKinematics_RWProductDefinitionRelationshipKinematics tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 764: {
      DeclareAndCast(StepKinematics_RackAndPinionPair, anent, ent);
      RWStepKinematics_RWRackAndPinionPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 765: {
      DeclareAndCast(StepKinematics_RackAndPinionPairValue, anent, ent);
      RWStepKinematics_RWRackAndPinionPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 766: {
      DeclareAndCast(StepKinematics_RackAndPinionPairWithRange, anent, ent);
      RWStepKinematics_RWRackAndPinionPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 767: {
      DeclareAndCast(StepKinematics_RevolutePair, anent, ent);
      RWStepKinematics_RWRevolutePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 768: {
      DeclareAndCast(StepKinematics_RevolutePairValue, anent, ent);
      RWStepKinematics_RWRevolutePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 769: {
      DeclareAndCast(StepKinematics_RevolutePairWithRange, anent, ent);
      RWStepKinematics_RWRevolutePairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 770: {
      DeclareAndCast(StepKinematics_RollingCurvePair, anent, ent);
      RWStepKinematics_RWRollingCurvePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 771: {
      DeclareAndCast(StepKinematics_RollingCurvePairValue, anent, ent);
      RWStepKinematics_RWRollingCurvePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 772: {
      DeclareAndCast(StepKinematics_RollingSurfacePair, anent, ent);
      RWStepKinematics_RWRollingSurfacePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 773: {
      DeclareAndCast(StepKinematics_RollingSurfacePairValue, anent, ent);
      RWStepKinematics_RWRollingSurfacePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 774: {
      DeclareAndCast(StepKinematics_ScrewPair, anent, ent);
      RWStepKinematics_RWScrewPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 775: {
      DeclareAndCast(StepKinematics_ScrewPairValue, anent, ent);
      RWStepKinematics_RWScrewPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 776: {
      DeclareAndCast(StepKinematics_ScrewPairWithRange, anent, ent);
      RWStepKinematics_RWScrewPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 777: {
      DeclareAndCast(StepKinematics_SlidingCurvePair, anent, ent);
      RWStepKinematics_RWSlidingCurvePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 778: {
      DeclareAndCast(StepKinematics_SlidingCurvePairValue, anent, ent);
      RWStepKinematics_RWSlidingCurvePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 779: {
      DeclareAndCast(StepKinematics_SlidingSurfacePair, anent, ent);
      RWStepKinematics_RWSlidingSurfacePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 780: {
      DeclareAndCast(StepKinematics_SlidingSurfacePairValue, anent, ent);
      RWStepKinematics_RWSlidingSurfacePairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 781: {
      DeclareAndCast(StepKinematics_SphericalPair, anent, ent);
      RWStepKinematics_RWSphericalPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 782: {
      DeclareAndCast(StepKinematics_SphericalPairValue, anent, ent);
      RWStepKinematics_RWSphericalPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 783: {
      DeclareAndCast(StepKinematics_SphericalPairWithPin, anent, ent);
      RWStepKinematics_RWSphericalPairWithPin tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 784: {
      DeclareAndCast(StepKinematics_SphericalPairWithPinAndRange, anent, ent);
      RWStepKinematics_RWSphericalPairWithPinAndRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 785: {
      DeclareAndCast(StepKinematics_SphericalPairWithRange, anent, ent);
      RWStepKinematics_RWSphericalPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 786: {
      DeclareAndCast(StepKinematics_SurfacePairWithRange, anent, ent);
      RWStepKinematics_RWSurfacePairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 787: {
      DeclareAndCast(StepKinematics_UnconstrainedPair, anent, ent);
      RWStepKinematics_RWUnconstrainedPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 788: {
      DeclareAndCast(StepKinematics_UnconstrainedPairValue, anent, ent);
      RWStepKinematics_RWUnconstrainedPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 789: {
      DeclareAndCast(StepKinematics_UniversalPair, anent, ent);
      RWStepKinematics_RWUniversalPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 790: {
      DeclareAndCast(StepKinematics_UniversalPairValue, anent, ent);
      RWStepKinematics_RWUniversalPairValue tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 791: {
      DeclareAndCast(StepKinematics_UniversalPairWithRange, anent, ent);
      RWStepKinematics_RWUniversalPairWithRange tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 792: {
      DeclareAndCast(StepKinematics_PairRepresentationRelationship, anent, ent);
      RWStepKinematics_RWPairRepresentationRelationship tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 793: {
      DeclareAndCast(StepKinematics_RigidLinkRepresentation, anent, ent);
      RWStepKinematics_RWRigidLinkRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 794: {
      DeclareAndCast(StepKinematics_KinematicTopologyDirectedStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyDirectedStructure tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 795: {
      DeclareAndCast(StepKinematics_KinematicTopologyNetworkStructure, anent, ent);
      RWStepKinematics_RWKinematicTopologyNetworkStructure tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 796: {
      DeclareAndCast(StepKinematics_LinearFlexibleAndPinionPair, anent, ent);
      RWStepKinematics_RWLinearFlexibleAndPinionPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 797: {
      DeclareAndCast(StepKinematics_LinearFlexibleAndPlanarCurvePair, anent, ent);
      RWStepKinematics_RWLinearFlexibleAndPlanarCurvePair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 798: {
      DeclareAndCast(StepKinematics_LinearFlexibleLinkRepresentation, anent, ent);
      RWStepKinematics_RWLinearFlexibleLinkRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 800: {
      DeclareAndCast(StepKinematics_ActuatedKinPairAndOrderKinPair, anent, ent);
      RWStepKinematics_RWActuatedKinPairAndOrderKinPair tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 801: {
      DeclareAndCast(StepKinematics_MechanismStateRepresentation, anent, ent);
      RWStepKinematics_RWMechanismStateRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
    case 802: {
      DeclareAndCast(StepVisual_RepositionedTessellatedGeometricSet, anEnt, ent);
      RWStepVisual_RWRepositionedTessellatedGeometricSet aTool;
      aTool.WriteStep(SW, anEnt);
      break;
    }
    case 803: {
      DeclareAndCast(StepVisual_RepositionedTessellatedItem, anEnt, ent);
      RWStepVisual_RWRepositionedTessellatedItem aTool;
      aTool.WriteStep(SW, anEnt);
      break;
    }

      // --------------------------------------------------------------------

    case 804: {
      DeclareAndCast(StepVisual_TessellatedConnectingEdge, anEnt, ent);
      RWStepVisual_RWTessellatedConnectingEdge aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 805: {
      DeclareAndCast(StepVisual_TessellatedEdge, anEnt, ent);
      RWStepVisual_RWTessellatedEdge aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 806: {
      DeclareAndCast(StepVisual_TessellatedPointSet, anEnt, ent);
      RWStepVisual_RWTessellatedPointSet aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 807: {
      DeclareAndCast(StepVisual_TessellatedShapeRepresentation, anEnt, ent);
      RWStepVisual_RWTessellatedShapeRepresentation aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 808: {
      DeclareAndCast(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters, anEnt, ent);
      RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 809: {
      DeclareAndCast(StepVisual_TessellatedShell, anEnt, ent);
      RWStepVisual_RWTessellatedShell aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 810: {
      DeclareAndCast(StepVisual_TessellatedSolid, anEnt, ent);
      RWStepVisual_RWTessellatedSolid aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 811: {
      DeclareAndCast(StepVisual_TessellatedStructuredItem, anEnt, ent);
      RWStepVisual_RWTessellatedStructuredItem aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 812: {
      DeclareAndCast(StepVisual_TessellatedVertex, anEnt, ent);
      RWStepVisual_RWTessellatedVertex aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 813: {
      DeclareAndCast(StepVisual_TessellatedWire, anEnt, ent);
      RWStepVisual_RWTessellatedWire aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 814: {
      DeclareAndCast(StepVisual_TriangulatedFace, anEnt, ent);
      RWStepVisual_RWTriangulatedFace aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 815: {
      DeclareAndCast(StepVisual_ComplexTriangulatedFace, anEnt, ent);
      RWStepVisual_RWComplexTriangulatedFace aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 816: {
      DeclareAndCast(StepVisual_ComplexTriangulatedSurfaceSet, anEnt, ent);
      RWStepVisual_RWComplexTriangulatedSurfaceSet aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 817: {
      DeclareAndCast(StepVisual_CubicBezierTessellatedEdge, anEnt, ent);
      RWStepVisual_RWCubicBezierTessellatedEdge aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 818: {
      DeclareAndCast(StepVisual_CubicBezierTriangulatedFace, anEnt, ent);
      RWStepVisual_RWCubicBezierTriangulatedFace aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 819: {
      DeclareAndCast(StepVisual_TriangulatedSurfaceSet, anEnt, ent);
      RWStepVisual_RWTriangulatedSurfaceSet aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 820: {
      DeclareAndCast(StepBasic_GeneralPropertyAssociation, anEnt, ent);
      RWStepBasic_RWGeneralPropertyAssociation aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 821: {
      DeclareAndCast(StepBasic_GeneralPropertyRelationship, anEnt, ent);
      RWStepBasic_RWGeneralPropertyRelationship aTool;
      aTool.WriteStep(SW, anEnt);
    }
    break;
    case 822: {
      DeclareAndCast(StepRepr_BooleanRepresentationItem, anent, ent);
      RWStepRepr_RWBooleanRepresentationItem aTool;
      aTool.WriteStep(SW, anent);
    }
    break;
    case 823: {
      DeclareAndCast(StepRepr_RealRepresentationItem, anent, ent);
      RWStepRepr_RWRealRepresentationItem aTool;
      aTool.WriteStep(SW, anent);
    }
    break;
    case 824: {
      DeclareAndCast(StepRepr_MechanicalDesignAndDraughtingRelationship, anent, ent);
      RWStepRepr_RWMechanicalDesignAndDraughtingRelationship aTool;
      aTool.WriteStep(SW, anent);
    }
    break;
    default:
      return;
  }
}
