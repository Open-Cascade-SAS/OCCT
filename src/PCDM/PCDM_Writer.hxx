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

#ifndef _PCDM_Writer_HeaderFile
#define _PCDM_Writer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class PCDM_DriverError;
class CDM_Document;
class TCollection_ExtendedString;


class PCDM_Writer;
DEFINE_STANDARD_HANDLE(PCDM_Writer, Standard_Transient)


class PCDM_Writer : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual void Write (const Handle(CDM_Document)& aDocument, const TCollection_ExtendedString& aFileName) = 0;

  //! Write <theDocument> to theOStream
  Standard_EXPORT virtual void Write (const Handle(CDM_Document)& theDocument, Standard_OStream& theOStream) = 0;



  DEFINE_STANDARD_RTTIEXT(PCDM_Writer,Standard_Transient)

protected:




private:




};







#endif // _PCDM_Writer_HeaderFile
