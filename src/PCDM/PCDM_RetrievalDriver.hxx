// Created on: 1997-08-07
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

#ifndef _PCDM_RetrievalDriver_HeaderFile
#define _PCDM_RetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <PCDM_Reader.hxx>
#include <PCDM_ReferenceIterator.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <PCDM_SequenceOfReference.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class PCDM_DriverError;
class CDM_MetaData;
class TCollection_ExtendedString;
class CDM_Document;
class CDM_Application;
class PCDM_Document;
class Storage_Schema;
class CDM_MessageDriver;
class TCollection_AsciiString;


class PCDM_RetrievalDriver;
DEFINE_STANDARD_HANDLE(PCDM_RetrievalDriver, PCDM_Reader)


class PCDM_RetrievalDriver : public PCDM_Reader
{

public:

  
  //! Warning -  raises DriverError if an error occurs during inside the
  //! Make method.
  //! retrieves the content of the file into a new Document.
  //!
  //! by  default  Read will  use the Schema method to read the file
  //! into a persistent document. and the Make   method to build a
  //! transient document.
  Standard_EXPORT virtual void Read (const TCollection_ExtendedString& aFileName, const Handle(CDM_Document)& aNewDocument, const Handle(CDM_Application)& anApplication) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Make (const Handle(PCDM_Document)& aPCDM, const Handle(CDM_Document)& aNewDocument) = 0;
  
  Standard_EXPORT virtual TCollection_ExtendedString SchemaName() const = 0;
  
  Standard_EXPORT virtual void LoadExtensions (const Handle(Storage_Schema)& aSchema, const TColStd_SequenceOfExtendedString& Extensions, const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT static void RaiseIfUnknownTypes (const Handle(Storage_Schema)& aSchema, const TCollection_ExtendedString& aFileName);
  
  Standard_EXPORT static Standard_Integer DocumentVersion (const TCollection_ExtendedString& aFileName, const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT static Standard_Integer ReferenceCounter (const TCollection_ExtendedString& aFileName, const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT void SetFormat (const TCollection_ExtendedString& aformat);
  
  Standard_EXPORT TCollection_ExtendedString GetFormat() const;


friend   
  Standard_EXPORT void PCDM_ReferenceIterator::Init (const Handle(CDM_MetaData)& aMetaData);


  DEFINE_STANDARD_RTTI(PCDM_RetrievalDriver,PCDM_Reader)

protected:




private:

  
  Standard_EXPORT static void References (const TCollection_ExtendedString& aFileName, PCDM_SequenceOfReference& theReferences, const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT static void Extensions (const TCollection_ExtendedString& aFileName, TColStd_SequenceOfExtendedString& theExtensions, const Handle(CDM_MessageDriver)& theMsgDriver);
  
  Standard_EXPORT static void UserInfo (const TCollection_ExtendedString& aFileName, const TCollection_AsciiString& Start, const TCollection_AsciiString& End, TColStd_SequenceOfExtendedString& theUserInfo, const Handle(CDM_MessageDriver)& theMsgDriver);

  TCollection_ExtendedString myFormat;


};







#endif // _PCDM_RetrievalDriver_HeaderFile
