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
  //! Write persistent data to a file.
  inline void Write(StdObjMgt_WriteData& theWriteData)
  {
    theWriteData << mySignature << myAxis1Reversed << myAxis2Reversed <<
      myAxis1 << myAxis2 << myValue1 << myValue2 << myNb1 << myNb2 << myMirror;
  }
  //! Gets persistent child objects
  inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
  {
    theChildren.Append(myAxis1);
    theChildren.Append(myAxis2);
    theChildren.Append(myValue1);
    theChildren.Append(myValue2);
    theChildren.Append(myNb1);
    theChildren.Append(myNb2);
    theChildren.Append(myMirror);
  }
  //! Returns persistent type name
  inline Standard_CString PName() const { return "PDataXtd_PatternStd"; }

  //! Import transient attribuite from the persistent data.
  void Import (const Handle(TDataXtd_PatternStd)& theAttribute) const;

private:
  Standard_Integer             mySignature;
  Standard_Boolean             myAxis1Reversed;
  Standard_Boolean             myAxis2Reversed;
  Handle(StdObjMgt_Persistent) myAxis1;
  Handle(StdObjMgt_Persistent) myAxis2;
  Handle(StdObjMgt_Persistent) myValue1;
  Handle(StdObjMgt_Persistent) myValue2;
  Handle(StdObjMgt_Persistent) myNb1;
  Handle(StdObjMgt_Persistent) myNb2;
  Handle(StdObjMgt_Persistent) myMirror;
};

#endif
