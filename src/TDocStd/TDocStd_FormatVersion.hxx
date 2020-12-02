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

//! Reading of new documents by old applications.
//! Open CASCADE Technology evolves and file format of OCAF document evolves too. Does it mean that an application based on an old version of Open CASCADE Technology
//! cannot read an OCAF document saved by an application using a newer version of Open CASCADE Technology?
//! Let us consider this question in detail and see when it is possible.
//! Storage version of OCAF document is stored in TDocStd_Document class and accessible via StorageFormatVersion() method.
//! In order to change it, call the method ChangeStorageFormatVersion(). In order to save a document in older version
//! (corresponding to an earlier version of Open CASCADE Technology),
//! change the storage version (by means of TDocStd_Document::ChangeStorageFormatVersion()) and save the document(via TDocStd_Application::SaveAs()).
//! It will be stored using the rules of an old version of Open CASCADE Technology.
//! What to do if you need to improve the file format of an OCAF document, and in the same time,
//! you would like the current application already sent to the users to read the new documents?
//! Here it is an example of a similar case, which might give you an idea how to implement it.
//! Example.
//! In order to improve speed of writing and reading of OCAF document in XML file format a developer decided to perform the following improvements in OCAF:
//!  - Removal of GUID of a TDataStd_TreeNode attribute, if it is default. Indeed, it seems reasonable not to write a GUID into XML file if it is default.
//!    It derceases size of file and speeds-up writing and reading of the file. The same approach concerns several other attributes with default GUIDs.
//!  - Compression of TDataStd_ExtStringArray attribute. It uses a new line for each value of the array abbreviated by XML tags.
//!    If all values were put into one line separated by a comma or any other separator, it would decrease the file size, speed-up reading and writing.
//! It is obvious that an old application, which does not know about an opportunity for a TDataStd_TreeNode attribute to have no GUID in XML file,
//! may fail to read such an improved XML file. It means that we should 'teach' Open CASCADE Technology of later versions to write document in an old format. How to do it?
//!  - First, we increase the storage format version of the document (in this TDocStd_FormatVerison.hxx file).
//!  - Then, we introduce a code in XmlMDataStd_TreeNodeDriver::Paste(), which checks the document storage version and if it is one of the previous versions,
//!    it writes the GUID in any case (in spite of it is default).
//!  - For a TDataStd_ExtStringArray we do the same : we check the document storage version and if it is one of the previous versions,
//!    we write values of the array in old manner : one per line surrounding each value by XML tags.
//!    For details, please see XmlMDataStd_ExtStringArrayDriver::Paste().
//! This way we obtain an opportunity to save OCAF document in an old file format, while for new applications we keep an improvement
//! (smaller files on disk, reading and writing of the XML files is faster).
enum TDocStd_FormatVersion
{
  TDocStd_FormatVersion_VERSION_2 = 2, //!< First supported version

  TDocStd_FormatVersion_VERSION_3,     //!< (OCCT 6.3.0) 
                                       //!<   XML: Adding DeltaOnModification functionality to set of Standard attributes [#0019403]
                                       //!<   BIN: Add Delta to numbers data, changes in ShapeSection [#0019986, #0019403]

  TDocStd_FormatVersion_VERSION_4,     //!< (OCCT 6.3.1) 
                                       //!<   XML: Naming mechanism improvement [#0021004]
                                       //!<   BIN: entry, ContextLabel for tree [#0021004]

  TDocStd_FormatVersion_VERSION_5,     //!< (OCCT 6.3.1) 
                                       //!<   XML: Separation of OCAF to Lite and Standard parts completion [#0021093]
                                       //!<   BIN: Convert old format to new [#0021093]

  TDocStd_FormatVersion_VERSION_6,     //!< (OCCT 6.5.0)  
                                       //!<   XML: Add location [#0022192]
                                       //!<   BIN: Add location [#0022192]

  TDocStd_FormatVersion_VERSION_7,     //!< (OCCT 6.7.0)  
                                       //!<   XML: Add orientation [#0023766]
                                       //!<   BIN: Add orientation, type migration [#0023766]

  TDocStd_FormatVersion_VERSION_8,     //!< (OCCT 7.0.0)  
                                       //!<   XML: Replace TPrsStd_AISPresentation attribute with TDataXtd_Presentation [#0026290]
                                       //!<   BIN: Stop convert old format [#0026290]

  TDocStd_FormatVersion_VERSION_9,     //!< (OCCT 7.1.0) 
                                       //!<   BIN: Add GUIDs, Process user defined guid [#0027932]

  TDocStd_FormatVersion_VERSION_10,    //!< (OCCT 7.2.0) 
                                       //!<   BIN: ReadTOC changed to handle 64-bit file length [#0028736]

  TDocStd_FormatVersion_CURRENT = TDocStd_FormatVersion_VERSION_10 //!< The latest version. 
};

#endif // _TDocStdFormatVersion_HeaderFile
