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

#ifndef _STEPCAFControl_Reader_HeaderFile
#define _STEPCAFControl_Reader_HeaderFile

#include <STEPControl_Reader.hxx>
#include <StepData_Factors.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <NCollection_DataMap.hxx>
#include <TDF_Label.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>

class XSControl_WorkSession;
class TDataStd_NamedData;
class TDocStd_Document;
class STEPCAFControl_ExternFile;
class TopoDS_Shape;
class XCAFDoc_ShapeTool;
class StepRepr_RepresentationItem;
class Transfer_TransientProcess;
class StepBasic_NamedUnit;
class StepBasic_Product;
class StepShape_ConnectedFaceSet;
class StepShape_ShapeDefinitionRepresentation;
class StepRepr_NextAssemblyUsageOccurrence;
class StepRepr_PropertyDefinition;
class STEPConstruct_Tool;
class StepDimTol_Datum;
class Transfer_Binder;

//! Provides a tool to read STEP file and put it into
//! DECAF document. Besides transfer of shapes (including
//! assemblies) provided by STEPControl, supports also
//! colors and part names
//!
//! This reader supports reading files with external references
//! i.e. multifile reading
//! It behaves as usual Reader (from STEPControl) for the main
//! file (e.g. if it is single file)
//! Results of reading other files can be accessed by name of the
//! file or by iterating on a readers
class STEPCAFControl_Reader
{
public:
  DEFINE_STANDARD_ALLOC

public:
  //! Creates a reader with an empty
  //! STEP model and sets ColorMode, LayerMode, NameMode and
  //! PropsMode to true.
  Standard_EXPORT STEPCAFControl_Reader();

  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT STEPCAFControl_Reader(const occ::handle<XSControl_WorkSession>& WS,
                                        const bool scratch = true);

  //! Destructor.
  Standard_EXPORT virtual ~STEPCAFControl_Reader();

  //! Clears the internal data structures and attaches to a new session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT void Init(const occ::handle<XSControl_WorkSession>& WS,
                            const bool               scratch = true);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param[in] theFileName  file to open
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile(const char* const theFileName);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param[in] theFileName  file to open
  //! @param[in] theParams  default configuration parameters
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile(const char* const   theFileName,
                                                 const DESTEP_Parameters& theParams);

  //! Loads a file from stream and returns the read status.
  //! @param[in] theName  auxiliary stream name
  //! @param[in] theIStream  stream to read from
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadStream(const char* const theName,
                                                   std::istream&          theIStream);

  //! Returns number of roots recognized for transfer
  //! Shortcut for Reader().NbRootsForTransfer()
  Standard_EXPORT int NbRootsForTransfer();

  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT bool
    TransferOneRoot(const int          num,
                    const occ::handle<TDocStd_Document>& doc,
                    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT bool
    Transfer(const occ::handle<TDocStd_Document>& doc,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  Standard_EXPORT bool
    Perform(const TCollection_AsciiString&  filename,
            const occ::handle<TDocStd_Document>& doc,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  Standard_EXPORT bool
    Perform(const TCollection_AsciiString&  filename,
            const occ::handle<TDocStd_Document>& doc,
            const DESTEP_Parameters&        theParams,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT bool
    Perform(const char* const          filename,
            const occ::handle<TDocStd_Document>& doc,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT bool
    Perform(const char* const          filename,
            const occ::handle<TDocStd_Document>& doc,
            const DESTEP_Parameters&        theParams,
            const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Returns data on external files
  //! Returns Null handle if no external files are read
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString,
                                            occ::handle<STEPCAFControl_ExternFile>>&
                        ExternFiles() const;

  //! Returns data on external file by its name
  //! Returns False if no external file with given name is read
  Standard_EXPORT bool ExternFile(const char* const             name,
                                              occ::handle<STEPCAFControl_ExternFile>& ef) const;

  //! Returns basic reader
  Standard_EXPORT STEPControl_Reader& ChangeReader();

  //! Returns basic reader as const
  Standard_EXPORT const STEPControl_Reader& Reader() const;

  //! Returns label of instance of an assembly component
  //! corresponding to a given NAUO
  Standard_EXPORT static TDF_Label FindInstance(
    const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NAUO,
    const occ::handle<XCAFDoc_ShapeTool>&                    STool,
    const STEPConstruct_Tool&                           Tool,
    const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>&                  ShapeLabelMap);

  //! Set ColorMode for indicate read Colors or not.
  Standard_EXPORT void SetColorMode(const bool colormode);

  Standard_EXPORT bool GetColorMode() const;

  //! Set NameMode for indicate read Name or not.
  Standard_EXPORT void SetNameMode(const bool namemode);

  Standard_EXPORT bool GetNameMode() const;

  //! Set LayerMode for indicate read Layers or not.
  Standard_EXPORT void SetLayerMode(const bool layermode);

  Standard_EXPORT bool GetLayerMode() const;

  //! PropsMode for indicate read Validation properties or not.
  Standard_EXPORT void SetPropsMode(const bool propsmode);

  Standard_EXPORT bool GetPropsMode() const;

  //! MetaMode for indicate read Metadata or not.
  Standard_EXPORT void SetMetaMode(const bool theMetaMode);

  Standard_EXPORT bool GetMetaMode() const;

  //! MetaMode for indicate whether to read Product Metadata or not.
  Standard_EXPORT void SetProductMetaMode(const bool theProductMetaMode);

  Standard_EXPORT bool GetProductMetaMode() const;

  //! Set SHUO mode for indicate write SHUO or not.
  Standard_EXPORT void SetSHUOMode(const bool shuomode);

  Standard_EXPORT bool GetSHUOMode() const;

  //! Set GDT mode for indicate write GDT or not.
  Standard_EXPORT void SetGDTMode(const bool gdtmode);

  Standard_EXPORT bool GetGDTMode() const;

  //! Set Material mode
  Standard_EXPORT void SetMatMode(const bool matmode);

  Standard_EXPORT bool GetMatMode() const;

  //! Set View mode
  Standard_EXPORT void SetViewMode(const bool viewmode);

  //! Get View mode
  Standard_EXPORT bool GetViewMode() const;

  const NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>& GetShapeLabelMap() const { return myMap; }

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
  //! Translates STEP file already loaded into the reader
  //! into the document
  //! If num==0, translates all roots, else only root number num
  //! Returns True if succeeded, and False in case of fail
  //! If asOne is True, in case of multiple results composes
  //! them into assembly. Fills sequence of produced labels
  Standard_EXPORT bool
    Transfer(STEPControl_Reader&             rd,
             const int          num,
             const occ::handle<TDocStd_Document>& doc,
             NCollection_Sequence<TDF_Label>&              Lseq,
             const bool          asOne       = false,
             const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Add a shape to a document
  //! Depending on a case, this shape can be added as one, or
  //! as assembly, or (in case if it is associated with external
  //! reference) taken as that referred shape
  Standard_EXPORT TDF_Label AddShape(const TopoDS_Shape&                         S,
                                     const occ::handle<XCAFDoc_ShapeTool>&            STool,
                                     const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                  NewShapesMap,
                                     const NCollection_DataMap<TopoDS_Shape, occ::handle<StepBasic_ProductDefinition>, TopTools_ShapeMapHasher>&      ShapePDMap,
                                     const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>, occ::handle<STEPCAFControl_ExternFile>>& PDFileMap);

  //! Reads (or if returns already read) extern file with
  //! given name
  Standard_EXPORT occ::handle<STEPCAFControl_ExternFile> ReadExternFile(
    const char* const          file,
    const char* const          fullpath,
    const occ::handle<TDocStd_Document>& doc,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads style assignments from STEP model and sets
  //! corresponding color assignments in the DECAF document
  Standard_EXPORT bool
    ReadColors(const occ::handle<XSControl_WorkSession>& WS,
               const occ::handle<TDocStd_Document>&      doc,
               const StepData_Factors&              theLocalFactors = StepData_Factors()) const;

  //! Reads names of parts defined in the STEP model and
  //! assigns them to corresponding labels in the DECAF document
  Standard_EXPORT bool
    ReadNames(const occ::handle<XSControl_WorkSession>&        WS,
              const occ::handle<TDocStd_Document>&             doc,
              const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>, occ::handle<STEPCAFControl_ExternFile>>& PDFileMap) const;

  //! Reads validation properties assigned to shapes in the STEP
  //! model and assigns them to corresponding labels in the DECAF
  //! document
  Standard_EXPORT bool
    ReadValProps(const occ::handle<XSControl_WorkSession>&        WS,
                 const occ::handle<TDocStd_Document>&             doc,
                 const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>, occ::handle<STEPCAFControl_ExternFile>>& PDFileMap,
                 const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Reads metadata assigned to shapes in the STEP model and
  //! assigns them to corresponding labels in the DECAF document
  Standard_EXPORT bool
    ReadMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                 const occ::handle<TDocStd_Document>&      theDoc,
                 const StepData_Factors&              theLocalFactors = StepData_Factors()) const;

  Standard_EXPORT bool
    ReadProductMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                        const occ::handle<TDocStd_Document>&      theDoc) const;

  //! Reads layers of parts defined in the STEP model and
  //! set reference between shape and layers in the DECAF document
  Standard_EXPORT bool ReadLayers(const occ::handle<XSControl_WorkSession>& WS,
                                              const occ::handle<TDocStd_Document>&      doc) const;

  //! Reads SHUO for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool
    ReadSHUOs(const occ::handle<XSControl_WorkSession>&        WS,
              const occ::handle<TDocStd_Document>&             doc,
              const NCollection_DataMap<occ::handle<StepBasic_ProductDefinition>, occ::handle<STEPCAFControl_ExternFile>>& PDFileMap) const;

  //! Reads D&GT for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool
    ReadGDTs(const occ::handle<XSControl_WorkSession>& WS,
             const occ::handle<TDocStd_Document>&      doc,
             const StepData_Factors&              theLocalFactors = StepData_Factors());

  //! Reads materials for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool
    ReadMaterials(const occ::handle<XSControl_WorkSession>&        WS,
                  const occ::handle<TDocStd_Document>&             doc,
                  const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& SeqPDS,
                  const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Reads Views for instances defined in the STEP model
  Standard_EXPORT bool
    ReadViews(const occ::handle<XSControl_WorkSession>& theWS,
              const occ::handle<TDocStd_Document>&      theDoc,
              const StepData_Factors&              theLocalFactors = StepData_Factors()) const;

  //! Populates the sub-Label of the passed TDF Label with shape
  //! data associated with the given STEP Representation Item,
  //! including naming and topological information.
  Standard_EXPORT TDF_Label SettleShapeData(const occ::handle<StepRepr_RepresentationItem>& theItem,
                                            const TDF_Label&                           theLab,
                                            const occ::handle<XCAFDoc_ShapeTool>&           theShapeTool,
                                            const occ::handle<Transfer_TransientProcess>&   theTP) const;

  //! Given the maps of already translated shapes, this method
  //! expands their correspondent Labels in XDE Document so that
  //! to have a dedicated sub-Label for each sub-shape coming
  //! with associated name in its STEP Representation Item.
  Standard_EXPORT void ExpandSubShapes(const occ::handle<XCAFDoc_ShapeTool>&       theShapeTool,
                                       const NCollection_DataMap<TopoDS_Shape, occ::handle<StepBasic_ProductDefinition>, TopTools_ShapeMapHasher>& theShapePDMap) const;

  //! Expands the topological structure of Manifold Solid BRep
  //! STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandManifoldSolidBrep(TDF_Label&                                 theLab,
                                               const occ::handle<StepRepr_RepresentationItem>& theItem,
                                               const occ::handle<Transfer_TransientProcess>&   theTP,
                                               const occ::handle<XCAFDoc_ShapeTool>& theShapeTool) const;

  //! Expands the topological structure of Shell-Based Surface
  //! Model STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandSBSM(TDF_Label&                                 theLab,
                                  const occ::handle<StepRepr_RepresentationItem>& theItem,
                                  const occ::handle<Transfer_TransientProcess>&   theTP,
                                  const occ::handle<XCAFDoc_ShapeTool>&           theShapeTool) const;

  //! Expands STEP Shell structure to OCAF sub-tree. Entities
  //! having no names associated via their Representation Items
  //! are skipped.
  Standard_EXPORT void ExpandShell(const occ::handle<StepShape_ConnectedFaceSet>& theShell,
                                   TDF_Label&                                theLab,
                                   const occ::handle<Transfer_TransientProcess>&  theTP,
                                   const occ::handle<XCAFDoc_ShapeTool>&          theShapeTool) const;

  //! Convert name into UNICODE text.
  Standard_EXPORT virtual TCollection_ExtendedString convertName(
    const TCollection_AsciiString& theName) const;

private:
  //! Internal method. Import all Datum attributes and set them to XCAF object. Set connection of
  //! Datum to GeomTolerance (theGDTL).
  bool setDatumToXCAF(const occ::handle<StepDimTol_Datum>& theDat,
                                  const TDF_Label                 theGDTL,
                                  const int          thePositionCounter,
                                  const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif>& theXCAFModifiers,
                                  const XCAFDimTolObjects_DatumModifWithValue theXCAFModifWithVal,
                                  const double                         theModifValue,
                                  const occ::handle<TDocStd_Document>&             theDoc,
                                  const occ::handle<XSControl_WorkSession>&        theWS,
                                  const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Internal method. Read Datums, connected to GeomTolerance theGDTL.
  bool readDatumsAP242(const occ::handle<Standard_Transient>&    theEnt,
                                   const TDF_Label                      theGDTL,
                                   const occ::handle<TDocStd_Document>&      theDoc,
                                   const occ::handle<XSControl_WorkSession>& theWS,
                                   const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Internal method. Read Dimension or GeomTolerance.
  TDF_Label createGDTObjectInXCAF(const occ::handle<Standard_Transient>&    theEnt,
                                  const occ::handle<TDocStd_Document>&      theDoc,
                                  const occ::handle<XSControl_WorkSession>& theWS,
                                  const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Prepares units for transfer
  void prepareUnits(const occ::handle<StepData_StepModel>& theModel,
                    const occ::handle<TDocStd_Document>&   theDoc,
                    StepData_Factors&                 theLocalFactors) const;

  //! Find RepresentationItems
  bool findReprItems(
    const occ::handle<XSControl_WorkSession>&                   theWS,
    const occ::handle<StepShape_ShapeDefinitionRepresentation>& theShDefRepr,
    NCollection_List<occ::handle<Transfer_Binder>>&             theBinders) const;

  //! Fill metadata
  bool fillAttributes(const occ::handle<XSControl_WorkSession>&       theWS,
                                  const occ::handle<StepRepr_PropertyDefinition>& thePropDef,
                                  const StepData_Factors&                    theLocalFactors,
                                  occ::handle<TDataStd_NamedData>&                theAttr) const;

  //! Returns the label of the shape associated with the given product definition.
  //! @param theTransferProcess The transfer process to use for finding the label.
  //! @param theProductDefinition The product definition for which to find the label.
  //! @return The label of the shape associated with the given product definition.
  TDF_Label getShapeLabelFromProductDefinition(
    const occ::handle<Transfer_TransientProcess>&   theTransferProcess,
    const occ::handle<StepBasic_ProductDefinition>& theProductDefinition) const;

  //! Returns the product associated with the given product definition.
  //! @param theProductDefinition The product definition for which to get the product.
  //! @return The product associated with the given product definition.
  occ::handle<StepBasic_Product> getProductFromProductDefinition(
    const occ::handle<StepBasic_ProductDefinition>& theProductDefinition) const;

  //! Collects property definitions from the given general property and stores them in a vector.
  //! @param theWorkSession The work session to use for collecting property definitions.
  //! @param theGeneralProperty The general property from which to collect property definitions.
  //! @return A vector of collected property definitions.
  std::vector<occ::handle<StepRepr_PropertyDefinition>> collectPropertyDefinitions(
    const occ::handle<XSControl_WorkSession>& theWorkSession,
    const occ::handle<Standard_Transient>&    theGeneralProperty) const;

  //! Collects shape labels from the given property definition.
  //! @param theWorkSession The work session to use for collecting shape labels.
  //! @param theTransferProcess The transfer process to use for collecting shape labels.
  //! @param theSource The property definition from which to collect shape labels.
  //! @return A vector of collected shape labels.
  std::vector<TDF_Label> collectShapeLabels(
    const occ::handle<XSControl_WorkSession>&       theWorkSession,
    const occ::handle<Transfer_TransientProcess>&   theTransferProcess,
    const occ::handle<StepRepr_PropertyDefinition>& theSource) const;

  //! Collects a group of related property definitions from the given property definition.
  //! @p theProperty itself will be added to the result as well.
  //! @param theWorkSession The work session to use for collecting related property definitions.
  //! @param theProperty The property definition from which to collect related property definitions.
  //! @return A vector of collected related property definitions.
  std::vector<occ::handle<StepRepr_PropertyDefinition>> collectRelatedPropertyDefinitions(
    const occ::handle<XSControl_WorkSession>&       theWorkSession,
    const occ::handle<StepRepr_PropertyDefinition>& theProperty) const;

  //! Helper method to get NamedData attribute assigned to the given label.
  //! @param theLabel The label to get NamedData attribute from.
  //! @return Handle to the NamedData attribute. Guarantees that the attribute is never null.
  occ::handle<TDataStd_NamedData> getNamedData(const TDF_Label& theLabel) const;

  //! Collects binders from the given property definition and stores them in the binder list.
  //! @param theWorkSession The work session to use for collecting binders.
  //! @param theTransientProcess The transient process to use for collecting binders.
  //! @param theSource The property definition from which to collect binders.
  //! @param theBinders The list to store the collected binders.
  void collectBinders(const occ::handle<XSControl_WorkSession>&       theWorkSession,
                      const occ::handle<Transfer_TransientProcess>&   theTransientProcess,
                      const occ::handle<StepRepr_PropertyDefinition>& theSource,
                      NCollection_List<occ::handle<Transfer_Binder>>& theBinders) const;

private:
  STEPControl_Reader                                                              myReader;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<STEPCAFControl_ExternFile>> myFiles;
  NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>                                                     myMap;
  bool                                                                myColorMode;
  bool                                                                myNameMode;
  bool                                                                myLayerMode;
  bool                                                                myPropsMode;
  bool                                                                myMetaMode;
  bool                                                                myProductMetaMode;
  bool                                                                mySHUOMode;
  bool                                                                myGDTMode;
  bool                                                                myMatMode;
  bool                                                                myViewMode;
  NCollection_DataMap<occ::handle<Standard_Transient>, TDF_Label>                      myGDTMap;
};

#endif // _STEPCAFControl_Reader_HeaderFile
