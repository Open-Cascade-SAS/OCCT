!!!!!!!!!! ----- Messages file for BOPAlgo package --------- !!!!!!!!!
!  Keyword is name of corresponding alert (error or warning) class

! Special key used to check that messages are loaded; see BOPAlgo_Options.cxx
.BOPAlgo_LOAD_CHECKER
N/A

.BOPAlgo_AlertBOPIsNotSet
The type of Boolean Operation is not set

.BOPAlgo_AlertBOPNotAllowed
Boolean operation of the given type is not allowed on the given inputs

.BOPAlgo_AlertSolidBuilderFailed
Building Fused solid has failed

.BOPAlgo_AlertTooFewArguments
There are no enough arguments to perform the operation

.BOPAlgo_AlertMultipleArguments
More than one argument is provided

.BOPAlgo_AlertNoFiller
The Pave Filler (the intersection tool) has not been created

.BOPAlgo_AlertIntersectionFailed
The intersection of the arguments has failed

.BOPAlgo_AlertBuilderFailed
Building of the result shape has failed

.BOPAlgo_AlertNullFace
The face given to be split is a null shape

.BOPAlgo_AlertNullInputShapes
No or null input shapes

.BOPAlgo_AlertPostTreatFF
Cannot connect face intersection curves

.BOPAlgo_AlertEmptyShape
Some of the arguments are empty shapes

.BOPAlgo_AlertSelfInterferingShape
Some of the arguments are self-interfering shapes

.BOPAlgo_AlertTooSmallEdge
Some edges are too small and have no valid range

.BOPAlgo_AlertNotSplittableEdge
Some edges are very small and have such a small valid range, that they cannot be split

.BOPAlgo_AlertBadPositioning
The positioning of the shapes leads to creation of the small edges without valid range

.BOPAlgo_AlertShellSplitterFailed
Unable to build loops from the given faces

.BOPAlgo_AlertRemovalOfIBForMDimShapes
Removal of internal boundaries among the multi-dimensional shapes is not supported yet

.BOPAlgo_AlertRemovalOfIBForSolidsFailed
Removal of internal boundaries among Solids has failed

.BOPAlgo_AlertRemovalOfIBForFacesFailed
Removal of internal boundaries among Faces has failed

.BOPAlgo_AlertRemovalOfIBForEdgesFailed
Removal of internal boundaries among Edges has failed

.BOPAlgo_AlertIntersectionOfPairOfShapesFailed
Intersection of pair of shapes has failed

.BOPAlgo_AlertBuildingPCurveFailed
Building 2D curve of edge on face has failed

.BOPAlgo_AlertAcquiredSelfIntersection
Some sub-shapes of some of the argument become connected through other shapes and the argument became self-interfered

.BOPAlgo_AlertUnsupportedType
Unsupported type of input shape

.BOPAlgo_AlertUnableToRemoveTheFeature
Unable to remove the feature

.BOPAlgo_AlertNoFacesToRemove
No faces have been found for removal

.BOPAlgo_AlertRemoveFeaturesFailed
The Feature Removal algorithm has failed

.BOPAlgo_AlertSolidBuilderUnusedFaces
Some of the faces passed to the Solid Builder algorithm have not been classified and not used for solids creation

.BOPAlgo_AlertFaceBuilderUnusedEdges
Some of the edges passed to the Face Builder algorithm have not been classified and not used for faces creation

.BOPAlgo_AlertUnableToOrientTheShape
Unable to orient the shape correctly

.BOPAlgo_AlertUnknownShape
Shape is unknown for operation

.BOPAlgo_AlertNoPeriodicityRequired
No periodicity has been requested for the shape

.BOPAlgo_AlertUnableToTrim
Unable to trim the shape for making it periodic (BOP Common fails)

.BOPAlgo_AlertUnableToMakeIdentical
Unable to make the shape to look identical on opposite sides (Splitter fails)

.BOPAlgo_AlertUnableToRepeat
Unable to repeat the shape (Gluer fails)

.BOPAlgo_AlertMultiDimensionalArguments
Multi-dimensional arguments

.BOPAlgo_AlertUnableToMakePeriodic
Unable to make the shape periodic

.BOPAlgo_AlertUnableToGlue
Unable to glue the shapes

.BOPAlgo_AlertShapeIsNotPeriodic
The shape is not periodic

.BOPAlgo_AlertUnableToMakeClosedEdgeOnFace
Unable to make closed edge on face.
