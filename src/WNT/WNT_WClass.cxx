// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <windows.h>


#include <InterfaceGraphic.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>
#include <WNT_ClassDefinitionError.hxx>
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>

#include <string.h>
//=======================================================================
//function : WNT_WClass
//purpose  : 
//=======================================================================
WNT_WClass::WNT_WClass (
               const Standard_CString aClassName,
               const Standard_Address aWndProc,
               const WNT_Uint&        aStyle,
               const Standard_Integer aClassExtra,
               const Standard_Integer aWindowExtra,
               const Aspect_Handle    aCursor,
               const Aspect_Handle    anIcon,
               const Standard_CString aMenuName
              ) {

  WNDCLASS wc;

  hInstance = GetModuleHandle ( NULL );

  wc.style         = aStyle;
  wc.lpfnWndProc   = ( aWndProc ) ? ( WNDPROC )aWndProc : DefWindowProc;
  wc.cbClsExtra    = aClassExtra;
  wc.cbWndExtra    = aWindowExtra;
  wc.hInstance     = ( HINSTANCE )hInstance;
  wc.hIcon         = ( anIcon )  ? ( HICON )anIcon    :
                                  LoadIcon ( NULL, IDI_APPLICATION );
  wc.hCursor       = ( aCursor ) ? ( HCURSOR )aCursor :
                                  LoadCursor ( NULL, IDC_NO );
  wc.hbrBackground = 0;
  wc.lpszMenuName  = aMenuName;
  wc.lpszClassName = aClassName;

  if (  !RegisterClass ( &wc )  )

    WNT_ClassDefinitionError :: Raise ( "Unable to register window class" );

  lpszName = new char[ strlen ( aClassName ) + 1 ];
  strcpy ( (Standard_PCharacter)lpszName, aClassName );
  lpfnWndProc = wc.lpfnWndProc;

}  // end constructor

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void WNT_WClass::Destroy ()
{

 UnregisterClass (  lpszName, ( HINSTANCE )hInstance  );
 delete [] (Standard_PCharacter)lpszName;

}  // end WNT_WClass :: Destroy

