// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _V3d_ListOfView_HeaderFile
#define _V3d_ListOfView_HeaderFile

class V3d_View;
#include <NCollection_List.hxx>

typedef NCollection_List<Handle(V3d_View)> V3d_ListOfView;
typedef V3d_ListOfView::Iterator           V3d_ListOfViewIterator;

#endif // _V3d_ListOfView_HeaderFile
