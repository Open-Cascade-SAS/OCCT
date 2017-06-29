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
//:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve -> surface_curve
// :j4 gka 15.03.99 S4134
// sln 03.10.2001. BUC61003. Correction of alphabetic order of complex entity's items 

#include <Interface_Check.hxx>
#include <Interface_ParamType.hxx>
#include <Interface_ReaderLib.hxx>
#include <RWStepAP214_ReadWriteModule.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepAP214_Protocol.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_WriterLib.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWStepAP214_ReadWriteModule,StepData_ReadWriteModule)

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


#include <RWStepBasic_RWAddress.hxx>
#include <RWStepShape_RWAdvancedBrepShapeRepresentation.hxx>
#include <RWStepShape_RWAdvancedFace.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationCurveOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationFillArea.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationFillAreaOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSubfigureOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationSymbolOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationText.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWAnnotationTextOccurrence.hxx>
#include <RWStepBasic_RWApplicationContext.hxx>
#include <RWStepBasic_RWApplicationContextElement.hxx>
#include <RWStepBasic_RWApplicationProtocolDefinition.hxx>
#include <RWStepBasic_RWApproval.hxx>
#include <RWStepBasic_RWApprovalPersonOrganization.hxx>
#include <RWStepBasic_RWApprovalRelationship.hxx>
#include <RWStepBasic_RWApprovalRole.hxx>
#include <RWStepBasic_RWApprovalStatus.hxx>
#include <RWStepVisual_RWAreaInSet.hxx>
#include <RWStepAP214_RWAutoDesignActualDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAutoDesignActualDateAssignment.hxx>
#include <RWStepAP214_RWAutoDesignApprovalAssignment.hxx>
#include <RWStepAP214_RWAutoDesignDateAndPersonAssignment.hxx>
#include <RWStepAP214_RWAutoDesignGroupAssignment.hxx>
#include <RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAutoDesignNominalDateAssignment.hxx>
#include <RWStepAP214_RWAutoDesignOrganizationAssignment.hxx>
#include <RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment.hxx>
#include <RWStepAP214_RWAutoDesignPresentedItem.hxx>
#include <RWStepAP214_RWAutoDesignSecurityClassificationAssignment.hxx>
// Removed from Rev2 to Rev4 : <RWStepAP214_RWAutoDesignViewArea.hxx>
#include <RWStepGeom_RWAxis1Placement.hxx>
#include <RWStepGeom_RWAxis2Placement2d.hxx>
#include <RWStepGeom_RWAxis2Placement3d.hxx>
#include <RWStepGeom_RWBSplineCurve.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnots.hxx>
#include <RWStepGeom_RWBSplineSurface.hxx>
#include <RWStepGeom_RWBSplineSurfaceWithKnots.hxx>
#include <RWStepVisual_RWBackgroundColour.hxx>
#include <RWStepGeom_RWBezierCurve.hxx>
#include <RWStepGeom_RWBezierSurface.hxx>
#include <RWStepShape_RWBlock.hxx>
#include <RWStepShape_RWBooleanResult.hxx>
#include <RWStepGeom_RWBoundaryCurve.hxx>
#include <RWStepGeom_RWBoundedCurve.hxx>
#include <RWStepGeom_RWBoundedSurface.hxx>
#include <RWStepShape_RWBoxDomain.hxx>
#include <RWStepShape_RWBoxedHalfSpace.hxx>
#include <RWStepShape_RWBrepWithVoids.hxx>
#include <RWStepBasic_RWCalendarDate.hxx>
#include <RWStepVisual_RWCameraImage.hxx>
#include <RWStepVisual_RWCameraModel.hxx>
#include <RWStepVisual_RWCameraModelD2.hxx>
#include <RWStepVisual_RWCameraModelD3.hxx>
#include <RWStepVisual_RWCameraUsage.hxx>
#include <RWStepGeom_RWCartesianPoint.hxx>
#include <RWStepGeom_RWCartesianTransformationOperator.hxx>
#include <RWStepGeom_RWCartesianTransformationOperator3d.hxx>
#include <RWStepGeom_RWCircle.hxx>
#include <RWStepShape_RWClosedShell.hxx>
#include <RWStepVisual_RWColour.hxx>
#include <RWStepVisual_RWColourRgb.hxx>
#include <RWStepVisual_RWColourSpecification.hxx>
#include <RWStepGeom_RWCompositeCurve.hxx>
#include <RWStepGeom_RWCompositeCurveOnSurface.hxx>
#include <RWStepGeom_RWCompositeCurveSegment.hxx>
#include <RWStepVisual_RWCompositeText.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWCompositeTextWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWCompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <RWStepVisual_RWCompositeTextWithExtent.hxx>

#include <RWStepGeom_RWConic.hxx>
#include <RWStepGeom_RWConicalSurface.hxx>
#include <RWStepShape_RWConnectedFaceSet.hxx>
#include <RWStepVisual_RWContextDependentInvisibility.hxx>
#include <RWStepVisual_RWContextDependentOverRidingStyledItem.hxx>
#include <RWStepBasic_RWConversionBasedUnit.hxx>
#include <RWStepBasic_RWCoordinatedUniversalTimeOffset.hxx>
// Removed from Rev2 to Rev4 : <RWStepShape_RWCsgRepresentation.hxx>
#include <RWStepShape_RWCsgShapeRepresentation.hxx>
#include <RWStepShape_RWCsgSolid.hxx>
#include <RWStepGeom_RWCurve.hxx>
#include <RWStepGeom_RWCurveBoundedSurface.hxx>
#include <RWStepGeom_RWCurveReplica.hxx>
#include <RWStepVisual_RWCurveStyle.hxx>
#include <RWStepVisual_RWCurveStyleFont.hxx>
#include <RWStepVisual_RWCurveStyleFontPattern.hxx>
#include <RWStepGeom_RWCylindricalSurface.hxx>
#include <RWStepBasic_RWDate.hxx>
#include <RWStepBasic_RWDateAndTime.hxx>
#include <RWStepBasic_RWDateRole.hxx>
#include <RWStepBasic_RWDateTimeRole.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDefinedSymbol.hxx>
#include <RWStepRepr_RWDefinitionalRepresentation.hxx>
#include <RWStepGeom_RWDegeneratePcurve.hxx>
#include <RWStepGeom_RWDegenerateToroidalSurface.hxx>
#include <RWStepRepr_RWDescriptiveRepresentationItem.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDimensionCurve.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDimensionCurveTerminator.hxx>
#include <RWStepBasic_RWDimensionalExponents.hxx>
#include <RWStepGeom_RWDirection.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingCallout.hxx>
#include <RWStepVisual_RWDraughtingPreDefinedColour.hxx>
#include <RWStepVisual_RWDraughtingPreDefinedCurveFont.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingSubfigureRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDraughtingTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDrawingDefinition.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWDrawingRevision.hxx>
#include <RWStepShape_RWEdge.hxx>
#include <RWStepShape_RWEdgeCurve.hxx>
#include <RWStepShape_RWEdgeLoop.hxx>
#include <RWStepGeom_RWElementarySurface.hxx>
#include <RWStepGeom_RWEllipse.hxx>
#include <RWStepGeom_RWEvaluatedDegeneratePcurve.hxx>
#include <RWStepBasic_RWExternalSource.hxx>
#include <RWStepVisual_RWExternallyDefinedCurveFont.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedHatchStyle.hxx>
#include <RWStepBasic_RWExternallyDefinedItem.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedTextFont.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWExternallyDefinedTileStyle.hxx>
#include <RWStepShape_RWExtrudedAreaSolid.hxx>
#include <RWStepShape_RWFace.hxx>
// Removed from Rev2 to Rev4 : <RWStepShape_RWFaceBasedSurfaceModel.hxx>
#include <RWStepShape_RWFaceBound.hxx>
#include <RWStepShape_RWFaceOuterBound.hxx>
#include <RWStepShape_RWFaceSurface.hxx>
#include <RWStepShape_RWFacetedBrep.hxx>
#include <RWStepShape_RWFacetedBrepShapeRepresentation.hxx>
#include <RWStepVisual_RWFillAreaStyle.hxx>
#include <RWStepVisual_RWFillAreaStyleColour.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleHatching.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleTileSymbolWithStyle.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWFillAreaStyleTiles.hxx>
#include <RWStepRepr_RWFunctionallyDefinedTransformation.hxx>
#include <RWStepShape_RWGeometricCurveSet.hxx>
#include <RWStepGeom_RWGeometricRepresentationContext.hxx>
#include <RWStepGeom_RWGeometricRepresentationItem.hxx>
#include <RWStepShape_RWGeometricSet.hxx>
#include <RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <RWStepRepr_RWGlobalUncertaintyAssignedContext.hxx>
#include <RWStepRepr_RWGlobalUnitAssignedContext.hxx>
#include <RWStepBasic_RWGroup.hxx>
#include <RWStepBasic_RWGroupRelationship.hxx>
#include <RWStepShape_RWHalfSpaceSolid.hxx>
#include <RWStepGeom_RWHyperbola.hxx>
#include <RWStepGeom_RWIntersectionCurve.hxx>
#include <RWStepVisual_RWInvisibility.hxx>
#include <RWStepBasic_RWLengthMeasureWithUnit.hxx>
#include <RWStepBasic_RWLengthUnit.hxx>
#include <RWStepGeom_RWLine.hxx>
#include <RWStepBasic_RWLocalTime.hxx>
#include <RWStepShape_RWLoop.hxx>
#include <RWStepShape_RWManifoldSolidBrep.hxx>
#include <RWStepShape_RWManifoldSurfaceShapeRepresentation.hxx>
#include <RWStepRepr_RWMappedItem.hxx>
#include <RWStepBasic_RWMeasureWithUnit.hxx>
#include <RWStepVisual_RWMechanicalDesignGeometricPresentationArea.hxx>
#include <RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWMechanicalDesignPresentationArea.hxx>
#include <RWStepBasic_RWNamedUnit.hxx>
#include <RWStepGeom_RWOffsetCurve3d.hxx>
#include <RWStepGeom_RWOffsetSurface.hxx>
// Removed from Rev2 to Rev4 : <RWStepAP214_RWOneDirectionRepeatFactor.hxx>
#include <RWStepShape_RWOpenShell.hxx>
#include <RWStepBasic_RWOrdinalDate.hxx>
#include <RWStepBasic_RWOrganization.hxx>
#include <RWStepBasic_RWOrganizationRole.hxx>
#include <RWStepBasic_RWOrganizationalAddress.hxx>
#include <RWStepShape_RWOrientedClosedShell.hxx>
#include <RWStepShape_RWOrientedEdge.hxx>
#include <RWStepShape_RWOrientedFace.hxx>
#include <RWStepShape_RWOrientedOpenShell.hxx>
#include <RWStepShape_RWOrientedPath.hxx>
#include <RWStepGeom_RWOuterBoundaryCurve.hxx>
#include <RWStepVisual_RWOverRidingStyledItem.hxx>
#include <RWStepGeom_RWParabola.hxx>
#include <RWStepRepr_RWParametricRepresentationContext.hxx>
#include <RWStepShape_RWPath.hxx>
#include <RWStepGeom_RWPcurve.hxx>
#include <RWStepBasic_RWPerson.hxx>
#include <RWStepBasic_RWPersonAndOrganization.hxx>
#include <RWStepBasic_RWPersonAndOrganizationRole.hxx>
#include <RWStepBasic_RWPersonalAddress.hxx>
#include <RWStepGeom_RWPlacement.hxx>
#include <RWStepVisual_RWPlanarBox.hxx>
#include <RWStepVisual_RWPlanarExtent.hxx>
#include <RWStepGeom_RWPlane.hxx>
#include <RWStepBasic_RWPlaneAngleMeasureWithUnit.hxx>
#include <RWStepBasic_RWPlaneAngleUnit.hxx>
#include <RWStepGeom_RWPoint.hxx>
#include <RWStepGeom_RWPointOnCurve.hxx>
#include <RWStepGeom_RWPointOnSurface.hxx>
#include <RWStepGeom_RWPointReplica.hxx>
#include <RWStepVisual_RWPointStyle.hxx>
#include <RWStepShape_RWPolyLoop.hxx>
#include <RWStepGeom_RWPolyline.hxx>
#include <RWStepVisual_RWPreDefinedColour.hxx>
#include <RWStepVisual_RWPreDefinedCurveFont.hxx>
#include <RWStepVisual_RWPreDefinedItem.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWPreDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWPreDefinedTextFont.hxx>
#include <RWStepVisual_RWPresentationArea.hxx>
#include <RWStepVisual_RWPresentationLayerAssignment.hxx>
#include <RWStepVisual_RWPresentationRepresentation.hxx>
#include <RWStepVisual_RWPresentationSet.hxx>
#include <RWStepVisual_RWPresentationSize.hxx>
#include <RWStepVisual_RWPresentationStyleAssignment.hxx>
#include <RWStepVisual_RWPresentationStyleByContext.hxx>
#include <RWStepVisual_RWPresentationView.hxx>
#include <RWStepBasic_RWProduct.hxx>
#include <RWStepBasic_RWProductCategory.hxx>
#include <RWStepBasic_RWProductContext.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWProductDataRepresentationView.hxx>
#include <RWStepBasic_RWProductDefinition.hxx>
#include <RWStepBasic_RWProductDefinitionContext.hxx>
#include <RWStepBasic_RWProductDefinitionFormation.hxx>
#include <RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource.hxx>
#include <RWStepRepr_RWProductDefinitionShape.hxx>
#include <RWStepBasic_RWProductRelatedProductCategory.hxx>
#include <RWStepBasic_RWProductType.hxx>
#include <RWStepRepr_RWPropertyDefinition.hxx>
#include <RWStepRepr_RWPropertyDefinitionRepresentation.hxx>
#include <RWStepGeom_RWQuasiUniformCurve.hxx>
#include <RWStepGeom_RWQuasiUniformSurface.hxx>
#include <RWStepBasic_RWRatioMeasureWithUnit.hxx>
#include <RWStepGeom_RWRationalBSplineCurve.hxx>
#include <RWStepGeom_RWRationalBSplineSurface.hxx>
#include <RWStepGeom_RWRectangularCompositeSurface.hxx>
#include <RWStepGeom_RWRectangularTrimmedSurface.hxx>
#include <RWStepAP214_RWRepItemGroup.hxx>
#include <RWStepGeom_RWReparametrisedCompositeCurveSegment.hxx>
#include <RWStepRepr_RWRepresentation.hxx>
#include <RWStepRepr_RWRepresentationContext.hxx>
#include <RWStepRepr_RWRepresentationItem.hxx>
#include <RWStepRepr_RWRepresentationMap.hxx>
#include <RWStepRepr_RWRepresentationRelationship.hxx>
#include <RWStepShape_RWRevolvedAreaSolid.hxx>
#include <RWStepShape_RWRightAngularWedge.hxx>
#include <RWStepShape_RWRightCircularCone.hxx>
#include <RWStepShape_RWRightCircularCylinder.hxx>
#include <RWStepGeom_RWSeamCurve.hxx>
#include <RWStepBasic_RWSecurityClassification.hxx>
#include <RWStepBasic_RWSecurityClassificationLevel.hxx>
#include <RWStepRepr_RWShapeAspect.hxx>
#include <RWStepRepr_RWShapeAspectRelationship.hxx>
#include <RWStepRepr_RWFeatureForDatumTargetRelationship.hxx>
#include <RWStepRepr_RWShapeAspectTransition.hxx>
#include <RWStepShape_RWShapeDefinitionRepresentation.hxx>
#include <RWStepShape_RWShapeRepresentation.hxx>
#include <RWStepShape_RWShellBasedSurfaceModel.hxx>
#include <RWStepBasic_RWSiUnit.hxx>
#include <RWStepBasic_RWSolidAngleMeasureWithUnit.hxx>
#include <RWStepShape_RWSolidModel.hxx>
#include <RWStepShape_RWSolidReplica.hxx>
#include <RWStepShape_RWSphere.hxx>
#include <RWStepGeom_RWSphericalSurface.hxx>
#include <RWStepVisual_RWStyledItem.hxx>
#include <RWStepGeom_RWSurface.hxx>
#include <RWStepGeom_RWSurfaceCurve.hxx>
#include <RWStepGeom_RWSurfaceOfLinearExtrusion.hxx>
#include <RWStepGeom_RWSurfaceOfRevolution.hxx>
#include <RWStepGeom_RWSurfaceCurveAndBoundedCurve.hxx>
#include <RWStepGeom_RWSurfacePatch.hxx>
#include <RWStepGeom_RWSurfaceReplica.hxx>
#include <RWStepVisual_RWSurfaceSideStyle.hxx>
#include <RWStepVisual_RWSurfaceStyleBoundary.hxx>
#include <RWStepVisual_RWSurfaceStyleControlGrid.hxx>
#include <RWStepVisual_RWSurfaceStyleFillArea.hxx>
#include <RWStepVisual_RWSurfaceStyleParameterLine.hxx>
#include <RWStepVisual_RWSurfaceStyleSegmentationCurve.hxx>
#include <RWStepVisual_RWSurfaceStyleSilhouette.hxx>
#include <RWStepVisual_RWSurfaceStyleUsage.hxx>
#include <RWStepShape_RWSweptAreaSolid.hxx>
#include <RWStepGeom_RWSweptSurface.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolColour.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolRepresentationMap.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolStyle.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWSymbolTarget.hxx>
#include <RWStepVisual_RWTemplate.hxx>
#include <RWStepVisual_RWTemplateInstance.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <RWStepVisual_RWTextLiteral.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithBlankingBox.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
#include <RWStepVisual_RWTextStyle.hxx>
#include <RWStepVisual_RWTextStyleForDefinedFont.hxx>
#include  <RWStepVisual_RWTextStyleWithBoxCharacteristics.hxx>
// Removed from Rev2 to Rev4 : <RWStepVisual_RWTextStyleWithMirror.hxx>
#include <RWStepShape_RWTopologicalRepresentationItem.hxx>
#include <RWStepGeom_RWToroidalSurface.hxx>
#include <RWStepShape_RWTorus.hxx>
#include <RWStepShape_RWTransitionalShapeRepresentation.hxx>
#include <RWStepGeom_RWTrimmedCurve.hxx>
// Removed from Rev2 to Rev4 : <RWStepAP214_RWTwoDirectionRepeatFactor.hxx>
#include <RWStepBasic_RWUncertaintyMeasureWithUnit.hxx>
#include <RWStepGeom_RWUniformCurve.hxx>
#include <RWStepGeom_RWUniformSurface.hxx>
#include <RWStepGeom_RWVector.hxx>
#include <RWStepShape_RWVertex.hxx>
#include <RWStepShape_RWVertexLoop.hxx>
#include <RWStepShape_RWVertexPoint.hxx>
#include <RWStepVisual_RWViewVolume.hxx>
#include <RWStepBasic_RWWeekOfYearAndDayDate.hxx>
#include <RWStepGeom_RWUniformCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWBezierCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepBasic_RWSiUnitAndLengthUnit.hxx>
#include <RWStepBasic_RWSiUnitAndPlaneAngleUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndLengthUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <RWStepShape_RWLoopAndPath.hxx>


// Added by FMA
#include <RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit.hxx>
#include <RWStepBasic_RWSiUnitAndSolidAngleUnit.hxx>
#include <RWStepBasic_RWSolidAngleUnit.hxx>
#include <RWStepShape_RWFacetedBrepAndBrepWithVoids.hxx>
#include <RWStepBasic_RWMechanicalContext.hxx>

// full Rev4
#include <RWStepBasic_RWSiUnitAndRatioUnit.hxx>
#include <RWStepBasic_RWSiUnitAndTimeUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndRatioUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndTimeUnit.hxx>
#include <RWStepBasic_RWApprovalDateTime.hxx>
//not yet #include <StepVisual_CameraImage2dWithScale.hxx> derived
//not yet #include <StepVisual_CameraImage3dWithScale.hxx> derived
//not yet #include <StepVisual_CartesianTransformationOperator2d.hxx> derived
#include <RWStepBasic_RWDerivedUnit.hxx>
#include <RWStepBasic_RWDerivedUnitElement.hxx>
#include <RWStepRepr_RWItemDefinedTransformation.hxx>
#include <RWStepVisual_RWPresentedItemRepresentation.hxx>
#include <RWStepVisual_RWPresentationLayerUsage.hxx>

//  Added by CKY (JUL-1998) for AP214 CC1 -> CC2

#include <RWStepAP214_RWAutoDesignDocumentReference.hxx>
#include <RWStepBasic_RWDocument.hxx>
#include <RWStepBasic_RWDocumentRelationship.hxx>
#include <RWStepBasic_RWDocumentType.hxx>
#include <RWStepBasic_RWDocumentUsageConstraint.hxx>
#include <RWStepBasic_RWEffectivity.hxx>
#include <RWStepBasic_RWProductDefinitionEffectivity.hxx>
#include <RWStepBasic_RWProductDefinitionRelationship.hxx>

#include <RWStepBasic_RWProductDefinitionWithAssociatedDocuments.hxx>

#include <RWStepRepr_RWMakeFromUsageOption.hxx>
#include <RWStepRepr_RWAssemblyComponentUsage.hxx>
#include <RWStepRepr_RWQuantifiedAssemblyComponentUsage.hxx>
#include <RWStepRepr_RWSpecifiedHigherUsageOccurrence.hxx>
#include <RWStepRepr_RWAssemblyComponentUsageSubstitute.hxx>
#include <RWStepRepr_RWRepresentationRelationshipWithTransformation.hxx>
#include <RWStepRepr_RWShapeRepresentationRelationshipWithTransformation.hxx>
#include <RWStepRepr_RWMaterialDesignation.hxx>

#include <RWStepShape_RWContextDependentShapeRepresentation.hxx>

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

#include <RWStepAP214_RWAppliedDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAppliedDateAssignment.hxx>
#include <RWStepAP214_RWAppliedApprovalAssignment.hxx>
#include <RWStepAP214_RWAppliedDocumentReference.hxx>
#include <RWStepAP214_RWAppliedGroupAssignment.hxx>
#include <RWStepAP214_RWAppliedOrganizationAssignment.hxx>
#include <RWStepAP214_RWAppliedPersonAndOrganizationAssignment.hxx>
#include <RWStepAP214_RWAppliedPresentedItem.hxx>
#include <RWStepAP214_RWAppliedSecurityClassificationAssignment.hxx>  
#include <RWStepBasic_RWDocumentFile.hxx>
#include <RWStepBasic_RWCharacterizedObject.hxx>  
#include <RWStepShape_RWExtrudedFaceSolid.hxx>
#include <RWStepShape_RWRevolvedFaceSolid.hxx>  
#include <RWStepShape_RWSweptFaceSolid.hxx> 

// Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <RWStepRepr_RWMeasureRepresentationItem.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx> 
#include <RWStepBasic_RWSiUnitAndAreaUnit.hxx>
#include <RWStepBasic_RWSiUnitAndVolumeUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndAreaUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndVolumeUnit.hxx>

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
#include <RWStepBasic_RWAction.hxx>
#include <RWStepBasic_RWActionAssignment.hxx>
#include <RWStepBasic_RWActionMethod.hxx>
#include <RWStepBasic_RWActionRequestAssignment.hxx>
#include <RWStepAP203_RWCcDesignApproval.hxx>
#include <RWStepAP203_RWCcDesignCertification.hxx>
#include <RWStepAP203_RWCcDesignContract.hxx>
#include <RWStepAP203_RWCcDesignDateAndTimeAssignment.hxx>
#include <RWStepAP203_RWCcDesignPersonAndOrganizationAssignment.hxx>
#include <RWStepAP203_RWCcDesignSecurityClassification.hxx>
#include <RWStepAP203_RWCcDesignSpecificationReference.hxx>
#include <RWStepBasic_RWCertification.hxx>
#include <RWStepBasic_RWCertificationAssignment.hxx>
#include <RWStepBasic_RWCertificationType.hxx>
#include <RWStepAP203_RWChange.hxx>
#include <RWStepAP203_RWChangeRequest.hxx>
#include <RWStepRepr_RWConfigurationDesign.hxx>
#include <RWStepRepr_RWConfigurationEffectivity.hxx>
#include <RWStepBasic_RWContract.hxx>
#include <RWStepBasic_RWContractAssignment.hxx>
#include <RWStepBasic_RWContractType.hxx>
#include <RWStepRepr_RWProductConcept.hxx>
#include <RWStepBasic_RWProductConceptContext.hxx>
#include <RWStepAP203_RWStartRequest.hxx>
#include <RWStepAP203_RWStartWork.hxx>
#include <RWStepBasic_RWVersionedActionRequest.hxx>
#include <RWStepBasic_RWProductCategoryRelationship.hxx>
#include <RWStepBasic_RWActionRequestSolution.hxx>

// Added by ABV 13.01.00 for CAX-IF TRJ3
#include <StepVisual_DraughtingModel.hxx>
#include <RWStepVisual_RWDraughtingModel.hxx>

// Added by ABV 18.04.00 for CAX-IF TRJ4 (dimensions)
#include <StepShape_AngularLocation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include <RWStepShape_RWAngularLocation.hxx>
#include <RWStepShape_RWAngularSize.hxx>
#include <RWStepShape_RWDimensionalCharacteristicRepresentation.hxx>
#include <RWStepShape_RWDimensionalLocation.hxx>
#include <RWStepShape_RWDimensionalLocationWithPath.hxx>
#include <RWStepShape_RWDimensionalSize.hxx>
#include <RWStepShape_RWDimensionalSizeWithPath.hxx>
#include <RWStepShape_RWShapeDimensionRepresentation.hxx>

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
#include <RWStepBasic_RWDocumentRepresentationType.hxx>
#include <RWStepBasic_RWObjectRole.hxx>
#include <RWStepBasic_RWRoleAssociation.hxx>
#include <RWStepBasic_RWIdentificationRole.hxx>
#include <RWStepBasic_RWIdentificationAssignment.hxx>
#include <RWStepBasic_RWExternalIdentificationAssignment.hxx>
#include <RWStepBasic_RWEffectivityAssignment.hxx>
#include <RWStepBasic_RWNameAssignment.hxx>
#include <RWStepBasic_RWGeneralProperty.hxx>
#include <RWStepAP214_RWClass.hxx>
#include <RWStepAP214_RWExternallyDefinedClass.hxx>
#include <RWStepAP214_RWExternallyDefinedGeneralProperty.hxx>
#include <RWStepAP214_RWAppliedExternalIdentificationAssignment.hxx>

// abv 11.07.00: CAX-IF TRJ4: k1_geo-ac.stp
#include <StepShape_DefinitionalRepresentationAndShapeRepresentation.hxx>
#include <RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation.hxx>


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
#include <RWStepShape_RWLimitsAndFits.hxx>
#include <RWStepShape_RWToleranceValue.hxx>
#include <RWStepShape_RWMeasureQualification.hxx>
#include <RWStepShape_RWPlusMinusTolerance.hxx>
#include <RWStepShape_RWPrecisionQualifier.hxx>
#include <RWStepShape_RWTypeQualifier.hxx>
#include <RWStepShape_RWQualifiedRepresentationItem.hxx>
#include <RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <RWStepRepr_RWCompoundRepresentationItem.hxx>

// abv 28.12.01: CAX-IF TRJ9: edge_based_wireframe
#include <StepShape_CompoundShapeRepresentation.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_ConnectedFaceShapeRepresentation.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_EdgeBasedWireframeShapeRepresentation.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <RWStepShape_RWCompoundShapeRepresentation.hxx>
#include <RWStepShape_RWConnectedEdgeSet.hxx>
#include <RWStepShape_RWConnectedFaceShapeRepresentation.hxx>
#include <RWStepShape_RWEdgeBasedWireframeModel.hxx>
#include <RWStepShape_RWEdgeBasedWireframeShapeRepresentation.hxx>
#include <RWStepShape_RWFaceBasedSurfaceModel.hxx>
#include <RWStepShape_RWNonManifoldSurfaceShapeRepresentation.hxx>

//gka 08.01.02 TRJ9
#include <StepGeom_OrientedSurface.hxx>
#include <StepShape_Subface.hxx>
#include <StepShape_Subedge.hxx>
#include <StepShape_SeamEdge.hxx>
#include <StepShape_ConnectedFaceSubSet.hxx>

#include <RWStepGeom_RWOrientedSurface.hxx>
#include <RWStepShape_RWSubface.hxx>
#include <RWStepShape_RWSubedge.hxx>
#include <RWStepShape_RWSeamEdge.hxx>
#include <RWStepShape_RWConnectedFaceSubSet.hxx>

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

#include <RWStepBasic_RWEulerAngles.hxx>
#include <RWStepBasic_RWMassUnit.hxx>
#include <RWStepBasic_RWThermodynamicTemperatureUnit.hxx>
#include <RWStepElement_RWAnalysisItemWithinRepresentation.hxx>
#include <RWStepElement_RWCurve3dElementDescriptor.hxx>
#include <RWStepElement_RWCurveElementEndReleasePacket.hxx>
#include <RWStepElement_RWCurveElementSectionDefinition.hxx>
#include <RWStepElement_RWCurveElementSectionDerivedDefinitions.hxx>
#include <RWStepElement_RWElementDescriptor.hxx>
#include <RWStepElement_RWElementMaterial.hxx>
#include <RWStepElement_RWSurface3dElementDescriptor.hxx>
#include <RWStepElement_RWSurfaceElementProperty.hxx>
#include <RWStepElement_RWSurfaceSection.hxx>
#include <RWStepElement_RWSurfaceSectionField.hxx>
#include <RWStepElement_RWSurfaceSectionFieldConstant.hxx>
#include <RWStepElement_RWSurfaceSectionFieldVarying.hxx>
#include <RWStepElement_RWUniformSurfaceSection.hxx>
#include <RWStepElement_RWVolume3dElementDescriptor.hxx>
#include <RWStepFEA_RWAlignedCurve3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWCurve3dElementProperty.hxx>
#include <RWStepFEA_RWCurve3dElementRepresentation.hxx>
#include <RWStepFEA_RWNode.hxx>
#include <RWStepFEA_RWCurveElementEndOffset.hxx>
#include <RWStepFEA_RWCurveElementEndRelease.hxx>
#include <RWStepFEA_RWCurveElementInterval.hxx>
#include <RWStepFEA_RWCurveElementIntervalConstant.hxx>
#include <RWStepFEA_RWDummyNode.hxx>
#include <RWStepFEA_RWCurveElementLocation.hxx>
#include <RWStepFEA_RWElementGeometricRelationship.hxx>
#include <RWStepFEA_RWElementGroup.hxx>
#include <RWStepFEA_RWElementRepresentation.hxx>
#include <RWStepFEA_RWFeaAreaDensity.hxx>
#include <RWStepFEA_RWFeaAxis2Placement3d.hxx>
#include <RWStepFEA_RWFeaGroup.hxx>
#include <RWStepFEA_RWFeaLinearElasticity.hxx>
#include <RWStepFEA_RWFeaMassDensity.hxx>
#include <RWStepFEA_RWFeaMaterialPropertyRepresentation.hxx>
#include <RWStepFEA_RWFeaMaterialPropertyRepresentationItem.hxx>
#include <RWStepFEA_RWFeaModel.hxx>
#include <RWStepFEA_RWFeaModel3d.hxx>
#include <RWStepFEA_RWFeaMoistureAbsorption.hxx>
#include <RWStepFEA_RWFeaParametricPoint.hxx>
#include <RWStepFEA_RWFeaRepresentationItem.hxx>
#include <RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion.hxx>
#include <RWStepFEA_RWFeaShellBendingStiffness.hxx>
#include <RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness.hxx>
#include <RWStepFEA_RWFeaShellMembraneStiffness.hxx>
#include <RWStepFEA_RWFeaShellShearStiffness.hxx>
#include <RWStepFEA_RWGeometricNode.hxx>
#include <RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion.hxx>
#include <RWStepFEA_RWNodeGroup.hxx>
#include <RWStepFEA_RWNodeRepresentation.hxx>
#include <RWStepFEA_RWNodeSet.hxx>
#include <RWStepFEA_RWNodeWithSolutionCoordinateSystem.hxx>
#include <RWStepFEA_RWNodeWithVector.hxx>
#include <RWStepFEA_RWParametricCurve3dElementCoordinateDirection.hxx>
#include <RWStepFEA_RWParametricCurve3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWParametricSurface3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWSurface3dElementRepresentation.hxx>
#include <RWStepFEA_RWVolume3dElementRepresentation.hxx>
#include <RWStepRepr_RWDataEnvironment.hxx>
#include <RWStepRepr_RWMaterialPropertyRepresentation.hxx>
#include <RWStepRepr_RWPropertyDefinitionRelationship.hxx>
#include <RWStepShape_RWPointRepresentation.hxx>
#include <RWStepRepr_RWMaterialProperty.hxx>
#include <RWStepFEA_RWFeaModelDefinition.hxx>
#include <RWStepFEA_RWFreedomAndCoefficient.hxx>
#include <RWStepFEA_RWFreedomsList.hxx>
#include <RWStepBasic_RWProductDefinitionFormationRelationship.hxx>
#include <RWStepFEA_RWNodeDefinition.hxx>
#include <RWStepRepr_RWStructuralResponseProperty.hxx>
#include <RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation.hxx>

#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <RWStepBasic_RWSiUnitAndMassUnit.hxx>
#include <RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepFEA_AlignedSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_ConstantSurface3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWAlignedSurface3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWConstantSurface3dElementCoordinateSystem.hxx>

// 23.01.2003
#include <StepFEA_CurveElementIntervalLinearlyVarying.hxx>
#include <StepFEA_FeaCurveSectionGeometricRelationship.hxx>
#include <StepFEA_FeaSurfaceSectionGeometricRelationship.hxx>
#include <RWStepFEA_RWCurveElementIntervalLinearlyVarying.hxx>
#include <RWStepFEA_RWFeaCurveSectionGeometricRelationship.hxx>
#include <RWStepFEA_RWFeaSurfaceSectionGeometricRelationship.hxx>

// PTV 28.01.2003 TRJ11 AP214 external references
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <RWStepBasic_RWDocumentProductAssociation.hxx>
#include <RWStepBasic_RWDocumentProductEquivalence.hxx>

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
#include <RWStepRepr_RWCompositeShapeAspect.hxx>
#include <RWStepRepr_RWDerivedShapeAspect.hxx>
#include <RWStepRepr_RWExtension.hxx>
#include <RWStepShape_RWShapeRepresentationWithParameters.hxx>
#include <RWStepDimTol_RWAngularityTolerance.hxx>
#include <RWStepDimTol_RWConcentricityTolerance.hxx>
#include <RWStepDimTol_RWCircularRunoutTolerance.hxx>
#include <RWStepDimTol_RWCoaxialityTolerance.hxx>
#include <RWStepDimTol_RWCylindricityTolerance.hxx>
#include <RWStepDimTol_RWFlatnessTolerance.hxx>
#include <RWStepDimTol_RWLineProfileTolerance.hxx>
#include <RWStepDimTol_RWParallelismTolerance.hxx>
#include <RWStepDimTol_RWPerpendicularityTolerance.hxx>
#include <RWStepDimTol_RWPositionTolerance.hxx>
#include <RWStepDimTol_RWRoundnessTolerance.hxx>
#include <RWStepDimTol_RWStraightnessTolerance.hxx>
#include <RWStepDimTol_RWSurfaceProfileTolerance.hxx>
#include <RWStepDimTol_RWSymmetryTolerance.hxx>
#include <RWStepDimTol_RWTotalRunoutTolerance.hxx>
#include <RWStepDimTol_RWGeometricTolerance.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithDatumReference.hxx>
#include <RWStepDimTol_RWModifiedGeometricTolerance.hxx>
#include <RWStepDimTol_RWDatum.hxx>
#include <RWStepDimTol_RWDatumFeature.hxx>
#include <RWStepDimTol_RWDatumReference.hxx>
#include <RWStepDimTol_RWCommonDatum.hxx>
#include <RWStepDimTol_RWDatumTarget.hxx>
#include <RWStepDimTol_RWPlacedDatumTargetFeature.hxx>
#include <StepDimTol_GeometricToleranceRelationship.hxx>
#include <RWStepDimTol_RWGeometricToleranceRelationship.hxx>

#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <RWStepRepr_RWReprItemAndLengthMeasureWithUnit.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>

// added by skl 10.02.2004 for TRJ13
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_MassMeasureWithUnit.hxx>
#include <RWStepBasic_RWMassMeasureWithUnit.hxx>

// Added by ika for GD&T AP242
#include <RWStepRepr_RWApex.hxx>
#include <RWStepRepr_RWCentreOfSymmetry.hxx>
#include <RWStepRepr_RWGeometricAlignment.hxx>
#include <RWStepRepr_RWParallelOffset.hxx>
#include <RWStepRepr_RWPerpendicularTo.hxx>
#include <RWStepRepr_RWTangent.hxx>
#include <RWStepAP242_RWGeometricItemSpecificUsage.hxx>
#include <RWStepAP242_RWIdAttribute.hxx>
#include <RWStepAP242_RWItemIdentifiedRepresentationUsage.hxx>
#include <RWStepRepr_RWAllAroundShapeAspect.hxx>
#include <RWStepRepr_RWBetweenShapeAspect.hxx>
#include <RWStepRepr_RWCompositeGroupShapeAspect.hxx>
#include <RWStepRepr_RWContinuosShapeAspect.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithDefinedUnit.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithMaximumTolerance.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithModifiers.hxx>
#include <RWStepDimTol_RWUnequallyDisposedGeometricTolerance.hxx>
#include <RWStepDimTol_RWNonUniformZoneDefinition.hxx>
#include <RWStepDimTol_RWProjectedZoneDefinition.hxx>
#include <RWStepDimTol_RWRunoutZoneDefinition.hxx>
#include <RWStepDimTol_RWRunoutZoneOrientation.hxx>
#include <RWStepDimTol_RWToleranceZone.hxx>
#include <RWStepDimTol_RWToleranceZoneDefinition.hxx>
#include <RWStepDimTol_RWToleranceZoneForm.hxx>
#include <RWStepShape_RWValueFormatTypeQualifier.hxx>
#include <RWStepDimTol_RWDatumReferenceCompartment.hxx>
#include <RWStepDimTol_RWDatumReferenceElement.hxx>
#include <RWStepDimTol_RWDatumReferenceModifierWithValue.hxx>
#include <RWStepDimTol_RWDatumSystem.hxx>
#include <RWStepDimTol_RWGeneralDatumReference.hxx>
#include <RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit.hxx>
#include <RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI.hxx>
#include <RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRef.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthMod.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol.hxx>
#include <RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <RWStepRepr_RWCompShAspAndDatumFeatAndShAsp.hxx>
#include <RWStepRepr_RWIntegerRepresentationItem.hxx>
#include <RWStepRepr_RWValueRepresentationItem.hxx>
#include <RWStepRepr_RWValueRepresentationItem.hxx>
#include <RWStepAP242_RWDraughtingModelItemAssociation.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol.hxx>
#include <RWStepVisual_RWAnnotationCurveOccurrence.hxx>
#include <RWStepVisual_RWAnnotationOccurrence.hxx>
#include <RWStepVisual_RWAnnotationPlane.hxx>
#include <RWStepVisual_RWDraughtingCallout.hxx>

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

#include <RWStepVisual_RWTessellatedAnnotationOccurrence.hxx>
#include <RWStepVisual_RWTessellatedItem.hxx>
#include <RWStepVisual_RWTessellatedGeometricSet.hxx>
#include <RWStepVisual_RWTessellatedCurveSet.hxx>
#include <RWStepVisual_RWCoordinatesList.hxx>
#include <RWStepRepr_RWCharacterizedRepresentation.hxx>
#include <RWStepRepr_RWConstructiveGeometryRepresentation.hxx>
#include <RWStepRepr_RWConstructiveGeometryRepresentationRelationship.hxx>
#include <StepVisual_CharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation.hxx>
#include <RWStepVisual_RWCharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation.hxx>
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>
#include <RWStepVisual_RWAnnotationFillArea.hxx>
#include <RWStepVisual_RWAnnotationFillAreaOccurrence.hxx>
#include <StepVisual_CameraModelD3MultiClipping.hxx>
#include <StepVisual_CameraModelD3MultiClippingIntersection.hxx>
#include <StepVisual_CameraModelD3MultiClippingUnion.hxx>
#include <RWStepVisual_RWCameraModelD3MultiClipping.hxx>
#include <RWStepVisual_RWCameraModelD3MultiClippingIntersection.hxx>
#include <RWStepVisual_RWCameraModelD3MultiClippingUnion.hxx>
#include <StepVisual_AnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem.hxx>
#include <RWStepVisual_RWAnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem.hxx>


// -- General Declarations (Recognize, StepType) ---

static TCollection_AsciiString PasReco("?");
static TCollection_AsciiString Reco_Address ("ADDRESS");
static TCollection_AsciiString Reco_AdvancedBrepShapeRepresentation ("ADVANCED_BREP_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_AdvancedFace ("ADVANCED_FACE");
static TCollection_AsciiString Reco_AnnotationCurveOccurrence ("ANNOTATION_CURVE_OCCURRENCE");
static TCollection_AsciiString Reco_AnnotationFillArea ("ANNOTATION_FILL_AREA");
static TCollection_AsciiString Reco_AnnotationFillAreaOccurrence ("ANNOTATION_FILL_AREA_OCCURRENCE");
static TCollection_AsciiString Reco_AnnotationOccurrence ("ANNOTATION_OCCURRENCE");
static TCollection_AsciiString Reco_AnnotationSubfigureOccurrence ("ANNOTATION_SUBFIGURE_OCCURRENCE");
static TCollection_AsciiString Reco_AnnotationSymbol ("ANNOTATION_SYMBOL");
static TCollection_AsciiString Reco_AnnotationSymbolOccurrence ("ANNOTATION_SYMBOL_OCCURRENCE");
static TCollection_AsciiString Reco_AnnotationText ("ANNOTATION_TEXT");
static TCollection_AsciiString Reco_AnnotationTextOccurrence ("ANNOTATION_TEXT_OCCURRENCE");
static TCollection_AsciiString Reco_ApplicationContext ("APPLICATION_CONTEXT");
static TCollection_AsciiString Reco_ApplicationContextElement ("APPLICATION_CONTEXT_ELEMENT");
static TCollection_AsciiString Reco_ApplicationProtocolDefinition ("APPLICATION_PROTOCOL_DEFINITION");
static TCollection_AsciiString Reco_Approval ("APPROVAL");
static TCollection_AsciiString Reco_ApprovalAssignment ("APPROVAL_ASSIGNMENT");
static TCollection_AsciiString Reco_ApprovalPersonOrganization ("APPROVAL_PERSON_ORGANIZATION");
static TCollection_AsciiString Reco_ApprovalRelationship ("APPROVAL_RELATIONSHIP");
static TCollection_AsciiString Reco_ApprovalRole ("APPROVAL_ROLE");
static TCollection_AsciiString Reco_ApprovalStatus ("APPROVAL_STATUS");
static TCollection_AsciiString Reco_AreaInSet ("AREA_IN_SET");
static TCollection_AsciiString Reco_AutoDesignActualDateAndTimeAssignment ("AUTO_DESIGN_ACTUAL_DATE_AND_TIME_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignActualDateAssignment ("AUTO_DESIGN_ACTUAL_DATE_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignApprovalAssignment ("AUTO_DESIGN_APPROVAL_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignDateAndPersonAssignment ("AUTO_DESIGN_DATE_AND_PERSON_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignGroupAssignment ("AUTO_DESIGN_GROUP_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignNominalDateAndTimeAssignment ("AUTO_DESIGN_NOMINAL_DATE_AND_TIME_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignNominalDateAssignment ("AUTO_DESIGN_NOMINAL_DATE_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignOrganizationAssignment ("AUTO_DESIGN_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignPersonAndOrganizationAssignment ("AUTO_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignPresentedItem ("AUTO_DESIGN_PRESENTED_ITEM");
static TCollection_AsciiString Reco_AutoDesignSecurityClassificationAssignment ("AUTO_DESIGN_SECURITY_CLASSIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_AutoDesignViewArea ("AUTO_DESIGN_VIEW_AREA");
static TCollection_AsciiString Reco_Axis1Placement ("AXIS1_PLACEMENT");
static TCollection_AsciiString Reco_Axis2Placement2d ("AXIS2_PLACEMENT_2D");
static TCollection_AsciiString Reco_Axis2Placement3d ("AXIS2_PLACEMENT_3D");
static TCollection_AsciiString Reco_BSplineCurve ("B_SPLINE_CURVE");
static TCollection_AsciiString Reco_BSplineCurveWithKnots ("B_SPLINE_CURVE_WITH_KNOTS");
static TCollection_AsciiString Reco_BSplineSurface ("B_SPLINE_SURFACE");
static TCollection_AsciiString Reco_BSplineSurfaceWithKnots ("B_SPLINE_SURFACE_WITH_KNOTS");
static TCollection_AsciiString Reco_BackgroundColour ("BACKGROUND_COLOUR");
static TCollection_AsciiString Reco_BezierCurve ("BEZIER_CURVE");
static TCollection_AsciiString Reco_BezierSurface ("BEZIER_SURFACE");
static TCollection_AsciiString Reco_Block ("BLOCK");
static TCollection_AsciiString Reco_BooleanResult ("BOOLEAN_RESULT");
static TCollection_AsciiString Reco_BoundaryCurve ("BOUNDARY_CURVE");
static TCollection_AsciiString Reco_BoundedCurve ("BOUNDED_CURVE");
static TCollection_AsciiString Reco_BoundedSurface ("BOUNDED_SURFACE");
static TCollection_AsciiString Reco_BoxDomain ("BOX_DOMAIN");
static TCollection_AsciiString Reco_BoxedHalfSpace ("BOXED_HALF_SPACE");
static TCollection_AsciiString Reco_BrepWithVoids ("BREP_WITH_VOIDS");
static TCollection_AsciiString Reco_CalendarDate ("CALENDAR_DATE");
static TCollection_AsciiString Reco_CameraImage ("CAMERA_IMAGE");
static TCollection_AsciiString Reco_CameraModel ("CAMERA_MODEL");
static TCollection_AsciiString Reco_CameraModelD2 ("CAMERA_MODEL_D2");
static TCollection_AsciiString Reco_CameraModelD3 ("CAMERA_MODEL_D3");
static TCollection_AsciiString Reco_CameraUsage ("CAMERA_USAGE");
static TCollection_AsciiString Reco_CartesianPoint ("CARTESIAN_POINT");
static TCollection_AsciiString Reco_CartesianTransformationOperator ("CARTESIAN_TRANSFORMATION_OPERATOR");
static TCollection_AsciiString Reco_CartesianTransformationOperator3d ("CARTESIAN_TRANSFORMATION_OPERATOR_3D");
static TCollection_AsciiString Reco_Circle ("CIRCLE");
static TCollection_AsciiString Reco_ClosedShell ("CLOSED_SHELL");
static TCollection_AsciiString Reco_Colour ("COLOUR");
static TCollection_AsciiString Reco_ColourRgb ("COLOUR_RGB");
static TCollection_AsciiString Reco_ColourSpecification ("COLOUR_SPECIFICATION");
static TCollection_AsciiString Reco_CompositeCurve ("COMPOSITE_CURVE");
static TCollection_AsciiString Reco_CompositeCurveOnSurface ("COMPOSITE_CURVE_ON_SURFACE");
static TCollection_AsciiString Reco_CompositeCurveSegment ("COMPOSITE_CURVE_SEGMENT");
static TCollection_AsciiString Reco_CompositeText ("COMPOSITE_TEXT");
static TCollection_AsciiString Reco_CompositeTextWithAssociatedCurves ("COMPOSITE_TEXT_WITH_ASSOCIATED_CURVES");
static TCollection_AsciiString Reco_CompositeTextWithBlankingBox ("COMPOSITE_TEXT_WITH_BLANKING_BOX");
static TCollection_AsciiString Reco_CompositeTextWithExtent ("COMPOSITE_TEXT_WITH_EXTENT");
static TCollection_AsciiString Reco_Conic ("CONIC");
static TCollection_AsciiString Reco_ConicalSurface ("CONICAL_SURFACE");
static TCollection_AsciiString Reco_ConnectedFaceSet ("CONNECTED_FACE_SET");
static TCollection_AsciiString Reco_ContextDependentInvisibility ("CONTEXT_DEPENDENT_INVISIBILITY");
static TCollection_AsciiString Reco_ContextDependentOverRidingStyledItem ("CONTEXT_DEPENDENT_OVER_RIDING_STYLED_ITEM");
static TCollection_AsciiString Reco_ConversionBasedUnit ("CONVERSION_BASED_UNIT");
static TCollection_AsciiString Reco_CoordinatedUniversalTimeOffset ("COORDINATED_UNIVERSAL_TIME_OFFSET");
static TCollection_AsciiString Reco_CsgRepresentation ("CSG_REPRESENTATION");
static TCollection_AsciiString Reco_CsgShapeRepresentation ("CSG_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_CsgSolid ("CSG_SOLID");
static TCollection_AsciiString Reco_Curve ("CURVE");
static TCollection_AsciiString Reco_CurveBoundedSurface ("CURVE_BOUNDED_SURFACE");
static TCollection_AsciiString Reco_CurveReplica ("CURVE_REPLICA");
static TCollection_AsciiString Reco_CurveStyle ("CURVE_STYLE");
static TCollection_AsciiString Reco_CurveStyleFont ("CURVE_STYLE_FONT");
static TCollection_AsciiString Reco_CurveStyleFontPattern ("CURVE_STYLE_FONT_PATTERN");
static TCollection_AsciiString Reco_CylindricalSurface ("CYLINDRICAL_SURFACE");
static TCollection_AsciiString Reco_Date ("DATE");
static TCollection_AsciiString Reco_DateAndTime ("DATE_AND_TIME");
static TCollection_AsciiString Reco_DateAndTimeAssignment ("DATE_AND_TIME_ASSIGNMENT");
static TCollection_AsciiString Reco_DateAssignment ("DATE_ASSIGNMENT");
static TCollection_AsciiString Reco_DateRole ("DATE_ROLE");
static TCollection_AsciiString Reco_DateTimeRole ("DATE_TIME_ROLE");
static TCollection_AsciiString Reco_DefinedSymbol ("DEFINED_SYMBOL");
static TCollection_AsciiString Reco_DefinitionalRepresentation ("DEFINITIONAL_REPRESENTATION");
static TCollection_AsciiString Reco_DegeneratePcurve ("DEGENERATE_PCURVE");
static TCollection_AsciiString Reco_DegenerateToroidalSurface ("DEGENERATE_TOROIDAL_SURFACE");
static TCollection_AsciiString Reco_DescriptiveRepresentationItem ("DESCRIPTIVE_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_DimensionCurve ("DIMENSION_CURVE");
static TCollection_AsciiString Reco_DimensionCurveTerminator ("DIMENSION_CURVE_TERMINATOR");
static TCollection_AsciiString Reco_DimensionalExponents ("DIMENSIONAL_EXPONENTS");
static TCollection_AsciiString Reco_Direction ("DIRECTION");
static TCollection_AsciiString Reco_DraughtingAnnotationOccurrence ("DRAUGHTING_ANNOTATION_OCCURRENCE");
static TCollection_AsciiString Reco_DraughtingCallout ("DRAUGHTING_CALLOUT");
static TCollection_AsciiString Reco_DraughtingPreDefinedColour ("DRAUGHTING_PRE_DEFINED_COLOUR");
static TCollection_AsciiString Reco_DraughtingPreDefinedCurveFont ("DRAUGHTING_PRE_DEFINED_CURVE_FONT");
static TCollection_AsciiString Reco_DraughtingSubfigureRepresentation ("DRAUGHTING_SUBFIGURE_REPRESENTATION");
static TCollection_AsciiString Reco_DraughtingSymbolRepresentation ("DRAUGHTING_SYMBOL_REPRESENTATION");
static TCollection_AsciiString Reco_DraughtingTextLiteralWithDelineation ("DRAUGHTING_TEXT_LITERAL_WITH_DELINEATION");
static TCollection_AsciiString Reco_DrawingDefinition ("DRAWING_DEFINITION");
static TCollection_AsciiString Reco_DrawingRevision ("DRAWING_REVISION");
static TCollection_AsciiString Reco_Edge ("EDGE");
static TCollection_AsciiString Reco_EdgeCurve ("EDGE_CURVE");
static TCollection_AsciiString Reco_EdgeLoop ("EDGE_LOOP");
static TCollection_AsciiString Reco_ElementarySurface ("ELEMENTARY_SURFACE");
static TCollection_AsciiString Reco_Ellipse ("ELLIPSE");
static TCollection_AsciiString Reco_EvaluatedDegeneratePcurve ("EVALUATED_DEGENERATE_PCURVE");
static TCollection_AsciiString Reco_ExternalSource ("EXTERNAL_SOURCE");
static TCollection_AsciiString Reco_ExternallyDefinedCurveFont ("EXTERNALLY_DEFINED_CURVE_FONT");
static TCollection_AsciiString Reco_ExternallyDefinedHatchStyle ("EXTERNALLY_DEFINED_HATCH_STYLE");
static TCollection_AsciiString Reco_ExternallyDefinedItem ("EXTERNALLY_DEFINED_ITEM");
static TCollection_AsciiString Reco_ExternallyDefinedSymbol ("EXTERNALLY_DEFINED_SYMBOL");
static TCollection_AsciiString Reco_ExternallyDefinedTextFont ("EXTERNALLY_DEFINED_TEXT_FONT");
static TCollection_AsciiString Reco_ExternallyDefinedTileStyle ("EXTERNALLY_DEFINED_TILE_STYLE");
static TCollection_AsciiString Reco_ExtrudedAreaSolid ("EXTRUDED_AREA_SOLID");
static TCollection_AsciiString Reco_Face ("FACE");
static TCollection_AsciiString Reco_FaceBound ("FACE_BOUND");
static TCollection_AsciiString Reco_FaceOuterBound ("FACE_OUTER_BOUND");
static TCollection_AsciiString Reco_FaceSurface ("FACE_SURFACE");
static TCollection_AsciiString Reco_FacetedBrep ("FACETED_BREP");
static TCollection_AsciiString Reco_FacetedBrepShapeRepresentation ("FACETED_BREP_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_FillAreaStyle ("FILL_AREA_STYLE");
static TCollection_AsciiString Reco_FillAreaStyleColour ("FILL_AREA_STYLE_COLOUR");
static TCollection_AsciiString Reco_FillAreaStyleHatching ("FILL_AREA_STYLE_HATCHING");
static TCollection_AsciiString Reco_FillAreaStyleTileSymbolWithStyle ("FILL_AREA_STYLE_TILE_SYMBOL_WITH_STYLE");
static TCollection_AsciiString Reco_FillAreaStyleTiles ("FILL_AREA_STYLE_TILES");
static TCollection_AsciiString Reco_FunctionallyDefinedTransformation ("FUNCTIONALLY_DEFINED_TRANSFORMATION");
static TCollection_AsciiString Reco_GeometricCurveSet ("GEOMETRIC_CURVE_SET");
static TCollection_AsciiString Reco_GeometricRepresentationContext ("GEOMETRIC_REPRESENTATION_CONTEXT");
static TCollection_AsciiString Reco_GeometricRepresentationItem ("GEOMETRIC_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_GeometricSet ("GEOMETRIC_SET");
static TCollection_AsciiString Reco_GeometricallyBoundedSurfaceShapeRepresentation ("GEOMETRICALLY_BOUNDED_SURFACE_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_GeometricallyBoundedWireframeShapeRepresentation ("GEOMETRICALLY_BOUNDED_WIREFRAME_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_GlobalUncertaintyAssignedContext ("GLOBAL_UNCERTAINTY_ASSIGNED_CONTEXT");
static TCollection_AsciiString Reco_GlobalUnitAssignedContext ("GLOBAL_UNIT_ASSIGNED_CONTEXT");
static TCollection_AsciiString Reco_Group ("GROUP");
static TCollection_AsciiString Reco_GroupAssignment ("GROUP_ASSIGNMENT");
static TCollection_AsciiString Reco_GroupRelationship ("GROUP_RELATIONSHIP");
static TCollection_AsciiString Reco_HalfSpaceSolid ("HALF_SPACE_SOLID");
static TCollection_AsciiString Reco_Hyperbola ("HYPERBOLA");
static TCollection_AsciiString Reco_IntersectionCurve ("INTERSECTION_CURVE");
static TCollection_AsciiString Reco_Invisibility ("INVISIBILITY");
static TCollection_AsciiString Reco_LengthMeasureWithUnit ("LENGTH_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_LengthUnit ("LENGTH_UNIT");
static TCollection_AsciiString Reco_Line ("LINE");
static TCollection_AsciiString Reco_LocalTime ("LOCAL_TIME");
static TCollection_AsciiString Reco_Loop ("LOOP");
static TCollection_AsciiString Reco_ManifoldSolidBrep ("MANIFOLD_SOLID_BREP");
static TCollection_AsciiString Reco_ManifoldSurfaceShapeRepresentation ("MANIFOLD_SURFACE_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_MappedItem ("MAPPED_ITEM");
static TCollection_AsciiString Reco_MeasureWithUnit ("MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_MechanicalDesignGeometricPresentationArea ("MECHANICAL_DESIGN_GEOMETRIC_PRESENTATION_AREA");
static TCollection_AsciiString Reco_MechanicalDesignGeometricPresentationRepresentation ("MECHANICAL_DESIGN_GEOMETRIC_PRESENTATION_REPRESENTATION");
static TCollection_AsciiString Reco_MechanicalDesignPresentationArea ("MECHANICAL_DESIGN_PRESENTATION_AREA");
static TCollection_AsciiString Reco_NamedUnit ("NAMED_UNIT");
static TCollection_AsciiString Reco_OffsetCurve3d ("OFFSET_CURVE_3D");
static TCollection_AsciiString Reco_OffsetSurface ("OFFSET_SURFACE");
static TCollection_AsciiString Reco_OneDirectionRepeatFactor ("ONE_DIRECTION_REPEAT_FACTOR");
static TCollection_AsciiString Reco_OpenShell ("OPEN_SHELL");
static TCollection_AsciiString Reco_OrdinalDate ("ORDINAL_DATE");
static TCollection_AsciiString Reco_Organization ("ORGANIZATION");
static TCollection_AsciiString Reco_OrganizationAssignment ("ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_OrganizationRole ("ORGANIZATION_ROLE");
static TCollection_AsciiString Reco_OrganizationalAddress ("ORGANIZATIONAL_ADDRESS");
static TCollection_AsciiString Reco_OrientedClosedShell ("ORIENTED_CLOSED_SHELL");
static TCollection_AsciiString Reco_OrientedEdge ("ORIENTED_EDGE");
static TCollection_AsciiString Reco_OrientedFace ("ORIENTED_FACE");
static TCollection_AsciiString Reco_OrientedOpenShell ("ORIENTED_OPEN_SHELL");
static TCollection_AsciiString Reco_OrientedPath ("ORIENTED_PATH");
static TCollection_AsciiString Reco_OuterBoundaryCurve ("OUTER_BOUNDARY_CURVE");
static TCollection_AsciiString Reco_OverRidingStyledItem ("OVER_RIDING_STYLED_ITEM");
static TCollection_AsciiString Reco_Parabola ("PARABOLA");
static TCollection_AsciiString Reco_ParametricRepresentationContext ("PARAMETRIC_REPRESENTATION_CONTEXT");
static TCollection_AsciiString Reco_Path ("PATH");
static TCollection_AsciiString Reco_Pcurve ("PCURVE");
static TCollection_AsciiString Reco_Person ("PERSON");
static TCollection_AsciiString Reco_PersonAndOrganization ("PERSON_AND_ORGANIZATION");
static TCollection_AsciiString Reco_PersonAndOrganizationAssignment ("PERSON_AND_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_PersonAndOrganizationRole ("PERSON_AND_ORGANIZATION_ROLE");
static TCollection_AsciiString Reco_PersonalAddress ("PERSONAL_ADDRESS");
static TCollection_AsciiString Reco_Placement ("PLACEMENT");
static TCollection_AsciiString Reco_PlanarBox ("PLANAR_BOX");
static TCollection_AsciiString Reco_PlanarExtent ("PLANAR_EXTENT");
static TCollection_AsciiString Reco_Plane ("PLANE");
static TCollection_AsciiString Reco_PlaneAngleMeasureWithUnit ("PLANE_ANGLE_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_PlaneAngleUnit ("PLANE_ANGLE_UNIT");
static TCollection_AsciiString Reco_Point ("POINT");
static TCollection_AsciiString Reco_PointOnCurve ("POINT_ON_CURVE");
static TCollection_AsciiString Reco_PointOnSurface ("POINT_ON_SURFACE");
static TCollection_AsciiString Reco_PointReplica ("POINT_REPLICA");
static TCollection_AsciiString Reco_PointStyle ("POINT_STYLE");
static TCollection_AsciiString Reco_PolyLoop ("POLY_LOOP");
static TCollection_AsciiString Reco_Polyline ("POLYLINE");
static TCollection_AsciiString Reco_PreDefinedColour ("PRE_DEFINED_COLOUR");
static TCollection_AsciiString Reco_PreDefinedCurveFont ("PRE_DEFINED_CURVE_FONT");
static TCollection_AsciiString Reco_PreDefinedItem ("PRE_DEFINED_ITEM");
static TCollection_AsciiString Reco_PreDefinedSymbol ("PRE_DEFINED_SYMBOL");
static TCollection_AsciiString Reco_PreDefinedTextFont ("PRE_DEFINED_TEXT_FONT");
static TCollection_AsciiString Reco_PresentationArea ("PRESENTATION_AREA");
static TCollection_AsciiString Reco_PresentationLayerAssignment ("PRESENTATION_LAYER_ASSIGNMENT");
static TCollection_AsciiString Reco_PresentationRepresentation ("PRESENTATION_REPRESENTATION");
static TCollection_AsciiString Reco_PresentationSet ("PRESENTATION_SET");
static TCollection_AsciiString Reco_PresentationSize ("PRESENTATION_SIZE");
static TCollection_AsciiString Reco_PresentationStyleAssignment ("PRESENTATION_STYLE_ASSIGNMENT");
static TCollection_AsciiString Reco_PresentationStyleByContext ("PRESENTATION_STYLE_BY_CONTEXT");
static TCollection_AsciiString Reco_PresentationView ("PRESENTATION_VIEW");
static TCollection_AsciiString Reco_PresentedItem ("PRESENTED_ITEM");
static TCollection_AsciiString Reco_Product ("PRODUCT");
static TCollection_AsciiString Reco_ProductCategory ("PRODUCT_CATEGORY");
static TCollection_AsciiString Reco_ProductContext ("PRODUCT_CONTEXT");
static TCollection_AsciiString Reco_ProductDataRepresentationView ("PRODUCT_DATA_REPRESENTATION_VIEW");
static TCollection_AsciiString Reco_ProductDefinition ("PRODUCT_DEFINITION");
static TCollection_AsciiString Reco_ProductDefinitionContext ("PRODUCT_DEFINITION_CONTEXT");
static TCollection_AsciiString Reco_ProductDefinitionFormation ("PRODUCT_DEFINITION_FORMATION");
static TCollection_AsciiString Reco_ProductDefinitionFormationWithSpecifiedSource ("PRODUCT_DEFINITION_FORMATION_WITH_SPECIFIED_SOURCE");
static TCollection_AsciiString Reco_ProductDefinitionShape ("PRODUCT_DEFINITION_SHAPE");
static TCollection_AsciiString Reco_ProductRelatedProductCategory ("PRODUCT_RELATED_PRODUCT_CATEGORY");
static TCollection_AsciiString Reco_ProductType ("PRODUCT_TYPE");
static TCollection_AsciiString Reco_PropertyDefinition ("PROPERTY_DEFINITION");
static TCollection_AsciiString Reco_PropertyDefinitionRepresentation ("PROPERTY_DEFINITION_REPRESENTATION");
static TCollection_AsciiString Reco_QuasiUniformCurve ("QUASI_UNIFORM_CURVE");
static TCollection_AsciiString Reco_QuasiUniformSurface ("QUASI_UNIFORM_SURFACE");
static TCollection_AsciiString Reco_RatioMeasureWithUnit ("RATIO_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_RationalBSplineCurve ("RATIONAL_B_SPLINE_CURVE");
static TCollection_AsciiString Reco_RationalBSplineSurface ("RATIONAL_B_SPLINE_SURFACE");
static TCollection_AsciiString Reco_RectangularCompositeSurface ("RECTANGULAR_COMPOSITE_SURFACE");
static TCollection_AsciiString Reco_RectangularTrimmedSurface ("RECTANGULAR_TRIMMED_SURFACE");
static TCollection_AsciiString Reco_RepItemGroup ("REP_ITEM_GROUP");
static TCollection_AsciiString Reco_ReparametrisedCompositeCurveSegment ("REPARAMETRISED_COMPOSITE_CURVE_SEGMENT");
static TCollection_AsciiString Reco_Representation ("REPRESENTATION");
static TCollection_AsciiString Reco_RepresentationContext ("REPRESENTATION_CONTEXT");
static TCollection_AsciiString Reco_RepresentationItem ("REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_RepresentationMap ("REPRESENTATION_MAP");
static TCollection_AsciiString Reco_RepresentationRelationship ("REPRESENTATION_RELATIONSHIP");
static TCollection_AsciiString Reco_RevolvedAreaSolid ("REVOLVED_AREA_SOLID");
static TCollection_AsciiString Reco_RightAngularWedge ("RIGHT_ANGULAR_WEDGE");
static TCollection_AsciiString Reco_RightCircularCone ("RIGHT_CIRCULAR_CONE");
static TCollection_AsciiString Reco_RightCircularCylinder ("RIGHT_CIRCULAR_CYLINDER");
static TCollection_AsciiString Reco_SeamCurve ("SEAM_CURVE");
static TCollection_AsciiString Reco_SecurityClassification ("SECURITY_CLASSIFICATION");
static TCollection_AsciiString Reco_SecurityClassificationAssignment ("SECURITY_CLASSIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_SecurityClassificationLevel ("SECURITY_CLASSIFICATION_LEVEL");
static TCollection_AsciiString Reco_ShapeAspect ("SHAPE_ASPECT");
static TCollection_AsciiString Reco_ShapeAspectRelationship ("SHAPE_ASPECT_RELATIONSHIP");
static TCollection_AsciiString Reco_ShapeAspectTransition ("SHAPE_ASPECT_TRANSITION");
static TCollection_AsciiString Reco_ShapeDefinitionRepresentation ("SHAPE_DEFINITION_REPRESENTATION");
static TCollection_AsciiString Reco_ShapeRepresentation ("SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_ShellBasedSurfaceModel ("SHELL_BASED_SURFACE_MODEL");
static TCollection_AsciiString Reco_SiUnit ("SI_UNIT");
static TCollection_AsciiString Reco_SolidAngleMeasureWithUnit ("SOLID_ANGLE_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_SolidModel ("SOLID_MODEL");
static TCollection_AsciiString Reco_SolidReplica ("SOLID_REPLICA");
static TCollection_AsciiString Reco_Sphere ("SPHERE");
static TCollection_AsciiString Reco_SphericalSurface ("SPHERICAL_SURFACE");
static TCollection_AsciiString Reco_StyledItem ("STYLED_ITEM");
static TCollection_AsciiString Reco_Surface ("SURFACE");
static TCollection_AsciiString Reco_SurfaceCurve ("SURFACE_CURVE");
static TCollection_AsciiString Reco_SurfaceOfLinearExtrusion ("SURFACE_OF_LINEAR_EXTRUSION");
static TCollection_AsciiString Reco_SurfaceOfRevolution ("SURFACE_OF_REVOLUTION");
static TCollection_AsciiString Reco_SurfacePatch ("SURFACE_PATCH");
static TCollection_AsciiString Reco_SurfaceReplica ("SURFACE_REPLICA");
static TCollection_AsciiString Reco_SurfaceSideStyle ("SURFACE_SIDE_STYLE");
static TCollection_AsciiString Reco_SurfaceStyleBoundary ("SURFACE_STYLE_BOUNDARY");
static TCollection_AsciiString Reco_SurfaceStyleControlGrid ("SURFACE_STYLE_CONTROL_GRID");
static TCollection_AsciiString Reco_SurfaceStyleFillArea ("SURFACE_STYLE_FILL_AREA");
static TCollection_AsciiString Reco_SurfaceStyleParameterLine ("SURFACE_STYLE_PARAMETER_LINE");
static TCollection_AsciiString Reco_SurfaceStyleSegmentationCurve ("SURFACE_STYLE_SEGMENTATION_CURVE");
static TCollection_AsciiString Reco_SurfaceStyleSilhouette ("SURFACE_STYLE_SILHOUETTE");
static TCollection_AsciiString Reco_SurfaceStyleUsage ("SURFACE_STYLE_USAGE");
static TCollection_AsciiString Reco_SweptAreaSolid ("SWEPT_AREA_SOLID");
static TCollection_AsciiString Reco_SweptSurface ("SWEPT_SURFACE");
static TCollection_AsciiString Reco_SymbolColour ("SYMBOL_COLOUR");
static TCollection_AsciiString Reco_SymbolRepresentation ("SYMBOL_REPRESENTATION");
static TCollection_AsciiString Reco_SymbolRepresentationMap ("SYMBOL_REPRESENTATION_MAP");
static TCollection_AsciiString Reco_SymbolStyle ("SYMBOL_STYLE");
static TCollection_AsciiString Reco_SymbolTarget ("SYMBOL_TARGET");
static TCollection_AsciiString Reco_Template ("TEMPLATE");
static TCollection_AsciiString Reco_TemplateInstance ("TEMPLATE_INSTANCE");
static TCollection_AsciiString Reco_TerminatorSymbol ("TERMINATOR_SYMBOL");
static TCollection_AsciiString Reco_TextLiteral ("TEXT_LITERAL");
static TCollection_AsciiString Reco_TextLiteralWithAssociatedCurves ("TEXT_LITERAL_WITH_ASSOCIATED_CURVES");
static TCollection_AsciiString Reco_TextLiteralWithBlankingBox ("TEXT_LITERAL_WITH_BLANKING_BOX");
static TCollection_AsciiString Reco_TextLiteralWithDelineation ("TEXT_LITERAL_WITH_DELINEATION");
static TCollection_AsciiString Reco_TextLiteralWithExtent ("TEXT_LITERAL_WITH_EXTENT");
static TCollection_AsciiString Reco_TextStyle ("TEXT_STYLE");
static TCollection_AsciiString Reco_TextStyleForDefinedFont ("TEXT_STYLE_FOR_DEFINED_FONT");
static TCollection_AsciiString Reco_TextStyleWithBoxCharacteristics ("TEXT_STYLE_WITH_BOX_CHARACTERISTICS");
static TCollection_AsciiString Reco_TextStyleWithMirror ("TEXT_STYLE_WITH_MIRROR");
static TCollection_AsciiString Reco_TopologicalRepresentationItem ("TOPOLOGICAL_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_ToroidalSurface ("TOROIDAL_SURFACE");
static TCollection_AsciiString Reco_Torus ("TORUS");
static TCollection_AsciiString Reco_TransitionalShapeRepresentation ("TRANSITIONAL_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_TrimmedCurve ("TRIMMED_CURVE");
static TCollection_AsciiString Reco_TwoDirectionRepeatFactor ("TWO_DIRECTION_REPEAT_FACTOR");
static TCollection_AsciiString Reco_UncertaintyMeasureWithUnit ("UNCERTAINTY_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_UniformCurve ("UNIFORM_CURVE");
static TCollection_AsciiString Reco_UniformSurface ("UNIFORM_SURFACE");
static TCollection_AsciiString Reco_Vector ("VECTOR");
static TCollection_AsciiString Reco_Vertex ("VERTEX");
static TCollection_AsciiString Reco_VertexLoop ("VERTEX_LOOP");
static TCollection_AsciiString Reco_VertexPoint ("VERTEX_POINT");
static TCollection_AsciiString Reco_ViewVolume ("VIEW_VOLUME");
static TCollection_AsciiString Reco_WeekOfYearAndDayDate ("WEEK_OF_YEAR_AND_DAY_DATE");

// Added by FMA for Rev4

static TCollection_AsciiString Reco_SolidAngleUnit("SOLID_ANGLE_UNIT");
static TCollection_AsciiString Reco_MechanicalContext("MECHANICAL_CONTEXT");
static TCollection_AsciiString Reco_DesignContext("DESIGN_CONTEXT");

// Added for full Rev4
static TCollection_AsciiString Reco_TimeMeasureWithUnit("TIME_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_RatioUnit("RATIO_UNIT");
static TCollection_AsciiString Reco_TimeUnit("TIME_UNIT");
static TCollection_AsciiString Reco_ApprovalDateTime("APPROVAL_DATE_TIME");
static TCollection_AsciiString Reco_CameraImage2dWithScale("CAMERA_IMAGE_2D_WITH_SCALE");
static TCollection_AsciiString Reco_CameraImage3dWithScale("CAMERA_IMAGE_3D_WITH_SCALE");
static TCollection_AsciiString Reco_CartesianTransformationOperator2d("CARTESIAN_TRANSFORMATION_OPERATOR_2D");
static TCollection_AsciiString Reco_DerivedUnit("DERIVED_UNIT");
static TCollection_AsciiString Reco_DerivedUnitElement("DERIVED_UNIT_ELEMENT");
static TCollection_AsciiString Reco_ItemDefinedTransformation("ITEM_DEFINED_TRANSFORMATION");
static TCollection_AsciiString Reco_PresentedItemRepresentation("PRESENTED_ITEM_REPRESENTATION");
static TCollection_AsciiString Reco_PresentationLayerUsage("PRESENTATION_LAYER_USAGE");

// Added for AP214 : CC1 -> CC2

static TCollection_AsciiString Reco_AutoDesignDocumentReference("AUTO_DESIGN_DOCUMENT_REFERENCE");
static TCollection_AsciiString Reco_Document("DOCUMENT");
static TCollection_AsciiString Reco_DigitalDocument("DIGITAL_DOCUMENT");
static TCollection_AsciiString Reco_DocumentRelationship("DOCUMENT_RELATIONSHIP");
static TCollection_AsciiString Reco_DocumentType("DOCUMENT_TYPE");
static TCollection_AsciiString Reco_DocumentUsageConstraint("DOCUMENT_USAGE_CONSTRAINT");
static TCollection_AsciiString Reco_Effectivity("EFFECTIVITY");
static TCollection_AsciiString Reco_ProductDefinitionEffectivity("PRODUCT_DEFINITION_EFFECTIVITY");
static TCollection_AsciiString Reco_ProductDefinitionRelationship("PRODUCT_DEFINITION_RELATIONSHIP");
static TCollection_AsciiString Reco_ProductDefinitionWithAssociatedDocuments("PRODUCT_DEFINITION_WITH_ASSOCIATED_DOCUMENTS");
static TCollection_AsciiString Reco_PhysicallyModeledProductDefinition("PHYSICALLY_MODELED_PRODUCT_DEFINITION");

static TCollection_AsciiString Reco_ProductDefinitionUsage("PRODUCT_DEFINITION_USAGE");
static TCollection_AsciiString Reco_MakeFromUsageOption("MAKE_FROM_USAGE_OPTION");
static TCollection_AsciiString Reco_AssemblyComponentUsage("ASSEMBLY_COMPONENT_USAGE");
static TCollection_AsciiString Reco_NextAssemblyUsageOccurrence("NEXT_ASSEMBLY_USAGE_OCCURRENCE");
static TCollection_AsciiString Reco_PromissoryUsageOccurrence("PROMISSORY_USAGE_OCCURRENCE");
static TCollection_AsciiString Reco_QuantifiedAssemblyComponentUsage("QUANTIFIED_ASSEMBLY_COMPONENT_USAGE");
static TCollection_AsciiString Reco_SpecifiedHigherUsageOccurrence("SPECIFIED_HIGHER_USAGE_OCCURRENCE");
static TCollection_AsciiString Reco_AssemblyComponentUsageSubstitute("ASSEMBLY_COMPONENT_USAGE_SUBSTITUTE");
static TCollection_AsciiString Reco_SuppliedPartRelationship("SUPPLIED_PART_RELATIONSHIP");
static TCollection_AsciiString Reco_ExternallyDefinedRepresentation("EXTERNALLY_DEFINED_REPRESENTATION");
static TCollection_AsciiString Reco_ShapeRepresentationRelationship("SHAPE_REPRESENTATION_RELATIONSHIP");
static TCollection_AsciiString Reco_RepresentationRelationshipWithTransformation("REPRESENTATION_RELATIONSHIP_WITH_TRANSFORMATION");
static TCollection_AsciiString Reco_MaterialDesignation("MATERIAL_DESIGNATION");
static TCollection_AsciiString Reco_ContextDependentShapeRepresentation ("CONTEXT_DEPENDENT_SHAPE_REPRESENTATION");

// Added from CD To DIS (S4134)
static TCollection_AsciiString Reco_AppliedDateAndTimeAssignment ("APPLIED_DATE_AND_TIME_ASSIGNMENT"); 
static TCollection_AsciiString Reco_AppliedDateAssignment ("APPLIED_DATE_ASSIGNMENT");
static TCollection_AsciiString Reco_AppliedApprovalAssignment ("APPLIED_APPROVAL_ASSIGNMENT");
static TCollection_AsciiString Reco_AppliedDocumentReference ("APPLIED_DOCUMENT_REFERENCE");
static TCollection_AsciiString Reco_AppliedGroupAssignment ("APPLIED_GROUP_ASSIGNMENT");
static TCollection_AsciiString Reco_AppliedOrganizationAssignment ("APPLIED_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_AppliedPersonAndOrganizationAssignment ("APPLIED_PERSON_AND_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_AppliedPresentedItem ("APPLIED_PRESENTED_ITEM");
static TCollection_AsciiString Reco_AppliedSecurityClassificationAssignment ("APPLIED_SECURITY_CLASSIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_DocumentFile ("DOCUMENT_FILE");
static TCollection_AsciiString Reco_CharacterizedObject ("CHARACTERIZED_OBJECT");
static TCollection_AsciiString Reco_ExtrudedFaceSolid ("EXTRUDED_FACE_SOLID");
static TCollection_AsciiString Reco_RevolvedFaceSolid ("REVOLVED_FACE_SOLID");
static TCollection_AsciiString Reco_SweptFaceSolid ("SWEPT_FACE_SOLID");

// Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
static TCollection_AsciiString Reco_MeasureRepresentationItem ("MEASURE_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_AreaUnit("AREA_UNIT");
static TCollection_AsciiString Reco_VolumeUnit("VOLUME_UNIT");

// Added by ABV 10.11.99 for AP203
static TCollection_AsciiString Reco_Action ("ACTION");
static TCollection_AsciiString Reco_ActionAssignment ("ACTION_ASSIGNMENT");
static TCollection_AsciiString Reco_ActionMethod ("ACTION_METHOD");
static TCollection_AsciiString Reco_ActionRequestAssignment ("ACTION_REQUEST_ASSIGNMENT");
static TCollection_AsciiString Reco_CcDesignApproval ("CC_DESIGN_APPROVAL");
static TCollection_AsciiString Reco_CcDesignCertification ("CC_DESIGN_CERTIFICATION");
static TCollection_AsciiString Reco_CcDesignContract ("CC_DESIGN_CONTRACT");
static TCollection_AsciiString Reco_CcDesignDateAndTimeAssignment ("CC_DESIGN_DATE_AND_TIME_ASSIGNMENT");
static TCollection_AsciiString Reco_CcDesignPersonAndOrganizationAssignment ("CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT");
static TCollection_AsciiString Reco_CcDesignSecurityClassification ("CC_DESIGN_SECURITY_CLASSIFICATION");
static TCollection_AsciiString Reco_CcDesignSpecificationReference ("CC_DESIGN_SPECIFICATION_REFERENCE");
static TCollection_AsciiString Reco_Certification ("CERTIFICATION");
static TCollection_AsciiString Reco_CertificationAssignment ("CERTIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_CertificationType ("CERTIFICATION_TYPE");
static TCollection_AsciiString Reco_Change ("CHANGE");
static TCollection_AsciiString Reco_ChangeRequest ("CHANGE_REQUEST");
static TCollection_AsciiString Reco_ConfigurationDesign ("CONFIGURATION_DESIGN");
static TCollection_AsciiString Reco_ConfigurationEffectivity ("CONFIGURATION_EFFECTIVITY");
static TCollection_AsciiString Reco_Contract ("CONTRACT");
static TCollection_AsciiString Reco_ContractAssignment ("CONTRACT_ASSIGNMENT");
static TCollection_AsciiString Reco_ContractType ("CONTRACT_TYPE");
static TCollection_AsciiString Reco_ProductConcept ("PRODUCT_CONCEPT");
static TCollection_AsciiString Reco_ProductConceptContext ("PRODUCT_CONCEPT_CONTEXT");
static TCollection_AsciiString Reco_StartRequest ("START_REQUEST");
static TCollection_AsciiString Reco_StartWork ("START_WORK");
static TCollection_AsciiString Reco_VersionedActionRequest ("VERSIONED_ACTION_REQUEST");
static TCollection_AsciiString Reco_ProductCategoryRelationship ("PRODUCT_CATEGORY_RELATIONSHIP");
static TCollection_AsciiString Reco_ActionRequestSolution ("ACTION_REQUEST_SOLUTION");
static TCollection_AsciiString Reco_DraughtingModel ("DRAUGHTING_MODEL");
// Added by ABV 18.04.00 for CAX-IF TRJ4
static TCollection_AsciiString Reco_AngularLocation ("ANGULAR_LOCATION");
static TCollection_AsciiString Reco_AngularSize ("ANGULAR_SIZE");
static TCollection_AsciiString Reco_DimensionalCharacteristicRepresentation ("DIMENSIONAL_CHARACTERISTIC_REPRESENTATION");
static TCollection_AsciiString Reco_DimensionalLocation ("DIMENSIONAL_LOCATION");
static TCollection_AsciiString Reco_DimensionalLocationWithPath ("DIMENSIONAL_LOCATION_WITH_PATH");
static TCollection_AsciiString Reco_DimensionalSize ("DIMENSIONAL_SIZE");
static TCollection_AsciiString Reco_DimensionalSizeWithPath ("DIMENSIONAL_SIZE_WITH_PATH");
static TCollection_AsciiString Reco_ShapeDimensionRepresentation ("SHAPE_DIMENSION_REPRESENTATION");
// Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
static TCollection_AsciiString Reco_DocumentRepresentationType ("DOCUMENT_REPRESENTATION_TYPE");
static TCollection_AsciiString Reco_ObjectRole ("OBJECT_ROLE");
static TCollection_AsciiString Reco_RoleAssociation ("ROLE_ASSOCIATION");
static TCollection_AsciiString Reco_IdentificationRole ("IDENTIFICATION_ROLE");
static TCollection_AsciiString Reco_IdentificationAssignment ("IDENTIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_ExternalIdentificationAssignment ("EXTERNAL_IDENTIFICATION_ASSIGNMENT");
static TCollection_AsciiString Reco_EffectivityAssignment ("EFFECTIVITY_ASSIGNMENT");
static TCollection_AsciiString Reco_NameAssignment ("NAME_ASSIGNMENT");
static TCollection_AsciiString Reco_GeneralProperty ("GENERAL_PROPERTY");
static TCollection_AsciiString Reco_Class ("CLASS");
static TCollection_AsciiString Reco_ExternallyDefinedClass ("EXTERNALLY_DEFINED_Class");
static TCollection_AsciiString Reco_ExternallyDefinedGeneralProperty ("EXTERNALLY_DEFINED_GENERAL_PROPERTY");
static TCollection_AsciiString Reco_AppliedExternalIdentificationAssignment ("APPLIED_EXTERNAL_IDENTIFICATION_ASSIGNMENT");
// Added by CKY , 25 APR 2001 for Dimensional Tolerances (CAX-IF TRJ7)
static TCollection_AsciiString Reco_CompositeShapeAspect ("COMPOSITE_SHAPE_ASPECT");
static TCollection_AsciiString Reco_DerivedShapeAspect   ("DERIVED_SHAPE_ASPECT");
static TCollection_AsciiString Reco_Extension  ("EXTENSION");
static TCollection_AsciiString Reco_DirectedDimensionalLocation ("DIRECTED_DIMENSIONAL_LOCATION");
static TCollection_AsciiString Reco_LimitsAndFits  ("LIMITS_AND_FITS");
static TCollection_AsciiString Reco_ToleranceValue ("TOLERANCE_VALUE");
static TCollection_AsciiString Reco_MeasureQualification ("MEASURE_QUALIFICATION");
static TCollection_AsciiString Reco_PlusMinusTolerance   ("PLUS_MINUS_TOLERANCE");
static TCollection_AsciiString Reco_PrecisionQualifier   ("PRECISION_QUALIFIER");
static TCollection_AsciiString Reco_TypeQualifier   ("TYPE_QUALIFIER");
static TCollection_AsciiString Reco_QualifiedRepresentationItem ("QUALIFIED_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_CompoundRepresentationItem  ("COMPOUND_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_ValueRange  ("VALUE_RANGE");
static TCollection_AsciiString Reco_ShapeAspectDerivingRelationship ("SHAPE_ASPECT_DERIVING_RELATIONSHIP");

static TCollection_AsciiString Reco_CompoundShapeRepresentation ("COMPOUND_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_ConnectedEdgeSet ("CONNECTED_EDGE_SET");
static TCollection_AsciiString Reco_ConnectedFaceShapeRepresentation ("CONNECTED_FACE_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_EdgeBasedWireframeModel ("EDGE_BASED_WIREFRAME_MODEL");
static TCollection_AsciiString Reco_EdgeBasedWireframeShapeRepresentation ("EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION");
static TCollection_AsciiString Reco_FaceBasedSurfaceModel ("FACE_BASED_SURFACE_MODEL");
static TCollection_AsciiString Reco_NonManifoldSurfaceShapeRepresentation ("NON_MANIFOLD_SURFACE_SHAPE_REPRESENTATION");

//gka 08.01.02
static TCollection_AsciiString Reco_OrientedSurface ("ORIENTED_SURFACE");
static TCollection_AsciiString Reco_Subface ("SUBFACE");
static TCollection_AsciiString Reco_Subedge ("SUBEDGE");
static TCollection_AsciiString Reco_SeamEdge("SEAM_EDGE");
static TCollection_AsciiString Reco_ConnectedFaceSubSet("CONNECTED_FACE_SUB_SET");

//Added for AP209
static TCollection_AsciiString Reco_EulerAngles("EULER_ANGLES");
static TCollection_AsciiString Reco_MassUnit("MASS_UNIT");
static TCollection_AsciiString Reco_MassMeasureWithUnit ("MASS_MEASURE_WITH_UNIT");
static TCollection_AsciiString Reco_ThermodynamicTemperatureUnit("THERMODYNAMIC_TEMPERATURE_UNIT");
static TCollection_AsciiString Reco_AnalysisItemWithinRepresentation("ANALYSIS_ITEM_WITHIN_REPRESENTATION");
static TCollection_AsciiString Reco_Curve3dElementDescriptor("CURVE_3D_ELEMENT_DESCRIPTOR");
static TCollection_AsciiString Reco_CurveElementEndReleasePacket("CURVE_ELEMENT_END_RELEASE_PACKET");
static TCollection_AsciiString Reco_CurveElementSectionDefinition("CURVE_ELEMENT_SECTION_DEFINITION");
static TCollection_AsciiString Reco_CurveElementSectionDerivedDefinitions("CURVE_ELEMENT_SECTION_DERIVED_DEFINITIONS");
static TCollection_AsciiString Reco_ElementDescriptor("ELEMENT_DESCRIPTOR");
static TCollection_AsciiString Reco_ElementMaterial("ELEMENT_MATERIAL");
static TCollection_AsciiString Reco_Surface3dElementDescriptor("SURFACE_3D_ELEMENT_DESCRIPTOR");
static TCollection_AsciiString Reco_SurfaceElementProperty("SURFACE_ELEMENT_PROPERTY");
static TCollection_AsciiString Reco_SurfaceSection("SURFACE_SECTION");
static TCollection_AsciiString Reco_SurfaceSectionField("SURFACE_SECTION_FIELD");
static TCollection_AsciiString Reco_SurfaceSectionFieldConstant("SURFACE_SECTION_FIELD_CONSTANT");
static TCollection_AsciiString Reco_SurfaceSectionFieldVarying("SURFACE_SECTION_FIELD_VARYING");
static TCollection_AsciiString Reco_UniformSurfaceSection("UNIFORM_SURFACE_SECTION");
static TCollection_AsciiString Reco_Volume3dElementDescriptor("VOLUME_3D_ELEMENT_DESCRIPTOR");
static TCollection_AsciiString Reco_AlignedCurve3dElementCoordinateSystem("ALIGNED_CURVE_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_ArbitraryVolume3dElementCoordinateSystem("ARBITRARY_VOLUME_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_Curve3dElementProperty("CURVE_3D_ELEMENT_PROPERTY");
static TCollection_AsciiString Reco_Curve3dElementRepresentation("CURVE_3D_ELEMENT_REPRESENTATION");
static TCollection_AsciiString Reco_Node("NODE");
//static TCollection_AsciiString Reco_CurveElementEndCoordinateSystem(" ");
static TCollection_AsciiString Reco_CurveElementEndOffset("CURVE_ELEMENT_END_OFFSET");
static TCollection_AsciiString Reco_CurveElementEndRelease("CURVE_ELEMENT_END_RELEASE");
static TCollection_AsciiString Reco_CurveElementInterval("CURVE_ELEMENT_INTERVAL");
static TCollection_AsciiString Reco_CurveElementIntervalConstant("CURVE_ELEMENT_INTERVAL_CONSTANT");
static TCollection_AsciiString Reco_DummyNode("DUMMY_NODE");
static TCollection_AsciiString Reco_CurveElementLocation("CURVE_ELEMENT_LOCATION");
static TCollection_AsciiString Reco_ElementGeometricRelationship("ELEMENT_GEOMETRIC_RELATIONSHIP");
static TCollection_AsciiString Reco_ElementGroup("ELEMENT_GROUP");
static TCollection_AsciiString Reco_ElementRepresentation("ELEMENT_REPRESENTATION");
static TCollection_AsciiString Reco_FeaAreaDensity("FEA_AREA_DENSITY");
static TCollection_AsciiString Reco_FeaAxis2Placement3d("FEA_AXIS2_PLACEMENT_3D");
static TCollection_AsciiString Reco_FeaGroup("FEA_GROUP");
static TCollection_AsciiString Reco_FeaLinearElasticity("FEA_LINEAR_ELASTICITY");
static TCollection_AsciiString Reco_FeaMassDensity("FEA_MASS_DENSITY");
static TCollection_AsciiString Reco_FeaMaterialPropertyRepresentation("FEA_MATERIAL_PROPERTY_REPRESENTATION");
static TCollection_AsciiString Reco_FeaMaterialPropertyRepresentationItem("FEA_MATERIAL_PROPERTY_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_FeaModel("FEA_MODEL");
static TCollection_AsciiString Reco_FeaModel3d("FEA_MODEL_3D");
static TCollection_AsciiString Reco_FeaMoistureAbsorption("FEA_MOISTURE_ABSORPTION");
static TCollection_AsciiString Reco_FeaParametricPoint("FEA_PARAMETRIC_POINT");
static TCollection_AsciiString Reco_FeaRepresentationItem("FEA_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_FeaSecantCoefficientOfLinearThermalExpansion("FEA_SECANT_COEFFICIENT_OF_LINEAR_THERMAL_EXPANSION");
static TCollection_AsciiString Reco_FeaShellBendingStiffness("FEA_SHELL_BENDING_STIFFNESS");
static TCollection_AsciiString Reco_FeaShellMembraneBendingCouplingStiffness("FEA_SHELL_MEMBRANE_BENDING_COUPLING_STIFFNESS");
static TCollection_AsciiString Reco_FeaShellMembraneStiffness("FEA_SHELL_MEMBRANE_STIFFNESS");
static TCollection_AsciiString Reco_FeaShellShearStiffness("FEA_SHELL_SHEAR_STIFFNESS");
static TCollection_AsciiString Reco_GeometricNode("GEOMETRIC_NODE");
static TCollection_AsciiString Reco_FeaTangentialCoefficientOfLinearThermalExpansion("FEA_TANGENTIAL_COEFFICIENT_OF_LINEAR_THERMAL_EXPANSION");
static TCollection_AsciiString Reco_NodeGroup("NODE_GROUP");
static TCollection_AsciiString Reco_NodeRepresentation("NODE_REPRESENTATION");
static TCollection_AsciiString Reco_NodeSet("NODE_SET");
static TCollection_AsciiString Reco_NodeWithSolutionCoordinateSystem("NODE_WITH_SOLUTION_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_NodeWithVector("NODE_WITH_VECTOR");
static TCollection_AsciiString Reco_ParametricCurve3dElementCoordinateDirection("PARAMETRIC_CURVE_3D_ELEMENT_COORDINATE_DIRECTION");
static TCollection_AsciiString Reco_ParametricCurve3dElementCoordinateSystem("PARAMETRIC_CURVE_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_ParametricSurface3dElementCoordinateSystem("PARAMETRIC_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_Surface3dElementRepresentation("SURFACE_3D_ELEMENT_REPRESENTATION");
//static TCollection_AsciiString Reco_SymmetricTensor22d(" ");
//static TCollection_AsciiString Reco_SymmetricTensor42d(" ");
//static TCollection_AsciiString Reco_SymmetricTensor43d(" ");
static TCollection_AsciiString Reco_Volume3dElementRepresentation("VOLUME_3D_ELEMENT_REPRESENTATION");
static TCollection_AsciiString Reco_DataEnvironment("DATA_ENVIRONMENT");
static TCollection_AsciiString Reco_MaterialPropertyRepresentation("MATERIAL_PROPERTY_REPRESENTATION");
static TCollection_AsciiString Reco_PropertyDefinitionRelationship("PROPERTY_DEFINITION_RELATIONSHIP");
static TCollection_AsciiString Reco_PointRepresentation("POINT_REPRESENTATION");
static TCollection_AsciiString Reco_MaterialProperty("MATERIAL_PROPERTY");
static TCollection_AsciiString Reco_FeaModelDefinition("FEA_MODEL_DEFINITION");
static TCollection_AsciiString Reco_FreedomAndCoefficient("FREEDOM_AND_COEFFICIENT");
static TCollection_AsciiString Reco_FreedomsList("FREEDOMS_LIST");
static TCollection_AsciiString Reco_ProductDefinitionFormationRelationship("PRODUCT_DEFINITION_FORMATION_RELATIONSHIP");
//static TCollection_AsciiString Reco_FeaModelDefinition("FEA_MODEL_DEFINITION");
static TCollection_AsciiString Reco_NodeDefinition("NODE_DEFINITION");
static TCollection_AsciiString Reco_StructuralResponseProperty("STRUCTURAL_RESPONSE_PROPERTY");
static TCollection_AsciiString Reco_StructuralResponsePropertyDefinitionRepresentation("STRUCTURAL_RESPONSE_PROPERTY_DEFINITION_REPRESENTATION");
static TCollection_AsciiString Reco_AlignedSurface3dElementCoordinateSystem("ALIGNED_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_ConstantSurface3dElementCoordinateSystem("CONSTANT_SURFACE_3D_ELEMENT_COORDINATE_SYSTEM");
static TCollection_AsciiString Reco_CurveElementIntervalLinearlyVarying("CURVE_ELEMENT_INTERVAL_LINEARLY_VARYING");
static TCollection_AsciiString Reco_FeaCurveSectionGeometricRelationship("FEA_CURVE_SECTION_GEOMETRIC_RELATIONSHIP");
static TCollection_AsciiString Reco_FeaSurfaceSectionGeometricRelationship("FEA_SURFACE_SECTION_GEOMETRIC_RELATIONSHIP");

// PTV 28.01.2003 TRJ11 AP214 external references
static TCollection_AsciiString Reco_DocumentProductAssociation ("DOCUMENT_PRODUCT_ASSOCIATION");
static TCollection_AsciiString Reco_DocumentProductEquivalence ("DOCUMENT_PRODUCT_EQUIVALENCE");

// Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
static TCollection_AsciiString Reco_ShapeRepresentationWithParameters("SHAPE_REPRESENTATION_WITH_PARAMETERS");
static TCollection_AsciiString Reco_AngularityTolerance("ANGULARITY_TOLERANCE");
static TCollection_AsciiString Reco_ConcentricityTolerance("CONCENTRICITY_TOLERANCE");
static TCollection_AsciiString Reco_CircularRunoutTolerance("CIRCULAR_RUNOUT_TOLERANCE");
static TCollection_AsciiString Reco_CoaxialityTolerance("COAXIALITY_TOLERANCE");
static TCollection_AsciiString Reco_CylindricityTolerance("CYLINDRICITY_TOLERANCE");
static TCollection_AsciiString Reco_FlatnessTolerance("FLATNESS_TOLERANCE");
static TCollection_AsciiString Reco_LineProfileTolerance("LINE_PROFILE_TOLERANCE");
static TCollection_AsciiString Reco_ParallelismTolerance("PARALLELISM_TOLERANCE");
static TCollection_AsciiString Reco_PerpendicularityTolerance("PERPENDICULARITY_TOLERANCE");
static TCollection_AsciiString Reco_PositionTolerance("POSITION_TOLERANCE");
static TCollection_AsciiString Reco_RoundnessTolerance("ROUNDNESS_TOLERANCE");
static TCollection_AsciiString Reco_StraightnessTolerance("STRAIGHTNESS_TOLERANCE");
static TCollection_AsciiString Reco_SurfaceProfileTolerance("SURFACE_PROFILE_TOLERANCE");
static TCollection_AsciiString Reco_SymmetryTolerance("SYMMETRY_TOLERANCE");
static TCollection_AsciiString Reco_TotalRunoutTolerance("TOTAL_RUNOUT_TOLERANCE");
static TCollection_AsciiString Reco_GeometricTolerance("GEOMETRIC_TOLERANCE");
static TCollection_AsciiString Reco_GeometricToleranceRelationship("GEOMETRIC_TOLERANCE_RELATIONSHIP");
static TCollection_AsciiString Reco_GeometricToleranceWithDatumReference("GEOMETRIC_TOLERANCE_WITH_DATUM_REFERENCE");
static TCollection_AsciiString Reco_ModifiedGeometricTolerance("MODIFIED_GEOMETRIC_TOLERANCE");
static TCollection_AsciiString Reco_Datum("DATUM");
static TCollection_AsciiString Reco_DatumFeature("DATUM_FEATURE");
static TCollection_AsciiString Reco_DatumReference("DATUM_REFERENCE");
static TCollection_AsciiString Reco_CommonDatum("COMMON_DATUM");
static TCollection_AsciiString Reco_DatumTarget("DATUM_TARGET");
static TCollection_AsciiString Reco_PlacedDatumTargetFeature("PLACED_DATUM_TARGET_FEATURE");

//Added by ika for GD&T AP242
static TCollection_AsciiString Reco_Apex("APEX");
static TCollection_AsciiString Reco_CentreOfSymmetry("CENTRE_OF_SYMMETRY");
static TCollection_AsciiString Reco_GeometricAlignment("GEOMETRIC_ALIGNMENT");
static TCollection_AsciiString Reco_PerpendicularTo("PERPENDICULAR_TO");
static TCollection_AsciiString Reco_Tangent("TANGENT");
static TCollection_AsciiString Reco_ParallelOffset("PARALLEL_OFFSET");
static TCollection_AsciiString Reco_GeometricItemSpecificUsage("GEOMETRIC_ITEM_SPECIFIC_USAGE");
static TCollection_AsciiString Reco_IdAttribute("ID_ATTRIBUTE");
static TCollection_AsciiString Reco_ItemIdentifiedRepresentationUsage("ITEM_IDENTIFIED_REPRESENTATION_USAGE");
static TCollection_AsciiString Reco_AllAroundShapeAspect("ALL_AROUND_SHAPE_ASPECT");
static TCollection_AsciiString Reco_BetweenShapeAspect("BETWEEN_SHAPE_ASPECT");
static TCollection_AsciiString Reco_CompositeGroupShapeAspect("COMPOSITE_GROUP_SHAPE_ASPECT");
static TCollection_AsciiString Reco_ContinuosShapeAspect("CONTINUOUS_SHAPE_ASPECT");
static TCollection_AsciiString Reco_GeometricToleranceWithDefinedAreaUnit("GEOMETRIC_TOLERANCE_WITH_DEFINED_AREA_UNIT");
static TCollection_AsciiString Reco_GeometricToleranceWithDefinedUnit("GEOMETRIC_TOLERANCE_WITH_DEFINED_UNIT");
static TCollection_AsciiString Reco_GeometricToleranceWithMaximumTolerance("GEOMETRIC_TOLERANCE_WITH_MAXIMUM_TOLERANCE");
static TCollection_AsciiString Reco_GeometricToleranceWithModifiers("GEOMETRIC_TOLERANCE_WITH_MODIFIERS");
static TCollection_AsciiString Reco_UnequallyDisposedGeometricTolerance("UNEQUALLY_DISPOSED_GEOMETRIC_TOLERANCE");
static TCollection_AsciiString Reco_NonUniformZoneDefinition("NON_UNIFORM_ZONE_DEFINITION");
static TCollection_AsciiString Reco_ProjectedZoneDefinition("PROJECTED_ZONE_DEFINITION");
static TCollection_AsciiString Reco_RunoutZoneDefinition("RUNOUT_ZONE_DEFINITION");
static TCollection_AsciiString Reco_RunoutZoneOrientation("RUNOUT_ZONE_ORIENTATION");
static TCollection_AsciiString Reco_ToleranceZone("TOLERANCE_ZONE");
static TCollection_AsciiString Reco_ToleranceZoneDefinition("TOLERANCE_ZONE_DEFINITION");
static TCollection_AsciiString Reco_ToleranceZoneForm("TOLERANCE_ZONE_FORM");
static TCollection_AsciiString Reco_ValueFormatTypeQualifier("VALUE_FORMAT_TYPE_QUALIFIER");
static TCollection_AsciiString Reco_DatumReferenceCompartment("DATUM_REFERENCE_COMPARTMENT");
static TCollection_AsciiString Reco_DatumReferenceElement("DATUM_REFERENCE_ELEMENT");
static TCollection_AsciiString Reco_DatumReferenceModifierWithValue("DATUM_REFERENCE_MODIFIER_WITH_VALUE");
static TCollection_AsciiString Reco_DatumSystem("DATUM_SYSTEM");
static TCollection_AsciiString Reco_GeneralDatumReference("GENERAL_DATUM_REFERENCE");
static TCollection_AsciiString Reco_IntegerRepresentationItem("INTEGER_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_ValueRepresentationItem("VALUE_REPRESENTATION_ITEM");
static TCollection_AsciiString Reco_FeatureForDatumTargetRelationship("FEATURE_FOR_DATUM_TARGET_RELATIONSHIP");
static TCollection_AsciiString Reco_DraughtingModelItemAssociation("DRAUGHTING_MODEL_ITEM_ASSOCIATION");
static TCollection_AsciiString Reco_AnnotationPlane("ANNOTATION_PLANE");

static TCollection_AsciiString Reco_TessellatedAnnotationOccurrence("TESSELLATED_ANNOTATION_OCCURRENCE");
static TCollection_AsciiString Reco_TessellatedGeometricSet("TESSELLATED_GEOMETRIC_SET");
static TCollection_AsciiString Reco_TessellatedCurveSet("TESSELLATED_CURVE_SET");
static TCollection_AsciiString Reco_CoordinatesList("COORDINATES_LIST");
static TCollection_AsciiString Reco_ConstructiveGeometryRepresentation("CONSTRUCTIVE_GEOMETRY_REPRESENTATION");
static TCollection_AsciiString Reco_ConstructiveGeometryRepresentationRelationship("CONSTRUCTIVE_GEOMETRY_REPRESENTATION_RELATIONSHIP");
static TCollection_AsciiString Reco_CharacterizedRepresentation("CHARACTERIZED_REPRESENTATION");
static TCollection_AsciiString Reco_CameraModelD3MultiClipping("CAMERA_MODEL_D3_MULTI_CLIPPING");
static TCollection_AsciiString Reco_CameraModelD3MultiClippingIntersection("CAMERA_MODEL_D3_MULTI_CLIPPING_INTERSECTION");
static TCollection_AsciiString Reco_CameraModelD3MultiClippingUnion("CAMERA_MODEL_D3_MULTI_CLIPPING_UNION");
// -- Definition of the libraries --

static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> typenums;
static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> typeshor;

RWStepAP214_ReadWriteModule::RWStepAP214_ReadWriteModule ()
{
//  Handle(StepAP214_Protocol) protocol = new StepAP214_Protocol;
//  StepData_WriterLib::SetGlobal(Handle(RWStepAP214_ReadWriteModule)::DownCast(This()),protocol);
//  Interface_ReaderLib::SetGlobal(Handle(RWStepAP214_ReadWriteModule)::DownCast(This()),protocol);
  if (!typenums.IsEmpty()) return;
  typenums.Bind (Reco_CartesianPoint, 59);
  typenums.Bind (Reco_VertexPoint, 316);
  typenums.Bind (Reco_OrientedEdge, 181);
  typenums.Bind (Reco_EdgeCurve, 116);
  typenums.Bind (Reco_SurfaceCurve, 272);
  typenums.Bind (Reco_EdgeLoop, 117);
  typenums.Bind (Reco_AdvancedFace, 3);
  typenums.Bind (Reco_FaceBound, 131);
  typenums.Bind (Reco_FaceOuterBound, 132);
  typenums.Bind (Reco_Direction, 105);
  typenums.Bind (Reco_Vector, 313);
  typenums.Bind (Reco_BSplineCurve, 38);
  typenums.Bind (Reco_BSplineCurveWithKnots, 39);
  typenums.Bind (Reco_BezierCurve, 43);
  typenums.Bind (Reco_Pcurve, 190);
  typenums.Bind (Reco_QuasiUniformCurve, 236);
  typenums.Bind (Reco_RationalBSplineCurve, 239);
  typenums.Bind (Reco_TrimmedCurve, 308);
  typenums.Bind (Reco_UniformCurve, 311);
  
  typenums.Bind (Reco_BSplineSurface, 40);
  typenums.Bind (Reco_BSplineSurfaceWithKnots, 41);
  typenums.Bind (Reco_QuasiUniformSurface, 237);
  typenums.Bind (Reco_RationalBSplineSurface, 240);
  typenums.Bind (Reco_UniformSurface, 312);
  typenums.Bind (Reco_BezierSurface, 44);
  typenums.Bind (Reco_ConicalSurface, 75);
  typenums.Bind (Reco_CylindricalSurface, 90);
  typenums.Bind (Reco_SphericalSurface, 269);
  typenums.Bind (Reco_ToroidalSurface, 305);
  typenums.Bind (Reco_SurfaceOfLinearExtrusion, 273);
  typenums.Bind (Reco_SurfaceOfRevolution, 274);
  
  typenums.Bind (Reco_Address, 1);
  typenums.Bind (Reco_AdvancedBrepShapeRepresentation, 2);
  typenums.Bind (Reco_AnnotationCurveOccurrence, 4);
  typenums.Bind (Reco_AnnotationFillArea, 5);
  typenums.Bind (Reco_AnnotationFillAreaOccurrence, 6);
  typenums.Bind (Reco_AnnotationOccurrence, 7);
  typenums.Bind (Reco_AnnotationSubfigureOccurrence, 8);
  typenums.Bind (Reco_AnnotationSymbol, 9);
  typenums.Bind (Reco_AnnotationSymbolOccurrence, 10);
  typenums.Bind (Reco_AnnotationText, 11);
  typenums.Bind (Reco_AnnotationTextOccurrence, 12);
  typenums.Bind (Reco_ApplicationContext, 13);
  typenums.Bind (Reco_ApplicationContextElement, 14);
  typenums.Bind (Reco_ApplicationProtocolDefinition, 15);
  typenums.Bind (Reco_Approval, 16);
  typenums.Bind (Reco_ApprovalAssignment, 17);
  typenums.Bind (Reco_ApprovalPersonOrganization, 18);
  typenums.Bind (Reco_ApprovalRelationship, 19);
  typenums.Bind (Reco_ApprovalRole, 20);
  typenums.Bind (Reco_ApprovalStatus, 21);
  typenums.Bind (Reco_AreaInSet, 22);
  typenums.Bind (Reco_AutoDesignActualDateAndTimeAssignment, 23);
  typenums.Bind (Reco_AutoDesignActualDateAssignment, 24);
  typenums.Bind (Reco_AutoDesignApprovalAssignment, 25);
  typenums.Bind (Reco_AutoDesignDateAndPersonAssignment, 26);
  typenums.Bind (Reco_AutoDesignGroupAssignment, 27);
  typenums.Bind (Reco_AutoDesignNominalDateAndTimeAssignment, 28);
  typenums.Bind (Reco_AutoDesignNominalDateAssignment, 29);
  typenums.Bind (Reco_AutoDesignOrganizationAssignment, 30);
  typenums.Bind (Reco_AutoDesignPersonAndOrganizationAssignment, 31);
  typenums.Bind (Reco_AutoDesignPresentedItem, 32);
  typenums.Bind (Reco_AutoDesignSecurityClassificationAssignment, 33);
  typenums.Bind (Reco_AutoDesignViewArea, 34);
  typenums.Bind (Reco_Axis1Placement, 35);
  typenums.Bind (Reco_Axis2Placement2d, 36);
  typenums.Bind (Reco_Axis2Placement3d, 37);
  typenums.Bind (Reco_BackgroundColour, 42);
  typenums.Bind (Reco_Block, 45);
  typenums.Bind (Reco_BooleanResult, 46);
  typenums.Bind (Reco_BoundaryCurve, 47);
  typenums.Bind (Reco_BoundedCurve, 48);
  typenums.Bind (Reco_BoundedSurface, 49);
  typenums.Bind (Reco_BoxDomain, 50);
  typenums.Bind (Reco_BoxedHalfSpace, 51);
  typenums.Bind (Reco_BrepWithVoids, 52);
  typenums.Bind (Reco_CalendarDate, 53);
  typenums.Bind (Reco_CameraImage, 54);
  typenums.Bind (Reco_CameraModel, 55);
  typenums.Bind (Reco_CameraModelD2, 56);
  typenums.Bind (Reco_CameraModelD3, 57);
  typenums.Bind (Reco_CameraUsage, 58);
  typenums.Bind (Reco_CartesianTransformationOperator, 60);
  typenums.Bind (Reco_CartesianTransformationOperator3d, 61);
  typenums.Bind (Reco_Circle, 62);
  typenums.Bind (Reco_ClosedShell, 63);
  typenums.Bind (Reco_Colour, 64);
  typenums.Bind (Reco_ColourRgb, 65);
  typenums.Bind (Reco_ColourSpecification, 66);
  typenums.Bind (Reco_CompositeCurve, 67);
  typenums.Bind (Reco_CompositeCurveOnSurface, 68);
  typenums.Bind (Reco_CompositeCurveSegment, 69);
  typenums.Bind (Reco_CompositeText, 70);
  typenums.Bind (Reco_CompositeTextWithAssociatedCurves, 71);
  typenums.Bind (Reco_CompositeTextWithBlankingBox, 72);
  typenums.Bind (Reco_CompositeTextWithExtent, 73);
  typenums.Bind (Reco_Conic, 74);
  typenums.Bind (Reco_ConnectedFaceSet, 76);
  typenums.Bind (Reco_ContextDependentInvisibility, 77);
  typenums.Bind (Reco_ContextDependentOverRidingStyledItem, 78);
  typenums.Bind (Reco_ConversionBasedUnit, 79);
  typenums.Bind (Reco_CoordinatedUniversalTimeOffset, 80);
  typenums.Bind (Reco_CsgRepresentation, 81);
  typenums.Bind (Reco_CsgShapeRepresentation, 82);
  typenums.Bind (Reco_CsgSolid, 83);
  typenums.Bind (Reco_Curve, 84);
  typenums.Bind (Reco_CurveBoundedSurface, 85);
  typenums.Bind (Reco_CurveReplica, 86);
  typenums.Bind (Reco_CurveStyle, 87);
  typenums.Bind (Reco_CurveStyleFont, 88);
  typenums.Bind (Reco_CurveStyleFontPattern, 89);
  typenums.Bind (Reco_Date, 91);
  typenums.Bind (Reco_DateAndTime, 92);
  typenums.Bind (Reco_DateAndTimeAssignment, 93);
  typenums.Bind (Reco_DateAssignment, 94);
  typenums.Bind (Reco_DateRole, 95);
  typenums.Bind (Reco_DateTimeRole, 96);
  typenums.Bind (Reco_DefinedSymbol, 97);
  typenums.Bind (Reco_DefinitionalRepresentation, 98);
  typenums.Bind (Reco_DegeneratePcurve, 99);
  typenums.Bind (Reco_DegenerateToroidalSurface, 100);
  typenums.Bind (Reco_DescriptiveRepresentationItem, 101);
  typenums.Bind (Reco_DimensionCurve, 102);
  typenums.Bind (Reco_DimensionCurveTerminator, 103);
  typenums.Bind (Reco_DimensionalExponents, 104);
  typenums.Bind (Reco_DraughtingAnnotationOccurrence, 106);
  typenums.Bind (Reco_DraughtingCallout, 107);
  typenums.Bind (Reco_DraughtingPreDefinedColour, 108);
  typenums.Bind (Reco_DraughtingPreDefinedCurveFont, 109);
  typenums.Bind (Reco_DraughtingSubfigureRepresentation, 110);
  typenums.Bind (Reco_DraughtingSymbolRepresentation, 111);
  typenums.Bind (Reco_DraughtingTextLiteralWithDelineation, 112);
  typenums.Bind (Reco_DrawingDefinition, 113);
  typenums.Bind (Reco_DrawingRevision, 114);
  typenums.Bind (Reco_Edge, 115);
  typenums.Bind (Reco_ElementarySurface, 118);
  typenums.Bind (Reco_Ellipse, 119);
  typenums.Bind (Reco_EvaluatedDegeneratePcurve, 120);
  typenums.Bind (Reco_ExternalSource, 121);
  typenums.Bind (Reco_ExternallyDefinedCurveFont, 122);
  typenums.Bind (Reco_ExternallyDefinedHatchStyle, 123);
  typenums.Bind (Reco_ExternallyDefinedItem, 124);
  typenums.Bind (Reco_ExternallyDefinedSymbol, 125);
  typenums.Bind (Reco_ExternallyDefinedTextFont, 126);
  typenums.Bind (Reco_ExternallyDefinedTileStyle, 127);
  typenums.Bind (Reco_ExtrudedAreaSolid, 128);
  typenums.Bind (Reco_Face, 129);
  typenums.Bind (Reco_FaceSurface, 133);
  typenums.Bind (Reco_FacetedBrep, 134);
  typenums.Bind (Reco_FacetedBrepShapeRepresentation, 135);
  typenums.Bind (Reco_FillAreaStyle, 136);
  typenums.Bind (Reco_FillAreaStyleColour, 137);
  typenums.Bind (Reco_FillAreaStyleHatching, 138);
  typenums.Bind (Reco_FillAreaStyleTileSymbolWithStyle, 139);
  typenums.Bind (Reco_FillAreaStyleTiles, 140);
  typenums.Bind (Reco_FunctionallyDefinedTransformation, 141);
  typenums.Bind (Reco_GeometricCurveSet, 142);
  typenums.Bind (Reco_GeometricRepresentationContext, 143);
  typenums.Bind (Reco_GeometricRepresentationItem, 144);
  typenums.Bind (Reco_GeometricSet, 145);
  typenums.Bind (Reco_GeometricallyBoundedSurfaceShapeRepresentation, 146);
  typenums.Bind (Reco_GeometricallyBoundedWireframeShapeRepresentation, 147);
  typenums.Bind (Reco_GlobalUncertaintyAssignedContext, 148);
  typenums.Bind (Reco_GlobalUnitAssignedContext, 149);
  typenums.Bind (Reco_Group, 150);
  typenums.Bind (Reco_GroupAssignment, 151);
  typenums.Bind (Reco_GroupRelationship, 152);
  typenums.Bind (Reco_HalfSpaceSolid, 153);
  typenums.Bind (Reco_Hyperbola, 154);
  typenums.Bind (Reco_IntersectionCurve, 155);
  typenums.Bind (Reco_Invisibility, 156);
  typenums.Bind (Reco_LengthMeasureWithUnit, 157);
  typenums.Bind (Reco_LengthUnit, 158);
  typenums.Bind (Reco_Line, 159);
  typenums.Bind (Reco_LocalTime, 160);
  typenums.Bind (Reco_Loop, 161);
  typenums.Bind (Reco_ManifoldSolidBrep, 162);
  typenums.Bind (Reco_ManifoldSurfaceShapeRepresentation, 163);
  typenums.Bind (Reco_MappedItem, 164);
  typenums.Bind (Reco_MeasureWithUnit, 165);
  typenums.Bind (Reco_MechanicalDesignGeometricPresentationArea, 166);
  typenums.Bind (Reco_MechanicalDesignGeometricPresentationRepresentation, 167);
  typenums.Bind (Reco_MechanicalDesignPresentationArea, 168);
  typenums.Bind (Reco_NamedUnit, 169);
  typenums.Bind (Reco_OffsetCurve3d, 171);
  typenums.Bind (Reco_OffsetSurface, 172);
  typenums.Bind (Reco_OneDirectionRepeatFactor, 173);
  typenums.Bind (Reco_OpenShell, 174);
  typenums.Bind (Reco_OrdinalDate, 175);
  typenums.Bind (Reco_Organization, 176);
  typenums.Bind (Reco_OrganizationAssignment, 177);
  typenums.Bind (Reco_OrganizationRole, 178);
  typenums.Bind (Reco_OrganizationalAddress, 179);
  typenums.Bind (Reco_OrientedClosedShell, 180);
  typenums.Bind (Reco_OrientedFace, 182);
  typenums.Bind (Reco_OrientedOpenShell, 183);
  typenums.Bind (Reco_OrientedPath, 184);
  typenums.Bind (Reco_OuterBoundaryCurve, 185);
  typenums.Bind (Reco_OverRidingStyledItem, 186);
  typenums.Bind (Reco_Parabola, 187);
  typenums.Bind (Reco_ParametricRepresentationContext, 188);
  typenums.Bind (Reco_Path, 189);
  typenums.Bind (Reco_Person, 191);
  typenums.Bind (Reco_PersonAndOrganization, 192);
  typenums.Bind (Reco_PersonAndOrganizationAssignment, 193);
  typenums.Bind (Reco_PersonAndOrganizationRole, 194);
  typenums.Bind (Reco_PersonalAddress, 195);
  typenums.Bind (Reco_Placement, 196);
  typenums.Bind (Reco_PlanarBox, 197);
  typenums.Bind (Reco_PlanarExtent, 198);
  typenums.Bind (Reco_Plane, 199);
  typenums.Bind (Reco_PlaneAngleMeasureWithUnit, 200);
  typenums.Bind (Reco_PlaneAngleUnit, 201);
  typenums.Bind (Reco_Point, 202);
  typenums.Bind (Reco_PointOnCurve, 203);
  typenums.Bind (Reco_PointOnSurface, 204);
  typenums.Bind (Reco_PointReplica, 205);
  typenums.Bind (Reco_PointStyle, 206);
  typenums.Bind (Reco_PolyLoop, 207);
  typenums.Bind (Reco_Polyline, 208);
  typenums.Bind (Reco_PreDefinedColour, 209);
  typenums.Bind (Reco_PreDefinedCurveFont, 210);
  typenums.Bind (Reco_PreDefinedItem, 211);
  typenums.Bind (Reco_PreDefinedSymbol, 212);
  typenums.Bind (Reco_PreDefinedTextFont, 213);
  typenums.Bind (Reco_PresentationArea, 214);
  typenums.Bind (Reco_PresentationLayerAssignment, 215);
  typenums.Bind (Reco_PresentationRepresentation, 216);
  typenums.Bind (Reco_PresentationSet, 217);
  typenums.Bind (Reco_PresentationSize, 218);
  typenums.Bind (Reco_PresentationStyleAssignment, 219);
  typenums.Bind (Reco_PresentationStyleByContext, 220);
  typenums.Bind (Reco_PresentationView, 221);
  typenums.Bind (Reco_PresentedItem, 222);
  typenums.Bind (Reco_Product, 223);
  typenums.Bind (Reco_ProductCategory, 224);
  typenums.Bind (Reco_ProductContext, 225);
  typenums.Bind (Reco_ProductDataRepresentationView, 226);
  typenums.Bind (Reco_ProductDefinition, 227);
  typenums.Bind (Reco_ProductDefinitionContext, 228);
  typenums.Bind (Reco_ProductDefinitionFormation, 229);
  typenums.Bind (Reco_ProductDefinitionFormationWithSpecifiedSource, 230);
  typenums.Bind (Reco_ProductDefinitionShape, 231);
  typenums.Bind (Reco_ProductRelatedProductCategory, 232);
  typenums.Bind (Reco_ProductType, 233);
  typenums.Bind (Reco_PropertyDefinition, 234);
  typenums.Bind (Reco_PropertyDefinitionRepresentation, 235);
  typenums.Bind (Reco_RatioMeasureWithUnit, 238);
  typenums.Bind (Reco_RectangularCompositeSurface, 241);
  typenums.Bind (Reco_RectangularTrimmedSurface, 242);
  typenums.Bind (Reco_RepItemGroup, 243);
  typenums.Bind (Reco_ReparametrisedCompositeCurveSegment, 244);
  typenums.Bind (Reco_Representation, 245);
  typenums.Bind (Reco_RepresentationContext, 246);
  typenums.Bind (Reco_RepresentationItem, 247);
  typenums.Bind (Reco_RepresentationMap, 248);
  typenums.Bind (Reco_RepresentationRelationship, 249);
  typenums.Bind (Reco_RevolvedAreaSolid, 250);
  typenums.Bind (Reco_RightAngularWedge, 251);
  typenums.Bind (Reco_RightCircularCone, 252);
  typenums.Bind (Reco_RightCircularCylinder, 253);
  typenums.Bind (Reco_SeamCurve, 254);
  typenums.Bind (Reco_SecurityClassification, 255);
  typenums.Bind (Reco_SecurityClassificationAssignment, 256);
  typenums.Bind (Reco_SecurityClassificationLevel, 257);
  typenums.Bind (Reco_ShapeAspect, 258);
  typenums.Bind (Reco_ShapeAspectRelationship, 259);
  typenums.Bind (Reco_ShapeAspectTransition, 260);
  typenums.Bind (Reco_ShapeDefinitionRepresentation, 261);
  typenums.Bind (Reco_ShapeRepresentation, 262);
  typenums.Bind (Reco_ShellBasedSurfaceModel, 263);
  typenums.Bind (Reco_SiUnit, 264);
  typenums.Bind (Reco_SolidAngleMeasureWithUnit, 265);
  typenums.Bind (Reco_SolidModel, 266);
  typenums.Bind (Reco_SolidReplica, 267);
  typenums.Bind (Reco_Sphere, 268);
  typenums.Bind (Reco_StyledItem, 270);
  typenums.Bind (Reco_Surface, 271);
  typenums.Bind (Reco_SurfacePatch, 275);
  typenums.Bind (Reco_SurfaceReplica, 276);
  typenums.Bind (Reco_SurfaceSideStyle, 277);
  typenums.Bind (Reco_SurfaceStyleBoundary, 278);
  typenums.Bind (Reco_SurfaceStyleControlGrid, 279);
  typenums.Bind (Reco_SurfaceStyleFillArea, 280);
  typenums.Bind (Reco_SurfaceStyleParameterLine, 281);
  typenums.Bind (Reco_SurfaceStyleSegmentationCurve, 282);
  typenums.Bind (Reco_SurfaceStyleSilhouette, 283);
  typenums.Bind (Reco_SurfaceStyleUsage, 284);
  typenums.Bind (Reco_SweptAreaSolid, 285);
  typenums.Bind (Reco_SweptSurface, 286);
  typenums.Bind (Reco_SymbolColour, 287);
  typenums.Bind (Reco_SymbolRepresentation, 288);
  typenums.Bind (Reco_SymbolRepresentationMap, 289);
  typenums.Bind (Reco_SymbolStyle, 290);
  typenums.Bind (Reco_SymbolTarget, 291);
  typenums.Bind (Reco_Template, 292);
  typenums.Bind (Reco_TemplateInstance, 293);
  typenums.Bind (Reco_TerminatorSymbol, 294);
  typenums.Bind (Reco_TextLiteral, 295);
  typenums.Bind (Reco_TextLiteralWithAssociatedCurves, 296);
  typenums.Bind (Reco_TextLiteralWithBlankingBox, 297);
  typenums.Bind (Reco_TextLiteralWithDelineation, 298);
  typenums.Bind (Reco_TextLiteralWithExtent, 299);
  typenums.Bind (Reco_TextStyle, 300);
  typenums.Bind (Reco_TextStyleForDefinedFont, 301);
  typenums.Bind (Reco_TextStyleWithBoxCharacteristics, 302);
  typenums.Bind (Reco_TextStyleWithMirror, 303);
  typenums.Bind (Reco_TopologicalRepresentationItem, 304);
  typenums.Bind (Reco_Torus, 306);
  typenums.Bind (Reco_TransitionalShapeRepresentation, 307);
  typenums.Bind (Reco_TwoDirectionRepeatFactor, 309);
  typenums.Bind (Reco_UncertaintyMeasureWithUnit, 310);
  typenums.Bind (Reco_Vertex, 314);
  typenums.Bind (Reco_VertexLoop, 315);
  typenums.Bind (Reco_ViewVolume, 317);
  typenums.Bind (Reco_WeekOfYearAndDayDate, 318);
  // Added by FMA  for Rev4
  typenums.Bind (Reco_SolidAngleUnit, 336);
  typenums.Bind (Reco_MechanicalContext, 339);
  typenums.Bind (Reco_DesignContext, 340);    // by CKY
  // full Rev4 (simple types)
  typenums.Bind (Reco_TimeMeasureWithUnit, 341);
  typenums.Bind (Reco_RatioUnit, 342);
  typenums.Bind (Reco_TimeUnit, 343);
  typenums.Bind (Reco_ApprovalDateTime, 348);
  typenums.Bind (Reco_CameraImage2dWithScale, 349);
  typenums.Bind (Reco_CameraImage3dWithScale, 350);
  typenums.Bind (Reco_CartesianTransformationOperator2d, 351);
  typenums.Bind (Reco_DerivedUnit, 352);
  typenums.Bind (Reco_DerivedUnitElement, 353);
  typenums.Bind (Reco_ItemDefinedTransformation, 354);
  typenums.Bind (Reco_PresentedItemRepresentation, 355);
  typenums.Bind (Reco_PresentationLayerUsage, 356);

//  AP214 : CC1 -> CC2

  typenums.Bind (Reco_AutoDesignDocumentReference, 366);

  typenums.Bind (Reco_Document, 367);
  typenums.Bind (Reco_DigitalDocument, 368);
  typenums.Bind (Reco_DocumentRelationship, 369);
  typenums.Bind (Reco_DocumentType, 370);
  typenums.Bind (Reco_DocumentUsageConstraint, 371);
  typenums.Bind (Reco_Effectivity, 372);
  typenums.Bind (Reco_ProductDefinitionEffectivity, 373);
  typenums.Bind (Reco_ProductDefinitionRelationship, 374);

  typenums.Bind (Reco_ProductDefinitionWithAssociatedDocuments, 375);
  typenums.Bind (Reco_PhysicallyModeledProductDefinition, 376);

  typenums.Bind (Reco_ProductDefinitionUsage, 377);
  typenums.Bind (Reco_MakeFromUsageOption, 378);
  typenums.Bind (Reco_AssemblyComponentUsage, 379);
  typenums.Bind (Reco_NextAssemblyUsageOccurrence, 380);
  typenums.Bind (Reco_PromissoryUsageOccurrence, 381);
  typenums.Bind (Reco_QuantifiedAssemblyComponentUsage, 382);
  typenums.Bind (Reco_SpecifiedHigherUsageOccurrence, 383);
  typenums.Bind (Reco_AssemblyComponentUsageSubstitute, 384);
  typenums.Bind (Reco_SuppliedPartRelationship, 385);
  typenums.Bind (Reco_ExternallyDefinedRepresentation, 386);
  typenums.Bind (Reco_ShapeRepresentationRelationship, 387);
  typenums.Bind (Reco_RepresentationRelationshipWithTransformation, 388);
  typenums.Bind (Reco_MaterialDesignation, 390);

  typenums.Bind (Reco_ContextDependentShapeRepresentation, 391);
  //Added from CD to DIS (S4134)
  typenums.Bind (Reco_AppliedDateAndTimeAssignment, 392);
  typenums.Bind (Reco_AppliedDateAssignment, 393);
  typenums.Bind (Reco_AppliedApprovalAssignment, 394);
  typenums.Bind (Reco_AppliedGroupAssignment, 395);
  typenums.Bind (Reco_AppliedOrganizationAssignment, 396);
  typenums.Bind (Reco_AppliedPersonAndOrganizationAssignment, 397);
  typenums.Bind (Reco_AppliedPresentedItem, 398);
  typenums.Bind (Reco_AppliedSecurityClassificationAssignment, 399);
  typenums.Bind (Reco_AppliedDocumentReference, 400);
  typenums.Bind (Reco_DocumentFile, 401);
  typenums.Bind (Reco_CharacterizedObject, 402);
  typenums.Bind (Reco_ExtrudedFaceSolid, 403);
  typenums.Bind (Reco_RevolvedFaceSolid, 404);
  typenums.Bind (Reco_SweptFaceSolid, 405);
  
  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  typenums.Bind (Reco_MeasureRepresentationItem, 406);
  typenums.Bind (Reco_AreaUnit,407);
  typenums.Bind (Reco_VolumeUnit,408);

  // Added by ABV 10.11.99 for AP203
  typenums.Bind (Reco_Action,413);
  typenums.Bind (Reco_ActionAssignment,414);
  typenums.Bind (Reco_ActionMethod,415);
  typenums.Bind (Reco_ActionRequestAssignment,416);
  typenums.Bind (Reco_CcDesignApproval,417);
  typenums.Bind (Reco_CcDesignCertification,418);
  typenums.Bind (Reco_CcDesignContract,419);
  typenums.Bind (Reco_CcDesignDateAndTimeAssignment,420);
  typenums.Bind (Reco_CcDesignPersonAndOrganizationAssignment,421);
  typenums.Bind (Reco_CcDesignSecurityClassification,422);
  typenums.Bind (Reco_CcDesignSpecificationReference,423);
  typenums.Bind (Reco_Certification,424);
  typenums.Bind (Reco_CertificationAssignment,425);
  typenums.Bind (Reco_CertificationType,426);
  typenums.Bind (Reco_Change,427);
  typenums.Bind (Reco_ChangeRequest,428);
  typenums.Bind (Reco_ConfigurationDesign,429);
  typenums.Bind (Reco_ConfigurationEffectivity,430);
  typenums.Bind (Reco_Contract,431);
  typenums.Bind (Reco_ContractAssignment,432);
  typenums.Bind (Reco_ContractType,433);
  typenums.Bind (Reco_ProductConcept,434);
  typenums.Bind (Reco_ProductConceptContext,435);
  typenums.Bind (Reco_StartRequest,436);
  typenums.Bind (Reco_StartWork,437);
  typenums.Bind (Reco_VersionedActionRequest,438);
  typenums.Bind (Reco_ProductCategoryRelationship,439);
  typenums.Bind (Reco_ActionRequestSolution,440);
  typenums.Bind (Reco_DraughtingModel,441);

  typenums.Bind (Reco_AngularLocation,442);
  typenums.Bind (Reco_AngularSize,443);
  typenums.Bind (Reco_DimensionalCharacteristicRepresentation,444);
  typenums.Bind (Reco_DimensionalLocation,445);
  typenums.Bind (Reco_DimensionalLocationWithPath,446);
  typenums.Bind (Reco_DimensionalSize,447);
  typenums.Bind (Reco_DimensionalSizeWithPath,448);
  typenums.Bind (Reco_ShapeDimensionRepresentation,449);

  // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  typenums.Bind (Reco_DocumentRepresentationType,450);
  typenums.Bind (Reco_ObjectRole,451);
  typenums.Bind (Reco_RoleAssociation,452);
  typenums.Bind (Reco_IdentificationRole,453);
  typenums.Bind (Reco_IdentificationAssignment,454);
  typenums.Bind (Reco_ExternalIdentificationAssignment,455);
  typenums.Bind (Reco_EffectivityAssignment,456);
  typenums.Bind (Reco_NameAssignment,457);
  typenums.Bind (Reco_GeneralProperty,458);
  typenums.Bind (Reco_Class,459);
  typenums.Bind (Reco_ExternallyDefinedClass,460);
  typenums.Bind (Reco_ExternallyDefinedGeneralProperty,461);
  typenums.Bind (Reco_AppliedExternalIdentificationAssignment,462);

  // Added by CKY, 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
  typenums.Bind (Reco_CompositeShapeAspect,470);
  typenums.Bind (Reco_DerivedShapeAspect,471);
  typenums.Bind (Reco_Extension,472);
  typenums.Bind (Reco_DirectedDimensionalLocation,473);
  typenums.Bind (Reco_LimitsAndFits,474);
  typenums.Bind (Reco_ToleranceValue,475);
  typenums.Bind (Reco_MeasureQualification,476);
  typenums.Bind (Reco_PlusMinusTolerance,477);
  typenums.Bind (Reco_PrecisionQualifier,478);
  typenums.Bind (Reco_TypeQualifier,479);
  typenums.Bind (Reco_QualifiedRepresentationItem,480);

  typenums.Bind (Reco_CompoundRepresentationItem,482);
  typenums.Bind (Reco_ValueRange,483);
  typenums.Bind (Reco_ShapeAspectDerivingRelationship,484);

  // abv 27.12.01
  typenums.Bind (Reco_CompoundShapeRepresentation,485);
  typenums.Bind (Reco_ConnectedEdgeSet,486);
  typenums.Bind (Reco_ConnectedFaceShapeRepresentation,487);
  typenums.Bind (Reco_EdgeBasedWireframeModel,488);
  typenums.Bind (Reco_EdgeBasedWireframeShapeRepresentation,489);
  typenums.Bind (Reco_FaceBasedSurfaceModel,490);
  typenums.Bind (Reco_NonManifoldSurfaceShapeRepresentation,491);
	
  
  // gka 08.01.02
  typenums.Bind (Reco_OrientedSurface,492);
  typenums.Bind (Reco_Subface,493);
  typenums.Bind (Reco_Subedge,494);
  typenums.Bind (Reco_SeamEdge,495);
  typenums.Bind (Reco_ConnectedFaceSubSet,496);

  //AP209
  typenums.Bind (Reco_EulerAngles,500);
  typenums.Bind (Reco_MassUnit,501);
  typenums.Bind (Reco_ThermodynamicTemperatureUnit,502);
  typenums.Bind (Reco_AnalysisItemWithinRepresentation,503);
  typenums.Bind (Reco_Curve3dElementDescriptor,504);
  typenums.Bind (Reco_CurveElementEndReleasePacket,505);
  typenums.Bind (Reco_CurveElementSectionDefinition,506);
  typenums.Bind (Reco_CurveElementSectionDerivedDefinitions,507);
  typenums.Bind (Reco_ElementDescriptor,508);
  typenums.Bind (Reco_ElementMaterial,509);
  typenums.Bind (Reco_Surface3dElementDescriptor,510);
  typenums.Bind (Reco_SurfaceElementProperty,511);
  typenums.Bind (Reco_SurfaceSection,512);
  typenums.Bind (Reco_SurfaceSectionField,513);
  typenums.Bind (Reco_SurfaceSectionFieldConstant,514);
  typenums.Bind (Reco_SurfaceSectionFieldVarying,515);
  typenums.Bind (Reco_UniformSurfaceSection,516);
  typenums.Bind (Reco_Volume3dElementDescriptor,517);
  typenums.Bind (Reco_AlignedCurve3dElementCoordinateSystem,518);
  typenums.Bind (Reco_ArbitraryVolume3dElementCoordinateSystem,519);
  typenums.Bind (Reco_Curve3dElementProperty,520);
  typenums.Bind (Reco_Curve3dElementRepresentation,521);
  typenums.Bind (Reco_Node,522);
//  typenums.Bind (Reco_CurveElementEndCoordinateSystem,523);
  typenums.Bind (Reco_CurveElementEndOffset,524);
  typenums.Bind (Reco_CurveElementEndRelease,525);
  typenums.Bind (Reco_CurveElementInterval,526);
  typenums.Bind (Reco_CurveElementIntervalConstant,527);
  typenums.Bind (Reco_DummyNode,528);
  typenums.Bind (Reco_CurveElementLocation,529);
  typenums.Bind (Reco_ElementGeometricRelationship,530);
  typenums.Bind (Reco_ElementGroup,531);
  typenums.Bind (Reco_ElementRepresentation,532);
  typenums.Bind (Reco_FeaAreaDensity,533);
  typenums.Bind (Reco_FeaAxis2Placement3d,534);
  typenums.Bind (Reco_FeaGroup,535);
  typenums.Bind (Reco_FeaLinearElasticity,536);
  typenums.Bind (Reco_FeaMassDensity,537);
  typenums.Bind (Reco_FeaMaterialPropertyRepresentation,538);
  typenums.Bind (Reco_FeaMaterialPropertyRepresentationItem,539);
  typenums.Bind (Reco_FeaModel,540);
  typenums.Bind (Reco_FeaModel3d,541);
  typenums.Bind (Reco_FeaMoistureAbsorption,542);
  typenums.Bind (Reco_FeaParametricPoint,543);
  typenums.Bind (Reco_FeaRepresentationItem,544);
  typenums.Bind (Reco_FeaSecantCoefficientOfLinearThermalExpansion,545);
  typenums.Bind (Reco_FeaShellBendingStiffness,546);
  typenums.Bind (Reco_FeaShellMembraneBendingCouplingStiffness,547);
  typenums.Bind (Reco_FeaShellMembraneStiffness,548);
  typenums.Bind (Reco_FeaShellShearStiffness,549);
  typenums.Bind (Reco_GeometricNode,550);
  typenums.Bind (Reco_FeaTangentialCoefficientOfLinearThermalExpansion,551);
  typenums.Bind (Reco_NodeGroup,552);
  typenums.Bind (Reco_NodeRepresentation,553);
  typenums.Bind (Reco_NodeSet,554);
  typenums.Bind (Reco_NodeWithSolutionCoordinateSystem,555);
  typenums.Bind (Reco_NodeWithVector,556);
  typenums.Bind (Reco_ParametricCurve3dElementCoordinateDirection,557);
  typenums.Bind (Reco_ParametricCurve3dElementCoordinateSystem,558);
  typenums.Bind (Reco_ParametricSurface3dElementCoordinateSystem,559);
  typenums.Bind (Reco_Surface3dElementRepresentation,560);
//  typenums.Bind (Reco_SymmetricTensor22d,561);
//  typenums.Bind (Reco_SymmetricTensor42d,562);
//  typenums.Bind (Reco_SymmetricTensor43d,563);
  typenums.Bind (Reco_Volume3dElementRepresentation,564);
  typenums.Bind (Reco_DataEnvironment,565);
  typenums.Bind (Reco_MaterialPropertyRepresentation,566);
  typenums.Bind (Reco_PropertyDefinitionRelationship,567);
  typenums.Bind (Reco_PointRepresentation,568);
  typenums.Bind (Reco_MaterialProperty,569);
  typenums.Bind (Reco_FeaModelDefinition,570);
  typenums.Bind (Reco_FreedomAndCoefficient,571);
  typenums.Bind (Reco_FreedomsList,572);
  typenums.Bind (Reco_ProductDefinitionFormationRelationship,573);
//  typenums.Bind (Reco_FeaModelDefinition,574);
  typenums.Bind (Reco_NodeDefinition,575);
  typenums.Bind (Reco_StructuralResponseProperty,576);
  typenums.Bind (Reco_StructuralResponsePropertyDefinitionRepresentation,577);
  typenums.Bind (Reco_AlignedSurface3dElementCoordinateSystem,579);
  typenums.Bind (Reco_ConstantSurface3dElementCoordinateSystem,580);
  typenums.Bind (Reco_CurveElementIntervalLinearlyVarying,581);
  typenums.Bind (Reco_FeaCurveSectionGeometricRelationship,582);
  typenums.Bind (Reco_FeaSurfaceSectionGeometricRelationship,583);
  
  // ptv 28.01.2003
  typenums.Bind (Reco_DocumentProductAssociation,600);
  typenums.Bind (Reco_DocumentProductEquivalence,601);
  
  // Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
  typenums.Bind (Reco_CylindricityTolerance,609);
  typenums.Bind (Reco_ShapeRepresentationWithParameters,610);
  typenums.Bind (Reco_AngularityTolerance,611);
  typenums.Bind (Reco_ConcentricityTolerance,612);
  typenums.Bind (Reco_CircularRunoutTolerance,613);
  typenums.Bind (Reco_CoaxialityTolerance,614);
  typenums.Bind (Reco_FlatnessTolerance,615);
  typenums.Bind (Reco_LineProfileTolerance,616);
  typenums.Bind (Reco_ParallelismTolerance,617);
  typenums.Bind (Reco_PerpendicularityTolerance,618);
  typenums.Bind (Reco_PositionTolerance,619);
  typenums.Bind (Reco_RoundnessTolerance,620);
  typenums.Bind (Reco_StraightnessTolerance,621);
  typenums.Bind (Reco_SurfaceProfileTolerance,622);
  typenums.Bind (Reco_SymmetryTolerance,623);
  typenums.Bind (Reco_TotalRunoutTolerance,624);
  typenums.Bind (Reco_GeometricTolerance,625);
  typenums.Bind (Reco_GeometricToleranceRelationship,626);
  typenums.Bind (Reco_GeometricToleranceWithDatumReference,627);
  typenums.Bind (Reco_ModifiedGeometricTolerance,628);
  typenums.Bind (Reco_Datum,629);
  typenums.Bind (Reco_DatumFeature,630);
  typenums.Bind (Reco_DatumReference,631);
  typenums.Bind (Reco_CommonDatum,632);
  typenums.Bind (Reco_DatumTarget,633);
  typenums.Bind (Reco_PlacedDatumTargetFeature,634);

  typenums.Bind (Reco_MassMeasureWithUnit,651);

  //Added by ika for GD&T AP242
  typenums.Bind (Reco_Apex, 660);
  typenums.Bind (Reco_CentreOfSymmetry, 661);
  typenums.Bind (Reco_GeometricAlignment, 662);
  typenums.Bind (Reco_PerpendicularTo, 663);
  typenums.Bind (Reco_Tangent, 664);
  typenums.Bind (Reco_ParallelOffset, 665);
  typenums.Bind (Reco_GeometricItemSpecificUsage, 666);
  typenums.Bind (Reco_IdAttribute, 667);
  typenums.Bind (Reco_ItemIdentifiedRepresentationUsage, 668);
  typenums.Bind (Reco_AllAroundShapeAspect, 669);
  typenums.Bind (Reco_BetweenShapeAspect, 670);
  typenums.Bind (Reco_CompositeGroupShapeAspect, 671);
  typenums.Bind (Reco_ContinuosShapeAspect, 672);
  typenums.Bind (Reco_GeometricToleranceWithDefinedAreaUnit, 673);
  typenums.Bind (Reco_GeometricToleranceWithDefinedUnit, 674);
  typenums.Bind (Reco_GeometricToleranceWithMaximumTolerance, 675);
  typenums.Bind (Reco_GeometricToleranceWithModifiers, 676);
  typenums.Bind (Reco_UnequallyDisposedGeometricTolerance, 677);
  typenums.Bind (Reco_NonUniformZoneDefinition, 678);
  typenums.Bind (Reco_ProjectedZoneDefinition, 679);
  typenums.Bind (Reco_RunoutZoneDefinition, 680);
  typenums.Bind (Reco_RunoutZoneOrientation, 681);
  typenums.Bind (Reco_ToleranceZone, 682);
  typenums.Bind (Reco_ToleranceZoneDefinition, 683);
  typenums.Bind (Reco_ToleranceZoneForm, 684);
  typenums.Bind (Reco_ValueFormatTypeQualifier, 685);
  typenums.Bind (Reco_DatumReferenceCompartment, 686);
  typenums.Bind (Reco_DatumReferenceElement, 687);
  typenums.Bind (Reco_DatumReferenceModifierWithValue, 688);
  typenums.Bind (Reco_DatumSystem, 689);
  typenums.Bind (Reco_GeneralDatumReference, 690);
  typenums.Bind (Reco_IntegerRepresentationItem, 700);
  typenums.Bind (Reco_ValueRepresentationItem, 701);
  typenums.Bind (Reco_FeatureForDatumTargetRelationship, 702);
  typenums.Bind (Reco_DraughtingModelItemAssociation, 703);
  typenums.Bind (Reco_AnnotationPlane, 704);

  typenums.Bind (Reco_TessellatedAnnotationOccurrence,707);
  typenums.Bind (Reco_TessellatedGeometricSet, 709);

  typenums.Bind ( Reco_TessellatedCurveSet, 710);
  typenums.Bind ( Reco_CoordinatesList, 711);
  typenums.Bind ( Reco_ConstructiveGeometryRepresentation, 712);
  typenums.Bind ( Reco_ConstructiveGeometryRepresentationRelationship, 713);
  typenums.Bind ( Reco_CharacterizedRepresentation, 714);
  typenums.Bind ( Reco_CameraModelD3MultiClipping, 716);
  typenums.Bind ( Reco_CameraModelD3MultiClippingIntersection, 717);
  typenums.Bind ( Reco_CameraModelD3MultiClippingUnion, 718);

  
//    SHORT NAMES
//    NB : la liste est celle de AP203
//    Directement exploite pour les types simples
//    Pour les types complexes, l option prise est de convertir les noms courts
//    en noms longs et de refaire l essai

//203  typeshor.Bind ("ACTASS",ACTION_ASSIGNMENT);
//203  typeshor.Bind ("ACTDRC",ACTION_DIRECTIVE);
//203  typeshor.Bind ("ACTMTH",ACTION_METHOD);
//203  typeshor.Bind ("ACRQAS",ACTION_REQUEST_ASSIGNMENT);
//203  typeshor.Bind ("ACRQSL",ACTION_REQUEST_SOLUTION);
//203  typeshor.Bind ("ACRQST",ACTION_REQUEST_STATUS);
//203  typeshor.Bind ("ACTSTT",ACTION_STATUS);
  typeshor.Bind ("ADDRSS",1);
  typeshor.Bind ("ABSR",2);
  typeshor.Bind ("ADVFC",3);
  typeshor.Bind ("ANCROC",4);
  typeshor.Bind ("ANFLAR",5);
  typeshor.Bind ("AFAO",6);
  typeshor.Bind ("ANNOCC",7);
  typeshor.Bind ("ANSBOC",8);
  typeshor.Bind ("ANNSYM",9);
  typeshor.Bind ("ANSYOC",10);
  typeshor.Bind ("ANNTXT",11);
  typeshor.Bind ("ANTXOC",12);
//203  typeshor.Bind ("ALPRRL",ALTERNATE_PRODUCT_RELATIONSHIP);
  typeshor.Bind ("APPCNT",13);
  typeshor.Bind ("APCNEL",14);
  typeshor.Bind ("APPRDF",15);
  typeshor.Bind ("APPRVL",16);
  typeshor.Bind ("APPASS",17);
  typeshor.Bind ("APDTTM",348);
  typeshor.Bind ("APPROR",18);
  typeshor.Bind ("APPRLT",19);
  typeshor.Bind ("APPRL",20);
  typeshor.Bind ("APPSTT",21);
  typeshor.Bind ("ARINST",22);
//203  typeshor.Bind ("AMWU",AREA_MEASURE_WITH_UNIT);
//203  typeshor.Bind ("ARUNT",AREA_UNIT);
  typeshor.Bind ("ASCMUS",379);
  typeshor.Bind ("ACUS",384);
  typeshor.Bind ("AX1PLC",35);
  typeshor.Bind ("A2PL2D",36);
  typeshor.Bind ("A2PL3D",37);
  typeshor.Bind ("BZRCRV",43);
  typeshor.Bind ("BZRSRF",44);
  typeshor.Bind ("BLNRSL",46);
  typeshor.Bind ("BNDCR",47);
  typeshor.Bind ("BNDCRV",48);
  typeshor.Bind ("BNDSRF",49);
  typeshor.Bind ("BRWTVD",52);
  typeshor.Bind ("BSPCR",38);
  typeshor.Bind ("BSCWK",39);
  typeshor.Bind ("BSPSR",40);
  typeshor.Bind ("BSSWK",41);
  typeshor.Bind ("BXDMN",50);
  typeshor.Bind ("BXHLSP",51);
  typeshor.Bind ("CLNDT",53);
  typeshor.Bind ("CMRIMG",54);
  typeshor.Bind ("CMRMDL",55);
  typeshor.Bind ("CMMDD3",57);
  typeshor.Bind ("CMRUSG",58);
  typeshor.Bind ("CRTPNT",59);
  typeshor.Bind ("CRTROP",60);
//203  typeshor.Bind ("CTO2",CARTESIAN_TRANSFORMATION_OPERATOR_2D);
  typeshor.Bind ("CTO3",61);
//203  typeshor.Bind ("CCDSAP",CC_DESIGN_APPROVAL);
//203  typeshor.Bind ("CCDSCR",CC_DESIGN_CERTIFICATION);
//203  typeshor.Bind ("CCDSCN",CC_DESIGN_CONTRACT);
//203  typeshor.Bind ("CDDATA",CC_DESIGN_DATE_AND_TIME_ASSIGNMENT);
//203  typeshor.Bind ("CDPAOA",CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT);
//203  typeshor.Bind ("CDSC",CC_DESIGN_SECURITY_CLASSIFICATION);
//203  typeshor.Bind ("CDS",CC_DESIGN_SPECIFICATION_REFERENCE);
//203  typeshor.Bind ("CRTFCT",CERTIFICATION);
//203  typeshor.Bind ("CRTASS",CERTIFICATION_ASSIGNMENT);
//203  typeshor.Bind ("CRTTYP",CERTIFICATION_TYPE);

//203  typeshor.Bind ("CHNRQS",CHANGE_REQUEST);

  typeshor.Bind ("CLSSHL",63);
  typeshor.Bind ("CLRRGB",65);
  typeshor.Bind ("CLRSPC",66);
  typeshor.Bind ("CMPCRV",67);
  typeshor.Bind ("CCOS",68);
  typeshor.Bind ("CMCRSG",69);
//203  typeshor.Bind ("CNFDSG",CONFIGURATION_DESIGN);
//203  typeshor.Bind ("CNFEFF",CONFIGURATION_EFFECTIVITY);
//203  typeshor.Bind ("CNFITM",CONFIGURATION_ITEM);

  typeshor.Bind ("CMPTXT",70);
  typeshor.Bind ("CTWAC",71);
  typeshor.Bind ("CTWBB",72);
  typeshor.Bind ("CTWE",73);
  typeshor.Bind ("CNCSRF",75);
//203  typeshor.Bind ("CNEDST",CONNECTED_EDGE_SET);
  typeshor.Bind ("CNFCST",76);
  typeshor.Bind ("CNDPIN",77);
  typeshor.Bind ("CDORSI",78);
  typeshor.Bind ("CDSR",391);
//203  typeshor.Bind ("CNDPUN",CONTEXT_DEPENDENT_UNIT);
//203  typeshor.Bind ("CNTRCT",CONTRACT);
//203  typeshor.Bind ("CNTASS",CONTRACT_ASSIGNMENT);
//203  typeshor.Bind ("CNTTYP",CONTRACT_TYPE);
  typeshor.Bind ("CNBSUN",79);
  typeshor.Bind ("CUTO",80);
  typeshor.Bind ("CSSHRP",82);
  typeshor.Bind ("CSGSLD",83);

  typeshor.Bind ("CRBNSR",85);
  typeshor.Bind ("CRVRPL",86);
  typeshor.Bind ("CRVSTY",87);
  typeshor.Bind ("CRSTFN",88);
  typeshor.Bind ("CSFP",89);
  typeshor.Bind ("CYLSRF",90);

//203  typeshor.Bind ("DTDEFF",DATED_EFFECTIVITY);
  typeshor.Bind ("DTANTM",92);
  typeshor.Bind ("DATA",93);
  typeshor.Bind ("DTASS",94);
  typeshor.Bind ("DTRL",95);
  typeshor.Bind ("DTTMRL",96);
  typeshor.Bind ("DFNSYM",97);
  typeshor.Bind ("DFNRPR",98);
  typeshor.Bind ("DGNPCR",99);
  typeshor.Bind ("DGTRSR",100);
  typeshor.Bind ("DSRPIT",101);
  typeshor.Bind ("DMNCRV",102);
  typeshor.Bind ("DMCRTR",103);
  typeshor.Bind ("DSGCNT",340);
//203  typeshor.Bind ("DMFR",DESIGN_MAKE_FROM_RELATIONSHIP);
  typeshor.Bind ("DMNEXP",104);
//203  typeshor.Bind ("DRCACT",DIRECTED_ACTION);
  typeshor.Bind ("DRCTN",105);
  typeshor.Bind ("DRANOC",106);
  typeshor.Bind ("DRGCLL",107);
  typeshor.Bind ("DPDC",108);
  typeshor.Bind ("DPDCF",109);
  typeshor.Bind ("DRSBRP",110);
  typeshor.Bind ("DRSYRP",111);
  typeshor.Bind ("DTLWD",112);
  typeshor.Bind ("DRWDFN",113);
  typeshor.Bind ("DRWRVS",114);
  typeshor.Bind ("DCMNT",367);
//203  typeshor.Bind ("DCMRFR",DOCUMENT_REFERENCE);
  typeshor.Bind ("DCMRLT",369);
  typeshor.Bind ("DCMTYP",370);
  typeshor.Bind ("DCUSCN",371);
//203  typeshor.Bind ("DCWTCL",DOCUMENT_WITH_CLASS);

//203  typeshor.Bind ("EBWM",EDGE_BASED_WIREFRAME_MODEL);
//203  typeshor.Bind ("EBWSR",EDGE_BASED_WIREFRAME_SHAPE_REPRESENTATION);
  typeshor.Bind ("EDGCRV",116);
  typeshor.Bind ("EDGLP",117);
  typeshor.Bind ("EFFCTV",372);
  typeshor.Bind ("ELMSRF",118);
  typeshor.Bind ("ELLPS",119);
  typeshor.Bind ("EVDGPC",120);
  typeshor.Bind ("EXTSRC",121);
  typeshor.Bind ("EDCF",122);
  typeshor.Bind ("EDHS",123);
  typeshor.Bind ("EXDFIT",124);
  typeshor.Bind ("EXDFSY",125);
  typeshor.Bind ("EDTF",126);
  typeshor.Bind ("EDTS",127);
  typeshor.Bind ("EXARSL",128);
//203  typeshor.Bind ("EXCACT",EXECUTED_ACTION);

  typeshor.Bind ("FCBND",131);
  typeshor.Bind ("FCOTBN",132);
  typeshor.Bind ("FCSRF",133);
  typeshor.Bind ("FCTBR",134);
  typeshor.Bind ("FBSR",135);
  typeshor.Bind ("FLARST",136);
  typeshor.Bind ("FASC",137);
  typeshor.Bind ("FASH",138);
  typeshor.Bind ("FASTSW",139);
  typeshor.Bind ("FAST",140);
  typeshor.Bind ("FNDFTR",141);
  typeshor.Bind ("GBSSR",146);
  typeshor.Bind ("GBWSR",147);
  typeshor.Bind ("GMCRST",142);
  typeshor.Bind ("GMRPCN",143);
  typeshor.Bind ("GMRPIT",144);
  typeshor.Bind ("GMTST",145);
  typeshor.Bind ("GC",148);
  typeshor.Bind ("GUAC",149);
  typeshor.Bind ("GRPASS",151);
  typeshor.Bind ("GRPRLT",152);
  typeshor.Bind ("HLSPSL",153);
  typeshor.Bind ("HYPRBL",154);
  typeshor.Bind ("INTCRV",155);
  typeshor.Bind ("INVSBL",156);
  typeshor.Bind ("ITDFTR",354);
  typeshor.Bind ("LMWU",157);
  typeshor.Bind ("LNGUNT",158);

  typeshor.Bind ("LCLTM",160);

//203  typeshor.Bind ("LTEFF",LOT_EFFECTIVITY);
  typeshor.Bind ("MNSLBR",162);
  typeshor.Bind ("MSSR",163);
  typeshor.Bind ("MPPITM",164);
  typeshor.Bind ("MDGPA",166);
  typeshor.Bind ("MDGPR",167);
//203  typeshor.Bind ("MMWU",MASS_MEASURE_WITH_UNIT);
//203  typeshor.Bind ("MSSUNT",MASS_UNIT);
  typeshor.Bind ("MSWTUN",165);
  typeshor.Bind ("MCHCNT",339);
  typeshor.Bind ("NMDUNT",169);
  typeshor.Bind ("NAUO",380);
//203  typeshor.Bind ("OFCR2D",OFFSET_CURVE_2D);
  typeshor.Bind ("OFCR3D",171);
  typeshor.Bind ("OFFSRF",172);
  typeshor.Bind ("ODRF",173);
  typeshor.Bind ("OPNSHL",174);
  typeshor.Bind ("ORDDT",175);
  typeshor.Bind ("ORGNZT",176);
  typeshor.Bind ("ORGASS",177);
  typeshor.Bind ("ORGRL",178);
  typeshor.Bind ("ORGADD",179);
//203  typeshor.Bind ("ORGPRJ",ORGANIZATIONAL_PROJECT);
//203  typeshor.Bind ("ORGRLT",ORGANIZATION_RELATIONSHIP);
  typeshor.Bind ("ORCLSH",180);
  typeshor.Bind ("ORNEDG",181);
  typeshor.Bind ("ORNFC",182);
  typeshor.Bind ("OROPSH",183);
  typeshor.Bind ("ORNPTH",184);
  typeshor.Bind ("OTBNCR",185);
  typeshor.Bind ("ORSI",186);
  typeshor.Bind ("PRBL",187);
  typeshor.Bind ("PRRPCN",188);

  typeshor.Bind ("PRANOR",192);
  typeshor.Bind ("PAOA",193);
  typeshor.Bind ("PAOR",194);
  typeshor.Bind ("PRSADD",195);
  typeshor.Bind ("PLCMNT",196);
  typeshor.Bind ("PLNBX",197);
  typeshor.Bind ("PLNEXT",198);

  typeshor.Bind ("PAMWU",200);
  typeshor.Bind ("PLANUN",201);

  typeshor.Bind ("PNONCR",203);
  typeshor.Bind ("PNONSR",204);
  typeshor.Bind ("PNTRPL",205);
  typeshor.Bind ("PNTSTY",206);
  typeshor.Bind ("PLYLP",207);
  typeshor.Bind ("PLYLN",208);
  typeshor.Bind ("PRDFCL",209);
  typeshor.Bind ("PDCF",210);
  typeshor.Bind ("PRDFIT",211);
  typeshor.Bind ("PRDFSY",212);
  typeshor.Bind ("PDTF",213);
  typeshor.Bind ("PRSAR",214);
  typeshor.Bind ("PRLYAS",215);
  typeshor.Bind ("PRSRPR",216);
  typeshor.Bind ("PRSST",217);
  typeshor.Bind ("PRSSZ",218);
  typeshor.Bind ("PRSTAS",219);
  typeshor.Bind ("PSBC",220);
  typeshor.Bind ("PRSVW",221);
  typeshor.Bind ("PRSITM",222);
  typeshor.Bind ("PRDCT",223);
  typeshor.Bind ("PRDCTG",224);
//203  typeshor.Bind ("PRCTRL",PRODUCT_CATEGORY_RELATIONSHIP);
//203  typeshor.Bind ("PRDCNC",PRODUCT_CONCEPT);
//203  typeshor.Bind ("PRCNCN",PRODUCT_CONCEPT_CONTEXT);
  typeshor.Bind ("PRDCNT",225);
  typeshor.Bind ("PRDDFN",227);
  typeshor.Bind ("PRDFCN",228);
  typeshor.Bind ("PRDFEF",373);
  typeshor.Bind ("PRDFFR",229);
  typeshor.Bind ("PDFWSS",230);
  typeshor.Bind ("PRDFRL",374);
  typeshor.Bind ("PRDFSH",231);
  typeshor.Bind ("PRDFUS",377);
  typeshor.Bind ("PDWAD",375);
  typeshor.Bind ("PRPC",232);
  typeshor.Bind ("PRUSOC",381);
  typeshor.Bind ("PRPDFN",234);
  typeshor.Bind ("PRDFRP",235);
  typeshor.Bind ("QACU",382);
  typeshor.Bind ("QSUNCR",236);
  typeshor.Bind ("QSUNSR",237);
  typeshor.Bind ("RMWU",238);
  typeshor.Bind ("RBSC",239);
  typeshor.Bind ("RBSS",240);
  typeshor.Bind ("RCCMSR",241);
  typeshor.Bind ("RCTRSR",242);
  typeshor.Bind ("RPITGR",243);
  typeshor.Bind ("RCCS",244);
  typeshor.Bind ("RPRSNT",245);
  typeshor.Bind ("RPRCNT",246);
  typeshor.Bind ("RPRITM",247);
  typeshor.Bind ("RPRMP",248);
  typeshor.Bind ("RPRRLT",249);
  typeshor.Bind ("RVARSL",250);
  typeshor.Bind ("RGANWD",251);
  typeshor.Bind ("RGCRCN",252);
  typeshor.Bind ("RGCRCY",253);
  typeshor.Bind ("RRWT",388);
  typeshor.Bind ("SMCRV",254);
  typeshor.Bind ("SCRCLS",255);
  typeshor.Bind ("SCCLAS",256);
  typeshor.Bind ("SCCLLV",257);
//203  typeshor.Bind ("SRNMEF",SERIAL_NUMBERED_EFFECTIVITY);
  typeshor.Bind ("SHPASP",258);
  typeshor.Bind ("SHASRL",259);
  typeshor.Bind ("SHDFRP",261);
  typeshor.Bind ("SHPRPR",262);
  typeshor.Bind ("SHRPRL",387);
  typeshor.Bind ("SBSM",263);
//203  typeshor.Bind ("SBWM",SHELL_BASED_WIREFRAME_MODEL);
//203  typeshor.Bind ("SBWSR",SHELL_BASED_WIREFRAME_SHAPE_REPRESENTATION);
  typeshor.Bind ("SUNT",264);
  typeshor.Bind ("SAMWU",265);
  typeshor.Bind ("SLANUN",336);
  typeshor.Bind ("SLDMDL",266);
  typeshor.Bind ("SLDRPL",267);
  typeshor.Bind ("SHUO",383);
  typeshor.Bind ("SPHSRF",269);
  typeshor.Bind ("STYITM",270);
//203  typeshor.Bind ("STRRQS",START_REQUEST);
//203  typeshor.Bind ("STRWRK",START_WORK);
  typeshor.Bind ("SPPRRL",385);
  typeshor.Bind ("SRFC",271);
  typeshor.Bind ("SRFCRV",272);
  typeshor.Bind ("SL",273);
  typeshor.Bind ("SROFRV",274);
  typeshor.Bind ("SRFPTC",275);
  typeshor.Bind ("SRFRPL",276);
  typeshor.Bind ("SRSDST",277);
  typeshor.Bind ("SRSTBN",278);
  typeshor.Bind ("SSCG",279);
  typeshor.Bind ("SSFA",280);
  typeshor.Bind ("SSPL",281);
  typeshor.Bind ("SSSC",282);
  typeshor.Bind ("SRSTSL",283);
  typeshor.Bind ("SRSTUS",284);
  typeshor.Bind ("SWARSL",285);
  typeshor.Bind ("SWPSRF",286);
  typeshor.Bind ("SYMCLR",287);
  typeshor.Bind ("SYMRPR",288);
  typeshor.Bind ("SYRPMP",289);
  typeshor.Bind ("SYMSTY",290);
  typeshor.Bind ("SYMTRG",291);
  typeshor.Bind ("TRMSYM",294);
  typeshor.Bind ("TXTLTR",295);
  typeshor.Bind ("TLWAC",296);
  typeshor.Bind ("TLWBB",297);
  typeshor.Bind ("TLWD",298);
  typeshor.Bind ("TLWE",299);
  typeshor.Bind ("TXTSTY",300);
  typeshor.Bind ("TSFDF",301);
  typeshor.Bind ("TSWBC",302);
  typeshor.Bind ("TSWM",303);
  typeshor.Bind ("TPRPIT",304);
  typeshor.Bind ("TRDSRF",305);
  typeshor.Bind ("TRMCRV",308);
  typeshor.Bind ("TDRF",309);
  typeshor.Bind ("UMWU",310);
  typeshor.Bind ("UNFCRV",311);
  typeshor.Bind ("UNFSRF",312);

//203  typeshor.Bind ("VRACRQ",VERSIONED_ACTION_REQUEST);

  typeshor.Bind ("VRTLP",315);
  typeshor.Bind ("VRTPNT",316);
//203  typeshor.Bind ("VRTSHL",VERTEX_SHELL);
//203  typeshor.Bind ("VMWU",VOLUME_MEASURE_WITH_UNIT);
//203  typeshor.Bind ("VLMUNT",VOLUME_UNIT);
  typeshor.Bind ("VWVLM",317);
  typeshor.Bind ("WOYADD",318);
  typeshor.Bind ("TMWU",341);
  typeshor.Bind ("RTUNT",342);
  typeshor.Bind ("TMUNT",343);
  typeshor.Bind ("CI3WS",350);
  typeshor.Bind ("CTO2",351);
  typeshor.Bind ("DRVUNT",352);
  typeshor.Bind ("DRUNEL",353);
  typeshor.Bind ("PRITRP",355);
  typeshor.Bind ("MFUO",378);
//203  typeshor.Bind ("WRSHL",WIRE_SHELL);
  typeshor.Bind ("MTRDSG",390);
  typeshor.Bind ("ADATA",392);
  typeshor.Bind ("APDTAS",393);
  typeshor.Bind ("APGRAS",395);
  typeshor.Bind ("APORAS",396);
  typeshor.Bind ("APAOA",397);
  typeshor.Bind ("APPRIT",398);
  typeshor.Bind ("ASCA",399);
  typeshor.Bind ("APDCRF",400);
  typeshor.Bind ("DCMFL",401);
  typeshor.Bind ("CHROBJ",402);
  typeshor.Bind ("EXFCSL",403);
  typeshor.Bind ("RVFCSL",404);
  typeshor.Bind ("SWFCSL",405);

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  typeshor.Bind ("MSRPIT",406);
  typeshor.Bind ("ARUNT",407);
  typeshor.Bind ("VLMUNT",408);

  // Added by ABV 10.11.99 for AP203
  typeshor.Bind ("ACTION",413);
  typeshor.Bind ("ACTASS",414);
  typeshor.Bind ("ACTMTH",415);
  typeshor.Bind ("ACRQAS",416);
  typeshor.Bind ("CCDSAP",417);
  typeshor.Bind ("CCDSCR",418);
  typeshor.Bind ("CCDSCN",419);
  typeshor.Bind ("CDDATA",420);
  typeshor.Bind ("CDPAOA",421);
  typeshor.Bind ("CDSC",422);
  typeshor.Bind ("CDS",423);
  typeshor.Bind ("CRTFCT",424);
  typeshor.Bind ("CRTASS",425);
  typeshor.Bind ("CRTTYP",426);
  typeshor.Bind ("CHANGE",427);
  typeshor.Bind ("CHNRQS",428);
  typeshor.Bind ("CNFDSG",429);
  typeshor.Bind ("CNFEFF",430);
  typeshor.Bind ("CNTRCT",431);
  typeshor.Bind ("CNTASS",432);
  typeshor.Bind ("CNTTYP",433);
  typeshor.Bind ("PRDCNC",434);
  typeshor.Bind ("PRCNCN",435);
  typeshor.Bind ("STRRQS",436);
  typeshor.Bind ("STRWRK",437);
  typeshor.Bind ("VRACRQ",438);
  typeshor.Bind ("PRCTRL",439);
  typeshor.Bind ("ACRQSL",440);
  typeshor.Bind ("DRGMDL",441);
  typeshor.Bind ("ANGLCT",442);
  typeshor.Bind ("ANGSZ",443);
  typeshor.Bind ("DMCHRP",444);
  typeshor.Bind ("DMNLCT",445);
  typeshor.Bind ("DLWP",446);
  typeshor.Bind ("DMNSZ",447);
  typeshor.Bind ("DSWP",448);
  typeshor.Bind ("SHDMRP",449);
  typeshor.Bind ("DCRPTY",450);
  typeshor.Bind ("OBJRL",451);
  typeshor.Bind ("RLASS",452);
  typeshor.Bind ("IDNRL",453);
  typeshor.Bind ("IDNASS",454);
  typeshor.Bind ("EXIDAS",455);
  typeshor.Bind ("EFFASS",456);
  typeshor.Bind ("NMASS",457);
  typeshor.Bind ("GNRPRP",458);
  typeshor.Bind ("EDGP",461);
  typeshor.Bind ("AEIA",462);
  typeshor.Bind ("CMSHAS",470);
  typeshor.Bind ("DRSHAS",471);
  typeshor.Bind ("EXTNSN",472);
  typeshor.Bind ("DRDMLC",473);
  typeshor.Bind ("LMANFT",474);
  typeshor.Bind ("TLRVL",475);
  typeshor.Bind ("MSRQLF",476);
  typeshor.Bind ("PLMNTL",477);
  typeshor.Bind ("PRCQLF",478);
  typeshor.Bind ("TYPQLF",479);
  typeshor.Bind ("QLRPIT", 480);
  typeshor.Bind ("CMRPIT", 482);
  typeshor.Bind ("CMRPIT", 483);
  typeshor.Bind ("CMS0", 485);
  typeshor.Bind ("CNEDST", 486);
  typeshor.Bind ("EBWM", 488);
  typeshor.Bind ("EBWSR", 489);
  typeshor.Bind ("NMSSR", 491);
  typeshor.Bind ("ORNSRF", 492);
  typeshor.Bind ("SBFC", 493);
  typeshor.Bind ("SBDG", 494);
  typeshor.Bind ("CFSS", 496);
  typeshor.Bind ("MSSUNT", 501);
  typeshor.Bind ("THTMUN", 502);
  typeshor.Bind ("DTENV", 565);
  typeshor.Bind ("MTPRRP", 566);
  typeshor.Bind ("PRDFR", 567);
  typeshor.Bind ("MTRPRP", 569);
  typeshor.Bind ("PDFR", 573);
  typeshor.Bind ("DCP1", 600);
  typeshor.Bind ("DCPREQ", 601);
//  typeshor.Bind (AngularLocation);
//  typeshor.Bind (AngularSize);
//  typeshor.Bind (DimensionalCharacteristicRepresentation);
//  typeshor.Bind (DimensionalLocation);
//  typeshor.Bind (DimensionalLocationWithPath);
//  typeshor.Bind (DimensionalSize);
//  typeshor.Bind (DimensionalSizeWithPath);
//  typeshor.Bind (ShapeDimensionRepresentation);
  typeshor.Bind ("CYLTLR", 609);
  typeshor.Bind ("SRWP", 610);
  typeshor.Bind ("ANGTLR", 611);
  typeshor.Bind ("CNCTLR", 612);
  typeshor.Bind ("CRRNTL", 613);
  typeshor.Bind ("CXLTLR", 614);
  typeshor.Bind ("FLTTLR", 615);
  typeshor.Bind ("LNP0", 616);
  typeshor.Bind ("PRLTLR", 617);
  typeshor.Bind ("PRPTLR", 618);
  typeshor.Bind ("PSTTLR", 619);
  typeshor.Bind ("RNDTLR", 620);
  typeshor.Bind ("STRTLR", 621);
  typeshor.Bind ("SRPRTL", 622);
  typeshor.Bind ("SYMTLR", 623);
  typeshor.Bind ("TTRNTL", 624);
  typeshor.Bind ("GMTTLR", 625);
  typeshor.Bind ("GMTLRL", 626);
  typeshor.Bind ("GTWDR", 627);
  typeshor.Bind ("MDGMTL", 628);
  typeshor.Bind ("DTMFTR", 630);
  typeshor.Bind ("DTMRFR", 631);
  typeshor.Bind ("CMMDTM", 632);
  typeshor.Bind ("DTMTRG", 633);
  typeshor.Bind ("PDT0", 634);
  typeshor.Bind ("MMWU",651);
  typeshor.Bind ("CNOFSY",661);
  typeshor.Bind ("GMTALG",662);
  typeshor.Bind ("PRPT",663);
  typeshor.Bind ("TNGNT",664);
  typeshor.Bind ("PRLOFF",665);
  typeshor.Bind ("GISU",666);
  typeshor.Bind ("IDATT",667);
  typeshor.Bind ("IIRU",668);
  typeshor.Bind ("GTWDU",674);
  typeshor.Bind ("PRZNDF",679);
  typeshor.Bind ("RNZNDF",680);
  typeshor.Bind ("RNZNOR",681);
  typeshor.Bind ("TLRZN",682);
  typeshor.Bind ("TLZNDF",683);
  typeshor.Bind ("TLZNFR",684);
  typeshor.Bind ("INRPIT",700);
  typeshor.Bind ("VLRPIT",701);
  typeshor.Bind ("DMIA", 703);
  typeshor.Bind ("ANNPLN", 704);
  typeshor.Bind ("CNGMRP", 712);
  typeshor.Bind ("CGRR", 713);
    
}

// --- Case Recognition ---

Standard_Integer RWStepAP214_ReadWriteModule::CaseStep
(const TCollection_AsciiString& key) const
{
  // FMA - le 25-07-96 : Optimisation -> on teste en premier les types les plus
  //                     frequents dans le fichier cad geometry/topology
  Standard_Integer num;
  if (key.IsEqual(Reco_CartesianPoint)) return 59;  // car tres courant
  if (typenums.Find(key, num)) return num;
  if (typeshor.Find(key, num)) return num; // AJOUT DES TYPES COURTS
  return 0;
}


// --- External Mapping Case Recognition ---

//=======================================================================
//function : CaseStep
//purpose  : 
//=======================================================================

Standard_Integer RWStepAP214_ReadWriteModule::CaseStep
(const TColStd_SequenceOfAsciiString& theTypes) const
{
  
  // Optimized by FMA : le test sur le nombre de composant est repete meme
  //                    si la valeur testee est la meme.
  
  Standard_Integer NbComp = theTypes.Length();
  if (NbComp < 2) {
#ifdef OCCT_DEBUG
    cout << "Not a Plex" << endl;
#endif
  }
  else {
// SHORT TYPES
//  Pas tres elegant : on convertit d abord
//  Sinon, il faudrait sortir des routines
    Standard_Integer i,num = 0;
    for (i = 1; i <= NbComp; i ++) {
      if (typeshor.IsBound(theTypes(i)))  {  num = 1;  break;  }
    }
    if (num > 0) {
      TColStd_SequenceOfAsciiString longs;
      for (i = 1; i <= NbComp; i ++) {
        if (typeshor.Find (theTypes(i), num)) longs.Append(StepType(num));
        else longs.Append (theTypes(i));
      }
      return CaseStep (longs);
    }

    // sln 03.10.2001. BUC61003. Correction of alphabetic order of complex entity's items (ascending sorting)
    TColStd_SequenceOfAsciiString types;
    for(i =  1; i<= theTypes.Length(); i++)
      types.Append(theTypes(i));
    // do ascending sorting
    Standard_Boolean isOK = Standard_False;
    TCollection_AsciiString tmpStr;
    Standard_Integer  aLen = types.Length()-1;
    while(!isOK)
    {
      isOK = Standard_True;
      for(i = 1; i<= aLen; i++)
        if(types(i) > types(i+1))
        {
          tmpStr = types(i);
          types(i) = types(i+1);
          types(i+1) = tmpStr;
          isOK = Standard_False;
        }
    }
    
    if (NbComp == 7) {
      if ((types(1).IsEqual(StepType(48))) &&
          (types(2).IsEqual(StepType(38))) &&
          (types(3).IsEqual(StepType(84))) &&
          (types(4).IsEqual(StepType(144))) &&
          (types(5).IsEqual(StepType(239))) &&
          (types(6).IsEqual(StepType(247))) &&
          (types(7).IsEqual(StepType(311)))) {
        return 319;
      }
      else if ((types(1).IsEqual(StepType(48))) &&
               (types(2).IsEqual(StepType(38))) &&
               (types(3).IsEqual(StepType(39))) &&
               (types(4).IsEqual(StepType(84))) &&
               (types(5).IsEqual(StepType(144))) &&
               (types(6).IsEqual(StepType(239))) &&
               (types(7).IsEqual(StepType(247)))) {
        return 320;
      }
      else if ((types(1).IsEqual(StepType(48))) &&
              (types(2).IsEqual(StepType(38))) &&
              (types(3).IsEqual(StepType(84))) &&
              (types(4).IsEqual(StepType(144))) &&
              (types(5).IsEqual(StepType(236))) &&
              (types(6).IsEqual(StepType(239))) &&
              (types(7).IsEqual(StepType(247)))) {
       return 321;
      }
      else if ((types(1).IsEqual(StepType(43))) &&
               (types(2).IsEqual(StepType(48))) &&
               (types(3).IsEqual(StepType(38))) &&
               (types(4).IsEqual(StepType(84))) &&
               (types(5).IsEqual(StepType(144))) &&
               (types(6).IsEqual(StepType(239))) &&
               (types(7).IsEqual(StepType(247)))) {
        return 322;
      }
      else if ((types(1).IsEqual(StepType(49))) &&
               (types(2).IsEqual(StepType(40))) &&
               (types(3).IsEqual(StepType(41))) &&
               (types(4).IsEqual(StepType(144))) &&
               (types(5).IsEqual(StepType(240))) &&
               (types(6).IsEqual(StepType(247))) &&
               (types(7).IsEqual(StepType(271)))) {
        return 323;
      }
      else if ((types(1).IsEqual(StepType(49))) &&
               (types(2).IsEqual(StepType(40))) &&
               (types(3).IsEqual(StepType(144))) &&
               (types(4).IsEqual(StepType(240))) &&
               (types(5).IsEqual(StepType(247))) &&
               (types(6).IsEqual(StepType(271))) &&
               (types(7).IsEqual(StepType(312)))) {
        return 324;
      }
      else if ((types(1).IsEqual(StepType(49))) &&
               (types(2).IsEqual(StepType(40))) &&
               (types(3).IsEqual(StepType(144))) &&
               (types(4).IsEqual(StepType(237))) &&
               (types(5).IsEqual(StepType(240))) &&
               (types(6).IsEqual(StepType(247))) &&
               (types(7).IsEqual(StepType(271)))) {
        return 325;
      }
      else if ((types(1).IsEqual(StepType(44))) &&
               (types(2).IsEqual(StepType(49))) &&
               (types(3).IsEqual(StepType(40))) &&
               (types(4).IsEqual(StepType(144))) &&
               (types(5).IsEqual(StepType(240))) &&
               (types(6).IsEqual(StepType(247))) &&
               (types(7).IsEqual(StepType(271)))) {
        return 326;
      }
      else if ((types(1).IsEqual(StepType(40))) &&
               (types(2).IsEqual(StepType(41))) &&
               (types(3).IsEqual(StepType(49))) &&
               (types(4).IsEqual(StepType(144))) &&
               (types(5).IsEqual(StepType(240))) &&
               (types(6).IsEqual(StepType(247))) &&
               (types(7).IsEqual(StepType(271)))) {
        return 323;
      }
    }
    // Added by FMA
    else if (NbComp == 6) {
      if (types(1).IsEqual(StepType(52))  &&
          types(2).IsEqual(StepType(134)) &&
          types(3).IsEqual(StepType(144)) &&
          types(4).IsEqual(StepType(162)) &&
          types(5).IsEqual(StepType(247)) &&
          types(6).IsEqual(StepType(266))) {
        return 337;
      }
    }
    else if (NbComp == 5) {
      //:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
      if ((types(1).IsEqual(StepType(48))) &&
          (types(2).IsEqual(StepType(84))) &&
          (types(3).IsEqual(StepType(144))) &&
          (types(4).IsEqual(StepType(247))) &&
          (types(5).IsEqual(StepType(272)))) {
        return 358;
      }
      else if ((types(1).IsEqual(StepType(157))) &&
               (types(2).IsEqual(StepType(406))) &&
               (types(3).IsEqual(StepType(165))) &&
               (types(4).IsEqual(StepType(480))) &&
               (types(5).IsEqual(StepType(247)))) {
        return 692;
      }
      else if ((types(1).IsEqual(StepType(406))) &&
               (types(2).IsEqual(StepType(165))) &&
               (types(3).IsEqual(StepType(200))) &&
               (types(4).IsEqual(StepType(480))) &&
               (types(5).IsEqual(StepType(247)))) {
        return 693;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(627)) &&
                types(4).IsEqual(StepType(675)) &&
                types(5).IsEqual(StepType(676)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(627)) &&
                types(3).IsEqual(StepType(675)) &&
                types(4).IsEqual(StepType(676)) &&
                (types(5).IsEqual(StepType(616)) ||
                 types(5).IsEqual(StepType(617)) ||
                 types(5).IsEqual(StepType(618)) ||
                 types(5).IsEqual(StepType(619)) ||
                 types(5).IsEqual(StepType(620)) ||
                 types(5).IsEqual(StepType(621)) ||
                 types(5).IsEqual(StepType(622)) ||
                 types(5).IsEqual(StepType(623)) ||
                 types(5).IsEqual(StepType(624))))) {
        return 705;
      }
      if ((types(1).IsEqual(StepType(4))) &&
          (types(2).IsEqual(StepType(7))) &&
          (types(3).IsEqual(StepType(144))) &&
          (types(4).IsEqual(StepType(247))) &&
          (types(5).IsEqual(StepType(270)))) {
        return 719;
      }
    }
    else if (NbComp == 4) {
      if ((types(1).IsEqual(StepType(161))) &&
          (types(2).IsEqual(StepType(189))) &&
          (types(3).IsEqual(StepType(247))) &&
          (types(4).IsEqual(StepType(304)))) {
        return 332;
      }
      // Added by FMA
      else if ((types(1).IsEqual(StepType(143)) &&
                types(2).IsEqual(StepType(148)) &&
                types(3).IsEqual(StepType(149)) &&
                types(4).IsEqual(StepType(246)))) {
        return 333;
      }

      else if ((types(1).IsEqual(StepType(157))) &&
               (types(2).IsEqual(StepType(406))) &&
               (types(3).IsEqual(StepType(165))) &&
               (types(4).IsEqual(StepType(247)))) {
        return 635;
      }
      else if ((types(1).IsEqual(StepType(625))) &&
               (types(2).IsEqual(StepType(627))) &&
               (types(3).IsEqual(StepType(628))) &&
               (types(4).IsEqual(StepType(619)))) {
        return 636;
      }
      else if ((types(1).IsEqual(StepType(406))) &&
               (types(2).IsEqual(StepType(165))) &&
               (types(3).IsEqual(StepType(200))) &&
               (types(4).IsEqual(StepType(247)))) {
        return 691;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(627)) &&
                types(4).IsEqual(StepType(676)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(627)) &&
                types(3).IsEqual(StepType(676)) &&
                (types(4).IsEqual(StepType(616)) ||
                 types(4).IsEqual(StepType(617)) ||
                 types(4).IsEqual(StepType(618)) ||
                 types(4).IsEqual(StepType(619)) ||
                 types(4).IsEqual(StepType(620)) ||
                 types(4).IsEqual(StepType(621)) ||
                 types(4).IsEqual(StepType(622)) ||
                 types(4).IsEqual(StepType(623)) ||
                 types(4).IsEqual(StepType(624))))) {
        return 695;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(627)) &&
                types(4).IsEqual(StepType(677)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(627)) &&
                types(4).IsEqual(StepType(677)) &&
                (types(3).IsEqual(StepType(616)) ||
                 types(3).IsEqual(StepType(617)) ||
                 types(3).IsEqual(StepType(618)) ||
                 types(3).IsEqual(StepType(619)) ||
                 types(3).IsEqual(StepType(620)) ||
                 types(3).IsEqual(StepType(621)) ||
                 types(3).IsEqual(StepType(622)) ||
                 types(3).IsEqual(StepType(623)) ||
                 types(3).IsEqual(StepType(624))))) {
        return 697;
      }
      else if (types(1).IsEqual(StepType(671)) &&
               types(2).IsEqual(StepType(470)) &&
               types(3).IsEqual(StepType(630)) &&
               types(4).IsEqual(StepType(258))) {
        return 698;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(675)) &&
                types(4).IsEqual(StepType(676)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(675)) &&
                types(3).IsEqual(StepType(676)) &&
                (types(4).IsEqual(StepType(616)) ||
                 types(4).IsEqual(StepType(617)) ||
                 types(4).IsEqual(StepType(618)) ||
                 types(4).IsEqual(StepType(619)) ||
                 types(4).IsEqual(StepType(620)) ||
                 types(4).IsEqual(StepType(621)) ||
                 types(4).IsEqual(StepType(622)) ||
                 types(4).IsEqual(StepType(623)) ||
                 types(4).IsEqual(StepType(624))))) {
        return 706;
      }
      else if (types(1).IsEqual(StepType(402)) &&
               types(2).IsEqual(StepType(714)) &&
               types(3).IsEqual(StepType(441)) &&
               types(4).IsEqual(StepType(245))) {
        return 715;
      }
    }
    else if (NbComp == 3) {
      if ((types(1).IsEqual(StepType(158))) &&
          (types(2).IsEqual(StepType(169))) &&
          (types(3).IsEqual(StepType(264)))) {
        return 327;
      } //pdn t3d_opt
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(264))) &&
               (types(3).IsEqual(StepType(158)))) {
        return 327;
      }
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(201))) &&
               (types(3).IsEqual(StepType(264)))) {
        return 328;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(158))) &&
               (types(3).IsEqual(StepType(169)))) {
        return 329;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(201)))) {
        return 330;
      }
      else if ((types(1).IsEqual(StepType(143))) &&
               (types(2).IsEqual(StepType(149))) &&
               (types(3).IsEqual(StepType(246)))) {
        return 331;
      }
      // Added by FMA
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(336)))) {
        return 334;
      }
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(264))) &&
               (types(3).IsEqual(StepType(336)))) {
        return 335;
      }
      else if ((types(1).IsEqual(StepType(143))) &&
               (types(2).IsEqual(StepType(188))) &&
               (types(3).IsEqual(StepType(246)))) {
        return 338;
      }
      // full Rev4 (CKY 30-MARS-1997)
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(342))) &&
               (types(3).IsEqual(StepType(264)))) {
        return 344;
      }
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(264))) &&
               (types(3).IsEqual(StepType(343)))) {
        return 345;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(342)))) {
        return 346;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(343)))) {
        return 347;
      }
      else if ((types(1).IsEqual(StepType(157))) &&
               (types(2).IsEqual(StepType(165))) &&
               (types(3).IsEqual(StepType(310)))) {
        return 357; // LECTURE SEULEMENT (READ ONLY), origine CATIA. CKY 2-SEP-1997
      }
//      Additional non-alphabetic (CKY 5 MAI 1998)
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(158))) &&
               (types(3).IsEqual(StepType(264)))) {
        return 327;
      }
//      CC1 -> CC2 (CKY 1-JUL-1998)
      else if ((types(1).IsEqual(StepType(249))) &&
               (types(2).IsEqual(StepType(388))) &&
               (types(3).IsEqual(StepType(387)))) {
        return 389;
      }
      else if ((types(1).IsEqual(StepType(407))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(264)))) {
        return 409;
      }
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(264))) &&
               (types(3).IsEqual(StepType(408)))) {
        return 410;
      }
      else if ((types(1).IsEqual(StepType(407))) &&
               (types(2).IsEqual(StepType(79)))  &&
               (types(3).IsEqual(StepType(169)))) {
        return 411;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(169)))  &&
               (types(3).IsEqual(StepType(408)))) {
        return 412;
      }
      // abv 11.07.00: CAX-IF TRJ4: k1_geo-ac.stp
      else if ((types(1).IsEqual(StepType(98))) &&
               (types(2).IsEqual(StepType(245))) &&
               (types(3).IsEqual(StepType(262)))) {
        return 463;
      }
      // CKY 25 APR 2001; CAX-IF TR7J (dim.tol.)
      else if ((types(1).IsEqual(StepType(406))) &&
               (types(2).IsEqual(StepType(480))) &&
               (types(3).IsEqual(StepType(247)))) {
        return 481;
      }
      else if ((types(1).IsEqual(StepType(501))) &&
               (types(2).IsEqual(StepType(169))) &&
               (types(3).IsEqual(StepType(264)))) {
        return 574;
      }
      else if ((types(1).IsEqual(StepType(169))) &&
               (types(2).IsEqual(StepType(264))) &&
               (types(3).IsEqual(StepType(502)))) {
        return 578;
      }
      else if ((types(1).IsEqual(StepType(79))) &&
               (types(2).IsEqual(StepType(501))) &&
               (types(3).IsEqual(StepType(169)))) {
        return 650;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(627)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(627)) &&
                (types(3).IsEqual(StepType(616)) ||
                 types(3).IsEqual(StepType(617)) ||
                 types(3).IsEqual(StepType(618)) ||
                 types(3).IsEqual(StepType(619)) ||
                 types(3).IsEqual(StepType(620)) ||
                 types(3).IsEqual(StepType(621)) ||
                 types(3).IsEqual(StepType(622)) ||
                 types(3).IsEqual(StepType(623)) ||
                 types(3).IsEqual(StepType(624))))) {
        return 694;
      }
      else if (((types(1).IsEqual(StepType(609)) ||
                 types(1).IsEqual(StepType(611)) ||
                 types(1).IsEqual(StepType(612)) ||
                 types(1).IsEqual(StepType(613)) ||
                 types(1).IsEqual(StepType(614)) ||
                 types(1).IsEqual(StepType(615))) &&
                types(2).IsEqual(StepType(625)) &&
                types(3).IsEqual(StepType(676)))
                ||
               (types(1).IsEqual(StepType(625)) &&
                types(2).IsEqual(StepType(676)) &&
                (types(3).IsEqual(StepType(616)) ||
                 types(3).IsEqual(StepType(617)) ||
                 types(3).IsEqual(StepType(618)) ||
                 types(3).IsEqual(StepType(619)) ||
                 types(3).IsEqual(StepType(620)) ||
                 types(3).IsEqual(StepType(621)) ||
                 types(3).IsEqual(StepType(622)) ||
                 types(3).IsEqual(StepType(623)) ||
                 types(3).IsEqual(StepType(624))))) {
        return 696;
      }
      else if (types(1).IsEqual(StepType(470)) &&
               types(2).IsEqual(StepType(630)) &&
               types(3).IsEqual(StepType(258))) {
        return 699;
      }
    }
    return 0;
  }
  return 0;
}


//=======================================================================
//function : IsComplex
//purpose  : External Mapping Recognition
//=======================================================================

Standard_Boolean RWStepAP214_ReadWriteModule::IsComplex
  (const Standard_Integer CN) const
{
  switch(CN)
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
    case 358: //:n5
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


//=======================================================================
//function : StepType
//purpose  : 
//=======================================================================

const TCollection_AsciiString& RWStepAP214_ReadWriteModule::StepType
  (const Standard_Integer CN) const
{
  switch (CN) {
  case 1 : return Reco_Address;
  case 2 : return Reco_AdvancedBrepShapeRepresentation;
  case 3 : return Reco_AdvancedFace;
  case 4 : return Reco_AnnotationCurveOccurrence;
  case 5 : return Reco_AnnotationFillArea;
  case 6 : return Reco_AnnotationFillAreaOccurrence;
  case 7 : return Reco_AnnotationOccurrence;
  case 8 : return Reco_AnnotationSubfigureOccurrence;
  case 9 : return Reco_AnnotationSymbol;
  case 10 : return Reco_AnnotationSymbolOccurrence;
  case 11 : return Reco_AnnotationText;
  case 12 : return Reco_AnnotationTextOccurrence;
  case 13 : return Reco_ApplicationContext;
  case 14 : return Reco_ApplicationContextElement;
  case 15 : return Reco_ApplicationProtocolDefinition;
  case 16 : return Reco_Approval;
  case 17 : return Reco_ApprovalAssignment;
  case 18 : return Reco_ApprovalPersonOrganization;
  case 19 : return Reco_ApprovalRelationship;
  case 20 : return Reco_ApprovalRole;
  case 21 : return Reco_ApprovalStatus;
  case 22 : return Reco_AreaInSet;
  case 23 : return Reco_AutoDesignActualDateAndTimeAssignment;
  case 24 : return Reco_AutoDesignActualDateAssignment;
  case 25 : return Reco_AutoDesignApprovalAssignment;
  case 26 : return Reco_AutoDesignDateAndPersonAssignment;
  case 27 : return Reco_AutoDesignGroupAssignment;
  case 28 : return Reco_AutoDesignNominalDateAndTimeAssignment;
  case 29 : return Reco_AutoDesignNominalDateAssignment;
  case 30 : return Reco_AutoDesignOrganizationAssignment;
  case 31 : return Reco_AutoDesignPersonAndOrganizationAssignment;
  case 32 : return Reco_AutoDesignPresentedItem;
  case 33 : return Reco_AutoDesignSecurityClassificationAssignment;
  case 34 : return Reco_AutoDesignViewArea;
  case 35 : return Reco_Axis1Placement;
  case 36 : return Reco_Axis2Placement2d;
  case 37 : return Reco_Axis2Placement3d;
  case 38 : return Reco_BSplineCurve;
  case 39 : return Reco_BSplineCurveWithKnots;
  case 40 : return Reco_BSplineSurface;
  case 41 : return Reco_BSplineSurfaceWithKnots;
  case 42 : return Reco_BackgroundColour;
  case 43 : return Reco_BezierCurve;
  case 44 : return Reco_BezierSurface;
  case 45 : return Reco_Block;
  case 46 : return Reco_BooleanResult;
  case 47 : return Reco_BoundaryCurve;
  case 48 : return Reco_BoundedCurve;
  case 49 : return Reco_BoundedSurface;
  case 50 : return Reco_BoxDomain;
  case 51 : return Reco_BoxedHalfSpace;
  case 52 : return Reco_BrepWithVoids;
  case 53 : return Reco_CalendarDate;
  case 54 : return Reco_CameraImage;
  case 55 : return Reco_CameraModel;
  case 56 : return Reco_CameraModelD2;
  case 57 : return Reco_CameraModelD3;
  case 58 : return Reco_CameraUsage;
  case 59 : return Reco_CartesianPoint;
  case 60 : return Reco_CartesianTransformationOperator;
  case 61 : return Reco_CartesianTransformationOperator3d;
  case 62 : return Reco_Circle;
  case 63 : return Reco_ClosedShell;
  case 64 : return Reco_Colour;
  case 65 : return Reco_ColourRgb;
  case 66 : return Reco_ColourSpecification;
  case 67 : return Reco_CompositeCurve;
  case 68 : return Reco_CompositeCurveOnSurface;
  case 69 : return Reco_CompositeCurveSegment;
  case 70 : return Reco_CompositeText;
  case 71 : return Reco_CompositeTextWithAssociatedCurves;
  case 72 : return Reco_CompositeTextWithBlankingBox;
  case 73 : return Reco_CompositeTextWithExtent;
  case 74 : return Reco_Conic;
  case 75 : return Reco_ConicalSurface;
  case 76 : return Reco_ConnectedFaceSet;
  case 77 : return Reco_ContextDependentInvisibility;
  case 78 : return Reco_ContextDependentOverRidingStyledItem;
  case 79 : return Reco_ConversionBasedUnit;
  case 80 : return Reco_CoordinatedUniversalTimeOffset;
  case 81 : return Reco_CsgRepresentation;
  case 82 : return Reco_CsgShapeRepresentation;
  case 83 : return Reco_CsgSolid;
  case 84 : return Reco_Curve;
  case 85 : return Reco_CurveBoundedSurface;
  case 86 : return Reco_CurveReplica;
  case 87 : return Reco_CurveStyle;
  case 88 : return Reco_CurveStyleFont;
  case 89 : return Reco_CurveStyleFontPattern;
  case 90 : return Reco_CylindricalSurface;
  case 91 : return Reco_Date;
  case 92 : return Reco_DateAndTime;
  case 93 : return Reco_DateAndTimeAssignment;
  case 94 : return Reco_DateAssignment;
  case 95 : return Reco_DateRole;
  case 96 : return Reco_DateTimeRole;
  case 97 : return Reco_DefinedSymbol;
  case 98 : return Reco_DefinitionalRepresentation;
  case 99 : return Reco_DegeneratePcurve;
  case 100 : return Reco_DegenerateToroidalSurface;
  case 101 : return Reco_DescriptiveRepresentationItem;
  case 102 : return Reco_DimensionCurve;
  case 103 : return Reco_DimensionCurveTerminator;
  case 104 : return Reco_DimensionalExponents;
  case 105 : return Reco_Direction;
  case 106 : return Reco_DraughtingAnnotationOccurrence;
  case 107 : return Reco_DraughtingCallout;
  case 108 : return Reco_DraughtingPreDefinedColour;
  case 109 : return Reco_DraughtingPreDefinedCurveFont;
  case 110 : return Reco_DraughtingSubfigureRepresentation;
  case 111 : return Reco_DraughtingSymbolRepresentation;
  case 112 : return Reco_DraughtingTextLiteralWithDelineation;
  case 113 : return Reco_DrawingDefinition;
  case 114 : return Reco_DrawingRevision;
  case 115 : return Reco_Edge;
  case 116 : return Reco_EdgeCurve;
  case 117 : return Reco_EdgeLoop;
  case 118 : return Reco_ElementarySurface;
  case 119 : return Reco_Ellipse;
  case 120 : return Reco_EvaluatedDegeneratePcurve;
  case 121 : return Reco_ExternalSource;
  case 122 : return Reco_ExternallyDefinedCurveFont;
  case 123 : return Reco_ExternallyDefinedHatchStyle;
  case 124 : return Reco_ExternallyDefinedItem;
  case 125 : return Reco_ExternallyDefinedSymbol;
  case 126 : return Reco_ExternallyDefinedTextFont;
  case 127 : return Reco_ExternallyDefinedTileStyle;
  case 128 : return Reco_ExtrudedAreaSolid;
  case 129 : return Reco_Face;
  case 131 : return Reco_FaceBound;
  case 132 : return Reco_FaceOuterBound;
  case 133 : return Reco_FaceSurface;
  case 134 : return Reco_FacetedBrep;
  case 135 : return Reco_FacetedBrepShapeRepresentation;
  case 136 : return Reco_FillAreaStyle;
  case 137 : return Reco_FillAreaStyleColour;
  case 138 : return Reco_FillAreaStyleHatching;
  case 139 : return Reco_FillAreaStyleTileSymbolWithStyle;
  case 140 : return Reco_FillAreaStyleTiles;
  case 141 : return Reco_FunctionallyDefinedTransformation;
  case 142 : return Reco_GeometricCurveSet;
  case 143 : return Reco_GeometricRepresentationContext;
  case 144 : return Reco_GeometricRepresentationItem;
  case 145 : return Reco_GeometricSet;
  case 146 : return Reco_GeometricallyBoundedSurfaceShapeRepresentation;
  case 147 : return Reco_GeometricallyBoundedWireframeShapeRepresentation;
  case 148 : return Reco_GlobalUncertaintyAssignedContext;
  case 149 : return Reco_GlobalUnitAssignedContext;
  case 150 : return Reco_Group;
  case 151 : return Reco_GroupAssignment;
  case 152 : return Reco_GroupRelationship;
  case 153 : return Reco_HalfSpaceSolid;
  case 154 : return Reco_Hyperbola;
  case 155 : return Reco_IntersectionCurve;
  case 156 : return Reco_Invisibility;
  case 157 : return Reco_LengthMeasureWithUnit;
  case 158 : return Reco_LengthUnit;
  case 159 : return Reco_Line;
  case 160 : return Reco_LocalTime;
  case 161 : return Reco_Loop;
  case 162 : return Reco_ManifoldSolidBrep;
  case 163 : return Reco_ManifoldSurfaceShapeRepresentation;
  case 164 : return Reco_MappedItem;
  case 165 : return Reco_MeasureWithUnit;
  case 166 : return Reco_MechanicalDesignGeometricPresentationArea;
  case 167 : return Reco_MechanicalDesignGeometricPresentationRepresentation;
  case 168 : return Reco_MechanicalDesignPresentationArea;
  case 169 : return Reco_NamedUnit;
  case 171 : return Reco_OffsetCurve3d;
  case 172 : return Reco_OffsetSurface;
  case 173 : return Reco_OneDirectionRepeatFactor;
  case 174 : return Reco_OpenShell;
  case 175 : return Reco_OrdinalDate;
  case 176 : return Reco_Organization;
  case 177 : return Reco_OrganizationAssignment;
  case 178 : return Reco_OrganizationRole;
  case 179 : return Reco_OrganizationalAddress;
  case 180 : return Reco_OrientedClosedShell;
  case 181 : return Reco_OrientedEdge;
  case 182 : return Reco_OrientedFace;
  case 183 : return Reco_OrientedOpenShell;
  case 184 : return Reco_OrientedPath;
  case 185 : return Reco_OuterBoundaryCurve;
  case 186 : return Reco_OverRidingStyledItem;
  case 187 : return Reco_Parabola;
  case 188 : return Reco_ParametricRepresentationContext;
  case 189 : return Reco_Path;
  case 190 : return Reco_Pcurve;
  case 191 : return Reco_Person;
  case 192 : return Reco_PersonAndOrganization;
  case 193 : return Reco_PersonAndOrganizationAssignment;
  case 194 : return Reco_PersonAndOrganizationRole;
  case 195 : return Reco_PersonalAddress;
  case 196 : return Reco_Placement;
  case 197 : return Reco_PlanarBox;
  case 198 : return Reco_PlanarExtent;
  case 199 : return Reco_Plane;
  case 200 : return Reco_PlaneAngleMeasureWithUnit;
  case 201 : return Reco_PlaneAngleUnit;
  case 202 : return Reco_Point;
  case 203 : return Reco_PointOnCurve;
  case 204 : return Reco_PointOnSurface;
  case 205 : return Reco_PointReplica;
  case 206 : return Reco_PointStyle;
  case 207 : return Reco_PolyLoop;
  case 208 : return Reco_Polyline;
  case 209 : return Reco_PreDefinedColour;
  case 210 : return Reco_PreDefinedCurveFont;
  case 211 : return Reco_PreDefinedItem;
  case 212 : return Reco_PreDefinedSymbol;
  case 213 : return Reco_PreDefinedTextFont;
  case 214 : return Reco_PresentationArea;
  case 215 : return Reco_PresentationLayerAssignment;
  case 216 : return Reco_PresentationRepresentation;
  case 217 : return Reco_PresentationSet;
  case 218 : return Reco_PresentationSize;
  case 219 : return Reco_PresentationStyleAssignment;
  case 220 : return Reco_PresentationStyleByContext;
  case 221 : return Reco_PresentationView;
  case 222 : return Reco_PresentedItem;
  case 223 : return Reco_Product;
  case 224 : return Reco_ProductCategory;
  case 225 : return Reco_ProductContext;
  case 226 : return Reco_ProductDataRepresentationView;
  case 227 : return Reco_ProductDefinition;
  case 228 : return Reco_ProductDefinitionContext;
  case 229 : return Reco_ProductDefinitionFormation;
  case 230 : return Reco_ProductDefinitionFormationWithSpecifiedSource;
  case 231 : return Reco_ProductDefinitionShape;
  case 232 : return Reco_ProductRelatedProductCategory;
  case 233 : return Reco_ProductType;
  case 234 : return Reco_PropertyDefinition;
  case 235 : return Reco_PropertyDefinitionRepresentation;
  case 236 : return Reco_QuasiUniformCurve;
  case 237 : return Reco_QuasiUniformSurface;
  case 238 : return Reco_RatioMeasureWithUnit;
  case 239 : return Reco_RationalBSplineCurve;
  case 240 : return Reco_RationalBSplineSurface;
  case 241 : return Reco_RectangularCompositeSurface;
  case 242 : return Reco_RectangularTrimmedSurface;
  case 243 : return Reco_RepItemGroup;
  case 244 : return Reco_ReparametrisedCompositeCurveSegment;
  case 245 : return Reco_Representation;
  case 246 : return Reco_RepresentationContext;
  case 247 : return Reco_RepresentationItem;
  case 248 : return Reco_RepresentationMap;
  case 249 : return Reco_RepresentationRelationship;
  case 250 : return Reco_RevolvedAreaSolid;
  case 251 : return Reco_RightAngularWedge;
  case 252 : return Reco_RightCircularCone;
  case 253 : return Reco_RightCircularCylinder;
  case 254 : return Reco_SeamCurve;
  case 255 : return Reco_SecurityClassification;
  case 256 : return Reco_SecurityClassificationAssignment;
  case 257 : return Reco_SecurityClassificationLevel;
  case 258 : return Reco_ShapeAspect;
  case 259 : return Reco_ShapeAspectRelationship;
  case 260 : return Reco_ShapeAspectTransition;
  case 261 : return Reco_ShapeDefinitionRepresentation;
  case 262 : return Reco_ShapeRepresentation;
  case 263 : return Reco_ShellBasedSurfaceModel;
  case 264 : return Reco_SiUnit;
  case 265 : return Reco_SolidAngleMeasureWithUnit;
  case 266 : return Reco_SolidModel;
  case 267 : return Reco_SolidReplica;
  case 268 : return Reco_Sphere;
  case 269 : return Reco_SphericalSurface;
  case 270 : return Reco_StyledItem;
  case 271 : return Reco_Surface;
  case 272 : return Reco_SurfaceCurve;
  case 273 : return Reco_SurfaceOfLinearExtrusion;
  case 274 : return Reco_SurfaceOfRevolution;
  case 275 : return Reco_SurfacePatch;
  case 276 : return Reco_SurfaceReplica;
  case 277 : return Reco_SurfaceSideStyle;
  case 278 : return Reco_SurfaceStyleBoundary;
  case 279 : return Reco_SurfaceStyleControlGrid;
  case 280 : return Reco_SurfaceStyleFillArea;
  case 281 : return Reco_SurfaceStyleParameterLine;
  case 282 : return Reco_SurfaceStyleSegmentationCurve;
  case 283 : return Reco_SurfaceStyleSilhouette;
  case 284 : return Reco_SurfaceStyleUsage;
  case 285 : return Reco_SweptAreaSolid;
  case 286 : return Reco_SweptSurface;
  case 287 : return Reco_SymbolColour;
  case 288 : return Reco_SymbolRepresentation;
  case 289 : return Reco_SymbolRepresentationMap;
  case 290 : return Reco_SymbolStyle;
  case 291 : return Reco_SymbolTarget;
  case 292 : return Reco_Template;
  case 293 : return Reco_TemplateInstance;
  case 294 : return Reco_TerminatorSymbol;
  case 295 : return Reco_TextLiteral;
  case 296 : return Reco_TextLiteralWithAssociatedCurves;
  case 297 : return Reco_TextLiteralWithBlankingBox;
  case 298 : return Reco_TextLiteralWithDelineation;
  case 299 : return Reco_TextLiteralWithExtent;
  case 300 : return Reco_TextStyle;
  case 301 : return Reco_TextStyleForDefinedFont;
  case 302 : return Reco_TextStyleWithBoxCharacteristics;
  case 303 : return Reco_TextStyleWithMirror;
  case 304 : return Reco_TopologicalRepresentationItem;
  case 305 : return Reco_ToroidalSurface;
  case 306 : return Reco_Torus;
  case 307 : return Reco_TransitionalShapeRepresentation;
  case 308 : return Reco_TrimmedCurve;
  case 309 : return Reco_TwoDirectionRepeatFactor;
  case 310 : return Reco_UncertaintyMeasureWithUnit;
  case 311 : return Reco_UniformCurve;
  case 312 : return Reco_UniformSurface;
  case 313 : return Reco_Vector;
  case 314 : return Reco_Vertex;
  case 315 : return Reco_VertexLoop;
  case 316 : return Reco_VertexPoint;
  case 317 : return Reco_ViewVolume;
  case 318 : return Reco_WeekOfYearAndDayDate;
    // Added by FMA
  case 336 : return Reco_SolidAngleUnit;
  case 339 : return Reco_MechanicalContext;
  case 340 : return Reco_DesignContext;
    // Added for full Rev4
  case 341 : return Reco_TimeMeasureWithUnit;
  case 342 : return Reco_RatioUnit;
  case 343 : return Reco_TimeUnit;
         case 348 : return Reco_ApprovalDateTime;
         case 349 : return Reco_CameraImage2dWithScale;
  case 350 : return Reco_CameraImage3dWithScale;
  case 351 : return Reco_CartesianTransformationOperator2d;
  case 352 : return Reco_DerivedUnit;
  case 353 : return Reco_DerivedUnitElement;
  case 354 : return Reco_ItemDefinedTransformation;
  case 355 : return Reco_PresentedItemRepresentation;
  case 356 : return Reco_PresentationLayerUsage;


//  AP214 : CC1 -> CC2

  case 366 : return Reco_AutoDesignDocumentReference;
  case 367: return Reco_Document;
  case 368: return Reco_DigitalDocument;
  case 369: return Reco_DocumentRelationship;
  case 370: return Reco_DocumentType;
  case 371: return Reco_DocumentUsageConstraint;
  case 372: return Reco_Effectivity;
  case 373: return Reco_ProductDefinitionEffectivity;
  case 374: return Reco_ProductDefinitionRelationship;

  case 375: return Reco_ProductDefinitionWithAssociatedDocuments;
  case 376: return Reco_PhysicallyModeledProductDefinition;

  case 377: return Reco_ProductDefinitionUsage;
  case 378: return Reco_MakeFromUsageOption;
  case 379: return Reco_AssemblyComponentUsage;
  case 380: return Reco_NextAssemblyUsageOccurrence;
  case 381: return Reco_PromissoryUsageOccurrence;
  case 382: return Reco_QuantifiedAssemblyComponentUsage;
  case 383: return Reco_SpecifiedHigherUsageOccurrence;
  case 384: return Reco_AssemblyComponentUsageSubstitute;
  case 385: return Reco_SuppliedPartRelationship;
  case 386: return Reco_ExternallyDefinedRepresentation;
  case 387: return Reco_ShapeRepresentationRelationship;
  case 388: return Reco_RepresentationRelationshipWithTransformation;

  case 390: return Reco_MaterialDesignation;
  case 391: return Reco_ContextDependentShapeRepresentation;
  //:S4134: Added from CD to DIS
  case 392: return Reco_AppliedDateAndTimeAssignment; 
  case 393: return Reco_AppliedDateAssignment;  
  case 394: return Reco_AppliedApprovalAssignment;  
  case 395: return Reco_AppliedGroupAssignment;  
  case 396: return Reco_AppliedOrganizationAssignment;  
  case 397: return Reco_AppliedPersonAndOrganizationAssignment;  
  case 398: return Reco_AppliedPresentedItem;  
  case 399: return Reco_AppliedSecurityClassificationAssignment;  
  case 400: return Reco_AppliedDocumentReference;
  case 401: return Reco_DocumentFile;  
  case 402: return Reco_CharacterizedObject;  
  case 403: return Reco_ExtrudedFaceSolid;   
  case 404: return Reco_RevolvedFaceSolid;  
  case 405: return Reco_SweptFaceSolid;  

  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  case 406: return Reco_MeasureRepresentationItem;  
  case 407: return Reco_AreaUnit;
  case 408: return Reco_VolumeUnit;
    
  // Added by ABV 10.11.99 for AP203
  case 413: return Reco_Action;
  case 414: return Reco_ActionAssignment;
  case 415: return Reco_ActionMethod;
  case 416: return Reco_ActionRequestAssignment;
  case 417: return Reco_CcDesignApproval;
  case 418: return Reco_CcDesignCertification;
  case 419: return Reco_CcDesignContract;
  case 420: return Reco_CcDesignDateAndTimeAssignment;
  case 421: return Reco_CcDesignPersonAndOrganizationAssignment;
  case 422: return Reco_CcDesignSecurityClassification;
  case 423: return Reco_CcDesignSpecificationReference;
  case 424: return Reco_Certification;
  case 425: return Reco_CertificationAssignment;
  case 426: return Reco_CertificationType;
  case 427: return Reco_Change;
  case 428: return Reco_ChangeRequest;
  case 429: return Reco_ConfigurationDesign;
  case 430: return Reco_ConfigurationEffectivity;
  case 431: return Reco_Contract;
  case 432: return Reco_ContractAssignment;
  case 433: return Reco_ContractType;
  case 434: return Reco_ProductConcept;
  case 435: return Reco_ProductConceptContext;
  case 436: return Reco_StartRequest;
  case 437: return Reco_StartWork;
  case 438: return Reco_VersionedActionRequest;
  case 439: return Reco_ProductCategoryRelationship;
  case 440: return Reco_ActionRequestSolution;
  case 441: return Reco_DraughtingModel;

  // Added by ABV 18.04.00 for CAX-IF TRJ4
  case 442: return Reco_AngularLocation;
  case 443: return Reco_AngularSize;
  case 444: return Reco_DimensionalCharacteristicRepresentation;
  case 445: return Reco_DimensionalLocation;
  case 446: return Reco_DimensionalLocationWithPath;
  case 447: return Reco_DimensionalSize;
  case 448: return Reco_DimensionalSizeWithPath;
  case 449: return Reco_ShapeDimensionRepresentation;

  // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  case 450: return Reco_DocumentRepresentationType;
  case 451: return Reco_ObjectRole;
  case 452: return Reco_RoleAssociation;
  case 453: return Reco_IdentificationRole;
  case 454: return Reco_IdentificationAssignment;
  case 455: return Reco_ExternalIdentificationAssignment;
  case 456: return Reco_EffectivityAssignment;
  case 457: return Reco_NameAssignment;
  case 458: return Reco_GeneralProperty;
  case 459: return Reco_Class;
  case 460: return Reco_ExternallyDefinedClass;
  case 461: return Reco_ExternallyDefinedGeneralProperty;
  case 462: return Reco_AppliedExternalIdentificationAssignment;

  // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dim.tol.)
  case 470: return Reco_CompositeShapeAspect;
  case 471: return Reco_DerivedShapeAspect;
  case 472: return Reco_Extension;
  case 473: return Reco_DirectedDimensionalLocation;
  case 474: return Reco_LimitsAndFits;
  case 475: return Reco_ToleranceValue;
  case 476: return Reco_MeasureQualification;
  case 477: return Reco_PlusMinusTolerance;
  case 478: return Reco_PrecisionQualifier;
  case 479: return Reco_TypeQualifier;
  case 480: return Reco_QualifiedRepresentationItem;

  case 482: return Reco_CompoundRepresentationItem;
  case 483: return Reco_ValueRange;
  case 484: return Reco_ShapeAspectDerivingRelationship;

  case 485: return Reco_CompoundShapeRepresentation;
  case 486: return Reco_ConnectedEdgeSet;
  case 487: return Reco_ConnectedFaceShapeRepresentation;
  case 488: return Reco_EdgeBasedWireframeModel;
  case 489: return Reco_EdgeBasedWireframeShapeRepresentation;
  case 490: return Reco_FaceBasedSurfaceModel;
  case 491: return Reco_NonManifoldSurfaceShapeRepresentation;
  
    
  //gka 08.01.02
  case 492: return Reco_OrientedSurface;
  case 493: return Reco_Subface;
  case 494: return Reco_Subedge;
  case 495: return Reco_SeamEdge;
  case 496: return Reco_ConnectedFaceSubSet;
  
  //AP209
  case 500: return Reco_EulerAngles;
  case 501: return Reco_MassUnit;
  case 502: return Reco_ThermodynamicTemperatureUnit;
  case 503: return Reco_AnalysisItemWithinRepresentation;
  case 504: return Reco_Curve3dElementDescriptor;
  case 505: return Reco_CurveElementEndReleasePacket;
  case 506: return Reco_CurveElementSectionDefinition;
  case 507: return Reco_CurveElementSectionDerivedDefinitions;
  case 508: return Reco_ElementDescriptor;
  case 509: return Reco_ElementMaterial;
  case 510: return Reco_Surface3dElementDescriptor;
  case 511: return Reco_SurfaceElementProperty;
  case 512: return Reco_SurfaceSection;
  case 513: return Reco_SurfaceSectionField;
  case 514: return Reco_SurfaceSectionFieldConstant;
  case 515: return Reco_SurfaceSectionFieldVarying;
  case 516: return Reco_UniformSurfaceSection;
  case 517: return Reco_Volume3dElementDescriptor;
  case 518: return Reco_AlignedCurve3dElementCoordinateSystem;
  case 519: return Reco_ArbitraryVolume3dElementCoordinateSystem;
  case 520: return Reco_Curve3dElementProperty;
  case 521: return Reco_Curve3dElementRepresentation;
  case 522: return Reco_Node;
//  case 523: return Reco_CurveElementEndCoordinateSystem;
  case 524: return Reco_CurveElementEndOffset;
  case 525: return Reco_CurveElementEndRelease;
  case 526: return Reco_CurveElementInterval;
  case 527: return Reco_CurveElementIntervalConstant;
  case 528: return Reco_DummyNode;
  case 529: return Reco_CurveElementLocation;
  case 530: return Reco_ElementGeometricRelationship;
  case 531: return Reco_ElementGroup;
  case 532: return Reco_ElementRepresentation;
  case 533: return Reco_FeaAreaDensity;
  case 534: return Reco_FeaAxis2Placement3d;
  case 535: return Reco_FeaGroup;
  case 536: return Reco_FeaLinearElasticity;
  case 537: return Reco_FeaMassDensity;
  case 538: return Reco_FeaMaterialPropertyRepresentation;
  case 539: return Reco_FeaMaterialPropertyRepresentationItem;
  case 540: return Reco_FeaModel;
  case 541: return Reco_FeaModel3d;
  case 542: return Reco_FeaMoistureAbsorption;
  case 543: return Reco_FeaParametricPoint;
  case 544: return Reco_FeaRepresentationItem;
  case 545: return Reco_FeaSecantCoefficientOfLinearThermalExpansion;
  case 546: return Reco_FeaShellBendingStiffness;
  case 547: return Reco_FeaShellMembraneBendingCouplingStiffness;
  case 548: return Reco_FeaShellMembraneStiffness;
  case 549: return Reco_FeaShellShearStiffness;
  case 550: return Reco_GeometricNode;
  case 551: return Reco_FeaTangentialCoefficientOfLinearThermalExpansion;
  case 552: return Reco_NodeGroup;
  case 553: return Reco_NodeRepresentation;
  case 554: return Reco_NodeSet;
  case 555: return Reco_NodeWithSolutionCoordinateSystem;
  case 556: return Reco_NodeWithVector;
  case 557: return Reco_ParametricCurve3dElementCoordinateDirection;
  case 558: return Reco_ParametricCurve3dElementCoordinateSystem;
  case 559: return Reco_ParametricSurface3dElementCoordinateSystem;
  case 560: return Reco_Surface3dElementRepresentation;
//  case 561: return Reco_SymmetricTensor22d;
//  case 562: return Reco_SymmetricTensor42d;
//  case 563: return Reco_SymmetricTensor43d;
  case 564: return Reco_Volume3dElementRepresentation;
  case 565: return Reco_DataEnvironment;
  case 566: return Reco_MaterialPropertyRepresentation;
  case 567: return Reco_PropertyDefinitionRelationship;
  case 568: return Reco_PointRepresentation;
  case 569: return Reco_MaterialProperty;
  case 570: return Reco_FeaModelDefinition;
  case 571: return Reco_FreedomAndCoefficient;
  case 572: return Reco_FreedomsList;  
  case 573: return Reco_ProductDefinitionFormationRelationship;
//  case 574: return Reco_FeaModelDefinition;
  case 575: return Reco_NodeDefinition;
  case 576: return Reco_StructuralResponseProperty;
  case 577: return Reco_StructuralResponsePropertyDefinitionRepresentation;
  case 579: return Reco_AlignedSurface3dElementCoordinateSystem;
  case 580: return Reco_ConstantSurface3dElementCoordinateSystem;
  case 581: return Reco_CurveElementIntervalLinearlyVarying;
  case 582: return Reco_FeaCurveSectionGeometricRelationship;
  case 583: return Reco_FeaSurfaceSectionGeometricRelationship;
  
  // ptv 28.01.2003
  case 600: return Reco_DocumentProductAssociation;
  case 601: return Reco_DocumentProductEquivalence;

  // Added by SKL 18.06.2003 for Dimensional Tolerances (CAX-IF TRJ11)
  case 609: return Reco_CylindricityTolerance;
  case 610: return Reco_ShapeRepresentationWithParameters;
  case 611: return Reco_AngularityTolerance;
  case 612: return Reco_ConcentricityTolerance;
  case 613: return Reco_CircularRunoutTolerance;
  case 614: return Reco_CoaxialityTolerance;
  case 615: return Reco_FlatnessTolerance;
  case 616: return Reco_LineProfileTolerance;
  case 617: return Reco_ParallelismTolerance;
  case 618: return Reco_PerpendicularityTolerance;
  case 619: return Reco_PositionTolerance;
  case 620: return Reco_RoundnessTolerance;
  case 621: return Reco_StraightnessTolerance;
  case 622: return Reco_SurfaceProfileTolerance;
  case 623: return Reco_SymmetryTolerance;
  case 624: return Reco_TotalRunoutTolerance;
  case 625: return Reco_GeometricTolerance;
  case 626: return Reco_GeometricToleranceRelationship;
  case 627: return Reco_GeometricToleranceWithDatumReference;
  case 628: return Reco_ModifiedGeometricTolerance;
  case 629: return Reco_Datum;
  case 630: return Reco_DatumFeature;
  case 631: return Reco_DatumReference;
  case 632: return Reco_CommonDatum;
  case 633: return Reco_DatumTarget;
  case 634: return Reco_PlacedDatumTargetFeature;

  case 651 : return Reco_MassMeasureWithUnit;

  // Added by ika for GD&T AP242
  case 660: return Reco_Apex;
  case 661: return Reco_CentreOfSymmetry;
  case 662: return Reco_GeometricAlignment;
  case 663: return Reco_PerpendicularTo;
  case 664: return Reco_Tangent;
  case 665: return Reco_ParallelOffset;
  case 666: return Reco_GeometricItemSpecificUsage;
  case 667: return Reco_IdAttribute;
  case 668: return Reco_ItemIdentifiedRepresentationUsage;
  case 669: return Reco_AllAroundShapeAspect;
  case 670: return Reco_BetweenShapeAspect;
  case 671: return Reco_CompositeGroupShapeAspect;
  case 672: return Reco_ContinuosShapeAspect;
  case 673: return Reco_GeometricToleranceWithDefinedAreaUnit;
  case 674: return Reco_GeometricToleranceWithDefinedUnit;
  case 675: return Reco_GeometricToleranceWithMaximumTolerance;
  case 676: return Reco_GeometricToleranceWithModifiers;
  case 677: return Reco_UnequallyDisposedGeometricTolerance;
  case 678: return Reco_NonUniformZoneDefinition;
  case 679: return Reco_ProjectedZoneDefinition;
  case 680: return Reco_RunoutZoneDefinition;
  case 681: return Reco_RunoutZoneOrientation;
  case 682: return Reco_ToleranceZone;
  case 683: return Reco_ToleranceZoneDefinition;
  case 684: return Reco_ToleranceZoneForm;
  case 685: return Reco_ValueFormatTypeQualifier;
  case 686: return Reco_DatumReferenceCompartment;
  case 687: return Reco_DatumReferenceElement;
  case 688: return Reco_DatumReferenceModifierWithValue;
  case 689: return Reco_DatumSystem;
  case 690: return Reco_GeneralDatumReference;
  case 700: return Reco_IntegerRepresentationItem;
  case 701: return Reco_ValueRepresentationItem;
  case 702: return Reco_FeatureForDatumTargetRelationship;
  case 703: return Reco_DraughtingModelItemAssociation;
  case 704: return Reco_AnnotationPlane;

  case 707 : return Reco_TessellatedAnnotationOccurrence;
  case 709 : return Reco_TessellatedGeometricSet;

  case 710 : return Reco_TessellatedCurveSet;
  case 711 : return Reco_CoordinatesList;
  case 712 : return Reco_ConstructiveGeometryRepresentation;
  case 713 : return Reco_ConstructiveGeometryRepresentationRelationship;
  case 714 : return Reco_CharacterizedRepresentation;
  case 716 : return Reco_CameraModelD3MultiClipping;
  case 717 : return Reco_CameraModelD3MultiClippingIntersection;
  case 718 : return Reco_CameraModelD3MultiClippingUnion;

  default : return PasReco;
  }
}


//=======================================================================
//function : ComplexType
//purpose  : Complex Type (list of types)
//=======================================================================

Standard_Boolean RWStepAP214_ReadWriteModule::ComplexType(const Standard_Integer CN,
                                                          TColStd_SequenceOfAsciiString& types) const
{
  switch(CN)
    {
    case 319:
      types.Append (StepType(48));
      types.Append (StepType(38));
      types.Append (StepType(84));
      types.Append (StepType(144));
      types.Append (StepType(239));
      types.Append (StepType(247));
      types.Append (StepType(311));
      break;
    case 320:
      types.Append (StepType(48));
      types.Append (StepType(38));
      types.Append (StepType(39));
      types.Append (StepType(84));
      types.Append (StepType(144));
      types.Append (StepType(239));
      types.Append (StepType(247));
      break;
    case 321:
      types.Append (StepType(48));
      types.Append (StepType(38));
      types.Append (StepType(84));
      types.Append (StepType(144));
      types.Append (StepType(236));
      types.Append (StepType(239));
      types.Append (StepType(247));
      break;
    case 322:
      types.Append (StepType(43));
      types.Append (StepType(48));
      types.Append (StepType(38));
      types.Append (StepType(84));
      types.Append (StepType(144));
      types.Append (StepType(239));
      types.Append (StepType(247));
      break;
    case 323:
      types.Append (StepType(49));
      types.Append (StepType(40));
      types.Append (StepType(41));
      types.Append (StepType(144));
      types.Append (StepType(240));
      types.Append (StepType(247));
      types.Append (StepType(271));
      break;
    case 324:
      types.Append (StepType(49));
      types.Append (StepType(40));
      types.Append (StepType(144));
      types.Append (StepType(240));
      types.Append (StepType(247));
      types.Append (StepType(271));
      types.Append (StepType(312));
      break;
    case 325:
      types.Append (StepType(49));
      types.Append (StepType(40));
      types.Append (StepType(144));
      types.Append (StepType(237));
      types.Append (StepType(240));
      types.Append (StepType(247));
      types.Append (StepType(271));
      break;
    case 326:
      types.Append (StepType(44));
      types.Append (StepType(49));
      types.Append (StepType(40));
      types.Append (StepType(144));
      types.Append (StepType(240));
      types.Append (StepType(247));
      types.Append (StepType(271));
      break;
    case 327:
      types.Append (StepType(158));
      types.Append (StepType(169));
      types.Append (StepType(264));
      break;
    case 328:
      types.Append (StepType(169));
      types.Append (StepType(201));
      types.Append (StepType(264));
      break;
    case 329:
      types.Append (StepType(79));
      types.Append (StepType(158));
      types.Append (StepType(169));
      break;
    case 330:
      types.Append (StepType(79));
      types.Append (StepType(169));
      types.Append (StepType(201));
      break;
    case 331:
      types.Append (StepType(143));
      types.Append (StepType(149));
      types.Append (StepType(246));
      break;
    case 332:
      types.Append (StepType(161));
      types.Append (StepType(189));
      types.Append (StepType(247));
      types.Append (StepType(304));
      break;
    case 333:
      types.Append (StepType(143));
      types.Append (StepType(148));
      types.Append (StepType(149));
      types.Append (StepType(246));
      break;
    case 334:
      types.Append (StepType(79));
      types.Append (StepType(169));
      types.Append (StepType(336));
      break;
    case 335:
      types.Append (StepType(169));
      types.Append (StepType(264));
      types.Append (StepType(336));
      break;
    case 337:
      types.Append (StepType(52));
      types.Append (StepType(134));
      types.Append (StepType(144));
      types.Append (StepType(162));
      types.Append (StepType(247));
      types.Append (StepType(266));
      break;
    case 338:
      types.Append (StepType(143));
      types.Append (StepType(188));
      types.Append (StepType(246));
      break;
    case 344:
      types.Append (StepType(169));
      types.Append (StepType(342));
      types.Append (StepType(264));
      break;
    case 345:
      types.Append (StepType(169));
      types.Append (StepType(264));
      types.Append (StepType(343));
      break;
    case 346:
      types.Append (StepType(79));
      types.Append (StepType(169));
      types.Append (StepType(342));
      break;
    case 347:
      types.Append (StepType(79));
      types.Append (StepType(169));
      types.Append (StepType(343));
      break;
    case 357:
      types.Append (StepType(157));
      types.Append (StepType(165));
      types.Append (StepType(310));
      break;
    case 358: //:n5
      types.Append (StepType(48));
      types.Append (StepType(84));
      types.Append (StepType(144));
      types.Append (StepType(247));
      types.Append (StepType(272));
      break;
    case 389:
      types.Append (StepType(249));
      types.Append (StepType(388));
      types.Append (StepType(387));
      break;
    case 409:
      types.Append (StepType(407));
      types.Append (StepType(169));
      types.Append (StepType(264));
      break;
    case 410:
      types.Append (StepType(169));
      types.Append (StepType(264));
      types.Append (StepType(408));
      break;
    case 411:
      types.Append (StepType(407));
      types.Append (StepType(79));
      types.Append (StepType(169));
      break;
    case 412:
      types.Append (StepType(79));
      types.Append (StepType(169));
      types.Append (StepType(408));
      break;
    case 463:
      types.Append (StepType(98));
      types.Append (StepType(245));
      types.Append (StepType(262));
      break;
    case 481:
      types.Append (StepType(406));
      types.Append (StepType(480));
      types.Append (StepType(247));
      break;
    case 574:
      types.Append (StepType(501));
      types.Append (StepType(169));
      types.Append (StepType(264));
      break;
    case 578:
      types.Append (StepType(169));
      types.Append (StepType(264));
      types.Append (StepType(502));
      break;
    case 635:
      types.Append (StepType(157));
      types.Append (StepType(406));
      types.Append (StepType(165));
      types.Append (StepType(247));
      break;
    case 636:
      types.Append (StepType(625));
      types.Append (StepType(627));
      types.Append (StepType(628));
      types.Append (StepType(619));
      break;
    case 650:
      types.Append (StepType(79));
      types.Append (StepType(501));
      types.Append (StepType(169));
      break;
    case 691:
      types.Append (StepType(406));
      types.Append (StepType(165));
      types.Append (StepType(200));
      types.Append (StepType(247));
      break;
    case 692:
      types.Append (StepType(157));
      types.Append (StepType(406));
      types.Append (StepType(165));
      types.Append (StepType(480));
      types.Append (StepType(247));
      break;
    case 693:
      types.Append (StepType(406));
      types.Append (StepType(165));
      types.Append (StepType(200));
      types.Append (StepType(480));
      types.Append (StepType(247));
      break;
    case 694:
      types.Append (StepType(625));
      types.Append (StepType(627));
      types.Append (StepType(625));
      break;
    case 695:
      types.Append (StepType(625));
      types.Append (StepType(627));
      types.Append (StepType(676));
      types.Append (StepType(625));
      break;
    case 696:
      types.Append (StepType(625));
      types.Append (StepType(676));
      types.Append (StepType(625));
      break;
    case 697:
      types.Append (StepType(625));
      types.Append (StepType(627));
      types.Append (StepType(625));
      types.Append (StepType(677));
      break;
      default : return Standard_False;
    case 698:
      types.Append (StepType(671));
      types.Append (StepType(470));
      types.Append (StepType(630));
      types.Append (StepType(258));
      break;
    case 699:
      types.Append (StepType(470));
      types.Append (StepType(630));
      types.Append (StepType(258));
      break;
    case 705:
      types.Append (StepType(625));
      types.Append (StepType(627));
      types.Append (StepType(675));
      types.Append (StepType(676));
      types.Append (StepType(625));
      break;
    case 706:
      types.Append (StepType(625));
      types.Append (StepType(675));
      types.Append (StepType(676));
      types.Append (StepType(625));
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
    }
  return Standard_True;
}


//=======================================================================
//function : ReadStep
//purpose  : Reading of a file
//=======================================================================

void RWStepAP214_ReadWriteModule::ReadStep(const Standard_Integer CN,
                                           const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(Standard_Transient)&ent) const
{
  if (CN == 0) {
#ifdef OCCT_DEBUG
    cout << "CN = 0 for num = " << num << endl;
#endif
    return;
  }
  switch (CN) {
  case 1 : 
    {
      DeclareAndCast(StepBasic_Address, anent, ent);
      RWStepBasic_RWAddress tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 2 : 
    {
      DeclareAndCast(StepShape_AdvancedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWAdvancedBrepShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 3 : 
    {
      DeclareAndCast(StepShape_AdvancedFace, anent, ent);
      RWStepShape_RWAdvancedFace tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 4 :
    {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrence, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrence tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 5:
  {
    DeclareAndCast(StepVisual_AnnotationFillArea, anent, ent);
    RWStepVisual_RWAnnotationFillArea tool;
    tool.ReadStep(data, num, ach, anent);
  }
    break;
  case 6:
  {
    DeclareAndCast(StepVisual_AnnotationFillAreaOccurrence, anent, ent);
    RWStepVisual_RWAnnotationFillAreaOccurrence tool;
    tool.ReadStep(data, num, ach, anent);
  }
    break;
  case 7 : 
    {
      DeclareAndCast(StepVisual_AnnotationOccurrence, anent, ent);
      RWStepVisual_RWAnnotationOccurrence tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 11 : 
    {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 12 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 13 : 
    {
      DeclareAndCast(StepBasic_ApplicationContext, anent, ent);
      RWStepBasic_RWApplicationContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 14 : 
    {
      DeclareAndCast(StepBasic_ApplicationContextElement, anent, ent);
      RWStepBasic_RWApplicationContextElement tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 15 : 
    {
      DeclareAndCast(StepBasic_ApplicationProtocolDefinition, anent, ent);
      RWStepBasic_RWApplicationProtocolDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 16 : 
    {
      DeclareAndCast(StepBasic_Approval, anent, ent);
      RWStepBasic_RWApproval tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 18 : 
    {
      DeclareAndCast(StepBasic_ApprovalPersonOrganization, anent, ent);
      RWStepBasic_RWApprovalPersonOrganization tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 19 : 
    {
      DeclareAndCast(StepBasic_ApprovalRelationship, anent, ent);
      RWStepBasic_RWApprovalRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 20 : 
    {
      DeclareAndCast(StepBasic_ApprovalRole, anent, ent);
      RWStepBasic_RWApprovalRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 21 : 
    {
      DeclareAndCast(StepBasic_ApprovalStatus, anent, ent);
      RWStepBasic_RWApprovalStatus tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 22 : 
    {
      DeclareAndCast(StepVisual_AreaInSet, anent, ent);
      RWStepVisual_RWAreaInSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 23 : 
    {
      DeclareAndCast(StepAP214_AutoDesignActualDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAndTimeAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 24 : 
    {
      DeclareAndCast(StepAP214_AutoDesignActualDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 25 : 
    {
      DeclareAndCast(StepAP214_AutoDesignApprovalAssignment, anent, ent);
      RWStepAP214_RWAutoDesignApprovalAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 26 : 
    {
      DeclareAndCast(StepAP214_AutoDesignDateAndPersonAssignment, anent, ent);
      RWStepAP214_RWAutoDesignDateAndPersonAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 27 : 
    {
      DeclareAndCast(StepAP214_AutoDesignGroupAssignment, anent, ent);
      RWStepAP214_RWAutoDesignGroupAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 28 : 
    {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 29 : 
    {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 30 : 
    {
      DeclareAndCast(StepAP214_AutoDesignOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignOrganizationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 31 : 
    {
      DeclareAndCast(StepAP214_AutoDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 32 : 
    {
      DeclareAndCast(StepAP214_AutoDesignPresentedItem, anent, ent);
      RWStepAP214_RWAutoDesignPresentedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 33 : 
    {
      DeclareAndCast(StepAP214_AutoDesignSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignSecurityClassificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }

    break;
  case 35 : 
    {
      DeclareAndCast(StepGeom_Axis1Placement, anent, ent);
      RWStepGeom_RWAxis1Placement tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 36 : 
    {
      DeclareAndCast(StepGeom_Axis2Placement2d, anent, ent);
      RWStepGeom_RWAxis2Placement2d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 37 : 
    {
      DeclareAndCast(StepGeom_Axis2Placement3d, anent, ent);
      RWStepGeom_RWAxis2Placement3d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 38 : 
    {
      DeclareAndCast(StepGeom_BSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 39 : 
    {
      DeclareAndCast(StepGeom_BSplineCurveWithKnots, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnots tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 40 : 
    {
      DeclareAndCast(StepGeom_BSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 41 : 
    {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnots, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnots tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 42 : 
    {
      DeclareAndCast(StepVisual_BackgroundColour, anent, ent);
      RWStepVisual_RWBackgroundColour tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 43 : 
    {
      DeclareAndCast(StepGeom_BezierCurve, anent, ent);
      RWStepGeom_RWBezierCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 44 : 
    {
      DeclareAndCast(StepGeom_BezierSurface, anent, ent);
      RWStepGeom_RWBezierSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 45 : 
    {
      DeclareAndCast(StepShape_Block, anent, ent);
      RWStepShape_RWBlock tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 46 : 
    {
      DeclareAndCast(StepShape_BooleanResult, anent, ent);
      RWStepShape_RWBooleanResult tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 47 : 
    {
      DeclareAndCast(StepGeom_BoundaryCurve, anent, ent);
      RWStepGeom_RWBoundaryCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 48 : 
    {
      DeclareAndCast(StepGeom_BoundedCurve, anent, ent);
      RWStepGeom_RWBoundedCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 49 : 
    {
      DeclareAndCast(StepGeom_BoundedSurface, anent, ent);
      RWStepGeom_RWBoundedSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 50 : 
    {
      DeclareAndCast(StepShape_BoxDomain, anent, ent);
      RWStepShape_RWBoxDomain tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 51 : 
    {
      DeclareAndCast(StepShape_BoxedHalfSpace, anent, ent);
      RWStepShape_RWBoxedHalfSpace tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 52 : 
    {
      DeclareAndCast(StepShape_BrepWithVoids, anent, ent);
      RWStepShape_RWBrepWithVoids tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 53 : 
    {
      DeclareAndCast(StepBasic_CalendarDate, anent, ent);
      RWStepBasic_RWCalendarDate tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 54 : 
    {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 55 : 
    {
      DeclareAndCast(StepVisual_CameraModel, anent, ent);
      RWStepVisual_RWCameraModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 56 : 
    {
      DeclareAndCast(StepVisual_CameraModelD2, anent, ent);
      RWStepVisual_RWCameraModelD2 tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 57 : 
    {
      DeclareAndCast(StepVisual_CameraModelD3, anent, ent);
      RWStepVisual_RWCameraModelD3 tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 58 : 
    {
      DeclareAndCast(StepVisual_CameraUsage, anent, ent);
      RWStepVisual_RWCameraUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 59 : 
    {
      DeclareAndCast(StepGeom_CartesianPoint, anent, ent);
      RWStepGeom_RWCartesianPoint tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 60 : 
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 61 : 
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator3d, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator3d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 62 : 
    {
      DeclareAndCast(StepGeom_Circle, anent, ent);
      RWStepGeom_RWCircle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 63 : 
    {
      DeclareAndCast(StepShape_ClosedShell, anent, ent);
      RWStepShape_RWClosedShell tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 64 : 
    {
      DeclareAndCast(StepVisual_Colour, anent, ent);
      RWStepVisual_RWColour tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 65 : 
    {
      DeclareAndCast(StepVisual_ColourRgb, anent, ent);
      RWStepVisual_RWColourRgb tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 66 : 
    {
      DeclareAndCast(StepVisual_ColourSpecification, anent, ent);
      RWStepVisual_RWColourSpecification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 67 : 
    {
      DeclareAndCast(StepGeom_CompositeCurve, anent, ent);
      RWStepGeom_RWCompositeCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 68 : 
    {
      DeclareAndCast(StepGeom_CompositeCurveOnSurface, anent, ent);
      RWStepGeom_RWCompositeCurveOnSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 69 : 
    {
      DeclareAndCast(StepGeom_CompositeCurveSegment, anent, ent);
      RWStepGeom_RWCompositeCurveSegment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 70 : 
    {
      DeclareAndCast(StepVisual_CompositeText, anent, ent);
      RWStepVisual_RWCompositeText tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 73 : 
    {
      DeclareAndCast(StepVisual_CompositeTextWithExtent, anent, ent);
      RWStepVisual_RWCompositeTextWithExtent tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 74 : 
    {
      DeclareAndCast(StepGeom_Conic, anent, ent);
      RWStepGeom_RWConic tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 75 : 
    {
      DeclareAndCast(StepGeom_ConicalSurface, anent, ent);
      RWStepGeom_RWConicalSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 76 : 
    {
      DeclareAndCast(StepShape_ConnectedFaceSet, anent, ent);
      RWStepShape_RWConnectedFaceSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 77 : 
    {
      DeclareAndCast(StepVisual_ContextDependentInvisibility, anent, ent);
      RWStepVisual_RWContextDependentInvisibility tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 78 : 
    {
      DeclareAndCast(StepVisual_ContextDependentOverRidingStyledItem, anent, ent);
      RWStepVisual_RWContextDependentOverRidingStyledItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 79 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 80 : 
    {
      DeclareAndCast(StepBasic_CoordinatedUniversalTimeOffset, anent, ent);
      RWStepBasic_RWCoordinatedUniversalTimeOffset tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 82 : 
    {
      DeclareAndCast(StepShape_CsgShapeRepresentation, anent, ent);
      RWStepShape_RWCsgShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 83 : 
    {
      DeclareAndCast(StepShape_CsgSolid, anent, ent);
      RWStepShape_RWCsgSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 84 : 
    {
      DeclareAndCast(StepGeom_Curve, anent, ent);
      RWStepGeom_RWCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 85 : 
    {
      DeclareAndCast(StepGeom_CurveBoundedSurface, anent, ent);
      RWStepGeom_RWCurveBoundedSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 86 : 
    {
      DeclareAndCast(StepGeom_CurveReplica, anent, ent);
      RWStepGeom_RWCurveReplica tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 87 : 
    {
      DeclareAndCast(StepVisual_CurveStyle, anent, ent);
      RWStepVisual_RWCurveStyle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 88 : 
    {
      DeclareAndCast(StepVisual_CurveStyleFont, anent, ent);
      RWStepVisual_RWCurveStyleFont tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 89 : 
    {
      DeclareAndCast(StepVisual_CurveStyleFontPattern, anent, ent);
      RWStepVisual_RWCurveStyleFontPattern tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 90 : 
    {
      DeclareAndCast(StepGeom_CylindricalSurface, anent, ent);
      RWStepGeom_RWCylindricalSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 91 : 
    {
      DeclareAndCast(StepBasic_Date, anent, ent);
      RWStepBasic_RWDate tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 92 : 
    {
      DeclareAndCast(StepBasic_DateAndTime, anent, ent);
      RWStepBasic_RWDateAndTime tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 95 : 
    {
      DeclareAndCast(StepBasic_DateRole, anent, ent);
      RWStepBasic_RWDateRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 96 : 
    {
      DeclareAndCast(StepBasic_DateTimeRole, anent, ent);
      RWStepBasic_RWDateTimeRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 98 : 
    {
      DeclareAndCast(StepRepr_DefinitionalRepresentation, anent, ent);
      RWStepRepr_RWDefinitionalRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 99 : 
    {
      DeclareAndCast(StepGeom_DegeneratePcurve, anent, ent);
      RWStepGeom_RWDegeneratePcurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 100 : 
    {
      DeclareAndCast(StepGeom_DegenerateToroidalSurface, anent, ent);
      RWStepGeom_RWDegenerateToroidalSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 101 : 
    {
      DeclareAndCast(StepRepr_DescriptiveRepresentationItem, anent, ent);
      RWStepRepr_RWDescriptiveRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 104 : 
    {
      DeclareAndCast(StepBasic_DimensionalExponents, anent, ent);
      RWStepBasic_RWDimensionalExponents tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 105 : 
    {
      DeclareAndCast(StepGeom_Direction, anent, ent);
      RWStepGeom_RWDirection tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 106 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 107 :
    {
      DeclareAndCast(StepVisual_DraughtingCallout, anent, ent);
      RWStepVisual_RWDraughtingCallout tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 108 : 
    {
      DeclareAndCast(StepVisual_DraughtingPreDefinedColour, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedColour tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 109 : 
    {
      DeclareAndCast(StepVisual_DraughtingPreDefinedCurveFont, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedCurveFont tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 115 : 
    {
      DeclareAndCast(StepShape_Edge, anent, ent);
      RWStepShape_RWEdge tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 116 : 
    {
      DeclareAndCast(StepShape_EdgeCurve, anent, ent);
      RWStepShape_RWEdgeCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 117 : 
    {
      DeclareAndCast(StepShape_EdgeLoop, anent, ent);
      RWStepShape_RWEdgeLoop tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 118 : 
    {
      DeclareAndCast(StepGeom_ElementarySurface, anent, ent);
      RWStepGeom_RWElementarySurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 119 : 
    {
      DeclareAndCast(StepGeom_Ellipse, anent, ent);
      RWStepGeom_RWEllipse tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 120 : 
    {
      DeclareAndCast(StepGeom_EvaluatedDegeneratePcurve, anent, ent);
      RWStepGeom_RWEvaluatedDegeneratePcurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 121 : 
    {
      DeclareAndCast(StepBasic_ExternalSource, anent, ent);
      RWStepBasic_RWExternalSource tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 122 : 
    {
      DeclareAndCast(StepVisual_ExternallyDefinedCurveFont, anent, ent);
      RWStepVisual_RWExternallyDefinedCurveFont tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 124 : 
  case 126 : 
    {
      DeclareAndCast(StepBasic_ExternallyDefinedItem, anent, ent);
      RWStepBasic_RWExternallyDefinedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 128 : 
    {
      DeclareAndCast(StepShape_ExtrudedAreaSolid, anent, ent);
      RWStepShape_RWExtrudedAreaSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 129 : 
    {
      DeclareAndCast(StepShape_Face, anent, ent);
      RWStepShape_RWFace tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 131 : 
    {
      DeclareAndCast(StepShape_FaceBound, anent, ent);
      RWStepShape_RWFaceBound tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 132 : 
    {
      DeclareAndCast(StepShape_FaceOuterBound, anent, ent);
      RWStepShape_RWFaceOuterBound tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 133 : 
    {
      DeclareAndCast(StepShape_FaceSurface, anent, ent);
      RWStepShape_RWFaceSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 134 : 
    {
      DeclareAndCast(StepShape_FacetedBrep, anent, ent);
      RWStepShape_RWFacetedBrep tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 135 : 
    {
      DeclareAndCast(StepShape_FacetedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWFacetedBrepShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 136 : 
    {
      DeclareAndCast(StepVisual_FillAreaStyle, anent, ent);
      RWStepVisual_RWFillAreaStyle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 137 : 
    {
      DeclareAndCast(StepVisual_FillAreaStyleColour, anent, ent);
      RWStepVisual_RWFillAreaStyleColour tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 141 : 
    {
      DeclareAndCast(StepRepr_FunctionallyDefinedTransformation, anent, ent);
      RWStepRepr_RWFunctionallyDefinedTransformation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 142 : 
    {
      DeclareAndCast(StepShape_GeometricCurveSet, anent, ent);
      RWStepShape_RWGeometricCurveSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 143 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 144 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationItem, anent, ent);
      RWStepGeom_RWGeometricRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 145 : 
    {
      DeclareAndCast(StepShape_GeometricSet, anent, ent);
      RWStepShape_RWGeometricSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 146 : 
    {
      DeclareAndCast(StepShape_GeometricallyBoundedSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 147 : 
    {
      DeclareAndCast(StepShape_GeometricallyBoundedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 148 : 
    {
      DeclareAndCast(StepRepr_GlobalUncertaintyAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUncertaintyAssignedContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 149 : 
    {
      DeclareAndCast(StepRepr_GlobalUnitAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUnitAssignedContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 150 : 
    {
      DeclareAndCast(StepBasic_Group, anent, ent);
      RWStepBasic_RWGroup tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 152 : 
    {
      DeclareAndCast(StepBasic_GroupRelationship, anent, ent);
      RWStepBasic_RWGroupRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 153 : 
    {
      DeclareAndCast(StepShape_HalfSpaceSolid, anent, ent);
      RWStepShape_RWHalfSpaceSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 154 : 
    {
      DeclareAndCast(StepGeom_Hyperbola, anent, ent);
      RWStepGeom_RWHyperbola tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 155 : 
    {
      DeclareAndCast(StepGeom_IntersectionCurve, anent, ent);
      RWStepGeom_RWIntersectionCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 156 : 
    {
      DeclareAndCast(StepVisual_Invisibility, anent, ent);
      RWStepVisual_RWInvisibility tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 157 : 
    {
      DeclareAndCast(StepBasic_LengthMeasureWithUnit, anent, ent);
      RWStepBasic_RWLengthMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 158 : 
    {
      DeclareAndCast(StepBasic_LengthUnit, anent, ent);
      RWStepBasic_RWLengthUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 159 : 
    {
      DeclareAndCast(StepGeom_Line, anent, ent);
      RWStepGeom_RWLine tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 160 : 
    {
      DeclareAndCast(StepBasic_LocalTime, anent, ent);
      RWStepBasic_RWLocalTime tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 161 : 
    {
      DeclareAndCast(StepShape_Loop, anent, ent);
      RWStepShape_RWLoop tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 162 : 
    {
      DeclareAndCast(StepShape_ManifoldSolidBrep, anent, ent);
      RWStepShape_RWManifoldSolidBrep tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 163 : 
    {
      DeclareAndCast(StepShape_ManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWManifoldSurfaceShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 164 : 
    {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 165 : 
    {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 166 : 
    {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationArea, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationArea tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 167 : 
    {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationRepresentation, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 169 : 
    {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 171 : 
    {
      DeclareAndCast(StepGeom_OffsetCurve3d, anent, ent);
      RWStepGeom_RWOffsetCurve3d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 172 : 
    {
      DeclareAndCast(StepGeom_OffsetSurface, anent, ent);
      RWStepGeom_RWOffsetSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 174 : 
    {
      DeclareAndCast(StepShape_OpenShell, anent, ent);
      RWStepShape_RWOpenShell tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 175 : 
    {
      DeclareAndCast(StepBasic_OrdinalDate, anent, ent);
      RWStepBasic_RWOrdinalDate tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 176 : 
    {
      DeclareAndCast(StepBasic_Organization, anent, ent);
      RWStepBasic_RWOrganization tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 178 : 
    {
      DeclareAndCast(StepBasic_OrganizationRole, anent, ent);
      RWStepBasic_RWOrganizationRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 179 : 
    {
      DeclareAndCast(StepBasic_OrganizationalAddress, anent, ent);
      RWStepBasic_RWOrganizationalAddress tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 180 : 
    {
      DeclareAndCast(StepShape_OrientedClosedShell, anent, ent);
      RWStepShape_RWOrientedClosedShell tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 181 : 
    {
      DeclareAndCast(StepShape_OrientedEdge, anent, ent);
      RWStepShape_RWOrientedEdge tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 182 : 
    {
      DeclareAndCast(StepShape_OrientedFace, anent, ent);
      RWStepShape_RWOrientedFace tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 183 : 
    {
      DeclareAndCast(StepShape_OrientedOpenShell, anent, ent);
      RWStepShape_RWOrientedOpenShell tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 184 : 
    {
      DeclareAndCast(StepShape_OrientedPath, anent, ent);
      RWStepShape_RWOrientedPath tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 185 : 
    {
      DeclareAndCast(StepGeom_OuterBoundaryCurve, anent, ent);
      RWStepGeom_RWOuterBoundaryCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 186 : 
    {
      DeclareAndCast(StepVisual_OverRidingStyledItem, anent, ent);
      RWStepVisual_RWOverRidingStyledItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 187 : 
    {
      DeclareAndCast(StepGeom_Parabola, anent, ent);
      RWStepGeom_RWParabola tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 188 : 
    {
      DeclareAndCast(StepRepr_ParametricRepresentationContext, anent, ent);
      RWStepRepr_RWParametricRepresentationContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 189 : 
    {
      DeclareAndCast(StepShape_Path, anent, ent);
      RWStepShape_RWPath tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 190 : 
    {
      DeclareAndCast(StepGeom_Pcurve, anent, ent);
      RWStepGeom_RWPcurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 191 : 
    {
      DeclareAndCast(StepBasic_Person, anent, ent);
      RWStepBasic_RWPerson tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 192 : 
    {
      DeclareAndCast(StepBasic_PersonAndOrganization, anent, ent);
      RWStepBasic_RWPersonAndOrganization tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 194 : 
    {
      DeclareAndCast(StepBasic_PersonAndOrganizationRole, anent, ent);
      RWStepBasic_RWPersonAndOrganizationRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 195 : 
    {
      DeclareAndCast(StepBasic_PersonalAddress, anent, ent);
      RWStepBasic_RWPersonalAddress tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 196 : 
    {
      DeclareAndCast(StepGeom_Placement, anent, ent);
      RWStepGeom_RWPlacement tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 197 : 
    {
      DeclareAndCast(StepVisual_PlanarBox, anent, ent);
      RWStepVisual_RWPlanarBox tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 198 : 
    {
      DeclareAndCast(StepVisual_PlanarExtent, anent, ent);
      RWStepVisual_RWPlanarExtent tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 199 : 
    {
      DeclareAndCast(StepGeom_Plane, anent, ent);
      RWStepGeom_RWPlane tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 200 : 
    {
      DeclareAndCast(StepBasic_PlaneAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWPlaneAngleMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 201 : 
    {
      DeclareAndCast(StepBasic_PlaneAngleUnit, anent, ent);
      RWStepBasic_RWPlaneAngleUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 202 : 
    {
      DeclareAndCast(StepGeom_Point, anent, ent);
      RWStepGeom_RWPoint tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 203 : 
    {
      DeclareAndCast(StepGeom_PointOnCurve, anent, ent);
      RWStepGeom_RWPointOnCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 204 : 
    {
      DeclareAndCast(StepGeom_PointOnSurface, anent, ent);
      RWStepGeom_RWPointOnSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 205 : 
    {
      DeclareAndCast(StepGeom_PointReplica, anent, ent);
      RWStepGeom_RWPointReplica tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 206 : 
    {
      DeclareAndCast(StepVisual_PointStyle, anent, ent);
      RWStepVisual_RWPointStyle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 207 : 
    {
      DeclareAndCast(StepShape_PolyLoop, anent, ent);
      RWStepShape_RWPolyLoop tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 208 : 
    {
      DeclareAndCast(StepGeom_Polyline, anent, ent);
      RWStepGeom_RWPolyline tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 209 : 
    {
      DeclareAndCast(StepVisual_PreDefinedColour, anent, ent);
      RWStepVisual_RWPreDefinedColour tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 210 : 
    {
      DeclareAndCast(StepVisual_PreDefinedCurveFont, anent, ent);
      RWStepVisual_RWPreDefinedCurveFont tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 211 : 
  case 213 : 
    {
      DeclareAndCast(StepVisual_PreDefinedItem, anent, ent);
      RWStepVisual_RWPreDefinedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 214 : 
    {
      DeclareAndCast(StepVisual_PresentationArea, anent, ent);
      RWStepVisual_RWPresentationArea tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 215 : 
    {
      DeclareAndCast(StepVisual_PresentationLayerAssignment, anent, ent);
      RWStepVisual_RWPresentationLayerAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 216 : 
    {
      DeclareAndCast(StepVisual_PresentationRepresentation, anent, ent);
      RWStepVisual_RWPresentationRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 217 : 
    {
      DeclareAndCast(StepVisual_PresentationSet, anent, ent);
      RWStepVisual_RWPresentationSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 218 : 
    {
      DeclareAndCast(StepVisual_PresentationSize, anent, ent);
      RWStepVisual_RWPresentationSize tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 219 : 
    {
      DeclareAndCast(StepVisual_PresentationStyleAssignment, anent, ent);
      RWStepVisual_RWPresentationStyleAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 220 : 
    {
      DeclareAndCast(StepVisual_PresentationStyleByContext, anent, ent);
      RWStepVisual_RWPresentationStyleByContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 221 : 
    {
      DeclareAndCast(StepVisual_PresentationView, anent, ent);
      RWStepVisual_RWPresentationView tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 223 : 
    {
      DeclareAndCast(StepBasic_Product, anent, ent);
      RWStepBasic_RWProduct tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 224 : 
    {
      DeclareAndCast(StepBasic_ProductCategory, anent, ent);
      RWStepBasic_RWProductCategory tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 225 : 
    {
      DeclareAndCast(StepBasic_ProductContext, anent, ent);
      RWStepBasic_RWProductContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 227 : 
    {
      DeclareAndCast(StepBasic_ProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 228 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 229 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormation, anent, ent);
      RWStepBasic_RWProductDefinitionFormation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 230 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormationWithSpecifiedSource, anent, ent);
      RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 231 : 
    {
      DeclareAndCast(StepRepr_ProductDefinitionShape, anent, ent);
      RWStepRepr_RWProductDefinitionShape tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 232 : 
    {
      DeclareAndCast(StepBasic_ProductRelatedProductCategory, anent, ent);
      RWStepBasic_RWProductRelatedProductCategory tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 233 : 
    {
      DeclareAndCast(StepBasic_ProductType, anent, ent);
      RWStepBasic_RWProductType tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 234 : 
    {
      DeclareAndCast(StepRepr_PropertyDefinition, anent, ent);
      RWStepRepr_RWPropertyDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 235 : 
    {
      DeclareAndCast(StepRepr_PropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWPropertyDefinitionRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 236 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 237 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 238 : 
    {
      DeclareAndCast(StepBasic_RatioMeasureWithUnit, anent, ent);
      RWStepBasic_RWRatioMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 239 : 
    {
      DeclareAndCast(StepGeom_RationalBSplineCurve, anent, ent);
      RWStepGeom_RWRationalBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 240 : 
    {
      DeclareAndCast(StepGeom_RationalBSplineSurface, anent, ent);
      RWStepGeom_RWRationalBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 241 : 
    {
      DeclareAndCast(StepGeom_RectangularCompositeSurface, anent, ent);
      RWStepGeom_RWRectangularCompositeSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 242 : 
    {
      DeclareAndCast(StepGeom_RectangularTrimmedSurface, anent, ent);
      RWStepGeom_RWRectangularTrimmedSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 243 : 
    {
      DeclareAndCast(StepAP214_RepItemGroup, anent, ent);
      RWStepAP214_RWRepItemGroup tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 244 : 
    {
      DeclareAndCast(StepGeom_ReparametrisedCompositeCurveSegment, anent, ent);
      RWStepGeom_RWReparametrisedCompositeCurveSegment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 245 : 
    {
      DeclareAndCast(StepRepr_Representation, anent, ent);
      RWStepRepr_RWRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 246 : 
    {
      DeclareAndCast(StepRepr_RepresentationContext, anent, ent);
      RWStepRepr_RWRepresentationContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 247 : 
    {
      DeclareAndCast(StepRepr_RepresentationItem, anent, ent);
      RWStepRepr_RWRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 248 : 
    {
      DeclareAndCast(StepRepr_RepresentationMap, anent, ent);
      RWStepRepr_RWRepresentationMap tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 249 : 
    {
      DeclareAndCast(StepRepr_RepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 250 : 
    {
      DeclareAndCast(StepShape_RevolvedAreaSolid, anent, ent);
      RWStepShape_RWRevolvedAreaSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 251 : 
    {
      DeclareAndCast(StepShape_RightAngularWedge, anent, ent);
      RWStepShape_RWRightAngularWedge tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 252 : 
    {
      DeclareAndCast(StepShape_RightCircularCone, anent, ent);
      RWStepShape_RWRightCircularCone tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 253 : 
    {
      DeclareAndCast(StepShape_RightCircularCylinder, anent, ent);
      RWStepShape_RWRightCircularCylinder tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 254 : 
    {
      DeclareAndCast(StepGeom_SeamCurve, anent, ent);
      RWStepGeom_RWSeamCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 255 : 
    {
      DeclareAndCast(StepBasic_SecurityClassification, anent, ent);
      RWStepBasic_RWSecurityClassification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 257 : 
    {
      DeclareAndCast(StepBasic_SecurityClassificationLevel, anent, ent);
      RWStepBasic_RWSecurityClassificationLevel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 258 : 
    {
      DeclareAndCast(StepRepr_ShapeAspect, anent, ent);
      RWStepRepr_RWShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 259 : 
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 260 : 
    {
      DeclareAndCast(StepRepr_ShapeAspectTransition, anent, ent);
      RWStepRepr_RWShapeAspectTransition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 261 : 
    {
      DeclareAndCast(StepShape_ShapeDefinitionRepresentation, anent, ent);
      RWStepShape_RWShapeDefinitionRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 262 : 
    {
      DeclareAndCast(StepShape_ShapeRepresentation, anent, ent);
      RWStepShape_RWShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 263 : 
    {
      DeclareAndCast(StepShape_ShellBasedSurfaceModel, anent, ent);
      RWStepShape_RWShellBasedSurfaceModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 264 : 
    {
      DeclareAndCast(StepBasic_SiUnit, anent, ent);
      RWStepBasic_RWSiUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 265 : 
    {
      DeclareAndCast(StepBasic_SolidAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWSolidAngleMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 266 : 
    {
      DeclareAndCast(StepShape_SolidModel, anent, ent);
      RWStepShape_RWSolidModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 267 : 
    {
      DeclareAndCast(StepShape_SolidReplica, anent, ent);
      RWStepShape_RWSolidReplica tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 268 : 
    {
      DeclareAndCast(StepShape_Sphere, anent, ent);
      RWStepShape_RWSphere tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 269 : 
    {
      DeclareAndCast(StepGeom_SphericalSurface, anent, ent);
      RWStepGeom_RWSphericalSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 270 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 271 : 
    {
      DeclareAndCast(StepGeom_Surface, anent, ent);
      RWStepGeom_RWSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 272 : 
    {
      DeclareAndCast(StepGeom_SurfaceCurve, anent, ent);
      RWStepGeom_RWSurfaceCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 273 : 
    {
      DeclareAndCast(StepGeom_SurfaceOfLinearExtrusion, anent, ent);
      RWStepGeom_RWSurfaceOfLinearExtrusion tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 274 : 
    {
      DeclareAndCast(StepGeom_SurfaceOfRevolution, anent, ent);
      RWStepGeom_RWSurfaceOfRevolution tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 275 : 
    {
      DeclareAndCast(StepGeom_SurfacePatch, anent, ent);
      RWStepGeom_RWSurfacePatch tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 276 : 
    {
      DeclareAndCast(StepGeom_SurfaceReplica, anent, ent);
      RWStepGeom_RWSurfaceReplica tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 277 : 
    {
      DeclareAndCast(StepVisual_SurfaceSideStyle, anent, ent);
      RWStepVisual_RWSurfaceSideStyle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 278 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleBoundary, anent, ent);
      RWStepVisual_RWSurfaceStyleBoundary tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 279 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleControlGrid, anent, ent);
      RWStepVisual_RWSurfaceStyleControlGrid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 280 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleFillArea, anent, ent);
      RWStepVisual_RWSurfaceStyleFillArea tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 281 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleParameterLine, anent, ent);
      RWStepVisual_RWSurfaceStyleParameterLine tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 282 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleSegmentationCurve, anent, ent);
      RWStepVisual_RWSurfaceStyleSegmentationCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 283 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleSilhouette, anent, ent);
      RWStepVisual_RWSurfaceStyleSilhouette tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 284 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleUsage, anent, ent);
      RWStepVisual_RWSurfaceStyleUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 285 : 
    {
      DeclareAndCast(StepShape_SweptAreaSolid, anent, ent);
      RWStepShape_RWSweptAreaSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 286 : 
    {
      DeclareAndCast(StepGeom_SweptSurface, anent, ent);
      RWStepGeom_RWSweptSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 292 : 
    {
      DeclareAndCast(StepVisual_Template, anent, ent);
      RWStepVisual_RWTemplate tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 293 : 
    {
      DeclareAndCast(StepVisual_TemplateInstance, anent, ent);
      RWStepVisual_RWTemplateInstance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 295 : 
    {
      DeclareAndCast(StepVisual_TextLiteral, anent, ent);
      RWStepVisual_RWTextLiteral tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 300 : 
    {
      DeclareAndCast(StepVisual_TextStyle, anent, ent);
      RWStepVisual_RWTextStyle tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 301 : 
    {
      DeclareAndCast(StepVisual_TextStyleForDefinedFont, anent, ent);
      RWStepVisual_RWTextStyleForDefinedFont tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 302 : 
    {
      DeclareAndCast(StepVisual_TextStyleWithBoxCharacteristics, anent, ent);
      RWStepVisual_RWTextStyleWithBoxCharacteristics tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 304 : 
    {
      DeclareAndCast(StepShape_TopologicalRepresentationItem, anent, ent);
      RWStepShape_RWTopologicalRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 305 : 
    {
      DeclareAndCast(StepGeom_ToroidalSurface, anent, ent);
      RWStepGeom_RWToroidalSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 306 : 
    {
      DeclareAndCast(StepShape_Torus, anent, ent);
      RWStepShape_RWTorus tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 307 : 
    {
      DeclareAndCast(StepShape_TransitionalShapeRepresentation, anent, ent);
      RWStepShape_RWTransitionalShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 308 : 
    {
      DeclareAndCast(StepGeom_TrimmedCurve, anent, ent);
      RWStepGeom_RWTrimmedCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 310 : 
    {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 311 : 
    {
      DeclareAndCast(StepGeom_UniformCurve, anent, ent);
      RWStepGeom_RWUniformCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 312 : 
    {
      DeclareAndCast(StepGeom_UniformSurface, anent, ent);
      RWStepGeom_RWUniformSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 313 : 
    {
      DeclareAndCast(StepGeom_Vector, anent, ent);
      RWStepGeom_RWVector tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 314 : 
    {
      DeclareAndCast(StepShape_Vertex, anent, ent);
      RWStepShape_RWVertex tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 315 : 
    {
      DeclareAndCast(StepShape_VertexLoop, anent, ent);
      RWStepShape_RWVertexLoop tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 316 : 
    {
      DeclareAndCast(StepShape_VertexPoint, anent, ent);
      RWStepShape_RWVertexPoint tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 317 : 
    {
      DeclareAndCast(StepVisual_ViewVolume, anent, ent);
      RWStepVisual_RWViewVolume tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 318 : 
    {
      DeclareAndCast(StepBasic_WeekOfYearAndDayDate, anent, ent);
      RWStepBasic_RWWeekOfYearAndDayDate tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 319 : 
    {
      DeclareAndCast(StepGeom_UniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWUniformCurveAndRationalBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 320 : 
    {
      DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 321 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 322 : 
    {
      DeclareAndCast(StepGeom_BezierCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBezierCurveAndRationalBSplineCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 323 : 
    {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 324 : 
    {
      DeclareAndCast(StepGeom_UniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 325 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 326 : 
    {
      DeclareAndCast(StepGeom_BezierSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 327 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWSiUnitAndLengthUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 328 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndPlaneAngleUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 329 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndLengthUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 330 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 331 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 332 : 
    {
      DeclareAndCast(StepShape_LoopAndPath, anent, ent);
      RWStepShape_RWLoopAndPath tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    // ------------
    // Added by FMA
    // ------------
    
  case 333 :
    {
      DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, anent, ent);
      RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx tool;
      tool.ReadStep(data,num,ach,anent);
    }
    break;
  case 334 :
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndSolidAngleUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit tool;
      tool.ReadStep(data,num,ach,anent);
    }
    break;
  case 335 :
    {
      DeclareAndCast(StepBasic_SiUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndSolidAngleUnit tool;
      tool.ReadStep(data,num,ach,anent);
    }
    break;
  case 336 :
    {
      DeclareAndCast(StepBasic_SolidAngleUnit, anent, ent);
      RWStepBasic_RWSolidAngleUnit tool;
      tool.ReadStep(data,num,ach,anent);
    }
    break;
  case 337 :
    {
      DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids, anent, ent);
      RWStepShape_RWFacetedBrepAndBrepWithVoids tool;
      tool.ReadStep(data,num,ach,anent);
    }
    break;
  case 338 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 339 : 
    {
      DeclareAndCast(StepBasic_MechanicalContext, anent, ent);
      RWStepBasic_RWMechanicalContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 340 :    // added by CKY : DesignContext cf ProductDefinitionContext
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
      tool.ReadStep (data,num,ach,anent);
      break;
    }

    // -----------
    // Added for Rev4
    // -----------

  case 341:  // TimeMeasureWithUnit
    {
      DeclareAndCast(StepBasic_MeasureWithUnit,anent,ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 342:
  case 343:  // RatioUnit, TimeUnit
    {
      DeclareAndCast(StepBasic_NamedUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 344:
    {
      DeclareAndCast(StepBasic_SiUnitAndRatioUnit,anent,ent);
      RWStepBasic_RWSiUnitAndRatioUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 345:
    {
      DeclareAndCast(StepBasic_SiUnitAndTimeUnit,anent,ent);
      RWStepBasic_RWSiUnitAndTimeUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 346:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndRatioUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndRatioUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 347:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndTimeUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndTimeUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 348:  // ApprovalDateTime
    {
      DeclareAndCast(StepBasic_ApprovalDateTime,anent,ent);
      RWStepBasic_RWApprovalDateTime tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 349: // CameraImage 2d and 3d
  case 350:
    {
      DeclareAndCast(StepVisual_CameraImage,anent,ent);
      RWStepVisual_RWCameraImage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 351:
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator,anent,ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 352:
    {
      DeclareAndCast(StepBasic_DerivedUnit,anent,ent);
      RWStepBasic_RWDerivedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 353:
    {
      DeclareAndCast(StepBasic_DerivedUnitElement,anent,ent);
      RWStepBasic_RWDerivedUnitElement tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 354:
    {
      DeclareAndCast(StepRepr_ItemDefinedTransformation,anent,ent);
      RWStepRepr_RWItemDefinedTransformation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 355:
    {
      DeclareAndCast(StepVisual_PresentedItemRepresentation,anent,ent);
      RWStepVisual_RWPresentedItemRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 356:
    {
      DeclareAndCast(StepVisual_PresentationLayerUsage,anent,ent);
      RWStepVisual_RWPresentationLayerUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    // Added for CATIA -  CKY 15-SEP-1997
    // RWUncertaintyMeasureWithUnit sait lire cette variante
  case 357 : 
    {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    //:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
  case 358: 
    {
      DeclareAndCast(StepGeom_SurfaceCurveAndBoundedCurve, anent, ent);
      RWStepGeom_RWSurfaceCurveAndBoundedCurve tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

//  AP214 : CC1 -> CC2
  case 366:
    {
      DeclareAndCast(StepAP214_AutoDesignDocumentReference,anent,ent);
      RWStepAP214_RWAutoDesignDocumentReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 367:
  case 368:
    {
      DeclareAndCast(StepBasic_Document,anent,ent);
      RWStepBasic_RWDocument tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 369:
    {
      DeclareAndCast(StepBasic_DocumentRelationship,anent,ent);
      RWStepBasic_RWDocumentRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 370:
    {
      DeclareAndCast(StepBasic_DocumentType,anent,ent);
      RWStepBasic_RWDocumentType tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 371:
    {
      DeclareAndCast(StepBasic_DocumentUsageConstraint,anent,ent);
      RWStepBasic_RWDocumentUsageConstraint tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 372:
    {
      DeclareAndCast(StepBasic_Effectivity,anent,ent);
      RWStepBasic_RWEffectivity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 373:
    {
      DeclareAndCast(StepBasic_ProductDefinitionEffectivity,anent,ent);
      RWStepBasic_RWProductDefinitionEffectivity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 374:
    {
      DeclareAndCast(StepBasic_ProductDefinitionRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 375:
    {
      DeclareAndCast(StepBasic_ProductDefinitionWithAssociatedDocuments,anent,ent);
      RWStepBasic_RWProductDefinitionWithAssociatedDocuments tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 376:
    {
      DeclareAndCast(StepBasic_PhysicallyModeledProductDefinition,anent,ent);
      RWStepBasic_RWProductDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    
  case 377:
    {
      DeclareAndCast(StepRepr_ProductDefinitionUsage,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 378:
    {
      DeclareAndCast(StepRepr_MakeFromUsageOption,anent,ent);
      RWStepRepr_RWMakeFromUsageOption tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 379:
  case 380:
  case 381:
    {
      DeclareAndCast(StepRepr_AssemblyComponentUsage,anent,ent);
      RWStepRepr_RWAssemblyComponentUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 382:
    {
      DeclareAndCast(StepRepr_QuantifiedAssemblyComponentUsage,anent,ent);
      RWStepRepr_RWQuantifiedAssemblyComponentUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 383:
    {
      DeclareAndCast(StepRepr_SpecifiedHigherUsageOccurrence,anent,ent);
      RWStepRepr_RWSpecifiedHigherUsageOccurrence tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 384:
    {
      DeclareAndCast(StepRepr_AssemblyComponentUsageSubstitute,anent,ent);
      RWStepRepr_RWAssemblyComponentUsageSubstitute tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 385:
    {
      DeclareAndCast(StepRepr_SuppliedPartRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 386:
    {
      DeclareAndCast(StepRepr_ExternallyDefinedRepresentation,anent,ent);
      RWStepRepr_RWRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 387:
    {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationship,anent,ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 388:
    {
      DeclareAndCast(StepRepr_RepresentationRelationshipWithTransformation,anent,ent);
      RWStepRepr_RWRepresentationRelationshipWithTransformation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 389:
    {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation,anent,ent);
      RWStepRepr_RWShapeRepresentationRelationshipWithTransformation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 390:
    {
      DeclareAndCast(StepRepr_MaterialDesignation,anent,ent);
      RWStepRepr_RWMaterialDesignation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 391:
    {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anent,ent);
      RWStepShape_RWContextDependentShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    //:S4134: Added from CD to DIS
  case 392:
    {
      DeclareAndCast(StepAP214_AppliedDateAndTimeAssignment,anent,ent);
      RWStepAP214_RWAppliedDateAndTimeAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 393:
    {
      DeclareAndCast(StepAP214_AppliedDateAssignment,anent,ent);
      RWStepAP214_RWAppliedDateAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 394:
    {
      DeclareAndCast(StepAP214_AppliedApprovalAssignment,anent,ent);
      RWStepAP214_RWAppliedApprovalAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 395:
    {
      DeclareAndCast(StepAP214_AppliedGroupAssignment,anent,ent);
      RWStepAP214_RWAppliedGroupAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 396:
    {
      DeclareAndCast(StepAP214_AppliedOrganizationAssignment,anent,ent);
      RWStepAP214_RWAppliedOrganizationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 397:
    {
      DeclareAndCast(StepAP214_AppliedPersonAndOrganizationAssignment,anent,ent);
      RWStepAP214_RWAppliedPersonAndOrganizationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 398:
    {
      DeclareAndCast(StepAP214_AppliedPresentedItem,anent,ent);
      RWStepAP214_RWAppliedPresentedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 399:
    {
      DeclareAndCast(StepAP214_AppliedSecurityClassificationAssignment,anent,ent);
      RWStepAP214_RWAppliedSecurityClassificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 400:
    {
      DeclareAndCast(StepAP214_AppliedDocumentReference,anent,ent);
      RWStepAP214_RWAppliedDocumentReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 401:
    {
      DeclareAndCast(StepBasic_DocumentFile,anent,ent);
      RWStepBasic_RWDocumentFile tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 402:
    {
      DeclareAndCast(StepBasic_CharacterizedObject,anent,ent);
      RWStepBasic_RWCharacterizedObject tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 403:
    {
      DeclareAndCast(StepShape_ExtrudedFaceSolid,anent,ent);
      RWStepShape_RWExtrudedFaceSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    
  case 404:
    {
      DeclareAndCast(StepShape_RevolvedFaceSolid,anent,ent);
      RWStepShape_RWRevolvedFaceSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 405:
    {
      DeclareAndCast(StepShape_SweptFaceSolid,anent,ent);
      RWStepShape_RWSweptFaceSolid tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    
  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  case 406:
    {
      DeclareAndCast(StepRepr_MeasureRepresentationItem,anent,ent);
      RWStepRepr_RWMeasureRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 407:
    {
      DeclareAndCast(StepBasic_AreaUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 408:
    {
      DeclareAndCast(StepBasic_VolumeUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 409:
    {
      DeclareAndCast(StepBasic_SiUnitAndAreaUnit,anent,ent);
      RWStepBasic_RWSiUnitAndAreaUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 410:
    {
      DeclareAndCast(StepBasic_SiUnitAndVolumeUnit,anent,ent);
      RWStepBasic_RWSiUnitAndVolumeUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 411:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndAreaUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndAreaUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 412:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndVolumeUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndVolumeUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    // Added by ABV 10.11.99 for AP203
    case 413:
    {
      DeclareAndCast(StepBasic_Action,anent,ent);
      RWStepBasic_RWAction tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 414:
    {
      DeclareAndCast(StepBasic_ActionAssignment,anent,ent);
      RWStepBasic_RWActionAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 415:
    {
      DeclareAndCast(StepBasic_ActionMethod,anent,ent);
      RWStepBasic_RWActionMethod tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 416:
    {
      DeclareAndCast(StepBasic_ActionRequestAssignment,anent,ent);
      RWStepBasic_RWActionRequestAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 417:
    {
      DeclareAndCast(StepAP203_CcDesignApproval,anent,ent);
      RWStepAP203_RWCcDesignApproval tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 418:
    {
      DeclareAndCast(StepAP203_CcDesignCertification,anent,ent);
      RWStepAP203_RWCcDesignCertification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 419:
    {
      DeclareAndCast(StepAP203_CcDesignContract,anent,ent);
      RWStepAP203_RWCcDesignContract tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 420:
    {
      DeclareAndCast(StepAP203_CcDesignDateAndTimeAssignment,anent,ent);
      RWStepAP203_RWCcDesignDateAndTimeAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 421:
    {
      DeclareAndCast(StepAP203_CcDesignPersonAndOrganizationAssignment,anent,ent);
      RWStepAP203_RWCcDesignPersonAndOrganizationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 422:
    {
      DeclareAndCast(StepAP203_CcDesignSecurityClassification,anent,ent);
      RWStepAP203_RWCcDesignSecurityClassification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 423:
    {
      DeclareAndCast(StepAP203_CcDesignSpecificationReference,anent,ent);
      RWStepAP203_RWCcDesignSpecificationReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 424:
    {
      DeclareAndCast(StepBasic_Certification,anent,ent);
      RWStepBasic_RWCertification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 425:
    {
      DeclareAndCast(StepBasic_CertificationAssignment,anent,ent);
      RWStepBasic_RWCertificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 426:
    {
      DeclareAndCast(StepBasic_CertificationType,anent,ent);
      RWStepBasic_RWCertificationType tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 427:
    {
      DeclareAndCast(StepAP203_Change,anent,ent);
      RWStepAP203_RWChange tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 428:
    {
      DeclareAndCast(StepAP203_ChangeRequest,anent,ent);
      RWStepAP203_RWChangeRequest tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 429:
    {
      DeclareAndCast(StepRepr_ConfigurationDesign,anent,ent);
      RWStepRepr_RWConfigurationDesign tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 430:
    {
      DeclareAndCast(StepRepr_ConfigurationEffectivity,anent,ent);
      RWStepRepr_RWConfigurationEffectivity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 431:
    {
      DeclareAndCast(StepBasic_Contract,anent,ent);
      RWStepBasic_RWContract tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 432:
    {
      DeclareAndCast(StepBasic_ContractAssignment,anent,ent);
      RWStepBasic_RWContractAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 433:
    {
      DeclareAndCast(StepBasic_ContractType,anent,ent);
      RWStepBasic_RWContractType tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 434:
    {
      DeclareAndCast(StepRepr_ProductConcept,anent,ent);
      RWStepRepr_RWProductConcept tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 435:
    {
      DeclareAndCast(StepBasic_ProductConceptContext,anent,ent);
      RWStepBasic_RWProductConceptContext tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 436:
    {
      DeclareAndCast(StepAP203_StartRequest,anent,ent);
      RWStepAP203_RWStartRequest tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 437:
    {
      DeclareAndCast(StepAP203_StartWork,anent,ent);
      RWStepAP203_RWStartWork tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 438:
    {
      DeclareAndCast(StepBasic_VersionedActionRequest,anent,ent);
      RWStepBasic_RWVersionedActionRequest tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 439:
    {
      DeclareAndCast(StepBasic_ProductCategoryRelationship,anent,ent);
      RWStepBasic_RWProductCategoryRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 440:
    {
      DeclareAndCast(StepBasic_ActionRequestSolution,anent,ent);
      RWStepBasic_RWActionRequestSolution tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  case 441:
    {
      DeclareAndCast(StepVisual_DraughtingModel,anent,ent);
      RWStepVisual_RWDraughtingModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    
  // Added by ABV 18.04.00 for CAX-IF TRJ4
  case 442:
    {
      DeclareAndCast(StepShape_AngularLocation,anent,ent);
      RWStepShape_RWAngularLocation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 443:
    {
      DeclareAndCast(StepShape_AngularSize,anent,ent);
      RWStepShape_RWAngularSize tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 444:
    {
      DeclareAndCast(StepShape_DimensionalCharacteristicRepresentation,anent,ent);
      RWStepShape_RWDimensionalCharacteristicRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 445:
    {
      DeclareAndCast(StepShape_DimensionalLocation,anent,ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 446:
    {
      DeclareAndCast(StepShape_DimensionalLocationWithPath,anent,ent);
      RWStepShape_RWDimensionalLocationWithPath tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 447:
    {
      DeclareAndCast(StepShape_DimensionalSize,anent,ent);
      RWStepShape_RWDimensionalSize tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 448:
    {
      DeclareAndCast(StepShape_DimensionalSizeWithPath,anent,ent);
      RWStepShape_RWDimensionalSizeWithPath tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 449:
    {
      DeclareAndCast(StepShape_ShapeDimensionRepresentation,anent,ent);
      RWStepShape_RWShapeDimensionRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

    // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  case 450:
    {
      DeclareAndCast(StepBasic_DocumentRepresentationType,anent,ent);
      RWStepBasic_RWDocumentRepresentationType tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 451:
    {
      DeclareAndCast(StepBasic_ObjectRole,anent,ent);
      RWStepBasic_RWObjectRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 452:
    {
      DeclareAndCast(StepBasic_RoleAssociation,anent,ent);
      RWStepBasic_RWRoleAssociation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 453:
    {
      DeclareAndCast(StepBasic_IdentificationRole,anent,ent);
      RWStepBasic_RWIdentificationRole tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 454:
    {
      DeclareAndCast(StepBasic_IdentificationAssignment,anent,ent);
      RWStepBasic_RWIdentificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 455:
    {
      DeclareAndCast(StepBasic_ExternalIdentificationAssignment,anent,ent);
      RWStepBasic_RWExternalIdentificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 456:
    {
      DeclareAndCast(StepBasic_EffectivityAssignment,anent,ent);
      RWStepBasic_RWEffectivityAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 457:
    {
      DeclareAndCast(StepBasic_NameAssignment,anent,ent);
      RWStepBasic_RWNameAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 458:
    {
      DeclareAndCast(StepBasic_GeneralProperty,anent,ent);
      RWStepBasic_RWGeneralProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 459:
    {
      DeclareAndCast(StepAP214_Class,anent,ent);
      RWStepAP214_RWClass tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 460:
    {
      DeclareAndCast(StepAP214_ExternallyDefinedClass,anent,ent);
      RWStepAP214_RWExternallyDefinedClass tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 461:
    {
      DeclareAndCast(StepAP214_ExternallyDefinedGeneralProperty,anent,ent);
      RWStepAP214_RWExternallyDefinedGeneralProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 462:
    {
      DeclareAndCast(StepAP214_AppliedExternalIdentificationAssignment,anent,ent);
      RWStepAP214_RWAppliedExternalIdentificationAssignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 463:
    {
      DeclareAndCast(StepShape_DefinitionalRepresentationAndShapeRepresentation,anent,ent);
      RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

      // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
  case 470:
    {
      DeclareAndCast(StepRepr_CompositeShapeAspect,anent,ent);
      RWStepRepr_RWCompositeShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 471:
    {
      DeclareAndCast(StepRepr_DerivedShapeAspect,anent,ent);
      RWStepRepr_RWDerivedShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 472:
    {
      DeclareAndCast(StepRepr_Extension,anent,ent);
      RWStepRepr_RWExtension tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 473:  // same as DimensionalLocation
    {
      DeclareAndCast(StepShape_DirectedDimensionalLocation,anent,ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 474:
    {
      DeclareAndCast(StepShape_LimitsAndFits,anent,ent);
      RWStepShape_RWLimitsAndFits tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 475:
    {
      DeclareAndCast(StepShape_ToleranceValue,anent,ent);
      RWStepShape_RWToleranceValue tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 476:
    {
      DeclareAndCast(StepShape_MeasureQualification,anent,ent);
      RWStepShape_RWMeasureQualification tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 477:
    {
      DeclareAndCast(StepShape_PlusMinusTolerance,anent,ent);
      RWStepShape_RWPlusMinusTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 478:
    {
      DeclareAndCast(StepShape_PrecisionQualifier,anent,ent);
      RWStepShape_RWPrecisionQualifier tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 479:
    {
      DeclareAndCast(StepShape_TypeQualifier,anent,ent);
      RWStepShape_RWTypeQualifier tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 480:
    {
      DeclareAndCast(StepShape_QualifiedRepresentationItem,anent,ent);
      RWStepShape_RWQualifiedRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 481:
    {
      DeclareAndCast(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,anent,ent);
      RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 482: //the same types
  case 483:
    {
      DeclareAndCast(StepRepr_CompoundRepresentationItem,anent,ent);
      RWStepRepr_RWCompoundRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 484:  // same as ShapeAspectRelationship
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship,anent,ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

  // Added by ABV 27.12.01 for CAX-IF TRJ9
  case 485:
    {
      DeclareAndCast(StepShape_CompoundShapeRepresentation,anent,ent);
      RWStepShape_RWCompoundShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 486:
    {
      DeclareAndCast(StepShape_ConnectedEdgeSet,anent,ent);
      RWStepShape_RWConnectedEdgeSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 487:
    {
      DeclareAndCast(StepShape_ConnectedFaceShapeRepresentation,anent,ent);
      RWStepShape_RWConnectedFaceShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 488:
    {
      DeclareAndCast(StepShape_EdgeBasedWireframeModel,anent,ent);
      RWStepShape_RWEdgeBasedWireframeModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 489:
    {
      DeclareAndCast(StepShape_EdgeBasedWireframeShapeRepresentation,anent,ent);
      RWStepShape_RWEdgeBasedWireframeShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 490:
    {
      DeclareAndCast(StepShape_FaceBasedSurfaceModel,anent,ent);
      RWStepShape_RWFaceBasedSurfaceModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 491:
    {
      DeclareAndCast(StepShape_NonManifoldSurfaceShapeRepresentation,anent,ent);
      RWStepShape_RWNonManifoldSurfaceShapeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  
    
  case 492:
    {
      DeclareAndCast(StepGeom_OrientedSurface,anent,ent);
      RWStepGeom_RWOrientedSurface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 493:
    {
      DeclareAndCast(StepShape_Subface,anent,ent);
      RWStepShape_RWSubface tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 494:
    {
      DeclareAndCast(StepShape_Subedge,anent,ent);
      RWStepShape_RWSubedge tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 495:
    {
      DeclareAndCast(StepShape_SeamEdge,anent,ent);
      RWStepShape_RWSeamEdge tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 496:
    {
      DeclareAndCast(StepShape_ConnectedFaceSubSet,anent,ent);
      RWStepShape_RWConnectedFaceSubSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 500:  
    {
      DeclareAndCast(StepBasic_EulerAngles,anent,ent);
      RWStepBasic_RWEulerAngles tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 501:  
    {
      DeclareAndCast(StepBasic_MassUnit,anent,ent);
      RWStepBasic_RWMassUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 502:  
    {
      DeclareAndCast(StepBasic_ThermodynamicTemperatureUnit,anent,ent);
      RWStepBasic_RWThermodynamicTemperatureUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 503:  
    {
      DeclareAndCast(StepElement_AnalysisItemWithinRepresentation,anent,ent);
      RWStepElement_RWAnalysisItemWithinRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 504:  
    {
      DeclareAndCast(StepElement_Curve3dElementDescriptor,anent,ent);
      RWStepElement_RWCurve3dElementDescriptor tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 505:  
    {
      DeclareAndCast(StepElement_CurveElementEndReleasePacket,anent,ent);
      RWStepElement_RWCurveElementEndReleasePacket tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 506:  
    {
      DeclareAndCast(StepElement_CurveElementSectionDefinition,anent,ent);
      RWStepElement_RWCurveElementSectionDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 507:  
    {
      DeclareAndCast(StepElement_CurveElementSectionDerivedDefinitions,anent,ent);
      RWStepElement_RWCurveElementSectionDerivedDefinitions tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 508:  
    {
      DeclareAndCast(StepElement_ElementDescriptor,anent,ent);
      RWStepElement_RWElementDescriptor tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 509:  
    {
      DeclareAndCast(StepElement_ElementMaterial,anent,ent);
      RWStepElement_RWElementMaterial tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 510:  
    {
      DeclareAndCast(StepElement_Surface3dElementDescriptor,anent,ent);
      RWStepElement_RWSurface3dElementDescriptor tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 511:  
    {
      DeclareAndCast(StepElement_SurfaceElementProperty,anent,ent);
      RWStepElement_RWSurfaceElementProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 512:  
    {
      DeclareAndCast(StepElement_SurfaceSection,anent,ent);
      RWStepElement_RWSurfaceSection tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 513:  
    {
      DeclareAndCast(StepElement_SurfaceSectionField,anent,ent);
      RWStepElement_RWSurfaceSectionField tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 514:  
    {
      DeclareAndCast(StepElement_SurfaceSectionFieldConstant,anent,ent);
      RWStepElement_RWSurfaceSectionFieldConstant tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 515:  
    {
      DeclareAndCast(StepElement_SurfaceSectionFieldVarying,anent,ent);
      RWStepElement_RWSurfaceSectionFieldVarying tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 516:  
    {
      DeclareAndCast(StepElement_UniformSurfaceSection,anent,ent);
      RWStepElement_RWUniformSurfaceSection tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 517:  
    {
      DeclareAndCast(StepElement_Volume3dElementDescriptor,anent,ent);
      RWStepElement_RWVolume3dElementDescriptor tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 518:  
    {
      DeclareAndCast(StepFEA_AlignedCurve3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWAlignedCurve3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 519:  
    {
      DeclareAndCast(StepFEA_ArbitraryVolume3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 520:  
    {
      DeclareAndCast(StepFEA_Curve3dElementProperty,anent,ent);
      RWStepFEA_RWCurve3dElementProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 521:  
    {
      DeclareAndCast(StepFEA_Curve3dElementRepresentation,anent,ent);
      RWStepFEA_RWCurve3dElementRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 522:  
    {
      DeclareAndCast(StepFEA_Node,anent,ent);
      RWStepFEA_RWNode tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
//case 523:  
//    {
//      DeclareAndCast(StepFEA_CurveElementEndCoordinateSystem,anent,ent);
//      RWStepFEA_RWCurveElementEndCoordinateSystem tool;
//      tool.ReadStep (data,num,ach,anent);
//    }
//    break;
  case 524:  
    {
      DeclareAndCast(StepFEA_CurveElementEndOffset,anent,ent);
      RWStepFEA_RWCurveElementEndOffset tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 525:  
    {
      DeclareAndCast(StepFEA_CurveElementEndRelease,anent,ent);
      RWStepFEA_RWCurveElementEndRelease tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 526:  
    {
      DeclareAndCast(StepFEA_CurveElementInterval,anent,ent);
      RWStepFEA_RWCurveElementInterval tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 527:  
    {
      DeclareAndCast(StepFEA_CurveElementIntervalConstant,anent,ent);
      RWStepFEA_RWCurveElementIntervalConstant tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 528:  
    {
      DeclareAndCast(StepFEA_DummyNode,anent,ent);
      RWStepFEA_RWDummyNode tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 529:  
    {
      DeclareAndCast(StepFEA_CurveElementLocation,anent,ent);
      RWStepFEA_RWCurveElementLocation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 530:  
    {
      DeclareAndCast(StepFEA_ElementGeometricRelationship,anent,ent);
      RWStepFEA_RWElementGeometricRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 531:  
    {
      DeclareAndCast(StepFEA_ElementGroup,anent,ent);
      RWStepFEA_RWElementGroup tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 532:  
    {
      DeclareAndCast(StepFEA_ElementRepresentation,anent,ent);
      RWStepFEA_RWElementRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 533:  
    {
      DeclareAndCast(StepFEA_FeaAreaDensity,anent,ent);
      RWStepFEA_RWFeaAreaDensity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 534:  
    {
      DeclareAndCast(StepFEA_FeaAxis2Placement3d,anent,ent);
      RWStepFEA_RWFeaAxis2Placement3d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 535:  
    {
      DeclareAndCast(StepFEA_FeaGroup,anent,ent);
      RWStepFEA_RWFeaGroup tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 536:  
    {
      DeclareAndCast(StepFEA_FeaLinearElasticity,anent,ent);
      RWStepFEA_RWFeaLinearElasticity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 537:  
    {
      DeclareAndCast(StepFEA_FeaMassDensity,anent,ent);
      RWStepFEA_RWFeaMassDensity tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 538:  
    {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentation,anent,ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 539:  
    {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentationItem,anent,ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 540:  
    {
      DeclareAndCast(StepFEA_FeaModel,anent,ent);
      RWStepFEA_RWFeaModel tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 541:  
    {
      DeclareAndCast(StepFEA_FeaModel3d,anent,ent);
      RWStepFEA_RWFeaModel3d tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 542:  
    {
      DeclareAndCast(StepFEA_FeaMoistureAbsorption,anent,ent);
      RWStepFEA_RWFeaMoistureAbsorption tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 543:  
    {
      DeclareAndCast(StepFEA_FeaParametricPoint,anent,ent);
      RWStepFEA_RWFeaParametricPoint tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 544:  
    {
      DeclareAndCast(StepFEA_FeaRepresentationItem,anent,ent);
      RWStepFEA_RWFeaRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 545:  
    {
      DeclareAndCast(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion,anent,ent);
      RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 546:  
    {
      DeclareAndCast(StepFEA_FeaShellBendingStiffness,anent,ent);
      RWStepFEA_RWFeaShellBendingStiffness tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 547:  
    {
      DeclareAndCast(StepFEA_FeaShellMembraneBendingCouplingStiffness,anent,ent);
      RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 548:  
    {
      DeclareAndCast(StepFEA_FeaShellMembraneStiffness,anent,ent);
      RWStepFEA_RWFeaShellMembraneStiffness tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 549:  
    {
      DeclareAndCast(StepFEA_FeaShellShearStiffness,anent,ent);
      RWStepFEA_RWFeaShellShearStiffness tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 550:  
    {
      DeclareAndCast(StepFEA_GeometricNode,anent,ent);
      RWStepFEA_RWGeometricNode tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 551:  
    {
      DeclareAndCast(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion,anent,ent);
      RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 552:  
    {
      DeclareAndCast(StepFEA_NodeGroup,anent,ent);
      RWStepFEA_RWNodeGroup tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 553:  
    {
      DeclareAndCast(StepFEA_NodeRepresentation,anent,ent);
      RWStepFEA_RWNodeRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 554:  
    {
      DeclareAndCast(StepFEA_NodeSet,anent,ent);
      RWStepFEA_RWNodeSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 555:  
    {
      DeclareAndCast(StepFEA_NodeWithSolutionCoordinateSystem,anent,ent);
      RWStepFEA_RWNodeWithSolutionCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 556:  
    {
      DeclareAndCast(StepFEA_NodeWithVector,anent,ent);
      RWStepFEA_RWNodeWithVector tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 557:  
    {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateDirection,anent,ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateDirection tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 558:  
    {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 559:  
    {
      DeclareAndCast(StepFEA_ParametricSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWParametricSurface3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 560:  
    {
      DeclareAndCast(StepFEA_Surface3dElementRepresentation,anent,ent);
      RWStepFEA_RWSurface3dElementRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
//case 561:  
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor22d,anent,ent);
//      RWStepFEA_RWSymmetricTensor22d tool;
//      tool.ReadStep (data,num,ach,anent);
//    }
//    break;
//case 562:  
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor42d,anent,ent);
//      RWStepFEA_RWSymmetricTensor42d tool;
//      tool.ReadStep (data,num,ach,anent);
//    }
//    break;
//case 563:  
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor43d,anent,ent);
//      RWStepFEA_RWSymmetricTensor43d tool;
//      tool.ReadStep (data,num,ach,anent);
//    }
//    break;
  case 564:  
    {
      DeclareAndCast(StepFEA_Volume3dElementRepresentation,anent,ent);
      RWStepFEA_RWVolume3dElementRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 565:  
    {
      DeclareAndCast(StepRepr_DataEnvironment,anent,ent);
      RWStepRepr_RWDataEnvironment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 566:  
    {
      DeclareAndCast(StepRepr_MaterialPropertyRepresentation,anent,ent);
      RWStepRepr_RWMaterialPropertyRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 567:  
    {
      DeclareAndCast(StepRepr_PropertyDefinitionRelationship,anent,ent);
      RWStepRepr_RWPropertyDefinitionRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 568:  
    {
      DeclareAndCast(StepShape_PointRepresentation,anent,ent);
      RWStepShape_RWPointRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 569:  
    {
      DeclareAndCast(StepRepr_MaterialProperty,anent,ent);
      RWStepRepr_RWMaterialProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 570:  
    {
      DeclareAndCast(StepFEA_FeaModelDefinition,anent,ent);
      RWStepFEA_RWFeaModelDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 571:  
    {
      DeclareAndCast(StepFEA_FreedomAndCoefficient,anent,ent);
      RWStepFEA_RWFreedomAndCoefficient tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 572:  
    {
      DeclareAndCast(StepFEA_FreedomsList,anent,ent);
      RWStepFEA_RWFreedomsList tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 573:  
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormationRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionFormationRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 574 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndMassUnit, anent, ent);
      RWStepBasic_RWSiUnitAndMassUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    
    break;
  case 575:  
    {
      DeclareAndCast(StepFEA_NodeDefinition,anent,ent);
      RWStepFEA_RWNodeDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 576:  
    {
      DeclareAndCast(StepRepr_StructuralResponseProperty,anent,ent);
      RWStepRepr_RWStructuralResponseProperty tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 577:  
    {
      DeclareAndCast(StepRepr_StructuralResponsePropertyDefinitionRepresentation,anent,ent);
      RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 578 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
  
    break;
  case 579:  
    {
      DeclareAndCast(StepFEA_AlignedSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWAlignedSurface3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 580:  
    {
      DeclareAndCast(StepFEA_ConstantSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWConstantSurface3dElementCoordinateSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 581:  
    {
      DeclareAndCast(StepFEA_CurveElementIntervalLinearlyVarying,anent,ent);
      RWStepFEA_RWCurveElementIntervalLinearlyVarying tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 582: 
    {
      DeclareAndCast(StepFEA_FeaCurveSectionGeometricRelationship,anent,ent);
      RWStepFEA_RWFeaCurveSectionGeometricRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 583:  
    {
      DeclareAndCast(StepFEA_FeaSurfaceSectionGeometricRelationship,anent,ent);
      RWStepFEA_RWFeaSurfaceSectionGeometricRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
   case 600:
    {
      DeclareAndCast(StepBasic_DocumentProductAssociation,anent,ent);
      RWStepBasic_RWDocumentProductAssociation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 601:
    {
      DeclareAndCast(StepBasic_DocumentProductEquivalence,anent,ent);
      RWStepBasic_RWDocumentProductEquivalence tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break; 
  case 609:
    {
      DeclareAndCast(StepDimTol_CylindricityTolerance,anent,ent);
      RWStepDimTol_RWCylindricityTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 610:  
    {
      DeclareAndCast(StepShape_ShapeRepresentationWithParameters,anent,ent);
      RWStepShape_RWShapeRepresentationWithParameters tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 611:  
    {
      DeclareAndCast(StepDimTol_AngularityTolerance,anent,ent);
      RWStepDimTol_RWAngularityTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 612:  
    {
      DeclareAndCast(StepDimTol_ConcentricityTolerance,anent,ent);
      RWStepDimTol_RWConcentricityTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 613:  
    {
      DeclareAndCast(StepDimTol_CircularRunoutTolerance,anent,ent);
      RWStepDimTol_RWCircularRunoutTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 614:  
    {
      DeclareAndCast(StepDimTol_CoaxialityTolerance,anent,ent);
      RWStepDimTol_RWCoaxialityTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 615:  
    {
      DeclareAndCast(StepDimTol_FlatnessTolerance,anent,ent);
      RWStepDimTol_RWFlatnessTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 616:  
    {
      DeclareAndCast(StepDimTol_LineProfileTolerance,anent,ent);
      RWStepDimTol_RWLineProfileTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 617:  
    {
      DeclareAndCast(StepDimTol_ParallelismTolerance,anent,ent);
      RWStepDimTol_RWParallelismTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 618:  
    {
      DeclareAndCast(StepDimTol_PerpendicularityTolerance,anent,ent);
      RWStepDimTol_RWPerpendicularityTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 619:  
    {
      DeclareAndCast(StepDimTol_PositionTolerance,anent,ent);
      RWStepDimTol_RWPositionTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 620:  
    {
      DeclareAndCast(StepDimTol_RoundnessTolerance,anent,ent);
      RWStepDimTol_RWRoundnessTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 621:  
    {
      DeclareAndCast(StepDimTol_StraightnessTolerance,anent,ent);
      RWStepDimTol_RWStraightnessTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 622:  
    {
      DeclareAndCast(StepDimTol_SurfaceProfileTolerance,anent,ent);
      RWStepDimTol_RWSurfaceProfileTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 623:  
    {
      DeclareAndCast(StepDimTol_SymmetryTolerance,anent,ent);
      RWStepDimTol_RWSymmetryTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 624:  
    {
      DeclareAndCast(StepDimTol_TotalRunoutTolerance,anent,ent);
      RWStepDimTol_RWTotalRunoutTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 625:  
    {
      DeclareAndCast(StepDimTol_GeometricTolerance,anent,ent);
      RWStepDimTol_RWGeometricTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 626:  
    {
      DeclareAndCast(StepDimTol_GeometricToleranceRelationship,anent,ent);
      RWStepDimTol_RWGeometricToleranceRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 627:  
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDatumReference,anent,ent);
      RWStepDimTol_RWGeometricToleranceWithDatumReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 628:  
    {
      DeclareAndCast(StepDimTol_ModifiedGeometricTolerance,anent,ent);
      RWStepDimTol_RWModifiedGeometricTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 629:  
    {
      DeclareAndCast(StepDimTol_Datum,anent,ent);
      RWStepDimTol_RWDatum tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 630:  
    {
      DeclareAndCast(StepDimTol_DatumFeature,anent,ent);
      RWStepDimTol_RWDatumFeature tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 631:  
    {
      DeclareAndCast(StepDimTol_DatumReference,anent,ent);
      RWStepDimTol_RWDatumReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 632:  
    {
      DeclareAndCast(StepDimTol_CommonDatum,anent,ent);
      RWStepDimTol_RWCommonDatum tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 633:  
    {
      DeclareAndCast(StepDimTol_DatumTarget,anent,ent);
      RWStepDimTol_RWDatumTarget tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 634:  
    {
      DeclareAndCast(StepDimTol_PlacedDatumTargetFeature,anent,ent);
      RWStepDimTol_RWPlacedDatumTargetFeature tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 635:  
    {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnit,anent,ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 636:  
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 650 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndMassUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndMassUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 651 : 
    {
      DeclareAndCast(StepBasic_MassMeasureWithUnit, anent, ent);
      RWStepBasic_RWMassMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 660 :
    {
      DeclareAndCast(StepRepr_Apex, anent, ent);
      RWStepRepr_RWApex tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 661 :
    {
      DeclareAndCast(StepRepr_CentreOfSymmetry, anent, ent);
      RWStepRepr_RWCentreOfSymmetry tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 662 :
    {
      DeclareAndCast(StepRepr_GeometricAlignment, anent, ent);
      RWStepRepr_RWGeometricAlignment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 663 :
    {
      DeclareAndCast(StepRepr_PerpendicularTo, anent, ent);
      RWStepRepr_RWPerpendicularTo tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 664 :
    {
      DeclareAndCast(StepRepr_Tangent, anent, ent);
      RWStepRepr_RWTangent tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 665 :
    {
      DeclareAndCast(StepRepr_ParallelOffset, anent, ent);
      RWStepRepr_RWParallelOffset tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 666 :
    {
      DeclareAndCast(StepAP242_GeometricItemSpecificUsage, anent, ent);
      RWStepAP242_RWGeometricItemSpecificUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 667 :
    {
      DeclareAndCast(StepAP242_IdAttribute, anent, ent);
      RWStepAP242_RWIdAttribute tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 668 :
    {
      DeclareAndCast(StepAP242_ItemIdentifiedRepresentationUsage, anent, ent);
      RWStepAP242_RWItemIdentifiedRepresentationUsage tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 669 :
    {
      DeclareAndCast(StepRepr_AllAroundShapeAspect, anent, ent);
      RWStepRepr_RWAllAroundShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 670 :
    {
      DeclareAndCast(StepRepr_BetweenShapeAspect, anent, ent);
      RWStepRepr_RWBetweenShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 671 :
    {
      DeclareAndCast(StepRepr_CompositeGroupShapeAspect, anent, ent);
      RWStepRepr_RWCompositeGroupShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 672 :
    {
      DeclareAndCast(StepRepr_ContinuosShapeAspect, anent, ent);
      RWStepRepr_RWContinuosShapeAspect tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 673 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedAreaUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 674 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 675 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithMaximumTolerance, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithMaximumTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 676 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithModifiers, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithModifiers tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 677 :
    {
      DeclareAndCast(StepDimTol_UnequallyDisposedGeometricTolerance, anent, ent);
      RWStepDimTol_RWUnequallyDisposedGeometricTolerance tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 678 :
    {
      DeclareAndCast(StepDimTol_NonUniformZoneDefinition, anent, ent);
      RWStepDimTol_RWNonUniformZoneDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 679 :
    {
      DeclareAndCast(StepDimTol_ProjectedZoneDefinition, anent, ent);
      RWStepDimTol_RWProjectedZoneDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 680 :
    {
      DeclareAndCast(StepDimTol_RunoutZoneDefinition, anent, ent);
      RWStepDimTol_RWRunoutZoneDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 681 :
    {
      DeclareAndCast(StepDimTol_RunoutZoneOrientation, anent, ent);
      RWStepDimTol_RWRunoutZoneOrientation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 682 :
    {
      DeclareAndCast(StepDimTol_ToleranceZone, anent, ent);
      RWStepDimTol_RWToleranceZone tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 683 :
    {
      DeclareAndCast(StepDimTol_ToleranceZoneDefinition, anent, ent);
      RWStepDimTol_RWToleranceZoneDefinition tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 684 :
    {
      DeclareAndCast(StepDimTol_ToleranceZoneForm, anent, ent);
      RWStepDimTol_RWToleranceZoneForm tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 685 :
    {
      DeclareAndCast(StepShape_ValueFormatTypeQualifier, anent, ent);
      RWStepShape_RWValueFormatTypeQualifier tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 686 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceCompartment, anent, ent);
      RWStepDimTol_RWDatumReferenceCompartment tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 687 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceElement, anent, ent);
      RWStepDimTol_RWDatumReferenceElement tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 688 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceModifierWithValue, anent, ent);
      RWStepDimTol_RWDatumReferenceModifierWithValue tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 689 :
    {
      DeclareAndCast(StepDimTol_DatumSystem, anent, ent);
      RWStepDimTol_RWDatumSystem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 690 :
    {
      DeclareAndCast(StepDimTol_GeneralDatumReference, anent, ent);
      RWStepDimTol_RWGeneralDatumReference tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 691:
    {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit,anent,ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 692:
    {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI,anent,ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 693:
    {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI,anent,ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 694:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRef,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRef tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 695:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 696:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMod tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 697:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 698:
    {
      DeclareAndCast(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 699:
    {
      DeclareAndCast(StepRepr_CompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompShAspAndDatumFeatAndShAsp tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 700:
    {
      DeclareAndCast(StepRepr_IntegerRepresentationItem,anent,ent);
      RWStepRepr_RWIntegerRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 701:
    {
      DeclareAndCast(StepRepr_ValueRepresentationItem,anent,ent);
      RWStepRepr_RWValueRepresentationItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 702:
    {
      DeclareAndCast(StepRepr_FeatureForDatumTargetRelationship,anent,ent);
      RWStepRepr_RWFeatureForDatumTargetRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 703:
    {
      DeclareAndCast(StepAP242_DraughtingModelItemAssociation,anent,ent);
      RWStepAP242_RWDraughtingModelItemAssociation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 704:
    {
      DeclareAndCast(StepVisual_AnnotationPlane,anent,ent);
      RWStepVisual_RWAnnotationPlane tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 705:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
  case 706:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
    case 707:
    {
      DeclareAndCast(StepVisual_TessellatedAnnotationOccurrence,anent,ent);
      RWStepVisual_RWTessellatedAnnotationOccurrence tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

     case 708:
    {
      DeclareAndCast(StepVisual_TessellatedItem,anent,ent);
      RWStepVisual_RWTessellatedItem tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;

     case 709:
    {
      DeclareAndCast(StepVisual_TessellatedGeometricSet,anent,ent);
      RWStepVisual_RWTessellatedGeometricSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
     case 710:
    {
      DeclareAndCast(StepVisual_TessellatedCurveSet,anent,ent);
      RWStepVisual_RWTessellatedCurveSet tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
     case 711:
    {
      DeclareAndCast(StepVisual_CoordinatesList,anent,ent);
      RWStepVisual_RWCoordinatesList tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
     case 712:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentation,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentation tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
     case 713:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentationRelationship,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentationRelationship tool;
      tool.ReadStep (data,num,ach,anent);
    }
    break;
     case 714:
    {
      DeclareAndCast(StepRepr_CharacterizedRepresentation, anent, ent);
      RWStepRepr_RWCharacterizedRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
     case 715:
    {
      DeclareAndCast(StepVisual_CharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation, anent, ent);
      RWStepVisual_RWCharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation tool;
      tool.ReadStep(data, num, ach, anent);
    }
    break;
     case 716:
   {
     DeclareAndCast(StepVisual_CameraModelD3MultiClipping, anent, ent);
     RWStepVisual_RWCameraModelD3MultiClipping tool;
     tool.ReadStep(data, num, ach, anent);
   }
   break;
     case 717:
   {
     DeclareAndCast(StepVisual_CameraModelD3MultiClippingIntersection, anent, ent);
     RWStepVisual_RWCameraModelD3MultiClippingIntersection tool;
     tool.ReadStep(data, num, ach, anent);
   }
   break;
     case 718:
   {
     DeclareAndCast(StepVisual_CameraModelD3MultiClippingUnion, anent, ent);
     RWStepVisual_RWCameraModelD3MultiClippingUnion tool;
     tool.ReadStep(data, num, ach, anent);
   }
   break;
     case 719:
   {
     DeclareAndCast(StepVisual_AnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem, anent, ent);
     RWStepVisual_RWAnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem tool;
     tool.ReadStep(data, num, ach, anent);
   }
   break;
   

  default: 
    ach->AddFail("Type Mismatch when reading - Entity");
  }
  return;
}


//=======================================================================
//function : WriteStep
//purpose  : Writing of a file
//=======================================================================

void RWStepAP214_ReadWriteModule::WriteStep(const Standard_Integer CN,
                                            StepData_StepWriter& SW,
                                            const Handle(Standard_Transient)&ent) const
{
  if (CN == 0) return;
  switch (CN) {
  case 1 : 
    {
      DeclareAndCast(StepBasic_Address, anent, ent);
      RWStepBasic_RWAddress tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 2 : 
    {
      DeclareAndCast(StepShape_AdvancedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWAdvancedBrepShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 3 : 
    {
      DeclareAndCast(StepShape_AdvancedFace, anent, ent);
      RWStepShape_RWAdvancedFace tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 4 :
    {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrence, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrence tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 5:
  {
    DeclareAndCast(StepVisual_AnnotationFillArea, anent, ent);
    RWStepVisual_RWAnnotationFillArea tool;
    tool.WriteStep(SW, anent);
  }
    break;
  case 6:
  {
    DeclareAndCast(StepVisual_AnnotationFillAreaOccurrence, anent, ent);
    RWStepVisual_RWAnnotationFillAreaOccurrence tool;
    tool.WriteStep(SW, anent);
  }
    break;
  case 7 : 
    {
      DeclareAndCast(StepVisual_AnnotationOccurrence, anent, ent);
      RWStepVisual_RWAnnotationOccurrence tool;
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 11 : 
    {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 12 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 13 : 
    {
      DeclareAndCast(StepBasic_ApplicationContext, anent, ent);
      RWStepBasic_RWApplicationContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 14 : 
    {
      DeclareAndCast(StepBasic_ApplicationContextElement, anent, ent);
      RWStepBasic_RWApplicationContextElement tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 15 : 
    {
      DeclareAndCast(StepBasic_ApplicationProtocolDefinition, anent, ent);
      RWStepBasic_RWApplicationProtocolDefinition tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 16 : 
    {
      DeclareAndCast(StepBasic_Approval, anent, ent);
      RWStepBasic_RWApproval tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 18 : 
    {
      DeclareAndCast(StepBasic_ApprovalPersonOrganization, anent, ent);
      RWStepBasic_RWApprovalPersonOrganization tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 19 : 
    {
      DeclareAndCast(StepBasic_ApprovalRelationship, anent, ent);
      RWStepBasic_RWApprovalRelationship tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 20 : 
    {
      DeclareAndCast(StepBasic_ApprovalRole, anent, ent);
      RWStepBasic_RWApprovalRole tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 21 : 
    {
      DeclareAndCast(StepBasic_ApprovalStatus, anent, ent);
      RWStepBasic_RWApprovalStatus tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 22 : 
    {
      DeclareAndCast(StepVisual_AreaInSet, anent, ent);
      RWStepVisual_RWAreaInSet tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 23 : 
    {
      DeclareAndCast(StepAP214_AutoDesignActualDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAndTimeAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 24 : 
    {
      DeclareAndCast(StepAP214_AutoDesignActualDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignActualDateAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 25 : 
    {
      DeclareAndCast(StepAP214_AutoDesignApprovalAssignment, anent, ent);
      RWStepAP214_RWAutoDesignApprovalAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 26 : 
    {
      DeclareAndCast(StepAP214_AutoDesignDateAndPersonAssignment, anent, ent);
      RWStepAP214_RWAutoDesignDateAndPersonAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 27 : 
    {
      DeclareAndCast(StepAP214_AutoDesignGroupAssignment, anent, ent);
      RWStepAP214_RWAutoDesignGroupAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 28 : 
    {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAndTimeAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 29 : 
    {
      DeclareAndCast(StepAP214_AutoDesignNominalDateAssignment, anent, ent);
      RWStepAP214_RWAutoDesignNominalDateAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 30 : 
    {
      DeclareAndCast(StepAP214_AutoDesignOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignOrganizationAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 31 : 
    {
      DeclareAndCast(StepAP214_AutoDesignPersonAndOrganizationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 32 : 
    {
      DeclareAndCast(StepAP214_AutoDesignPresentedItem, anent, ent);
      RWStepAP214_RWAutoDesignPresentedItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 33 : 
    {
      DeclareAndCast(StepAP214_AutoDesignSecurityClassificationAssignment, anent, ent);
      RWStepAP214_RWAutoDesignSecurityClassificationAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 35 : 
    {
      DeclareAndCast(StepGeom_Axis1Placement, anent, ent);
      RWStepGeom_RWAxis1Placement tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 36 : 
    {
      DeclareAndCast(StepGeom_Axis2Placement2d, anent, ent);
      RWStepGeom_RWAxis2Placement2d tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 37 : 
    {
      DeclareAndCast(StepGeom_Axis2Placement3d, anent, ent);
      RWStepGeom_RWAxis2Placement3d tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 38 : 
    {
      DeclareAndCast(StepGeom_BSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 39 : 
    {
      DeclareAndCast(StepGeom_BSplineCurveWithKnots, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnots tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 40 : 
    {
      DeclareAndCast(StepGeom_BSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 41 : 
    {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnots, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnots tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 42 : 
    {
      DeclareAndCast(StepVisual_BackgroundColour, anent, ent);
      RWStepVisual_RWBackgroundColour tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 43 : 
    {
      DeclareAndCast(StepGeom_BezierCurve, anent, ent);
      RWStepGeom_RWBezierCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 44 : 
    {
      DeclareAndCast(StepGeom_BezierSurface, anent, ent);
      RWStepGeom_RWBezierSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 45 : 
    {
      DeclareAndCast(StepShape_Block, anent, ent);
      RWStepShape_RWBlock tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 46 : 
    {
      DeclareAndCast(StepShape_BooleanResult, anent, ent);
      RWStepShape_RWBooleanResult tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 47 : 
    {
      DeclareAndCast(StepGeom_BoundaryCurve, anent, ent);
      RWStepGeom_RWBoundaryCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 48 : 
    {
      DeclareAndCast(StepGeom_BoundedCurve, anent, ent);
      RWStepGeom_RWBoundedCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 49 : 
    {
      DeclareAndCast(StepGeom_BoundedSurface, anent, ent);
      RWStepGeom_RWBoundedSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 50 : 
    {
      DeclareAndCast(StepShape_BoxDomain, anent, ent);
      RWStepShape_RWBoxDomain tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 51 : 
    {
      DeclareAndCast(StepShape_BoxedHalfSpace, anent, ent);
      RWStepShape_RWBoxedHalfSpace tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 52 : 
    {
      DeclareAndCast(StepShape_BrepWithVoids, anent, ent);
      RWStepShape_RWBrepWithVoids tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 53 : 
    {
      DeclareAndCast(StepBasic_CalendarDate, anent, ent);
      RWStepBasic_RWCalendarDate tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 54 : 
    {
      DeclareAndCast(StepVisual_CameraImage, anent, ent);
      RWStepVisual_RWCameraImage tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 55 : 
    {
      DeclareAndCast(StepVisual_CameraModel, anent, ent);
      RWStepVisual_RWCameraModel tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 56 : 
    {
      DeclareAndCast(StepVisual_CameraModelD2, anent, ent);
      RWStepVisual_RWCameraModelD2 tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 57 : 
    {
      DeclareAndCast(StepVisual_CameraModelD3, anent, ent);
      RWStepVisual_RWCameraModelD3 tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 58 : 
    {
      DeclareAndCast(StepVisual_CameraUsage, anent, ent);
      RWStepVisual_RWCameraUsage tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 59 : 
    {
      DeclareAndCast(StepGeom_CartesianPoint, anent, ent);
      RWStepGeom_RWCartesianPoint tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 60 : 
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 61 : 
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator3d, anent, ent);
      RWStepGeom_RWCartesianTransformationOperator3d tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 62 : 
    {
      DeclareAndCast(StepGeom_Circle, anent, ent);
      RWStepGeom_RWCircle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 63 : 
    {
      DeclareAndCast(StepShape_ClosedShell, anent, ent);
      RWStepShape_RWClosedShell tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 64 : 
    {
      DeclareAndCast(StepVisual_Colour, anent, ent);
      RWStepVisual_RWColour tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 65 : 
    {
      DeclareAndCast(StepVisual_ColourRgb, anent, ent);
      RWStepVisual_RWColourRgb tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 66 : 
    {
      DeclareAndCast(StepVisual_ColourSpecification, anent, ent);
      RWStepVisual_RWColourSpecification tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 67 : 
    {
      DeclareAndCast(StepGeom_CompositeCurve, anent, ent);
      RWStepGeom_RWCompositeCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 68 : 
    {
      DeclareAndCast(StepGeom_CompositeCurveOnSurface, anent, ent);
      RWStepGeom_RWCompositeCurveOnSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 69 : 
    {
      DeclareAndCast(StepGeom_CompositeCurveSegment, anent, ent);
      RWStepGeom_RWCompositeCurveSegment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 70 : 
    {
      DeclareAndCast(StepVisual_CompositeText, anent, ent);
      RWStepVisual_RWCompositeText tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 73 : 
    {
      DeclareAndCast(StepVisual_CompositeTextWithExtent, anent, ent);
      RWStepVisual_RWCompositeTextWithExtent tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 74 : 
    {
      DeclareAndCast(StepGeom_Conic, anent, ent);
      RWStepGeom_RWConic tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 75 : 
    {
      DeclareAndCast(StepGeom_ConicalSurface, anent, ent);
      RWStepGeom_RWConicalSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 76 : 
    {
      DeclareAndCast(StepShape_ConnectedFaceSet, anent, ent);
      RWStepShape_RWConnectedFaceSet tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 77 : 
    {
      DeclareAndCast(StepVisual_ContextDependentInvisibility, anent, ent);
      RWStepVisual_RWContextDependentInvisibility tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 78 : 
    {
      DeclareAndCast(StepVisual_ContextDependentOverRidingStyledItem, anent, ent);
      RWStepVisual_RWContextDependentOverRidingStyledItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 79 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 80 : 
    {
      DeclareAndCast(StepBasic_CoordinatedUniversalTimeOffset, anent, ent);
      RWStepBasic_RWCoordinatedUniversalTimeOffset tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 82 : 
    {
      DeclareAndCast(StepShape_CsgShapeRepresentation, anent, ent);
      RWStepShape_RWCsgShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 83 : 
    {
      DeclareAndCast(StepShape_CsgSolid, anent, ent);
      RWStepShape_RWCsgSolid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 84 : 
    {
      DeclareAndCast(StepGeom_Curve, anent, ent);
      RWStepGeom_RWCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 85 : 
    {
      DeclareAndCast(StepGeom_CurveBoundedSurface, anent, ent);
      RWStepGeom_RWCurveBoundedSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 86 : 
    {
      DeclareAndCast(StepGeom_CurveReplica, anent, ent);
      RWStepGeom_RWCurveReplica tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 87 : 
    {
      DeclareAndCast(StepVisual_CurveStyle, anent, ent);
      RWStepVisual_RWCurveStyle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 88 : 
    {
      DeclareAndCast(StepVisual_CurveStyleFont, anent, ent);
      RWStepVisual_RWCurveStyleFont tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 89 : 
    {
      DeclareAndCast(StepVisual_CurveStyleFontPattern, anent, ent);
      RWStepVisual_RWCurveStyleFontPattern tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 90 : 
    {
      DeclareAndCast(StepGeom_CylindricalSurface, anent, ent);
      RWStepGeom_RWCylindricalSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 91 : 
    {
      DeclareAndCast(StepBasic_Date, anent, ent);
      RWStepBasic_RWDate tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 92 : 
    {
      DeclareAndCast(StepBasic_DateAndTime, anent, ent);
      RWStepBasic_RWDateAndTime tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 95 : 
    {
      DeclareAndCast(StepBasic_DateRole, anent, ent);
      RWStepBasic_RWDateRole tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 96 : 
    {
      DeclareAndCast(StepBasic_DateTimeRole, anent, ent);
      RWStepBasic_RWDateTimeRole tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 98 : 
    {
      DeclareAndCast(StepRepr_DefinitionalRepresentation, anent, ent);
      RWStepRepr_RWDefinitionalRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 99 : 
    {
      DeclareAndCast(StepGeom_DegeneratePcurve, anent, ent);
      RWStepGeom_RWDegeneratePcurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 100 : 
    {
      DeclareAndCast(StepGeom_DegenerateToroidalSurface, anent, ent);
      RWStepGeom_RWDegenerateToroidalSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 101 : 
    {
      DeclareAndCast(StepRepr_DescriptiveRepresentationItem, anent, ent);
      RWStepRepr_RWDescriptiveRepresentationItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 104 : 
    {
      DeclareAndCast(StepBasic_DimensionalExponents, anent, ent);
      RWStepBasic_RWDimensionalExponents tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 105 : 
    {
      DeclareAndCast(StepGeom_Direction, anent, ent);
      RWStepGeom_RWDirection tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 106 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 107 :
    {
      DeclareAndCast(StepVisual_DraughtingCallout, anent, ent);
      RWStepVisual_RWDraughtingCallout tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 108 : 
    {
      DeclareAndCast(StepVisual_DraughtingPreDefinedColour, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedColour tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 109 : 
    {
      DeclareAndCast(StepVisual_DraughtingPreDefinedCurveFont, anent, ent);
      RWStepVisual_RWDraughtingPreDefinedCurveFont tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 115 : 
    {
      DeclareAndCast(StepShape_Edge, anent, ent);
      RWStepShape_RWEdge tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 116 : 
    {
      DeclareAndCast(StepShape_EdgeCurve, anent, ent);
      RWStepShape_RWEdgeCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 117 : 
    {
      DeclareAndCast(StepShape_EdgeLoop, anent, ent);
      RWStepShape_RWEdgeLoop tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 118 : 
    {
      DeclareAndCast(StepGeom_ElementarySurface, anent, ent);
      RWStepGeom_RWElementarySurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 119 : 
    {
      DeclareAndCast(StepGeom_Ellipse, anent, ent);
      RWStepGeom_RWEllipse tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 120 : 
    {
      DeclareAndCast(StepGeom_EvaluatedDegeneratePcurve, anent, ent);
      RWStepGeom_RWEvaluatedDegeneratePcurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 121 : 
    {
      DeclareAndCast(StepBasic_ExternalSource, anent, ent);
      RWStepBasic_RWExternalSource tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 122 : 
    {
      DeclareAndCast(StepVisual_ExternallyDefinedCurveFont, anent, ent);
      RWStepVisual_RWExternallyDefinedCurveFont tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 124 : 
  case 126 : 
    {
      DeclareAndCast(StepBasic_ExternallyDefinedItem, anent, ent);
      RWStepBasic_RWExternallyDefinedItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 128 : 
    {
      DeclareAndCast(StepShape_ExtrudedAreaSolid, anent, ent);
      RWStepShape_RWExtrudedAreaSolid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 129 : 
    {
      DeclareAndCast(StepShape_Face, anent, ent);
      RWStepShape_RWFace tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 131 : 
    {
      DeclareAndCast(StepShape_FaceBound, anent, ent);
      RWStepShape_RWFaceBound tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 132 : 
    {
      DeclareAndCast(StepShape_FaceOuterBound, anent, ent);
      RWStepShape_RWFaceOuterBound tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 133 : 
    {
      DeclareAndCast(StepShape_FaceSurface, anent, ent);
      RWStepShape_RWFaceSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 134 : 
    {
      DeclareAndCast(StepShape_FacetedBrep, anent, ent);
      RWStepShape_RWFacetedBrep tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 135 : 
    {
      DeclareAndCast(StepShape_FacetedBrepShapeRepresentation, anent, ent);
      RWStepShape_RWFacetedBrepShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 136 : 
    {
      DeclareAndCast(StepVisual_FillAreaStyle, anent, ent);
      RWStepVisual_RWFillAreaStyle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 137 : 
    {
      DeclareAndCast(StepVisual_FillAreaStyleColour, anent, ent);
      RWStepVisual_RWFillAreaStyleColour tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 141 : 
    {
      DeclareAndCast(StepRepr_FunctionallyDefinedTransformation, anent, ent);
      RWStepRepr_RWFunctionallyDefinedTransformation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 142 : 
    {
      DeclareAndCast(StepShape_GeometricCurveSet, anent, ent);
      RWStepShape_RWGeometricCurveSet tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 143 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 144 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationItem, anent, ent);
      RWStepGeom_RWGeometricRepresentationItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 145 : 
    {
      DeclareAndCast(StepShape_GeometricSet, anent, ent);
      RWStepShape_RWGeometricSet tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 146 : 
    {
      DeclareAndCast(StepShape_GeometricallyBoundedSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 147 : 
    {
      DeclareAndCast(StepShape_GeometricallyBoundedWireframeShapeRepresentation, anent, ent);
      RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 148 : 
    {
      DeclareAndCast(StepRepr_GlobalUncertaintyAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUncertaintyAssignedContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 149 : 
    {
      DeclareAndCast(StepRepr_GlobalUnitAssignedContext, anent, ent);
      RWStepRepr_RWGlobalUnitAssignedContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 150 : 
    {
      DeclareAndCast(StepBasic_Group, anent, ent);
      RWStepBasic_RWGroup tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 152 : 
    {
      DeclareAndCast(StepBasic_GroupRelationship, anent, ent);
      RWStepBasic_RWGroupRelationship tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 153 : 
    {
      DeclareAndCast(StepShape_HalfSpaceSolid, anent, ent);
      RWStepShape_RWHalfSpaceSolid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 154 : 
    {
      DeclareAndCast(StepGeom_Hyperbola, anent, ent);
      RWStepGeom_RWHyperbola tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 155 : 
    {
      DeclareAndCast(StepGeom_IntersectionCurve, anent, ent);
      RWStepGeom_RWIntersectionCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 156 : 
    {
      DeclareAndCast(StepVisual_Invisibility, anent, ent);
      RWStepVisual_RWInvisibility tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 157 : 
    {
      DeclareAndCast(StepBasic_LengthMeasureWithUnit, anent, ent);
      RWStepBasic_RWLengthMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 158 : 
    {
      DeclareAndCast(StepBasic_LengthUnit, anent, ent);
      RWStepBasic_RWLengthUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 159 : 
    {
      DeclareAndCast(StepGeom_Line, anent, ent);
      RWStepGeom_RWLine tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 160 : 
    {
      DeclareAndCast(StepBasic_LocalTime, anent, ent);
      RWStepBasic_RWLocalTime tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 161 : 
    {
      DeclareAndCast(StepShape_Loop, anent, ent);
      RWStepShape_RWLoop tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 162 : 
    {
      DeclareAndCast(StepShape_ManifoldSolidBrep, anent, ent);
      RWStepShape_RWManifoldSolidBrep tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 163 : 
    {
      DeclareAndCast(StepShape_ManifoldSurfaceShapeRepresentation, anent, ent);
      RWStepShape_RWManifoldSurfaceShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 164 : 
    {
      DeclareAndCast(StepRepr_MappedItem, anent, ent);
      RWStepRepr_RWMappedItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 165 : 
    {
      DeclareAndCast(StepBasic_MeasureWithUnit, anent, ent);
      RWStepBasic_RWMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 166 : 
    {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationArea, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationArea tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 167 : 
    {
      DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationRepresentation, anent, ent);
      RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 169 : 
    {
      DeclareAndCast(StepBasic_NamedUnit, anent, ent);
      RWStepBasic_RWNamedUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 171 : 
    {
      DeclareAndCast(StepGeom_OffsetCurve3d, anent, ent);
      RWStepGeom_RWOffsetCurve3d tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 172 : 
    {
      DeclareAndCast(StepGeom_OffsetSurface, anent, ent);
      RWStepGeom_RWOffsetSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 174 : 
    {
      DeclareAndCast(StepShape_OpenShell, anent, ent);
      RWStepShape_RWOpenShell tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 175 : 
    {
      DeclareAndCast(StepBasic_OrdinalDate, anent, ent);
      RWStepBasic_RWOrdinalDate tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 176 : 
    {
      DeclareAndCast(StepBasic_Organization, anent, ent);
      RWStepBasic_RWOrganization tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 178 : 
    {
      DeclareAndCast(StepBasic_OrganizationRole, anent, ent);
      RWStepBasic_RWOrganizationRole tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 179 : 
    {
      DeclareAndCast(StepBasic_OrganizationalAddress, anent, ent);
      RWStepBasic_RWOrganizationalAddress tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 180 : 
    {
      DeclareAndCast(StepShape_OrientedClosedShell, anent, ent);
      RWStepShape_RWOrientedClosedShell tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 181 : 
    {
      DeclareAndCast(StepShape_OrientedEdge, anent, ent);
      RWStepShape_RWOrientedEdge tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 182 : 
    {
      DeclareAndCast(StepShape_OrientedFace, anent, ent);
      RWStepShape_RWOrientedFace tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 183 : 
    {
      DeclareAndCast(StepShape_OrientedOpenShell, anent, ent);
      RWStepShape_RWOrientedOpenShell tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 184 : 
    {
      DeclareAndCast(StepShape_OrientedPath, anent, ent);
      RWStepShape_RWOrientedPath tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 185 : 
    {
      DeclareAndCast(StepGeom_OuterBoundaryCurve, anent, ent);
      RWStepGeom_RWOuterBoundaryCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 186 : 
    {
      DeclareAndCast(StepVisual_OverRidingStyledItem, anent, ent);
      RWStepVisual_RWOverRidingStyledItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 187 : 
    {
      DeclareAndCast(StepGeom_Parabola, anent, ent);
      RWStepGeom_RWParabola tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 188 : 
    {
      DeclareAndCast(StepRepr_ParametricRepresentationContext, anent, ent);
      RWStepRepr_RWParametricRepresentationContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 189 : 
    {
      DeclareAndCast(StepShape_Path, anent, ent);
      RWStepShape_RWPath tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 190 : 
    {
      DeclareAndCast(StepGeom_Pcurve, anent, ent);
      RWStepGeom_RWPcurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 191 : 
    {
      DeclareAndCast(StepBasic_Person, anent, ent);
      RWStepBasic_RWPerson tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 192 : 
    {
      DeclareAndCast(StepBasic_PersonAndOrganization, anent, ent);
      RWStepBasic_RWPersonAndOrganization tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 194 : 
    {
      DeclareAndCast(StepBasic_PersonAndOrganizationRole, anent, ent);
      RWStepBasic_RWPersonAndOrganizationRole tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 195 : 
    {
      DeclareAndCast(StepBasic_PersonalAddress, anent, ent);
      RWStepBasic_RWPersonalAddress tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 196 : 
    {
      DeclareAndCast(StepGeom_Placement, anent, ent);
      RWStepGeom_RWPlacement tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 197 : 
    {
      DeclareAndCast(StepVisual_PlanarBox, anent, ent);
      RWStepVisual_RWPlanarBox tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 198 : 
    {
      DeclareAndCast(StepVisual_PlanarExtent, anent, ent);
      RWStepVisual_RWPlanarExtent tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 199 : 
    {
      DeclareAndCast(StepGeom_Plane, anent, ent);
      RWStepGeom_RWPlane tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 200 : 
    {
      DeclareAndCast(StepBasic_PlaneAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWPlaneAngleMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 201 : 
    {
      DeclareAndCast(StepBasic_PlaneAngleUnit, anent, ent);
      RWStepBasic_RWPlaneAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 202 : 
    {
      DeclareAndCast(StepGeom_Point, anent, ent);
      RWStepGeom_RWPoint tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 203 : 
    {
      DeclareAndCast(StepGeom_PointOnCurve, anent, ent);
      RWStepGeom_RWPointOnCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 204 : 
    {
      DeclareAndCast(StepGeom_PointOnSurface, anent, ent);
      RWStepGeom_RWPointOnSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 205 : 
    {
      DeclareAndCast(StepGeom_PointReplica, anent, ent);
      RWStepGeom_RWPointReplica tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 206 : 
    {
      DeclareAndCast(StepVisual_PointStyle, anent, ent);
      RWStepVisual_RWPointStyle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 207 : 
    {
      DeclareAndCast(StepShape_PolyLoop, anent, ent);
      RWStepShape_RWPolyLoop tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 208 : 
    {
      DeclareAndCast(StepGeom_Polyline, anent, ent);
      RWStepGeom_RWPolyline tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 209 : 
    {
      DeclareAndCast(StepVisual_PreDefinedColour, anent, ent);
      RWStepVisual_RWPreDefinedColour tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 210 : 
    {
      DeclareAndCast(StepVisual_PreDefinedCurveFont, anent, ent);
      RWStepVisual_RWPreDefinedCurveFont tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 211 : 
  case 213 : 
    {
      DeclareAndCast(StepVisual_PreDefinedItem, anent, ent);
      RWStepVisual_RWPreDefinedItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 214 : 
    {
      DeclareAndCast(StepVisual_PresentationArea, anent, ent);
      RWStepVisual_RWPresentationArea tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 215 : 
    {
      DeclareAndCast(StepVisual_PresentationLayerAssignment, anent, ent);
      RWStepVisual_RWPresentationLayerAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 216 : 
    {
      DeclareAndCast(StepVisual_PresentationRepresentation, anent, ent);
      RWStepVisual_RWPresentationRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 217 : 
    {
      DeclareAndCast(StepVisual_PresentationSet, anent, ent);
      RWStepVisual_RWPresentationSet tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 218 : 
    {
      DeclareAndCast(StepVisual_PresentationSize, anent, ent);
      RWStepVisual_RWPresentationSize tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 219 : 
    {
      DeclareAndCast(StepVisual_PresentationStyleAssignment, anent, ent);
      RWStepVisual_RWPresentationStyleAssignment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 220 : 
    {
      DeclareAndCast(StepVisual_PresentationStyleByContext, anent, ent);
      RWStepVisual_RWPresentationStyleByContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 221 : 
    {
      DeclareAndCast(StepVisual_PresentationView, anent, ent);
      RWStepVisual_RWPresentationView tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 223 : 
    {
      DeclareAndCast(StepBasic_Product, anent, ent);
      RWStepBasic_RWProduct tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 224 : 
    {
      DeclareAndCast(StepBasic_ProductCategory, anent, ent);
      RWStepBasic_RWProductCategory tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 225 : 
    {
      DeclareAndCast(StepBasic_ProductContext, anent, ent);
      RWStepBasic_RWProductContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 227 : 
    {
      DeclareAndCast(StepBasic_ProductDefinition, anent, ent);
      RWStepBasic_RWProductDefinition tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 228 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 229 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormation, anent, ent);
      RWStepBasic_RWProductDefinitionFormation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 230 : 
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormationWithSpecifiedSource, anent, ent);
      RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 231 : 
    {
      DeclareAndCast(StepRepr_ProductDefinitionShape, anent, ent);
      RWStepRepr_RWProductDefinitionShape tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 232 : 
    {
      DeclareAndCast(StepBasic_ProductRelatedProductCategory, anent, ent);
      RWStepBasic_RWProductRelatedProductCategory tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 233 : 
    {
      DeclareAndCast(StepBasic_ProductType, anent, ent);
      RWStepBasic_RWProductType tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 234 : 
    {
      DeclareAndCast(StepRepr_PropertyDefinition, anent, ent);
      RWStepRepr_RWPropertyDefinition tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 235 : 
    {
      DeclareAndCast(StepRepr_PropertyDefinitionRepresentation, anent, ent);
      RWStepRepr_RWPropertyDefinitionRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 236 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 237 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 238 : 
    {
      DeclareAndCast(StepBasic_RatioMeasureWithUnit, anent, ent);
      RWStepBasic_RWRatioMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 239 : 
    {
      DeclareAndCast(StepGeom_RationalBSplineCurve, anent, ent);
      RWStepGeom_RWRationalBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 240 : 
    {
      DeclareAndCast(StepGeom_RationalBSplineSurface, anent, ent);
      RWStepGeom_RWRationalBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 241 : 
    {
      DeclareAndCast(StepGeom_RectangularCompositeSurface, anent, ent);
      RWStepGeom_RWRectangularCompositeSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 242 : 
    {
      DeclareAndCast(StepGeom_RectangularTrimmedSurface, anent, ent);
      RWStepGeom_RWRectangularTrimmedSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 243 : 
    {
      DeclareAndCast(StepAP214_RepItemGroup, anent, ent);
      RWStepAP214_RWRepItemGroup tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 244 : 
    {
      DeclareAndCast(StepGeom_ReparametrisedCompositeCurveSegment, anent, ent);
      RWStepGeom_RWReparametrisedCompositeCurveSegment tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 245 : 
    {
      DeclareAndCast(StepRepr_Representation, anent, ent);
      RWStepRepr_RWRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 246 : 
    {
      DeclareAndCast(StepRepr_RepresentationContext, anent, ent);
      RWStepRepr_RWRepresentationContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 247 : 
    {
      DeclareAndCast(StepRepr_RepresentationItem, anent, ent);
      RWStepRepr_RWRepresentationItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 248 : 
    {
      DeclareAndCast(StepRepr_RepresentationMap, anent, ent);
      RWStepRepr_RWRepresentationMap tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 249 : 
    {
      DeclareAndCast(StepRepr_RepresentationRelationship, anent, ent);
      RWStepRepr_RWRepresentationRelationship tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 250 : 
    {
      DeclareAndCast(StepShape_RevolvedAreaSolid, anent, ent);
      RWStepShape_RWRevolvedAreaSolid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 251 : 
    {
      DeclareAndCast(StepShape_RightAngularWedge, anent, ent);
      RWStepShape_RWRightAngularWedge tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 252 : 
    {
      DeclareAndCast(StepShape_RightCircularCone, anent, ent);
      RWStepShape_RWRightCircularCone tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 253 : 
    {
      DeclareAndCast(StepShape_RightCircularCylinder, anent, ent);
      RWStepShape_RWRightCircularCylinder tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 254 : 
    {
      DeclareAndCast(StepGeom_SeamCurve, anent, ent);
      RWStepGeom_RWSeamCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 255 : 
    {
      DeclareAndCast(StepBasic_SecurityClassification, anent, ent);
      RWStepBasic_RWSecurityClassification tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 257 : 
    {
      DeclareAndCast(StepBasic_SecurityClassificationLevel, anent, ent);
      RWStepBasic_RWSecurityClassificationLevel tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 258 : 
    {
      DeclareAndCast(StepRepr_ShapeAspect, anent, ent);
      RWStepRepr_RWShapeAspect tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 259 : 
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship, anent, ent);
      RWStepRepr_RWShapeAspectRelationship tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 260 : 
    {
      DeclareAndCast(StepRepr_ShapeAspectTransition, anent, ent);
      RWStepRepr_RWShapeAspectTransition tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 261 : 
    {
      DeclareAndCast(StepShape_ShapeDefinitionRepresentation, anent, ent);
      RWStepShape_RWShapeDefinitionRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 262 : 
    {
      DeclareAndCast(StepShape_ShapeRepresentation, anent, ent);
      RWStepShape_RWShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 263 : 
    {
      DeclareAndCast(StepShape_ShellBasedSurfaceModel, anent, ent);
      RWStepShape_RWShellBasedSurfaceModel tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 264 : 
    {
      DeclareAndCast(StepBasic_SiUnit, anent, ent);
      RWStepBasic_RWSiUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 265 : 
    {
      DeclareAndCast(StepBasic_SolidAngleMeasureWithUnit, anent, ent);
      RWStepBasic_RWSolidAngleMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 266 : 
    {
      DeclareAndCast(StepShape_SolidModel, anent, ent);
      RWStepShape_RWSolidModel tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 267 : 
    {
      DeclareAndCast(StepShape_SolidReplica, anent, ent);
      RWStepShape_RWSolidReplica tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 268 : 
    {
      DeclareAndCast(StepShape_Sphere, anent, ent);
      RWStepShape_RWSphere tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 269 : 
    {
      DeclareAndCast(StepGeom_SphericalSurface, anent, ent);
      RWStepGeom_RWSphericalSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 270 : 
    {
      DeclareAndCast(StepVisual_StyledItem, anent, ent);
      RWStepVisual_RWStyledItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 271 : 
    {
      DeclareAndCast(StepGeom_Surface, anent, ent);
      RWStepGeom_RWSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 272 : 
    {
      DeclareAndCast(StepGeom_SurfaceCurve, anent, ent);
      RWStepGeom_RWSurfaceCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 273 : 
    {
      DeclareAndCast(StepGeom_SurfaceOfLinearExtrusion, anent, ent);
      RWStepGeom_RWSurfaceOfLinearExtrusion tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 274 : 
    {
      DeclareAndCast(StepGeom_SurfaceOfRevolution, anent, ent);
      RWStepGeom_RWSurfaceOfRevolution tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 275 : 
    {
      DeclareAndCast(StepGeom_SurfacePatch, anent, ent);
      RWStepGeom_RWSurfacePatch tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 276 : 
    {
      DeclareAndCast(StepGeom_SurfaceReplica, anent, ent);
      RWStepGeom_RWSurfaceReplica tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 277 : 
    {
      DeclareAndCast(StepVisual_SurfaceSideStyle, anent, ent);
      RWStepVisual_RWSurfaceSideStyle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 278 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleBoundary, anent, ent);
      RWStepVisual_RWSurfaceStyleBoundary tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 279 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleControlGrid, anent, ent);
      RWStepVisual_RWSurfaceStyleControlGrid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 280 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleFillArea, anent, ent);
      RWStepVisual_RWSurfaceStyleFillArea tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 281 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleParameterLine, anent, ent);
      RWStepVisual_RWSurfaceStyleParameterLine tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 282 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleSegmentationCurve, anent, ent);
      RWStepVisual_RWSurfaceStyleSegmentationCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 283 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleSilhouette, anent, ent);
      RWStepVisual_RWSurfaceStyleSilhouette tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 284 : 
    {
      DeclareAndCast(StepVisual_SurfaceStyleUsage, anent, ent);
      RWStepVisual_RWSurfaceStyleUsage tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 285 : 
    {
      DeclareAndCast(StepShape_SweptAreaSolid, anent, ent);
      RWStepShape_RWSweptAreaSolid tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 286 : 
    {
      DeclareAndCast(StepGeom_SweptSurface, anent, ent);
      RWStepGeom_RWSweptSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 292 : 
    {
      DeclareAndCast(StepVisual_Template, anent, ent);
      RWStepVisual_RWTemplate tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 293 : 
    {
      DeclareAndCast(StepVisual_TemplateInstance, anent, ent);
      RWStepVisual_RWTemplateInstance tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 295 : 
    {
      DeclareAndCast(StepVisual_TextLiteral, anent, ent);
      RWStepVisual_RWTextLiteral tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 300 : 
    {
      DeclareAndCast(StepVisual_TextStyle, anent, ent);
      RWStepVisual_RWTextStyle tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 301 : 
    {
      DeclareAndCast(StepVisual_TextStyleForDefinedFont, anent, ent);
      RWStepVisual_RWTextStyleForDefinedFont tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 302 : 
    {
      DeclareAndCast(StepVisual_TextStyleWithBoxCharacteristics, anent, ent);
      RWStepVisual_RWTextStyleWithBoxCharacteristics tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 304 : 
    {
      DeclareAndCast(StepShape_TopologicalRepresentationItem, anent, ent);
      RWStepShape_RWTopologicalRepresentationItem tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 305 : 
    {
      DeclareAndCast(StepGeom_ToroidalSurface, anent, ent);
      RWStepGeom_RWToroidalSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 306 : 
    {
      DeclareAndCast(StepShape_Torus, anent, ent);
      RWStepShape_RWTorus tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 307 : 
    {
      DeclareAndCast(StepShape_TransitionalShapeRepresentation, anent, ent);
      RWStepShape_RWTransitionalShapeRepresentation tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 308 : 
    {
      DeclareAndCast(StepGeom_TrimmedCurve, anent, ent);
      RWStepGeom_RWTrimmedCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 310 : 
    {
      DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit, anent, ent);
      RWStepBasic_RWUncertaintyMeasureWithUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 311 : 
    {
      DeclareAndCast(StepGeom_UniformCurve, anent, ent);
      RWStepGeom_RWUniformCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 312 : 
    {
      DeclareAndCast(StepGeom_UniformSurface, anent, ent);
      RWStepGeom_RWUniformSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 313 : 
    {
      DeclareAndCast(StepGeom_Vector, anent, ent);
      RWStepGeom_RWVector tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 314 : 
    {
      DeclareAndCast(StepShape_Vertex, anent, ent);
      RWStepShape_RWVertex tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 315 : 
    {
      DeclareAndCast(StepShape_VertexLoop, anent, ent);
      RWStepShape_RWVertexLoop tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 316 : 
    {
      DeclareAndCast(StepShape_VertexPoint, anent, ent);
      RWStepShape_RWVertexPoint tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 317 : 
    {
      DeclareAndCast(StepVisual_ViewVolume, anent, ent);
      RWStepVisual_RWViewVolume tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 318 : 
    {
      DeclareAndCast(StepBasic_WeekOfYearAndDayDate, anent, ent);
      RWStepBasic_RWWeekOfYearAndDayDate tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 319 : 
    {
      DeclareAndCast(StepGeom_UniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWUniformCurveAndRationalBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 320 : 
    {
      DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 321 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 322 : 
    {
      DeclareAndCast(StepGeom_BezierCurveAndRationalBSplineCurve, anent, ent);
      RWStepGeom_RWBezierCurveAndRationalBSplineCurve tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 323 : 
    {
      DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 324 : 
    {
      DeclareAndCast(StepGeom_UniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 325 : 
    {
      DeclareAndCast(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 326 : 
    {
      DeclareAndCast(StepGeom_BezierSurfaceAndRationalBSplineSurface, anent, ent);
      RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 327 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWSiUnitAndLengthUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 328 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndPlaneAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 329 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndLengthUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndLengthUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 330 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndPlaneAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 331 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 332 : 
    {
      DeclareAndCast(StepShape_LoopAndPath, anent, ent);
      RWStepShape_RWLoopAndPath tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    
    break;
    
    // ------------
    // Added by FMA
    // ------------
    
  case 333 :
    {
      DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, anent, ent);
      RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 334 :
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 335 :
    {
      DeclareAndCast(StepBasic_SiUnitAndSolidAngleUnit, anent, ent);
      RWStepBasic_RWSiUnitAndSolidAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
  case 336 :
    {
      DeclareAndCast(StepBasic_SolidAngleUnit, anent, ent);
      RWStepBasic_RWSolidAngleUnit tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
  case 337 :
    {
      DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids, anent, ent);
      RWStepShape_RWFacetedBrepAndBrepWithVoids tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
  case 338 : 
    {
      DeclareAndCast(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext, anent, ent);
      RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;
  case 339 : 
    {
      DeclareAndCast(StepBasic_MechanicalContext, anent, ent);
      RWStepBasic_RWMechanicalContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
    }
    break;

  case 340 :    // added by CKY : DesignContext cf ProductDefinitionContext
    {
      DeclareAndCast(StepBasic_ProductDefinitionContext, anent, ent);
      RWStepBasic_RWProductDefinitionContext tool;
//      if (anent.IsNull()) return; 
      tool.WriteStep (SW,anent);
      break;
    }

    // -----------
    // Added for Rev4
    // -----------
    
  case 341:  // TimeMeasureWithUnit
    {
      DeclareAndCast(StepBasic_MeasureWithUnit,anent,ent);
      RWStepBasic_RWMeasureWithUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;

  case 342:
  case 343:  // RatioUnit, TimeUnit
    {
      DeclareAndCast(StepBasic_NamedUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 344:
    {
      DeclareAndCast(StepBasic_SiUnitAndRatioUnit,anent,ent);
      RWStepBasic_RWSiUnitAndRatioUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 345:
    {
      DeclareAndCast(StepBasic_SiUnitAndTimeUnit,anent,ent);
      RWStepBasic_RWSiUnitAndTimeUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 346:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndRatioUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndRatioUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 347:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndTimeUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndTimeUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 348:  // ApprovalDateTime
    {
      DeclareAndCast(StepBasic_ApprovalDateTime,anent,ent);
      RWStepBasic_RWApprovalDateTime tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 349: // CameraImage 2d and 3d
  case 350:
    {
      DeclareAndCast(StepVisual_CameraImage,anent,ent);
      RWStepVisual_RWCameraImage tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 351:
    {
      DeclareAndCast(StepGeom_CartesianTransformationOperator,anent,ent);
      RWStepGeom_RWCartesianTransformationOperator tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 352:
    {
      DeclareAndCast(StepBasic_DerivedUnit,anent,ent);
      RWStepBasic_RWDerivedUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 353:
    {
      DeclareAndCast(StepBasic_DerivedUnitElement,anent,ent);
      RWStepBasic_RWDerivedUnitElement tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 354:
    {
      DeclareAndCast(StepRepr_ItemDefinedTransformation,anent,ent);
      RWStepRepr_RWItemDefinedTransformation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 355:
    {
      DeclareAndCast(StepVisual_PresentedItemRepresentation,anent,ent);
      RWStepVisual_RWPresentedItemRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 356:
    {
      DeclareAndCast(StepVisual_PresentationLayerUsage,anent,ent);
      RWStepVisual_RWPresentationLayerUsage tool;
      tool.WriteStep (SW,anent);
    }
    break;

    //:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
  case 358 : 
    {
      DeclareAndCast(StepGeom_SurfaceCurveAndBoundedCurve, anent, ent);
      RWStepGeom_RWSurfaceCurveAndBoundedCurve tool;
      tool.WriteStep (SW,anent);
      break;
    }

    //  AP214 : CC1 -> CC2
  case 366:
    {
      DeclareAndCast(StepAP214_AutoDesignDocumentReference,anent,ent);
      RWStepAP214_RWAutoDesignDocumentReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 367:
  case 368:
    {
      DeclareAndCast(StepBasic_Document,anent,ent);
      RWStepBasic_RWDocument tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 369:
    {
      DeclareAndCast(StepBasic_DocumentRelationship,anent,ent);
      RWStepBasic_RWDocumentRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 370:
    {
      DeclareAndCast(StepBasic_DocumentType,anent,ent);
      RWStepBasic_RWDocumentType tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 371:
    {
      DeclareAndCast(StepBasic_DocumentUsageConstraint,anent,ent);
      RWStepBasic_RWDocumentUsageConstraint tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 372:
    {
      DeclareAndCast(StepBasic_Effectivity,anent,ent);
      RWStepBasic_RWEffectivity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 373:
    {
      DeclareAndCast(StepBasic_ProductDefinitionEffectivity,anent,ent);
      RWStepBasic_RWProductDefinitionEffectivity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 374:
    {
      DeclareAndCast(StepBasic_ProductDefinitionRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 375:
    {
      DeclareAndCast(StepBasic_ProductDefinitionWithAssociatedDocuments,anent,ent);
      RWStepBasic_RWProductDefinitionWithAssociatedDocuments tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 376:
    {
      DeclareAndCast(StepBasic_PhysicallyModeledProductDefinition,anent,ent);
      RWStepBasic_RWProductDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 377:
    {
      DeclareAndCast(StepRepr_ProductDefinitionUsage,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 378:
    {
      DeclareAndCast(StepRepr_MakeFromUsageOption,anent,ent);
      RWStepRepr_RWMakeFromUsageOption tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 379:
  case 380:
  case 381:
    {
      DeclareAndCast(StepRepr_AssemblyComponentUsage,anent,ent);
      RWStepRepr_RWAssemblyComponentUsage tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 382:
    {
      DeclareAndCast(StepRepr_QuantifiedAssemblyComponentUsage,anent,ent);
      RWStepRepr_RWQuantifiedAssemblyComponentUsage tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 383:
    {
      DeclareAndCast(StepRepr_SpecifiedHigherUsageOccurrence,anent,ent);
      RWStepRepr_RWSpecifiedHigherUsageOccurrence tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 384:
    {
      DeclareAndCast(StepRepr_AssemblyComponentUsageSubstitute,anent,ent);
      RWStepRepr_RWAssemblyComponentUsageSubstitute tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 385:
    {
      DeclareAndCast(StepRepr_SuppliedPartRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 386:
    {
      DeclareAndCast(StepRepr_ExternallyDefinedRepresentation,anent,ent);
      RWStepRepr_RWRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 387:
    {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationship,anent,ent);
      RWStepRepr_RWRepresentationRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 388:
    {
      DeclareAndCast(StepRepr_RepresentationRelationshipWithTransformation,anent,ent);
      RWStepRepr_RWRepresentationRelationshipWithTransformation tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 389:
    {
      DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation,anent,ent);
      RWStepRepr_RWShapeRepresentationRelationshipWithTransformation tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 390:
    {
      DeclareAndCast(StepRepr_MaterialDesignation,anent,ent);
      RWStepRepr_RWMaterialDesignation tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 391:
    {
      DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anent,ent);
      RWStepShape_RWContextDependentShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;

    //:S4134: Added from CD to DIS
  case 392:
    {
      DeclareAndCast(StepAP214_AppliedDateAndTimeAssignment,anent,ent);
      RWStepAP214_RWAppliedDateAndTimeAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 393:
    {
      DeclareAndCast(StepAP214_AppliedDateAssignment,anent,ent);
      RWStepAP214_RWAppliedDateAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 394:
    {
      DeclareAndCast(StepAP214_AppliedApprovalAssignment,anent,ent);
      RWStepAP214_RWAppliedApprovalAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 395:
    {
      DeclareAndCast(StepAP214_AppliedGroupAssignment,anent,ent);
      RWStepAP214_RWAppliedGroupAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 396:
    {
      DeclareAndCast(StepAP214_AppliedOrganizationAssignment,anent,ent);
      RWStepAP214_RWAppliedOrganizationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 397:
    {
      DeclareAndCast(StepAP214_AppliedPersonAndOrganizationAssignment,anent,ent);
      RWStepAP214_RWAppliedPersonAndOrganizationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 398:
    {
      DeclareAndCast(StepAP214_AppliedPresentedItem,anent,ent);
      RWStepAP214_RWAppliedPresentedItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 399:
    {
      DeclareAndCast(StepAP214_AppliedSecurityClassificationAssignment,anent,ent);
      RWStepAP214_RWAppliedSecurityClassificationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 400:
    {
      DeclareAndCast(StepAP214_AppliedDocumentReference,anent,ent);
      RWStepAP214_RWAppliedDocumentReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 401:
    {
      DeclareAndCast(StepBasic_DocumentFile,anent,ent);
      RWStepBasic_RWDocumentFile tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 402:
    {
      DeclareAndCast(StepBasic_CharacterizedObject,anent,ent);
      RWStepBasic_RWCharacterizedObject tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 403:
    {
      DeclareAndCast(StepShape_ExtrudedFaceSolid,anent,ent);
      RWStepShape_RWExtrudedFaceSolid tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  case 404:
    {
      DeclareAndCast(StepShape_RevolvedFaceSolid,anent,ent);
      RWStepShape_RWRevolvedFaceSolid tool;
	tool.WriteStep (SW,anent);
    }
    break;
  case 405:
    {
      DeclareAndCast(StepShape_SweptFaceSolid,anent,ent);
      RWStepShape_RWSweptFaceSolid tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
  case 406:
    {
      DeclareAndCast(StepRepr_MeasureRepresentationItem,anent,ent);
      RWStepRepr_RWMeasureRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 407:
    {
      DeclareAndCast(StepBasic_AreaUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;    
  case 408:
    {
      DeclareAndCast(StepBasic_VolumeUnit,anent,ent);
      RWStepBasic_RWNamedUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;    
  case 409:
    {
      DeclareAndCast(StepBasic_SiUnitAndAreaUnit,anent,ent);
      RWStepBasic_RWSiUnitAndAreaUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 410:
    {
      DeclareAndCast(StepBasic_SiUnitAndVolumeUnit,anent,ent);
      RWStepBasic_RWSiUnitAndVolumeUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 411:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndAreaUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndAreaUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 412:
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndVolumeUnit,anent,ent);
      RWStepBasic_RWConversionBasedUnitAndVolumeUnit tool;
      tool.WriteStep (SW,anent);
    }
    break; 

  // Added by ABV 10.11.99 for AP203
  case 413:
    {
      DeclareAndCast(StepBasic_Action,anent,ent);
      RWStepBasic_RWAction tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 414:
    {
      DeclareAndCast(StepBasic_ActionAssignment,anent,ent);
      RWStepBasic_RWActionAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 415:
    {
      DeclareAndCast(StepBasic_ActionMethod,anent,ent);
      RWStepBasic_RWActionMethod tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 416:
    {
      DeclareAndCast(StepBasic_ActionRequestAssignment,anent,ent);
      RWStepBasic_RWActionRequestAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 417:
    {
      DeclareAndCast(StepAP203_CcDesignApproval,anent,ent);
      RWStepAP203_RWCcDesignApproval tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 418:
    {
      DeclareAndCast(StepAP203_CcDesignCertification,anent,ent);
      RWStepAP203_RWCcDesignCertification tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 419:
    {
      DeclareAndCast(StepAP203_CcDesignContract,anent,ent);
      RWStepAP203_RWCcDesignContract tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 420:
    {
      DeclareAndCast(StepAP203_CcDesignDateAndTimeAssignment,anent,ent);
      RWStepAP203_RWCcDesignDateAndTimeAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 421:
    {
      DeclareAndCast(StepAP203_CcDesignPersonAndOrganizationAssignment,anent,ent);
      RWStepAP203_RWCcDesignPersonAndOrganizationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 422:
    {
      DeclareAndCast(StepAP203_CcDesignSecurityClassification,anent,ent);
      RWStepAP203_RWCcDesignSecurityClassification tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 423:
    {
      DeclareAndCast(StepAP203_CcDesignSpecificationReference,anent,ent);
      RWStepAP203_RWCcDesignSpecificationReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 424:
    {
      DeclareAndCast(StepBasic_Certification,anent,ent);
      RWStepBasic_RWCertification tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 425:
    {
      DeclareAndCast(StepBasic_CertificationAssignment,anent,ent);
      RWStepBasic_RWCertificationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 426:
    {
      DeclareAndCast(StepBasic_CertificationType,anent,ent);
      RWStepBasic_RWCertificationType tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 427:
    {
      DeclareAndCast(StepAP203_Change,anent,ent);
      RWStepAP203_RWChange tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 428:
    {
      DeclareAndCast(StepAP203_ChangeRequest,anent,ent);
      RWStepAP203_RWChangeRequest tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 429:
    {
      DeclareAndCast(StepRepr_ConfigurationDesign,anent,ent);
      RWStepRepr_RWConfigurationDesign tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 430:
    {
      DeclareAndCast(StepRepr_ConfigurationEffectivity,anent,ent);
      RWStepRepr_RWConfigurationEffectivity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 431:
    {
      DeclareAndCast(StepBasic_Contract,anent,ent);
      RWStepBasic_RWContract tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 432:
    {
      DeclareAndCast(StepBasic_ContractAssignment,anent,ent);
      RWStepBasic_RWContractAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 433:
    {
      DeclareAndCast(StepBasic_ContractType,anent,ent);
      RWStepBasic_RWContractType tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 434:
    {
      DeclareAndCast(StepRepr_ProductConcept,anent,ent);
      RWStepRepr_RWProductConcept tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 435:
    {
      DeclareAndCast(StepBasic_ProductConceptContext,anent,ent);
      RWStepBasic_RWProductConceptContext tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 436:
    {
      DeclareAndCast(StepAP203_StartRequest,anent,ent);
      RWStepAP203_RWStartRequest tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 437:
    {
      DeclareAndCast(StepAP203_StartWork,anent,ent);
      RWStepAP203_RWStartWork tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 438:
    {
      DeclareAndCast(StepBasic_VersionedActionRequest,anent,ent);
      RWStepBasic_RWVersionedActionRequest tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 439:
    {
      DeclareAndCast(StepBasic_ProductCategoryRelationship,anent,ent);
      RWStepBasic_RWProductCategoryRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 440:
    {
      DeclareAndCast(StepBasic_ActionRequestSolution,anent,ent);
      RWStepBasic_RWActionRequestSolution tool;
      tool.WriteStep (SW,anent);
    }
    break;

  case 441:
    {
      DeclareAndCast(StepVisual_DraughtingModel,anent,ent);
      RWStepVisual_RWDraughtingModel tool;
      tool.WriteStep (SW,anent);
    }
    break;
    
  // Added by ABV 18.04.00 for CAX-IF TRJ4
  case 442:
    {
      DeclareAndCast(StepShape_AngularLocation,anent,ent);
      RWStepShape_RWAngularLocation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 443:
    {
      DeclareAndCast(StepShape_AngularSize,anent,ent);
      RWStepShape_RWAngularSize tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 444:
    {
      DeclareAndCast(StepShape_DimensionalCharacteristicRepresentation,anent,ent);
      RWStepShape_RWDimensionalCharacteristicRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 445:
    {
      DeclareAndCast(StepShape_DimensionalLocation,anent,ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 446:
    {
      DeclareAndCast(StepShape_DimensionalLocationWithPath,anent,ent);
      RWStepShape_RWDimensionalLocationWithPath tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 447:
    {
      DeclareAndCast(StepShape_DimensionalSize,anent,ent);
      RWStepShape_RWDimensionalSize tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 448:
    {
      DeclareAndCast(StepShape_DimensionalSizeWithPath,anent,ent);
      RWStepShape_RWDimensionalSizeWithPath tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 449:
    {
      DeclareAndCast(StepShape_ShapeDimensionRepresentation,anent,ent);
      RWStepShape_RWShapeDimensionRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;

    // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
  case 450:
    {
      DeclareAndCast(StepBasic_DocumentRepresentationType,anent,ent);
      RWStepBasic_RWDocumentRepresentationType tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 451:
    {
      DeclareAndCast(StepBasic_ObjectRole,anent,ent);
      RWStepBasic_RWObjectRole tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 452:
    {
      DeclareAndCast(StepBasic_RoleAssociation,anent,ent);
      RWStepBasic_RWRoleAssociation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 453:
    {
      DeclareAndCast(StepBasic_IdentificationRole,anent,ent);
      RWStepBasic_RWIdentificationRole tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 454:
    {
      DeclareAndCast(StepBasic_IdentificationAssignment,anent,ent);
      RWStepBasic_RWIdentificationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 455:
    {
      DeclareAndCast(StepBasic_ExternalIdentificationAssignment,anent,ent);
      RWStepBasic_RWExternalIdentificationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 456:
    {
      DeclareAndCast(StepBasic_EffectivityAssignment,anent,ent);
      RWStepBasic_RWEffectivityAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 457:
    {
      DeclareAndCast(StepBasic_NameAssignment,anent,ent);
      RWStepBasic_RWNameAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 458:
    {
      DeclareAndCast(StepBasic_GeneralProperty,anent,ent);
      RWStepBasic_RWGeneralProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 459:
    {
      DeclareAndCast(StepAP214_Class,anent,ent);
      RWStepAP214_RWClass tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 460:
    {
      DeclareAndCast(StepAP214_ExternallyDefinedClass,anent,ent);
      RWStepAP214_RWExternallyDefinedClass tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 461:
    {
      DeclareAndCast(StepAP214_ExternallyDefinedGeneralProperty,anent,ent);
      RWStepAP214_RWExternallyDefinedGeneralProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 462:
    {
      DeclareAndCast(StepAP214_AppliedExternalIdentificationAssignment,anent,ent);
      RWStepAP214_RWAppliedExternalIdentificationAssignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 463:
    {
      DeclareAndCast(StepShape_DefinitionalRepresentationAndShapeRepresentation,anent,ent);
      RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;

      // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
  case 470:
    {
      DeclareAndCast(StepRepr_CompositeShapeAspect,anent,ent);
      RWStepRepr_RWCompositeShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 471:
    {
      DeclareAndCast(StepRepr_DerivedShapeAspect,anent,ent);
      RWStepRepr_RWDerivedShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 472:
    {
      DeclareAndCast(StepRepr_Extension,anent,ent);
      RWStepRepr_RWExtension tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 473:  // same as DimensionalLocation
    {
      DeclareAndCast(StepShape_DirectedDimensionalLocation,anent,ent);
      RWStepShape_RWDimensionalLocation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 474:
    {
      DeclareAndCast(StepShape_LimitsAndFits,anent,ent);
      RWStepShape_RWLimitsAndFits tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 475:
    {
      DeclareAndCast(StepShape_ToleranceValue,anent,ent);
      RWStepShape_RWToleranceValue tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 476:
    {
      DeclareAndCast(StepShape_MeasureQualification,anent,ent);
      RWStepShape_RWMeasureQualification tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 477:
    {
      DeclareAndCast(StepShape_PlusMinusTolerance,anent,ent);
      RWStepShape_RWPlusMinusTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 478:
    {
      DeclareAndCast(StepShape_PrecisionQualifier,anent,ent);
      RWStepShape_RWPrecisionQualifier tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 479:
    {
      DeclareAndCast(StepShape_TypeQualifier,anent,ent);
      RWStepShape_RWTypeQualifier tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 480:
    {
      DeclareAndCast(StepShape_QualifiedRepresentationItem,anent,ent);
      RWStepShape_RWQualifiedRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 481:
    {
      DeclareAndCast(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,anent,ent);
      RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 482:
  case 483:
    {
      DeclareAndCast(StepRepr_CompoundRepresentationItem,anent,ent);
      RWStepRepr_RWCompoundRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;

  case 484:  // same as ShapeAspectRelationship
    {
      DeclareAndCast(StepRepr_ShapeAspectRelationship,anent,ent);
      RWStepRepr_RWShapeAspectRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;

    // abv 28.12.01
  case 485:
    {
      DeclareAndCast(StepShape_CompoundShapeRepresentation,anent,ent);
      RWStepShape_RWCompoundShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 486:
    {
      DeclareAndCast(StepShape_ConnectedEdgeSet,anent,ent);
      RWStepShape_RWConnectedEdgeSet tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 487:
    {
      DeclareAndCast(StepShape_ConnectedFaceShapeRepresentation,anent,ent);
      RWStepShape_RWConnectedFaceShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 488:
    {
      DeclareAndCast(StepShape_EdgeBasedWireframeModel,anent,ent);
      RWStepShape_RWEdgeBasedWireframeModel tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 489:
    {
      DeclareAndCast(StepShape_EdgeBasedWireframeShapeRepresentation,anent,ent);
      RWStepShape_RWEdgeBasedWireframeShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 490:
    {
      DeclareAndCast(StepShape_FaceBasedSurfaceModel,anent,ent);
      RWStepShape_RWFaceBasedSurfaceModel tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 491:
    {
      DeclareAndCast(StepShape_NonManifoldSurfaceShapeRepresentation,anent,ent);
      RWStepShape_RWNonManifoldSurfaceShapeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
   case 492:
    {
      DeclareAndCast(StepGeom_OrientedSurface,anent,ent);
      RWStepGeom_RWOrientedSurface tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 493:
    {
      DeclareAndCast(StepShape_Subface,anent,ent);
      RWStepShape_RWSubface tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 494:
    {
      DeclareAndCast(StepShape_Subedge,anent,ent);
      RWStepShape_RWSubedge tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 495:
    {
      DeclareAndCast(StepShape_SeamEdge,anent,ent);
      RWStepShape_RWSeamEdge tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 496:
    {
      DeclareAndCast(StepShape_ConnectedFaceSubSet,anent,ent);
      RWStepShape_RWConnectedFaceSubSet tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 500:
    {
      DeclareAndCast(StepBasic_EulerAngles,anent,ent);
      RWStepBasic_RWEulerAngles tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 501:
    {
      DeclareAndCast(StepBasic_MassUnit,anent,ent);
      RWStepBasic_RWMassUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 502:
    {
      DeclareAndCast(StepBasic_ThermodynamicTemperatureUnit,anent,ent);
      RWStepBasic_RWThermodynamicTemperatureUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 503:
    {
      DeclareAndCast(StepElement_AnalysisItemWithinRepresentation,anent,ent);
      RWStepElement_RWAnalysisItemWithinRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 504:
    {
      DeclareAndCast(StepElement_Curve3dElementDescriptor,anent,ent);
      RWStepElement_RWCurve3dElementDescriptor tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 505:
    {
      DeclareAndCast(StepElement_CurveElementEndReleasePacket,anent,ent);
      RWStepElement_RWCurveElementEndReleasePacket tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 506:
    {
      DeclareAndCast(StepElement_CurveElementSectionDefinition,anent,ent);
      RWStepElement_RWCurveElementSectionDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 507:
    {
      DeclareAndCast(StepElement_CurveElementSectionDerivedDefinitions,anent,ent);
      RWStepElement_RWCurveElementSectionDerivedDefinitions tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 508:
    {
      DeclareAndCast(StepElement_ElementDescriptor,anent,ent);
      RWStepElement_RWElementDescriptor tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 509:
    {
      DeclareAndCast(StepElement_ElementMaterial,anent,ent);
      RWStepElement_RWElementMaterial tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 510:
    {
      DeclareAndCast(StepElement_Surface3dElementDescriptor,anent,ent);
      RWStepElement_RWSurface3dElementDescriptor tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 511:
    {
      DeclareAndCast(StepElement_SurfaceElementProperty,anent,ent);
      RWStepElement_RWSurfaceElementProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 512:
    {
      DeclareAndCast(StepElement_SurfaceSection,anent,ent);
      RWStepElement_RWSurfaceSection tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 513:
    {
      DeclareAndCast(StepElement_SurfaceSectionField,anent,ent);
      RWStepElement_RWSurfaceSectionField tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 514:
    {
      DeclareAndCast(StepElement_SurfaceSectionFieldConstant,anent,ent);
      RWStepElement_RWSurfaceSectionFieldConstant tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 515:
    {
      DeclareAndCast(StepElement_SurfaceSectionFieldVarying,anent,ent);
      RWStepElement_RWSurfaceSectionFieldVarying tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 516:
    {
      DeclareAndCast(StepElement_UniformSurfaceSection,anent,ent);
      RWStepElement_RWUniformSurfaceSection tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 517:
    {
      DeclareAndCast(StepElement_Volume3dElementDescriptor,anent,ent);
      RWStepElement_RWVolume3dElementDescriptor tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 518:
    {
      DeclareAndCast(StepFEA_AlignedCurve3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWAlignedCurve3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 519:
    {
      DeclareAndCast(StepFEA_ArbitraryVolume3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 520:
    {
      DeclareAndCast(StepFEA_Curve3dElementProperty,anent,ent);
      RWStepFEA_RWCurve3dElementProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 521:
    {
      DeclareAndCast(StepFEA_Curve3dElementRepresentation,anent,ent);
      RWStepFEA_RWCurve3dElementRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 522:
    {
      DeclareAndCast(StepFEA_Node,anent,ent);
      RWStepFEA_RWNode tool;
      tool.WriteStep (SW,anent);
    }
    break;
//case 523:
//    {
//      DeclareAndCast(StepFEA_CurveElementEndCoordinateSystem,anent,ent);
//      RWStepFEA_RWCurveElementEndCoordinateSystem tool;
//      tool.WriteStep (SW,anent);
//    }
//    break;
  case 524:
    {
      DeclareAndCast(StepFEA_CurveElementEndOffset,anent,ent);
      RWStepFEA_RWCurveElementEndOffset tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 525:
    {
      DeclareAndCast(StepFEA_CurveElementEndRelease,anent,ent);
      RWStepFEA_RWCurveElementEndRelease tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 526:
    {
      DeclareAndCast(StepFEA_CurveElementInterval,anent,ent);
      RWStepFEA_RWCurveElementInterval tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 527:
    {
      DeclareAndCast(StepFEA_CurveElementIntervalConstant,anent,ent);
      RWStepFEA_RWCurveElementIntervalConstant tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 528:
    {
      DeclareAndCast(StepFEA_DummyNode,anent,ent);
      RWStepFEA_RWDummyNode tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 529:
    {
      DeclareAndCast(StepFEA_CurveElementLocation,anent,ent);
      RWStepFEA_RWCurveElementLocation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 530:
    {
      DeclareAndCast(StepFEA_ElementGeometricRelationship,anent,ent);
      RWStepFEA_RWElementGeometricRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 531:
    {
      DeclareAndCast(StepFEA_ElementGroup,anent,ent);
      RWStepFEA_RWElementGroup tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 532:
    {
      DeclareAndCast(StepFEA_ElementRepresentation,anent,ent);
      RWStepFEA_RWElementRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 533:
    {
      DeclareAndCast(StepFEA_FeaAreaDensity,anent,ent);
      RWStepFEA_RWFeaAreaDensity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 534:
    {
      DeclareAndCast(StepFEA_FeaAxis2Placement3d,anent,ent);
      RWStepFEA_RWFeaAxis2Placement3d tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 535:
    {
      DeclareAndCast(StepFEA_FeaGroup,anent,ent);
      RWStepFEA_RWFeaGroup tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 536:
    {
      DeclareAndCast(StepFEA_FeaLinearElasticity,anent,ent);
      RWStepFEA_RWFeaLinearElasticity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 537:
    {
      DeclareAndCast(StepFEA_FeaMassDensity,anent,ent);
      RWStepFEA_RWFeaMassDensity tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 538:
    {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentation,anent,ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 539:
    {
      DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentationItem,anent,ent);
      RWStepFEA_RWFeaMaterialPropertyRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 540:
    {
      DeclareAndCast(StepFEA_FeaModel,anent,ent);
      RWStepFEA_RWFeaModel tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 541:
    {
      DeclareAndCast(StepFEA_FeaModel3d,anent,ent);
      RWStepFEA_RWFeaModel3d tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 542:
    {
      DeclareAndCast(StepFEA_FeaMoistureAbsorption,anent,ent);
      RWStepFEA_RWFeaMoistureAbsorption tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 543:
    {
      DeclareAndCast(StepFEA_FeaParametricPoint,anent,ent);
      RWStepFEA_RWFeaParametricPoint tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 544:
    {
      DeclareAndCast(StepFEA_FeaRepresentationItem,anent,ent);
      RWStepFEA_RWFeaRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 545:
    {
      DeclareAndCast(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion,anent,ent);
      RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 546:
    {
      DeclareAndCast(StepFEA_FeaShellBendingStiffness,anent,ent);
      RWStepFEA_RWFeaShellBendingStiffness tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 547:
    {
      DeclareAndCast(StepFEA_FeaShellMembraneBendingCouplingStiffness,anent,ent);
      RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 548:
    {
      DeclareAndCast(StepFEA_FeaShellMembraneStiffness,anent,ent);
      RWStepFEA_RWFeaShellMembraneStiffness tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 549:
    {
      DeclareAndCast(StepFEA_FeaShellShearStiffness,anent,ent);
      RWStepFEA_RWFeaShellShearStiffness tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 550:
    {
      DeclareAndCast(StepFEA_GeometricNode,anent,ent);
      RWStepFEA_RWGeometricNode tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 551:
    {
      DeclareAndCast(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion,anent,ent);
      RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 552:
    {
      DeclareAndCast(StepFEA_NodeGroup,anent,ent);
      RWStepFEA_RWNodeGroup tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 553:
    {
      DeclareAndCast(StepFEA_NodeRepresentation,anent,ent);
      RWStepFEA_RWNodeRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 554:
    {
      DeclareAndCast(StepFEA_NodeSet,anent,ent);
      RWStepFEA_RWNodeSet tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 555:
    {
      DeclareAndCast(StepFEA_NodeWithSolutionCoordinateSystem,anent,ent);
      RWStepFEA_RWNodeWithSolutionCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 556:
    {
      DeclareAndCast(StepFEA_NodeWithVector,anent,ent);
      RWStepFEA_RWNodeWithVector tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 557:
    {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateDirection,anent,ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateDirection tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 558:
    {
      DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWParametricCurve3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 559:
    {
      DeclareAndCast(StepFEA_ParametricSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWParametricSurface3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 560:
    {
      DeclareAndCast(StepFEA_Surface3dElementRepresentation,anent,ent);
      RWStepFEA_RWSurface3dElementRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
//case 561:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor22d,anent,ent);
//      RWStepFEA_RWSymmetricTensor22d tool;
//      tool.WriteStep (SW,anent);
//    }
//    break;
//case 562:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor42d,anent,ent);
//      RWStepFEA_RWSymmetricTensor42d tool;
//      tool.WriteStep (SW,anent);
//    }
//    break;
//case 563:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor43d,anent,ent);
//      RWStepFEA_RWSymmetricTensor43d tool;
//      tool.WriteStep (SW,anent);
//    }
//    break;
  case 564:
    {
      DeclareAndCast(StepFEA_Volume3dElementRepresentation,anent,ent);
      RWStepFEA_RWVolume3dElementRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 565:
    {
      DeclareAndCast(StepRepr_DataEnvironment,anent,ent);
      RWStepRepr_RWDataEnvironment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 566:
    {
      DeclareAndCast(StepRepr_MaterialPropertyRepresentation,anent,ent);
      RWStepRepr_RWMaterialPropertyRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 567:
    {
      DeclareAndCast(StepRepr_PropertyDefinitionRelationship,anent,ent);
      RWStepRepr_RWPropertyDefinitionRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 568:
    {
      DeclareAndCast(StepShape_PointRepresentation,anent,ent);
      RWStepShape_RWPointRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 569:
    {
      DeclareAndCast(StepRepr_MaterialProperty,anent,ent);
      RWStepRepr_RWMaterialProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 570:
    {
      DeclareAndCast(StepFEA_FeaModelDefinition,anent,ent);
      RWStepFEA_RWFeaModelDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 571:
    {
      DeclareAndCast(StepFEA_FreedomAndCoefficient,anent,ent);
      RWStepFEA_RWFreedomAndCoefficient tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 572:
    {
      DeclareAndCast(StepFEA_FreedomsList,anent,ent);
      RWStepFEA_RWFreedomsList tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 573:
    {
      DeclareAndCast(StepBasic_ProductDefinitionFormationRelationship,anent,ent);
      RWStepBasic_RWProductDefinitionFormationRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 574 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndMassUnit, anent, ent);
      RWStepBasic_RWSiUnitAndMassUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 575:
    {
      DeclareAndCast(StepFEA_NodeDefinition,anent,ent);
      RWStepFEA_RWNodeDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 576:
    {
      DeclareAndCast(StepRepr_StructuralResponseProperty,anent,ent);
      RWStepRepr_RWStructuralResponseProperty tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 577:
    {
      DeclareAndCast(StepRepr_StructuralResponsePropertyDefinitionRepresentation,anent,ent);
      RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 578 : 
    {
      DeclareAndCast(StepBasic_SiUnitAndThermodynamicTemperatureUnit, anent, ent);
      RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 579:
    {
      DeclareAndCast(StepFEA_AlignedSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWAlignedSurface3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 580:
    {
      DeclareAndCast(StepFEA_ConstantSurface3dElementCoordinateSystem,anent,ent);
      RWStepFEA_RWConstantSurface3dElementCoordinateSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 581:
    {
      DeclareAndCast(StepFEA_CurveElementIntervalLinearlyVarying,anent,ent);
      RWStepFEA_RWCurveElementIntervalLinearlyVarying tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 582:
    {
      DeclareAndCast(StepFEA_FeaCurveSectionGeometricRelationship,anent,ent);
      RWStepFEA_RWFeaCurveSectionGeometricRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 583:
    {
      DeclareAndCast(StepFEA_FeaSurfaceSectionGeometricRelationship,anent,ent);
      RWStepFEA_RWFeaSurfaceSectionGeometricRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 600:
    {
      DeclareAndCast(StepBasic_DocumentProductAssociation,anent,ent);
      RWStepBasic_RWDocumentProductAssociation tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 601:
    {
      DeclareAndCast(StepBasic_DocumentProductEquivalence,anent,ent);
      RWStepBasic_RWDocumentProductEquivalence tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 609:
    {
      DeclareAndCast(StepDimTol_CylindricityTolerance,anent,ent);
      RWStepDimTol_RWCylindricityTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 610:
    {
      DeclareAndCast(StepShape_ShapeRepresentationWithParameters,anent,ent);
      RWStepShape_RWShapeRepresentationWithParameters tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 611:
    {
      DeclareAndCast(StepDimTol_AngularityTolerance,anent,ent);
      RWStepDimTol_RWAngularityTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 612:
    {
      DeclareAndCast(StepDimTol_ConcentricityTolerance,anent,ent);
      RWStepDimTol_RWConcentricityTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 613:
    {
      DeclareAndCast(StepDimTol_CircularRunoutTolerance,anent,ent);
      RWStepDimTol_RWCircularRunoutTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 614:
    {
      DeclareAndCast(StepDimTol_CoaxialityTolerance,anent,ent);
      RWStepDimTol_RWCoaxialityTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 615:
    {
      DeclareAndCast(StepDimTol_FlatnessTolerance,anent,ent);
      RWStepDimTol_RWFlatnessTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 616:
    {
      DeclareAndCast(StepDimTol_LineProfileTolerance,anent,ent);
      RWStepDimTol_RWLineProfileTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 617:
    {
      DeclareAndCast(StepDimTol_ParallelismTolerance,anent,ent);
      RWStepDimTol_RWParallelismTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 618:
    {
      DeclareAndCast(StepDimTol_PerpendicularityTolerance,anent,ent);
      RWStepDimTol_RWPerpendicularityTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 619:
    {
      DeclareAndCast(StepDimTol_PositionTolerance,anent,ent);
      RWStepDimTol_RWPositionTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 620:
    {
      DeclareAndCast(StepDimTol_RoundnessTolerance,anent,ent);
      RWStepDimTol_RWRoundnessTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 621:
    {
      DeclareAndCast(StepDimTol_StraightnessTolerance,anent,ent);
      RWStepDimTol_RWStraightnessTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 622:
    {
      DeclareAndCast(StepDimTol_SurfaceProfileTolerance,anent,ent);
      RWStepDimTol_RWSurfaceProfileTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 623:
    {
      DeclareAndCast(StepDimTol_SymmetryTolerance,anent,ent);
      RWStepDimTol_RWSymmetryTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 624:
    {
      DeclareAndCast(StepDimTol_TotalRunoutTolerance,anent,ent);
      RWStepDimTol_RWTotalRunoutTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 625:
    {
      DeclareAndCast(StepDimTol_GeometricTolerance,anent,ent);
      RWStepDimTol_RWGeometricTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 626:
    {
      DeclareAndCast(StepDimTol_GeometricToleranceRelationship,anent,ent);
      RWStepDimTol_RWGeometricToleranceRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 627:
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDatumReference,anent,ent);
      RWStepDimTol_RWGeometricToleranceWithDatumReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 628:
    {
      DeclareAndCast(StepDimTol_ModifiedGeometricTolerance,anent,ent);
      RWStepDimTol_RWModifiedGeometricTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 629:
    {
      DeclareAndCast(StepDimTol_Datum,anent,ent);
      RWStepDimTol_RWDatum tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 630:
    {
      DeclareAndCast(StepDimTol_DatumFeature,anent,ent);
      RWStepDimTol_RWDatumFeature tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 631:
    {
      DeclareAndCast(StepDimTol_DatumReference,anent,ent);
      RWStepDimTol_RWDatumReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 632:
    {
      DeclareAndCast(StepDimTol_CommonDatum,anent,ent);
      RWStepDimTol_RWCommonDatum tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 633:
    {
      DeclareAndCast(StepDimTol_DatumTarget,anent,ent);
      RWStepDimTol_RWDatumTarget tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 634:
    {
      DeclareAndCast(StepDimTol_PlacedDatumTargetFeature,anent,ent);
      RWStepDimTol_RWPlacedDatumTargetFeature tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 635:
    {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnit,anent,ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 636:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 650 : 
    {
      DeclareAndCast(StepBasic_ConversionBasedUnitAndMassUnit, anent, ent);
      RWStepBasic_RWConversionBasedUnitAndMassUnit tool;
      tool.WriteStep (SW,anent);
    }
    
    break;
  case 651 : 
    {
      DeclareAndCast(StepBasic_MassMeasureWithUnit, anent, ent);
      RWStepBasic_RWMassMeasureWithUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 660 :
    {
      DeclareAndCast(StepRepr_Apex, anent, ent);
      RWStepRepr_RWApex tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 661 :
    {
      DeclareAndCast(StepRepr_CentreOfSymmetry, anent, ent);
      RWStepRepr_RWCentreOfSymmetry tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 662 :
    {
      DeclareAndCast(StepRepr_GeometricAlignment, anent, ent);
      RWStepRepr_RWGeometricAlignment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 663 :
    {
      DeclareAndCast(StepRepr_PerpendicularTo, anent, ent);
      RWStepRepr_RWPerpendicularTo tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 664 :
    {
      DeclareAndCast(StepRepr_Tangent, anent, ent);
      RWStepRepr_RWTangent tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 665 :
    {
      DeclareAndCast(StepRepr_ParallelOffset, anent, ent);
      RWStepRepr_RWParallelOffset tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 666 :
    {
      DeclareAndCast(StepAP242_GeometricItemSpecificUsage, anent, ent);
      RWStepAP242_RWGeometricItemSpecificUsage tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 667 :
    {
      DeclareAndCast(StepAP242_IdAttribute, anent, ent);
      RWStepAP242_RWIdAttribute tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 668 :
    {
      DeclareAndCast(StepAP242_ItemIdentifiedRepresentationUsage, anent, ent);
      RWStepAP242_RWItemIdentifiedRepresentationUsage tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 669 :
    {
      DeclareAndCast(StepRepr_AllAroundShapeAspect, anent, ent);
      RWStepRepr_RWAllAroundShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 670 :
    {
      DeclareAndCast(StepRepr_BetweenShapeAspect, anent, ent);
      RWStepRepr_RWBetweenShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 671 :
    {
      DeclareAndCast(StepRepr_CompositeGroupShapeAspect, anent, ent);
      RWStepRepr_RWCompositeGroupShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 672 :
    {
      DeclareAndCast(StepRepr_ContinuosShapeAspect, anent, ent);
      RWStepRepr_RWContinuosShapeAspect tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 673 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedAreaUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 674 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedUnit, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithDefinedUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 675 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithMaximumTolerance, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithMaximumTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 676 :
    {
      DeclareAndCast(StepDimTol_GeometricToleranceWithModifiers, anent, ent);
      RWStepDimTol_RWGeometricToleranceWithModifiers tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 677 :
    {
      DeclareAndCast(StepDimTol_UnequallyDisposedGeometricTolerance, anent, ent);
      RWStepDimTol_RWUnequallyDisposedGeometricTolerance tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 678 :
    {
      DeclareAndCast(StepDimTol_NonUniformZoneDefinition, anent, ent);
      RWStepDimTol_RWNonUniformZoneDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 679 :
    {
      DeclareAndCast(StepDimTol_ProjectedZoneDefinition, anent, ent);
      RWStepDimTol_RWProjectedZoneDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 680 :
    {
      DeclareAndCast(StepDimTol_RunoutZoneDefinition, anent, ent);
      RWStepDimTol_RWRunoutZoneDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 681 :
    {
      DeclareAndCast(StepDimTol_RunoutZoneOrientation, anent, ent);
      RWStepDimTol_RWRunoutZoneOrientation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 682 :
    {
      DeclareAndCast(StepDimTol_ToleranceZone, anent, ent);
      RWStepDimTol_RWToleranceZone tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 683 :
    {
      DeclareAndCast(StepDimTol_ToleranceZoneDefinition, anent, ent);
      RWStepDimTol_RWToleranceZoneDefinition tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 684 :
    {
      DeclareAndCast(StepDimTol_ToleranceZoneForm, anent, ent);
      RWStepDimTol_RWToleranceZoneForm tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 685 :
    {
      DeclareAndCast(StepShape_ValueFormatTypeQualifier, anent, ent);
      RWStepShape_RWValueFormatTypeQualifier tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 686 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceCompartment, anent, ent);
      RWStepDimTol_RWDatumReferenceCompartment tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 687 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceElement, anent, ent);
      RWStepDimTol_RWDatumReferenceElement tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 688 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceModifierWithValue, anent, ent);
      RWStepDimTol_RWDatumReferenceModifierWithValue tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 689 :
    {
      DeclareAndCast(StepDimTol_DatumSystem, anent, ent);
      RWStepDimTol_RWDatumSystem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 690 :
    {
      DeclareAndCast(StepDimTol_GeneralDatumReference, anent, ent);
      RWStepDimTol_RWGeneralDatumReference tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 691:
    {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit,anent,ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnit tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 692:
    {
      DeclareAndCast(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI,anent,ent);
      RWStepRepr_RWReprItemAndLengthMeasureWithUnitAndQRI tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 693:
    {
      DeclareAndCast(StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI,anent,ent);
      RWStepRepr_RWReprItemAndPlaneAngleMeasureWithUnitAndQRI tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 694:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRef,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRef tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 695:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 696:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMod tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 697:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 698:
    {
      DeclareAndCast(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 699:
    {
      DeclareAndCast(StepRepr_CompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompShAspAndDatumFeatAndShAsp tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 700:
    {
      DeclareAndCast(StepRepr_IntegerRepresentationItem,anent,ent);
      RWStepRepr_RWIntegerRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 701:
    {
      DeclareAndCast(StepRepr_ValueRepresentationItem,anent,ent);
      RWStepRepr_RWValueRepresentationItem tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 702:
    {
      DeclareAndCast(StepRepr_FeatureForDatumTargetRelationship,anent,ent);
      RWStepRepr_RWFeatureForDatumTargetRelationship tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 703:
    {
      DeclareAndCast(StepAP242_DraughtingModelItemAssociation,anent,ent);
      RWStepAP242_RWDraughtingModelItemAssociation tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 704:
    {
      DeclareAndCast(StepVisual_AnnotationPlane,anent,ent);
      RWStepVisual_RWAnnotationPlane tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 705:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol tool;
      tool.WriteStep (SW,anent);
    }
    break;
  case 706:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol tool;
      tool.WriteStep (SW,anent);
    }
    break;
    case 707:
    {
      DeclareAndCast(StepVisual_TessellatedAnnotationOccurrence,anent,ent);
      RWStepVisual_RWTessellatedAnnotationOccurrence tool;
      tool.WriteStep (SW,anent);
    }
    break;

     case 708:
    {
      DeclareAndCast(StepVisual_TessellatedItem,anent,ent);
      RWStepVisual_RWTessellatedItem tool;
      tool.WriteStep (SW,anent);
    }
    break;

     case 709:
    {
      DeclareAndCast(StepVisual_TessellatedGeometricSet,anent,ent);
      RWStepVisual_RWTessellatedGeometricSet tool;
      tool.WriteStep (SW,anent);
    }
    break;
     case 710:
    {
      DeclareAndCast(StepVisual_TessellatedCurveSet,anent,ent);
      RWStepVisual_RWTessellatedCurveSet tool;
      tool.WriteStep (SW,anent);
    }
    break;
      case 711:
    {
      DeclareAndCast(StepVisual_CoordinatesList,anent,ent);
      RWStepVisual_RWCoordinatesList tool;
      tool.WriteStep(SW,anent);
    }
    break;
      case 712:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentation,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentation tool;
      tool.WriteStep(SW,anent);
    }
    break;
      case 713:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentationRelationship,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentationRelationship tool;
      tool.WriteStep(SW,anent);
    }
    break;
      case 714:
    {
      DeclareAndCast(StepRepr_CharacterizedRepresentation, anent, ent);
      RWStepRepr_RWCharacterizedRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
      case 715:
    {
      DeclareAndCast(StepVisual_CharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation, anent, ent);
      RWStepVisual_RWCharacterizedObjectAndCharacterizedRepresentationAndDraughtingModelAndRepresentation tool;
      tool.WriteStep(SW, anent);
    }
    break;
      case 716:
    {
      DeclareAndCast(StepVisual_CameraModelD3MultiClipping, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClipping tool;
      tool.WriteStep(SW, anent);
    }
    break;
      case 717:
    {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingIntersection, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingIntersection tool;
      tool.WriteStep(SW, anent);
    }
    break;
      case 718:
    {
      DeclareAndCast(StepVisual_CameraModelD3MultiClippingUnion, anent, ent);
      RWStepVisual_RWCameraModelD3MultiClippingUnion tool;
      tool.WriteStep(SW, anent);
    }
    break;
      case 719:
    {
      DeclareAndCast(StepVisual_AnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem, anent, ent);
      RWStepVisual_RWAnnotationCurveOccurrenceAndAnnotationOccurrenceAndGeomReprItemAndReprItemAndStyledItem tool;
      tool.WriteStep(SW, anent);
    }
      break;

  default: 
    return;
  }
}
