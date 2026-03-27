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
#include <TDF_LabelSequence.hxx>
#include <TopTools_MapOfShape.hxx>
#include <STEPCAFControl_DataMapOfShapePD.hxx>
#include <STEPCAFControl_DataMapOfPDExternFile.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
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

  
  //! Creates a reader with an empty
  //! STEP model and sets ColorMode, LayerMode, NameMode and
  //! PropsMode to true.
  Standard_EXPORT STEPCAFControl_Reader();
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT STEPCAFControl_Reader(const occ::handle<XSControl_WorkSession>& WS, const bool scratch = true);

  //! Destructor.
  Standard_EXPORT virtual ~STEPCAFControl_Reader();

  //! Clears the internal data structures and attaches to a new session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT void Init (const occ::handle<XSControl_WorkSession>& WS, const bool scratch = true);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param theFileName [in] file to open
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile (const char* const theFileName);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param theFileName [in] file to open
  //! @param theParams [in] default configuration parameters
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile(const char* const theFileName,
                                                 const StepData_ConfParameters& theParams);

  //! Loads a file from stream and returns the read status.
  //! @param theName [in] auxiliary stream name
  //! @param theIStream [in] stream to read from
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadStream (const char* const theName,
                                                    std::istream& theIStream);

  //! Returns number of roots recognized for transfer
  //! Shortcut for Reader().NbRootsForTransfer()
  Standard_EXPORT int NbRootsForTransfer();
  
  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT bool TransferOneRoot (const int num,
                                                    const occ::handle<TDocStd_Document>& doc,
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT bool Transfer (const occ::handle<TDocStd_Document>& doc,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT bool Perform (const TCollection_AsciiString& filename,
                                            const occ::handle<TDocStd_Document>& doc,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT bool Perform (const TCollection_AsciiString& filename,
                                            const occ::handle<TDocStd_Document>& doc,
                                            const StepData_ConfParameters& theParams,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
 
  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT bool Perform (const char* const filename,
                                            const occ::handle<TDocStd_Document>& doc,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT bool Perform (const char* const filename,
                                            const occ::handle<TDocStd_Document>& doc,
                                            const StepData_ConfParameters& theParams,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Returns data on external files
  //! Returns Null handle if no external files are read
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, occ::handle<STEPCAFControl_ExternFile>> & ExternFiles() const;
  
  //! Returns data on external file by its name
  //! Returns False if no external file with given name is read
  Standard_EXPORT bool ExternFile (const char* const name, occ::handle<STEPCAFControl_ExternFile>& ef) const;
  
  //! Returns basic reader
  Standard_EXPORT STEPControl_Reader& ChangeReader();
  
  //! Returns basic reader as const
  Standard_EXPORT const STEPControl_Reader& Reader() const;
  
  //! Returns label of instance of an assembly component
  //! corresponding to a given NAUO
  Standard_EXPORT static TDF_Label FindInstance
                (const occ::handle<StepRepr_NextAssemblyUsageOccurrence>& NAUO, 
                 const occ::handle<XCAFDoc_ShapeTool>& STool,
                 const STEPConstruct_Tool& Tool, 
                 const XCAFDoc_DataMapOfShapeLabel& ShapeLabelMap);
  
  //! Set ColorMode for indicate read Colors or not.
  Standard_EXPORT void SetColorMode (const bool colormode);
  
  Standard_EXPORT bool GetColorMode() const;
  
  //! Set NameMode for indicate read Name or not.
  Standard_EXPORT void SetNameMode (const bool namemode);
  
  Standard_EXPORT bool GetNameMode() const;

  //! Set LayerMode for indicate read Layers or not.
  Standard_EXPORT void SetLayerMode (const bool layermode);
  
  Standard_EXPORT bool GetLayerMode() const;
  
  //! PropsMode for indicate read Validation properties or not.
  Standard_EXPORT void SetPropsMode (const bool propsmode);
  
  Standard_EXPORT bool GetPropsMode() const;

  //! MetaMode for indicate read Metadata or not.
  Standard_EXPORT void SetMetaMode(const bool theMetaMode);

  Standard_EXPORT bool GetMetaMode() const;
  
  //! Set SHUO mode for indicate write SHUO or not.
  Standard_EXPORT void SetSHUOMode (const bool shuomode);
  
  Standard_EXPORT bool GetSHUOMode() const;
  
  //! Set GDT mode for indicate write GDT or not.
  Standard_EXPORT void SetGDTMode (const bool gdtmode);
  
  Standard_EXPORT bool GetGDTMode() const;
  
  //! Set Material mode
  Standard_EXPORT void SetMatMode (const bool matmode);
  
  Standard_EXPORT bool GetMatMode() const;
  
  //! Set View mode
  Standard_EXPORT void SetViewMode(const bool viewmode);

  //! Get View mode
  Standard_EXPORT bool GetViewMode() const;

  const XCAFDoc_DataMapOfShapeLabel& GetShapeLabelMap() const { return myMap; }

protected:

  
  //! Translates STEP file already loaded into the reader
  //! into the document
  //! If num==0, translates all roots, else only root number num
  //! Returns True if succeeded, and False in case of fail
  //! If asOne is True, in case of multiple results composes
  //! them into assembly. Fills sequence of produced labels
  Standard_EXPORT bool Transfer (STEPControl_Reader& rd,
                                             const int num,
                                             const occ::handle<TDocStd_Document>& doc,
                                             TDF_LabelSequence& Lseq,
                                             const bool asOne = false,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Add a shape to a document
  //! Depending on a case, this shape can be added as one, or
  //! as assembly, or (in case if it is associated with external
  //! reference) taken as that referred shape
  Standard_EXPORT TDF_Label AddShape (const TopoDS_Shape& S, const occ::handle<XCAFDoc_ShapeTool>& STool, const TopTools_MapOfShape& NewShapesMap, const STEPCAFControl_DataMapOfShapePD& ShapePDMap, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap);
  
  //! Reads (or if returns already read) extern file with
  //! given name
  Standard_EXPORT occ::handle<STEPCAFControl_ExternFile> ReadExternFile (const char* const file,
                                                                    const char* const fullpath,
                                                                    const occ::handle<TDocStd_Document>& doc,
                                   const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Reads style assignments from STEP model and sets
  //! corresponding color assignments in the DECAF document
  Standard_EXPORT bool ReadColors
                (const occ::handle<XSControl_WorkSession>& WS,
                 const occ::handle<TDocStd_Document>& doc,
                 const StepData_Factors& theLocalFactors = StepData_Factors()) const;
  
  //! Reads names of parts defined in the STEP model and
  //! assigns them to corresponding labels in the DECAF document
  Standard_EXPORT bool ReadNames (const occ::handle<XSControl_WorkSession>& WS, const occ::handle<TDocStd_Document>& doc, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap) const;
  
  //! Reads validation properties assigned to shapes in the STEP
  //! model and assigns them to corresponding labels in the DECAF
  //! document
  Standard_EXPORT bool ReadValProps (const occ::handle<XSControl_WorkSession>& WS,
                                                 const occ::handle<TDocStd_Document>& doc,
                                                 const STEPCAFControl_DataMapOfPDExternFile& PDFileMap,
                                                 const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Reads metadata assigned to shapes in the STEP model and
  //! assigns them to corresponding labels in the DECAF document
  Standard_EXPORT bool ReadMetadata(const occ::handle<XSControl_WorkSession>& theWS,
                                                const occ::handle<TDocStd_Document>& theDoc,
                                                const StepData_Factors& theLocalFactors = StepData_Factors()) const;
  
  //! Reads layers of parts defined in the STEP model and
  //! set reference between shape and layers in the DECAF document
  Standard_EXPORT bool ReadLayers (const occ::handle<XSControl_WorkSession>& WS, const occ::handle<TDocStd_Document>& doc) const;
  
  //! Reads SHUO for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool ReadSHUOs (const occ::handle<XSControl_WorkSession>& WS, const occ::handle<TDocStd_Document>& doc, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap) const;
  
  //! Reads D&GT for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool ReadGDTs (const occ::handle<XSControl_WorkSession>& WS,
                                             const occ::handle<TDocStd_Document>& doc,
                                             const StepData_Factors& theLocalFactors = StepData_Factors());
  
  //! Reads materials for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT bool ReadMaterials (const occ::handle<XSControl_WorkSession>& WS,
                                                  const occ::handle<TDocStd_Document>& doc,
                                                  const occ::handle<TColStd_HSequenceOfTransient>& SeqPDS,
                                                  const StepData_Factors& theLocalFactors = StepData_Factors()) const;
  
  //! Reads Views for instances defined in the STEP model
  Standard_EXPORT bool ReadViews(const occ::handle<XSControl_WorkSession>& theWS,
                                             const occ::handle<TDocStd_Document>& theDoc,
                                             const StepData_Factors& theLocalFactors = StepData_Factors()) const;

  //! Populates the sub-Label of the passed TDF Label with shape
  //! data associated with the given STEP Representation Item,
  //! including naming and topological information.
  Standard_EXPORT TDF_Label SettleShapeData (const occ::handle<StepRepr_RepresentationItem>& theItem, const TDF_Label& theLab, const occ::handle<XCAFDoc_ShapeTool>& theShapeTool, const occ::handle<Transfer_TransientProcess>& theTP) const;
  
  //! Given the maps of already translated shapes, this method
  //! expands their correspondent Labels in XDE Document so that
  //! to have a dedicated sub-Label for each sub-shape coming
  //! with associated name in its STEP Representation Item.
  Standard_EXPORT void ExpandSubShapes (const occ::handle<XCAFDoc_ShapeTool>& theShapeTool, const STEPCAFControl_DataMapOfShapePD& theShapePDMap) const;
  
  //! Expands the topological structure of Manifold Solid BRep
  //! STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandManifoldSolidBrep (TDF_Label& theLab, const occ::handle<StepRepr_RepresentationItem>& theItem, const occ::handle<Transfer_TransientProcess>& theTP, const occ::handle<XCAFDoc_ShapeTool>& theShapeTool) const;
  
  //! Expands the topological structure of Shell-Based Surface
  //! Model STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandSBSM (TDF_Label& theLab, const occ::handle<StepRepr_RepresentationItem>& theItem, const occ::handle<Transfer_TransientProcess>& theTP, const occ::handle<XCAFDoc_ShapeTool>& theShapeTool) const;
  
  //! Expands STEP Shell structure to OCAF sub-tree. Entities
  //! having no names associated via their Representation Items
  //! are skipped.
  Standard_EXPORT void ExpandShell (const occ::handle<StepShape_ConnectedFaceSet>& theShell, TDF_Label& theLab, const occ::handle<Transfer_TransientProcess>& theTP, const occ::handle<XCAFDoc_ShapeTool>& theShapeTool) const;

  //! Convert name into UNICODE text.
  Standard_EXPORT virtual TCollection_ExtendedString convertName (const TCollection_AsciiString& theName) const;

private:

  //! Internal method. Import all Datum attributes and set them to XCAF object. Set connection of Datum to GeomTolerance (theGDTL).
  bool setDatumToXCAF(const occ::handle<StepDimTol_Datum>& theDat,
    const TDF_Label theGDTL,
    const int thePositionCounter,
    const XCAFDimTolObjects_DatumModifiersSequence& theXCAFModifiers,
    const XCAFDimTolObjects_DatumModifWithValue theXCAFModifWithVal,
    const double theModifValue,
    const occ::handle<TDocStd_Document>& theDoc,
    const occ::handle<XSControl_WorkSession>& theWS,
    const StepData_Factors& theLocalFactors = StepData_Factors());
  
  //! Internal method. Read Datums, connected to GeomTolerance theGDTL.
  bool readDatumsAP242(const occ::handle<Standard_Transient>& theEnt,
    const TDF_Label theGDTL,
    const occ::handle<TDocStd_Document>& theDoc,
    const occ::handle<XSControl_WorkSession>& theWS,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Internal method. Read Dimension or GeomTolerance.
  TDF_Label createGDTObjectInXCAF(const occ::handle<Standard_Transient>& theEnt,
    const occ::handle<TDocStd_Document>& theDoc,
    const occ::handle<XSControl_WorkSession>& theWS,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  //! Prepares units for transfer
  void prepareUnits(const occ::handle<StepData_StepModel>& theModel,
                    const occ::handle<TDocStd_Document>& theDoc,
                    StepData_Factors& theLocalFactors) const;

  //! Find RepresentationItems
  bool findReprItems(const occ::handle<XSControl_WorkSession> & theWS,
                                 const occ::handle<StepShape_ShapeDefinitionRepresentation> & theShDefRepr,
                                 NCollection_List<occ::handle<Transfer_Binder>>& theBinders) const;

  //! Fill metadata
  bool fillAttributes(const occ::handle<XSControl_WorkSession>& theWS,
                                  const occ::handle<StepRepr_PropertyDefinition>& thePropDef,
                                  const StepData_Factors& theLocalFactors,
                                  occ::handle<TDataStd_NamedData>& theAttr) const;

private:

  STEPControl_Reader myReader;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<STEPCAFControl_ExternFile>> myFiles;
  XCAFDoc_DataMapOfShapeLabel myMap;
  bool myColorMode;
  bool myNameMode;
  bool myLayerMode;
  bool myPropsMode;
  bool myMetaMode;
  bool mySHUOMode;
  bool myGDTMode;
  bool myMatMode;
  bool myViewMode;
  NCollection_DataMap<occ::handle<Standard_Transient>, TDF_Label> myGDTMap;

};

#endif // _STEPCAFControl_Reader_HeaderFile
