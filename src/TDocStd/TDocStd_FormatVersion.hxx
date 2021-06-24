// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _TDocStdFormatVersion_HeaderFile
#define _TDocStdFormatVersion_HeaderFile

//! Storage format versions of OCAF documents in XML and binary file formats.
//!
//! OCAF document file format evolves and a new version number indicates each improvement of the format.
//! This enumeration lists all versions of an OCAF document. TDocStd_FormatVersion_CURRENT value refers to the last file format version.
//! By default, Open CASCADE Technology writes new documents using the last file format version.
//! The last version of Open CASCADE Technology is able to read old documents of any version.
//! However, a previous version of Open CASCADE Technology may not be able to read a new document.
//! In this case use the method ChangeStorageFormatVersion() from TDocStd_Document to change the file format version.
//! Then, save the document by means of SaveAs() from TDocStd_Application.
//!
//! If it is necessary to improve an XML or binary file format of OCAF document, follow please the next steps:
//! - increment the file format version in this enumeration. Put a reference to the last file format version by means of TDocStd_FormatVersion_CURRENT.
//! - introduce the improvement in OCAF attribute storage and retrieval drivers, if necessary.
//!   As an example, please consider the file XmlMDataStd_TreeNodeDriver.cxx.
//! - test the improvement on current file format version and on the previous one.
enum TDocStd_FormatVersion
{
  TDocStd_FormatVersion_VERSION_2 = 2, //!< First supported version

  TDocStd_FormatVersion_VERSION_3,     //!< OCCT 6.3.0
                                       //!< * XML: Adding DeltaOnModification functionality to set of Standard attributes [#0019403]
                                       //!< * BIN: Add Delta to numbers data, changes in ShapeSection [#0019986, #0019403]

  TDocStd_FormatVersion_VERSION_4,     //!< OCCT 6.3.1
                                       //!< * XML: Naming mechanism improvement [#0021004]
                                       //!< * BIN: entry, ContextLabel for tree [#0021004]

  TDocStd_FormatVersion_VERSION_5,     //!< OCCT 6.3.1
                                       //!< * XML: Separation of OCAF to Lite and Standard parts completion [#0021093]
                                       //!< * BIN: Convert old format to new [#0021093]

  TDocStd_FormatVersion_VERSION_6,     //!< OCCT 6.5.0
                                       //!< * XML: Add location [#0022192]
                                       //!< * BIN: Add location [#0022192]

  TDocStd_FormatVersion_VERSION_7,     //!< OCCT 6.7.0
                                       //!< * XML: Add orientation [#0023766]
                                       //!< * BIN: Add orientation, type migration [#0023766]

  TDocStd_FormatVersion_VERSION_8,     //!< OCCT 7.0.0
                                       //!< * XML: Replace TPrsStd_AISPresentation attribute with TDataXtd_Presentation [#0026290]
                                       //!< * BIN: Stop convert old format [#0026290]

  TDocStd_FormatVersion_VERSION_9,     //!< OCCT 7.1.0
                                       //!< * BIN: Add GUIDs, Process user defined guid [#0027932]

  TDocStd_FormatVersion_VERSION_10,    //!< OCCT 7.2.0
                                       //!< * BIN: ReadTOC changed to handle 64-bit file length [#0028736]

  TDocStd_FormatVersion_VERSION_11,    //!< OCCT 7.6.0
                                       //!< * BIN, XML: TopTools_FormatVersion_CURRENT changed to 3 and 
                                       //!< BinTools_FormatVersion_CURRENT changed to 4 to preserve per-vertex normal 
                                       //!< information in case of triangulation-only Faces [#0031136]
  TDocStd_FormatVersion_VERSION_12,    //!< OCCT 7.6.0
                                       //!< * BIN: New binary format for fast reading of part of OCAF document [#0031918]

  TDocStd_FormatVersion_CURRENT = TDocStd_FormatVersion_VERSION_12 //!< Current version
};

enum
{
  TDocStd_FormatVersion_LOWER   = TDocStd_FormatVersion_VERSION_2,
  TDocStd_FormatVersion_UPPER   = TDocStd_FormatVersion_VERSION_12
};


#endif // _TDocStdFormatVersion_HeaderFile
