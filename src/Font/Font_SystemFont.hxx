// Created on: 2008-01-20
// Created by: Alexander A. BORODIN
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Font_SystemFont_HeaderFile
#define _Font_SystemFont_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Font_FontAspect.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class Font_SystemFont;
DEFINE_STANDARD_HANDLE(Font_SystemFont, Standard_Transient)

//! Structure for store of Font System Information
class Font_SystemFont : public Standard_Transient
{

public:

  
  //! Creates empty font object
  Standard_EXPORT Font_SystemFont();
  
  //! Creates Font object initialized with <FontName> as name
  //! <FontAspect>.... TODO
  Standard_EXPORT Font_SystemFont(const Handle(TCollection_HAsciiString)& theFontName, const Font_FontAspect theFontAspect, const Handle(TCollection_HAsciiString)& theFilePath);
  
  //! Creates Font object and initialize class fields with
  //! values taken from XLFD (X Logical Font Description)
  Standard_EXPORT Font_SystemFont(const Handle(TCollection_HAsciiString)& theXLFD, const Handle(TCollection_HAsciiString)& theFilePath);
  
  //! Returns font family name
  Standard_EXPORT Handle(TCollection_HAsciiString) FontName() const;
  
  //! Returns font file path
  //! Level: Public
  Standard_EXPORT Handle(TCollection_HAsciiString) FontPath() const;
  
  //! Returns font aspect
  //! Level: Public
  Standard_EXPORT Font_FontAspect FontAspect() const;
  
  //! Returns font height
  //! If returned value is equal -1 it means that font is resizable
  //! Level: Public
  Standard_EXPORT Standard_Integer FontHeight() const;
  
  Standard_EXPORT Standard_Boolean IsValid() const;
  
  //! Return true if the FontName, FontAspect and FontSize are the same.
  //! Level: Public
  Standard_EXPORT Standard_Boolean IsEqual (const Handle(Font_SystemFont)& theOtherFont) const;




  DEFINE_STANDARD_RTTIEXT(Font_SystemFont,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) MyFontName;
  Font_FontAspect MyFontAspect;
  Standard_Integer MyFaceSize;
  Handle(TCollection_HAsciiString) MyFilePath;
  Standard_Boolean MyVerification;


};







#endif // _Font_SystemFont_HeaderFile
