// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XSAlgo_ShapeProcessor_HeaderFile
#define _XSAlgo_ShapeProcessor_HeaderFile

#include <DE_ShapeFixParameters.hxx>
#include <ShapeProcess.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

class ShapeProcess_ShapeContext;
class ShapeExtend_MsgRegistrator;
class Transfer_TransientProcess;
class Transfer_FinderProcess;
class Transfer_Binder;

//! Shape Processing module.
//! Allows to define and apply general Shape Processing as a customizable sequence of operators.
class XSAlgo_ShapeProcessor
{
public:
  using ParameterMap   = NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>;
  using ProcessingData = std::pair<ParameterMap, ShapeProcess::OperationsFlags>;
  // Flags defining operations to be performed on shapes. Since there is no std::optional in C++11,
  // we use a pair. The first element is the flags, the second element is a boolean value that
  // indicates whether the flags were set.
  using ProcessingFlags = std::pair<ShapeProcess::OperationsFlags, bool>;

public:
  //! Constructor.
  //! @param theParameters Pre-filled parameter map to be used in the processing.
  //! @param theShapeFixParameters Shape healing parameters to be used in the processing.
  //!        If @p theParameters has some shape healing values, they will override the
  //!        corresponding values from @p theShapeFixParameters.
  Standard_EXPORT XSAlgo_ShapeProcessor(const ParameterMap&          theParameters,
                                        const DE_ShapeFixParameters& theShapeFixParameters = {});

  //! Constructor.
  //! @param theParameters Parameters to be used in the processing.
  Standard_EXPORT XSAlgo_ShapeProcessor(const DE_ShapeFixParameters& theParameters);

  //! Process the shape by applying the specified operations.
  //! @param theShape Shape to process.
  //! @param theOperations Operations to be performed.
  //! @param theProgress Progress indicator.
  //! @return Processed shape. May be the same as the input shape if no modifications were made.
  Standard_EXPORT TopoDS_Shape ProcessShape(const TopoDS_Shape&                  theShape,
                                            const ShapeProcess::OperationsFlags& theOperations,
                                            const Message_ProgressRange&         theProgress);

  //! Get the context of the last processing.
  //! Only valid after the ProcessShape() method was called.
  //! @return Shape context.
  Handle(ShapeProcess_ShapeContext) GetContext() { return myContext; }

  //! Merge the results of the shape processing with the transfer process.
  //! @param theTransientProcess Transfer process to merge with.
  //! @param theFirstTPItemIndex Index of the first item in the transfer process to merge with.
  Standard_EXPORT void MergeTransferInfo(
    const Handle(Transfer_TransientProcess)& theTransientProcess,
    const Standard_Integer                   theFirstTPItemIndex) const;

  //! Merge the results of the shape processing with the finder process.
  //! @param theFinderProcess Finder process to merge with.
  Standard_EXPORT void MergeTransferInfo(
    const Handle(Transfer_FinderProcess)& theFinderProcess) const;

  //! Check quality of pcurve of the edge on the given face, and correct it if necessary.
  //! @param theEdge Edge to check.
  //! @param theFace Face on which the edge is located.
  //! @param thePrecision Precision to use for checking.
  //! @param theIsSeam Flag indicating whether the edge is a seam edge.
  //! @return True if the pcurve was corrected, false if it was dropped.
  Standard_EXPORT static Standard_Boolean CheckPCurve(const TopoDS_Edge&     theEdge,
                                                      const TopoDS_Face&     theFace,
                                                      const Standard_Real    thePrecision,
                                                      const Standard_Boolean theIsSeam);

  //! Reads the parameter map from and operation flags from the file specified in static interface.
  //! @param theFileResourceName Name of the parameter in interface static that contains the name
  //!        of the file. For example, parameter "read.iges.resource.name" may contain string
  //!        "IGES".
  //! @param theScopeResourceName Name of the parameter in interface static that contains the name
  //!        of the scope. For example, parameter "read.iges.sequence" may contain string
  //!        "FromIGES".
  //! @return Read parameter map.
  Standard_EXPORT static ProcessingData ReadProcessingData(
    const TCollection_AsciiString& theFileResourceName,
    const TCollection_AsciiString& theScopeResourceName);

  //! Fill the parameter map with the values from the specified parameters.
  //! @param theParameters Parameters to be used in the processing.
  //! @param theIsForce Flag indicating whether parameter should be replaced if it already exists in
  //! the map.
  //! @param theMap Map to fill.
  Standard_EXPORT static void FillParameterMap(const DE_ShapeFixParameters& theParameters,
                                               const bool                   theIsReplace,
                                               ParameterMap&                theMap);

  //! Sets parameters for shape processing.
  //! Parameters from @p theParameters are copied to the output map.
  //! Parameters from @p theAdditionalParameters are copied to the output map
  //! if they are not present in @p theParameters.
  //! @param theParameters the parameters for shape processing.
  //! @param theAdditionalParameters the additional parameters for shape processing.
  //! @param theTargetParameterMap Map to set the parameters in.
  Standard_EXPORT static void SetShapeFixParameters(const DE_ShapeFixParameters& theParameters,
                                                    const ParameterMap& theAdditionalParameters,
                                                    ParameterMap&       theTargetParameterMap);

  //! Set the parameter in the map.
  //! @param theKey Key of the parameter.
  //! @param theValue Value of the parameter.
  //! @param theIsReplace Flag indicating whether parameter should be replaced if it already exists
  //! in the map.
  //! @param theMap Map to set the parameter in.
  Standard_EXPORT static void SetParameter(const char*                          theKey,
                                           const DE_ShapeFixParameters::FixMode theValue,
                                           const bool                           theIsReplace,
                                           XSAlgo_ShapeProcessor::ParameterMap& theMap);

  //! Set the parameter in the map.
  //! @param theKey Key of the parameter.
  //! @param theValue Value of the parameter.
  //! @param theIsReplace Flag indicating whether parameter should be replaced if it already exists
  //! in the map.
  //! @param theMap Map to set the parameter in.
  Standard_EXPORT static void SetParameter(const char*   theKey,
                                           const double  theValue,
                                           const bool    theIsReplace,
                                           ParameterMap& theMap);

  //! Set the parameter in the map.
  //! @param theKey Key of the parameter.
  //! @param theValue Value of the parameter.
  //! @param theIsReplace Flag indicating whether parameter should be replaced if it already exists
  //! in the map.
  //! @param theMap Map to set the parameter in.
  Standard_EXPORT static void SetParameter(const char*                    theKey,
                                           const TCollection_AsciiString& theValue,
                                           const bool                     theIsReplace,
                                           ParameterMap&                  theMap);

  //! The function is designed to set the length unit for the application before performing a
  //! transfer operation. It ensures that the length unit is correctly configured based on the
  //! value associated with the key "xstep.cascade.unit".
  Standard_EXPORT static void PrepareForTransfer();

  //! Merge the results of the shape processing with the finder process.
  //! @param theTransientProcess Transfer process to merge with.
  //! @param theModifiedShapesMap Map of modified shapes.
  //! @param theFirstTPItemIndex Index of the first item in the transfer process to merge with.
  //! @param theMessages Messages to add.
  Standard_EXPORT static void MergeShapeTransferInfo(
    const Handle(Transfer_TransientProcess)& theFinderProcess,
    const TopTools_DataMapOfShapeShape&      theModifiedShapesMap,
    const Standard_Integer                   theFirstTPItemIndex,
    Handle(ShapeExtend_MsgRegistrator)       theMessages);

  //! Merge the results of the shape processing with the transfer process.
  //! @param theTransientProcess Transfer process to merge with.
  //! @param theModifiedShapesMap Map of modified shapes.
  //! @param theFirstTPItemIndex Index of the first item in the transfer process to merge with.
  //! @param theMessages Messages to add.
  Standard_EXPORT static void MergeShapeTransferInfo(
    const Handle(Transfer_FinderProcess)& theTransientProcess,
    const TopTools_DataMapOfShapeShape&   theModifiedShapesMap,
    Handle(ShapeExtend_MsgRegistrator)    theMessages);

private:
  //! Initialize the context with the specified shape.
  //! @param theShape Shape to process.
  void initializeContext(const TopoDS_Shape& theShape);

  //! Add messages from the specified shape to the transfer binder.
  //! @param theMessages Container with messages.
  //! @param theShape Shape to get messages from.
  //! @param theBinder Transfer binder to add messages to.
  static void addMessages(const Handle(ShapeExtend_MsgRegistrator)& theMessages,
                          const TopoDS_Shape&                       theShape,
                          Handle(Transfer_Binder)&                  theBinder);

  //! Create a new edge with the same geometry as the source edge.
  //! @param theSourceEdge Source edge.
  //! @return New edge with the same geometry.
  static TopoDS_Edge MakeEdgeOnCurve(const TopoDS_Edge& aSourceEdge);

private:
  ParameterMap                      myParameters; //!< Parameters to be used in the processing.
  Handle(ShapeProcess_ShapeContext) myContext;    //!< Shape context.
};

#endif // _XSAlgo_ShapeProcessor_HeaderFile
