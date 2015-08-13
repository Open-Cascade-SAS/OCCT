// Created on: 2000-08-17
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

#ifndef _IGESCAFControl_Writer_HeaderFile
#define _IGESCAFControl_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IGESControl_Writer.hxx>
#include <Standard_CString.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFPrs_DataMapOfShapeStyle.hxx>
#include <XCAFPrs_DataMapOfStyleTransient.hxx>
#include <TopTools_MapOfShape.hxx>
class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;
class TopoDS_Shape;
class XCAFPrs_Style;

//! Provides a tool to write DECAF document to the
//! IGES file. Besides transfer of shapes (including
//! assemblies) provided by IGESControl, supports also
//! colors and part names
//! IGESCAFControl_Writer writer();
//! Methods for writing IGES file:
//! writer.Transfer (Document);
//! writer.Write("filename") or writer.Write(OStream)  or
//! writer.Perform(Document,"filename");
//! Methods for managing the writing of attributes.
//! Colors
//! writer.SetColorMode(colormode);
//! Standard_Boolean colormode = writer.GetColorMode();
//! Layers
//! writer.SetLayerMode(layermode);
//! Standard_Boolean layermode = writer.GetLayerMode();
//! Names
//! writer.SetNameMode(namemode);
//! Standard_Boolean namemode = writer.GetNameMode();
class IGESCAFControl_Writer  : public IGESControl_Writer
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a writer with an empty
  //! IGES model and sets ColorMode, LayerMode and NameMode to Standard_True.
  Standard_EXPORT IGESCAFControl_Writer();
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for IGES
  Standard_EXPORT IGESCAFControl_Writer(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Transfers a document to a IGES model
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Transfer (const Handle(TDocStd_Document)& doc);

  //! Transfers labels to a IGES model
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Transfer (const TDF_LabelSequence& labels);

  //! Transfers label to a IGES model
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Transfer (const TDF_Label& label);

  Standard_EXPORT Standard_Boolean Perform (const Handle(TDocStd_Document)& doc, const TCollection_AsciiString& filename);
  
  //! Transfers a document and writes it to a IGES file
  //! Returns True if translation is OK
  Standard_EXPORT Standard_Boolean Perform (const Handle(TDocStd_Document)& doc, const Standard_CString filename);
  
  //! Set ColorMode for indicate write Colors or not.
  Standard_EXPORT void SetColorMode (const Standard_Boolean colormode);
  
  Standard_EXPORT Standard_Boolean GetColorMode() const;
  
  //! Set NameMode for indicate write Name or not.
  Standard_EXPORT void SetNameMode (const Standard_Boolean namemode);
  
  Standard_EXPORT Standard_Boolean GetNameMode() const;
  
  //! Set LayerMode for indicate write Layers or not.
  Standard_EXPORT void SetLayerMode (const Standard_Boolean layermode);
  
  Standard_EXPORT Standard_Boolean GetLayerMode() const;




protected:
 
  //! Reads colors from DECAF document and assigns them
  //! to corresponding IGES entities
  Standard_EXPORT Standard_Boolean WriteAttributes (const TDF_LabelSequence& labels);
  
  //! Reads layers from DECAF document and assigns them
  //! to corresponding IGES entities
  Standard_EXPORT Standard_Boolean WriteLayers (const TDF_LabelSequence& labels);
  
  //! Recursivile iterates on subshapes and assign names
  //! to IGES entity
  Standard_EXPORT Standard_Boolean WriteNames (const TDF_LabelSequence& labels);




private:

  
  //! Recursively iterates on subshapes and assigns colors
  //! to faces and edges (if set)
  Standard_EXPORT void MakeColors (const TopoDS_Shape& S, const XCAFPrs_DataMapOfShapeStyle& settings, XCAFPrs_DataMapOfStyleTransient& colors, TopTools_MapOfShape& Map, const XCAFPrs_Style& inherit);


  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;


};







#endif // _IGESCAFControl_Writer_HeaderFile
