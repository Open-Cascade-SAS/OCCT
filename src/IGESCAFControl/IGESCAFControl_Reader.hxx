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

#ifndef _IGESCAFControl_Reader_HeaderFile
#define _IGESCAFControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IGESControl_Reader.hxx>
#include <Standard_CString.hxx>
class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;


//! Provides a tool to read IGES file and put it into
//! DECAF document. Besides transfer of shapes (including
//! assemblies) provided by IGESControl, supports also
//! colors and part names
//! IGESCAFControl_Reader reader; Methods for translation of an IGES file:
//! reader.ReadFile("filename");
//! reader.Transfer(Document); or
//! reader.Perform("filename",doc);
//! Methods for managing reading attributes.
//! Colors
//! reader.SetColorMode(colormode);
//! Standard_Boolean colormode = reader.GetColorMode();
//! Layers
//! reader.SetLayerMode(layermode);
//! Standard_Boolean layermode = reader.GetLayerMode();
//! Names
//! reader.SetNameMode(namemode);
//! Standard_Boolean namemode = reader.GetNameMode();
class IGESCAFControl_Reader  : public IGESControl_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a reader with an empty
  //! IGES model and sets ColorMode, LayerMode and NameMode to Standard_True.
  Standard_EXPORT IGESCAFControl_Reader();
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for IGES
  Standard_EXPORT IGESCAFControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Translates currently loaded IGES file into the document
  //! Returns True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Transfer (Handle(TDocStd_Document)& doc);
  
  Standard_EXPORT Standard_Boolean Perform (const TCollection_AsciiString& filename, Handle(TDocStd_Document)& doc);
  
  //! Translate IGES file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Perform (const Standard_CString filename, Handle(TDocStd_Document)& doc);
  
  //! Set ColorMode for indicate read Colors or not.
  Standard_EXPORT void SetColorMode (const Standard_Boolean colormode);
  
  Standard_EXPORT Standard_Boolean GetColorMode() const;
  
  //! Set NameMode for indicate read Name or not.
  Standard_EXPORT void SetNameMode (const Standard_Boolean namemode);
  
  Standard_EXPORT Standard_Boolean GetNameMode() const;
  
  //! Set LayerMode for indicate read Layers or not.
  Standard_EXPORT void SetLayerMode (const Standard_Boolean layermode);
  
  Standard_EXPORT Standard_Boolean GetLayerMode() const;




protected:

  
  //! Reads colors of IGES entities and sets
  //! corresponding color assignments in the DECAF document
  Standard_EXPORT Standard_Boolean ReadColors (Handle(TDocStd_Document)& doc) const;
  
  //! Reads Names of IGES entities and sets
  //! corresponding name to label with shape in the DECAF document
  Standard_EXPORT Standard_Boolean ReadNames (Handle(TDocStd_Document)& doc) const;
  
  //! Reads layers of parts defined in the IGES model and
  //! set reference between shape and layers in the DECAF document
  Standard_EXPORT Standard_Boolean ReadLayers (Handle(TDocStd_Document)& doc) const;




private:



  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;


};







#endif // _IGESCAFControl_Reader_HeaderFile
