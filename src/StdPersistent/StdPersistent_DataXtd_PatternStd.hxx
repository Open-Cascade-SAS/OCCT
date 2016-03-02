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


#ifndef _StdPersistent_DataXtd_PatternStd_HeaderFile
#define _StdPersistent_DataXtd_PatternStd_HeaderFile

#include <StdObjMgt_Attribute.hxx>

#include <TDataXtd_PatternStd.hxx>


class StdPersistent_DataXtd_PatternStd
  : public StdObjMgt_Attribute<TDataXtd_PatternStd>
{
public:
  //! Read persistent data from a file.
  inline void Read (StdObjMgt_ReadData& theReadData)
  {
    theReadData >> mySignature >> myAxis1Reversed >> myAxis2Reversed >>
      myAxis1 >> myAxis2 >> myValue1 >> myValue2 >> myNb1 >> myNb2 >> myMirror;
  }

  //! Import transient attribuite from the persistent data.
  void Import (const Handle(TDataXtd_PatternStd)& theAttribute) const;

private:
  Value<Standard_Integer> mySignature;
  Value<Standard_Boolean> myAxis1Reversed;
  Value<Standard_Boolean> myAxis2Reversed;
  Reference<>             myAxis1;
  Reference<>             myAxis2;
  Reference<>             myValue1;
  Reference<>             myValue2;
  Reference<>             myNb1;
  Reference<>             myNb2;
  Reference<>             myMirror;
};

#endif
