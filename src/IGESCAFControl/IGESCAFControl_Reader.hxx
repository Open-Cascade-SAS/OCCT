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

#include <IGESControl_Reader.hxx>
#include <TCollection_AsciiString.hxx>
class XSControl_WorkSession;
class TDocStd_Document;


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
  IGESCAFControl_Reader()
  : myColorMode( Standard_True ),
    myNameMode ( Standard_True ),
    myLayerMode( Standard_True )
  {}
  
  //! Creates a reader tool and attaches it to an already existing Session
  //! Clears the session if it was not yet set for IGES
  IGESCAFControl_Reader(const Handle(XSControl_WorkSession)& theWS, const Standard_Boolean FromScratch = Standard_True)
  : myColorMode( Standard_True ),
    myNameMode ( Standard_True ),
    myLayerMode( Standard_True )
  { SetWS (theWS,FromScratch); }
  
  //! Translates currently loaded IGES file into the document
  //! Returns True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Transfer (const Handle(TDocStd_Document)& theDoc,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  Standard_Boolean Perform (const TCollection_AsciiString& theFileName,
                            const Handle(TDocStd_Document)& theDoc,
                            const Message_ProgressRange& theProgress = Message_ProgressRange())
  { return Perform (theFileName.ToCString(), theDoc, theProgress); }
  
  //! Translate IGES file given by filename into the document
  //! Return True if succeeded, and False in case of fail
  Standard_EXPORT Standard_Boolean Perform (const Standard_CString theFileName,
                                            const Handle(TDocStd_Document)& theDoc,
                                            const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Set ColorMode for indicate read Colors or not.
  void SetColorMode (const Standard_Boolean theMode)
  { myColorMode = theMode; }

  Standard_Boolean GetColorMode() const
  { return myColorMode; }
  
  //! Set NameMode for indicate read Name or not.
  void SetNameMode (const Standard_Boolean theMode)
  { myNameMode = theMode; }

  Standard_Boolean GetNameMode() const
  { return myNameMode; }
  
  //! Set LayerMode for indicate read Layers or not.
  void SetLayerMode (const Standard_Boolean theMode)
  { myLayerMode = theMode; }

  Standard_Boolean GetLayerMode() const
  { return myLayerMode; }

 private:

  Standard_Boolean myColorMode;
  Standard_Boolean myNameMode;
  Standard_Boolean myLayerMode;
};

#endif // _IGESCAFControl_Reader_HeaderFile
