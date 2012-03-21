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


// include windows.h first to have all definitions available
#include <windows.h>
#include <WNT_Image.ixx>

#include <W32_Allocator.hxx>

//=======================================================================
//function : WNT_Image
//purpose  : 
//=======================================================================

WNT_Image::WNT_Image ( const Aspect_Handle aBitmap,
                       const Standard_Integer aHashCode )
{

  myImage = ( Standard_Address )HeapAlloc (
                                GetProcessHeap (),
                                HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS,
                                sizeof ( WNT_Bitmap )
                               );
 
  (  ( PW32_Bitmap )myImage ) -> hBmp  = ( ::HBITMAP )aBitmap;
  (  ( PW32_Bitmap )myImage ) -> nUsed = 1;
  myHashCode                           = aHashCode;

}  // end constructor

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void WNT_Image::Destroy () {

  if (  --(  ( PW32_Bitmap )myImage  ) -> nUsed == 0 &&
          (  ( PW32_Bitmap )myImage  ) -> hBmp  != NULL
 ) {
 
    DeleteObject (   (  ( PW32_Bitmap )myImage  ) -> hBmp   );
    HeapFree (  GetProcessHeap (), 0, ( PVOID )myImage  );

  }  // end if

}  // end WNT_Image :: Destroy

