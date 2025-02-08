// Created on: 2000-08-15
// Created by: Andrey BETENEV
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

#ifndef _STEPCAFControl_Writer_HeaderFile
#define _STEPCAFControl_Writer_HeaderFile

#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <STEPCAFControl_DataMapOfLabelShape.hxx>
#include <STEPCAFControl_DataMapOfLabelExternFile.hxx>
#include <STEPControl_Writer.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <DESTEP_Parameters.hxx>
#include <StepData_Factors.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_LabelMap.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>

class XSControl_WorkSession;
class TDocStd_Document;
class STEPCAFControl_ExternFile;
class TopoDS_Shape;

//! Provides a tool to write DECAF document to the
//! STEP file. Besides transfer of shapes (including
//! assemblies) provided by STEPControl, supports also
//! colors and part names
//!
//! Also supports multifile writing
class STEPCAFControl_Writer
{
public:
  DEFINE_STANDARD_ALLOC

public:
  //! Creates a writer with an empty
  //! STEP model and sets ColorMode, LayerMode, NameMode and
  //! PropsMode to Standard_True.
  Standard_EXPORT STEPCAFControl_Writer();

  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for STEP
  //! Clears the internal data structures
  Standard_EXPORT STEPCAFControl_Writer(const Handle(XSControl_WorkSession)& theWS,
                                        const Standard_Boolean theScratch = Standard_True);

  //! Clears the internal data structures and attaches to a new session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT void Init(const Handle(XSControl_WorkSession)& theWS,
                            const Standard_Boolean               theScratch = Standard_True);

  //! Writes all the produced models into file
  //! In case of multimodel with extern references,
  //! filename will be a name of root file, all other files
  //! have names of corresponding parts
  //! Provided for use like single-file writer
  Standard_EXPORT IFSelect_ReturnStatus Write(const Standard_CString theFileName);

  //! Writes all the produced models into the stream.
  //! Provided for use like single-file writer
  Standard_EXPORT IFSelect_ReturnStatus WriteStream(std::ostream& theStream);

  //! Transfers a document (or single label) to a STEP model
  //! The mode of translation of shape is AsIs
  //! If multi is not null pointer, it switches to multifile
  //! mode (with external refs), and string pointed by <multi>
  //! gives prefix for names of extern files (can be empty string)
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean
    Transfer(const Handle(TDocStd_Document)& theDoc,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Transfers a document (or single label) to a STEP model
  //! This method uses if need to set parameters avoiding
  //! initialization from Interface_Static
  //! @param theParams  configuration parameters
  //! @param theMode    mode of translation of shape is AsIs
  //! @param theIsMulti if multi is not null pointer, it switches to multifile
  //!                   mode (with external refs), and string pointed by <multi>
  //!                   gives prefix for names of extern files (can be empty string)
  //! @param theProgress progress indicator
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean
    Transfer(const Handle(TDocStd_Document)& theDoc,
             const DESTEP_Parameters&        theParams,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Method to transfer part of the document specified by label
  Standard_EXPORT Standard_Boolean
    Transfer(const TDF_Label&                theLabel,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Method to transfer part of the document specified by label
  //! This method uses if need to set parameters avoiding
  //! initialization from Interface_Static
  Standard_EXPORT Standard_Boolean
    Transfer(const TDF_Label&                theLabel,
             const DESTEP_Parameters&        theParams,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Method to writing sequence of root assemblies
  //! or part of the file specified by use by one label
  Standard_EXPORT Standard_Boolean
    Transfer(const TDF_LabelSequence&        theLabelSeq,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Method to writing sequence of root assemblies
  //! or part of the file specified by use by one label.
  //! This method is utilized if there's a need to set parameters avoiding
  //! initialization from Interface_Static
  Standard_EXPORT Standard_Boolean
    Transfer(const TDF_LabelSequence&        theLabelSeq,
             const DESTEP_Parameters&        theParams,
             const STEPControl_StepModelType theMode     = STEPControl_AsIs,
             const Standard_CString          theIsMulti  = 0,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  Standard_EXPORT Standard_Boolean
    Perform(const Handle(TDocStd_Document)& theDoc,
            const TCollection_AsciiString&  theFileName,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Transfers a document and writes it to a STEP file
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean
    Perform(const Handle(TDocStd_Document)& theDoc,
            const Standard_CString          theFileName,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Transfers a document and writes it to a STEP file
  //! This method is utilized if there's a need to set parameters avoiding
  //! initialization from Interface_Static
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean
    Perform(const Handle(TDocStd_Document)& theDoc,
            const Standard_CString          theFileName,
            const DESTEP_Parameters&        theParams,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Returns data on external files
  //! Returns Null handle if no external files are read
  const NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>&
    ExternFiles() const
  {
    return myFiles;
  };

  //! Returns data on external file by its original label
  //! Returns False if no external file with given name is read
  Standard_EXPORT Standard_Boolean ExternFile(const TDF_Label&                   theLabel,
                                              Handle(STEPCAFControl_ExternFile)& theExtFile) const;

  //! Returns data on external file by its name
  //! Returns False if no external file with given name is read
  Standard_EXPORT Standard_Boolean ExternFile(const Standard_CString             theName,
                                              Handle(STEPCAFControl_ExternFile)& theExtFile) const;

  //! Returns basic reader for root file
  STEPControl_Writer& ChangeWriter() { return myWriter; }

  //! Returns basic reader as const
  const STEPControl_Writer& Writer() const { return myWriter; }

  //! Set ColorMode for indicate write Colors or not.
  void SetColorMode(const Standard_Boolean theColorMode) { myColorMode = theColorMode; }

  Standard_Boolean GetColorMode() const { return myColorMode; }

  //! Set NameMode for indicate write Name or not.
  void SetNameMode(const Standard_Boolean theNameMode) { myNameMode = theNameMode; }

  Standard_Boolean GetNameMode() const { return myNameMode; }

  //! Set LayerMode for indicate write Layers or not.
  void SetLayerMode(const Standard_Boolean theLayerMode) { myLayerMode = theLayerMode; }

  Standard_Boolean GetLayerMode() const { return myLayerMode; }

  //! PropsMode for indicate write Validation properties or not.
  void SetPropsMode(const Standard_Boolean thePropsMode) { myPropsMode = thePropsMode; }

  Standard_Boolean GetPropsMode() const { return myPropsMode; }

  //! Set SHUO mode for indicate write SHUO or not.
  void SetSHUOMode(const Standard_Boolean theSHUOMode) { mySHUOMode = theSHUOMode; }

  Standard_Boolean GetSHUOMode() const { return mySHUOMode; }

  //! Set dimtolmode for indicate write D&GTs or not.
  void SetDimTolMode(const Standard_Boolean theDimTolMode) { myGDTMode = theDimTolMode; };

  Standard_Boolean GetDimTolMode() const { return myGDTMode; }

  //! Set dimtolmode for indicate write D&GTs or not.
  void SetMaterialMode(const Standard_Boolean theMaterialMode) { myMatMode = theMaterialMode; }

  Standard_Boolean GetMaterialMode() const { return myMatMode; }

  //! Sets parameters for shape processing.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(
    const XSAlgo_ShapeProcessor::ParameterMap& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters are moved from the input map.
  //! @param theParameters the parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(XSAlgo_ShapeProcessor::ParameterMap&& theParameters);

  //! Sets parameters for shape processing.
  //! Parameters from @p theParameters are copied to the internal map.
  //! Parameters from @p theAdditionalParameters are copied to the internal map
  //! if they are not present in @p theParameters.
  //! @param theParameters the parameters for shape processing.
  //! @param theAdditionalParameters the additional parameters for shape processing.
  Standard_EXPORT void SetShapeFixParameters(
    const DE_ShapeFixParameters&               theParameters,
    const XSAlgo_ShapeProcessor::ParameterMap& theAdditionalParameters = {});

  //! Returns parameters for shape processing that was set by SetParameters() method.
  //! @return the parameters for shape processing. Empty map if no parameters were set.
  Standard_EXPORT const XSAlgo_ShapeProcessor::ParameterMap& GetShapeFixParameters() const;

  //! Sets flags defining operations to be performed on shapes.
  //! @param theFlags The flags defining operations to be performed on shapes.
  Standard_EXPORT void SetShapeProcessFlags(const ShapeProcess::OperationsFlags& theFlags);

  //! Returns flags defining operations to be performed on shapes.
  //! @return Pair of values defining operations to be performed on shapes and a boolean value
  //!         that indicates whether the flags were set.
  Standard_EXPORT const XSAlgo_ShapeProcessor::ProcessingFlags& GetShapeProcessFlags() const;

protected:
  //! Transfers labels to a STEP model
  //! Returns True if translation is OK
  //! isExternFile setting from transferExternFiles method
  Standard_Boolean transfer(STEPControl_Writer&             theWriter,
                            const TDF_LabelSequence&        theLabels,
                            const STEPControl_StepModelType theMode      = STEPControl_AsIs,
                            const Standard_CString          theIsMulti   = 0,
                            const Standard_Boolean          isExternFile = Standard_False,
                            const Message_ProgressRange&    theProgress  = Message_ProgressRange());

  //! Parses assembly structure of label L, writes all the simple
  //! shapes each to its own file named by name of its label plus
  //! prefix
  //! Returns shape representing that assembly structure
  //! in the form of nested empty compounds (and a sequence of
  //! labels which are newly written nodes of this assembly)
  TopoDS_Shape transferExternFiles(
    const TDF_Label&                theLabel,
    const STEPControl_StepModelType theMode,
    TDF_LabelSequence&              theLabelSeq,
    const StepData_Factors&         theLocalFactors = StepData_Factors(),
    const Standard_CString          thePrefix       = "",
    const Message_ProgressRange&    theProgress     = Message_ProgressRange());

  //! Write external references to STEP
  Standard_Boolean writeExternRefs(const Handle(XSControl_WorkSession)& theWS,
                                   const TDF_LabelSequence&             theLabels) const;

  //! Write colors assigned to specified labels, to STEP model
  Standard_Boolean writeColors(const Handle(XSControl_WorkSession)& theWS,
                               const TDF_LabelSequence&             theLabels);

  //! Write names assigned to specified labels, to STEP model
  Standard_Boolean writeNames(const Handle(XSControl_WorkSession)& theWS,
                              const TDF_LabelSequence&             theLabels) const;

  //! Write D&GTs assigned to specified labels, to STEP model
  Standard_Boolean writeDGTs(const Handle(XSControl_WorkSession)& theWS,
                             const TDF_LabelSequence&             theLabels) const;

  //! Write D&GTs assigned to specified labels, to STEP model, according AP242
  Standard_Boolean writeDGTsAP242(const Handle(XSControl_WorkSession)& theWS,
                                  const TDF_LabelSequence&             theLabels,
                                  const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Write materials assigned to specified labels, to STEP model
  Standard_Boolean writeMaterials(const Handle(XSControl_WorkSession)& theWS,
                                  const TDF_LabelSequence&             theLabels) const;

  //! Write validation properties assigned to specified labels,
  //! to STEP model
  Standard_Boolean writeValProps(const Handle(XSControl_WorkSession)& theWS,
                                 const TDF_LabelSequence&             theLabels,
                                 const Standard_CString               theIsMulti) const;

  //! Write layers assigned to specified labels, to STEP model
  Standard_Boolean writeLayers(const Handle(XSControl_WorkSession)& theWS,
                               const TDF_LabelSequence&             theLabels) const;

  //! Write SHUO assigned to specified component, to STEP model
  Standard_Boolean writeSHUOs(const Handle(XSControl_WorkSession)& theWS,
                              const TDF_LabelSequence&             theLabels);

  //! Finds length units located in root of label
  //! If it exists, initializes local length unit from it
  //! Else initializes according to Cascade length unit
  void prepareUnit(const TDF_Label&                  theLabel,
                   const Handle(StepData_StepModel)& theModel,
                   StepData_Factors&                 theLocalFactors);

  Handle(StepRepr_ShapeAspect) writeShapeAspect(
    const Handle(XSControl_WorkSession)&          theWS,
    const TDF_Label                               theLabel,
    const TopoDS_Shape&                           theShape,
    Handle(StepRepr_RepresentationContext)&       theRC,
    Handle(StepAP242_GeometricItemSpecificUsage)& theGISU);

  void writePresentation(const Handle(XSControl_WorkSession)&    theWS,
                         const TopoDS_Shape&                     thePresentation,
                         const Handle(TCollection_HAsciiString)& thePrsName,
                         const Standard_Boolean                  theHasSemantic,
                         const Standard_Boolean                  theHasPlane,
                         const gp_Ax2&                           theAnnotationPlane,
                         const gp_Pnt&                           theTextPosition,
                         const Handle(Standard_Transient)&       theDimension,
                         const StepData_Factors& theLocalFactors = StepData_Factors());

  Handle(StepDimTol_Datum) writeDatumAP242(
    const Handle(XSControl_WorkSession)& theWS,
    const TDF_LabelSequence&             theShapeL,
    const TDF_Label&                     theDatumL,
    const Standard_Boolean               isFirstDTarget,
    const Handle(StepDimTol_Datum)&      theWrittenDatum,
    const StepData_Factors&              theLocalFactors = StepData_Factors());

  void writeToleranceZone(const Handle(XSControl_WorkSession)&                 theWS,
                          const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObject,
                          const Handle(StepDimTol_GeometricTolerance)&         theEntity,
                          const Handle(StepRepr_RepresentationContext)&        theRC);

  void writeGeomTolerance(const Handle(XSControl_WorkSession)&                      theWS,
                          const TDF_LabelSequence&                                  theShapeSeqL,
                          const TDF_Label&                                          theGeomTolL,
                          const Handle(StepDimTol_HArray1OfDatumSystemOrReference)& theDatumSystem,
                          const Handle(StepRepr_RepresentationContext)&             theRC,
                          const StepData_Factors& theLocalFactors = StepData_Factors());

private:
  STEPControl_Writer                                                              myWriter;
  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> myFiles;
  TDF_LabelMap                                                                    myRootLabels;
  STEPCAFControl_DataMapOfLabelShape                                              myLabels;
  STEPCAFControl_DataMapOfLabelExternFile                                         myLabEF;
  STEPCAFControl_DataMapOfLabelShape                                              myPureRefLabels;
  Standard_Boolean                                                                myColorMode;
  Standard_Boolean                                                                myNameMode;
  Standard_Boolean                                                                myLayerMode;
  Standard_Boolean                                                                myPropsMode;
  Standard_Boolean                                                                mySHUOMode;
  MoniTool_DataMapOfShapeTransient                                                myMapCompMDGPR;
  Standard_Boolean                                                                myGDTMode;
  Standard_Boolean                                                                myMatMode;
  NCollection_Vector<Handle(StepRepr_RepresentationItem)>                         myGDTAnnotations;
  Handle(StepVisual_DraughtingModel)                      myGDTPresentationDM;
  Handle(StepVisual_HArray1OfPresentationStyleAssignment) myGDTPrsCurveStyle;
  Handle(StepRepr_ProductDefinitionShape)                 myGDTCommonPDS;
};

#endif // _STEPCAFControl_Writer_HeaderFile
