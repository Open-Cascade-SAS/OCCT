// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef __WNT_WindowData_HeaderFile
# define __WNT_WindowData_HeaderFile

# ifndef __WINDOWS_H_INCLUDED
#  define __WINDOWS_H_INCLUDED
#  ifndef STRICT
#   define STRICT
#  endif // STRICT
#  include <windows.h>
#  include <windowsx.h>

#  ifdef DrawText
#   undef DrawText
#  endif // DrawText

#  ifdef THIS
#   undef THIS
#  endif  // THIS
# endif  // __WINDOWS_H_INCLUDED

# ifndef __STANDARD_TYPE_HXX_INCLUDED
#  define __STANDARD_TYPE_HXX_INCLUDED
#  include <Standard_Type.hxx>
# endif  // __STANDARD_TYPE_HXX_INCLUDED 

#include <InterfaceGraphic_WNT.hxx>

typedef WINDOW_DATA WNT_WindowData;

extern const Handle( Standard_Type )& STANDARD_TYPE( WNT_WindowData );

#endif  // __WNT_WindowData_HeaderFile
