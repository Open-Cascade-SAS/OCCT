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

#ifndef _AIS_SelectionScheme_HeaderFile
#define _AIS_SelectionScheme_HeaderFile

//! Sets selection schemes for interactive contexts.
enum AIS_SelectionScheme
{
  AIS_SelectionScheme_UNKNOWN = -1, //!< undefined scheme
  AIS_SelectionScheme_Replace = 0,  //!< clears current selection and select detected objects
  AIS_SelectionScheme_Add,          //!< adds    detected object to current selection
  AIS_SelectionScheme_Remove,       //!< removes detected object from the current selection
  AIS_SelectionScheme_XOR,          //!< performs XOR for detected objects, other selected not touched
  AIS_SelectionScheme_Clear,        //!< clears current selection
  AIS_SelectionScheme_ReplaceExtra, //!< replace with one difference: if result of replace is an empty,
                                    //!< and current selection contains detected element, it will be selected
};

#endif // _AIS_SelectionScheme_HeaderFile
