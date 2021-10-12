// Created on: 1995-03-22
// Created by: Jean-Louis  Frenkel
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _CDF_StoreList_HeaderFile
#define _CDF_StoreList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <CDM_MapOfDocument.hxx>
#include <CDM_ListOfDocument.hxx>
#include <CDM_MapIteratorOfMapOfDocument.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Message_ProgressIndicator.hxx>

class CDM_Document;
class CDM_MetaData;
class TCollection_ExtendedString;


class CDF_StoreList;
DEFINE_STANDARD_HANDLE(CDF_StoreList, Standard_Transient)


class CDF_StoreList : public Standard_Transient
{

public:

  
  Standard_EXPORT CDF_StoreList(const Handle(CDM_Document)& aDocument);
  
  Standard_EXPORT Standard_Boolean IsConsistent() const;
  
  //! stores each object of the storelist in the reverse
  //! order of which they had been added.
  Standard_EXPORT PCDM_StoreStatus Store (Handle(CDM_MetaData)& aMetaData, 
                                          TCollection_ExtendedString& aStatusAssociatedText,
                                          const Message_ProgressRange& theRange = Message_ProgressRange());
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT Handle(CDM_Document) Value() const;




  DEFINE_STANDARD_RTTIEXT(CDF_StoreList,Standard_Transient)

protected:




private:

  
  Standard_EXPORT void Add (const Handle(CDM_Document)& aDocument);

  CDM_MapOfDocument myItems;
  CDM_ListOfDocument myStack;
  CDM_MapIteratorOfMapOfDocument myIterator;
  Handle(CDM_Document) myMainDocument;


};







#endif // _CDF_StoreList_HeaderFile
