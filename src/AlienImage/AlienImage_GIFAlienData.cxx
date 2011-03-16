// File:        AlienImage_GIFAlienData.cxx
// Created:     Quebex 20 October 1998
// Author:      DCB
// Notes:
//    Read()/Write() code is taken from ZOV's sources from Xw package.
//    _convert24to8() sources and all concerned code is taken from
//      EUG's code of WNT_ImageProcessor.
// Copyright:   MatraDatavision 1998

#define TEST	//GG_140699
//		Check GIF format first.

#ifdef WNT
//it is important to undefine NOGDI and enforce including <windows.h> before
//Standard_Macro.hxx defines it and includes <windows.h> making GDI-related
//stuff unavailable and causing compilation errors
#undef NOGDI
#include <windows.h>
#endif

#include <AlienImage_GIFAlienData.ixx>
#include <AlienImage_GIFLZWDict.hxx>

#include <Aspect_GenericColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Image_Convertor.hxx>
#include <Image_DitheringMethod.hxx>

#ifdef _DEBUG
//# define TRACE
#endif

#ifdef TRACE
# include <OSD_Timer.hxx>
#endif

#define STGMGR_ALLOC(size)    Standard::Allocate(size)
#define STGMGR_FREE(buf,size) Standard::Free((void*&)buf)
#define COLORS_SIZE           256*sizeof(BYTE)
#define RED                   ((PBYTE)myRedColors)
#define GREEN                 ((PBYTE)myGreenColors)
#define BLUE                  ((PBYTE)myBlueColors)
#define PIXEL                 ((PBYTE)myData)

#define ZERO_COLORS()                     \
  memset (myRedColors,   0, COLORS_SIZE); \
  memset (myGreenColors, 0, COLORS_SIZE); \
  memset (myBlueColors,  0, COLORS_SIZE);

#define _ADD_PIXEL(idx)                                     \
{                                                           \
  if (y < height)                                           \
   *((PBYTE)myData + y*width + x) = ((BYTE)(idx));          \
  if (++x == width) {                                       \
    x = 0;                                                  \
    if (!isInterlace)  ++y;                                 \
    else  switch (pass) {                                   \
    case 0:  y += 8; if (y >= height) ++pass, y = 4; break; \
    case 1:  y += 8; if (y >= height) ++pass, y = 2; break; \
    case 2:  y += 4; if (y >= height) ++pass, y = 1; break; \
    default: y += 2;                                        \
    }                                                       \
  }                                                         \
}

//================================================================
AlienImage_GIFAlienData::AlienImage_GIFAlienData()
                        : AlienImage_AlienImageData ()
{
  myRedColors      = NULL;
  myGreenColors    = NULL;
  myBlueColors     = NULL;

  myData   = NULL;
  myWidth  = 0;
  myHeight = 0;
}

//================================================================
void AlienImage_GIFAlienData::Clear ()
{
  if (myRedColors) {
    STGMGR_FREE (myRedColors, COLORS_SIZE);
    myRedColors = NULL;
  }
  if (myGreenColors) {
    STGMGR_FREE (myGreenColors, COLORS_SIZE);
    myGreenColors = NULL;
  }
  if (myBlueColors) {
    STGMGR_FREE (myBlueColors, COLORS_SIZE);
    myBlueColors = NULL;
  }

  if (myData) {
    STGMGR_FREE (myData, myWidth*myHeight);
    myData = NULL;
    myWidth = myHeight = 0;
  }
}

//================================================================
Standard_Boolean AlienImage_GIFAlienData::Read (OSD_File& file)
{
  Standard_Integer nFileSize = file.Size(), nReadCount;
  unsigned *OutCode=NULL, *Prefix=NULL, *Suffix=NULL,
    BitMask, CodeSize, ClearCode, EOFCode, FreeCode, x, y, pass, width, height,
    InitCodeSize, MaxCode, ReadMask, FirstFree, OutCount, BitOffset,
    ByteOffset, Code, CurCode, OldCode=0, FinChar=0, InCode;
  int isInterlace, hasColormap;
  int i, ncolors;
  BYTE byte, byte1;
  PBYTE pFileStream = NULL;
  // Allocate memory to read the file
  PBYTE pFileBuffer =
    (PBYTE) STGMGR_ALLOC (nFileSize);
  PBYTE rasterPtr = NULL, ptr1;
  pFileStream = pFileBuffer + 10;

#ifdef TRACE
  OSD_Timer timer;
  cout << "AlienImage_GIFAlienData::Read () starts" << endl << flush;
  timer.Start ();
#endif

  // Read file into memory
  file.Read ((void*&)pFileBuffer, nFileSize, nReadCount);
  if (nFileSize != nReadCount) {
    cout << "GIFAlienData::Read() : BAD file size." << endl << flush;
    goto _ExitReadError;
  }

#ifdef TEST
  if( strncmp ((char*)pFileBuffer, "GIF87a", 6) &&
		strncmp ((char*)pFileBuffer, "GIF89a", 6) )
    goto _ExitReadError;
#endif

  // Determine if the image has colormap
  byte        = *pFileStream++; // Color descriptor byte (M#CR0#BP)
  hasColormap = byte & 0x80;
  ncolors     = hasColormap ? 1<<((byte & 0x07) + 1): 1<<8;
  BitMask     = ncolors - 1;

  pFileStream += 2; // Skip background byte and following zero byte
  if (ncolors > 0 && ncolors <= 256) { // Allocate Color Table entries
    myRedColors   = STGMGR_ALLOC (COLORS_SIZE);
    myGreenColors = STGMGR_ALLOC (COLORS_SIZE);
    myBlueColors  = STGMGR_ALLOC (COLORS_SIZE);
    ZERO_COLORS ();

    for (i=0; i<ncolors; i++) {
      // Fill in array of XColor's used
      // Note, that if there's no colormap specified then I use gray scale
      RED  [i] = ((BYTE)(hasColormap ? *pFileStream++: i));
      GREEN[i] = ((BYTE)(hasColormap ? *pFileStream++: i));
      BLUE [i] = ((BYTE)(hasColormap ? *pFileStream++: i));
    }
  } else {
    cout << "GIFAlienData::Read() : There's no colormap"
         << " in the image (or too big): " << ncolors << endl << flush;
    goto _ExitReadError;
  }

  // Skip extension blocks if any.
  // Format:  <'!'><size><...><size><...><0>
  while (*pFileStream == '!') {
    pFileStream += 2; // skip code byte followed '!' sign
    while (*pFileStream)
      pFileStream += (unsigned)(1 + *(PBYTE)pFileStream);
    pFileStream ++;
  }

  if (*pFileStream++ != ',') { // must be an image descriptor
    cout << "GIFAlienData::Read() : There's no separator"
         << " following the colormap" << endl << flush;
    goto _ExitReadError;
  }

  pFileStream += 2*2; // Skip image left & top offsets
  width   = (unsigned) *pFileStream++;
  width  += ((unsigned)*pFileStream++) << 8;
  height  = (unsigned) *pFileStream++;
  height += ((unsigned)*pFileStream++) << 8;

  byte = *pFileStream++;
  isInterlace = byte & 0x40;
  if (byte & 0x80) {
    cout << "GIFAlienData::Read() : Can't read GIF image"
         << " with locally defined colormap" << endl << flush;
    goto _ExitReadError;
  }

  // Allocate the pixel buffer
  rasterPtr = (PBYTE) STGMGR_ALLOC (nFileSize);

  OutCode = (unsigned *) STGMGR_ALLOC (1025 * sizeof (unsigned int));
  Prefix  = (unsigned *) STGMGR_ALLOC (4096 * sizeof (unsigned int));
  Suffix  = (unsigned *) STGMGR_ALLOC (4096 * sizeof (unsigned int));

  // Decode compressed raster data.
  CodeSize  = *pFileStream++;
  ClearCode = 1 << CodeSize;
  EOFCode   = ClearCode + 1;
  FreeCode  = FirstFree = EOFCode + 1;

  ++CodeSize;
  InitCodeSize = CodeSize;
  MaxCode      = 1 << CodeSize;
  ReadMask     = MaxCode - 1;
  ptr1         = rasterPtr;

  // Read encoded data to a continuous array pointed to by rasterPtr
  do {
    byte = byte1 = *pFileStream++;
    while (byte--)
      *ptr1++ = *pFileStream++;
    if ((long) (ptr1 - rasterPtr) > nFileSize) {// Currupt file
      cout << "GIFAlienData::Read() : BAD file size." << endl << flush;
      goto _ExitReadError;
    }
  } while (byte1);
  // The file data has been already read
  STGMGR_FREE (pFileBuffer, nFileSize);
  pFileBuffer = NULL;

  // Allocate image data
  myWidth  = width;
  myHeight = height;
  myData = STGMGR_ALLOC (myWidth*myHeight);

  x = y = pass = OutCount = BitOffset = ByteOffset = 0;
  // Fetch the next code (3 to 12 bits) from the raster data stream
  Code = rasterPtr[0] + (((unsigned) rasterPtr[1]) << 8);
  if (CodeSize >= 8)
    Code += ((unsigned) rasterPtr[2]) << 16;

  Code >>= BitOffset & 0x7;
  BitOffset += CodeSize;
  Code      &= ReadMask;

  while (Code != EOFCode) {
    if (Code == ClearCode) {
      // Read the next code
      CodeSize   = InitCodeSize;
      MaxCode    = 1 << CodeSize;
      ReadMask   = MaxCode - 1;
      FreeCode   = FirstFree;
      ByteOffset = BitOffset >> 3;
      Code       = rasterPtr[ByteOffset]
                   + (((unsigned) rasterPtr[ByteOffset + 1]) << 8);

      if (CodeSize >= 8)
        Code += ((unsigned) rasterPtr[ByteOffset + 2]) << 16;

      Code      >>= BitOffset & 0x7;
      BitOffset +=  CodeSize;
      Code      &=  ReadMask;

      CurCode = OldCode = Code;
      FinChar = CurCode & BitMask;

      _ADD_PIXEL (FinChar);
    } else {
      CurCode = InCode = Code;
      if (CurCode >= FreeCode) {
        CurCode = OldCode;
        OutCode[OutCount++] = FinChar;
      }
      while (CurCode > BitMask) {
        if (OutCount > 1024) {
          cout << "GIFAlienData::Read() : BAD file size." << endl << flush;
          goto _ExitReadError;
	}
        OutCode [OutCount++] = Suffix [CurCode];
        CurCode = Prefix [CurCode];
      }
      FinChar = CurCode & BitMask;
      OutCode [OutCount++] = FinChar;
      for (i = OutCount - 1; i >= 0; --i)
        _ADD_PIXEL (OutCode [i]);

      OutCount = 0;
      Prefix [FreeCode] = OldCode;
      Suffix [FreeCode] = FinChar;
      OldCode = InCode;
      ++FreeCode;
      if (FreeCode >= MaxCode) {
        if (CodeSize < 12) {
          ++CodeSize;
          MaxCode <<= 1;
          ReadMask = (1 << CodeSize) - 1;
        }
      }
    }

    ByteOffset = BitOffset >> 3;
    Code = (unsigned) rasterPtr[ByteOffset]
           + (((unsigned) rasterPtr[ByteOffset + 1]) << 8);

    if (CodeSize >= 8)
      Code += ((unsigned) rasterPtr[ByteOffset + 2]) << 16;

    Code >>= (BitOffset & 0x7);
    BitOffset += CodeSize;
    Code &=  ReadMask;
  } // while (Code != EOFCode)

  // Free allocated memory
  STGMGR_FREE (rasterPtr, nFileSize);
  STGMGR_FREE (OutCode, 1025 * sizeof (unsigned int));
  STGMGR_FREE (Prefix,  4096 * sizeof (unsigned int));
  STGMGR_FREE (Suffix,  4096 * sizeof (unsigned int));
#ifdef TRACE
  timer.Stop ();
  timer.Show (cout);
  cout << "AlienImage_GIFAlienData::Read () finished\n" << endl << flush;
#endif
  return Standard_True;

_ExitReadError:
//  cout << "GIFAlienData::Read() : Read file error." << endl << flush;
  if (pFileBuffer)  STGMGR_FREE (pFileBuffer, nFileSize);
  if (OutCode)      STGMGR_FREE (OutCode, 1025 * sizeof (unsigned int));
  if (Prefix)       STGMGR_FREE (Prefix,  4096 * sizeof (unsigned int));
  if (Suffix)       STGMGR_FREE (Suffix,  4096 * sizeof (unsigned int));
  if (myRedColors) {
    STGMGR_FREE (myRedColors, COLORS_SIZE);
    myRedColors = NULL;
  }
  if (myGreenColors) {
    STGMGR_FREE (myGreenColors, COLORS_SIZE);
    myGreenColors = NULL;
  }
  if (myBlueColors) {
    STGMGR_FREE (myBlueColors, COLORS_SIZE);
    myBlueColors = NULL;
  }
  if (myData) {
    STGMGR_FREE (myData, myWidth*myHeight);
    myData = NULL;
    myWidth = myHeight = 0;
  }

  return Standard_False;
}

//================================================================
Standard_Boolean AlienImage_GIFAlienData::Write (OSD_File& file) const
{
  SCREEN_DESCR sd;
  IMAGE_DESCR  id;
  BYTE image_sep = 0x2C; // gif colormap delimiter
  WORD wZero = 0x00;
  BYTE colors256 [256][3];
  BYTE bEnd  = 0x3B;
  int i;

#ifdef TRACE
  OSD_Timer timer;
  cout << "AlienImage_GIFAlienData::Write () starts" << endl << flush;
  timer.Start ();
#endif
  // Check if image is loaded
  if (myData == NULL || myRedColors == NULL ||
      myGreenColors == NULL || myBlueColors == NULL ||
      myWidth == 0 || myHeight == 0)
    goto _ExitWriteError;

  // Build file header
  memcpy (sd.gifID, "GIF87a", 6);
  sd.scrnWidth  = SWAP_WORD ((WORD) myWidth );
  sd.scrnHeight = SWAP_WORD ((WORD) myHeight);
  sd.scrnFlag   = 0x80 | (  ( 7/*[=depth-1]*/ << 4 ) & 0x70  ) | 0x07;
  id.imgX       = 0;
  id.imgY       = 0;
  id.imgWidth   = SWAP_WORD ((WORD) myWidth );
  id.imgHeight  = SWAP_WORD ((WORD) myHeight);
//          imgFlag
//        +-+-+-+-+-+-----+       M=0 - Use global color map, ignore 'pixel'
//        |M|I|0|0|0|pixel| 10    M=1 - Local color map follows, use 'pixel'
//        +-+-+-+-+-+-----+       I=0 - Image formatted in Sequential order
//                                I=1 - Image formatted in Interlaced order
//                                pixel+1 - # bits per pixel for this image
  id.imgFlag   = 0x07; // Global color map, Sequential order, 8 bits per pixel

  for (i = 0; i < 256; i++) {
    colors256[i] [0/*R*/] = RED   [i];
    colors256[i] [1/*G*/] = GREEN [i];
    colors256[i] [2/*B*/] = BLUE  [i];
  }

  // Write off the buffers
  file.Write (&sd, 11); // Screen descriptor
  if (file.Failed())
    goto _ExitWriteError;

  file.Write (&wZero, 2); // Zero word
  if (file.Failed())
    goto _ExitWriteError;

  file.Write (colors256, 256*3); // Colormap
  if (file.Failed())
    goto _ExitWriteError;

  file.Write (&image_sep, 1); // Separator
  if (file.Failed())
    goto _ExitWriteError;

  file.Write (&id, 9); // Image descriptor
  if (file.Failed())
    goto _ExitWriteError;

  // Write off the image data
  if (!_lzw_encode (file, (PBYTE) myData, myWidth, myHeight, myWidth))
    goto _ExitWriteError;

  file.Write (&bEnd, 1); // End of image
  if (file.Failed())
    goto _ExitWriteError;

  // Return SUCCESS status if there were no errors.
#ifdef TRACE
  timer.Stop ();
  timer.Show (cout);
  cout << "AlienImage_GIFAlienData::Write () finished\n" << endl << flush;
#endif
  return Standard_True;

  // Exit on error
_ExitWriteError:
  file.Seek (0, OSD_FromBeginning);
  return Standard_False;
}

//================================================================
Handle_Image_Image AlienImage_GIFAlienData::ToImage () const
{
#ifdef TRACE
  OSD_Timer timer;
  cout << "AlienImage_GIFAlienData::ToImage () starts" << endl << flush;
  timer.Start ();
#endif
  Standard_Integer i, x, y, LowX, LowY;
  Standard_Real r, g, b;
  Aspect_ColorMapEntry entry;
  Aspect_IndexPixel index;
  Quantity_Color color;

  // Build colormap
  Handle(Aspect_GenericColorMap) aColorMap =
    new Aspect_GenericColorMap ();
  for (i = 0; i < 256; i++) {
    r = ((float)RED   [i] / 255.);
    g = ((float)GREEN [i] / 255.);
    b = ((float)BLUE  [i] / 255.);
    color.SetValues (r, g, b, Quantity_TOC_RGB);
    entry.SetValue  (i, color);
    aColorMap -> AddEntry (entry);
  }

  // Fill image data
  Handle(Image_Image) theImage =
    new Image_PseudoColorImage (0, 0, myWidth, myHeight, aColorMap);
  LowX = theImage -> LowerX ();
  LowY = theImage -> LowerY ();
  for (y = 0; y < myHeight; y++) {
    for (x = 0; x < myWidth; x++) {
      index.SetValue (PIXEL[y*myWidth + x]);
      theImage -> SetPixel (LowX + x, LowY + y, index);
    }
  }
#ifdef TRACE
  timer.Stop ();
  timer.Show (cout);
  cout << "AlienImage_GIFAlienData::ToImage () finished\n" << endl << flush;
#endif
  return theImage;
}

//================================================================
void AlienImage_GIFAlienData::FromImage (const Handle_Image_Image& anImage)
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
      "Attempt to convert a unknown Image_Image type to a GIFAlienData");
  }
}

//================================================================
void AlienImage_GIFAlienData::FromPseudoColorImage (
                const Handle(Image_PseudoColorImage)& anImage)
{
  int width  = anImage -> Width  ();
  int height = anImage -> Height ();
  unsigned short x, y, i;
  Standard_Real r, g, b;
  Aspect_ColorMapEntry entry;
  Aspect_IndexPixel index;
  Quantity_Color color;
  Standard_Integer LowX = anImage -> LowerX();
  Standard_Integer LowY = anImage -> LowerY();
  BYTE ei;

#ifdef TRACE
  OSD_Timer timer;
  cout << "AlienImage_GIFAlienData::FromPseudoColorImage () starts" << endl << flush;
  timer.Start ();
#endif
  if (width*height > 0) {
    Handle(Aspect_ColorMap) aColorMap = anImage -> ColorMap ();
    // Clear old values if any
    Clear ();

    myRedColors   = STGMGR_ALLOC (COLORS_SIZE);
    myGreenColors = STGMGR_ALLOC (COLORS_SIZE);
    myBlueColors  = STGMGR_ALLOC (COLORS_SIZE);
    ZERO_COLORS ();

    // Build colors from colormap
    for (i = 1; i <= aColorMap -> Size (); i++) {
      entry = aColorMap -> Entry (i);
      ei = entry.Index ();
      color = entry.Color ();
      color.Values (r, g, b, Quantity_TOC_RGB);
      RED  [ei] = (BYTE)(r*255.);
      GREEN[ei] = (BYTE)(g*255.);
      BLUE [ei] = (BYTE)(b*255.);
    }

    // Build imagedata from Image_PseudoColorImage
    myWidth = width;
    myHeight = height;
    myData = STGMGR_ALLOC (myWidth*myHeight);
    for (y = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        index = anImage -> Pixel (LowX + x, LowY + y);
        PIXEL[y*myWidth + x] = (BYTE)index.Value ();
      }
    }
  }
#ifdef TRACE
  timer.Stop ();
  timer.Show (cout);
  cout << "AlienImage_GIFAlienData::FromPseudoColorImage () finished\n" << endl << flush;
#endif
}

//================================================================
void AlienImage_GIFAlienData::FromColorImage (
                const Handle(Image_ColorImage)& anImage)
{
#ifdef TRACE
  OSD_Timer timer;
  cout << "AlienImage_GIFAlienData::FromColorImage () starts" << endl << flush;
  timer.Start ();
#endif // TRACE

  int width  = anImage -> Width  ();
  int height = anImage -> Height ();
  int i, x, y, LowX = anImage -> LowerX(), LowY = anImage -> LowerY();
  Quantity_Color color;
  Standard_Real r, g, b;

  if (width*height > 0) {
    Aspect_ColorMapEntry entry;
    // Clear old values if any
    Clear ();
    myWidth  = width;
    myHeight = height;
    LPRGBQUAD pColors = (LPRGBQUAD) STGMGR_ALLOC (256*sizeof(RGBQUAD));
    PBYTE     pBits24 = (PBYTE)     STGMGR_ALLOC (width*height*3);
    memset (pColors, 0, 256*sizeof(RGBQUAD));

    myData        = STGMGR_ALLOC (width*height);
    myRedColors   = STGMGR_ALLOC (COLORS_SIZE);
    myGreenColors = STGMGR_ALLOC (COLORS_SIZE);
    myBlueColors  = STGMGR_ALLOC (COLORS_SIZE);

    for (y = 0, i = 0; y < myHeight; y++) {
      for (x = 0; x < myWidth; x++) {
        color = anImage -> PixelColor (LowX + x, LowY + y);
        color.Values (r, g, b, Quantity_TOC_RGB);
        pBits24 [i + 0] = (BYTE)(b*255.);
        pBits24 [i + 1] = (BYTE)(g*255.);
        pBits24 [i + 2] = (BYTE)(r*255.);
        i += 3;
      }
    }

    if (_convert24to8 (pColors, pBits24, (PBYTE)myData, myWidth, myHeight)) {
      Handle(Aspect_GenericColorMap) aColorMap = new Aspect_GenericColorMap ();
      for (i = 0; i < 256; i++) {
        r = ((float)pColors[i].rgbRed   / 255.);
        g = ((float)pColors[i].rgbGreen / 255.);
        b = ((float)pColors[i].rgbBlue  / 255.);
        color.SetValues (r, g, b, Quantity_TOC_RGB);
        entry.SetValue (i, color);
        aColorMap -> AddEntry (entry);
        RED  [i] = pColors[i].rgbRed;
        GREEN[i] = pColors[i].rgbGreen;
        BLUE [i] = pColors[i].rgbBlue;
      }
    } else {
      Image_Convertor aConvertor;
      aConvertor.SetDitheringMethod (Image_DM_ErrorDiffusion);
      Handle(Aspect_ColorMap) aColorMap  = anImage -> ChooseColorMap (256);
      Handle(Image_PseudoColorImage) aPImage =
        aConvertor.Convert (anImage, aColorMap);
      FromPseudoColorImage (aPImage);
    }

    STGMGR_FREE (pColors, 256*sizeof(RGBQUAD));
    STGMGR_FREE (pBits24, width*height*3);
  }

#ifdef TRACE
  timer.Stop ();
  timer.Show (cout);
  cout << "AlienImage_GIFAlienData::FromColorImage () finished\n" << endl << flush;
#endif // TRACE
}
 
