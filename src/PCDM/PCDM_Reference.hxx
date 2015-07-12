// Created on: 1997-12-09
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

#ifndef _PCDM_Reference_HeaderFile
#define _PCDM_Reference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_ExtendedString.hxx>
class TCollection_ExtendedString;



class PCDM_Reference 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT PCDM_Reference();
  
  Standard_EXPORT PCDM_Reference(const Standard_Integer aReferenceIdentifier, const TCollection_ExtendedString& aFileName, const Standard_Integer aDocumentVersion);
  
  Standard_EXPORT Standard_Integer ReferenceIdentifier() const;
  
  Standard_EXPORT TCollection_ExtendedString FileName() const;
  
  Standard_EXPORT Standard_Integer DocumentVersion() const;




protected:





private:



  Standard_Integer myReferenceIdentifier;
  TCollection_ExtendedString myFileName;
  Standard_Integer myDocumentVersion;


};







#endif // _PCDM_Reference_HeaderFile
