// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Font_UnicodeSubset_HeaderFile
#define _Font_UnicodeSubset_HeaderFile

//! Enumeration defining Unicode subsets.
enum Font_UnicodeSubset
{
  Font_UnicodeSubset_Western, //!< western letters
  Font_UnicodeSubset_Korean,  //!< modern Korean letters
  Font_UnicodeSubset_CJK,     //!< Chinese characters (Chinese, Japanese, Korean and Vietnam)
  Font_UnicodeSubset_Arabic,  //!< Arabic  characters
};

enum { Font_UnicodeSubset_NB = Font_UnicodeSubset_Arabic };

#endif // _Font_UnicodeSubset_HeaderFile
