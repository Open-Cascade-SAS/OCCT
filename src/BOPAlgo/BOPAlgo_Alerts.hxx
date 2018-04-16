// Created on: 2017-06-26
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_Alerts_HeaderFile
#define _BOPAlgo_Alerts_HeaderFile

#include <TopoDS_AlertWithShape.hxx>

//! Boolean operation of given type is not allowed on the given inputs
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertBOPNotAllowed)

//! The type of Boolean Operation is not set
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertBOPNotSet)

//! Building of the result shape has failed
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertBuilderFailed)

//! The intersection of the arguments has failed
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertIntersectionFailed)

//! The type of Boolean Operation is not set
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertMultipleArguments)

//! The Pave Filler (the intersection tool) has not been created
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertNoFiller)

//! Null input shapes
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertNullInputShapes)

//! Cannot connect face intersection curves
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertPostTreatFF)

//! The BuilderSolid algorithm has failed
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertSolidBuilderFailed)

//! There are no enough arguments to perform the operation
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertTooFewArguments)

//! The positioning of the shapes leads to creation of the small edges without valid range
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertBadPositioning)

//! Some of the arguments are empty shapes
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertEmptyShape)

//! Some edges are very small and have such a small valid range, that they cannot be split
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertNotSplittableEdge)

//! Removal of internal boundaries among Edges has failed
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertRemovalOfIBForEdgesFailed)

//! Removal of internal boundaries among Faces has failed
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertRemovalOfIBForFacesFailed)

//! Removal of internal boundaries among the multi-dimensional shapes is not supported yet
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertRemovalOfIBForMDimShapes)

//! Removal of internal boundaries among Solids has failed
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertRemovalOfIBForSolidsFailed)

//! Some of the arguments are self-interfering shapes
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertSelfInterferingShape)

//! The positioning of the shapes leads to creation of the small edges without valid range
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertShellSplitterFailed)

//! Some edges are too small and have no valid range
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertTooSmallEdge)

//! Intersection of pair of shapes has failed
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertIntersectionOfPairOfShapesFailed)

//! Building 2D curve of edge on face has failed
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertBuildingPCurveFailed)

//! Some sub-shapes of some of the argument become connected through
//! other shapes and the argument became self-interfered
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertAcquiredSelfIntersection)

//! Unsupported type of input shape
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertUnsupportedType)

//! No faces have been found for removal
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertNoFacesToRemove)

//! Unable to remove the feature
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertUnableToRemoveTheFeature)

//! The Feature Removal algorithm has failed
DEFINE_SIMPLE_ALERT(BOPAlgo_AlertRemoveFeaturesFailed)

//! Some of the faces passed to the Solid Builder algorithm have not been classified
//! and not used for solids creation
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertSolidBuilderUnusedFaces)

//! Unable to orient the shape correctly
DEFINE_ALERT_WITH_SHAPE(BOPAlgo_AlertUnableToOrientTheShape)

#endif // _BOPAlgo_Alerts_HeaderFile
