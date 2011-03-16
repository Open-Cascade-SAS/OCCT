// File:      WNT_Image.cxx
// Copyright: Open Cascade 2008

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

