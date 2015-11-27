// Created on: 1997-12-18
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _PCDM_Reader_HeaderFile
#define _PCDM_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PCDM_ReaderStatus.hxx>
#include <Standard_Transient.hxx>
#include <Standard_IStream.hxx>
#include <Storage_Data.hxx>

class PCDM_DriverError;
class CDM_Document;
class TCollection_ExtendedString;
class CDM_Application;


class PCDM_Reader;
DEFINE_STANDARD_HANDLE(PCDM_Reader, Standard_Transient)


class PCDM_Reader : public Standard_Transient
{

public:

  
  //! this method is called by the framework before the read method.
  Standard_EXPORT virtual Handle(CDM_Document) CreateDocument() = 0;
  
  //! retrieves the content of the file into a new Document.
  Standard_EXPORT virtual void Read (const TCollection_ExtendedString& aFileName, const Handle(CDM_Document)& aNewDocument, const Handle(CDM_Application)& anApplication) = 0;

  Standard_EXPORT virtual void Read (Standard_IStream&               theIStream,
                                      const Handle(Storage_Data)&    theStorageData,
                                      const Handle(CDM_Document)&    theDoc,
                                      const Handle(CDM_Application)& theApplication) = 0;
  
    PCDM_ReaderStatus GetStatus() const;




  DEFINE_STANDARD_RTTIEXT(PCDM_Reader,Standard_Transient)

protected:


  PCDM_ReaderStatus myReaderStatus;


private:




};


#include <PCDM_Reader.lxx>





#endif // _PCDM_Reader_HeaderFile
