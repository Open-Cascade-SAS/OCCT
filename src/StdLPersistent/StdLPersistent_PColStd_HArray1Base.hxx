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


#ifndef _StdLPersistent_PColStd_HArray1Base_HeaderFile
#define _StdLPersistent_PColStd_HArray1Base_HeaderFile

#include <StdObjMgt_Persistent.hxx>


class StdLPersistent_PColStd_HArray1Base : public StdObjMgt_Persistent
{
public:
  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

protected:
  virtual void CreateArray (const Standard_Integer theLowerBound,
                            const Standard_Integer theUpperBound) = 0;

  virtual void ReadValue (StdObjMgt_ReadData& theReadData,
                          const Standard_Integer theIndex) = 0;
};

#endif
