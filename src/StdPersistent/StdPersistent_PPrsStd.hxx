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


#ifndef _StdPersistent_PPrsStd_PatternStd_HeaderFile
#define _StdPersistent_PPrsStd_PatternStd_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HString.hxx>

#include <TDataXtd_Presentation.hxx>

class StdPersistent_PPrsStd
{
public:
  class AISPresentation : public StdObjMgt_Attribute<TDataXtd_Presentation>
  {
  public:
    //! Read persistent data from a file.
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      theReadData >> myIsDisplayed >> myDriverGUID
        >> myTransparency >> myColor >> myMaterial >> myWidth;
    }

    //! Import transient attribuite from the persistent data.
    void Import (const Handle(TDataXtd_Presentation)& theAttribute) const;

  private:
    Value<Standard_Boolean> myIsDisplayed;
    Reference<>             myDriverGUID;
    Value<Standard_Real>    myTransparency;
    Value<Standard_Integer> myColor;
    Value<Standard_Integer> myMaterial;
    Value<Standard_Real>    myWidth;
  };

  class AISPresentation_1 : public AISPresentation
  {
  public:
    //! Read persistent data from a file.
    inline void Read (StdObjMgt_ReadData& theReadData)
    {
      AISPresentation::Read (theReadData);
      theReadData >> myMode;
    }

    //! Import transient attribuite from the persistent data.
    void Import (const Handle(TDataXtd_Presentation)& theAttribute) const;

  private:
    Value<Standard_Integer> myMode;
  };
};

#endif
