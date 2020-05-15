// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Image_SupportedFormats_HeaderFile
#define _Image_SupportedFormats_HeaderFile

#include <Image_CompressedFormat.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Type.hxx>

//! Structure holding information about supported texture formats.
class Image_SupportedFormats : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Image_SupportedFormats, Standard_Transient)
public:

  //! Empty constructor.
  Standard_EXPORT Image_SupportedFormats();

  //! Return TRUE if image format is supported.
  bool IsSupported (Image_Format theFormat) const { return myFormats.Value (theFormat); }

  //! Set if image format is supported or not.
  void Add (Image_Format theFormat) { myFormats.SetValue (theFormat, true); }

  //! Return TRUE if there are compressed image formats supported.
  bool HasCompressed() const { return myHasCompressed; }

  //! Return TRUE if compressed image format is supported.
  bool IsSupported (Image_CompressedFormat theFormat) const { return myFormats.Value (theFormat); }

  //! Set if compressed image format is supported or not.
  void Add (Image_CompressedFormat theFormat)
  {
    myFormats.SetValue (theFormat, true);
    myHasCompressed = true;
  }

  //! Reset flags.
  void Clear()
  {
    myFormats.Init (false);
    myHasCompressed = false;
  }

protected:

  NCollection_Array1<bool> myFormats; //!< list of supported formats
  Standard_Boolean   myHasCompressed; //!< flag indicating that some compressed image formats are supported

};

#endif // _Image_SupportedFormats_HeaderFile
