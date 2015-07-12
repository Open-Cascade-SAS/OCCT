// Created on: 1997-08-04
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

#ifndef _CDM_ReferenceIterator_HeaderFile
#define _CDM_ReferenceIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <CDM_ListIteratorOfListOfReferences.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class CDM_Document;



class CDM_ReferenceIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT CDM_ReferenceIterator(const Handle(CDM_Document)& aDocument);
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT Handle(CDM_Document) Document() const;
  
  Standard_EXPORT Standard_Integer ReferenceIdentifier() const;
  
  //! returns the Document Version in the reference.
  Standard_EXPORT Standard_Integer DocumentVersion() const;




protected:





private:



  CDM_ListIteratorOfListOfReferences myIterator;


};







#endif // _CDM_ReferenceIterator_HeaderFile
