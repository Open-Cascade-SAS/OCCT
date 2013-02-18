// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <windows.h>
#include <WNT_WClass.ixx>

#include <Standard_PCharacter.hxx>
#include <InterfaceGraphic_WNT.hxx>

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
  wc.cbWndExtra    = aWindowExtra + sizeof ( WINDOW_DATA* );
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

