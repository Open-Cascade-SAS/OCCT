// Created on: 2014-12-18
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_ViewAffinity_HeaderFile
#define _Graphic3d_ViewAffinity_HeaderFile

#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

//! Structure display state.
class Graphic3d_ViewAffinity : public Standard_Transient
{
public:

  //! Empty constructor.
  Graphic3d_ViewAffinity()
  {
    SetVisible (Standard_True);
  }

  //! Return visibility flag.
  bool IsVisible (const Standard_Integer theViewId) const
  {
    const unsigned int aBit = 1 << theViewId;
    return (myMask & aBit) != 0;
  }

  //! Setup visibility flag for all views.
  void SetVisible (const Standard_Boolean theIsVisible)
  {
    ::memset (&myMask, theIsVisible ? 0xFF : 0x00, sizeof(myMask));
  }

  //! Setup visibility flag.
  void SetVisible (const Standard_Integer theViewId,
                   const bool             theIsVisible)
  {
    const unsigned int aBit = 1 << theViewId;
    if (theIsVisible)
    {
      myMask |=  aBit;
    }
    else
    {
      myMask &= ~aBit;
    }
  }

private:

  unsigned int myMask; //!< affinity mask

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_ViewAffinity,Standard_Transient)

};

DEFINE_STANDARD_HANDLE(Graphic3d_ViewAffinity, Standard_Transient)

#endif // _Graphic3d_ViewAffinity_HeaderFile
