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

#ifndef _Prs3d_TypeOfHighlight_HeaderFile
#define _Prs3d_TypeOfHighlight_HeaderFile

//! Type of highlighting to apply specific style.
enum Prs3d_TypeOfHighlight
{
  Prs3d_TypeOfHighlight_None = 0,      //!< no highlighting
  Prs3d_TypeOfHighlight_Selected,      //!< entire object is selected
  Prs3d_TypeOfHighlight_Dynamic,       //!< entire object is dynamically highlighted
  Prs3d_TypeOfHighlight_LocalSelected, //!< part of the object is selected
  Prs3d_TypeOfHighlight_LocalDynamic,  //!< part of the object is dynamically highlighted
  Prs3d_TypeOfHighlight_SubIntensity,  //!< sub-intensity style
  Prs3d_TypeOfHighlight_NB
};

#endif // _Prs3d_TypeOfHighlight_HeaderFile
