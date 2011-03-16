// File:      WNT_WClass.cxx
// Copyright: Open Cascade 2008

#include <windows.h>
#include <WNT_WClass.ixx>

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

