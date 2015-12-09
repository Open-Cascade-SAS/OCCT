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


#ifndef _StdLPersistent_PCollection_HExtendedString_HeaderFile
#define _StdLPersistent_PCollection_HExtendedString_HeaderFile

#include <StdObjMgt_Persistent.hxx>

class TCollection_HExtendedString;


class StdLPersistent_PCollection_HExtendedString : public StdObjMgt_Persistent
{
public:
  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

  //! Get the value.
  Handle(TCollection_HExtendedString) Value() const { return myValue; }

private:
  Handle(TCollection_HExtendedString) myValue;
};

#endif
