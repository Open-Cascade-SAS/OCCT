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
class TDocStd_Document;
class STEPCAFControl_ExternFile;
class TopoDS_Shape;
class XCAFDoc_ShapeTool;
class StepRepr_RepresentationItem;
class Transfer_TransientProcess;
class StepShape_ConnectedFaceSet;
class StepRepr_NextAssemblyUsageOccurrence;
class STEPConstruct_Tool;
class StepDimTol_Datum;
class StepData_Factors;


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
  //! PropsMode to Standard_True.
  Standard_EXPORT STEPCAFControl_Reader();
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT STEPCAFControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);

  //! Destructor.
  Standard_EXPORT virtual ~STEPCAFControl_Reader();

  //! Clears the internal data structures and attaches to a new session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT void Init (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param theFileName [in] file to open
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile (const Standard_CString theFileName);

  //! Loads a file and returns the read status
  //! Provided for use like single-file reader.
  //! @param theFileName [in] file to open
  //! @param theParams [in] default configuration parameters
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadFile(const Standard_CString theFileName,
                                                 const StepData_ConfParameters& theParams);

  //! Loads a file from stream and returns the read status.
  //! @param theName [in] auxiliary stream name
  //! @param theIStream [in] stream to read from
  //! @return read status
  Standard_EXPORT IFSelect_ReturnStatus ReadStream (const Standard_CString theName,
                                                    std::istream& theIStream);

  //! Returns number of roots recognized for transfer
  //! Shortcut for Reader().NbRootsForTransfer()
  Standard_EXPORT Standard_Integer NbRootsForTransfer();
  
  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT Standard_Boolean TransferOneRoot (const Standard_Integer num,
                                                    const Handle(TDocStd_Document)& doc,
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates currently loaded STEP file into the document
  //! Returns True if succeeded, and False in case of fail
  //! Provided for use like single-file reader
  Standard_EXPORT Standard_Boolean Transfer (const Handle(TDocStd_Document)& doc,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_EXPORT Standard_Boolean Perform (const TCollection_AsciiString& filename,
                                            const Handle(TDocStd_Document)& doc,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT Standard_Boolean Perform (const TCollection_AsciiString& filename,
                                            const Handle(TDocStd_Document)& doc,
                                            const StepData_ConfParameters& theParams,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
 
  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Perform (const Standard_CString filename,
                                            const Handle(TDocStd_Document)& doc,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Translate STEP file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Perform (const Standard_CString filename,
                                            const Handle(TDocStd_Document)& doc,
                                            const StepData_ConfParameters& theParams,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Returns data on external files
  //! Returns Null handle if no external files are read
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> & ExternFiles() const;
  
  //! Returns data on external file by its name
  //! Returns False if no external file with given name is read
  Standard_EXPORT Standard_Boolean ExternFile (const Standard_CString name, Handle(STEPCAFControl_ExternFile)& ef) const;
  
  //! Returns basic reader
  Standard_EXPORT STEPControl_Reader& ChangeReader();
  
  //! Returns basic reader as const
  Standard_EXPORT const STEPControl_Reader& Reader() const;
  
  //! Returns label of instance of an assembly component
  //! corresponding to a given NAUO
  Standard_EXPORT static TDF_Label FindInstance
                (const Handle(StepRepr_NextAssemblyUsageOccurrence)& NAUO, 
                 const Handle(XCAFDoc_ShapeTool)& STool,
                 const STEPConstruct_Tool& Tool, 
                 const XCAFDoc_DataMapOfShapeLabel& ShapeLabelMap);
  
  //! Set ColorMode for indicate read Colors or not.
  Standard_EXPORT void SetColorMode (const Standard_Boolean colormode);
  
  Standard_EXPORT Standard_Boolean GetColorMode() const;
  
  //! Set NameMode for indicate read Name or not.
  Standard_EXPORT void SetNameMode (const Standard_Boolean namemode);
  
  Standard_EXPORT Standard_Boolean GetNameMode() const;

  //! Set LayerMode for indicate read Layers or not.
  Standard_EXPORT void SetLayerMode (const Standard_Boolean layermode);
  
  Standard_EXPORT Standard_Boolean GetLayerMode() const;
  
  //! PropsMode for indicate read Validation properties or not.
  Standard_EXPORT void SetPropsMode (const Standard_Boolean propsmode);
  
  Standard_EXPORT Standard_Boolean GetPropsMode() const;
  
  //! Set SHUO mode for indicate write SHUO or not.
  Standard_EXPORT void SetSHUOMode (const Standard_Boolean shuomode);
  
  Standard_EXPORT Standard_Boolean GetSHUOMode() const;
  
  //! Set GDT mode for indicate write GDT or not.
  Standard_EXPORT void SetGDTMode (const Standard_Boolean gdtmode);
  
  Standard_EXPORT Standard_Boolean GetGDTMode() const;
  
  //! Set Material mode
  Standard_EXPORT void SetMatMode (const Standard_Boolean matmode);
  
  Standard_EXPORT Standard_Boolean GetMatMode() const;
  
  //! Set View mode
  Standard_EXPORT void SetViewMode(const Standard_Boolean viewmode);

  //! Get View mode
  Standard_EXPORT Standard_Boolean GetViewMode() const;

  const XCAFDoc_DataMapOfShapeLabel& GetShapeLabelMap() const { return myMap; }

protected:

  
  //! Translates STEP file already loaded into the reader
  //! into the document
  //! If num==0, translates all roots, else only root number num
  //! Returns True if succeeded, and False in case of fail
  //! If asOne is True, in case of multiple results composes
  //! them into assembly. Fills sequence of produced labels
  Standard_EXPORT Standard_Boolean Transfer (STEPControl_Reader& rd,
                                             const Standard_Integer num,
                                             const Handle(TDocStd_Document)& doc,
                                             TDF_LabelSequence& Lseq,
                                             const Standard_Boolean asOne = Standard_False,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Add a shape to a document
  //! Depending on a case, this shape can be added as one, or
  //! as assembly, or (in case if it is associated with external
  //! reference) taken as that referred shape
  Standard_EXPORT TDF_Label AddShape (const TopoDS_Shape& S, const Handle(XCAFDoc_ShapeTool)& STool, const TopTools_MapOfShape& NewShapesMap, const STEPCAFControl_DataMapOfShapePD& ShapePDMap, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap);
  
  //! Reads (or if returns already read) extern file with
  //! given name
  Standard_EXPORT Handle(STEPCAFControl_ExternFile) ReadExternFile (const Standard_CString file,
                                                                    const Standard_CString fullpath,
                                                                    const Handle(TDocStd_Document)& doc,
                                   const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Reads style assignments from STEP model and sets
  //! corresponding color assignments in the DECAF document
  Standard_EXPORT Standard_Boolean ReadColors
                (const Handle(XSControl_WorkSession)& WS,
                 const Handle(TDocStd_Document)& doc,
                 const StepData_Factors& theLocalFactors) const;
  
  //! Reads names of parts defined in the STEP model and
  //! assigns them to corresponding labels in the DECAF document
  Standard_EXPORT Standard_Boolean ReadNames (const Handle(XSControl_WorkSession)& WS, const Handle(TDocStd_Document)& doc, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap) const;
  
  //! Reads validation properties assigned to shapes in the STEP
  //! model and assigns them to corresponding labels in the DECAF
  //! document
  Standard_EXPORT Standard_Boolean ReadValProps (const Handle(XSControl_WorkSession)& WS,
                                                 const Handle(TDocStd_Document)& doc,
                                                 const STEPCAFControl_DataMapOfPDExternFile& PDFileMap,
                                                 const StepData_Factors& theLocalFactors) const;
  
  //! Reads layers of parts defined in the STEP model and
  //! set reference between shape and layers in the DECAF document
  Standard_EXPORT Standard_Boolean ReadLayers (const Handle(XSControl_WorkSession)& WS, const Handle(TDocStd_Document)& doc) const;
  
  //! Reads SHUO for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT Standard_Boolean ReadSHUOs (const Handle(XSControl_WorkSession)& WS, const Handle(TDocStd_Document)& doc, const STEPCAFControl_DataMapOfPDExternFile& PDFileMap) const;
  
  //! Reads D&GT for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT Standard_Boolean ReadGDTs (const Handle(XSControl_WorkSession)& WS,
                                             const Handle(TDocStd_Document)& doc,
                                             const StepData_Factors& theLocalFactors);
  
  //! Reads materials for instances defined in the STEP model and
  //! set reference between shape instances from different assemblyes
  Standard_EXPORT Standard_Boolean ReadMaterials (const Handle(XSControl_WorkSession)& WS,
                                                  const Handle(TDocStd_Document)& doc,
                                                  const Handle(TColStd_HSequenceOfTransient)& SeqPDS,
                                                  const StepData_Factors& theLocalFactors) const;
  
  //! Reads Views for instances defined in the STEP model
  Standard_EXPORT Standard_Boolean ReadViews(const Handle(XSControl_WorkSession)& theWS,
                                             const Handle(TDocStd_Document)& theDoc,
                                             const StepData_Factors& theLocalFactors) const;

  //! Populates the sub-Label of the passed TDF Label with shape
  //! data associated with the given STEP Representation Item,
  //! including naming and topological information.
  Standard_EXPORT TDF_Label SettleShapeData (const Handle(StepRepr_RepresentationItem)& theItem, const TDF_Label& theLab, const Handle(XCAFDoc_ShapeTool)& theShapeTool, const Handle(Transfer_TransientProcess)& theTP) const;
  
  //! Given the maps of already translated shapes, this method
  //! expands their correspondent Labels in XDE Document so that
  //! to have a dedicated sub-Label for each sub-shape coming
  //! with associated name in its STEP Representation Item.
  Standard_EXPORT void ExpandSubShapes (const Handle(XCAFDoc_ShapeTool)& theShapeTool, const STEPCAFControl_DataMapOfShapePD& theShapePDMap) const;
  
  //! Expands the topological structure of Manifold Solid BRep
  //! STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandManifoldSolidBrep (TDF_Label& theLab, const Handle(StepRepr_RepresentationItem)& theItem, const Handle(Transfer_TransientProcess)& theTP, const Handle(XCAFDoc_ShapeTool)& theShapeTool) const;
  
  //! Expands the topological structure of Shell-Based Surface
  //! Model STEP entity to OCAF sub-tree. Entities having no names
  //! associated via their Representation Items are skipped.
  Standard_EXPORT void ExpandSBSM (TDF_Label& theLab, const Handle(StepRepr_RepresentationItem)& theItem, const Handle(Transfer_TransientProcess)& theTP, const Handle(XCAFDoc_ShapeTool)& theShapeTool) const;
  
  //! Expands STEP Shell structure to OCAF sub-tree. Entities
  //! having no names associated via their Representation Items
  //! are skipped.
  Standard_EXPORT void ExpandShell (const Handle(StepShape_ConnectedFaceSet)& theShell, TDF_Label& theLab, const Handle(Transfer_TransientProcess)& theTP, const Handle(XCAFDoc_ShapeTool)& theShapeTool) const;

  //! Convert name into UNICODE text.
  Standard_EXPORT virtual TCollection_ExtendedString convertName (const TCollection_AsciiString& theName) const;

private:

  //! Internal method. Import all Datum attributes and set them to XCAF object. Set connection of Datum to GeomTolerance (theGDTL).
  Standard_Boolean setDatumToXCAF(const Handle(StepDimTol_Datum)& theDat,
    const TDF_Label theGDTL,
    const Standard_Integer thePositionCounter,
    const XCAFDimTolObjects_DatumModifiersSequence& theXCAFModifiers,
    const XCAFDimTolObjects_DatumModifWithValue theXCAFModifWithVal,
    const Standard_Real theModifValue,
    const Handle(TDocStd_Document)& theDoc,
    const Handle(XSControl_WorkSession)& theWS,
    const StepData_Factors& theLocalFactors);
  
  //! Internal method. Read Datums, connected to GeomTolerance theGDTL.
  Standard_Boolean readDatumsAP242(const Handle(Standard_Transient)& theEnt,
    const TDF_Label theGDTL,
    const Handle(TDocStd_Document)& theDoc,
    const Handle(XSControl_WorkSession)& theWS,
    const StepData_Factors& theLocalFactors);

  //! Internal method. Read Dimension or GeomTolerance.
  TDF_Label createGDTObjectInXCAF(const Handle(Standard_Transient)& theEnt,
    const Handle(TDocStd_Document)& theDoc,
    const Handle(XSControl_WorkSession)& theWS,
    const StepData_Factors& theLocalFactors);

  //! Prepares units for transfer
  void prepareUnits(const Handle(StepData_StepModel)& theModel,
                    const Handle(TDocStd_Document)& theDoc,
                    StepData_Factors& theLocalFactors) const;

private:

  STEPControl_Reader myReader;
  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> myFiles;
  XCAFDoc_DataMapOfShapeLabel myMap;
  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;
  Standard_Boolean myPropsMode;
  Standard_Boolean mySHUOMode;
  Standard_Boolean myGDTMode;
  Standard_Boolean myMatMode;
  Standard_Boolean myViewMode;
  NCollection_DataMap<Handle(Standard_Transient), TDF_Label> myGDTMap;

};

#endif // _STEPCAFControl_Reader_HeaderFile
