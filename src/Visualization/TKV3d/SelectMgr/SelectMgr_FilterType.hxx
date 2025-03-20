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

#ifndef _SelectMgr_FilterType_HeaderFile
#define _SelectMgr_FilterType_HeaderFile

//! Enumeration defines the filter type.
enum SelectMgr_FilterType
{
  SelectMgr_FilterType_AND, //!< an object should be suitable for all filters.
  SelectMgr_FilterType_OR   //!< an object should be suitable at least one filter.
};

#endif // _SelectMgr_FilterType_HeaderFile
