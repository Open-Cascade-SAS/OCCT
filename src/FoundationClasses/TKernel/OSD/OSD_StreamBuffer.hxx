// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OSD_StreamBuffer_HeaderFile
#define _OSD_StreamBuffer_HeaderFile

#include <memory>
#include <string>

//! A file stream implementation initialized from std::shared_ptr<std::streambuf>.
template <typename T>
class OSD_StreamBuffer : public T
{
public:
  //! Main constructor.
  OSD_StreamBuffer(const std::string& theUrl, const std::shared_ptr<std::streambuf>& theBuffer)
      : T(theBuffer.get()),
        myUrl(theUrl),
        myBuffer(theBuffer)
  {
  }

  //! Return an opened URL.
  const std::string& Url() const { return myUrl; }

protected:
  std::string                     myUrl;
  std::shared_ptr<std::streambuf> myBuffer;
};

typedef OSD_StreamBuffer<std::istream>  OSD_IStreamBuffer;
typedef OSD_StreamBuffer<std::ostream>  OSD_OStreamBuffer;
typedef OSD_StreamBuffer<std::iostream> OSD_IOStreamBuffer;

#endif // _OSD_StreamBuffer_HeaderFile
