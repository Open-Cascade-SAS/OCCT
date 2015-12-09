// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdLPersistent_PDF_Data_HeaderFile
#define _StdLPersistent_PDF_Data_HeaderFile

#include <StdObjMgt_Persistent.hxx>

#include <StdLPersistent_PColStd_HArray1.hxx>

class TDF_Data;


class StdLPersistent_PDF_Data : public StdObjMgt_Persistent
{
public:
  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

  //! Import transient data from the persistent data.
  Standard_EXPORT Handle(TDF_Data) Import() const;

private:
  class Parser;

private:
  Standard_Integer myVersion;
  Handle(StdLPersistent_PColStd_HArray1OfInteger) myLabels;
  Handle(StdLPersistent_PDF_HAttributeArray1) myAttributes;
};

#endif
