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

//:k4 abv 30.11.98: TR9: warnings for BWV
//:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
//:j4 gka 16.03.99 S4134

#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <RWStepAP203_RWCcDesignApproval.hxx>
#include <RWStepAP203_RWCcDesignCertification.hxx>
#include <RWStepAP203_RWCcDesignContract.hxx>
#include <RWStepAP203_RWCcDesignDateAndTimeAssignment.hxx>
#include <RWStepAP203_RWCcDesignPersonAndOrganizationAssignment.hxx>
#include <RWStepAP203_RWCcDesignSecurityClassification.hxx>
#include <RWStepAP203_RWCcDesignSpecificationReference.hxx>
#include <RWStepAP203_RWChange.hxx>
#include <RWStepAP203_RWChangeRequest.hxx>
#include <RWStepAP203_RWStartRequest.hxx>
#include <RWStepAP203_RWStartWork.hxx>
#include <RWStepAP214.hxx>
#include <RWStepAP214_GeneralModule.hxx>
#include <RWStepAP214_RWAppliedApprovalAssignment.hxx>
#include <RWStepAP214_RWAppliedDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAppliedDateAssignment.hxx>
#include <RWStepAP214_RWAppliedDocumentReference.hxx>
#include <RWStepAP214_RWAppliedExternalIdentificationAssignment.hxx>
#include <RWStepAP214_RWAppliedGroupAssignment.hxx>
#include <RWStepAP214_RWAppliedOrganizationAssignment.hxx>
#include <RWStepAP214_RWAppliedPersonAndOrganizationAssignment.hxx>
#include <RWStepAP214_RWAppliedPresentedItem.hxx>
#include <RWStepAP214_RWAppliedSecurityClassificationAssignment.hxx>
#include <RWStepAP214_RWAutoDesignActualDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAutoDesignActualDateAssignment.hxx>
#include <RWStepAP214_RWAutoDesignApprovalAssignment.hxx>
#include <RWStepAP214_RWAutoDesignDateAndPersonAssignment.hxx>
#include <RWStepAP214_RWAutoDesignDocumentReference.hxx>
#include <RWStepAP214_RWAutoDesignGroupAssignment.hxx>
#include <RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment.hxx>
#include <RWStepAP214_RWAutoDesignNominalDateAssignment.hxx>
#include <RWStepAP214_RWAutoDesignOrganizationAssignment.hxx>
#include <RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment.hxx>
#include <RWStepAP214_RWAutoDesignPresentedItem.hxx>
#include <RWStepAP214_RWAutoDesignSecurityClassificationAssignment.hxx>
#include <RWStepAP214_RWClass.hxx>
#include <RWStepAP214_RWExternallyDefinedClass.hxx>
#include <RWStepAP214_RWExternallyDefinedGeneralProperty.hxx>
#include <RWStepAP214_RWRepItemGroup.hxx>
#include <RWStepBasic_RWAction.hxx>
#include <RWStepBasic_RWActionAssignment.hxx>
#include <RWStepBasic_RWActionMethod.hxx>
#include <RWStepBasic_RWActionRequestAssignment.hxx>
#include <RWStepBasic_RWActionRequestSolution.hxx>
#include <RWStepBasic_RWAddress.hxx>
#include <RWStepBasic_RWApplicationContext.hxx>
#include <RWStepBasic_RWApplicationContextElement.hxx>
#include <RWStepBasic_RWApplicationProtocolDefinition.hxx>
#include <RWStepBasic_RWApproval.hxx>
#include <RWStepBasic_RWApprovalDateTime.hxx>
#include <RWStepBasic_RWApprovalPersonOrganization.hxx>
#include <RWStepBasic_RWApprovalRelationship.hxx>
#include <RWStepBasic_RWApprovalRole.hxx>
#include <RWStepBasic_RWApprovalStatus.hxx>
#include <RWStepBasic_RWCalendarDate.hxx>
#include <RWStepBasic_RWCertification.hxx>
#include <RWStepBasic_RWCertificationAssignment.hxx>
#include <RWStepBasic_RWCertificationType.hxx>
#include <RWStepBasic_RWCharacterizedObject.hxx>
#include <RWStepBasic_RWContract.hxx>
#include <RWStepBasic_RWContractAssignment.hxx>
#include <RWStepBasic_RWContractType.hxx>
#include <RWStepBasic_RWConversionBasedUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndAreaUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndLengthUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndMassUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndRatioUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndTimeUnit.hxx>
#include <RWStepBasic_RWConversionBasedUnitAndVolumeUnit.hxx>
#include <RWStepBasic_RWCoordinatedUniversalTimeOffset.hxx>
#include <RWStepBasic_RWDate.hxx>
#include <RWStepBasic_RWDateAndTime.hxx>
#include <RWStepBasic_RWDateRole.hxx>
#include <RWStepBasic_RWDateTimeRole.hxx>
#include <RWStepBasic_RWDerivedUnit.hxx>
#include <RWStepBasic_RWDerivedUnitElement.hxx>
#include <RWStepBasic_RWDimensionalExponents.hxx>
#include <RWStepBasic_RWDocument.hxx>
#include <RWStepBasic_RWDocumentFile.hxx>
#include <RWStepBasic_RWDocumentProductAssociation.hxx>
#include <RWStepBasic_RWDocumentProductEquivalence.hxx>
#include <RWStepBasic_RWDocumentRelationship.hxx>
#include <RWStepBasic_RWDocumentRepresentationType.hxx>
#include <RWStepBasic_RWDocumentType.hxx>
#include <RWStepBasic_RWDocumentUsageConstraint.hxx>
#include <RWStepBasic_RWEffectivity.hxx>
#include <RWStepBasic_RWEffectivityAssignment.hxx>
#include <RWStepBasic_RWEulerAngles.hxx>
#include <RWStepBasic_RWExternalIdentificationAssignment.hxx>
#include <RWStepBasic_RWExternallyDefinedItem.hxx>
#include <RWStepBasic_RWExternalSource.hxx>
#include <RWStepBasic_RWGeneralProperty.hxx>
#include <RWStepBasic_RWGroup.hxx>
#include <RWStepBasic_RWGroupRelationship.hxx>
#include <RWStepBasic_RWIdentificationAssignment.hxx>
#include <RWStepBasic_RWIdentificationRole.hxx>
#include <RWStepBasic_RWLengthMeasureWithUnit.hxx>
#include <RWStepBasic_RWLengthUnit.hxx>
#include <RWStepBasic_RWLocalTime.hxx>
#include <RWStepBasic_RWMassMeasureWithUnit.hxx>
#include <RWStepBasic_RWMassUnit.hxx>
#include <RWStepBasic_RWMeasureWithUnit.hxx>
#include <RWStepBasic_RWMechanicalContext.hxx>
#include <RWStepBasic_RWNameAssignment.hxx>
#include <RWStepBasic_RWNamedUnit.hxx>
#include <RWStepBasic_RWObjectRole.hxx>
#include <RWStepBasic_RWOrdinalDate.hxx>
#include <RWStepBasic_RWOrganization.hxx>
#include <RWStepBasic_RWOrganizationalAddress.hxx>
#include <RWStepBasic_RWOrganizationRole.hxx>
#include <RWStepBasic_RWPerson.hxx>
#include <RWStepBasic_RWPersonalAddress.hxx>
#include <RWStepBasic_RWPersonAndOrganization.hxx>
#include <RWStepBasic_RWPersonAndOrganizationRole.hxx>
#include <RWStepBasic_RWPlaneAngleMeasureWithUnit.hxx>
#include <RWStepBasic_RWPlaneAngleUnit.hxx>
#include <RWStepBasic_RWProduct.hxx>
#include <RWStepBasic_RWProductCategory.hxx>
#include <RWStepBasic_RWProductCategoryRelationship.hxx>
#include <RWStepBasic_RWProductConceptContext.hxx>
#include <RWStepBasic_RWProductContext.hxx>
#include <RWStepBasic_RWProductDefinition.hxx>
#include <RWStepBasic_RWProductDefinitionContext.hxx>
#include <RWStepBasic_RWProductDefinitionEffectivity.hxx>
#include <RWStepBasic_RWProductDefinitionFormation.hxx>
#include <RWStepBasic_RWProductDefinitionFormationRelationship.hxx>
#include <RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource.hxx>
#include <RWStepBasic_RWProductDefinitionRelationship.hxx>
#include <RWStepBasic_RWProductDefinitionWithAssociatedDocuments.hxx>
#include <RWStepBasic_RWProductRelatedProductCategory.hxx>
#include <RWStepBasic_RWProductType.hxx>
#include <RWStepBasic_RWRatioMeasureWithUnit.hxx>
#include <RWStepBasic_RWRoleAssociation.hxx>
#include <RWStepBasic_RWSecurityClassification.hxx>
#include <RWStepBasic_RWSecurityClassificationLevel.hxx>
#include <RWStepBasic_RWSiUnit.hxx>
#include <RWStepBasic_RWSiUnitAndLengthUnit.hxx>
#include <RWStepBasic_RWSiUnitAndMassUnit.hxx>
#include <RWStepBasic_RWSiUnitAndPlaneAngleUnit.hxx>
#include <RWStepBasic_RWSiUnitAndRatioUnit.hxx>
#include <RWStepBasic_RWSiUnitAndSolidAngleUnit.hxx>
#include <RWStepBasic_RWSiUnitAndThermodynamicTemperatureUnit.hxx>
#include <RWStepBasic_RWSiUnitAndTimeUnit.hxx>
#include <RWStepBasic_RWSolidAngleMeasureWithUnit.hxx>
#include <RWStepBasic_RWSolidAngleUnit.hxx>
#include <RWStepBasic_RWThermodynamicTemperatureUnit.hxx>
#include <RWStepBasic_RWUncertaintyMeasureWithUnit.hxx>
#include <RWStepBasic_RWVersionedActionRequest.hxx>
#include <RWStepBasic_RWWeekOfYearAndDayDate.hxx>
#include <RWStepDimTol_RWAngularityTolerance.hxx>
#include <RWStepDimTol_RWCircularRunoutTolerance.hxx>
#include <RWStepDimTol_RWCoaxialityTolerance.hxx>
#include <RWStepDimTol_RWCommonDatum.hxx>
#include <RWStepDimTol_RWConcentricityTolerance.hxx>
#include <RWStepDimTol_RWCylindricityTolerance.hxx>
#include <RWStepDimTol_RWDatum.hxx>
#include <RWStepDimTol_RWDatumFeature.hxx>
#include <RWStepDimTol_RWDatumReference.hxx>
#include <RWStepDimTol_RWDatumTarget.hxx>
#include <RWStepDimTol_RWFlatnessTolerance.hxx>
#include <RWStepDimTol_RWGeometricTolerance.hxx>
#include <RWStepDimTol_RWGeometricToleranceRelationship.hxx>
#include <RWStepDimTol_RWGeometricToleranceWithDatumReference.hxx>
#include <RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <RWStepDimTol_RWLineProfileTolerance.hxx>
#include <RWStepDimTol_RWModifiedGeometricTolerance.hxx>
#include <RWStepDimTol_RWParallelismTolerance.hxx>
#include <RWStepDimTol_RWPerpendicularityTolerance.hxx>
#include <RWStepDimTol_RWPlacedDatumTargetFeature.hxx>
#include <RWStepDimTol_RWPositionTolerance.hxx>
#include <RWStepDimTol_RWRoundnessTolerance.hxx>
#include <RWStepDimTol_RWStraightnessTolerance.hxx>
#include <RWStepDimTol_RWSurfaceProfileTolerance.hxx>
#include <RWStepDimTol_RWSymmetryTolerance.hxx>
#include <RWStepDimTol_RWTotalRunoutTolerance.hxx>
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
#include <RWStepFEA_RWAlignedSurface3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWConstantSurface3dElementCoordinateSystem.hxx>
#include <RWStepFEA_RWCurve3dElementProperty.hxx>
#include <RWStepFEA_RWCurve3dElementRepresentation.hxx>
#include <RWStepFEA_RWCurveElementEndOffset.hxx>
#include <RWStepFEA_RWCurveElementEndRelease.hxx>
#include <RWStepFEA_RWCurveElementInterval.hxx>
#include <RWStepFEA_RWCurveElementIntervalConstant.hxx>
#include <RWStepFEA_RWCurveElementIntervalLinearlyVarying.hxx>
#include <RWStepFEA_RWCurveElementLocation.hxx>
#include <RWStepFEA_RWDummyNode.hxx>
#include <RWStepFEA_RWElementGeometricRelationship.hxx>
#include <RWStepFEA_RWElementGroup.hxx>
#include <RWStepFEA_RWElementRepresentation.hxx>
#include <RWStepFEA_RWFeaAreaDensity.hxx>
#include <RWStepFEA_RWFeaAxis2Placement3d.hxx>
#include <RWStepFEA_RWFeaCurveSectionGeometricRelationship.hxx>
#include <RWStepFEA_RWFeaGroup.hxx>
#include <RWStepFEA_RWFeaLinearElasticity.hxx>
#include <RWStepFEA_RWFeaMassDensity.hxx>
#include <RWStepFEA_RWFeaMaterialPropertyRepresentation.hxx>
#include <RWStepFEA_RWFeaMaterialPropertyRepresentationItem.hxx>
#include <RWStepFEA_RWFeaModel.hxx>
#include <RWStepFEA_RWFeaModel3d.hxx>
#include <RWStepFEA_RWFeaModelDefinition.hxx>
#include <RWStepFEA_RWFeaMoistureAbsorption.hxx>
#include <RWStepFEA_RWFeaParametricPoint.hxx>
#include <RWStepFEA_RWFeaRepresentationItem.hxx>
#include <RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion.hxx>
#include <RWStepFEA_RWFeaShellBendingStiffness.hxx>
#include <RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness.hxx>
#include <RWStepFEA_RWFeaShellMembraneStiffness.hxx>
#include <RWStepFEA_RWFeaShellShearStiffness.hxx>
#include <RWStepFEA_RWFeaSurfaceSectionGeometricRelationship.hxx>
#include <RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion.hxx>
#include <RWStepFEA_RWFreedomAndCoefficient.hxx>
#include <RWStepFEA_RWFreedomsList.hxx>
#include <RWStepFEA_RWGeometricNode.hxx>
#include <RWStepFEA_RWNode.hxx>
#include <RWStepFEA_RWNodeDefinition.hxx>
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
#include <RWStepGeom_RWAxis1Placement.hxx>
#include <RWStepGeom_RWAxis2Placement2d.hxx>
#include <RWStepGeom_RWAxis2Placement3d.hxx>
#include <RWStepGeom_RWBezierCurve.hxx>
#include <RWStepGeom_RWBezierCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWBezierSurface.hxx>
#include <RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWBoundaryCurve.hxx>
#include <RWStepGeom_RWBoundedCurve.hxx>
#include <RWStepGeom_RWBoundedSurface.hxx>
#include <RWStepGeom_RWBSplineCurve.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnots.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWBSplineSurface.hxx>
#include <RWStepGeom_RWBSplineSurfaceWithKnots.hxx>
#include <RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWCartesianPoint.hxx>
#include <RWStepGeom_RWCartesianTransformationOperator.hxx>
#include <RWStepGeom_RWCartesianTransformationOperator3d.hxx>
#include <RWStepGeom_RWCircle.hxx>
#include <RWStepGeom_RWCompositeCurve.hxx>
#include <RWStepGeom_RWCompositeCurveOnSurface.hxx>
#include <RWStepGeom_RWCompositeCurveSegment.hxx>
#include <RWStepGeom_RWConic.hxx>
#include <RWStepGeom_RWConicalSurface.hxx>
#include <RWStepGeom_RWCurve.hxx>
#include <RWStepGeom_RWCurveBoundedSurface.hxx>
#include <RWStepGeom_RWCurveReplica.hxx>
#include <RWStepGeom_RWCylindricalSurface.hxx>
#include <RWStepGeom_RWDegeneratePcurve.hxx>
#include <RWStepGeom_RWDegenerateToroidalSurface.hxx>
#include <RWStepGeom_RWDirection.hxx>
#include <RWStepGeom_RWElementarySurface.hxx>
#include <RWStepGeom_RWEllipse.hxx>
#include <RWStepGeom_RWEvaluatedDegeneratePcurve.hxx>
#include <RWStepGeom_RWGeometricRepresentationContext.hxx>
#include <RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <RWStepGeom_RWGeometricRepresentationItem.hxx>
#include <RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <RWStepGeom_RWHyperbola.hxx>
#include <RWStepGeom_RWIntersectionCurve.hxx>
#include <RWStepGeom_RWLine.hxx>
#include <RWStepGeom_RWOffsetCurve3d.hxx>
#include <RWStepGeom_RWOffsetSurface.hxx>
#include <RWStepGeom_RWOrientedSurface.hxx>
#include <RWStepGeom_RWOuterBoundaryCurve.hxx>
#include <RWStepGeom_RWParabola.hxx>
#include <RWStepGeom_RWPcurve.hxx>
#include <RWStepGeom_RWPlacement.hxx>
#include <RWStepGeom_RWPlane.hxx>
#include <RWStepGeom_RWPoint.hxx>
#include <RWStepGeom_RWPointOnCurve.hxx>
#include <RWStepGeom_RWPointOnSurface.hxx>
#include <RWStepGeom_RWPointReplica.hxx>
#include <RWStepGeom_RWPolyline.hxx>
#include <RWStepGeom_RWQuasiUniformCurve.hxx>
#include <RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWQuasiUniformSurface.hxx>
#include <RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWRationalBSplineCurve.hxx>
#include <RWStepGeom_RWRationalBSplineSurface.hxx>
#include <RWStepGeom_RWRectangularCompositeSurface.hxx>
#include <RWStepGeom_RWRectangularTrimmedSurface.hxx>
#include <RWStepGeom_RWReparametrisedCompositeCurveSegment.hxx>
#include <RWStepGeom_RWSeamCurve.hxx>
#include <RWStepGeom_RWSphericalSurface.hxx>
#include <RWStepGeom_RWSurface.hxx>
#include <RWStepGeom_RWSurfaceCurve.hxx>
#include <RWStepGeom_RWSurfaceCurveAndBoundedCurve.hxx>
#include <RWStepGeom_RWSurfaceOfLinearExtrusion.hxx>
#include <RWStepGeom_RWSurfaceOfRevolution.hxx>
#include <RWStepGeom_RWSurfacePatch.hxx>
#include <RWStepGeom_RWSurfaceReplica.hxx>
#include <RWStepGeom_RWSweptSurface.hxx>
#include <RWStepGeom_RWToroidalSurface.hxx>
#include <RWStepGeom_RWTrimmedCurve.hxx>
#include <RWStepGeom_RWUniformCurve.hxx>
#include <RWStepGeom_RWUniformCurveAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWUniformSurface.hxx>
#include <RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWVector.hxx>
#include <RWStepRepr_RWAssemblyComponentUsage.hxx>
#include <RWStepRepr_RWAssemblyComponentUsageSubstitute.hxx>
#include <RWStepRepr_RWCompositeShapeAspect.hxx>
#include <RWStepRepr_RWCompoundRepresentationItem.hxx>
#include <RWStepRepr_RWConfigurationDesign.hxx>
#include <RWStepRepr_RWConfigurationEffectivity.hxx>
#include <RWStepRepr_RWConstructiveGeometryRepresentation.hxx>
#include <RWStepRepr_RWConstructiveGeometryRepresentationRelationship.hxx>
#include <RWStepRepr_RWDataEnvironment.hxx>
#include <RWStepRepr_RWDefinitionalRepresentation.hxx>
#include <RWStepRepr_RWDerivedShapeAspect.hxx>
#include <RWStepRepr_RWDescriptiveRepresentationItem.hxx>
#include <RWStepRepr_RWExtension.hxx>
#include <RWStepRepr_RWFunctionallyDefinedTransformation.hxx>
#include <RWStepRepr_RWGlobalUncertaintyAssignedContext.hxx>
#include <RWStepRepr_RWGlobalUnitAssignedContext.hxx>
#include <RWStepRepr_RWItemDefinedTransformation.hxx>
#include <RWStepRepr_RWMakeFromUsageOption.hxx>
#include <RWStepRepr_RWMappedItem.hxx>
#include <RWStepRepr_RWMaterialDesignation.hxx>
#include <RWStepRepr_RWMaterialProperty.hxx>
#include <RWStepRepr_RWMaterialPropertyRepresentation.hxx>
#include <RWStepRepr_RWMeasureRepresentationItem.hxx>
#include <RWStepRepr_RWParametricRepresentationContext.hxx>
#include <RWStepRepr_RWProductConcept.hxx>
#include <RWStepRepr_RWProductDefinitionShape.hxx>
#include <RWStepRepr_RWPropertyDefinition.hxx>
#include <RWStepRepr_RWPropertyDefinitionRelationship.hxx>
#include <RWStepRepr_RWPropertyDefinitionRepresentation.hxx>
#include <RWStepRepr_RWQuantifiedAssemblyComponentUsage.hxx>
#include <RWStepRepr_RWRepresentation.hxx>
#include <RWStepRepr_RWRepresentationContext.hxx>
#include <RWStepRepr_RWRepresentationItem.hxx>
#include <RWStepRepr_RWRepresentationMap.hxx>
#include <RWStepRepr_RWRepresentationRelationship.hxx>
#include <RWStepRepr_RWRepresentationRelationshipWithTransformation.hxx>
#include <RWStepRepr_RWReprItemAndLengthMeasureWithUnit.hxx>
#include <RWStepRepr_RWShapeAspect.hxx>
#include <RWStepRepr_RWShapeAspectRelationship.hxx>
#include <RWStepRepr_RWFeatureForDatumTargetRelationship.hxx>
#include <RWStepRepr_RWShapeAspectTransition.hxx>
#include <RWStepRepr_RWShapeRepresentationRelationshipWithTransformation.hxx>
#include <RWStepRepr_RWSpecifiedHigherUsageOccurrence.hxx>
#include <RWStepRepr_RWStructuralResponseProperty.hxx>
#include <RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation.hxx>
#include <RWStepShape_RWAdvancedBrepShapeRepresentation.hxx>
#include <RWStepShape_RWAdvancedFace.hxx>
#include <RWStepShape_RWAngularLocation.hxx>
#include <RWStepShape_RWAngularSize.hxx>
#include <RWStepShape_RWBlock.hxx>
#include <RWStepShape_RWBooleanResult.hxx>
#include <RWStepShape_RWBoxDomain.hxx>
#include <RWStepShape_RWBoxedHalfSpace.hxx>
#include <RWStepShape_RWBrepWithVoids.hxx>
#include <RWStepShape_RWClosedShell.hxx>
#include <RWStepShape_RWCompoundShapeRepresentation.hxx>
#include <RWStepShape_RWConnectedEdgeSet.hxx>
#include <RWStepShape_RWConnectedFaceSet.hxx>
#include <RWStepShape_RWConnectedFaceShapeRepresentation.hxx>
#include <RWStepShape_RWConnectedFaceSubSet.hxx>
#include <RWStepShape_RWContextDependentShapeRepresentation.hxx>
#include <RWStepShape_RWCsgShapeRepresentation.hxx>
#include <RWStepShape_RWCsgSolid.hxx>
#include <RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation.hxx>
#include <RWStepShape_RWDimensionalCharacteristicRepresentation.hxx>
#include <RWStepShape_RWDimensionalLocation.hxx>
#include <RWStepShape_RWDimensionalLocationWithPath.hxx>
#include <RWStepShape_RWDimensionalSize.hxx>
#include <RWStepShape_RWDimensionalSizeWithPath.hxx>
#include <RWStepShape_RWEdge.hxx>
#include <RWStepShape_RWEdgeBasedWireframeModel.hxx>
#include <RWStepShape_RWEdgeBasedWireframeShapeRepresentation.hxx>
#include <RWStepShape_RWEdgeCurve.hxx>
#include <RWStepShape_RWEdgeLoop.hxx>
#include <RWStepShape_RWExtrudedAreaSolid.hxx>
#include <RWStepShape_RWExtrudedFaceSolid.hxx>
#include <RWStepShape_RWFace.hxx>
#include <RWStepShape_RWFaceBasedSurfaceModel.hxx>
#include <RWStepShape_RWFaceBound.hxx>
#include <RWStepShape_RWFaceOuterBound.hxx>
#include <RWStepShape_RWFaceSurface.hxx>
#include <RWStepShape_RWFacetedBrep.hxx>
#include <RWStepShape_RWFacetedBrepAndBrepWithVoids.hxx>
#include <RWStepShape_RWFacetedBrepShapeRepresentation.hxx>
#include <RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <RWStepShape_RWGeometricCurveSet.hxx>
#include <RWStepShape_RWGeometricSet.hxx>
#include <RWStepShape_RWHalfSpaceSolid.hxx>
#include <RWStepShape_RWLimitsAndFits.hxx>
#include <RWStepShape_RWLoop.hxx>
#include <RWStepShape_RWLoopAndPath.hxx>
#include <RWStepShape_RWManifoldSolidBrep.hxx>
#include <RWStepShape_RWManifoldSurfaceShapeRepresentation.hxx>
#include <RWStepShape_RWMeasureQualification.hxx>
#include <RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <RWStepShape_RWNonManifoldSurfaceShapeRepresentation.hxx>
#include <RWStepShape_RWOpenShell.hxx>
#include <RWStepShape_RWOrientedClosedShell.hxx>
#include <RWStepShape_RWOrientedEdge.hxx>
#include <RWStepShape_RWOrientedFace.hxx>
#include <RWStepShape_RWOrientedOpenShell.hxx>
#include <RWStepShape_RWOrientedPath.hxx>
#include <RWStepShape_RWPath.hxx>
#include <RWStepShape_RWPlusMinusTolerance.hxx>
#include <RWStepShape_RWPointRepresentation.hxx>
#include <RWStepShape_RWPolyLoop.hxx>
#include <RWStepShape_RWPrecisionQualifier.hxx>
#include <RWStepShape_RWQualifiedRepresentationItem.hxx>
#include <RWStepShape_RWRevolvedAreaSolid.hxx>
#include <RWStepShape_RWRevolvedFaceSolid.hxx>
#include <RWStepShape_RWRightAngularWedge.hxx>
#include <RWStepShape_RWRightCircularCone.hxx>
#include <RWStepShape_RWRightCircularCylinder.hxx>
#include <RWStepShape_RWSeamEdge.hxx>
#include <RWStepShape_RWShapeDefinitionRepresentation.hxx>
#include <RWStepShape_RWShapeDimensionRepresentation.hxx>
#include <RWStepShape_RWShapeRepresentation.hxx>
#include <RWStepShape_RWShapeRepresentationWithParameters.hxx>
#include <RWStepShape_RWShellBasedSurfaceModel.hxx>
#include <RWStepShape_RWSolidModel.hxx>
#include <RWStepShape_RWSolidReplica.hxx>
#include <RWStepShape_RWSphere.hxx>
#include <RWStepShape_RWSubedge.hxx>
#include <RWStepShape_RWSubface.hxx>
#include <RWStepShape_RWSweptAreaSolid.hxx>
#include <RWStepShape_RWSweptFaceSolid.hxx>
#include <RWStepShape_RWToleranceValue.hxx>
#include <RWStepShape_RWTopologicalRepresentationItem.hxx>
#include <RWStepShape_RWTorus.hxx>
#include <RWStepShape_RWTransitionalShapeRepresentation.hxx>
#include <RWStepShape_RWTypeQualifier.hxx>
#include <RWStepShape_RWVertex.hxx>
#include <RWStepShape_RWVertexLoop.hxx>
#include <RWStepShape_RWVertexPoint.hxx>
#include <RWStepVisual_RWAreaInSet.hxx>
#include <RWStepVisual_RWBackgroundColour.hxx>
#include <RWStepVisual_RWCameraImage.hxx>
#include <RWStepVisual_RWCameraModel.hxx>
#include <RWStepVisual_RWCameraModelD2.hxx>
#include <RWStepVisual_RWCameraModelD3.hxx>
#include <RWStepVisual_RWCameraUsage.hxx>
#include <RWStepVisual_RWColour.hxx>
#include <RWStepVisual_RWColourRgb.hxx>
#include <RWStepVisual_RWColourSpecification.hxx>
#include <RWStepVisual_RWCompositeText.hxx>
#include <RWStepVisual_RWCompositeTextWithExtent.hxx>
#include <RWStepVisual_RWContextDependentInvisibility.hxx>
#include <RWStepVisual_RWContextDependentOverRidingStyledItem.hxx>
#include <RWStepVisual_RWCurveStyle.hxx>
#include <RWStepVisual_RWCurveStyleFont.hxx>
#include <RWStepVisual_RWCurveStyleFontPattern.hxx>
#include <RWStepVisual_RWDraughtingModel.hxx>
#include <RWStepVisual_RWDraughtingPreDefinedColour.hxx>
#include <RWStepVisual_RWDraughtingPreDefinedCurveFont.hxx>
#include <RWStepVisual_RWExternallyDefinedCurveFont.hxx>
#include <RWStepVisual_RWFillAreaStyle.hxx>
#include <RWStepVisual_RWFillAreaStyleColour.hxx>
#include <RWStepVisual_RWInvisibility.hxx>
#include <RWStepVisual_RWMechanicalDesignGeometricPresentationArea.hxx>
#include <RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation.hxx>
#include <RWStepVisual_RWOverRidingStyledItem.hxx>
#include <RWStepVisual_RWPlanarBox.hxx>
#include <RWStepVisual_RWPlanarExtent.hxx>
#include <RWStepVisual_RWPointStyle.hxx>
#include <RWStepVisual_RWPreDefinedColour.hxx>
#include <RWStepVisual_RWPreDefinedCurveFont.hxx>
#include <RWStepVisual_RWPreDefinedItem.hxx>
#include <RWStepVisual_RWPresentationArea.hxx>
#include <RWStepVisual_RWPresentationLayerAssignment.hxx>
#include <RWStepVisual_RWPresentationLayerUsage.hxx>
#include <RWStepVisual_RWPresentationRepresentation.hxx>
#include <RWStepVisual_RWPresentationSet.hxx>
#include <RWStepVisual_RWPresentationSize.hxx>
#include <RWStepVisual_RWPresentationStyleAssignment.hxx>
#include <RWStepVisual_RWPresentationStyleByContext.hxx>
#include <RWStepVisual_RWPresentationView.hxx>
#include <RWStepVisual_RWPresentedItemRepresentation.hxx>
#include <RWStepVisual_RWStyledItem.hxx>
#include <RWStepVisual_RWSurfaceSideStyle.hxx>
#include <RWStepVisual_RWSurfaceStyleBoundary.hxx>
#include <RWStepVisual_RWSurfaceStyleControlGrid.hxx>
#include <RWStepVisual_RWSurfaceStyleFillArea.hxx>
#include <RWStepVisual_RWSurfaceStyleParameterLine.hxx>
#include <RWStepVisual_RWSurfaceStyleReflectanceAmbient.hxx>
#include <RWStepVisual_RWSurfaceStyleRendering.hxx>
#include <RWStepVisual_RWSurfaceStyleRenderingWithProperties.hxx>
#include <RWStepVisual_RWSurfaceStyleSegmentationCurve.hxx>
#include <RWStepVisual_RWSurfaceStyleSilhouette.hxx>
#include <RWStepVisual_RWSurfaceStyleTransparent.hxx>
#include <RWStepVisual_RWSurfaceStyleUsage.hxx>
#include <RWStepVisual_RWTemplate.hxx>
#include <RWStepVisual_RWTemplateInstance.hxx>
#include <RWStepVisual_RWTextLiteral.hxx>
#include <RWStepVisual_RWTextStyle.hxx>
#include <RWStepVisual_RWTextStyleForDefinedFont.hxx>
#include <RWStepVisual_RWTextStyleWithBoxCharacteristics.hxx>
#include <RWStepVisual_RWViewVolume.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepAP203_CcDesignApproval.hxx>
#include <StepAP203_CcDesignCertification.hxx>
#include <StepAP203_CcDesignContract.hxx>
#include <StepAP203_CcDesignDateAndTimeAssignment.hxx>
#include <StepAP203_CcDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepAP203_CcDesignSpecificationReference.hxx>
#include <StepAP203_Change.hxx>
#include <StepAP203_ChangeRequest.hxx>
#include <StepAP203_StartRequest.hxx>
#include <StepAP203_StartWork.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_AppliedDateAndTimeAssignment.hxx>
#include <StepAP214_AppliedDateAssignment.hxx>
#include <StepAP214_AppliedDocumentReference.hxx>
#include <StepAP214_AppliedExternalIdentificationAssignment.hxx>
#include <StepAP214_AppliedGroupAssignment.hxx>
#include <StepAP214_AppliedOrganizationAssignment.hxx>
#include <StepAP214_AppliedPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AppliedPresentedItem.hxx>
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepAP214_AutoDesignActualDateAndTimeAssignment.hxx>
#include <StepAP214_AutoDesignActualDateAssignment.hxx>
#include <StepAP214_AutoDesignApprovalAssignment.hxx>
#include <StepAP214_AutoDesignDateAndPersonAssignment.hxx>
#include <StepAP214_AutoDesignDocumentReference.hxx>
#include <StepAP214_AutoDesignGroupAssignment.hxx>
#include <StepAP214_AutoDesignNominalDateAndTimeAssignment.hxx>
#include <StepAP214_AutoDesignNominalDateAssignment.hxx>
#include <StepAP214_AutoDesignOrganizationAssignment.hxx>
#include <StepAP214_AutoDesignPersonAndOrganizationAssignment.hxx>
#include <StepAP214_AutoDesignPresentedItem.hxx>
#include <StepAP214_AutoDesignSecurityClassificationAssignment.hxx>
#include <StepAP214_Class.hxx>
#include <StepAP214_ExternallyDefinedClass.hxx>
#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepAP214_RepItemGroup.hxx>
#include <StepBasic_Action.hxx>
#include <StepBasic_ActionAssignment.hxx>
#include <StepBasic_ActionMethod.hxx>
#include <StepBasic_ActionRequestAssignment.hxx>
#include <StepBasic_ActionRequestSolution.hxx>
#include <StepBasic_Address.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationContextElement.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalDateTime.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ApprovalRelationship.hxx>
#include <StepBasic_ApprovalRole.hxx>
#include <StepBasic_ApprovalStatus.hxx>
#include <StepBasic_AreaUnit.hxx>
#include <StepBasic_CalendarDate.hxx>
#include <StepBasic_Certification.hxx>
#include <StepBasic_CertificationAssignment.hxx>
#include <StepBasic_CertificationType.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepBasic_Contract.hxx>
#include <StepBasic_ContractAssignment.hxx>
#include <StepBasic_ContractType.hxx>
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndAreaUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndRatioUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndSolidAngleUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndTimeUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndVolumeUnit.hxx>
#include <StepBasic_CoordinatedUniversalTimeOffset.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_DateRole.hxx>
#include <StepBasic_DateTimeRole.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <StepBasic_DesignContext.hxx>
#include <StepBasic_DigitalDocument.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepBasic_DocumentRelationship.hxx>
#include <StepBasic_DocumentRepresentationType.hxx>
#include <StepBasic_DocumentType.hxx>
#include <StepBasic_DocumentUsageConstraint.hxx>
#include <StepBasic_Effectivity.hxx>
#include <StepBasic_EffectivityAssignment.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepBasic_ExternalIdentificationAssignment.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <StepBasic_IdentificationAssignment.hxx>
#include <StepBasic_IdentificationRole.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepBasic_LengthUnit.hxx>
#include <StepBasic_LocalTime.hxx>
#include <StepBasic_MassMeasureWithUnit.hxx>
#include <StepBasic_MassUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_MechanicalContext.hxx>
#include <StepBasic_NameAssignment.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_ObjectRole.hxx>
#include <StepBasic_OrdinalDate.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_OrganizationalAddress.hxx>
#include <StepBasic_OrganizationRole.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonalAddress.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepBasic_PersonAndOrganizationRole.hxx>
#include <StepBasic_PhysicallyModeledProductDefinition.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepBasic_PlaneAngleUnit.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <StepBasic_ProductConceptContext.hxx>
#include <StepBasic_ProductContext.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepBasic_ProductDefinitionEffectivity.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <StepBasic_ProductDefinitionFormationWithSpecifiedSource.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepBasic_ProductDefinitionWithAssociatedDocuments.hxx>
#include <StepBasic_ProductRelatedProductCategory.hxx>
#include <StepBasic_ProductType.hxx>
#include <StepBasic_RatioMeasureWithUnit.hxx>
#include <StepBasic_RatioUnit.hxx>
#include <StepBasic_RoleAssociation.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepBasic_SecurityClassificationLevel.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>
#include <StepBasic_SiUnitAndMassUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndRatioUnit.hxx>
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepBasic_SiUnitAndTimeUnit.hxx>
#include <StepBasic_SiUnitAndVolumeUnit.hxx>
#include <StepBasic_SolidAngleMeasureWithUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>
#include <StepBasic_TimeMeasureWithUnit.hxx>
#include <StepBasic_TimeUnit.hxx>
#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <StepBasic_VersionedActionRequest.hxx>
#include <StepBasic_VolumeUnit.hxx>
#include <StepBasic_WeekOfYearAndDayDate.hxx>
#include <StepDimTol_AngularityTolerance.hxx>
#include <StepDimTol_CircularRunoutTolerance.hxx>
#include <StepDimTol_CoaxialityTolerance.hxx>
#include <StepDimTol_CommonDatum.hxx>
#include <StepDimTol_ConcentricityTolerance.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumFeature.hxx>
#include <StepDimTol_DatumReference.hxx>
#include <StepDimTol_DatumTarget.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_GeometricToleranceRelationship.hxx>
#include <StepDimTol_GeometricToleranceWithDatumReference.hxx>
#include <StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_ModifiedGeometricTolerance.hxx>
#include <StepDimTol_ParallelismTolerance.hxx>
#include <StepDimTol_PerpendicularityTolerance.hxx>
#include <StepDimTol_PlacedDatumTargetFeature.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepDimTol_SymmetryTolerance.hxx>
#include <StepDimTol_TotalRunoutTolerance.hxx>
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
#include <StepFEA_AlignedSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_ArbitraryVolume3dElementCoordinateSystem.hxx>
#include <StepFEA_ConstantSurface3dElementCoordinateSystem.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <StepFEA_Curve3dElementRepresentation.hxx>
#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
#include <StepFEA_CurveElementEndOffset.hxx>
#include <StepFEA_CurveElementEndRelease.hxx>
#include <StepFEA_CurveElementInterval.hxx>
#include <StepFEA_CurveElementIntervalConstant.hxx>
#include <StepFEA_CurveElementIntervalLinearlyVarying.hxx>
#include <StepFEA_CurveElementLocation.hxx>
#include <StepFEA_DummyNode.hxx>
#include <StepFEA_ElementGeometricRelationship.hxx>
#include <StepFEA_ElementGroup.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepFEA_FeaAreaDensity.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>
#include <StepFEA_FeaCurveSectionGeometricRelationship.hxx>
#include <StepFEA_FeaGroup.hxx>
#include <StepFEA_FeaLinearElasticity.hxx>
#include <StepFEA_FeaMassDensity.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentation.hxx>
#include <StepFEA_FeaMaterialPropertyRepresentationItem.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepFEA_FeaModelDefinition.hxx>
#include <StepFEA_FeaMoistureAbsorption.hxx>
#include <StepFEA_FeaParametricPoint.hxx>
#include <StepFEA_FeaRepresentationItem.hxx>
#include <StepFEA_FeaSecantCoefficientOfLinearThermalExpansion.hxx>
#include <StepFEA_FeaShellBendingStiffness.hxx>
#include <StepFEA_FeaShellMembraneBendingCouplingStiffness.hxx>
#include <StepFEA_FeaShellMembraneStiffness.hxx>
#include <StepFEA_FeaShellShearStiffness.hxx>
#include <StepFEA_FeaSurfaceSectionGeometricRelationship.hxx>
#include <StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion.hxx>
#include <StepFEA_FreedomAndCoefficient.hxx>
#include <StepFEA_FreedomsList.hxx>
#include <StepFEA_GeometricNode.hxx>
#include <StepFEA_Node.hxx>
#include <StepFEA_NodeDefinition.hxx>
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
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BezierCurve.hxx>
#include <StepGeom_BezierCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_BezierSurface.hxx>
#include <StepGeom_BezierSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_BSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianTransformationOperator.hxx>
#include <StepGeom_CartesianTransformationOperator2d.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveOnSurface.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_CurveReplica.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_DegenerateToroidalSurface.hxx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_EvaluatedDegeneratePcurve.hxx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>
#include <StepGeom_GeometricRepresentationContextAndParametricRepresentationContext.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_IntersectionCurve.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <StepGeom_OffsetSurface.hxx>
#include <StepGeom_OrientedSurface.hxx>
#include <StepGeom_OuterBoundaryCurve.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_Placement.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Point.hxx>
#include <StepGeom_PointOnCurve.hxx>
#include <StepGeom_PointOnSurface.hxx>
#include <StepGeom_PointReplica.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepGeom_QuasiUniformCurve.hxx>
#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineCurve.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_ReparametrisedCompositeCurveSegment.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SurfaceCurveAndBoundedCurve.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepGeom_SurfaceReplica.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_UniformCurve.hxx>
#include <StepGeom_UniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_Vector.hxx>
#include <StepRepr_AssemblyComponentUsage.hxx>
#include <StepRepr_AssemblyComponentUsageSubstitute.hxx>
#include <StepRepr_CompositeShapeAspect.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationEffectivity.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentationRelationship.hxx>
#include <StepRepr_DataEnvironment.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepRepr_DerivedShapeAspect.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_Extension.hxx>
#include <StepRepr_ExternallyDefinedRepresentation.hxx>
#include <StepRepr_FunctionallyDefinedTransformation.hxx>
#include <StepRepr_GlobalUncertaintyAssignedContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_MakeFromUsageOption.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_MaterialDesignation.hxx>
#include <StepRepr_MaterialProperty.hxx>
#include <StepRepr_MaterialPropertyRepresentation.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ParametricRepresentationContext.hxx>
#include <StepRepr_ProductConcept.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ProductDefinitionUsage.hxx>
#include <StepRepr_PromissoryUsageOccurrence.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_QuantifiedAssemblyComponentUsage.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationRelationship.hxx>
#include <StepRepr_RepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <StepRepr_FeatureForDatumTargetRelationship.hxx>
#include <StepRepr_ShapeAspectTransition.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepRepr_ShapeRepresentationRelationshipWithTransformation.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepRepr_StructuralResponseProperty.hxx>
#include <StepRepr_StructuralResponsePropertyDefinitionRepresentation.hxx>
#include <StepRepr_SuppliedPartRelationship.hxx>
#include <StepRepr_ValueRange.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepShape_AdvancedFace.hxx>
#include <StepShape_AngularLocation.hxx>
#include <StepShape_AngularSize.hxx>
#include <StepShape_Block.hxx>
#include <StepShape_BooleanResult.hxx>
#include <StepShape_BoxDomain.hxx>
#include <StepShape_BoxedHalfSpace.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_CompoundShapeRepresentation.hxx>
#include <StepShape_ConnectedEdgeSet.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_ConnectedFaceShapeRepresentation.hxx>
#include <StepShape_ConnectedFaceSubSet.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_CsgShapeRepresentation.hxx>
#include <StepShape_CsgSolid.hxx>
#include <StepShape_DefinitionalRepresentationAndShapeRepresentation.hxx>
#include <StepShape_DimensionalCharacteristicRepresentation.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalLocationWithPath.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>
#include <StepShape_DirectedDimensionalLocation.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeBasedWireframeModel.hxx>
#include <StepShape_EdgeBasedWireframeShapeRepresentation.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_ExtrudedAreaSolid.hxx>
#include <StepShape_ExtrudedFaceSolid.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceBasedSurfaceModel.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_FaceOuterBound.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_FacetedBrepShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedSurfaceShapeRepresentation.hxx>
#include <StepShape_GeometricallyBoundedWireframeShapeRepresentation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_HalfSpaceSolid.hxx>
#include <StepShape_LimitsAndFits.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_LoopAndPath.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_ManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem.hxx>
#include <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_OrientedFace.hxx>
#include <StepShape_OrientedOpenShell.hxx>
#include <StepShape_OrientedPath.hxx>
#include <StepShape_Path.hxx>
#include <StepShape_PlusMinusTolerance.hxx>
#include <StepShape_PointRepresentation.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepShape_PrecisionQualifier.hxx>
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepShape_RevolvedAreaSolid.hxx>
#include <StepShape_RevolvedFaceSolid.hxx>
#include <StepShape_RightAngularWedge.hxx>
#include <StepShape_RightCircularCone.hxx>
#include <StepShape_RightCircularCylinder.hxx>
#include <StepShape_SeamEdge.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_ShapeRepresentationWithParameters.hxx>
#include <StepShape_ShellBasedSurfaceModel.hxx>
#include <StepShape_SolidModel.hxx>
#include <StepShape_SolidReplica.hxx>
#include <StepShape_Sphere.hxx>
#include <StepShape_Subedge.hxx>
#include <StepShape_Subface.hxx>
#include <StepShape_SweptAreaSolid.hxx>
#include <StepShape_SweptFaceSolid.hxx>
#include <StepShape_ToleranceValue.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_Torus.hxx>
#include <StepShape_TransitionalShapeRepresentation.hxx>
#include <StepShape_TypeQualifier.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepVisual_AnnotationOccurrence.hxx>
#include <StepVisual_AnnotationText.hxx>
#include <StepVisual_AnnotationTextOccurrence.hxx>
#include <StepVisual_AreaInSet.hxx>
#include <StepVisual_BackgroundColour.hxx>
#include <StepVisual_CameraImage.hxx>
#include <StepVisual_CameraImage2dWithScale.hxx>
#include <StepVisual_CameraImage3dWithScale.hxx>
#include <StepVisual_CameraModel.hxx>
#include <StepVisual_CameraModelD2.hxx>
#include <StepVisual_CameraModelD3.hxx>
#include <StepVisual_CameraUsage.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_ColourRgb.hxx>
#include <StepVisual_ColourSpecification.hxx>
#include <StepVisual_CompositeText.hxx>
#include <StepVisual_CompositeTextWithExtent.hxx>
#include <StepVisual_ContextDependentInvisibility.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>
#include <StepVisual_DraughtingAnnotationOccurrence.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_DraughtingPreDefinedColour.hxx>
#include <StepVisual_DraughtingPreDefinedCurveFont.hxx>
#include <StepVisual_ExternallyDefinedCurveFont.hxx>
#include <StepVisual_ExternallyDefinedTextFont.hxx>
#include <StepVisual_FillAreaStyle.hxx>
#include <StepVisual_FillAreaStyleColour.hxx>
#include <StepVisual_Invisibility.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationArea.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_OverRidingStyledItem.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepVisual_PreDefinedColour.hxx>
#include <StepVisual_PreDefinedCurveFont.hxx>
#include <StepVisual_PreDefinedItem.hxx>
#include <StepVisual_PreDefinedTextFont.hxx>
#include <StepVisual_PresentationArea.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_PresentationLayerUsage.hxx>
#include <StepVisual_PresentationRepresentation.hxx>
#include <StepVisual_PresentationSet.hxx>
#include <StepVisual_PresentationSize.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_PresentationView.hxx>
#include <StepVisual_PresentedItemRepresentation.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleBoundary.hxx>
#include <StepVisual_SurfaceStyleControlGrid.hxx>
#include <StepVisual_SurfaceStyleFillArea.hxx>
#include <StepVisual_SurfaceStyleParameterLine.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>
#include <StepVisual_SurfaceStyleRendering.hxx>
#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>
#include <StepVisual_SurfaceStyleSegmentationCurve.hxx>
#include <StepVisual_SurfaceStyleSilhouette.hxx>
#include <StepVisual_SurfaceStyleTransparent.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <StepVisual_Template.hxx>
#include <StepVisual_TemplateInstance.hxx>
#include <StepVisual_TextLiteral.hxx>
#include <StepVisual_TextStyle.hxx>
#include <StepVisual_TextStyleForDefinedFont.hxx>
#include <StepVisual_TextStyleWithBoxCharacteristics.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWStepAP214_GeneralModule,StepData_GeneralModule)

//#define DeclareAndCast(atype,result,start) \  NON car Name
// Handle(atype) result = Handle(atype)::DownCast (start)
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationCurveOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationFillArea.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationFillAreaOccurrence.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationSubfigureOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationSymbol.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_AnnotationSymbolOccurrence.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepAP214_AutoDesignViewArea.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_CompositeTextWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_CompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepShape_CsgRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DefinedSymbol.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DimensionCurve.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DimensionCurveTerminator.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_DraughtingCallout.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DraughtingSubfigureRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DraughtingSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DraughtingTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DrawingDefinition.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_DrawingRevision.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_ExternallyDefinedHatchStyle.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_ExternallyDefinedSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_ExternallyDefinedTileStyle.hxx>
// Removed from Rev2 to Rev4 :  <StepShape_FaceBasedSurfaceModel.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_FillAreaStyleHatching.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_FillAreaStyleTileSymbolWithStyle.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_FillAreaStyleTiles.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_MechanicalDesignPresentationArea.hxx>
// Removed from Rev2 to Rev4 :  <StepShape_NonManifoldSurfaceShapeRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <StepAP214_OneDirectionRepeatFactor.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_PreDefinedSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_ProductDataRepresentationView.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_SymbolColour.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_SymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_SymbolRepresentationMap.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_SymbolStyle.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_SymbolTarget.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_TerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_TextLiteralWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_TextLiteralWithBlankingBox.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_TextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 :  <StepVisual_TextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <StepVisual_TextStyleWithMirror.hxx>
// Removed from Rev2 to Rev4 :  <StepAP214_TwoDirectionRepeatFactor.hxx>
// Added by FMA
// Added by CKY  for Rev4
// full Rev4
//  Added by CKY (JUL-1998) for AP214 CC1 -> CC2
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationCurveOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationFillArea.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationFillAreaOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationSubfigureOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationSymbol.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationSymbolOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationText.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWAnnotationTextOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepAP214_RWAutoDesignViewArea.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWCompositeTextWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWCompositeTextWithBlankingBox.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <RWStepShape_RWCsgRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDimensionCurve.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDimensionCurveTerminator.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDraughtingAnnotationOccurrence.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDraughtingCallout.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDraughtingSubfigureRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDraughtingSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDraughtingTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDrawingDefinition.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWDrawingRevision.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWExternallyDefinedHatchStyle.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWExternallyDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWExternallyDefinedTextFont.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWExternallyDefinedTileStyle.hxx>
// Removed from Rev2 to Rev4 :  <RWStepShape_RWFaceBasedSurfaceModel.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWFillAreaStyleHatching.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWFillAreaStyleTileSymbolWithStyle.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWFillAreaStyleTiles.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWMechanicalDesignPresentationArea.hxx>
// Removed from Rev2 to Rev4 :  <RWStepShape_RWNonManifoldSurfaceShapeRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepAP214_RWOneDirectionRepeatFactor.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWPreDefinedSymbol.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWPreDefinedTextFont.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWProductDataRepresentationView.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWSymbolColour.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWSymbolRepresentation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWSymbolRepresentationMap.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWSymbolStyle.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWSymbolTarget.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTerminatorSymbol.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTextLiteralWithAssociatedCurves.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTextLiteralWithBlankingBox.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTextLiteralWithDelineation.hxx>
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTextLiteralWithExtent.hxx>
// Removed from CC1-Rev2 to CC1-Rev4, re-added CC2-Rev4 :
// Removed from Rev2 to Rev4 :  <RWStepVisual_RWTextStyleWithMirror.hxx>
// Removed from Rev2 to Rev4 :  <RWStepAP214_RWTwoDirectionRepeatFactor.hxx>
// Added by FMA  for Rev4
// full Rev4
//  Added by CKY (JUL-1998) for AP214 CC1 -> CC2
//Added from CC2 to DIS :j4
// Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
// Added by ABV 10.11.99 for AP203
// Added by ABV 13.01.00 for CAX-IF TRJ3
// Added by ABV 18.04.00 for CAX-IF TRJ4 (dimensions)
// Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
// Added by CKY , 25 APR 2001 for Dimensional Tolerances (CAX-IF TRJ7)
// abv 28.12.01: CAX-IF TRJ9: edge_based_wireframe
//Addef for AP209
// 23.01.2003
// ptv 28.01.2003
//  TR12J 04.06.2003 G&DT entities GKA 
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

#include <RWStepVisual_RWTessellatedAnnotationOccurrence.hxx>
#include <RWStepVisual_RWTessellatedItem.hxx>
#include <RWStepVisual_RWTessellatedGeometricSet.hxx>
#include <RWStepVisual_RWTessellatedCurveSet.hxx>
#include <RWStepVisual_RWCoordinatesList.hxx>
#include <StepRepr_CharacterizedRepresentation.hxx>
#include <RWStepRepr_RWCharacterizedRepresentation.hxx>
#include <StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel.hxx>
#include <RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel.hxx>
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
#include <StepVisual_AnnotationCurveOccurrenceAndGeomReprItem.hxx>
#include <RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem.hxx>

static Standard_Integer catsh,catdr,catstr,catdsc,cataux;


//=======================================================================
//function : RWStepAP214_GeneralModule
//purpose  : 
//=======================================================================

RWStepAP214_GeneralModule::RWStepAP214_GeneralModule ()
{
  Interface_Category::Init();
  catsh  = Interface_Category::Number("Shape");
  catdr  = Interface_Category::Number("Drawing");
  catstr = Interface_Category::Number("Structure");
  catdsc = Interface_Category::Number("Description");
  cataux = Interface_Category::Number("Auxiliary");
}


//=======================================================================
//function : FillShared
//purpose  : 
//=======================================================================
/*
void RWStepAP214_GeneralModule::FillShared (const Handle(Interface_InterfaceModel)& model,
                                            const Standard_Integer CN,
                                            const Handle(Standard_Transient)& ent,
                                            Interface_EntityIterator& iter) const
{
  switch (CN) {
  case 261 : {
    DeclareAndCast(StepShape_ShapeDefinitionRepresentation,anent,ent);
    RWStepShape_RWShapeDefinitionRepresentation tool;
    tool.Share(model,anent,iter);
  }
    break;
  case 391 : {
    DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anent,ent);
    RWStepShape_RWContextDependentShapeRepresentation tool;
    tool.Share(model,anent,iter);
  }
    break;
    default : FillSharedCase (CN,ent,iter);  // all other cases
    break;
  }
}
*/

//=======================================================================
//function : FillSharedCase
//purpose  : 
//=======================================================================

void RWStepAP214_GeneralModule::FillSharedCase(const Standard_Integer CN,
                                               const Handle(Standard_Transient)& ent,
                                               Interface_EntityIterator& iter) const
{
  switch(CN)
    {
      
    case 2:
      {
	DeclareAndCast(StepShape_AdvancedBrepShapeRepresentation,anent,ent);
	RWStepShape_RWAdvancedBrepShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 3:
      {
	DeclareAndCast(StepShape_AdvancedFace,anent,ent);
	RWStepShape_RWAdvancedFace tool;
	tool.Share(anent,iter);
      }
      break;
    case 4:
      {
	DeclareAndCast(StepVisual_AnnotationCurveOccurrence,anent,ent);
	RWStepVisual_RWAnnotationCurveOccurrence tool;
	tool.Share(anent,iter);
      }
      break;
    case 5:
    {
      DeclareAndCast(StepVisual_AnnotationFillArea, anent, ent);
      RWStepVisual_RWAnnotationFillArea tool;
      tool.Share(anent, iter);
    }
    break;
    case 6:
    {
      DeclareAndCast(StepVisual_AnnotationFillAreaOccurrence, anent, ent);
      RWStepVisual_RWAnnotationFillAreaOccurrence tool;
      tool.Share(anent, iter);
    }
    break;
    case 7:
      {
	DeclareAndCast(StepVisual_AnnotationOccurrence,anent,ent);
	RWStepVisual_RWAnnotationOccurrence tool;
	tool.Share(anent,iter);
      }
      break;
    case 11:
      {
	DeclareAndCast(StepRepr_MappedItem,anent,ent);
	RWStepRepr_RWMappedItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 12:
      {
	DeclareAndCast(StepVisual_StyledItem,anent,ent);
	RWStepVisual_RWStyledItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 14:
      {
	DeclareAndCast(StepBasic_ApplicationContextElement,anent,ent);
	RWStepBasic_RWApplicationContextElement tool;
	tool.Share(anent,iter);
      }
      break;
    case 15:
      {
	DeclareAndCast(StepBasic_ApplicationProtocolDefinition,anent,ent);
	RWStepBasic_RWApplicationProtocolDefinition tool;
	tool.Share(anent,iter);
      }
      break;
    case 16:
      {
	DeclareAndCast(StepBasic_Approval,anent,ent);
	RWStepBasic_RWApproval tool;
	tool.Share(anent,iter);
      }
      break;
    case 18:
      {
	DeclareAndCast(StepBasic_ApprovalPersonOrganization,anent,ent);
	RWStepBasic_RWApprovalPersonOrganization tool;
	tool.Share(anent,iter);
      }
      break;
    case 19:
      {
	DeclareAndCast(StepBasic_ApprovalRelationship,anent,ent);
	RWStepBasic_RWApprovalRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 22:
      {
	DeclareAndCast(StepVisual_AreaInSet,anent,ent);
	RWStepVisual_RWAreaInSet tool;
	tool.Share(anent,iter);
      }
      break;
    case 23:
      {
	DeclareAndCast(StepAP214_AutoDesignActualDateAndTimeAssignment,anent,ent);
	RWStepAP214_RWAutoDesignActualDateAndTimeAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 24:
      {
	DeclareAndCast(StepAP214_AutoDesignActualDateAssignment,anent,ent);
	RWStepAP214_RWAutoDesignActualDateAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 25:
      {
	DeclareAndCast(StepAP214_AutoDesignApprovalAssignment,anent,ent);
	RWStepAP214_RWAutoDesignApprovalAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 26:
      {
	DeclareAndCast(StepAP214_AutoDesignDateAndPersonAssignment,anent,ent);
	RWStepAP214_RWAutoDesignDateAndPersonAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 27:
      {
	DeclareAndCast(StepAP214_AutoDesignGroupAssignment,anent,ent);
	RWStepAP214_RWAutoDesignGroupAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 28:
      {
	DeclareAndCast(StepAP214_AutoDesignNominalDateAndTimeAssignment,anent,ent);
	RWStepAP214_RWAutoDesignNominalDateAndTimeAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 29:
      {
	DeclareAndCast(StepAP214_AutoDesignNominalDateAssignment,anent,ent);
	RWStepAP214_RWAutoDesignNominalDateAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 30:
      {
	DeclareAndCast(StepAP214_AutoDesignOrganizationAssignment,anent,ent);
	RWStepAP214_RWAutoDesignOrganizationAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 31:
      {
	DeclareAndCast(StepAP214_AutoDesignPersonAndOrganizationAssignment,anent,ent);
	RWStepAP214_RWAutoDesignPersonAndOrganizationAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 32:
      {
	DeclareAndCast(StepAP214_AutoDesignPresentedItem,anent,ent);
	RWStepAP214_RWAutoDesignPresentedItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 33:
      {
	DeclareAndCast(StepAP214_AutoDesignSecurityClassificationAssignment,anent,ent);
	RWStepAP214_RWAutoDesignSecurityClassificationAssignment tool;
	tool.Share(anent,iter);
      }
      break;

    case 35:
      {
	DeclareAndCast(StepGeom_Axis1Placement,anent,ent);
	RWStepGeom_RWAxis1Placement tool;
	tool.Share(anent,iter);
      }
      break;
    case 36:
      {
	DeclareAndCast(StepGeom_Axis2Placement2d,anent,ent);
	RWStepGeom_RWAxis2Placement2d tool;
	tool.Share(anent,iter);
      }
      break;
    case 37:
      {
	DeclareAndCast(StepGeom_Axis2Placement3d,anent,ent);
	RWStepGeom_RWAxis2Placement3d tool;
	tool.Share(anent,iter);
      }
      break;
    case 38:
      {
	DeclareAndCast(StepGeom_BSplineCurve,anent,ent);
	RWStepGeom_RWBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 39:
      {
	DeclareAndCast(StepGeom_BSplineCurveWithKnots,anent,ent);
	RWStepGeom_RWBSplineCurveWithKnots tool;
	tool.Share(anent,iter);
      }
      break;
    case 40:
      {
	DeclareAndCast(StepGeom_BSplineSurface,anent,ent);
	RWStepGeom_RWBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 41:
      {
	DeclareAndCast(StepGeom_BSplineSurfaceWithKnots,anent,ent);
	RWStepGeom_RWBSplineSurfaceWithKnots tool;
	tool.Share(anent,iter);
      }
      break;
    case 42:
      {
	DeclareAndCast(StepVisual_BackgroundColour,anent,ent);
	RWStepVisual_RWBackgroundColour tool;
	tool.Share(anent,iter);
      }
      break;
    case 43:
      {
	DeclareAndCast(StepGeom_BezierCurve,anent,ent);
	RWStepGeom_RWBezierCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 44:
      {
	DeclareAndCast(StepGeom_BezierSurface,anent,ent);
	RWStepGeom_RWBezierSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 45:
      {
	DeclareAndCast(StepShape_Block,anent,ent);
	RWStepShape_RWBlock tool;
	tool.Share(anent,iter);
      }
      break;
    case 46:
      {
	DeclareAndCast(StepShape_BooleanResult,anent,ent);
	RWStepShape_RWBooleanResult tool;
	tool.Share(anent,iter);
      }
      break;
    case 47:
      {
	DeclareAndCast(StepGeom_BoundaryCurve,anent,ent);
	RWStepGeom_RWBoundaryCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 50:
      {
	DeclareAndCast(StepShape_BoxDomain,anent,ent);
	RWStepShape_RWBoxDomain tool;
	tool.Share(anent,iter);
      }
      break;
    case 51:
      {
	DeclareAndCast(StepShape_BoxedHalfSpace,anent,ent);
	RWStepShape_RWBoxedHalfSpace tool;
	tool.Share(anent,iter);
      }
      break;
    case 52:
      {
	DeclareAndCast(StepShape_BrepWithVoids,anent,ent);
	RWStepShape_RWBrepWithVoids tool;
	tool.Share(anent,iter);
      }
      break;
    case 54:
      {
	DeclareAndCast(StepVisual_CameraImage,anent,ent);
	RWStepVisual_RWCameraImage tool;
	tool.Share(anent,iter);
      }
      break;
    case 56:
      {
	DeclareAndCast(StepVisual_CameraModelD2,anent,ent);
	RWStepVisual_RWCameraModelD2 tool;
	tool.Share(anent,iter);
      }
      break;
    case 57:
      {
	DeclareAndCast(StepVisual_CameraModelD3,anent,ent);
	RWStepVisual_RWCameraModelD3 tool;
	tool.Share(anent,iter);
      }
      break;
    case 58:
      {
	DeclareAndCast(StepVisual_CameraUsage,anent,ent);
	RWStepVisual_RWCameraUsage tool;
	tool.Share(anent,iter);
      }
      break;
    case 60:
      {
	DeclareAndCast(StepGeom_CartesianTransformationOperator,anent,ent);
	RWStepGeom_RWCartesianTransformationOperator tool;
	tool.Share(anent,iter);
      }
      break;
    case 61:
      {
	DeclareAndCast(StepGeom_CartesianTransformationOperator3d,anent,ent);
	RWStepGeom_RWCartesianTransformationOperator3d tool;
	tool.Share(anent,iter);
      }
      break;
    case 62:
      {
	DeclareAndCast(StepGeom_Circle,anent,ent);
	RWStepGeom_RWCircle tool;
	tool.Share(anent,iter);
      }
      break;
    case 63:
      {
	DeclareAndCast(StepShape_ClosedShell,anent,ent);
	RWStepShape_RWClosedShell tool;
	tool.Share(anent,iter);
      }
      break;
    case 67:
      {
	DeclareAndCast(StepGeom_CompositeCurve,anent,ent);
	RWStepGeom_RWCompositeCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 68:
      {
	DeclareAndCast(StepGeom_CompositeCurveOnSurface,anent,ent);
	RWStepGeom_RWCompositeCurveOnSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 69:
      {
	DeclareAndCast(StepGeom_CompositeCurveSegment,anent,ent);
	RWStepGeom_RWCompositeCurveSegment tool;
	tool.Share(anent,iter);
      }
      break;

    case 70:
      {
	DeclareAndCast(StepVisual_CompositeText,anent,ent);
	RWStepVisual_RWCompositeText tool;
	tool.Share(anent,iter);
      }
      break;
    case 73:
      {
	DeclareAndCast(StepVisual_CompositeTextWithExtent,anent,ent);
	RWStepVisual_RWCompositeTextWithExtent tool;
	tool.Share(anent,iter);
      }
      break;
    case 74:
      {
	DeclareAndCast(StepGeom_Conic,anent,ent);
	RWStepGeom_RWConic tool;
	tool.Share(anent,iter);
      }
      break;
    case 75:
      {
	DeclareAndCast(StepGeom_ConicalSurface,anent,ent);
	RWStepGeom_RWConicalSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 76:
      {
	DeclareAndCast(StepShape_ConnectedFaceSet,anent,ent);
	RWStepShape_RWConnectedFaceSet tool;
	tool.Share(anent,iter);
      }
      break;
    case 77:
      {
	DeclareAndCast(StepVisual_ContextDependentInvisibility,anent,ent);
	RWStepVisual_RWContextDependentInvisibility tool;
	tool.Share(anent,iter);
      }
      break;
    case 78:
      {
	DeclareAndCast(StepVisual_ContextDependentOverRidingStyledItem,anent,ent);
	RWStepVisual_RWContextDependentOverRidingStyledItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 79:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnit tool;
	tool.Share(anent,iter);
      }
      break;

    case 82:
      {
	DeclareAndCast(StepShape_CsgShapeRepresentation,anent,ent);
	RWStepShape_RWCsgShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 83:
      {
	DeclareAndCast(StepShape_CsgSolid,anent,ent);
	RWStepShape_RWCsgSolid tool;
	tool.Share(anent,iter);
      }
      break;
    case 85:
      {
	DeclareAndCast(StepGeom_CurveBoundedSurface,anent,ent);
	RWStepGeom_RWCurveBoundedSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 86:
      {
	DeclareAndCast(StepGeom_CurveReplica,anent,ent);
	RWStepGeom_RWCurveReplica tool;
	tool.Share(anent,iter);
      }
      break;
    case 87:
      {
	DeclareAndCast(StepVisual_CurveStyle,anent,ent);
	RWStepVisual_RWCurveStyle tool;
	tool.Share(anent,iter);
      }
      break;
    case 88:
      {
	DeclareAndCast(StepVisual_CurveStyleFont,anent,ent);
	RWStepVisual_RWCurveStyleFont tool;
	tool.Share(anent,iter);
      }
      break;
    case 90:
      {
	DeclareAndCast(StepGeom_CylindricalSurface,anent,ent);
	RWStepGeom_RWCylindricalSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 92:
      {
	DeclareAndCast(StepBasic_DateAndTime,anent,ent);
	RWStepBasic_RWDateAndTime tool;
	tool.Share(anent,iter);
      }
      break;
    case 98:
      {
	DeclareAndCast(StepRepr_DefinitionalRepresentation,anent,ent);
	RWStepRepr_RWDefinitionalRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 99:
      {
	DeclareAndCast(StepGeom_DegeneratePcurve,anent,ent);
	RWStepGeom_RWDegeneratePcurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 100:
      {
	DeclareAndCast(StepGeom_DegenerateToroidalSurface,anent,ent);
	RWStepGeom_RWDegenerateToroidalSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 106:
      {
	DeclareAndCast(StepVisual_StyledItem,anent,ent);
	RWStepVisual_RWStyledItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 107:
      {
	DeclareAndCast(StepVisual_DraughtingCallout,anent,ent);
	RWStepVisual_RWDraughtingCallout tool;
	tool.Share(anent,iter);
      }
      break;
    case 116:
      {
	DeclareAndCast(StepShape_EdgeCurve,anent,ent);
	RWStepShape_RWEdgeCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 117:
      {
	DeclareAndCast(StepShape_EdgeLoop,anent,ent);
	RWStepShape_RWEdgeLoop tool;
	tool.Share(anent,iter);
      }
      break;
    case 118:
      {
	DeclareAndCast(StepGeom_ElementarySurface,anent,ent);
	RWStepGeom_RWElementarySurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 119:
      {
	DeclareAndCast(StepGeom_Ellipse,anent,ent);
	RWStepGeom_RWEllipse tool;
	tool.Share(anent,iter);
      }
      break;
    case 120:
      {
	DeclareAndCast(StepGeom_EvaluatedDegeneratePcurve,anent,ent);
	RWStepGeom_RWEvaluatedDegeneratePcurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 122:
      {
	DeclareAndCast(StepVisual_ExternallyDefinedCurveFont,anent,ent);
	RWStepVisual_RWExternallyDefinedCurveFont tool;
	tool.Share(anent,iter);
      }
      break;
    case 124:
    case 126:
      {
	DeclareAndCast(StepBasic_ExternallyDefinedItem,anent,ent);
	RWStepBasic_RWExternallyDefinedItem tool;
	tool.Share(anent,iter);
      }
      break;

    case 128:
      {
	DeclareAndCast(StepShape_ExtrudedAreaSolid,anent,ent);
	RWStepShape_RWExtrudedAreaSolid tool;
	tool.Share(anent,iter);
      }
      break;
    case 129:
      {
	DeclareAndCast(StepShape_Face,anent,ent);
	RWStepShape_RWFace tool;
	tool.Share(anent,iter);
      }
      break;
    case 131:
      {
	DeclareAndCast(StepShape_FaceBound,anent,ent);
	RWStepShape_RWFaceBound tool;
	tool.Share(anent,iter);
      }
      break;
    case 132:
      {
	DeclareAndCast(StepShape_FaceOuterBound,anent,ent);
	RWStepShape_RWFaceOuterBound tool;
	tool.Share(anent,iter);
      }
      break;
    case 133:
      {
	DeclareAndCast(StepShape_FaceSurface,anent,ent);
	RWStepShape_RWFaceSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 134:
      {
	DeclareAndCast(StepShape_FacetedBrep,anent,ent);
	RWStepShape_RWFacetedBrep tool;
	tool.Share(anent,iter);
      }
      break;
    case 135:
      {
	DeclareAndCast(StepShape_FacetedBrepShapeRepresentation,anent,ent);
	RWStepShape_RWFacetedBrepShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 136:
      {
	DeclareAndCast(StepVisual_FillAreaStyle,anent,ent);
	RWStepVisual_RWFillAreaStyle tool;
	tool.Share(anent,iter);
      }
      break;
    case 137:
      {
	DeclareAndCast(StepVisual_FillAreaStyleColour,anent,ent);
	RWStepVisual_RWFillAreaStyleColour tool;
	tool.Share(anent,iter);
      }
      break;
    case 142:
      {
	DeclareAndCast(StepShape_GeometricCurveSet,anent,ent);
	RWStepShape_RWGeometricCurveSet tool;
	tool.Share(anent,iter);
      }
      break;
    case 145:
      {
	DeclareAndCast(StepShape_GeometricSet,anent,ent);
	RWStepShape_RWGeometricSet tool;
	tool.Share(anent,iter);
      }
      break;
    case 146:
      {
	DeclareAndCast(StepShape_GeometricallyBoundedSurfaceShapeRepresentation,anent,ent);
	RWStepShape_RWGeometricallyBoundedSurfaceShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 147:
      {
	DeclareAndCast(StepShape_GeometricallyBoundedWireframeShapeRepresentation,anent,ent);
	RWStepShape_RWGeometricallyBoundedWireframeShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 148:
      {
	DeclareAndCast(StepRepr_GlobalUncertaintyAssignedContext,anent,ent);
	RWStepRepr_RWGlobalUncertaintyAssignedContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 149:
      {
	DeclareAndCast(StepRepr_GlobalUnitAssignedContext,anent,ent);
	RWStepRepr_RWGlobalUnitAssignedContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 152:
      {
	DeclareAndCast(StepBasic_GroupRelationship,anent,ent);
	RWStepBasic_RWGroupRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 153:
      {
	DeclareAndCast(StepShape_HalfSpaceSolid,anent,ent);
	RWStepShape_RWHalfSpaceSolid tool;
	tool.Share(anent,iter);
      }
      break;
    case 154:
      {
	DeclareAndCast(StepGeom_Hyperbola,anent,ent);
	RWStepGeom_RWHyperbola tool;
	tool.Share(anent,iter);
      }
      break;
    case 155:
      {
	DeclareAndCast(StepGeom_IntersectionCurve,anent,ent);
	RWStepGeom_RWIntersectionCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 156:
      {
	DeclareAndCast(StepVisual_Invisibility,anent,ent);
	RWStepVisual_RWInvisibility tool;
	tool.Share(anent,iter);
      }
      break;
    case 157:
      {
	DeclareAndCast(StepBasic_LengthMeasureWithUnit,anent,ent);
	RWStepBasic_RWLengthMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 158:
      {
	DeclareAndCast(StepBasic_LengthUnit,anent,ent);
	RWStepBasic_RWLengthUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 159:
      {
	DeclareAndCast(StepGeom_Line,anent,ent);
	RWStepGeom_RWLine tool;
	tool.Share(anent,iter);
      }
      break;
    case 160:
      {
	DeclareAndCast(StepBasic_LocalTime,anent,ent);
	RWStepBasic_RWLocalTime tool;
	tool.Share(anent,iter);
      }
      break;
    case 162:
      {
	DeclareAndCast(StepShape_ManifoldSolidBrep,anent,ent);
	RWStepShape_RWManifoldSolidBrep tool;
	tool.Share(anent,iter);
      }
      break;
    case 163:
      {
	DeclareAndCast(StepShape_ManifoldSurfaceShapeRepresentation,anent,ent);
	RWStepShape_RWManifoldSurfaceShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 164:
      {
	DeclareAndCast(StepRepr_MappedItem,anent,ent);
	RWStepRepr_RWMappedItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 165:
      {
	DeclareAndCast(StepBasic_MeasureWithUnit,anent,ent);
	RWStepBasic_RWMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 166:
      {
	DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationArea,anent,ent);
	RWStepVisual_RWMechanicalDesignGeometricPresentationArea tool;
	tool.Share(anent,iter);
      }
      break;
    case 167:
      {
	DeclareAndCast(StepVisual_MechanicalDesignGeometricPresentationRepresentation,anent,ent);
	RWStepVisual_RWMechanicalDesignGeometricPresentationRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 169:
      {
	DeclareAndCast(StepBasic_NamedUnit,anent,ent);
	RWStepBasic_RWNamedUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 171:
      {
	DeclareAndCast(StepGeom_OffsetCurve3d,anent,ent);
	RWStepGeom_RWOffsetCurve3d tool;
	tool.Share(anent,iter);
      }
      break;
    case 172:
      {
	DeclareAndCast(StepGeom_OffsetSurface,anent,ent);
	RWStepGeom_RWOffsetSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 174:
      {
	DeclareAndCast(StepShape_OpenShell,anent,ent);
	RWStepShape_RWOpenShell tool;
	tool.Share(anent,iter);
      }
      break;
    case 179:
      {
	DeclareAndCast(StepBasic_OrganizationalAddress,anent,ent);
	RWStepBasic_RWOrganizationalAddress tool;
	tool.Share(anent,iter);
      }
      break;
    case 180:
      {
	DeclareAndCast(StepShape_OrientedClosedShell,anent,ent);
	RWStepShape_RWOrientedClosedShell tool;
	tool.Share(anent,iter);
      }
      break;
    case 181:
      {
	DeclareAndCast(StepShape_OrientedEdge,anent,ent);
	RWStepShape_RWOrientedEdge tool;
	tool.Share(anent,iter);
      }
      break;
    case 182:
      {
	DeclareAndCast(StepShape_OrientedFace,anent,ent);
	RWStepShape_RWOrientedFace tool;
	tool.Share(anent,iter);
      }
      break;
    case 183:
      {
	DeclareAndCast(StepShape_OrientedOpenShell,anent,ent);
	RWStepShape_RWOrientedOpenShell tool;
	tool.Share(anent,iter);
      }
      break;
    case 184:
      {
	DeclareAndCast(StepShape_OrientedPath,anent,ent);
	RWStepShape_RWOrientedPath tool;
	tool.Share(anent,iter);
      }
      break;
    case 185:
      {
	DeclareAndCast(StepGeom_OuterBoundaryCurve,anent,ent);
	RWStepGeom_RWOuterBoundaryCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 186:
      {
	DeclareAndCast(StepVisual_OverRidingStyledItem,anent,ent);
	RWStepVisual_RWOverRidingStyledItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 187:
      {
	DeclareAndCast(StepGeom_Parabola,anent,ent);
	RWStepGeom_RWParabola tool;
	tool.Share(anent,iter);
      }
      break;
    case 189:
      {
	DeclareAndCast(StepShape_Path,anent,ent);
	RWStepShape_RWPath tool;
	tool.Share(anent,iter);
      }
      break;
    case 190:
      {
	DeclareAndCast(StepGeom_Pcurve,anent,ent);
	RWStepGeom_RWPcurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 192:
      {
	DeclareAndCast(StepBasic_PersonAndOrganization,anent,ent);
	RWStepBasic_RWPersonAndOrganization tool;
	tool.Share(anent,iter);
      }
      break;
    case 195:
      {
	DeclareAndCast(StepBasic_PersonalAddress,anent,ent);
	RWStepBasic_RWPersonalAddress tool;
	tool.Share(anent,iter);
      }
      break;
    case 196:
      {
	DeclareAndCast(StepGeom_Placement,anent,ent);
	RWStepGeom_RWPlacement tool;
	tool.Share(anent,iter);
      }
      break;
    case 197:
      {
	DeclareAndCast(StepVisual_PlanarBox,anent,ent);
	RWStepVisual_RWPlanarBox tool;
	tool.Share(anent,iter);
      }
      break;
    case 199:
      {
	DeclareAndCast(StepGeom_Plane,anent,ent);
	RWStepGeom_RWPlane tool;
	tool.Share(anent,iter);
      }
      break;
    case 200:
      {
	DeclareAndCast(StepBasic_PlaneAngleMeasureWithUnit,anent,ent);
	RWStepBasic_RWPlaneAngleMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 201:
      {
	DeclareAndCast(StepBasic_PlaneAngleUnit,anent,ent);
	RWStepBasic_RWPlaneAngleUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 203:
      {
	DeclareAndCast(StepGeom_PointOnCurve,anent,ent);
	RWStepGeom_RWPointOnCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 204:
      {
	DeclareAndCast(StepGeom_PointOnSurface,anent,ent);
	RWStepGeom_RWPointOnSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 205:
      {
	DeclareAndCast(StepGeom_PointReplica,anent,ent);
	RWStepGeom_RWPointReplica tool;
	tool.Share(anent,iter);
      }
      break;
    case 206:
      {
	DeclareAndCast(StepVisual_PointStyle,anent,ent);
	RWStepVisual_RWPointStyle tool;
	tool.Share(anent,iter);
      }
      break;
    case 207:
      {
	DeclareAndCast(StepShape_PolyLoop,anent,ent);
	RWStepShape_RWPolyLoop tool;
	tool.Share(anent,iter);
      }
      break;
    case 208:
      {
	DeclareAndCast(StepGeom_Polyline,anent,ent);
	RWStepGeom_RWPolyline tool;
	tool.Share(anent,iter);
      }
      break;
    case 214:
      {
	DeclareAndCast(StepVisual_PresentationArea,anent,ent);
	RWStepVisual_RWPresentationArea tool;
	tool.Share(anent,iter);
      }
      break;
    case 215:
      {
	DeclareAndCast(StepVisual_PresentationLayerAssignment,anent,ent);
	RWStepVisual_RWPresentationLayerAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 216:
      {
	DeclareAndCast(StepVisual_PresentationRepresentation,anent,ent);
	RWStepVisual_RWPresentationRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 218:
      {
	DeclareAndCast(StepVisual_PresentationSize,anent,ent);
	RWStepVisual_RWPresentationSize tool;
	tool.Share(anent,iter);
      }
      break;
    case 219:
      {
	DeclareAndCast(StepVisual_PresentationStyleAssignment,anent,ent);
	RWStepVisual_RWPresentationStyleAssignment tool;
	tool.Share(anent,iter);
      }
      break;
    case 220:
      {
	DeclareAndCast(StepVisual_PresentationStyleByContext,anent,ent);
	RWStepVisual_RWPresentationStyleByContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 221:
      {
	DeclareAndCast(StepVisual_PresentationView,anent,ent);
	RWStepVisual_RWPresentationView tool;
	tool.Share(anent,iter);
      }
      break;
    case 223:
      {
	DeclareAndCast(StepBasic_Product,anent,ent);
	RWStepBasic_RWProduct tool;
	tool.Share(anent,iter);
      }
      break;
    case 225:
      {
	DeclareAndCast(StepBasic_ProductContext,anent,ent);
	RWStepBasic_RWProductContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 227:
      {
	DeclareAndCast(StepBasic_ProductDefinition,anent,ent);
	RWStepBasic_RWProductDefinition tool;
	tool.Share(anent,iter);
      }
      break;
    case 228:
      {
	DeclareAndCast(StepBasic_ProductDefinitionContext,anent,ent);
	RWStepBasic_RWProductDefinitionContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 229:
      {
	DeclareAndCast(StepBasic_ProductDefinitionFormation,anent,ent);
	RWStepBasic_RWProductDefinitionFormation tool;
	tool.Share(anent,iter);
      }
      break;
    case 230:
      {
	DeclareAndCast(StepBasic_ProductDefinitionFormationWithSpecifiedSource,anent,ent);
	RWStepBasic_RWProductDefinitionFormationWithSpecifiedSource tool;
	tool.Share(anent,iter);
      }
      break;
    case 231:
      {
	DeclareAndCast(StepRepr_ProductDefinitionShape,anent,ent);
	RWStepRepr_RWProductDefinitionShape tool;
	tool.Share(anent,iter);
      }
      break;
    case 232:
      {
	DeclareAndCast(StepBasic_ProductRelatedProductCategory,anent,ent);
	RWStepBasic_RWProductRelatedProductCategory tool;
	tool.Share(anent,iter);
      }
      break;
    case 233:
      {
	DeclareAndCast(StepBasic_ProductType,anent,ent);
	RWStepBasic_RWProductType tool;
	tool.Share(anent,iter);
      }
      break;
    case 234:
      {
	DeclareAndCast(StepRepr_PropertyDefinition,anent,ent);
	RWStepRepr_RWPropertyDefinition tool;
	tool.Share(anent,iter);
      }
      break;
    case 235:
      {
	DeclareAndCast(StepRepr_PropertyDefinitionRepresentation,anent,ent);
	RWStepRepr_RWPropertyDefinitionRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 236:
      {
	DeclareAndCast(StepGeom_QuasiUniformCurve,anent,ent);
	RWStepGeom_RWQuasiUniformCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 237:
      {
	DeclareAndCast(StepGeom_QuasiUniformSurface,anent,ent);
	RWStepGeom_RWQuasiUniformSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 238:
      {
	DeclareAndCast(StepBasic_RatioMeasureWithUnit,anent,ent);
	RWStepBasic_RWRatioMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 239:
      {
	DeclareAndCast(StepGeom_RationalBSplineCurve,anent,ent);
	RWStepGeom_RWRationalBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 240:
      {
	DeclareAndCast(StepGeom_RationalBSplineSurface,anent,ent);
	RWStepGeom_RWRationalBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 241:
      {
	DeclareAndCast(StepGeom_RectangularCompositeSurface,anent,ent);
	RWStepGeom_RWRectangularCompositeSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 242:
      {
	DeclareAndCast(StepGeom_RectangularTrimmedSurface,anent,ent);
	RWStepGeom_RWRectangularTrimmedSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 244:
      {
	DeclareAndCast(StepGeom_ReparametrisedCompositeCurveSegment,anent,ent);
	RWStepGeom_RWReparametrisedCompositeCurveSegment tool;
	tool.Share(anent,iter);
      }
      break;
    case 245:
      {
	DeclareAndCast(StepRepr_Representation,anent,ent);
	RWStepRepr_RWRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 248:
      {
	DeclareAndCast(StepRepr_RepresentationMap,anent,ent);
	RWStepRepr_RWRepresentationMap tool;
	tool.Share(anent,iter);
      }
      break;
    case 249:
      {
	DeclareAndCast(StepRepr_RepresentationRelationship,anent,ent);
	RWStepRepr_RWRepresentationRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 250:
      {
	DeclareAndCast(StepShape_RevolvedAreaSolid,anent,ent);
	RWStepShape_RWRevolvedAreaSolid tool;
	tool.Share(anent,iter);
      }
      break;
    case 251:
      {
	DeclareAndCast(StepShape_RightAngularWedge,anent,ent);
	RWStepShape_RWRightAngularWedge tool;
	tool.Share(anent,iter);
      }
      break;
    case 252:
      {
	DeclareAndCast(StepShape_RightCircularCone,anent,ent);
	RWStepShape_RWRightCircularCone tool;
	tool.Share(anent,iter);
      }
      break;
    case 253:
      {
	DeclareAndCast(StepShape_RightCircularCylinder,anent,ent);
	RWStepShape_RWRightCircularCylinder tool;
	tool.Share(anent,iter);
      }
      break;
    case 254:
      {
	DeclareAndCast(StepGeom_SeamCurve,anent,ent);
	RWStepGeom_RWSeamCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 255:
      {
	DeclareAndCast(StepBasic_SecurityClassification,anent,ent);
	RWStepBasic_RWSecurityClassification tool;
	tool.Share(anent,iter);
      }
      break;
    case 258:
      {
	DeclareAndCast(StepRepr_ShapeAspect,anent,ent);
	RWStepRepr_RWShapeAspect tool;
	tool.Share(anent,iter);
      }
      break;
    case 259:
      {
	DeclareAndCast(StepRepr_ShapeAspectRelationship,anent,ent);
	RWStepRepr_RWShapeAspectRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 260:
      {
	DeclareAndCast(StepRepr_ShapeAspectTransition,anent,ent);
	RWStepRepr_RWShapeAspectTransition tool;
	tool.Share(anent,iter);
      }
      break;
    case 261:
      {
	DeclareAndCast(StepShape_ShapeDefinitionRepresentation,anent,ent);
	RWStepShape_RWShapeDefinitionRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 262:
      {
	DeclareAndCast(StepShape_ShapeRepresentation,anent,ent);
	RWStepShape_RWShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 263:
      {
	DeclareAndCast(StepShape_ShellBasedSurfaceModel,anent,ent);
	RWStepShape_RWShellBasedSurfaceModel tool;
	tool.Share(anent,iter);
      }
      break;
    case 265:
      {
	DeclareAndCast(StepBasic_SolidAngleMeasureWithUnit,anent,ent);
	RWStepBasic_RWSolidAngleMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 267:
      {
	DeclareAndCast(StepShape_SolidReplica,anent,ent);
	RWStepShape_RWSolidReplica tool;
	tool.Share(anent,iter);
      }
      break;
    case 268:
      {
	DeclareAndCast(StepShape_Sphere,anent,ent);
	RWStepShape_RWSphere tool;
	tool.Share(anent,iter);
      }
      break;
    case 269:
      {
	DeclareAndCast(StepGeom_SphericalSurface,anent,ent);
	RWStepGeom_RWSphericalSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 270:
      {
	DeclareAndCast(StepVisual_StyledItem,anent,ent);
	RWStepVisual_RWStyledItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 272:
      {
	DeclareAndCast(StepGeom_SurfaceCurve,anent,ent);
	RWStepGeom_RWSurfaceCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 273:
      {
	DeclareAndCast(StepGeom_SurfaceOfLinearExtrusion,anent,ent);
	RWStepGeom_RWSurfaceOfLinearExtrusion tool;
	tool.Share(anent,iter);
      }
      break;
    case 274:
      {
	DeclareAndCast(StepGeom_SurfaceOfRevolution,anent,ent);
	RWStepGeom_RWSurfaceOfRevolution tool;
	tool.Share(anent,iter);
      }
      break;
    case 275:
      {
	DeclareAndCast(StepGeom_SurfacePatch,anent,ent);
	RWStepGeom_RWSurfacePatch tool;
	tool.Share(anent,iter);
      }
      break;
    case 276:
      {
	DeclareAndCast(StepGeom_SurfaceReplica,anent,ent);
	RWStepGeom_RWSurfaceReplica tool;
	tool.Share(anent,iter);
      }
      break;
    case 277:
      {
	DeclareAndCast(StepVisual_SurfaceSideStyle,anent,ent);
	RWStepVisual_RWSurfaceSideStyle tool;
	tool.Share(anent,iter);
      }
      break;
    case 278:
      {
	DeclareAndCast(StepVisual_SurfaceStyleBoundary,anent,ent);
	RWStepVisual_RWSurfaceStyleBoundary tool;
	tool.Share(anent,iter);
      }
      break;
    case 279:
      {
	DeclareAndCast(StepVisual_SurfaceStyleControlGrid,anent,ent);
	RWStepVisual_RWSurfaceStyleControlGrid tool;
	tool.Share(anent,iter);
      }
      break;
    case 280:
      {
	DeclareAndCast(StepVisual_SurfaceStyleFillArea,anent,ent);
	RWStepVisual_RWSurfaceStyleFillArea tool;
	tool.Share(anent,iter);
      }
      break;
    case 281:
      {
	DeclareAndCast(StepVisual_SurfaceStyleParameterLine,anent,ent);
	RWStepVisual_RWSurfaceStyleParameterLine tool;
	tool.Share(anent,iter);
      }
      break;
    case 282:
      {
	DeclareAndCast(StepVisual_SurfaceStyleSegmentationCurve,anent,ent);
	RWStepVisual_RWSurfaceStyleSegmentationCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 283:
      {
	DeclareAndCast(StepVisual_SurfaceStyleSilhouette,anent,ent);
	RWStepVisual_RWSurfaceStyleSilhouette tool;
	tool.Share(anent,iter);
      }
      break;
    case 284:
      {
	DeclareAndCast(StepVisual_SurfaceStyleUsage,anent,ent);
	RWStepVisual_RWSurfaceStyleUsage tool;
	tool.Share(anent,iter);
      }
      break;
    case 285:
      {
	DeclareAndCast(StepShape_SweptAreaSolid,anent,ent);
	RWStepShape_RWSweptAreaSolid tool;
	tool.Share(anent,iter);
      }
      break;
    case 286:
      {
	DeclareAndCast(StepGeom_SweptSurface,anent,ent);
	RWStepGeom_RWSweptSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 292:
      {
	DeclareAndCast(StepVisual_Template,anent,ent);
	RWStepVisual_RWTemplate tool;
	tool.Share(anent,iter);
      }
      break;
    case 293:
      {
	DeclareAndCast(StepVisual_TemplateInstance,anent,ent);
	RWStepVisual_RWTemplateInstance tool;
	tool.Share(anent,iter);
      }
      break;
    case 295:
      {
	DeclareAndCast(StepVisual_TextLiteral,anent,ent);
	RWStepVisual_RWTextLiteral tool;
	tool.Share(anent,iter);
      }
      break;
    case 300:
      {
	DeclareAndCast(StepVisual_TextStyle,anent,ent);
	RWStepVisual_RWTextStyle tool;
	tool.Share(anent,iter);
      }
      break;
    case 301:
      {
	DeclareAndCast(StepVisual_TextStyleForDefinedFont,anent,ent);
	RWStepVisual_RWTextStyleForDefinedFont tool;
	tool.Share(anent,iter);
      }
      break;
    case 302:
      {
	DeclareAndCast(StepVisual_TextStyleWithBoxCharacteristics,anent,ent);
	RWStepVisual_RWTextStyleWithBoxCharacteristics tool;
	tool.Share(anent,iter);
      }
      break;

    case 305:
      {
	DeclareAndCast(StepGeom_ToroidalSurface,anent,ent);
	RWStepGeom_RWToroidalSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 306:
      {
	DeclareAndCast(StepShape_Torus,anent,ent);
	RWStepShape_RWTorus tool;
	tool.Share(anent,iter);
      }
      break;
    case 307:
      {
	DeclareAndCast(StepShape_TransitionalShapeRepresentation,anent,ent);
	RWStepShape_RWTransitionalShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 308:
      {
	DeclareAndCast(StepGeom_TrimmedCurve,anent,ent);
	RWStepGeom_RWTrimmedCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 310:
      {
	DeclareAndCast(StepBasic_UncertaintyMeasureWithUnit,anent,ent);
	RWStepBasic_RWUncertaintyMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 311:
      {
	DeclareAndCast(StepGeom_UniformCurve,anent,ent);
	RWStepGeom_RWUniformCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 312:
      {
	DeclareAndCast(StepGeom_UniformSurface,anent,ent);
	RWStepGeom_RWUniformSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 313:
      {
	DeclareAndCast(StepGeom_Vector,anent,ent);
	RWStepGeom_RWVector tool;
	tool.Share(anent,iter);
      }
      break;
    case 315:
      {
	DeclareAndCast(StepShape_VertexLoop,anent,ent);
	RWStepShape_RWVertexLoop tool;
	tool.Share(anent,iter);
      }
      break;
    case 316:
      {
	DeclareAndCast(StepShape_VertexPoint,anent,ent);
	RWStepShape_RWVertexPoint tool;
	tool.Share(anent,iter);
      }
      break;
    case 317:
      {
	DeclareAndCast(StepVisual_ViewVolume,anent,ent);
	RWStepVisual_RWViewVolume tool;
	tool.Share(anent,iter);
      }
      break;
    case 319:
      {
	DeclareAndCast(StepGeom_UniformCurveAndRationalBSplineCurve,anent,ent);
	RWStepGeom_RWUniformCurveAndRationalBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 320:
      {
	DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve,anent,ent);
	RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 321:
      {
	DeclareAndCast(StepGeom_QuasiUniformCurveAndRationalBSplineCurve,anent,ent);
	RWStepGeom_RWQuasiUniformCurveAndRationalBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 322:
      {
	DeclareAndCast(StepGeom_BezierCurveAndRationalBSplineCurve,anent,ent);
	RWStepGeom_RWBezierCurveAndRationalBSplineCurve tool;
	tool.Share(anent,iter);
      }
      break;
    case 323:
      {
	DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface,anent,ent);
	RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 324:
      {
	DeclareAndCast(StepGeom_UniformSurfaceAndRationalBSplineSurface,anent,ent);
	RWStepGeom_RWUniformSurfaceAndRationalBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 325:
      {
	DeclareAndCast(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface,anent,ent);
	RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 326:
      {
	DeclareAndCast(StepGeom_BezierSurfaceAndRationalBSplineSurface,anent,ent);
	RWStepGeom_RWBezierSurfaceAndRationalBSplineSurface tool;
	tool.Share(anent,iter);
      }
      break;
    case 329:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndLengthUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnitAndLengthUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 330:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndPlaneAngleUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnitAndPlaneAngleUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 331:
      {
	DeclareAndCast(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext,anent,ent);
	RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 332:
      {
	DeclareAndCast(StepShape_LoopAndPath,anent,ent);
	RWStepShape_RWLoopAndPath tool;
	tool.Share(anent,iter);
      }
      break;
      
      // ------------
      // Added by FMA
      // ------------
      
    case 333 :
      {
	DeclareAndCast(StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx, anent, ent);
	RWStepGeom_RWGeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx tool;
	tool.Share(anent,iter);
      }
      break;
    case 334 :
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndSolidAngleUnit, anent, ent);
	RWStepBasic_RWConversionBasedUnitAndSolidAngleUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 336 :
      {
	DeclareAndCast(StepBasic_SolidAngleUnit, anent, ent);
	RWStepBasic_RWSolidAngleUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 337 :
      {
	DeclareAndCast(StepShape_FacetedBrepAndBrepWithVoids, anent, ent);
	RWStepShape_RWFacetedBrepAndBrepWithVoids tool;
	tool.Share(anent,iter);
      }
      break;
    case 338:
      {
	DeclareAndCast(StepGeom_GeometricRepresentationContextAndParametricRepresentationContext,anent,ent);
	RWStepGeom_RWGeometricRepresentationContextAndParametricRepresentationContext tool;
	tool.Share(anent,iter);
      }
      break;
    case 339:
      {
	DeclareAndCast(StepBasic_MechanicalContext,anent,ent);
	RWStepBasic_RWMechanicalContext tool;
	tool.Share(anent,iter);
      }
      break;
      
      // ------------
      // Added by CKY
      // ------------

    case 340:
      {
	DeclareAndCast(StepBasic_ProductDefinitionContext,anent,ent);
	RWStepBasic_RWProductDefinitionContext tool;
	tool.Share(anent,iter);
      }
      break;

      // -----------
      // Added for Rev4
      // -----------

    case 341:  // TimeMeasureWithUnit
      {
	DeclareAndCast(StepBasic_MeasureWithUnit,anent,ent);
	RWStepBasic_RWMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;

    case 342:
    case 343:  // RatioUnit, TimeUnit
      {
	DeclareAndCast(StepBasic_NamedUnit,anent,ent);
	RWStepBasic_RWNamedUnit tool;
	tool.Share(anent,iter);
      }
      break;
//  343 a 347 : no Shared
    case 348:  // ApprovalDateTime
      {
	DeclareAndCast(StepBasic_ApprovalDateTime,anent,ent);
	RWStepBasic_RWApprovalDateTime tool;
	tool.Share(anent,iter);
      }
      break;
    case 349: // CameraImage 2d and 3d
    case 350:
      {
	DeclareAndCast(StepVisual_CameraImage,anent,ent);
	RWStepVisual_RWCameraImage tool;
	tool.Share(anent,iter);
      }
      break;
    case 351:
      {
	DeclareAndCast(StepGeom_CartesianTransformationOperator,anent,ent);
	RWStepGeom_RWCartesianTransformationOperator tool;
	tool.Share(anent,iter);
      }
      break;
    case 352:
      {
	DeclareAndCast(StepBasic_DerivedUnit,anent,ent);
	RWStepBasic_RWDerivedUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 353:
      {
	DeclareAndCast(StepBasic_DerivedUnitElement,anent,ent);
	RWStepBasic_RWDerivedUnitElement tool;
	tool.Share(anent,iter);
      }
      break;
    case 354:
      {
	DeclareAndCast(StepRepr_ItemDefinedTransformation,anent,ent);
	RWStepRepr_RWItemDefinedTransformation tool;
	tool.Share(anent,iter);
      }
      break;
    case 355:
      {
	DeclareAndCast(StepVisual_PresentedItemRepresentation,anent,ent);
	RWStepVisual_RWPresentedItemRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 356:
      {
	DeclareAndCast(StepVisual_PresentationLayerUsage,anent,ent);
	RWStepVisual_RWPresentationLayerUsage tool;
	tool.Share(anent,iter);
      }
      break;
    //:n5 abv 15 Feb 99: S4132 complex type bounded_curve + surface_curve
    case 358:
      {
	DeclareAndCast(StepGeom_SurfaceCurveAndBoundedCurve,anent,ent);
	RWStepGeom_RWSurfaceCurveAndBoundedCurve tool;
	tool.Share(anent,iter);
      }
      break;

//  AP214 : CC1 -> CC2
    case 366:
      {
	DeclareAndCast(StepAP214_AutoDesignDocumentReference,anent,ent);
	RWStepAP214_RWAutoDesignDocumentReference tool;
	tool.Share(anent,iter);
      }
      break;
    case 367:
    case 368:
      {
	DeclareAndCast(StepBasic_Document,anent,ent);
	RWStepBasic_RWDocument tool;
	tool.Share(anent,iter);
      }
      break;
    case 369:
      {
	DeclareAndCast(StepBasic_DocumentRelationship,anent,ent);
	RWStepBasic_RWDocumentRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 370:
      {
	DeclareAndCast(StepBasic_DocumentType,anent,ent);
	RWStepBasic_RWDocumentType tool;
	tool.Share(anent,iter);
      }
      break;
    case 371:
      {
	DeclareAndCast(StepBasic_DocumentUsageConstraint,anent,ent);
	RWStepBasic_RWDocumentUsageConstraint tool;
	tool.Share(anent,iter);
      }
      break;
    case 372:
      {
	DeclareAndCast(StepBasic_Effectivity,anent,ent);
	RWStepBasic_RWEffectivity tool;
	tool.Share(anent,iter);
      }
      break;
    case 373:
      {
	DeclareAndCast(StepBasic_ProductDefinitionEffectivity,anent,ent);
	RWStepBasic_RWProductDefinitionEffectivity tool;
	tool.Share(anent,iter);
      }
      break;
    case 374:
      {
	DeclareAndCast(StepBasic_ProductDefinitionRelationship,anent,ent);
	RWStepBasic_RWProductDefinitionRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 375:
      {
	DeclareAndCast(StepBasic_ProductDefinitionWithAssociatedDocuments,anent,ent);
	RWStepBasic_RWProductDefinitionWithAssociatedDocuments tool;
	tool.Share(anent,iter);
      }
      break;
    case 376:
      {
	DeclareAndCast(StepBasic_PhysicallyModeledProductDefinition,anent,ent);
	RWStepBasic_RWProductDefinition tool;
	tool.Share(anent,iter);
      }
      break;

    case 377:
      {
	DeclareAndCast(StepRepr_ProductDefinitionUsage,anent,ent);
	RWStepBasic_RWProductDefinitionRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 378:
      {
	DeclareAndCast(StepRepr_MakeFromUsageOption,anent,ent);
	RWStepRepr_RWMakeFromUsageOption tool;
	tool.Share(anent,iter);
      }
      break;
    case 379:
    case 380:
    case 381:
      {
	DeclareAndCast(StepRepr_AssemblyComponentUsage,anent,ent);
	RWStepRepr_RWAssemblyComponentUsage tool;
	tool.Share(anent,iter);
      }
      break;
    case 382:
      {
	DeclareAndCast(StepRepr_QuantifiedAssemblyComponentUsage,anent,ent);
	RWStepRepr_RWQuantifiedAssemblyComponentUsage tool;
	tool.Share(anent,iter);
      }
      break;
    case 383:
      {
	DeclareAndCast(StepRepr_SpecifiedHigherUsageOccurrence,anent,ent);
	RWStepRepr_RWSpecifiedHigherUsageOccurrence tool;
	tool.Share(anent,iter);
      }
      break;
    case 384:
      {
	DeclareAndCast(StepRepr_AssemblyComponentUsageSubstitute,anent,ent);
	RWStepRepr_RWAssemblyComponentUsageSubstitute tool;
	tool.Share(anent,iter);
      }
      break;
    case 385:
      {
	DeclareAndCast(StepRepr_SuppliedPartRelationship,anent,ent);
	RWStepBasic_RWProductDefinitionRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 386:
      {
	DeclareAndCast(StepRepr_ExternallyDefinedRepresentation,anent,ent);
	RWStepRepr_RWRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 387:
      {
	DeclareAndCast(StepRepr_ShapeRepresentationRelationship,anent,ent);
	RWStepRepr_RWRepresentationRelationship tool;
	tool.Share(anent,iter);
      }
      break;
    case 388:
      {
	DeclareAndCast(StepRepr_RepresentationRelationshipWithTransformation,anent,ent);
	RWStepRepr_RWRepresentationRelationshipWithTransformation tool;
	tool.Share(anent,iter);
      }
      break;

    case 389:
      {
	DeclareAndCast(StepRepr_ShapeRepresentationRelationshipWithTransformation,anent,ent);
	RWStepRepr_RWShapeRepresentationRelationshipWithTransformation tool;
	tool.Share(anent,iter);
      }
      break;

    case 390:
      {
	DeclareAndCast(StepRepr_MaterialDesignation,anent,ent);
	RWStepRepr_RWMaterialDesignation tool;
	tool.Share (anent,iter);
      }
      break;

    case 391:
      {
	DeclareAndCast(StepShape_ContextDependentShapeRepresentation,anent,ent);
	RWStepShape_RWContextDependentShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
      
    //:S4134: Added from CD to DIS
    case 392:
      {
	DeclareAndCast(StepAP214_AppliedDateAndTimeAssignment,anent,ent);
	RWStepAP214_RWAppliedDateAndTimeAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 393:
      {
	DeclareAndCast(StepAP214_AppliedDateAssignment,anent,ent);
	RWStepAP214_RWAppliedDateAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 394:
      {
	DeclareAndCast(StepAP214_AppliedApprovalAssignment,anent,ent);
	RWStepAP214_RWAppliedApprovalAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 395:
      {
	DeclareAndCast(StepAP214_AppliedGroupAssignment,anent,ent);
	RWStepAP214_RWAppliedGroupAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 396:
      {
	DeclareAndCast(StepAP214_AppliedOrganizationAssignment,anent,ent);
	RWStepAP214_RWAppliedOrganizationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 397:
      {
	DeclareAndCast(StepAP214_AppliedPersonAndOrganizationAssignment,anent,ent);
	RWStepAP214_RWAppliedPersonAndOrganizationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 398:
      {
	DeclareAndCast(StepAP214_AppliedPresentedItem,anent,ent);
	RWStepAP214_RWAppliedPresentedItem tool;
	tool.Share (anent,iter);
      }
      break;
    case 399:
      {
	DeclareAndCast(StepAP214_AppliedSecurityClassificationAssignment,anent,ent);
	RWStepAP214_RWAppliedSecurityClassificationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 400:
      {
	DeclareAndCast(StepAP214_AppliedDocumentReference,anent,ent);
	RWStepAP214_RWAppliedDocumentReference tool;
	tool.Share (anent,iter);
      }
      break;
    case 401:
      {
	DeclareAndCast(StepBasic_DocumentFile,anent,ent);
	RWStepBasic_RWDocumentFile tool;
	tool.Share (anent,iter);
      }
      break;
    case 402:
      {
//	DeclareAndCast(StepBasic_CharacterizedObject,anent,ent);
//	RWStepBasic_RWCharacterizedObject tool;
//	tool.Share (anent,iter);
      }
      break;
    case 403:
      {
	DeclareAndCast(StepShape_ExtrudedFaceSolid,anent,ent);
	RWStepShape_RWExtrudedFaceSolid tool;
	tool.Share (anent,iter);
      }
      break;
    
    case 404:
      {
	DeclareAndCast(StepShape_RevolvedFaceSolid,anent,ent);
	RWStepShape_RWRevolvedFaceSolid tool;
	tool.Share (anent,iter);
      }
      break;
    case 405:
      {
	DeclareAndCast(StepShape_SweptFaceSolid,anent,ent);
	RWStepShape_RWSweptFaceSolid tool;
	tool.Share (anent,iter);
      }
      break;
      
    // Added by ABV 08.09.99 for CAX TRJ 2 (validation properties)
    case 406:
      {
	DeclareAndCast(StepRepr_MeasureRepresentationItem,anent,ent);
	RWStepRepr_RWMeasureRepresentationItem tool;
	tool.Share (anent,iter);
      }
      break;
    case 407:
      {
	DeclareAndCast(StepBasic_AreaUnit,anent,ent);
	RWStepBasic_RWNamedUnit tool;
	tool.Share (anent,iter);
      }
      break;
    case 408:
      {
	DeclareAndCast(StepBasic_VolumeUnit,anent,ent);
	RWStepBasic_RWNamedUnit tool;
	tool.Share (anent,iter);
      }
      break;
    case 411:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndAreaUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnitAndAreaUnit tool;
	tool.Share (anent,iter);
      }
      break;
    case 412:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndVolumeUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnitAndVolumeUnit tool;
	tool.Share (anent,iter);
      }
      break;

    // Added by ABV 10.11.99 for AP203
    case 413:
      {
	DeclareAndCast(StepBasic_Action,anent,ent);
	RWStepBasic_RWAction tool;
	tool.Share (anent,iter);
      }
      break;
    case 414:
      {
	DeclareAndCast(StepBasic_ActionAssignment,anent,ent);
	RWStepBasic_RWActionAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 415:
      {
	DeclareAndCast(StepBasic_ActionMethod,anent,ent);
	RWStepBasic_RWActionMethod tool;
	tool.Share (anent,iter);
      }
      break;
    case 416:
      {
	DeclareAndCast(StepBasic_ActionRequestAssignment,anent,ent);
	RWStepBasic_RWActionRequestAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 417:
      {
	DeclareAndCast(StepAP203_CcDesignApproval,anent,ent);
	RWStepAP203_RWCcDesignApproval tool;
	tool.Share (anent,iter);
      }
      break;
    case 418:
      {
	DeclareAndCast(StepAP203_CcDesignCertification,anent,ent);
	RWStepAP203_RWCcDesignCertification tool;
	tool.Share (anent,iter);
      }
      break;
    case 419:
      {
	DeclareAndCast(StepAP203_CcDesignContract,anent,ent);
	RWStepAP203_RWCcDesignContract tool;
	tool.Share (anent,iter);
      }
      break;
    case 420:
      {
	DeclareAndCast(StepAP203_CcDesignDateAndTimeAssignment,anent,ent);
	RWStepAP203_RWCcDesignDateAndTimeAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 421:
      {
	DeclareAndCast(StepAP203_CcDesignPersonAndOrganizationAssignment,anent,ent);
	RWStepAP203_RWCcDesignPersonAndOrganizationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 422:
      {
	DeclareAndCast(StepAP203_CcDesignSecurityClassification,anent,ent);
	RWStepAP203_RWCcDesignSecurityClassification tool;
	tool.Share (anent,iter);
      }
      break;
    case 423:
      {
	DeclareAndCast(StepAP203_CcDesignSpecificationReference,anent,ent);
	RWStepAP203_RWCcDesignSpecificationReference tool;
	tool.Share (anent,iter);
      }
      break;
    case 424:
      {
	DeclareAndCast(StepBasic_Certification,anent,ent);
	RWStepBasic_RWCertification tool;
	tool.Share (anent,iter);
      }
      break;
    case 425:
      {
	DeclareAndCast(StepBasic_CertificationAssignment,anent,ent);
	RWStepBasic_RWCertificationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 426:
      {
	DeclareAndCast(StepBasic_CertificationType,anent,ent);
	RWStepBasic_RWCertificationType tool;
	tool.Share (anent,iter);
      }
      break;
    case 427:
      {
	DeclareAndCast(StepAP203_Change,anent,ent);
	RWStepAP203_RWChange tool;
	tool.Share (anent,iter);
      }
      break;
    case 428:
      {
	DeclareAndCast(StepAP203_ChangeRequest,anent,ent);
	RWStepAP203_RWChangeRequest tool;
	tool.Share (anent,iter);
      }
      break;
    case 429:
      {
	DeclareAndCast(StepRepr_ConfigurationDesign,anent,ent);
	RWStepRepr_RWConfigurationDesign tool;
	tool.Share (anent,iter);
      }
      break;
    case 430:
      {
	DeclareAndCast(StepRepr_ConfigurationEffectivity,anent,ent);
	RWStepRepr_RWConfigurationEffectivity tool;
	tool.Share (anent,iter);
      }
      break;
    case 431:
      {
	DeclareAndCast(StepBasic_Contract,anent,ent);
	RWStepBasic_RWContract tool;
	tool.Share (anent,iter);
      }
      break;
    case 432:
      {
	DeclareAndCast(StepBasic_ContractAssignment,anent,ent);
	RWStepBasic_RWContractAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 433:
      {
	DeclareAndCast(StepBasic_ContractType,anent,ent);
	RWStepBasic_RWContractType tool;
	tool.Share (anent,iter);
      }
      break;
    case 434:
      {
	DeclareAndCast(StepRepr_ProductConcept,anent,ent);
	RWStepRepr_RWProductConcept tool;
	tool.Share (anent,iter);
      }
      break;
    case 435:
      {
	DeclareAndCast(StepBasic_ProductConceptContext,anent,ent);
	RWStepBasic_RWProductConceptContext tool;
	tool.Share (anent,iter);
      }
      break;
    case 436:
      {
	DeclareAndCast(StepAP203_StartRequest,anent,ent);
	RWStepAP203_RWStartRequest tool;
	tool.Share (anent,iter);
      }
      break;
    case 437:
      {
	DeclareAndCast(StepAP203_StartWork,anent,ent);
	RWStepAP203_RWStartWork tool;
	tool.Share (anent,iter);
      }
      break;
    case 438:
      {
	DeclareAndCast(StepBasic_VersionedActionRequest,anent,ent);
	RWStepBasic_RWVersionedActionRequest tool;
	tool.Share (anent,iter);
      }
      break;
    case 439:
      {
	DeclareAndCast(StepBasic_ProductCategoryRelationship,anent,ent);
	RWStepBasic_RWProductCategoryRelationship tool;
	tool.Share (anent,iter);
      }
      break;
    case 440:
      {
	DeclareAndCast(StepBasic_ActionRequestSolution,anent,ent);
	RWStepBasic_RWActionRequestSolution tool;
	tool.Share (anent,iter);
      }
      break;

    case 441:
      {
	DeclareAndCast(StepVisual_DraughtingModel,anent,ent);
	RWStepVisual_RWDraughtingModel tool;
	tool.Share (anent,iter);
      }
      break;

      // Added by ABV 18.04.00 for CAX-IF TRJ4
    case 442:
      {
	DeclareAndCast(StepShape_AngularLocation,anent,ent);
	RWStepShape_RWAngularLocation tool;
	tool.Share (anent,iter);
      }
      break;
    case 443:
      {
	DeclareAndCast(StepShape_AngularSize,anent,ent);
	RWStepShape_RWAngularSize tool;
	tool.Share (anent,iter);
      }
      break;
    case 444:
      {
	DeclareAndCast(StepShape_DimensionalCharacteristicRepresentation,anent,ent);
	RWStepShape_RWDimensionalCharacteristicRepresentation tool;
	tool.Share (anent,iter);
      }
      break;
    case 445:
      {
	DeclareAndCast(StepShape_DimensionalLocation,anent,ent);
	RWStepShape_RWDimensionalLocation tool;
	tool.Share (anent,iter);
      }
      break;
    case 446:
      {
	DeclareAndCast(StepShape_DimensionalLocationWithPath,anent,ent);
	RWStepShape_RWDimensionalLocationWithPath tool;
	tool.Share (anent,iter);
      }
      break;
    case 447:
      {
	DeclareAndCast(StepShape_DimensionalSize,anent,ent);
	RWStepShape_RWDimensionalSize tool;
	tool.Share (anent,iter);
      }
      break;
    case 448:
      {
	DeclareAndCast(StepShape_DimensionalSizeWithPath,anent,ent);
	RWStepShape_RWDimensionalSizeWithPath tool;
	tool.Share (anent,iter);
      }
      break;
    case 449:
      {
	DeclareAndCast(StepShape_ShapeDimensionRepresentation,anent,ent);
	RWStepShape_RWShapeDimensionRepresentation tool;
	tool.Share (anent,iter);
      }
      break;

      // Added by ABV 10.05.00 for CAX-IF TRJ4 (external references)
    case 450:
      {
	DeclareAndCast(StepBasic_DocumentRepresentationType,anent,ent);
	RWStepBasic_RWDocumentRepresentationType tool;
	tool.Share (anent,iter);
      }
      break;
    case 451:
      {
	DeclareAndCast(StepBasic_ObjectRole,anent,ent);
	RWStepBasic_RWObjectRole tool;
	tool.Share (anent,iter);
      }
      break;
    case 452:
      {
	DeclareAndCast(StepBasic_RoleAssociation,anent,ent);
	RWStepBasic_RWRoleAssociation tool;
	tool.Share (anent,iter);
      }
      break;
    case 453:
      {
	DeclareAndCast(StepBasic_IdentificationRole,anent,ent);
	RWStepBasic_RWIdentificationRole tool;
	tool.Share (anent,iter);
      }
      break;
    case 454:
      {
	DeclareAndCast(StepBasic_IdentificationAssignment,anent,ent);
	RWStepBasic_RWIdentificationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 455:
      {
	DeclareAndCast(StepBasic_ExternalIdentificationAssignment,anent,ent);
	RWStepBasic_RWExternalIdentificationAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 456:
      {
	DeclareAndCast(StepBasic_EffectivityAssignment,anent,ent);
	RWStepBasic_RWEffectivityAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 457:
      {
	DeclareAndCast(StepBasic_NameAssignment,anent,ent);
	RWStepBasic_RWNameAssignment tool;
	tool.Share (anent,iter);
      }
      break;
    case 458:
      {
	DeclareAndCast(StepBasic_GeneralProperty,anent,ent);
	RWStepBasic_RWGeneralProperty tool;
	tool.Share (anent,iter);
      }
      break;
    case 459:
      {
	DeclareAndCast(StepAP214_Class,anent,ent);
	RWStepAP214_RWClass tool;
	tool.Share (anent,iter);
      }
      break;
    case 460:
      {
	DeclareAndCast(StepAP214_ExternallyDefinedClass,anent,ent);
	RWStepAP214_RWExternallyDefinedClass tool;
	tool.Share (anent,iter);
      }
      break;
    case 461:
      {
	DeclareAndCast(StepAP214_ExternallyDefinedGeneralProperty,anent,ent);
	RWStepAP214_RWExternallyDefinedGeneralProperty tool;
	tool.Share (anent,iter);
      }
      break;
    case 462:
      {
	DeclareAndCast(StepAP214_AppliedExternalIdentificationAssignment,anent,ent);
	RWStepAP214_RWAppliedExternalIdentificationAssignment tool;
	tool.Share (anent,iter);
      }
      break;

    case 463:
      {
	DeclareAndCast(StepShape_DefinitionalRepresentationAndShapeRepresentation,anent,ent);
	RWStepShape_RWDefinitionalRepresentationAndShapeRepresentation tool;
	tool.Share (anent,iter);
      }
      break;

      // Added by CKY 25 APR 2001 for CAX-IF TRJ7 (dimensional tolerances)
    case 470:
      {
        DeclareAndCast(StepRepr_CompositeShapeAspect,anent,ent);
	RWStepRepr_RWCompositeShapeAspect tool;
	tool.Share(anent,iter);
      }
      break;
    case 471: 
      {
      DeclareAndCast(StepRepr_DerivedShapeAspect,anent,ent);
      RWStepRepr_RWDerivedShapeAspect tool;
      tool.Share(anent,iter);
      }
      break;
    case 472:  // same as ShapeAspect
      {
	DeclareAndCast(StepRepr_Extension,anent,ent);
	RWStepRepr_RWExtension tool;
	tool.Share(anent,iter);
      }
      break;
    case 473:  // same as DimensionalLocation
      {
	DeclareAndCast(StepShape_DirectedDimensionalLocation,anent,ent);
	RWStepShape_RWDimensionalLocation tool;
	tool.Share (anent,iter);
      }
      break;
// cases 474, 478, 479 : no shared entities
    case 475:
      {
	DeclareAndCast(StepShape_ToleranceValue,anent,ent);
	RWStepShape_RWToleranceValue tool;
	tool.Share(anent,iter);
      }
      break;
    case 476:
      {
	DeclareAndCast(StepShape_MeasureQualification,anent,ent);
	RWStepShape_RWMeasureQualification tool;
	tool.Share(anent,iter);
      }
      break;
    case 477:
      {
	DeclareAndCast(StepShape_PlusMinusTolerance,anent,ent);
	RWStepShape_RWPlusMinusTolerance tool;
	tool.Share(anent,iter);
      }
      break;
    case 480:
      {
	DeclareAndCast(StepShape_QualifiedRepresentationItem,anent,ent);
	RWStepShape_RWQualifiedRepresentationItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 481:
      {
	DeclareAndCast(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,anent,ent);
	RWStepShape_RWMeasureRepresentationItemAndQualifiedRepresentationItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 482:
    case 483:
      {
	DeclareAndCast(StepRepr_CompoundRepresentationItem,anent,ent);
	RWStepRepr_RWCompoundRepresentationItem tool;
	tool.Share(anent,iter);
      }
      break;
    case 484:  // same as ShapeAspectRelationship
      {
	DeclareAndCast(StepRepr_ShapeAspectRelationship,anent,ent);
	RWStepRepr_RWShapeAspectRelationship tool;
	tool.Share(anent,iter);
      }
      break;

    // abv 28.12.01
    case 485:
      {
	DeclareAndCast(StepShape_CompoundShapeRepresentation,anent,ent);
	RWStepShape_RWCompoundShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 486:
      {
	DeclareAndCast(StepShape_ConnectedEdgeSet,anent,ent);
	RWStepShape_RWConnectedEdgeSet tool;
	tool.Share(anent,iter);
      }
      break;
    case 487:
      {
	DeclareAndCast(StepShape_ConnectedFaceShapeRepresentation,anent,ent);
	RWStepShape_RWConnectedFaceShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 488:
      {
	DeclareAndCast(StepShape_EdgeBasedWireframeModel,anent,ent);
	RWStepShape_RWEdgeBasedWireframeModel tool;
	tool.Share(anent,iter);
      }
      break;
    case 489:
      {
	DeclareAndCast(StepShape_EdgeBasedWireframeShapeRepresentation,anent,ent);
	RWStepShape_RWEdgeBasedWireframeShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
    case 490:
      {
	DeclareAndCast(StepShape_FaceBasedSurfaceModel,anent,ent);
	RWStepShape_RWFaceBasedSurfaceModel tool;
	tool.Share(anent,iter);
      }
      break;
    case 491:
      {
	DeclareAndCast(StepShape_NonManifoldSurfaceShapeRepresentation,anent,ent);
	RWStepShape_RWNonManifoldSurfaceShapeRepresentation tool;
	tool.Share(anent,iter);
      }
      break;
      //gka 0.8.01.02 TRJ9
      case 492:
    {
      DeclareAndCast(StepGeom_OrientedSurface,anent,ent);
      RWStepGeom_RWOrientedSurface tool;
      tool.Share(anent,iter);
    }
    break;
    case 493:
    {
      DeclareAndCast(StepShape_Subface,anent,ent);
      RWStepShape_RWSubface tool;
      tool.Share(anent,iter);
    }
    break;
    case 494:
    {
      DeclareAndCast(StepShape_Subedge,anent,ent);
      RWStepShape_RWSubedge tool;
      tool.Share(anent,iter);
    }
    break;
    case 495:
    {
      DeclareAndCast(StepShape_SeamEdge,anent,ent);
      RWStepShape_RWSeamEdge tool;
      tool.Share(anent,iter);
    }
    break;
    case 496:
    {
      DeclareAndCast(StepShape_ConnectedFaceSubSet,anent,ent);
      RWStepShape_RWConnectedFaceSubSet tool;
      tool.Share(anent,iter);
    }
    break;
    case 500:
    {
      DeclareAndCast(StepBasic_EulerAngles,anent,ent);
      RWStepBasic_RWEulerAngles tool;
      tool.Share(anent,iter);
    }
    break;
    case 501:
      {
        DeclareAndCast(StepBasic_MassUnit,anent,ent);
        RWStepBasic_RWMassUnit tool;
        tool.Share(anent,iter);
      }
      break;
    case 502:
      {
        DeclareAndCast(StepBasic_ThermodynamicTemperatureUnit,anent,ent);
        RWStepBasic_RWThermodynamicTemperatureUnit tool;
        tool.Share(anent,iter);
      }
      break;
    case 503:
      {
        DeclareAndCast(StepElement_AnalysisItemWithinRepresentation,anent,ent);
        RWStepElement_RWAnalysisItemWithinRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 504:
      {
        DeclareAndCast(StepElement_Curve3dElementDescriptor,anent,ent);
        RWStepElement_RWCurve3dElementDescriptor tool;
        tool.Share(anent,iter);
      }
      break;
    case 505:
      {
        DeclareAndCast(StepElement_CurveElementEndReleasePacket,anent,ent);
        RWStepElement_RWCurveElementEndReleasePacket tool;
        tool.Share(anent,iter);
      }
      break;
    case 506:
      {
        DeclareAndCast(StepElement_CurveElementSectionDefinition,anent,ent);
        RWStepElement_RWCurveElementSectionDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    case 507:
      {
        DeclareAndCast(StepElement_CurveElementSectionDerivedDefinitions,anent,ent);
        RWStepElement_RWCurveElementSectionDerivedDefinitions tool;
        tool.Share(anent,iter);
      }
      break;
    case 508:
      {
        DeclareAndCast(StepElement_ElementDescriptor,anent,ent);
        RWStepElement_RWElementDescriptor tool;
        tool.Share(anent,iter);
      }
      break;
    case 509:
      {
        DeclareAndCast(StepElement_ElementMaterial,anent,ent);
        RWStepElement_RWElementMaterial tool;
        tool.Share(anent,iter);
      }
      break;
    case 510:
      {
        DeclareAndCast(StepElement_Surface3dElementDescriptor,anent,ent);
        RWStepElement_RWSurface3dElementDescriptor tool;
        tool.Share(anent,iter);
      }
      break;
    case 511:
      {
        DeclareAndCast(StepElement_SurfaceElementProperty,anent,ent);
        RWStepElement_RWSurfaceElementProperty tool;
        tool.Share(anent,iter);
      }
      break;
    case 512:
      {
        DeclareAndCast(StepElement_SurfaceSection,anent,ent);
        RWStepElement_RWSurfaceSection tool;
        tool.Share(anent,iter);
      }
      break;
    case 513:
      {
        DeclareAndCast(StepElement_SurfaceSectionField,anent,ent);
        RWStepElement_RWSurfaceSectionField tool;
        tool.Share(anent,iter);
      }
      break;
    case 514:
      {
        DeclareAndCast(StepElement_SurfaceSectionFieldConstant,anent,ent);
        RWStepElement_RWSurfaceSectionFieldConstant tool;
        tool.Share(anent,iter);
      }
      break;
    case 515:
      {
        DeclareAndCast(StepElement_SurfaceSectionFieldVarying,anent,ent);
        RWStepElement_RWSurfaceSectionFieldVarying tool;
        tool.Share(anent,iter);
      }
      break;
    case 516:
      {
        DeclareAndCast(StepElement_UniformSurfaceSection,anent,ent);
        RWStepElement_RWUniformSurfaceSection tool;
        tool.Share(anent,iter);
      }
      break;
    case 517:
      {
        DeclareAndCast(StepElement_Volume3dElementDescriptor,anent,ent);
        RWStepElement_RWVolume3dElementDescriptor tool;
        tool.Share(anent,iter);
      }
      break;
    case 518:
      {
        DeclareAndCast(StepFEA_AlignedCurve3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWAlignedCurve3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 519:
      {
        DeclareAndCast(StepFEA_ArbitraryVolume3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 520:
      {
        DeclareAndCast(StepFEA_Curve3dElementProperty,anent,ent);
        RWStepFEA_RWCurve3dElementProperty tool;
        tool.Share(anent,iter);
      }
      break;
    case 521:
      {
        DeclareAndCast(StepFEA_Curve3dElementRepresentation,anent,ent);
        RWStepFEA_RWCurve3dElementRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 522:
      {
        DeclareAndCast(StepFEA_Node,anent,ent);
        RWStepFEA_RWNode tool;
        tool.Share(anent,iter);
      }
      break;
//case 523:
//    {
//      DeclareAndCast(StepFEA_CurveElementEndCoordinateSystem,anent,ent);
//      RWStepFEA_RWCurveElementEndCoordinateSystem tool;
//      tool.Share(anent,iter);
//    }
//    break;
    case 524:
      {
        DeclareAndCast(StepFEA_CurveElementEndOffset,anent,ent);
        RWStepFEA_RWCurveElementEndOffset tool;
        tool.Share(anent,iter);
      }
      break;
    case 525:
      {
        DeclareAndCast(StepFEA_CurveElementEndRelease,anent,ent);
        RWStepFEA_RWCurveElementEndRelease tool;
        tool.Share(anent,iter);
      }
      break;
    case 526:
      {
        DeclareAndCast(StepFEA_CurveElementInterval,anent,ent);
        RWStepFEA_RWCurveElementInterval tool;
        tool.Share(anent,iter);
      }
      break;
    case 527:
      {
        DeclareAndCast(StepFEA_CurveElementIntervalConstant,anent,ent);
        RWStepFEA_RWCurveElementIntervalConstant tool;
        tool.Share(anent,iter);
      }
      break;
    case 528:
      {
        DeclareAndCast(StepFEA_DummyNode,anent,ent);
        RWStepFEA_RWDummyNode tool;
        tool.Share(anent,iter);
      }
      break;
    case 529:
      {
        DeclareAndCast(StepFEA_CurveElementLocation,anent,ent);
        RWStepFEA_RWCurveElementLocation tool;
        tool.Share(anent,iter);
      }
      break;
    case 530:
      {
        DeclareAndCast(StepFEA_ElementGeometricRelationship,anent,ent);
        RWStepFEA_RWElementGeometricRelationship tool;
        tool.Share(anent,iter);
      }
      break;
    case 531:
      {
        DeclareAndCast(StepFEA_ElementGroup,anent,ent);
        RWStepFEA_RWElementGroup tool;
        tool.Share(anent,iter);
      }
      break;
    case 532:
      {
        DeclareAndCast(StepFEA_ElementRepresentation,anent,ent);
        RWStepFEA_RWElementRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 533:
      {
        DeclareAndCast(StepFEA_FeaAreaDensity,anent,ent);
        RWStepFEA_RWFeaAreaDensity tool;
        tool.Share(anent,iter);
      }
      break;
    case 534:
      {
        DeclareAndCast(StepFEA_FeaAxis2Placement3d,anent,ent);
        RWStepFEA_RWFeaAxis2Placement3d tool;
        tool.Share(anent,iter);
      }
      break;
    case 535:
      {
        DeclareAndCast(StepFEA_FeaGroup,anent,ent);
        RWStepFEA_RWFeaGroup tool;
        tool.Share(anent,iter);
      }
      break;
    case 536:
      {
        DeclareAndCast(StepFEA_FeaLinearElasticity,anent,ent);
        RWStepFEA_RWFeaLinearElasticity tool;
        tool.Share(anent,iter);
      }
      break;
    case 537:
      {
        DeclareAndCast(StepFEA_FeaMassDensity,anent,ent);
        RWStepFEA_RWFeaMassDensity tool;
        tool.Share(anent,iter);
      }
      break;
    case 538:
      {
        DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentation,anent,ent);
        RWStepFEA_RWFeaMaterialPropertyRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 539:
      {
        DeclareAndCast(StepFEA_FeaMaterialPropertyRepresentationItem,anent,ent);
        RWStepFEA_RWFeaMaterialPropertyRepresentationItem tool;
        tool.Share(anent,iter);
      }
      break;
    case 540:
      {
        DeclareAndCast(StepFEA_FeaModel,anent,ent);
        RWStepFEA_RWFeaModel tool;
        tool.Share(anent,iter);
      }
      break;
    case 541:
      {
        DeclareAndCast(StepFEA_FeaModel3d,anent,ent);
        RWStepFEA_RWFeaModel3d tool;
        tool.Share(anent,iter);
      }
      break;
    case 542:
      {
        DeclareAndCast(StepFEA_FeaMoistureAbsorption,anent,ent);
        RWStepFEA_RWFeaMoistureAbsorption tool;
        tool.Share(anent,iter);
      }
      break;
    case 543:
      {
        DeclareAndCast(StepFEA_FeaParametricPoint,anent,ent);
        RWStepFEA_RWFeaParametricPoint tool;
        tool.Share(anent,iter);
      }
      break;
    case 544:
      {
        DeclareAndCast(StepFEA_FeaRepresentationItem,anent,ent);
        RWStepFEA_RWFeaRepresentationItem tool;
        tool.Share(anent,iter);
      }
      break;
    case 545:
      {
        DeclareAndCast(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion,anent,ent);
        RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion tool;
        tool.Share(anent,iter);
      }
      break;
    case 546:
      {
        DeclareAndCast(StepFEA_FeaShellBendingStiffness,anent,ent);
        RWStepFEA_RWFeaShellBendingStiffness tool;
        tool.Share(anent,iter);
      }
      break;
    case 547:
      {
        DeclareAndCast(StepFEA_FeaShellMembraneBendingCouplingStiffness,anent,ent);
        RWStepFEA_RWFeaShellMembraneBendingCouplingStiffness tool;
        tool.Share(anent,iter);
      }
      break;
    case 548:
      {
        DeclareAndCast(StepFEA_FeaShellMembraneStiffness,anent,ent);
        RWStepFEA_RWFeaShellMembraneStiffness tool;
        tool.Share(anent,iter);
      }
      break;
    case 549:
      {
        DeclareAndCast(StepFEA_FeaShellShearStiffness,anent,ent);
        RWStepFEA_RWFeaShellShearStiffness tool;
        tool.Share(anent,iter);
      }
      break;
    case 550:
      {
        DeclareAndCast(StepFEA_GeometricNode,anent,ent);
        RWStepFEA_RWGeometricNode tool;
        tool.Share(anent,iter);
      }
      break;
    case 551:
      {
        DeclareAndCast(StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion,anent,ent);
        RWStepFEA_RWFeaTangentialCoefficientOfLinearThermalExpansion tool;
        tool.Share(anent,iter);
      }
      break;
    case 552:
      {
        DeclareAndCast(StepFEA_NodeGroup,anent,ent);
        RWStepFEA_RWNodeGroup tool;
        tool.Share(anent,iter);
      }
      break;
    case 553:
      {
        DeclareAndCast(StepFEA_NodeRepresentation,anent,ent);
        RWStepFEA_RWNodeRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 554:
      {
        DeclareAndCast(StepFEA_NodeSet,anent,ent);
        RWStepFEA_RWNodeSet tool;
        tool.Share(anent,iter);
      }
      break;
    case 555:
      {
        DeclareAndCast(StepFEA_NodeWithSolutionCoordinateSystem,anent,ent);
        RWStepFEA_RWNodeWithSolutionCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 556:
      {
        DeclareAndCast(StepFEA_NodeWithVector,anent,ent);
        RWStepFEA_RWNodeWithVector tool;
        tool.Share(anent,iter);
      }
      break;
    case 557:
      {
        DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateDirection,anent,ent);
        RWStepFEA_RWParametricCurve3dElementCoordinateDirection tool;
        tool.Share(anent,iter);
      }
      break;
    case 558:
      {
        DeclareAndCast(StepFEA_ParametricCurve3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWParametricCurve3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 559:
      {
        DeclareAndCast(StepFEA_ParametricSurface3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWParametricSurface3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 560:
      {
        DeclareAndCast(StepFEA_Surface3dElementRepresentation,anent,ent);
        RWStepFEA_RWSurface3dElementRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
//case 561:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor22d,anent,ent);
//      RWStepFEA_RWSymmetricTensor22d tool;
//      tool.Share(anent,iter);
//    }
//    break;
//case 562:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor42d,anent,ent);
//      RWStepFEA_RWSymmetricTensor42d tool;
//      tool.Share(anent,iter);
//    }
//    break;
//case 563:
//    {
//      DeclareAndCast(StepFEA_SymmetricTensor43d,anent,ent);
//      RWStepFEA_RWSymmetricTensor43d tool;
//      tool.Share(anent,iter);
//    }
//    break;
    case 564:
      {
        DeclareAndCast(StepFEA_Volume3dElementRepresentation,anent,ent);
        RWStepFEA_RWVolume3dElementRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 565:
      {
        DeclareAndCast(StepRepr_DataEnvironment,anent,ent);
        RWStepRepr_RWDataEnvironment tool;
        tool.Share(anent,iter);
      }
      break;
    case 566:
      {
        DeclareAndCast(StepRepr_MaterialPropertyRepresentation,anent,ent);
        RWStepRepr_RWMaterialPropertyRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 567:
      {
        DeclareAndCast(StepRepr_PropertyDefinitionRelationship,anent,ent);
        RWStepRepr_RWPropertyDefinitionRelationship tool;
        tool.Share(anent,iter);
      }
      break;
    case 568:
      {
        DeclareAndCast(StepShape_PointRepresentation,anent,ent);
        RWStepShape_RWPointRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 569:
      {
        DeclareAndCast(StepRepr_MaterialProperty,anent,ent);
        RWStepRepr_RWMaterialProperty tool;
        tool.Share(anent,iter);
      }
      break;
    case 570:
      {
        DeclareAndCast(StepFEA_FeaModelDefinition,anent,ent);
        RWStepFEA_RWFeaModelDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    case 571:
      {
        DeclareAndCast(StepFEA_FreedomAndCoefficient,anent,ent);
        RWStepFEA_RWFreedomAndCoefficient tool;
        tool.Share(anent,iter);
      }
      break;
    case 572:
      {
        DeclareAndCast(StepFEA_FreedomsList,anent,ent);
        RWStepFEA_RWFreedomsList tool;
        tool.Share(anent,iter);
      }
      break;
    case 573:
      {
        DeclareAndCast(StepBasic_ProductDefinitionFormationRelationship,anent,ent);
        RWStepBasic_RWProductDefinitionFormationRelationship tool;
        tool.Share(anent,iter);
      }
      break;
//case 574:
//    {
//      DeclareAndCast(StepFEA_FeaModelDefinition,anent,ent);
//      RWStepFEA_RWFeaModelDefinition tool;
//      tool.Share(anent,iter);
//    }
//    break;
    case 575:
      {
        DeclareAndCast(StepFEA_NodeDefinition,anent,ent);
        RWStepFEA_RWNodeDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    case 576:
      {
        DeclareAndCast(StepRepr_StructuralResponseProperty,anent,ent);
        RWStepRepr_RWStructuralResponseProperty tool;
        tool.Share(anent,iter);
      }
      break;
    case 577:
      {
        DeclareAndCast(StepRepr_StructuralResponsePropertyDefinitionRepresentation,anent,ent);
        RWStepRepr_RWStructuralResponsePropertyDefinitionRepresentation tool;
        tool.Share(anent,iter);
      }
      break;
    case 579:
      {
        DeclareAndCast(StepFEA_AlignedSurface3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWAlignedSurface3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 580:
      {
        DeclareAndCast(StepFEA_ConstantSurface3dElementCoordinateSystem,anent,ent);
        RWStepFEA_RWConstantSurface3dElementCoordinateSystem tool;
        tool.Share(anent,iter);
      }
      break;
    case 581:
      {
        DeclareAndCast(StepFEA_CurveElementIntervalLinearlyVarying,anent,ent);
        RWStepFEA_RWCurveElementIntervalLinearlyVarying tool;
        tool.Share(anent,iter);
      }
      break;
    case 582:
      {
        DeclareAndCast(StepFEA_FeaCurveSectionGeometricRelationship,anent,ent);
        RWStepFEA_RWFeaCurveSectionGeometricRelationship tool;
        tool.Share(anent,iter);
      }
      break;
    case 583:
      {
        DeclareAndCast(StepFEA_FeaSurfaceSectionGeometricRelationship,anent,ent);
        RWStepFEA_RWFeaSurfaceSectionGeometricRelationship tool;
        tool.Share(anent,iter);
      }
      break;
    case 600:
      {
        DeclareAndCast(StepBasic_DocumentProductAssociation,anent,ent);
        RWStepBasic_RWDocumentProductAssociation tool;
        tool.Share(anent,iter);
      }
      break;
    case 601:
      {
        DeclareAndCast(StepBasic_DocumentProductEquivalence,anent,ent);
        RWStepBasic_RWDocumentProductEquivalence tool;
        tool.Share(anent,iter);
      }
      break;
      
      //  TR12J 04.06.2003 G&DT entities GKA      
    case 609:
    {
      DeclareAndCast(StepDimTol_CylindricityTolerance,anent,ent);
      RWStepDimTol_RWCylindricityTolerance tool;
      tool.Share(anent,iter);
    }
    break;
    case 610:
      {
        DeclareAndCast(StepShape_ShapeRepresentationWithParameters,anent,ent);
        RWStepShape_RWShapeRepresentationWithParameters tool;
        tool.Share(anent,iter);
      }
      break;
    case 611:
      {
        DeclareAndCast(StepDimTol_AngularityTolerance,anent,ent);
        RWStepDimTol_RWAngularityTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 612:
      {
        DeclareAndCast(StepDimTol_ConcentricityTolerance,anent,ent);
        RWStepDimTol_RWConcentricityTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 613:
      {
        DeclareAndCast(StepDimTol_CircularRunoutTolerance,anent,ent);
        RWStepDimTol_RWCircularRunoutTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 614:
      {
        DeclareAndCast(StepDimTol_CoaxialityTolerance,anent,ent);
        RWStepDimTol_RWCoaxialityTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 615:
      {
        DeclareAndCast(StepDimTol_FlatnessTolerance,anent,ent);
        RWStepDimTol_RWFlatnessTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 616:
      {
        DeclareAndCast(StepDimTol_LineProfileTolerance,anent,ent);
        RWStepDimTol_RWLineProfileTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 617:
      {
        DeclareAndCast(StepDimTol_ParallelismTolerance,anent,ent);
        RWStepDimTol_RWParallelismTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 618:
      {
        DeclareAndCast(StepDimTol_PerpendicularityTolerance,anent,ent);
        RWStepDimTol_RWPerpendicularityTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 619:
      {
        DeclareAndCast(StepDimTol_PositionTolerance,anent,ent);
        RWStepDimTol_RWPositionTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 620:
      {
        DeclareAndCast(StepDimTol_RoundnessTolerance,anent,ent);
        RWStepDimTol_RWRoundnessTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 621:
      {
        DeclareAndCast(StepDimTol_StraightnessTolerance,anent,ent);
        RWStepDimTol_RWStraightnessTolerance tool;
        tool.Share(anent,iter);
      }
      break;  
    case 622:
      {
        DeclareAndCast(StepDimTol_SurfaceProfileTolerance,anent,ent);
        RWStepDimTol_RWSurfaceProfileTolerance tool;
        tool.Share(anent,iter);
      }
      break; 
    case 623:
      {
        DeclareAndCast(StepDimTol_SymmetryTolerance,anent,ent);
        RWStepDimTol_RWSymmetryTolerance tool;
        tool.Share(anent,iter);
      }
      break; 
    case 624:
      {
        DeclareAndCast(StepDimTol_TotalRunoutTolerance,anent,ent);
        RWStepDimTol_RWTotalRunoutTolerance tool;
        tool.Share(anent,iter);
      }
      break; 
    case 625:
      {
        DeclareAndCast(StepDimTol_GeometricTolerance,anent,ent);
        RWStepDimTol_RWGeometricTolerance tool;
        tool.Share(anent,iter);
      }
      break; 
    case 626:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceRelationship,anent,ent);
        RWStepDimTol_RWGeometricToleranceRelationship tool;
        tool.Share(anent,iter);
      }
      break; 
      
    case 627:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceWithDatumReference,anent,ent);
        RWStepDimTol_RWGeometricToleranceWithDatumReference tool;
        tool.Share(anent,iter);
      }
      break; 
    case 628:
      {
        DeclareAndCast(StepDimTol_ModifiedGeometricTolerance,anent,ent);
        RWStepDimTol_RWModifiedGeometricTolerance tool;
        tool.Share(anent,iter);
      }
      break; 
    case 629:
      {
        DeclareAndCast(StepDimTol_Datum,anent,ent);
        RWStepDimTol_RWDatum tool;
        tool.Share(anent,iter);
      }
      break; 
      
    case 630:
      {
        DeclareAndCast(StepDimTol_DatumFeature,anent,ent);
        RWStepDimTol_RWDatumFeature tool;
        tool.Share(anent,iter);
      }
      break; 
    case 631:
      {
        DeclareAndCast(StepDimTol_DatumReference,anent,ent);
        RWStepDimTol_RWDatumReference tool;
        tool.Share(anent,iter);
      }
      break; 
      
    case 632:
      {
        DeclareAndCast(StepDimTol_CommonDatum,anent,ent);
        RWStepDimTol_RWCommonDatum tool;
        tool.Share(anent,iter);
      }
      break; 
    case 633:
      {
        DeclareAndCast(StepDimTol_DatumTarget,anent,ent);
        RWStepDimTol_RWDatumTarget tool;
        tool.Share(anent,iter);
      }
      break; 
    case 634:
      {
        DeclareAndCast(StepDimTol_PlacedDatumTargetFeature,anent,ent);
        RWStepDimTol_RWPlacedDatumTargetFeature tool;
        tool.Share(anent,iter);
      }
      break;   
    case 636:
      {
        DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol,anent,ent);
        RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol tool;
        tool.Share(anent,iter);
      }
      break;   
    case 650:
      {
	DeclareAndCast(StepBasic_ConversionBasedUnitAndMassUnit,anent,ent);
	RWStepBasic_RWConversionBasedUnitAndMassUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 651:
      {
	DeclareAndCast(StepBasic_MassMeasureWithUnit,anent,ent);
	RWStepBasic_RWMassMeasureWithUnit tool;
	tool.Share(anent,iter);
      }
      break;
    case 660:
      {
        DeclareAndCast(StepRepr_Apex,anent,ent);
        RWStepRepr_RWApex tool;
        tool.Share(anent,iter);
      }
      break;
    case 661:
      {
        DeclareAndCast(StepRepr_CentreOfSymmetry,anent,ent);
        RWStepRepr_RWCentreOfSymmetry tool;
        tool.Share(anent,iter);
      }
      break;
    case 662:
      {
        DeclareAndCast(StepRepr_GeometricAlignment,anent,ent);
        RWStepRepr_RWGeometricAlignment tool;
        tool.Share(anent,iter);
      }
      break;
    case 663:
      {
        DeclareAndCast(StepRepr_PerpendicularTo,anent,ent);
        RWStepRepr_RWPerpendicularTo tool;
        tool.Share(anent,iter);
      }
      break;
    case 664:
      {
        DeclareAndCast(StepRepr_Tangent,anent,ent);
        RWStepRepr_RWTangent tool;
        tool.Share(anent,iter);
      }
      break;
    case 665:
      {
        DeclareAndCast(StepRepr_ParallelOffset,anent,ent);
        RWStepRepr_RWParallelOffset tool;
        tool.Share(anent,iter);
      }
      break;
    case 666:
      {
        DeclareAndCast(StepAP242_GeometricItemSpecificUsage,anent,ent);
        RWStepAP242_RWGeometricItemSpecificUsage tool;
        tool.Share(anent,iter);
      }
      break;
    case 667:
      {
        DeclareAndCast(StepAP242_IdAttribute,anent,ent);
        RWStepAP242_RWIdAttribute tool;
        tool.Share(anent,iter);
      }
      break;
    case 668:
      {
        DeclareAndCast(StepAP242_ItemIdentifiedRepresentationUsage,anent,ent);
        RWStepAP242_RWItemIdentifiedRepresentationUsage tool;
        tool.Share(anent,iter);
      }
      break;
    case 669:
      {
        DeclareAndCast(StepRepr_AllAroundShapeAspect,anent,ent);
        RWStepRepr_RWAllAroundShapeAspect tool;
        tool.Share(anent,iter);
      }
      break;
    case 670:
      {
        DeclareAndCast(StepRepr_BetweenShapeAspect,anent,ent);
        RWStepRepr_RWBetweenShapeAspect tool;
        tool.Share(anent,iter);
      }
      break;
    case 671:
      {
        DeclareAndCast(StepRepr_CompositeGroupShapeAspect,anent,ent);
        RWStepRepr_RWCompositeGroupShapeAspect tool;
        tool.Share(anent,iter);
      }
      break;
    case 672:
      {
        DeclareAndCast(StepRepr_ContinuosShapeAspect,anent,ent);
        RWStepRepr_RWContinuosShapeAspect tool;
        tool.Share(anent,iter);
      }
      break;
    case 673:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedAreaUnit,anent,ent);
        RWStepDimTol_RWGeometricToleranceWithDefinedAreaUnit tool;
        tool.Share(anent,iter);
      }
      break;
    case 674:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceWithDefinedUnit,anent,ent);
        RWStepDimTol_RWGeometricToleranceWithDefinedUnit tool;
        tool.Share(anent,iter);
      }
      break;
    case 675:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceWithMaximumTolerance,anent,ent);
        RWStepDimTol_RWGeometricToleranceWithMaximumTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 676:
      {
        DeclareAndCast(StepDimTol_GeometricToleranceWithModifiers,anent,ent);
        RWStepDimTol_RWGeometricToleranceWithModifiers tool;
        tool.Share(anent,iter);
      }
      break;
    case 677:
      {
        DeclareAndCast(StepDimTol_UnequallyDisposedGeometricTolerance,anent,ent);
        RWStepDimTol_RWUnequallyDisposedGeometricTolerance tool;
        tool.Share(anent,iter);
      }
      break;
    case 678:
      {
        DeclareAndCast(StepDimTol_NonUniformZoneDefinition,anent,ent);
        RWStepDimTol_RWNonUniformZoneDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    case 679:
      {
        DeclareAndCast(StepDimTol_ProjectedZoneDefinition,anent,ent);
        RWStepDimTol_RWProjectedZoneDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    case 680:
      {
        DeclareAndCast(StepDimTol_RunoutZoneDefinition,anent,ent);
        RWStepDimTol_RWRunoutZoneDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    //case 681: no shared attributes
    case 682:
      {
        DeclareAndCast(StepDimTol_ToleranceZone,anent,ent);
        RWStepDimTol_RWToleranceZone tool;
        tool.Share(anent,iter);
      }
      break;
    case 683:
      {
        DeclareAndCast(StepDimTol_ToleranceZoneDefinition,anent,ent);
        RWStepDimTol_RWToleranceZoneDefinition tool;
        tool.Share(anent,iter);
      }
      break;
    //case 684: no shared attributes
    //case 685: no shared attributes
    case 686 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceCompartment, anent, ent);
      RWStepDimTol_RWDatumReferenceCompartment tool;
      tool.Share(anent,iter);
    }
    break;
  case 687 :
    {
      DeclareAndCast(StepDimTol_DatumReferenceElement, anent, ent);
      RWStepDimTol_RWDatumReferenceElement tool;
      tool.Share(anent,iter);
    }
    break;
  //case 688: no shared attributes
  case 689 :
    {
      DeclareAndCast(StepDimTol_DatumSystem, anent, ent);
      RWStepDimTol_RWDatumSystem tool;
      tool.Share(anent,iter);
    }
    break;
  case 690 :
    {
      DeclareAndCast(StepDimTol_GeneralDatumReference, anent, ent);
      RWStepDimTol_RWGeneralDatumReference tool;
      tool.Share(anent,iter);
    }
    break;
  case 694:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRef,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRef tool;
      tool.Share(anent,iter);
    }
    break;
  case 695:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMod tool;
      tool.Share(anent,iter);
    }
    break;
  case 696:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMod,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMod tool;
      tool.Share(anent,iter);
    }
    break;
  case 697:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndUneqDisGeoTol tool;
      tool.Share(anent,iter);
    }
    break;
  case 698:
    {
      DeclareAndCast(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp tool;
      tool.Share(anent,iter);
    }
    break;
  case 699:
    {
      DeclareAndCast(StepRepr_CompShAspAndDatumFeatAndShAsp,anent,ent);
      RWStepRepr_RWCompShAspAndDatumFeatAndShAsp tool;
      tool.Share(anent,iter);
    }
    break;
  case 702:
    {
      DeclareAndCast(StepRepr_FeatureForDatumTargetRelationship,anent,ent);
      RWStepRepr_RWFeatureForDatumTargetRelationship tool;
      tool.Share(anent,iter);
    }
    break;
  case 703:
    {
      DeclareAndCast(StepAP242_DraughtingModelItemAssociation,anent,ent);
      RWStepAP242_RWDraughtingModelItemAssociation tool;
      tool.Share(anent,iter);
    }
    break;
  case 704:
    {
      DeclareAndCast(StepVisual_AnnotationPlane,anent,ent);
      RWStepVisual_RWAnnotationPlane tool;
      tool.Share(anent,iter);
    }
    break;
  case 705:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol tool;
      tool.Share(anent,iter);
    }
    break;
  case 706:
    {
      DeclareAndCast(StepDimTol_GeoTolAndGeoTolWthMaxTol,anent,ent);
      RWStepDimTol_RWGeoTolAndGeoTolWthMaxTol tool;
      tool.Share(anent,iter);
    }
    break;
  case 707:
    {
      DeclareAndCast(StepVisual_TessellatedAnnotationOccurrence,anent,ent);
      RWStepVisual_RWTessellatedAnnotationOccurrence tool;
      tool.Share(anent,iter);
    }
    break;
  case 709:
    {
      DeclareAndCast(StepVisual_TessellatedGeometricSet,anent,ent);
      RWStepVisual_RWTessellatedGeometricSet tool;
      tool.Share(anent,iter);
    }
    break;
  case 710:
    {
      DeclareAndCast(StepVisual_TessellatedCurveSet,anent,ent);
      RWStepVisual_RWTessellatedCurveSet tool;
      tool.Share(anent,iter);
    }
    break;
  case 712:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentation,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentation tool;
      tool.Share(anent,iter);
    }
    break;
  case 713:
    {
      DeclareAndCast(StepRepr_ConstructiveGeometryRepresentationRelationship,anent,ent);
      RWStepRepr_RWConstructiveGeometryRepresentationRelationship tool;
      tool.Share(anent,iter);
    }
    break;
  case 714:
  {
    DeclareAndCast(StepRepr_CharacterizedRepresentation, anent, ent);
    RWStepRepr_RWCharacterizedRepresentation tool;
    tool.Share(anent, iter);
  }
  break;
  case 715:
  {
    DeclareAndCast(StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel, anent, ent);
    RWStepVisual_RWCharacterizedObjAndRepresentationAndDraughtingModel tool;
    tool.Share(anent, iter);
  }
  break;
  case 716:
  {
    DeclareAndCast(StepVisual_CameraModelD3MultiClipping, anent, ent);
    RWStepVisual_RWCameraModelD3MultiClipping tool;
    tool.Share(anent, iter);
  }
  break;
  case 717:
  {
    DeclareAndCast(StepVisual_CameraModelD3MultiClippingIntersection, anent, ent);
    RWStepVisual_RWCameraModelD3MultiClippingIntersection tool;
    tool.Share(anent, iter);
  }
  break;
  case 718:
  {
    DeclareAndCast(StepVisual_CameraModelD3MultiClippingUnion, anent, ent);
    RWStepVisual_RWCameraModelD3MultiClippingUnion tool;
    tool.Share(anent, iter);
  }
  break;
  case 719:
  {
    DeclareAndCast(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem, anent, ent);
    RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem tool;
    tool.Share(anent, iter);
  }
  break;
  case 720:
  {
    DeclareAndCast(StepVisual_SurfaceStyleTransparent, anent, ent);
    RWStepVisual_RWSurfaceStyleTransparent tool;
    tool.Share(anent, iter);
  }
  break;
  case 721:
  {
    DeclareAndCast(StepVisual_SurfaceStyleReflectanceAmbient, anent, ent);
    RWStepVisual_RWSurfaceStyleReflectanceAmbient tool;
    tool.Share(anent, iter);
  }
  break;
  case 722:
  {
    DeclareAndCast(StepVisual_SurfaceStyleRendering, anent, ent);
    RWStepVisual_RWSurfaceStyleRendering tool;
    tool.Share(anent, iter);
  }
  break;
  case 723:
  {
    DeclareAndCast(StepVisual_SurfaceStyleRenderingWithProperties, anent, ent);
    RWStepVisual_RWSurfaceStyleRenderingWithProperties tool;
    tool.Share(anent, iter);
  }
  break;
    default : break;
    }
}


//=======================================================================
//function : CheckCase
//purpose  : 
//=======================================================================

void RWStepAP214_GeneralModule::CheckCase(const Standard_Integer CN,
                                          const Handle(Standard_Transient)& ent,
                                          const Interface_ShareTool& shares,
                                          Handle(Interface_Check)& ach) const
{  
  switch (CN)
    {
    case 39:
      {
	DeclareAndCast(StepGeom_BSplineCurveWithKnots,anent,ent);
	RWStepGeom_RWBSplineCurveWithKnots tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 41 :
      {
	DeclareAndCast(StepGeom_BSplineSurfaceWithKnots,anent,ent);
	RWStepGeom_RWBSplineSurfaceWithKnots tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 52:
      {
	DeclareAndCast(StepShape_BrepWithVoids,anent,ent);
	RWStepShape_RWBrepWithVoids tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 105:
      {
	DeclareAndCast(StepGeom_Direction,anent,ent);
	RWStepGeom_RWDirection tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 116:
      {
	DeclareAndCast(StepShape_EdgeCurve,anent,ent);
	RWStepShape_RWEdgeCurve tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 117:
      {
	DeclareAndCast(StepShape_EdgeLoop,anent,ent);
	RWStepShape_RWEdgeLoop tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 119:
      {
	DeclareAndCast(StepGeom_Ellipse,anent,ent);
	RWStepGeom_RWEllipse tool;
	tool.Check(anent,shares,ach);
	break;
      }
//    case 131:
//      {
//	DeclareAndCast(StepShape_FaceBound,anent,ent);
//	RWStepShape_RWFaceBound tool;
//	tool.Check(anent,shares,ach);
//	break;
//      }
    case 239:
      {
	DeclareAndCast(StepGeom_RationalBSplineCurve,anent,ent);
	RWStepGeom_RWRationalBSplineCurve tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 240 :
      {
	DeclareAndCast(StepGeom_RationalBSplineSurface,anent,ent);
	RWStepGeom_RWRationalBSplineSurface tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 305 :
      {
	DeclareAndCast(StepGeom_ToroidalSurface,anent,ent);
	RWStepGeom_RWToroidalSurface tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 313:
      {
	DeclareAndCast(StepGeom_Vector,anent,ent);
	RWStepGeom_RWVector tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 320:
      {
	DeclareAndCast(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve,anent,ent);
	RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve tool;
	tool.Check(anent,shares,ach);
	break;
      }
    case 323:
      {
	DeclareAndCast(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface,anent,ent);
	RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface tool;
	tool.Check(anent,shares,ach);
	break;
      }
      default : break;
    }
}


//=======================================================================
//function : CopyCase
//purpose  : 
//=======================================================================

void RWStepAP214_GeneralModule::CopyCase(const Standard_Integer /*CN*/, 
					 const Handle(Standard_Transient)& /*entfrom*/, 
					 const Handle(Standard_Transient)& /*entto*/, 
					 Interface_CopyTool& /*TC*/) const
{  
}

// --- Construction of empty classe ---

//=======================================================================
//function : NewVoid
//purpose  : 
//=======================================================================

Standard_Boolean RWStepAP214_GeneralModule::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
  if (CN == 0) return Standard_False;
  switch (CN) {
  case 1 : 
    ent = new StepBasic_Address;
    break;
  case 2 : 
    ent = new StepShape_AdvancedBrepShapeRepresentation;
    break;
  case 3 : 
    ent = new StepShape_AdvancedFace;
    break;
  case 4 : 
    ent = new StepVisual_AnnotationCurveOccurrence;
    break;
  case 5:
    ent = new StepVisual_AnnotationFillArea;
  break;
  case 6:
    ent = new StepVisual_AnnotationFillAreaOccurrence;
  break;
  case 7 : 
    ent = new StepVisual_AnnotationOccurrence;
    break;
  case 11 : 
    ent = new StepVisual_AnnotationText;
    break;
  case 12 : 
    ent = new StepVisual_AnnotationTextOccurrence;
    break;
  case 13 : 
    ent = new StepBasic_ApplicationContext;
    break;
  case 14 : 
    ent = new StepBasic_ApplicationContextElement;
    break;
  case 15 : 
    ent = new StepBasic_ApplicationProtocolDefinition;
    break;
  case 16 : 
    ent = new StepBasic_Approval;
    break;
  case 18 : 
    ent = new StepBasic_ApprovalPersonOrganization;
    break;
  case 19 : 
    ent = new StepBasic_ApprovalRelationship;
    break;
  case 20 : 
    ent = new StepBasic_ApprovalRole;
    break;
  case 21 : 
    ent = new StepBasic_ApprovalStatus;
    break;
  case 22 : 
    ent = new StepVisual_AreaInSet;
    break;
  case 23 : 
    ent = new StepAP214_AutoDesignActualDateAndTimeAssignment;
    break;
  case 24 : 
    ent = new StepAP214_AutoDesignActualDateAssignment;
    break;
  case 25 : 
    ent = new StepAP214_AutoDesignApprovalAssignment;
    break;
  case 26 : 
    ent = new StepAP214_AutoDesignDateAndPersonAssignment;
    break;
  case 27 : 
    ent = new StepAP214_AutoDesignGroupAssignment;
    break;
  case 28 : 
    ent = new StepAP214_AutoDesignNominalDateAndTimeAssignment;
    break;
  case 29 : 
    ent = new StepAP214_AutoDesignNominalDateAssignment;
    break;
  case 30 : 
    ent = new StepAP214_AutoDesignOrganizationAssignment;
    break;
  case 31 : 
    ent = new StepAP214_AutoDesignPersonAndOrganizationAssignment;
    break;
  case 32 : 
    ent = new StepAP214_AutoDesignPresentedItem;
    break;
  case 33 : 
    ent = new StepAP214_AutoDesignSecurityClassificationAssignment;
    break;
  case 35 : 
    ent = new StepGeom_Axis1Placement;
    break;
  case 36 : 
    ent = new StepGeom_Axis2Placement2d;
    break;
  case 37 : 
    ent = new StepGeom_Axis2Placement3d;
    break;
  case 38 : 
    ent = new StepGeom_BSplineCurve;
    break;
  case 39 : 
    ent = new StepGeom_BSplineCurveWithKnots;
    break;
  case 40 : 
    ent = new StepGeom_BSplineSurface;
    break;
  case 41 : 
    ent = new StepGeom_BSplineSurfaceWithKnots;
    break;
  case 42 : 
    ent = new StepVisual_BackgroundColour;
    break;
  case 43 : 
    ent = new StepGeom_BezierCurve;
    break;
  case 44 : 
    ent = new StepGeom_BezierSurface;
    break;
  case 45 : 
    ent = new StepShape_Block;
    break;
  case 46 : 
    ent = new StepShape_BooleanResult;
    break;
  case 47 : 
    ent = new StepGeom_BoundaryCurve;
    break;
  case 48 : 
    ent = new StepGeom_BoundedCurve;
    break;
  case 49 : 
    ent = new StepGeom_BoundedSurface;
    break;
  case 50 : 
    ent = new StepShape_BoxDomain;
    break;
  case 51 : 
    ent = new StepShape_BoxedHalfSpace;
    break;
  case 52 : 
    ent = new StepShape_BrepWithVoids;
    break;
  case 53 : 
    ent = new StepBasic_CalendarDate;
    break;
  case 54 : 
    ent = new StepVisual_CameraImage;
    break;
  case 55 : 
    ent = new StepVisual_CameraModel;
    break;
  case 56 : 
    ent = new StepVisual_CameraModelD2;
    break;
  case 57 : 
    ent = new StepVisual_CameraModelD3;
    break;
  case 58 : 
    ent = new StepVisual_CameraUsage;
    break;
  case 59 : 
    ent = new StepGeom_CartesianPoint;
    break;
  case 60 : 
    ent = new StepGeom_CartesianTransformationOperator;
    break;
  case 61 : 
    ent = new StepGeom_CartesianTransformationOperator3d;
    break;
  case 62 : 
    ent = new StepGeom_Circle;
    break;
  case 63 : 
    ent = new StepShape_ClosedShell;
    break;
  case 64 : 
    ent = new StepVisual_Colour;
    break;
  case 65 : 
    ent = new StepVisual_ColourRgb;
    break;
  case 66 : 
    ent = new StepVisual_ColourSpecification;
    break;
  case 67 : 
    ent = new StepGeom_CompositeCurve;
    break;
  case 68 : 
    ent = new StepGeom_CompositeCurveOnSurface;
    break;
  case 69 : 
    ent = new StepGeom_CompositeCurveSegment;
    break;
  case 70 : 
    ent = new StepVisual_CompositeText;
    break;
  case 73 : 
    ent = new StepVisual_CompositeTextWithExtent;
    break;
  case 74 : 
    ent = new StepGeom_Conic;
    break;
  case 75 : 
    ent = new StepGeom_ConicalSurface;
    break;
  case 76 : 
    ent = new StepShape_ConnectedFaceSet;
    break;
  case 77 : 
    ent = new StepVisual_ContextDependentInvisibility;
    break;
  case 78 : 
    ent = new StepVisual_ContextDependentOverRidingStyledItem;
    break;
  case 79 : 
    ent = new StepBasic_ConversionBasedUnit;
    break;
  case 80 : 
    ent = new StepBasic_CoordinatedUniversalTimeOffset;
    break;
  case 82 : 
    ent = new StepShape_CsgShapeRepresentation;
    break;
  case 83 : 
    ent = new StepShape_CsgSolid;
    break;
  case 84 : 
    ent = new StepGeom_Curve;
    break;
  case 85 : 
    ent = new StepGeom_CurveBoundedSurface;
    break;
  case 86 : 
    ent = new StepGeom_CurveReplica;
    break;
  case 87 : 
    ent = new StepVisual_CurveStyle;
    break;
  case 88 : 
    ent = new StepVisual_CurveStyleFont;
    break;
  case 89 : 
    ent = new StepVisual_CurveStyleFontPattern;
    break;
  case 90 : 
    ent = new StepGeom_CylindricalSurface;
    break;
  case 91 : 
    ent = new StepBasic_Date;
    break;
  case 92 : 
    ent = new StepBasic_DateAndTime;
    break;
  case 95 : 
    ent = new StepBasic_DateRole;
    break;
  case 96 : 
    ent = new StepBasic_DateTimeRole;
    break;
  case 98 : 
    ent = new StepRepr_DefinitionalRepresentation;
    break;
  case 99 : 
    ent = new StepGeom_DegeneratePcurve;
    break;
  case 100 : 
    ent = new StepGeom_DegenerateToroidalSurface;
    break;
  case 101 : 
    ent = new StepRepr_DescriptiveRepresentationItem;
    break;
  case 104 : 
    ent = new StepBasic_DimensionalExponents;
    break;
  case 105 : 
    ent = new StepGeom_Direction;
    break;
  case 106 : 
    ent = new StepVisual_AnnotationOccurrence;
    break;
  case 107 : 
    ent = new StepVisual_DraughtingCallout;
    break;
  case 108 : 
    ent = new StepVisual_DraughtingPreDefinedColour;
    break;
  case 109 : 
    ent = new StepVisual_DraughtingPreDefinedCurveFont;
    break;
  case 115 : 
    ent = new StepShape_Edge;
    break;
  case 116 : 
    ent = new StepShape_EdgeCurve;
    break;
  case 117 : 
    ent = new StepShape_EdgeLoop;
    break;
  case 118 : 
    ent = new StepGeom_ElementarySurface;
    break;
  case 119 : 
    ent = new StepGeom_Ellipse;
    break;
  case 120 : 
    ent = new StepGeom_EvaluatedDegeneratePcurve;
    break;
  case 121 : 
    ent = new StepBasic_ExternalSource;
    break;
  case 122 : 
    ent = new StepVisual_ExternallyDefinedCurveFont;
    break;
  case 124 : 
    ent = new StepBasic_ExternallyDefinedItem;
    break;
  case 126 : 
    ent = new StepVisual_ExternallyDefinedTextFont;
    break;
  case 128 : 
    ent = new StepShape_ExtrudedAreaSolid;
    break;
  case 129 : 
    ent = new StepShape_Face;
    break;
  case 131 : 
    ent = new StepShape_FaceBound;
    break;
  case 132 : 
    ent = new StepShape_FaceOuterBound;
    break;
  case 133 : 
    ent = new StepShape_FaceSurface;
    break;
  case 134 : 
    ent = new StepShape_FacetedBrep;
    break;
  case 135 : 
    ent = new StepShape_FacetedBrepShapeRepresentation;
    break;
  case 136 : 
    ent = new StepVisual_FillAreaStyle;
    break;
  case 137 : 
    ent = new StepVisual_FillAreaStyleColour;
    break;
  case 141 : 
    ent = new StepRepr_FunctionallyDefinedTransformation;
    break;
  case 142 : 
    ent = new StepShape_GeometricCurveSet;
    break;
  case 143 : 
    ent = new StepGeom_GeometricRepresentationContext;
    break;
  case 144 : 
    ent = new StepGeom_GeometricRepresentationItem;
    break;
  case 145 : 
    ent = new StepShape_GeometricSet;
    break;
  case 146 : 
    ent = new StepShape_GeometricallyBoundedSurfaceShapeRepresentation;
    break;
  case 147 : 
    ent = new StepShape_GeometricallyBoundedWireframeShapeRepresentation;
    break;
  case 148 : 
    ent = new StepRepr_GlobalUncertaintyAssignedContext;
    break;
  case 149 : 
    ent = new StepRepr_GlobalUnitAssignedContext;
    break;
  case 150 : 
    ent = new StepBasic_Group;
    break;
  case 152 : 
    ent = new StepBasic_GroupRelationship;
    break;
  case 153 : 
    ent = new StepShape_HalfSpaceSolid;
    break;
  case 154 : 
    ent = new StepGeom_Hyperbola;
    break;
  case 155 : 
    ent = new StepGeom_IntersectionCurve;
    break;
  case 156 : 
    ent = new StepVisual_Invisibility;
    break;
  case 157 : 
    ent = new StepBasic_LengthMeasureWithUnit;
    break;
  case 158 : 
    ent = new StepBasic_LengthUnit;
    break;
  case 159 : 
    ent = new StepGeom_Line;
    break;
  case 160 : 
    ent = new StepBasic_LocalTime;
    break;
  case 161 : 
    ent = new StepShape_Loop;
    break;
  case 162 : 
    ent = new StepShape_ManifoldSolidBrep;
    break;
  case 163 : 
    ent = new StepShape_ManifoldSurfaceShapeRepresentation;
    break;
  case 164 : 
    ent = new StepRepr_MappedItem;
    break;
  case 165 : 
    ent = new StepBasic_MeasureWithUnit;
    break;
  case 166 : 
    ent = new StepVisual_MechanicalDesignGeometricPresentationArea;
    break;
  case 167 : 
    ent = new StepVisual_MechanicalDesignGeometricPresentationRepresentation;
    break;
  case 169 : 
    ent = new StepBasic_NamedUnit;
    break;
  case 171 : 
    ent = new StepGeom_OffsetCurve3d;
    break;
  case 172 : 
    ent = new StepGeom_OffsetSurface;
    break;
  case 174 : 
    ent = new StepShape_OpenShell;
    break;
  case 175 : 
    ent = new StepBasic_OrdinalDate;
    break;
  case 176 : 
    ent = new StepBasic_Organization;
    break;
  case 178 : 
    ent = new StepBasic_OrganizationRole;
    break;
  case 179 : 
    ent = new StepBasic_OrganizationalAddress;
    break;
  case 180 : 
    ent = new StepShape_OrientedClosedShell;
    break;
  case 181 : 
    ent = new StepShape_OrientedEdge;
    break;
  case 182 : 
    ent = new StepShape_OrientedFace;
    break;
  case 183 : 
    ent = new StepShape_OrientedOpenShell;
    break;
  case 184 : 
    ent = new StepShape_OrientedPath;
    break;
  case 185 : 
    ent = new StepGeom_OuterBoundaryCurve;
    break;
  case 186 : 
    ent = new StepVisual_OverRidingStyledItem;
    break;
  case 187 : 
    ent = new StepGeom_Parabola;
    break;
  case 188 : 
    ent = new StepRepr_ParametricRepresentationContext;
    break;
  case 189 : 
    ent = new StepShape_Path;
    break;
  case 190 : 
    ent = new StepGeom_Pcurve;
    break;
  case 191 : 
    ent = new StepBasic_Person;
    break;
  case 192 : 
    ent = new StepBasic_PersonAndOrganization;
    break;
  case 194 : 
    ent = new StepBasic_PersonAndOrganizationRole;
    break;
  case 195 : 
    ent = new StepBasic_PersonalAddress;
    break;
  case 196 : 
    ent = new StepGeom_Placement;
    break;
  case 197 : 
    ent = new StepVisual_PlanarBox;
    break;
  case 198 : 
    ent = new StepVisual_PlanarExtent;
    break;
  case 199 : 
    ent = new StepGeom_Plane;
    break;
  case 200 : 
    ent = new StepBasic_PlaneAngleMeasureWithUnit;
    break;
  case 201 : 
    ent = new StepBasic_PlaneAngleUnit;
    break;
  case 202 : 
    ent = new StepGeom_Point;
    break;
  case 203 : 
    ent = new StepGeom_PointOnCurve;
    break;
  case 204 : 
    ent = new StepGeom_PointOnSurface;
    break;
  case 205 : 
    ent = new StepGeom_PointReplica;
    break;
  case 206 : 
    ent = new StepVisual_PointStyle;
    break;
  case 207 : 
    ent = new StepShape_PolyLoop;
    break;
  case 208 : 
    ent = new StepGeom_Polyline;
    break;
  case 209 : 
    ent = new StepVisual_PreDefinedColour;
    break;
  case 210 : 
    ent = new StepVisual_PreDefinedCurveFont;
    break;
  case 211 : 
    ent = new StepVisual_PreDefinedItem;
    break;
  case 213 : 
    ent = new StepVisual_PreDefinedTextFont;
    break;
  case 214 : 
    ent = new StepVisual_PresentationArea;
    break;
  case 215 : 
    ent = new StepVisual_PresentationLayerAssignment;
    break;
  case 216 : 
    ent = new StepVisual_PresentationRepresentation;
    break;
  case 217 : 
    ent = new StepVisual_PresentationSet;
    break;
  case 218 : 
    ent = new StepVisual_PresentationSize;
    break;
  case 219 : 
    ent = new StepVisual_PresentationStyleAssignment;
    break;
  case 220 : 
    ent = new StepVisual_PresentationStyleByContext;
    break;
  case 221 : 
    ent = new StepVisual_PresentationView;
    break;
  case 223 : 
    ent = new StepBasic_Product;
    break;
  case 224 : 
    ent = new StepBasic_ProductCategory;
    break;
  case 225 : 
    ent = new StepBasic_ProductContext;
    break;
  case 227 : 
    ent = new StepBasic_ProductDefinition;
    break;
  case 228 : 
    ent = new StepBasic_ProductDefinitionContext;
    break;
  case 229 : 
    ent = new StepBasic_ProductDefinitionFormation;
    break;
  case 230 : 
    ent = new StepBasic_ProductDefinitionFormationWithSpecifiedSource;
    break;
  case 231 : 
    ent = new StepRepr_ProductDefinitionShape;
    break;
  case 232 : 
    ent = new StepBasic_ProductRelatedProductCategory;
    break;
  case 233 : 
    ent = new StepBasic_ProductType;
    break;
  case 234 : 
    ent = new StepRepr_PropertyDefinition;
    break;
  case 235 : 
    ent = new StepRepr_PropertyDefinitionRepresentation;
    break;
  case 236 : 
    ent = new StepGeom_QuasiUniformCurve;
    break;
  case 237 : 
    ent = new StepGeom_QuasiUniformSurface;
    break;
  case 238 : 
    ent = new StepBasic_RatioMeasureWithUnit;
    break;
  case 239 : 
    ent = new StepGeom_RationalBSplineCurve;
    break;
  case 240 : 
    ent = new StepGeom_RationalBSplineSurface;
    break;
  case 241 : 
    ent = new StepGeom_RectangularCompositeSurface;
    break;
  case 242 : 
    ent = new StepGeom_RectangularTrimmedSurface;
    break;
  case 243 : 
    ent = new StepAP214_RepItemGroup;
    break;
  case 244 : 
    ent = new StepGeom_ReparametrisedCompositeCurveSegment;
    break;
  case 245 : 
    ent = new StepRepr_Representation;
    break;
  case 246 : 
    ent = new StepRepr_RepresentationContext;
    break;
  case 247 : 
    ent = new StepRepr_RepresentationItem;
    break;
  case 248 : 
    ent = new StepRepr_RepresentationMap;
    break;
  case 249 : 
    ent = new StepRepr_RepresentationRelationship;
    break;
  case 250 : 
    ent = new StepShape_RevolvedAreaSolid;
    break;
  case 251 : 
    ent = new StepShape_RightAngularWedge;
    break;
  case 252 : 
    ent = new StepShape_RightCircularCone;
    break;
  case 253 : 
    ent = new StepShape_RightCircularCylinder;
    break;
  case 254 : 
    ent = new StepGeom_SeamCurve;
    break;
  case 255 : 
    ent = new StepBasic_SecurityClassification;
    break;
  case 257 : 
    ent = new StepBasic_SecurityClassificationLevel;
    break;
  case 258 : 
    ent = new StepRepr_ShapeAspect;
    break;
  case 259 : 
    ent = new StepRepr_ShapeAspectRelationship;
    break;
  case 260 : 
    ent = new StepRepr_ShapeAspectTransition;
    break;
  case 261 : 
    ent = new StepShape_ShapeDefinitionRepresentation;
    break;
  case 262 : 
    ent = new StepShape_ShapeRepresentation;
    break;
  case 263 : 
    ent = new StepShape_ShellBasedSurfaceModel;
    break;
  case 264 : 
    ent = new StepBasic_SiUnit;
    break;
  case 265 : 
    ent = new StepBasic_SolidAngleMeasureWithUnit;
    break;
  case 266 : 
    ent = new StepShape_SolidModel;
    break;
  case 267 : 
    ent = new StepShape_SolidReplica;
    break;
  case 268 : 
    ent = new StepShape_Sphere;
    break;
  case 269 : 
    ent = new StepGeom_SphericalSurface;
    break;
  case 270 : 
    ent = new StepVisual_StyledItem;
    break;
  case 271 : 
    ent = new StepGeom_Surface;
    break;
  case 272 : 
    ent = new StepGeom_SurfaceCurve;
    break;
  case 273 : 
    ent = new StepGeom_SurfaceOfLinearExtrusion;
    break;
  case 274 : 
    ent = new StepGeom_SurfaceOfRevolution;
    break;
  case 275 : 
    ent = new StepGeom_SurfacePatch;
    break;
  case 276 : 
    ent = new StepGeom_SurfaceReplica;
    break;
  case 277 : 
    ent = new StepVisual_SurfaceSideStyle;
    break;
  case 278 : 
    ent = new StepVisual_SurfaceStyleBoundary;
    break;
  case 279 : 
    ent = new StepVisual_SurfaceStyleControlGrid;
    break;
  case 280 : 
    ent = new StepVisual_SurfaceStyleFillArea;
    break;
  case 281 : 
    ent = new StepVisual_SurfaceStyleParameterLine;
    break;
  case 282 : 
    ent = new StepVisual_SurfaceStyleSegmentationCurve;
    break;
  case 283 : 
    ent = new StepVisual_SurfaceStyleSilhouette;
    break;
  case 284 : 
    ent = new StepVisual_SurfaceStyleUsage;
    break;
  case 285 : 
    ent = new StepShape_SweptAreaSolid;
    break;
  case 286 : 
    ent = new StepGeom_SweptSurface;
    break;
  case 292 : 
    ent = new StepVisual_Template;
    break;
  case 293 : 
    ent = new StepVisual_TemplateInstance;
    break;
  case 295 : 
    ent = new StepVisual_TextLiteral;
    break;
  case 300 : 
    ent = new StepVisual_TextStyle;
    break;
  case 301 : 
    ent = new StepVisual_TextStyleForDefinedFont;
    break;
  case 302 : 
    ent = new StepVisual_TextStyleWithBoxCharacteristics;
    break;
  case 304 : 
    ent = new StepShape_TopologicalRepresentationItem;
    break;
  case 305 : 
    ent = new StepGeom_ToroidalSurface;
    break;
  case 306 : 
    ent = new StepShape_Torus;
    break;
  case 307 : 
    ent = new StepShape_TransitionalShapeRepresentation;
    break;
  case 308 : 
    ent = new StepGeom_TrimmedCurve;
    break;
  case 310 : 
    ent = new StepBasic_UncertaintyMeasureWithUnit;
    break;
  case 311 : 
    ent = new StepGeom_UniformCurve;
    break;
  case 312 : 
    ent = new StepGeom_UniformSurface;
    break;
  case 313 : 
    ent = new StepGeom_Vector;
    break;
  case 314 : 
    ent = new StepShape_Vertex;
    break;
  case 315 : 
    ent = new StepShape_VertexLoop;
    break;
  case 316 : 
    ent = new StepShape_VertexPoint;
    break;
  case 317 : 
    ent = new StepVisual_ViewVolume;
    break;
  case 318 : 
    ent = new StepBasic_WeekOfYearAndDayDate;
    break;
  case 319 : 
    ent = new StepGeom_UniformCurveAndRationalBSplineCurve;
    break;
  case 320 : 
    ent = new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
    break;
  case 321 : 
    ent = new StepGeom_QuasiUniformCurveAndRationalBSplineCurve;
    break;
  case 322 : 
    ent = new StepGeom_BezierCurveAndRationalBSplineCurve;
    break;
  case 323 : 
    ent = new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
    break;
  case 324 : 
    ent = new StepGeom_UniformSurfaceAndRationalBSplineSurface;
    break;
  case 325 : 
    ent = new StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface;
    break;
  case 326 : 
    ent = new StepGeom_BezierSurfaceAndRationalBSplineSurface;
    break;
  case 327 : 
    ent = new StepBasic_SiUnitAndLengthUnit;
    break;
  case 328 : 
    ent = new StepBasic_SiUnitAndPlaneAngleUnit;
    break;
  case 329 : 
    ent = new StepBasic_ConversionBasedUnitAndLengthUnit;
    break;
  case 330 : 
    ent = new StepBasic_ConversionBasedUnitAndPlaneAngleUnit;
    break;
  case 331 : 
    ent = new StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext;
    break;
  case 332 : 
    ent = new StepShape_LoopAndPath;
    break;
    
    // ------------
    // Added by FMA
    // ------------
    
  case 333 :
    ent = new StepGeom_GeomRepContextAndGlobUnitAssCtxAndGlobUncertaintyAssCtx;
    break;
  case 334 :
    ent = new StepBasic_ConversionBasedUnitAndSolidAngleUnit;
    break;
  case 335 :
    ent = new StepBasic_SiUnitAndSolidAngleUnit;
    break;
  case 336 :
    ent = new StepBasic_SolidAngleUnit;
    break;
  case 337 :
    ent = new StepShape_FacetedBrepAndBrepWithVoids;
    break;
  case 338 : 
    ent = new StepGeom_GeometricRepresentationContextAndParametricRepresentationContext;
    break;
  case 339:
    ent = new StepBasic_MechanicalContext;
    break;
  
    // ------------
    // Added by CKY
    // ------------

  case 340:
    ent = new StepBasic_DesignContext;
    break;

    // ------------
    // Added for Rev4
    // ------------

  case 341:
    ent = new StepBasic_TimeMeasureWithUnit;
    break;
  case 342:
    ent = new StepBasic_RatioUnit;
    break;
  case 343:
    ent = new StepBasic_TimeUnit;
    break;
  case 344:
    ent = new StepBasic_SiUnitAndRatioUnit;
    break;
  case 345:
    ent = new StepBasic_SiUnitAndTimeUnit;
    break;
  case 346:
    ent = new StepBasic_ConversionBasedUnitAndRatioUnit;
    break;
  case 347:
    ent = new StepBasic_ConversionBasedUnitAndTimeUnit;
    break;
  case 348:
    ent = new StepBasic_ApprovalDateTime;
    break;
  case 349:
    ent = new StepVisual_CameraImage2dWithScale;
    break;
  case 350:
    ent = new StepVisual_CameraImage3dWithScale;
    break;
  case 351:
    ent = new StepGeom_CartesianTransformationOperator2d;
    break;
  case 352:
    ent = new StepBasic_DerivedUnit;
    break;
  case 353:
    ent = new StepBasic_DerivedUnitElement;
    break;
  case 354:
    ent = new StepRepr_ItemDefinedTransformation;
    break;
  case 355:
    ent = new StepVisual_PresentedItemRepresentation;
    break;
  case 356:
    ent = new StepVisual_PresentationLayerUsage;
    break;
  case 357:  // LECTURE SEULEMENT, origine CATIA. CKY 2-SEP-1997
    ent = new StepBasic_UncertaintyMeasureWithUnit;
    break;
  case 358: //:n5
    ent = new StepGeom_SurfaceCurveAndBoundedCurve;
    break;

//  Added CKY : AP214 CC1 -> CC2
  case 366:
    ent = new StepAP214_AutoDesignDocumentReference;
    break;
  case 367:
    ent = new StepBasic_Document;
    break;
  case 368:
    ent = new StepBasic_DigitalDocument;
    break;
  case 369:
    ent = new StepBasic_DocumentRelationship;
    break;
  case 370:
    ent = new StepBasic_DocumentType;
    break;
  case 371:
    ent = new StepBasic_DocumentUsageConstraint;
    break;
  case 372:
    ent = new StepBasic_Effectivity;
    break;
  case 373:
    ent = new StepBasic_ProductDefinitionEffectivity;
    break;
  case 374:
    ent = new StepBasic_ProductDefinitionRelationship;
    break;

  case 375:
    ent = new StepBasic_ProductDefinitionWithAssociatedDocuments;
    break;
  case 376:
    ent = new StepBasic_PhysicallyModeledProductDefinition;
    break;

  case 377:
    ent = new StepRepr_ProductDefinitionUsage;
    break;
  case 378:
    ent = new StepRepr_MakeFromUsageOption;
    break;
  case 379:
    ent = new StepRepr_AssemblyComponentUsage;
    break;
  case 380:
    ent = new StepRepr_NextAssemblyUsageOccurrence;
    break;
  case 381:
    ent = new StepRepr_PromissoryUsageOccurrence;
    break;
  case 382:
    ent = new StepRepr_QuantifiedAssemblyComponentUsage;
    break;
  case 383:
    ent = new StepRepr_SpecifiedHigherUsageOccurrence;
    break;
  case 384:
    ent = new StepRepr_AssemblyComponentUsageSubstitute;
    break;
  case 385:
    ent = new StepRepr_SuppliedPartRelationship;
    break;
  case 386:
    ent = new StepRepr_ExternallyDefinedRepresentation;
    break;
  case 387:
    ent = new StepRepr_ShapeRepresentationRelationship;
    break;
  case 388:
    ent = new StepRepr_RepresentationRelationshipWithTransformation;
    break;
  case 389:
    ent = new StepRepr_ShapeRepresentationRelationshipWithTransformation;
    break;
  case 390:
    ent = new StepRepr_MaterialDesignation;
    break;

  case 391:
    ent = new StepShape_ContextDependentShapeRepresentation;
    break;

  //:S4134: Added from CD to DIS
  case 392:
    ent = new StepAP214_AppliedDateAndTimeAssignment;
    break;
  case 393:
    ent = new StepAP214_AppliedDateAssignment;
    break;
  case 394:
    ent = new StepAP214_AppliedApprovalAssignment;
    break;
  case 395:
    ent = new StepAP214_AppliedGroupAssignment;
    break;
  case 396:
    ent = new StepAP214_AppliedOrganizationAssignment;
    break;
  case 397:
    ent = new StepAP214_AppliedPersonAndOrganizationAssignment;
    break;
  case 398:
    ent = new StepAP214_AppliedPresentedItem;
    break;
  case 399:
    ent = new StepAP214_AppliedSecurityClassificationAssignment;
    break;
  case 400:
    ent = new StepAP214_AppliedDocumentReference;
    break;
  case 401:
    ent = new StepBasic_DocumentFile;
    break;
  case 402:
    ent = new StepBasic_CharacterizedObject;
    break;
  case 403:
    ent = new StepShape_ExtrudedFaceSolid;
    break;
  case 404:
    ent = new StepShape_RevolvedFaceSolid ;
    break;
  case 405:
    ent = new StepShape_SweptFaceSolid;
    break;
  case 406:
    ent = new StepRepr_MeasureRepresentationItem;
    break;
  case 407:
    ent = new StepBasic_AreaUnit;
    break;
  case 408:
    ent = new StepBasic_VolumeUnit;
    break;
  case 409:
    ent = new StepBasic_SiUnitAndAreaUnit;
    break;
  case 410:
    ent = new StepBasic_SiUnitAndVolumeUnit;
    break;
  case 411:
    ent = new StepBasic_ConversionBasedUnitAndAreaUnit;
    break;
  case 412:
    ent = new StepBasic_ConversionBasedUnitAndVolumeUnit;
    break;
  // Added by ABV 10.11.99 for AP203
  case 413:
    ent = new StepBasic_Action;
    break;
  case 414:
    ent = new StepBasic_ActionAssignment;
    break;
  case 415:
    ent = new StepBasic_ActionMethod;
    break;
  case 416:
    ent = new StepBasic_ActionRequestAssignment;
    break;
  case 417:
    ent = new StepAP203_CcDesignApproval;
    break;
  case 418:
    ent = new StepAP203_CcDesignCertification;
    break;
  case 419:
    ent = new StepAP203_CcDesignContract;
    break;
  case 420:
    ent = new StepAP203_CcDesignDateAndTimeAssignment;
    break;
  case 421:
    ent = new StepAP203_CcDesignPersonAndOrganizationAssignment;
    break;
  case 422:
    ent = new StepAP203_CcDesignSecurityClassification;
    break;
  case 423:
    ent = new StepAP203_CcDesignSpecificationReference;
    break;
  case 424:
    ent = new StepBasic_Certification;
    break;
  case 425:
    ent = new StepBasic_CertificationAssignment;
    break;
  case 426:
    ent = new StepBasic_CertificationType;
    break;
  case 427:
    ent = new StepAP203_Change;
    break;
  case 428:
    ent = new StepAP203_ChangeRequest;
    break;
  case 429:
    ent = new StepRepr_ConfigurationDesign;
    break;
  case 430:
    ent = new StepRepr_ConfigurationEffectivity;
    break;
  case 431:
    ent = new StepBasic_Contract;
    break;
  case 432:
    ent = new StepBasic_ContractAssignment;
    break;
  case 433:
    ent = new StepBasic_ContractType;
    break;
  case 434:
    ent = new StepRepr_ProductConcept;
    break;
  case 435:
    ent = new StepBasic_ProductConceptContext;
    break;
  case 436:
    ent = new StepAP203_StartRequest;
    break;
  case 437:
    ent = new StepAP203_StartWork;
    break;
  case 438:
    ent = new StepBasic_VersionedActionRequest;
    break;
  case 439:
    ent = new StepBasic_ProductCategoryRelationship;
    break;
  case 440:
    ent = new StepBasic_ActionRequestSolution;
    break;
  case 441:
    ent = new StepVisual_DraughtingModel;
    break;
  case 442:
    ent = new StepShape_AngularLocation;
    break;
  case 443:
    ent = new StepShape_AngularSize;
    break;
  case 444:
    ent = new StepShape_DimensionalCharacteristicRepresentation;
    break;
  case 445:
    ent = new StepShape_DimensionalLocation;
    break;
  case 446:
    ent = new StepShape_DimensionalLocationWithPath;
    break;
  case 447:
    ent = new StepShape_DimensionalSize;
    break;
  case 448:
    ent = new StepShape_DimensionalSizeWithPath;
    break;
  case 449:
    ent = new StepShape_ShapeDimensionRepresentation;
    break;
  case 450:
    ent = new StepBasic_DocumentRepresentationType;
    break;
  case 451:
    ent = new StepBasic_ObjectRole;
    break;
  case 452:
    ent = new StepBasic_RoleAssociation;
    break;
  case 453:
    ent = new StepBasic_IdentificationRole;
    break;
  case 454:
    ent = new StepBasic_IdentificationAssignment;
    break;
  case 455:
    ent = new StepBasic_ExternalIdentificationAssignment;
    break;
  case 456:
    ent = new StepBasic_EffectivityAssignment;
    break;
  case 457:
    ent = new StepBasic_NameAssignment;
    break;
  case 458:
    ent = new StepBasic_GeneralProperty;
    break;
  case 459:
    ent = new StepAP214_Class;
    break;
  case 460:
    ent = new StepAP214_ExternallyDefinedClass;
    break;
  case 461:
    ent = new StepAP214_ExternallyDefinedGeneralProperty;
    break;
  case 462:
    ent = new StepAP214_AppliedExternalIdentificationAssignment;
    break;
  case 463:
    ent = new StepShape_DefinitionalRepresentationAndShapeRepresentation;
    break;
//  added by CKY, 25 APR 2001
  case 470:
    ent = new StepRepr_CompositeShapeAspect;
    break;
  case 471:
    ent = new StepRepr_DerivedShapeAspect;
    break;
  case 472:
    ent = new StepRepr_Extension;
    break;
  case 473:
    ent = new StepShape_DirectedDimensionalLocation;
    break;
  case 474:
    ent = new StepShape_LimitsAndFits;
    break;
  case 475:
    ent = new StepShape_ToleranceValue;
    break;
  case 476:
    ent = new StepShape_MeasureQualification;
    break;
  case 477:
    ent = new StepShape_PlusMinusTolerance;
    break;
  case 478:
    ent = new StepShape_PrecisionQualifier;
    break;
  case 479:
    ent = new StepShape_TypeQualifier;
    break;
  case 480:
    ent = new StepShape_QualifiedRepresentationItem;
    break;
  case 481:
    ent = new StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem;
    break;
  case 482:
    ent = new StepRepr_CompoundRepresentationItem;
    break;
  case 483:
    ent = new StepRepr_ValueRange;
    break;
  case 484:
    ent = new StepRepr_ShapeAspectDerivingRelationship;
    break;

  case 485:
    ent = new StepShape_CompoundShapeRepresentation;
    break;
  case 486:
    ent = new StepShape_ConnectedEdgeSet;
    break;
  case 487:
    ent = new StepShape_ConnectedFaceShapeRepresentation;
    break;
  case 488:
    ent = new StepShape_EdgeBasedWireframeModel;
    break;
  case 489:
    ent = new StepShape_EdgeBasedWireframeShapeRepresentation;
    break;
  case 490:
    ent = new StepShape_FaceBasedSurfaceModel;
    break;
  case 491:
    ent = new StepShape_NonManifoldSurfaceShapeRepresentation;
    break;
    //Add gka 08.01.02 TRJ9 DIS->IS
  case 492:  
    ent = new StepGeom_OrientedSurface;
    break;
  case 493:  
    ent = new StepShape_Subface;
    break;
   case 494:  
    ent = new StepShape_Subedge;
    break; 
    
   case 495:  
    ent = new StepShape_SeamEdge;
    break; 
    
  case 496:  
    ent = new StepShape_ConnectedFaceSubSet;
    break;   
    
  case 500:  
    ent = new StepBasic_EulerAngles;
    break;
  case 501:  
    ent = new StepBasic_MassUnit;
    break;
  case 502:  
    ent = new StepBasic_ThermodynamicTemperatureUnit;
    break;
  case 503:  
    ent = new StepElement_AnalysisItemWithinRepresentation;
    break;
  case 504:  
    ent = new StepElement_Curve3dElementDescriptor;
    break;
  case 505:  
    ent = new StepElement_CurveElementEndReleasePacket;
    break;
  case 506:  
    ent = new StepElement_CurveElementSectionDefinition;
    break;
  case 507:  
    ent = new StepElement_CurveElementSectionDerivedDefinitions;
    break;
  case 508:  
    ent = new StepElement_ElementDescriptor;
    break;
  case 509:  
    ent = new StepElement_ElementMaterial;
    break;
  case 510:  
    ent = new StepElement_Surface3dElementDescriptor;
    break;
  case 511:  
    ent = new StepElement_SurfaceElementProperty;
    break;
  case 512:  
    ent = new StepElement_SurfaceSection;
    break;
  case 513:  
    ent = new StepElement_SurfaceSectionField;
    break;
  case 514:  
    ent = new StepElement_SurfaceSectionFieldConstant;
    break;
  case 515:  
    ent = new StepElement_SurfaceSectionFieldVarying;
    break;
  case 516:  
    ent = new StepElement_UniformSurfaceSection;
    break;
  case 517:  
    ent = new StepElement_Volume3dElementDescriptor;
    break;
  case 518:  
    ent = new StepFEA_AlignedCurve3dElementCoordinateSystem;
    break;
  case 519:  
    ent = new StepFEA_ArbitraryVolume3dElementCoordinateSystem;
    break;
  case 520:  
    ent = new StepFEA_Curve3dElementProperty;
    break;
  case 521:  
    ent = new StepFEA_Curve3dElementRepresentation;
    break;
  case 522:  
    ent = new StepFEA_Node;
    break;
//case 523:  
//    ent = new StepFEA_CurveElementEndCoordinateSystem;
//    break;
  case 524:  
    ent = new StepFEA_CurveElementEndOffset;
    break;
  case 525:  
    ent = new StepFEA_CurveElementEndRelease;
    break;
  case 526:  
    ent = new StepFEA_CurveElementInterval;
    break;
  case 527:  
    ent = new StepFEA_CurveElementIntervalConstant;
    break;
  case 528:  
    ent = new StepFEA_DummyNode;
    break;
  case 529:  
    ent = new StepFEA_CurveElementLocation;
    break;
  case 530:  
    ent = new StepFEA_ElementGeometricRelationship;
    break;
  case 531:  
    ent = new StepFEA_ElementGroup;
    break;
  case 532:  
    ent = new StepFEA_ElementRepresentation;
    break;
  case 533:  
    ent = new StepFEA_FeaAreaDensity;
    break;
  case 534:  
    ent = new StepFEA_FeaAxis2Placement3d;
    break;
  case 535:  
    ent = new StepFEA_FeaGroup;
    break;
  case 536:  
    ent = new StepFEA_FeaLinearElasticity;
    break;
  case 537:  
    ent = new StepFEA_FeaMassDensity;
    break;
  case 538:  
    ent = new StepFEA_FeaMaterialPropertyRepresentation;
    break;
  case 539:  
    ent = new StepFEA_FeaMaterialPropertyRepresentationItem;
    break;
  case 540:  
    ent = new StepFEA_FeaModel;
    break;
  case 541:  
    ent = new StepFEA_FeaModel3d;
    break;
  case 542:  
    ent = new StepFEA_FeaMoistureAbsorption;
    break;
  case 543:  
    ent = new StepFEA_FeaParametricPoint;
    break;
  case 544:  
    ent = new StepFEA_FeaRepresentationItem;
    break;
  case 545:  
    ent = new StepFEA_FeaSecantCoefficientOfLinearThermalExpansion;
    break;
  case 546:  
    ent = new StepFEA_FeaShellBendingStiffness;
    break;
  case 547:  
    ent = new StepFEA_FeaShellMembraneBendingCouplingStiffness;
    break;
  case 548:  
    ent = new StepFEA_FeaShellMembraneStiffness;
    break;
  case 549:  
    ent = new StepFEA_FeaShellShearStiffness;
    break;
  case 550:  
    ent = new StepFEA_GeometricNode;
    break;
  case 551:  
    ent = new StepFEA_FeaTangentialCoefficientOfLinearThermalExpansion;
    break;
  case 552:  
    ent = new StepFEA_NodeGroup;
    break;
  case 553:  
    ent = new StepFEA_NodeRepresentation;
    break;
  case 554:  
    ent = new StepFEA_NodeSet;
    break;
  case 555:  
    ent = new StepFEA_NodeWithSolutionCoordinateSystem;
    break;
  case 556:  
    ent = new StepFEA_NodeWithVector;
    break;
  case 557:  
    ent = new StepFEA_ParametricCurve3dElementCoordinateDirection;
    break;
  case 558:  
    ent = new StepFEA_ParametricCurve3dElementCoordinateSystem;
    break;
  case 559:  
    ent = new StepFEA_ParametricSurface3dElementCoordinateSystem;
    break;
  case 560:  
    ent = new StepFEA_Surface3dElementRepresentation;
    break;
//case 561:  
//    ent = new StepFEA_SymmetricTensor22d;
//    break;
//case 562:  
//    ent = new StepFEA_SymmetricTensor42d;
//    break;
//case 563:  
//    ent = new StepFEA_SymmetricTensor43d;
//    break;
  case 564:  
    ent = new StepFEA_Volume3dElementRepresentation;
    break;
  case 565:  
    ent = new StepRepr_DataEnvironment;
    break;
  case 566:  
    ent = new StepRepr_MaterialPropertyRepresentation;
    break;
  case 567:  
    ent = new StepRepr_PropertyDefinitionRelationship;
    break;
  case 568:  
    ent = new StepShape_PointRepresentation;
    break;
  case 569:  
    ent = new StepRepr_MaterialProperty;
    break;
  case 570:  
    ent = new StepFEA_FeaModelDefinition;
    break;
  case 571:  
    ent = new StepFEA_FreedomAndCoefficient;
    break;
  case 572:  
    ent = new StepFEA_FreedomsList;
    break;    
  case 573:  
    ent = new StepBasic_ProductDefinitionFormationRelationship;
    break;
  case 574:  
    ent = new StepBasic_SiUnitAndMassUnit;
    break;
  case 575:  
    ent = new StepFEA_NodeDefinition;
    break;
  case 576:  
    ent = new StepRepr_StructuralResponseProperty;
    break;
  case 577:  
    ent = new StepRepr_StructuralResponsePropertyDefinitionRepresentation;
    break;
  case 578:  
    ent = new StepBasic_SiUnitAndThermodynamicTemperatureUnit;
    break;
  case 579:  
    ent = new StepFEA_AlignedSurface3dElementCoordinateSystem;
    break;
  case 580:  
    ent = new StepFEA_ConstantSurface3dElementCoordinateSystem;
    break;
  case 581: 
    ent = new StepFEA_CurveElementIntervalLinearlyVarying;
    break;
  case 582:  
    ent = new StepFEA_FeaCurveSectionGeometricRelationship;
    break;
  case 583:  
    ent = new StepFEA_FeaSurfaceSectionGeometricRelationship;
    break;
    
  case 600:
    ent = new StepBasic_DocumentProductAssociation;
    break;
  case 601:
    ent = new StepBasic_DocumentProductEquivalence;
    break;
  case 609:
    ent = new StepDimTol_CylindricityTolerance;
    break;    
  case 610:
    ent = new StepShape_ShapeRepresentationWithParameters;
    break;
    
  case 611:
    ent = new StepDimTol_AngularityTolerance;
    break;
    
  case 612:
    ent = new StepDimTol_ConcentricityTolerance;
    break;
  case 613:
    ent = new StepDimTol_CircularRunoutTolerance;
    break;
  case 614:
    ent = new StepDimTol_CoaxialityTolerance;
    break;
  case 615:
    ent = new StepDimTol_FlatnessTolerance;
    break;
  case 616:
    ent = new StepDimTol_LineProfileTolerance;
    break;
  case 617:
    ent = new StepDimTol_ParallelismTolerance;
    break;
  case 618:
    ent = new StepDimTol_PerpendicularityTolerance;
    break;
  case 619:
    ent = new StepDimTol_PositionTolerance;
    break;
  case 620:
    ent = new StepDimTol_RoundnessTolerance;
    break;
  case 621:
    ent = new StepDimTol_StraightnessTolerance;
    break;
  case 622:
    ent = new StepDimTol_SurfaceProfileTolerance;
    break;
  case 623:
    ent = new StepDimTol_SymmetryTolerance;
    break;
  case 624:
    ent = new StepDimTol_TotalRunoutTolerance;
    break;
  case 625:
    ent = new StepDimTol_GeometricTolerance;
    break;
  case 626:
    ent = new StepDimTol_GeometricToleranceRelationship;
    break;
  case 627:
    ent = new StepDimTol_GeometricToleranceWithDatumReference;
    break;
  case 628:
    ent = new StepDimTol_ModifiedGeometricTolerance;
    break;
  case 629:
    ent = new StepDimTol_Datum;
    break;
  case 630:
    ent = new StepDimTol_DatumFeature;
    break;
  case 631:
    ent = new StepDimTol_DatumReference;
    break;
  case 632:
    ent = new StepDimTol_CommonDatum;
    break;
  case 633:
    ent = new StepDimTol_DatumTarget;
    break;
  case 634:
    ent = new StepDimTol_PlacedDatumTargetFeature;
    break;
  case 635:  
    ent = new StepRepr_ReprItemAndLengthMeasureWithUnit;
    break;
  case 636:
    ent = new StepDimTol_GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol;
    break;
  case 650 : 
    ent = new StepBasic_ConversionBasedUnitAndMassUnit;
    break;
  case 651 : 
    ent = new StepBasic_MassMeasureWithUnit;
    break;
  case 660 : 
    ent = new StepRepr_Apex;
    break;
  case 661 : 
    ent = new StepRepr_CentreOfSymmetry;
    break;
  case 662 : 
    ent = new StepRepr_GeometricAlignment;
    break;
  case 663 : 
    ent = new StepRepr_PerpendicularTo;
    break;
  case 664 : 
    ent = new StepRepr_Tangent;
    break;
  case 665 : 
    ent = new StepRepr_ParallelOffset;
    break;
  case 666 : 
    ent = new StepAP242_GeometricItemSpecificUsage;
    break;
  case 667 : 
    ent = new StepAP242_IdAttribute;
    break;
  case 668 : 
    ent = new StepAP242_ItemIdentifiedRepresentationUsage;
    break;
  case 669 : 
    ent = new StepRepr_AllAroundShapeAspect;
    break;
  case 670 : 
    ent = new StepRepr_BetweenShapeAspect;
    break;
  case 671 : 
    ent = new StepRepr_CompositeGroupShapeAspect;
    break;
  case 672 : 
    ent = new StepRepr_ContinuosShapeAspect;
    break;
  case 673 : 
    ent = new StepDimTol_GeometricToleranceWithDefinedAreaUnit;
    break;
  case 674 : 
    ent = new StepDimTol_GeometricToleranceWithDefinedUnit;
    break;
  case 675 : 
    ent = new StepDimTol_GeometricToleranceWithMaximumTolerance;
    break;
  case 676 : 
    ent = new StepDimTol_GeometricToleranceWithModifiers;
    break;
  case 677 : 
    ent = new StepDimTol_UnequallyDisposedGeometricTolerance;
    break;
  case 678 : 
    ent = new StepDimTol_NonUniformZoneDefinition;
    break;
  case 679 : 
    ent = new StepDimTol_ProjectedZoneDefinition;
    break;
  case 680 : 
    ent = new StepDimTol_RunoutZoneDefinition;
    break;
  case 681 : 
    ent = new StepDimTol_RunoutZoneOrientation;
    break;
  case 682 : 
    ent = new StepDimTol_ToleranceZone;
    break;
  case 683 : 
    ent = new StepDimTol_ToleranceZoneDefinition;
    break;
  case 684 : 
    ent = new StepDimTol_ToleranceZoneForm;
    break;
  case 685 : 
    ent = new StepShape_ValueFormatTypeQualifier;
    break;
  case 686 : 
    ent = new StepDimTol_DatumReferenceCompartment;
    break;
  case 687 : 
    ent = new StepDimTol_DatumReferenceElement;
    break;
  case 688 : 
    ent = new StepDimTol_DatumReferenceModifierWithValue;
    break;
  case 689 : 
    ent = new StepDimTol_DatumSystem;
    break;
  case 690 : 
    ent = new StepDimTol_GeneralDatumReference;
    break;
  case 691 : 
    ent = new StepRepr_ReprItemAndPlaneAngleMeasureWithUnit;
    break;
  case 692 : 
    ent = new StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI;
    break;
  case 693 : 
    ent = new StepRepr_ReprItemAndPlaneAngleMeasureWithUnitAndQRI;
    break;
  case 694 : 
    ent = new StepDimTol_GeoTolAndGeoTolWthDatRef;
    break;
  case 695 : 
    ent = new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMod;
    break;
  case 696 : 
    ent = new StepDimTol_GeoTolAndGeoTolWthMod;
    break;
  case 697 : 
    ent = new StepDimTol_GeoTolAndGeoTolWthDatRefAndUneqDisGeoTol;
    break;
  case 698 : 
    ent = new StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp;
    break;
  case 699 : 
    ent = new StepRepr_CompShAspAndDatumFeatAndShAsp;
    break;
  case 700:
    ent = new StepRepr_IntegerRepresentationItem;
    break;
  case 701:
    ent = new StepRepr_ValueRepresentationItem;
    break;
  case 702:
    ent = new StepRepr_FeatureForDatumTargetRelationship;
    break;
  case 703:
    ent = new StepAP242_DraughtingModelItemAssociation;
    break;
  case 704:
    ent = new StepVisual_AnnotationPlane;
    break;
  case 705:
    ent = new StepDimTol_GeoTolAndGeoTolWthDatRefAndGeoTolWthMaxTol;
    break;
  case 706:
    ent = new StepDimTol_GeoTolAndGeoTolWthMaxTol;
    break;
   case 707:
        ent = new StepVisual_TessellatedAnnotationOccurrence;
     break;
   case 708:
     ent = new StepVisual_TessellatedItem;     
    break;
   case 709:
     ent = new StepVisual_TessellatedGeometricSet;
   break;
   case 710:
     ent = new StepVisual_TessellatedCurveSet;
   break;
   case 711:
       ent = new StepVisual_CoordinatesList;
   break;
   case 712:
       ent = new StepRepr_ConstructiveGeometryRepresentation;
   break;
   case 713:
       ent = new StepRepr_ConstructiveGeometryRepresentationRelationship;
   break;
   case 714:
   ent = new StepRepr_CharacterizedRepresentation;
   break;
   case 715:
   ent = new StepVisual_CharacterizedObjAndRepresentationAndDraughtingModel;
   break;
   case 716:
     ent = new StepVisual_CameraModelD3MultiClipping;
   break;
   case 717:
     ent = new StepVisual_CameraModelD3MultiClippingIntersection;
   break;
   case 718:
     ent = new StepVisual_CameraModelD3MultiClippingUnion;
   break;
   case 719:
     ent = new StepVisual_AnnotationCurveOccurrenceAndGeomReprItem;
   break;
   case 720:
     ent = new StepVisual_SurfaceStyleTransparent;
   break;
   case 721:
     ent = new StepVisual_SurfaceStyleReflectanceAmbient;
   break;
   case 722:
     ent = new StepVisual_SurfaceStyleRendering;
   break;
   case 723:
     ent = new StepVisual_SurfaceStyleRenderingWithProperties;
   break;
  default: 
    return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : CategoryNumber
//purpose  : 
//=======================================================================

Standard_Integer  RWStepAP214_GeneralModule::CategoryNumber
  (const Standard_Integer CN, const Handle(Standard_Transient)& /*ent*/, const Interface_ShareTool& /*shares*/) const
{
  switch (CN) {
  case   1: return catdsc;
  case   2:
  case   3: return catsh;
  case   4:
  case   5:
  case   6:
  case   7:
  case   8:
  case   9:
  case  10:
  case  11:
  case  12: return catdr;
  case  13:
  case  14:
  case  15:
  case  16:
  case  17:
  case  18:
  case  19:
  case  20:
  case  21: return catdsc;
  case  22: return catdr;
  case  23:
  case  24:
  case  25:
  case  26:
  case  27:
  case  28:
  case  29:
  case  30:
  case  31:
  case  32:
  case  33:
  case  34: return catdsc;
  case  35:
  case  36:
  case  37:
  case  38:
  case  39:
  case  40:
  case  41: return catsh;
  case  42: return catdr;
  case  43:
  case  44:
  case  45:
  case  46:
  case  47:
  case  48:
  case  49:
  case  50:
  case  51:
  case  52: return catsh;
  case  53: return catdsc;
  case  54:
  case  55:
  case  56:
  case  57:
  case  58: return cataux;
  case  59:
  case  60:
  case  61:
  case  62:
  case  63: return catsh;
  case  64:
  case  65:
  case  66: return catdr;
  case  67:
  case  68:
  case  69: return catsh;
  case  70:
  case  71:
  case  72:
  case  73: return catdr;
  case  74:
  case  75:
  case  76: return catsh;
  case  77:
  case  78: return catdr;
  case  79: return cataux;
  case  80: return catdsc;
  case  81:
  case  82:
  case  83:
  case  84:
  case  85:
  case  86:
  case  87:
  case  88:
  case  89:
  case  90: return catsh;
  case  91:
  case  92:
  case  93:
  case  94:
  case  95:
  case  96: return catdsc;
  case  97:
  case  98: return catdr;
  case  99:

  case 100: return catsh;
  case 101:
  case 102:
  case 103:
  case 104: return catdr;
  case 105: return catsh;
  case 106:
  case 107:
  case 108:
  case 109:
  case 110:
  case 111:
  case 112:
  case 113:
  case 114: return catdr;
  case 115:
  case 116:
  case 117:
  case 118:
  case 119:
  case 120: return catsh;
  case 121: return catdsc;
  case 122:
  case 123: return catdr;
  case 124: return catdsc;
  case 125:
  case 126:
  case 127: return catdr;
  case 128:
  case 129:
  case 130:
  case 131:
  case 132:
  case 133:
  case 134:
  case 135: return catsh;
  case 136:
  case 137:
  case 138:
  case 139:
  case 140: return catdr;
  case 141: return cataux;
  case 142: return catsh;
  case 143: return catdr;
  case 144:
  case 145:
  case 146:
  case 147: return catsh;
  case 148:
  case 149: return cataux;
  case 150:
  case 151: return catstr;
  case 152:
  case 153:
  case 154:
  case 155: return catsh;
  case 156: return catdr;
  case 157:
  case 158: return cataux;
  case 159: return catsh;
  case 160: return catdsc;
  case 161:
  case 162:
  case 163: return catsh;
  case 164: return catdr;
  case 165: return cataux;
  case 166:
  case 167:
  case 168: return catdr;
  case 169: return cataux;
  case 170:
  case 171:
  case 172: return catsh;
  case 173: return catdr;
  case 174: return catsh;
  case 175:
  case 176:
  case 177:
  case 178:
  case 179: return catdsc;
  case 180:
  case 181:
  case 182:
  case 183:
  case 184:
  case 185: return catsh;
  case 186: return catdr;
  case 187: return catsh;
  case 188: return catdr;
  case 189:
  case 190: return catsh;
  case 191: 
  case 192:
  case 193:
  case 194:
  case 195: return catdsc;
  case 196: return catsh;
  case 197:
  case 198: return catdr;
  case 199: return catsh;

  case 200:
  case 201: return cataux;
  case 202:
  case 203:
  case 204:
  case 205: return catsh;
  case 206: return catdr;
  case 207:
  case 208: return catsh;
  case 209:
  case 210: return catdr;
  case 211: return catdsc;
  case 212:
  case 213: return catdr;
  case 214:
  case 215:
  case 216:
  case 217:
  case 218:
  case 219:
  case 220:
  case 221: return catdr;
  case 222:
  case 223:
  case 224:
  case 225:
  case 226:
  case 227:
  case 228:
  case 229:
  case 230:
  case 231:
  case 232:
  case 233:
  case 234:
  case 235: return catdsc;
  case 236:
  case 237: return catsh;
  case 238: return cataux;
  case 239:
  case 240:
  case 241:
  case 242: return catsh;
  case 243: return catstr;
  case 244: return catsh;
  case 245:
  case 246:
  case 247:
  case 248:
  case 249: return catstr;
  case 250:
  case 251:
  case 252:
  case 253:
  case 254: return catsh;
  case 255:
  case 256:
  case 257: return catdsc;
  case 258:
  case 259:
  case 260: return catdr;
  case 261: return catsh;  // plutot que dsc
  case 262:
  case 263: return catsh;
  case 264:
  case 265: return cataux;
  case 266:
  case 267:
  case 268:
  case 269: return catsh;
  case 270: return catdr;
  case 271:
  case 272:
  case 273:
  case 274:
  case 275:
  case 276:
  case 277: return catsh;
  case 278:
  case 279:
  case 280:
  case 281:
  case 282:
  case 283:
  case 284: return catdr;
  case 285:
  case 286: return catsh;
  case 287:
  case 288:
  case 289:
  case 290:
  case 291: return catdr;
  case 292:
  case 293: return catstr;
  case 294:
  case 295:
  case 296:
  case 297:
  case 298:
  case 299:

  case 300:
  case 301:
  case 302:
  case 303: return catdr;
  case 304:
  case 305:
  case 306: return catsh;
  case 307: return catstr;
  case 308: return catsh;
  case 309: return catdr;
  case 310: return cataux;
  case 311:
  case 312:
  case 313:
  case 314:
  case 315:
  case 316:
  case 317: return catsh;
  case 318: return catdsc;

  case 319:
  case 320:
  case 321:
  case 322:
  case 323:
  case 324:
  case 325:
  case 326: return catsh;
  case 327:
  case 328:
  case 329:
  case 330:
  case 331: return cataux;
  case 332: return catsh;
  case 333:
  case 334:
  case 335:
  case 336: return cataux;
  case 337:
  case 338: return catsh;
  case 339:
  case 340: return catdsc;
  case 341:
  case 342:
  case 343:
  case 344:
  case 345:
  case 346:
  case 347: return cataux;
  case 348: return catdsc;
  case 349:
  case 350: return cataux;
  case 351: return catsh;
  case 352:
  case 353:
  case 354: return cataux;
  case 355:
  case 356: return catdr;
  case 357: return cataux;
  case 358: return catsh; //:n5
//  CC1 -> CC2
  case 366:
  case 367:
  case 368:
  case 369:
  case 370:
  case 371:
  case 372:
  case 373:
  case 374:
  case 375:
  case 376: return catdsc;
  case 377:
  case 378:
  case 379:
  case 380:
  case 381:
  case 382:
  case 383:
  case 384:
  case 385:
  case 386: return catstr;
  case 387: 
  case 388:
  case 389: return catsh;
  case 390: return cataux;
  case 391: return catsh;
  // CD -> DIS
  case 392: 
  case 393: 
  case 394: 
  case 395: 
  case 396: 
  case 397: 
  case 398: 
  case 399: 
  case 400: 
  case 401: 
  case 402: return catdsc;
  case 403: 
  case 404: 
  case 405: return catsh;    
  // CAX TRJ 2
  case 406: 
  case 407: 
  case 408: 
  case 409: 
  case 410: 
  case 411: 
  case 412: return cataux;
  // AP203
  case 413: 
  case 414: 
  case 415: 
  case 416: 
  case 417: 
  case 418: 
  case 419: 
  case 420: 
  case 421: 
  case 422: 
  case 423: 
  case 424: 
  case 425: 
  case 426: 
  case 427: 
  case 428: 
  case 429: 
  case 430: 
  case 431: 
  case 432: 
  case 433: 
  case 434: 
  case 435: 
  case 436: 
  case 437: 
  case 438: 
  case 439: 
  case 440: 
  case 441: return catdsc;
  case 442:
  case 443:
  case 444:
  case 445:
  case 446:
  case 447:
  case 448:
  case 449: return catdsc; // ??
  case 450:
  case 451:
  case 452:
  case 453:
  case 454:
  case 455:
  case 456:
  case 457:
  case 458:
  case 459:
  case 460:
  case 461:
  case 462: return catdsc; // ??
  case 463: return catdr; // same as 98
  case 471:
  case 472:
  case 473:
  case 474:
  case 475:
  case 476:
  case 477:
  case 478:
  case 479:
  case 480:
  case 481:
  case 482:
  case 483:
  case 484: return catdr;
  case 485:
  case 486:
  case 487:
  case 488:
  case 489:
  case 490:
  case 491: return catsh;
  case 492:
  case 493:
  case 494:
  case 495:
  case 496: return catsh;
  case 600:
  case 601: return cataux;
  case 609:
  case 610: 
  case 611:
  case 612:
  case 613:
  case 614:
  case 615:
  case 616:
  case 617:
  case 618:
  case 619:
  case 620:
  case 621:
  case 622:
  case 623:
  case 624:
  case 625:
  case 626:
  case 627:
  case 628:
  case 629:
  case 630:  
  case 631:
  case 632:
  case 633:
  case 634: return cataux;
  case 635: return cataux;
  case 636: return cataux;
  case 650: 
  case 651: return cataux;
  case 660:
  case 661:
  case 662:
  case 663:
  case 664:
  case 665: 
  case 666:
  case 667:
  case 668:
  case 669:
  case 670:
  case 671:
  case 672: return catdr;
  case 673:
  case 674:
  case 675:
  case 676:
  case 677:
  case 678:
  case 679:
  case 680:
  case 681:
  case 682:
  case 683:
  case 684: return cataux;
  case 685: return catdr;
  case 686:
  case 687:
  case 688:
  case 689:
  case 690:
  case 691:
  case 692:
  case 693:
  case 694:
  case 695:
  case 696:
  case 697: return cataux;
  case 698:
  case 699:
  case 700:
  case 701:
  case 702:
  case 703:
  case 704: return catdr;
  case 705:
  case 706: 
  case 707:  
  case 708: 
  case 709:
  case 710:
  case 711: return cataux;
  case 712:
  case 713: return catsh;
  case 714: return catstr;
  case 715: return catdsc;
  case 716:
  case 717:
  case 718: return cataux;
  case 719: return catdr;
  case 720:
  case 721:
  case 722:
  case 723: return catdr;

  default : break;
  }
  return 0;
}


//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString)  RWStepAP214_GeneralModule::Name
       (const Standard_Integer /*CN*/, const Handle(Standard_Transient)& /*ent*/,
        const Interface_ShareTool& ) const
{
  //   On joue par down-cast et non par CN, car Name est en general heritee
  //   (on pourrait filtrer par CN pour decider quel down-cast faire ...)
  Handle(TCollection_HAsciiString) nom;
  return nom;
}
