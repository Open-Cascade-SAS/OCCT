// Created by: DCB
// Copyright (c) 1998-1999 Matra Datavision
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

// Notes:
//    Read()/Write() code is taken from ZOV's sources from Xw package.

// include windows.h first to ensure that it is loaded completely
#ifdef WNT
#include <windows.h>
#endif

#include <AlienImage_BMPAlienData.ixx>
#include <AlienImage_BMPHeader.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Quantity_Color.hxx>
#include <Image_PseudoColorImage.hxx>
#include <Image_ColorImage.hxx>
#include <OSD_FromWhere.hxx>
#include <Standard.hxx>

static unsigned long __swaptest = 1;

WORD static _TestSwapWORD (WORD __w)
{
  return LOW_VAL_AT_LOW_ADDR ? __w:
  ((__w&0xFF) << 8) | ((__w&0xFF00) >> 8);
}

DWORD static _TestSwapDWORD (DWORD __dw)
{
  return LOW_VAL_AT_LOW_ADDR ? __dw:
  ((__dw&0x000000FF) << 24) | ((__dw&0x0000FF00) << 8)
    | ((__dw&0xFF0000) >> 8)  |  ((__dw&0xFF000000) >> 24);
}

#define STGMGR_ALLOC(size)    Standard::Allocate(size)
#define STGMGR_FREE(buf,size) Standard::Free((void*&)buf)
#define BPIXEL                ((BYTE *) myData)
#define DWPIXEL               ((DWORD*) myData)

//================================================================
AlienImage_BMPAlienData::AlienImage_BMPAlienData ()
                        : AlienImage_AlienImageData ()
{
  myData     = NULL;
  myWidth    = 0;
  myHeight   = 0;
  myColorMap.Nullify ();
}

//================================================================
void AlienImage_BMPAlienData::Clear ()
{
  if (myData) {
    STGMGR_FREE (myData, (!myColorMap.IsNull() ? myWidth*myHeight :
                                                 myWidth*myHeight*4));
    myData = NULL;
  }
  myWidth  = 0;
  myHeight = 0;
  myColorMap.Nullify ();
}

//================================================================
Standard_Boolean AlienImage_BMPAlienData::Read (OSD_File& file)
{
  BYTE *pData = NULL, *pbData=NULL; // pointer to bitmap pixels
  BYTE *ptrByte;
  int   i, isize, ncolors, bytes_per_line, x, y, width, height;
  DWORD dwRMask, dwGMask, dwBMask, dwSize;
  int   nRMult, nGMult, nBMult;
  int   isInsideOut, hasMask, nBitCount;
  int   iCompression, isOS2Format, hasColormap;
  int   nBytes, iDataSize;
  char  bmpSign[2];
  AlienImage_BMPHeader bmfh; // Standard BMP file header
  BITMAPINFOHEADER bmih; // Windows BMP header
  BITMAPCOREHEADER bmch; // OS/2 BMP header
  Standard_Address lpVoid;
  Quantity_Color color;
  Standard_Real r, g, b;
  Aspect_ColorMapEntry entry;

  RGBQUAD* pColors256 = (RGBQUAD*) STGMGR_ALLOC (sizeof(RGBQUAD)*256);

  // Check 'BM' signature
  file.Seek (0, OSD_FromBeginning);
  lpVoid = Standard_Address(&bmpSign[0]);
  file.Read ((void*&)lpVoid, 2, nBytes);
  if (file.Failed () || strncmp (bmpSign, "BM", 2) || nBytes != 2) {
//    cout << "AlienImage_BMPAlienData::Read() : 'BM' signature not found."
//         << endl << flush;
    goto _ExitReadError;
  }

  lpVoid = Standard_Address(&bmfh);
  file.Read ((void*&)lpVoid, sizeof(AlienImage_BMPHeader), nBytes);
  if (file.Failed () || nBytes != sizeof(AlienImage_BMPHeader)) {
    cout << "AlienImage_BMPAlienData::Read() : Reading AlienImage_BMPHeader."
         << endl << flush;
    goto _ExitReadError;
  }

  lpVoid = Standard_Address (&dwSize);
  file.Read ((void*&)lpVoid, sizeof (DWORD), nBytes);
  if (file.Failed() || nBytes != sizeof (DWORD)) {
    cout << "AlienImage_BMPAlienData::Read() : BAD file size." << endl << flush;
    goto _ExitReadError;
  }
  file.Seek (-(int)sizeof(DWORD), OSD_FromHere);

  isOS2Format = _TestSwapDWORD(dwSize) == sizeof (BITMAPCOREHEADER);
  if (isOS2Format) {
    lpVoid = Standard_Address(&bmch);
    file.Read ((void*&)lpVoid, sizeof(BITMAPCOREHEADER), nBytes);
    if (file.Failed() || nBytes != sizeof(BITMAPCOREHEADER)) {
      cout << "AlienImage_BMPAlienData::Read() : Reading BITMAPCOREHEADER."
           << endl << flush;
      goto _ExitReadError;
    }
  } else {
    lpVoid = Standard_Address(&bmih);
    file.Read ((void*&)lpVoid, sizeof(BITMAPINFOHEADER), nBytes);
    if (file.Failed() || nBytes != sizeof(BITMAPINFOHEADER)) {
      cout << "AlienImage_BMPAlienData::Read() : Reading BITMAPINFOHEADER."
           << endl << flush;
      goto _ExitReadError;
    }
  }

  nBitCount = _TestSwapWORD (isOS2Format ? bmch.bcBitCount: bmih.biBitCount);
  if ((nBitCount != 1 &&  nBitCount != 4 && nBitCount != 8 &&
       nBitCount != 16 && nBitCount != 24 && nBitCount != 32) ||
       isOS2Format && (nBitCount == 16 || nBitCount == 32)) {
    cout << "AlienImage_BMPAlienData::Read() : Bad <nBitCount> value :"
         << nBitCount << " " << isOS2Format << endl << flush;
    goto _ExitReadError;
  }

  iCompression = isOS2Format ? BI_RGB: _TestSwapDWORD (bmih.biCompression);
  hasColormap = nBitCount <= 8;
  hasMask = iCompression == BI_BITFIELDS;
  ncolors = hasColormap ? 1 << nBitCount: 0;
  width   = isOS2Format ? _TestSwapWORD  (bmch.bcWidth)
                        : _TestSwapDWORD (bmih.biWidth);
  height  = isOS2Format ? _TestSwapWORD  (bmch.bcHeight)
                        : _TestSwapDWORD (bmih.biHeight);
  isInsideOut = !isOS2Format && height < 0;
  height = isInsideOut ? -height: height;
  bytes_per_line = width * nBitCount;
  bytes_per_line = (bytes_per_line>>3) + (bytes_per_line&0x7 ? 1: 0);
  bytes_per_line += (bytes_per_line&0x3)? (4-(bytes_per_line&0x3)): 0;

  myWidth  = width;
  myHeight = height;

  if (hasColormap) {
    // Retreive array of colors (as RGBQUAD sequence).
    // Note, that there can be less then ncolors colors.
    isize = ncolors * sizeof (RGBQUAD);

    file.Read ((void*&)pColors256, isize, nBytes);
    if (file.Failed() || nBytes != isize) {
      cout << "AlienImage_BMPAlienData::Read() : Reading ColorMap."
           << endl << flush;
      goto _ExitReadError;
    }
    // Build colormap
    myColorMap = new Aspect_GenericColorMap ();
    for (i=0; i<ncolors; i++) {
      r = ((float)pColors256[i].rgbRed   / 255.);
      g = ((float)pColors256[i].rgbGreen / 255.);
      b = ((float)pColors256[i].rgbBlue  / 255.);
      color.SetValues (r, g, b, Quantity_TOC_RGB);
      entry.SetValue (i, color);
      myColorMap -> AddEntry (entry);
    }
  }

  if (hasMask) {
    PDWORD pdw;
    // Retrieve three DWORD's that contain the masks for R, G and B respectively
    file.Read ((void*&)(pdw = &dwRMask), sizeof (DWORD), nBytes);
    if (file.Failed() || nBytes != sizeof(DWORD)) {
      cout << "AlienImage_BMPAlienData::Read() : BAD file size." << endl << flush;
      goto _ExitReadError;
    }

    file.Read ((void*&)(pdw = &dwGMask), sizeof (DWORD), nBytes);
    if (file.Failed() || nBytes != sizeof(DWORD)) {
      cout << "AlienImage_BMPAlienData::Read() : BAD file size." << endl << flush;
      goto _ExitReadError;
    }

    file.Read ((void*&)(pdw = &dwBMask), sizeof (DWORD), nBytes);
    if (file.Failed() || nBytes != sizeof(DWORD)) {
      cout << "AlienImage_BMPAlienData::Read() : BAD file size." << endl << flush;
      goto _ExitReadError;
    }

    if (hasColormap || !dwRMask || !dwGMask || !dwBMask) {
      cout << "AlienImage_BMPAlienData::Read() : BAD image colormap." << endl << flush;
      goto _ExitReadError;
    }
    
    dwRMask = _TestSwapDWORD (dwRMask);
    dwGMask = _TestSwapDWORD (dwGMask);
    dwBMask = _TestSwapDWORD (dwBMask);

    nRMult = nGMult = nBMult = 0;

    for (i=dwRMask; (i&1)^1; i >>= 1)
      nRMult++;
    for (i=dwGMask; (i&1)^1; i >>= 1)
      nGMult++;
    for (i=dwBMask; (i&1)^1; i >>= 1)
      nBMult++;
  } else {
    dwBMask = 0x000000ff, dwGMask = 0x0000ff00, dwRMask = 0x00ff0000,
    nBMult = 0, nGMult = 8, nRMult = 16;
  }

  // Allocate the pixel buffer and load raw data from file
  i = _TestSwapDWORD (bmfh.bfOffBits);
  iDataSize = _TestSwapDWORD (bmfh.bfSize) - i;
  pData = (BYTE *) STGMGR_ALLOC (iDataSize);
  pbData = pData;

  file.Seek (i, OSD_FromBeginning);
  file.Read ((void*&)pData, iDataSize, nBytes);
  if (file.Failed () || nBytes != iDataSize) {
    cout << "AlienImage_BMPAlienData::Read() : Image data."
         << endl << flush;
    goto _ExitReadError;
  }

  if (iCompression == BI_RLE4 || iCompression == BI_RLE8) {
    pbData = (BYTE*) STGMGR_ALLOC (width*height);
    // Note, that it is possible that "spots" could appear
    // after decompressing the data encoded using RLE algorith.
    // The method assumes that the image is drawn to an already
    // defined background. That's why I have to fill in these
    // probable spots with some color (I have chosen 0-indexed one).
    memset (pbData, 0, iDataSize);
    // Decompress the data to array of 8-bit color indices
    x = 0;
    y = 0;
    ptrByte = pData;
    for (;;) {
      BYTE bPixCount = *ptrByte++;
      BYTE bCode, bColor;
      if (bPixCount) {
        bColor = *ptrByte++;
        //fprintf (stderr, "[%02x S]", (int)bPixCount);
        if (iCompression == BI_RLE8) {
          while (bPixCount--)
           pbData [width*y + x++] = bColor;
        } else {
          BYTE bColor0 = (bColor & 0xf0) >> 4;
          BYTE bColor1 =  bColor & 0x0f;
          i = 0;
          while (bPixCount--)
            pbData [width*y + x++] = i++ & 1 ? bColor1: bColor0;
        }
        continue;
      }
      // Zero code has been reached.
      // Do the command specified in the second byte.
      bCode = *ptrByte++;

      if (!bCode) { // EOL
        x = 0, y++; //,fprintf (stderr, "\r\n");
      } else if (bCode == 1) { // EOF
        break;
      } else if (bCode == 2) {  // Delta
        x += (unsigned) *ptrByte++;
        y += (unsigned) *ptrByte++;
        //fprintf (stderr, "[POS=%d,%d]", x, y);
      } else { // Absolute mode
        bPixCount = bCode;
        //fprintf (stderr, "[%02x D]", (int)bPixCount);
        if (iCompression == BI_RLE8) {
          while (bPixCount--)
            pbData [width*y + x++] = *ptrByte++;
        } else {
          i = 0;
          while (bPixCount--)
            pbData [width*y + x++] = i++ & 1 ?  *ptrByte & 0x0f
                                             : (*ptrByte++ & 0xf0) >> 4;
        }
        // each run must aligned on a word boundary
        if (iCompression == BI_RLE8 && (bCode & 1) ||
            iCompression == BI_RLE4 && (bCode & 3))
          ptrByte ++;
      }
    }
 
    bytes_per_line = width;
    nBitCount = 8;
    STGMGR_FREE (pData, iDataSize);
    iDataSize = width*height;
  }

  // Data isn't comressed. Can just use the raw data
  // Alloc image data (pidata) and fill in the buffer with RGBQUAD's
  isize = width*height;
  myData = STGMGR_ALLOC (!myColorMap.IsNull() ? myWidth*myHeight : myWidth*myHeight*4);
  x = 0;
  y = isInsideOut? 0: height-1;
  for (i = 0; i < isize; i++) {
    DWORD dwValue=0;
    BYTE* pbLine = pbData + y*bytes_per_line;
    switch (nBitCount) {
      case 32:
        dwValue = *(((DWORD *) pbLine) + x); break; // next double word value
      case 16:
        dwValue = *(((WORD *) pbLine) + x); break; // next word
      case 8:
        dwValue = *(((BYTE *) pbLine) + x); break; // next byte
      case 24: {
        BYTE* pbTripple = pbLine + x+(x<<1);
        dwValue = *pbTripple + (*(pbTripple +1) <<8) + (*(pbTripple +2) <<16);
      } break;
      case 4:
        dwValue = (x&1) ? *(pbLine+(x>>1))&0x0f: (*(pbLine+(x>>1))&0xf0) >> 4; break;
      case 1: {
        int iBit = 7 - (x&7);
        dwValue = (*(pbLine+(x>>3)) & (1 <<iBit))  >>iBit;
      } break; // next bit
    }
    if (hasColormap) {
      BPIXEL [i] = (BYTE)  dwValue; // put index in colormap
    } else {
      DWPIXEL[i] = (DWORD) dwValue; // put RGB
    }
    if (++x == width) {
      x = 0;
      y += isInsideOut? 1: -1;
    }
  }

  STGMGR_FREE (pColors256, sizeof(RGBQUAD)*256);
  STGMGR_FREE (pbData, iDataSize);
  return Standard_True;

_ExitReadError:
//  cout << "AlienImage_BMPAlienData::Read() : Read file error." << endl << flush;
  STGMGR_FREE (pColors256, sizeof(RGBQUAD)*256);
  if (pbData) STGMGR_FREE (pbData, iDataSize);
  Clear ();
  return Standard_False;
}

//================================================================
Standard_Boolean AlienImage_BMPAlienData::Write (OSD_File& file) const
{
  AlienImage_BMPHeader bfh;
  BITMAPINFOHEADER bih;
  int   isize, x, y;
  BYTE *pbData = NULL, *pbCell = NULL;
  WORD  sign   = _TestSwapWORD(0x4D42); // BMP file signature: SWAP('MB')
  DWORD    dwPixel;
  RGBQUAD* prgbColor = (RGBQUAD*)&dwPixel;
  Quantity_Color color;
  Standard_Real r, g, b;

  if (myData == NULL || myWidth == 0 || myHeight == 0 )
    return Standard_False;
  // Get the size of array of pixels
  isize  = myWidth*3; // actual size of data in each scan line
  isize += (isize & 3 ? 4 - (isize & 3): 0); // plus pad size
  isize *= myHeight;
  pbData = (BYTE*) STGMGR_ALLOC (isize);

  // Build and write File Header.
  bfh.bfSize     = _TestSwapDWORD(2+sizeof (AlienImage_BMPHeader) +
                                    sizeof (BITMAPINFOHEADER) +
                                    sizeof (RGBQUAD)*0 +
                                    isize);
  bfh.bfReserved = 0;
  bfh.bfOffBits  = _TestSwapDWORD(2+sizeof (AlienImage_BMPHeader) +
                                    sizeof (BITMAPINFOHEADER) +
                                    sizeof (RGBQUAD)*0);

  // Write file signature
  file.Write (&sign, 2);
  if (file.Failed ())
    goto _ExitWriteError;

  // Write file header
  file.Write (&bfh, sizeof (bfh));
  if (file.Failed ())
    goto _ExitWriteError;

  // Build and write Info Header.
  bih.biSize          = _TestSwapDWORD(sizeof (BITMAPINFOHEADER));
  bih.biWidth         = _TestSwapDWORD(myWidth );
  bih.biHeight        = _TestSwapDWORD(myHeight);
  bih.biPlanes        = _TestSwapWORD (1);
  bih.biBitCount      = _TestSwapWORD (24); // 8 bits per each of R,G and B value
  bih.biCompression   = 0; // BI_RGB
  bih.biSizeImage     = 0; // can be zero for BI_RGB
  bih.biXPelsPerMeter = 0;
  bih.biYPelsPerMeter = 0; 
  bih.biClrUsed       = 0; // all colors used
  bih.biClrImportant  = 0; // all colors imortant

  file.Write (&bih, sizeof (bih));
  if (file.Failed ())
    goto _ExitWriteError;

  // Fill in array of pixels and write out the buffer.
  pbCell = pbData;
  for (y = myHeight-1; y >= 0; y--) {
    for (x = 0; x < myWidth; x++) {
      if (!myColorMap.IsNull ()) {
        color = myColorMap -> FindEntry (BPIXEL[y*myWidth + x]).Color ();
        color.Values (r, g, b, Quantity_TOC_RGB);
        *pbCell++ = (BYTE)(b*255.);
        *pbCell++ = (BYTE)(g*255.);
        *pbCell++ = (BYTE)(r*255.);
      } else {
        dwPixel = _TestSwapDWORD (DWPIXEL[y*myWidth + x]);
        *pbCell++ = prgbColor -> rgbBlue;
        *pbCell++ = prgbColor -> rgbGreen;
        *pbCell++ = prgbColor -> rgbRed;
      }
    }
    x *= 3;
    // Pad each scan line with zeroes to end on a LONG data-type boundary
    while (x++ & 0x03) // i.e. until 4 devides x
      *pbCell++ = 0;
  }

  file.Write (pbData, isize);
  if (file.Failed ())
    goto _ExitWriteError;

  STGMGR_FREE (pbData, isize);
  return Standard_True;
        
_ExitWriteError:
  cout << "AlienImage_BMPAlienData::Write() : Write file error." << endl << flush;
  STGMGR_FREE (pbData, isize);
  return Standard_False;
}

//================================================================
Handle_Image_Image AlienImage_BMPAlienData::ToImage () const
{
  Standard_Integer    x, y, LowX, LowY;
  Handle(Image_Image) theImage;
  if (!myColorMap.IsNull()) {
    Aspect_IndexPixel iPixel;
    theImage = new Image_PseudoColorImage (0, 0, myWidth, myHeight, myColorMap);
    LowX = theImage -> LowerX ();
    LowY = theImage -> LowerY ();
    for (y = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        iPixel.SetValue (BPIXEL[y*myWidth + x]);
        theImage -> SetPixel (LowX + x, LowY + y, iPixel);
      }
    }
  } else {
    DWORD    dwPixel;
    RGBQUAD* pRgb = (RGBQUAD*)&dwPixel;
    Standard_Real r, g, b;
    Quantity_Color color;
    Aspect_ColorPixel cPixel;
    theImage = new Image_ColorImage (0, 0, myWidth, myHeight);
    LowX = theImage -> LowerX ();
    LowY = theImage -> LowerY ();
    for (y = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        dwPixel = _TestSwapDWORD (DWPIXEL[y*myWidth + x]);
        r = ((float)pRgb -> rgbRed   / 255.);
        g = ((float)pRgb -> rgbGreen / 255.);
        b = ((float)pRgb -> rgbBlue  / 255.);
        color.SetValues (r, g, b, Quantity_TOC_RGB);
        cPixel.SetValue (color);
        theImage -> SetPixel (LowX + x, LowY + y, cPixel);
      }
    }
  }
  return theImage;
}

//================================================================
void AlienImage_BMPAlienData::FromImage (const Handle_Image_Image& anImage)
{
  if (anImage -> Type() == Image_TOI_PseudoColorImage) {
    // Build from PseudoColorImage
    Handle(Image_PseudoColorImage) aPImage =
      Handle(Image_PseudoColorImage)::DownCast(anImage);
    FromPseudoColorImage (aPImage);
  } else if (anImage -> Type() == Image_TOI_ColorImage) {
    // Build from ColorImage
    Handle(Image_ColorImage) aCImage =
      Handle(Image_ColorImage)::DownCast(anImage);
    FromColorImage (aCImage);
  } else {
    // Unknown type of image
    Standard_TypeMismatch_Raise_if (Standard_True,
      "Attempt to convert a unknown Image_Image type to a BMPAlienData");
  }
}

//================================================================
void AlienImage_BMPAlienData::FromPseudoColorImage (
                const Handle(Image_PseudoColorImage)& anImage)
{
  int width  = anImage -> Width  ();
  int height = anImage -> Height ();
  int x, y, LowX = anImage -> LowerX(), LowY = anImage -> LowerY();
  Aspect_IndexPixel iPixel;
  BYTE index;

  if (width*height > 0) {
    Handle(Aspect_ColorMap) aColorMap = anImage -> ColorMap ();
    // Clear old data if any
    Clear ();
    // Fill colormap
    myColorMap = new Aspect_GenericColorMap ();
    for (int i = 1; i <= aColorMap -> Size (); i++)
      myColorMap -> AddEntry (aColorMap -> Entry (i));
    // Fill image's data
    myWidth = width;
    myHeight = height;
    myData = STGMGR_ALLOC (myWidth*myHeight);
    for (y = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        iPixel = anImage -> Pixel (LowX + x, LowY + y);
        index = aColorMap -> FindEntry (iPixel.Value ()).Index ();
        BPIXEL[y*myWidth + x] = index;
      }
    }
  }
}

//================================================================
void AlienImage_BMPAlienData::FromColorImage (
                const Handle(Image_ColorImage)& anImage)
{
  int width  = anImage -> Width  ();
  int height = anImage -> Height ();
  int x, y, LowX = anImage -> LowerX(), LowY = anImage -> LowerY();
  Standard_Real r, g, b;
  Quantity_Color color;
  RGBQUAD rgbColor;
  DWORD* pdwPixel = (DWORD*)&rgbColor;

  if (width*height > 0) {
    // Clear old data if any
    Clear ();
    // Fill image's data
    myWidth = width;
    myHeight = height;
    myData = STGMGR_ALLOC (myWidth*myHeight*4);
    for (y = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        color = anImage -> PixelColor (LowX + x, LowY + y);
        color.Values (r, g, b, Quantity_TOC_RGB);
        rgbColor.rgbRed      = (int)(r*255.);
        rgbColor.rgbGreen    = (int)(g*255.);
        rgbColor.rgbBlue     = (int)(b*255.);
        rgbColor.rgbReserved = 0;
        DWPIXEL[y*myWidth + x] = _TestSwapDWORD (*pdwPixel);
      }
    }
  }
}
 
