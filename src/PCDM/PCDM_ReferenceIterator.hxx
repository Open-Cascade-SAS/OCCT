// Created on: 1997-12-01
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

#ifndef _PCDM_ReferenceIterator_HeaderFile
#define _PCDM_ReferenceIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PCDM_SequenceOfReference.hxx>
#include <CDM_MetaDataLookUpTable.hxx>

class Message_Messenger;
class CDM_Document;
class CDM_MetaData;
class CDM_Application;

class PCDM_ReferenceIterator;
DEFINE_STANDARD_HANDLE(PCDM_ReferenceIterator, Standard_Transient)


class PCDM_ReferenceIterator : public Standard_Transient
{

public:

  
  //! Warning! The constructor does not initialization.
  Standard_EXPORT PCDM_ReferenceIterator(const Handle(Message_Messenger)& theMessageDriver);
  
  Standard_EXPORT void LoadReferences (const Handle(CDM_Document)& aDocument, const Handle(CDM_MetaData)& aMetaData, const Handle(CDM_Application)& anApplication, const Standard_Boolean UseStorageConfiguration);
  
  Standard_EXPORT virtual void Init (const Handle(CDM_MetaData)& aMetaData);




  DEFINE_STANDARD_RTTIEXT(PCDM_ReferenceIterator,Standard_Transient)

protected:




private:

  
  Standard_EXPORT virtual Standard_Boolean More() const;
  
  Standard_EXPORT virtual void Next();
  
  Standard_EXPORT virtual Handle(CDM_MetaData) MetaData (CDM_MetaDataLookUpTable& theLookUpTable,
                                                         const Standard_Boolean UseStorageConfiguration) const;
  
  Standard_EXPORT virtual Standard_Integer ReferenceIdentifier() const;
  
  //! returns the version of the document in the reference
  Standard_EXPORT virtual Standard_Integer DocumentVersion() const;

private:
  PCDM_SequenceOfReference myReferences;
  Standard_Integer myIterator;
  Handle(Message_Messenger) myMessageDriver;
};







#endif // _PCDM_ReferenceIterator_HeaderFile
