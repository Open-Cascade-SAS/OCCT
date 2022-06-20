// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _BinLDrivers_DocumentSection_HeaderFile
#define _BinLDrivers_DocumentSection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Message_ProgressIndicator.hxx>
#include <TDocStd_FormatVersion.hxx>



//! More or less independent part of the saved/restored document
//! that is distinct from OCAF data themselves but may be referred
//! by them.
class BinLDrivers_DocumentSection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BinLDrivers_DocumentSection();
  
  //! Constructor
  Standard_EXPORT BinLDrivers_DocumentSection(const TCollection_AsciiString& theName, const Standard_Boolean isPostRead);
  
  //! Query the name of the section.
  Standard_EXPORT const TCollection_AsciiString& Name() const;
  
  //! Query the status: if the Section should be read after OCAF;
  //! False means that the Section is read before starting to
  //! read OCAF data.
  Standard_EXPORT Standard_Boolean IsPostRead() const;
  
  //! Query the offset of the section in the persistent file
  Standard_EXPORT uint64_t Offset() const;
  
  //! Set the offset of the section in the persistent file
  Standard_EXPORT void SetOffset (const uint64_t theOffset);
  
  //! Query the length of the section in the persistent file
  Standard_EXPORT uint64_t Length() const;
  
  //! Set the length of the section in the persistent file
  Standard_EXPORT void SetLength (const uint64_t theLength);
  
  //! Create a Section entry in the Document TOC (list of sections)
  Standard_EXPORT void WriteTOC (Standard_OStream& theOS,
                                 const TDocStd_FormatVersion theDocFormatVersion);
  
  //! Save Offset and Length data into the Section entry
  //! in the Document TOC (list of sections)
  Standard_EXPORT void Write (Standard_OStream& theOS, const uint64_t theOffset,
                              const TDocStd_FormatVersion theDocFormatVersion);
  
  //! Fill a DocumentSection instance from the data that are read
  //! from TOC. Returns false in case of the stream reading problem.
  Standard_EXPORT static Standard_Boolean ReadTOC (BinLDrivers_DocumentSection& theSection,
                                                   Standard_IStream& theIS,
                                                   const TDocStd_FormatVersion theDocFormatVersion);




protected:





private:



  TCollection_AsciiString myName;
  uint64_t myValue[2];
  Standard_Boolean myIsPostRead;


};







#endif // _BinLDrivers_DocumentSection_HeaderFile
