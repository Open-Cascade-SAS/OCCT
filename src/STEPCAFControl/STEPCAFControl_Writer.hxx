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

#include <IFSelect_ReturnStatus.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <STEPCAFControl_DataMapOfLabelShape.hxx>
#include <STEPCAFControl_DataMapOfLabelExternFile.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <StepAP242_GeometricItemSpecificUsage.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_HArray1OfDatumSystemOrReference.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepVisual_DraughtingModel.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>

class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;
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
  
  
  //! Creates a writer with an empty
  //! STEP model and sets ColorMode, LayerMode, NameMode and
  //! PropsMode to Standard_True.
  Standard_EXPORT STEPCAFControl_Writer();
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for STEP
  //! Clears the internal data structures
  Standard_EXPORT STEPCAFControl_Writer(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Clears the internal data structures and attaches to a new session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT void Init (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Writes all the produced models into file
  //! In case of multimodel with extern references,
  //! filename will be a name of root file, all other files
  //! have names of corresponding parts
  //! Provided for use like single-file writer
  Standard_EXPORT IFSelect_ReturnStatus Write (const Standard_CString filename);
  
  //! Transfers a document (or single label) to a STEP model
  //! The mode of translation of shape is AsIs
  //! If multi is not null pointer, it switches to multifile
  //! mode (with external refs), and string pointed by <multi>
  //! gives prefix for names of extern files (can be empty string)
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Transfer (const Handle(TDocStd_Document)& doc,
                                             const STEPControl_StepModelType mode = STEPControl_AsIs,
                                             const Standard_CString multi = 0,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Method to transfer part of the document specified by label
  Standard_EXPORT Standard_Boolean Transfer (const TDF_Label& L,
                                             const STEPControl_StepModelType mode = STEPControl_AsIs,
                                             const Standard_CString multi = 0,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());

  Standard_EXPORT Standard_Boolean Perform (const Handle(TDocStd_Document)& doc,
                                            const TCollection_AsciiString& filename,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers a document and writes it to a STEP file
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Perform (const Handle(TDocStd_Document)& doc,
                                            const Standard_CString filename,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Returns data on external files
  //! Returns Null handle if no external files are read
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>& ExternFiles() const;
  
  //! Returns data on external file by its original label
  //! Returns False if no external file with given name is read
  Standard_EXPORT Standard_Boolean ExternFile (const TDF_Label& L, Handle(STEPCAFControl_ExternFile)& ef) const;
  
  //! Returns data on external file by its name
  //! Returns False if no external file with given name is read
  Standard_EXPORT Standard_Boolean ExternFile (const Standard_CString name, Handle(STEPCAFControl_ExternFile)& ef) const;
  
  //! Returns basic reader for root file
  Standard_EXPORT STEPControl_Writer& ChangeWriter();
  
  //! Returns basic reader as const
  Standard_EXPORT const STEPControl_Writer& Writer() const;
  
  //! Set ColorMode for indicate write Colors or not.
  Standard_EXPORT void SetColorMode (const Standard_Boolean colormode);
  
  Standard_EXPORT Standard_Boolean GetColorMode() const;
  
  //! Set NameMode for indicate write Name or not.
  Standard_EXPORT void SetNameMode (const Standard_Boolean namemode);
  
  Standard_EXPORT Standard_Boolean GetNameMode() const;
  
  //! Set LayerMode for indicate write Layers or not.
  Standard_EXPORT void SetLayerMode (const Standard_Boolean layermode);
  
  Standard_EXPORT Standard_Boolean GetLayerMode() const;
  
  //! PropsMode for indicate write Validation properties or not.
  Standard_EXPORT void SetPropsMode (const Standard_Boolean propsmode);
  
  Standard_EXPORT Standard_Boolean GetPropsMode() const;
  
  //! Set SHUO mode for indicate write SHUO or not.
  Standard_EXPORT void SetSHUOMode (const Standard_Boolean shuomode);
  
  Standard_EXPORT Standard_Boolean GetSHUOMode() const;
  
  //! Set dimtolmode for indicate write D&GTs or not.
  Standard_EXPORT void SetDimTolMode (const Standard_Boolean dimtolmode);
  
  Standard_EXPORT Standard_Boolean GetDimTolMode() const;
  
  //! Set dimtolmode for indicate write D&GTs or not.
  Standard_EXPORT void SetMaterialMode (const Standard_Boolean matmode);
  
  Standard_EXPORT Standard_Boolean GetMaterialMode() const;




protected:
  //! Mehod to writing sequence of root assemblies or part of the file specified by use by one label 
  Standard_EXPORT Standard_Boolean Transfer (const TDF_LabelSequence& L,
                                             const STEPControl_StepModelType mode = STEPControl_AsIs,
                                             const Standard_CString multi = 0,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Transfers labels to a STEP model
  //! Returns True if translation is OK
  //! isExternFile setting from TransferExternFiles method
  Standard_EXPORT Standard_Boolean Transfer (STEPControl_Writer& wr,
                                             const TDF_LabelSequence& labels,
                                             const STEPControl_StepModelType mode = STEPControl_AsIs,
                                             const Standard_CString multi = 0,
                                             const Standard_Boolean isExternFile = Standard_False,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange()) ;
  
  //! Parses assembly structure of label L, writes all the simple
  //! shapes each to its own file named by name of its label plus
  //! prefix
  //! Returns shape representing that assembly structure
  //! in the form of nested empty compounds (and a sequence of
  //! labels which are newly written nodes of this assembly)
  Standard_EXPORT TopoDS_Shape TransferExternFiles (const TDF_Label& L,
                                                    const STEPControl_StepModelType mode,
                                                    TDF_LabelSequence& Lseq,
                                                    const Standard_CString prefix = "",
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Write external references to STEP
  Standard_EXPORT Standard_Boolean WriteExternRefs (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels) const;
  
  //! Write colors assigned to specified labels, to STEP model
  Standard_EXPORT Standard_Boolean WriteColors (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels);
  
  //! Write names assigned to specified labels, to STEP model
  Standard_EXPORT Standard_Boolean WriteNames (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels) const;
  
  //! Write D&GTs assigned to specified labels, to STEP model
  Standard_EXPORT Standard_Boolean WriteDGTs (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels) const;
  
   //! Write D&GTs assigned to specified labels, to STEP model, according AP242
  Standard_EXPORT Standard_Boolean WriteDGTsAP242 (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels);

  //! Write materials assigned to specified labels, to STEP model
  Standard_EXPORT Standard_Boolean WriteMaterials (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels) const;
  
  //! Write validation properties assigned to specified labels,
  //! to STEP model
  Standard_EXPORT Standard_Boolean WriteValProps (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels, const Standard_CString multi) const;
  
  //! Write layers assigned to specified labels, to STEP model
  Standard_EXPORT Standard_Boolean WriteLayers (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels) const;
  
  //! Write SHUO assigned to specified component, to STEP model
  Standard_EXPORT Standard_Boolean WriteSHUOs (const Handle(XSControl_WorkSession)& WS, const TDF_LabelSequence& labels);

  

private:
  Standard_EXPORT Handle(StepRepr_ShapeAspect) WriteShapeAspect(const Handle(XSControl_WorkSession) &WS,
    const TDF_Label theLabel, const TopoDS_Shape theShape, Handle(StepRepr_RepresentationContext)& theRC,
    Handle(StepAP242_GeometricItemSpecificUsage)& theGISU);

  Standard_EXPORT void WritePresentation(const Handle(XSControl_WorkSession) &WS, const TopoDS_Shape thePresentation,
    const Handle(TCollection_HAsciiString)& thePrsName, const Standard_Boolean hasSemantic, const Standard_Boolean hasPlane,
    const gp_Ax2 theAnnotationPlane, const gp_Pnt theTextPosition, const Handle(Standard_Transient) theDimension);

  Standard_EXPORT Handle(StepDimTol_Datum) WriteDatumAP242(const Handle(XSControl_WorkSession) &WS, const TDF_LabelSequence theShapeL,
    const TDF_Label theDatumL, const Standard_Boolean isFirstDTarget, const Handle(StepDimTol_Datum) theWrittenDatum);

  Standard_EXPORT void WriteToleranceZone(const Handle(XSControl_WorkSession) &WS, const Handle(XCAFDimTolObjects_GeomToleranceObject)& theObject,
    const Handle(StepDimTol_GeometricTolerance)& theEntity, const Handle(StepRepr_RepresentationContext)& theRC);

  Standard_EXPORT void WriteGeomTolerance(const Handle(XSControl_WorkSession) &WS, const TDF_LabelSequence theShapeSeqL,
    const TDF_Label theGeomTolL, const Handle(StepDimTol_HArray1OfDatumSystemOrReference)& theDatumSystem,
    const Handle(StepRepr_RepresentationContext)& theRC);



  STEPControl_Writer myWriter;
  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> myFiles;
  STEPCAFControl_DataMapOfLabelShape myLabels;
  STEPCAFControl_DataMapOfLabelExternFile myLabEF;
  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;
  Standard_Boolean myPropsMode;
  Standard_Boolean mySHUOMode;
  MoniTool_DataMapOfShapeTransient myMapCompMDGPR;
  Standard_Boolean myGDTMode;
  Standard_Boolean myMatMode;
  NCollection_Vector<Handle(StepRepr_RepresentationItem)> myGDTAnnotations;
  Handle(StepVisual_DraughtingModel) myGDTPresentationDM;
  Handle(StepVisual_HArray1OfPresentationStyleAssignment) myGDTPrsCurveStyle;
  Handle(StepRepr_ProductDefinitionShape) myGDTCommonPDS;

};




#endif // _STEPCAFControl_Writer_HeaderFile
