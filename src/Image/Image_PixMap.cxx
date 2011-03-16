// File       Image_PixMap.cxx
// Created    16 September 2010
// Author     KGV
// Copyright  OpenCASCADE 2010

#ifdef HAVE_FREEIMAGE
  #include <FreeImagePlus.h>
  #include <Image_PixMap.ixx>
  /* OCC22216 NOTE: linker dependency can be switched off by undefining macro */ 
  #ifdef _MSC_VER
  #pragma comment( lib, "FreeImage.lib" )
  #pragma comment( lib, "FreeImagePlus.lib" )
  #endif
#else
  #include <Image_PixMap.ixx>
  #include <fstream>

  #if (defined(BYTE_ORDER)   && BYTE_ORDER==BIG_ENDIAN) || \
      (defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || \
       defined(__BIG_ENDIAN__)
    #define THE_BIGENDIAN
  #endif

  // dummy class which can only dump to PPM format
  class fipImage
  {
  public:

    typedef struct tagRGBQuad {
    #ifndef THE_BIGENDIAN
      Standard_Byte rgbBlue;
      Standard_Byte rgbGreen;
      Standard_Byte rgbRed;
    #else
      Standard_Byte rgbRed;
      Standard_Byte rgbGreen;
      Standard_Byte rgbBlue;
    #endif
      Standard_Byte rgbReserved;
    } RGBQuad_t;

  public:

    fipImage()
    : myDataPtr(NULL),
      mySizeX(0),
      mySizeY(0),
      myBytesPerLine(0),
      myBytesPerPixel(3)
    {
      //
    }

    fipImage (const Standard_Integer theSizeX, const Standard_Integer theSizeY,
              const Standard_Integer theBytesPerLine = 0, const Standard_Integer theBytesPerPixel = 3)
    : myDataPtr(NULL),
      mySizeX (theSizeX),
      mySizeY (theSizeY),
      myBytesPerLine (theBytesPerLine),
      myBytesPerPixel (theBytesPerPixel)
    {
      if (myBytesPerLine == 0)
      {
        myBytesPerLine = mySizeX * myBytesPerPixel;
      }
      myDataPtr = new Standard_Byte[myBytesPerLine * mySizeY];
    }

    ~fipImage()
    {
      delete[] myDataPtr;
    }

    Standard_Integer getHeight() const
    {
      return mySizeY;
    }

    Standard_Integer getWidth() const
    {
      return mySizeX;
    }

    Standard_Integer getBytesPerPixel() const
    {
      return myBytesPerPixel;
    }

    Standard_Integer getBytesPerLine() const
    {
      return myBytesPerLine;
    }

    Standard_Byte* getData() const
    {
      return myDataPtr;
    }

    Standard_Byte* getScanLine (const Standard_Integer theRow) const
    {
      return &myDataPtr[theRow * myBytesPerLine];
    }

    Quantity_Color getPixelColor (const Standard_Integer theCol,
                                  const Standard_Integer theRow) const
    {
      RGBQuad_t* aPixel = (RGBQuad_t* )&getScanLine (theRow)[theCol * myBytesPerPixel];
      return Quantity_Color (Standard_Real (aPixel->rgbRed)   / 255.0,
                             Standard_Real (aPixel->rgbGreen) / 255.0,
                             Standard_Real (aPixel->rgbBlue)  / 255.0,
                             Quantity_TOC_RGB);
    }

    Standard_Boolean savePPM (const Standard_CString theFileName) const
    {
      // Open file
      FILE* pFile = fopen (theFileName, "wb");
      if (pFile == NULL)
      {
        return Standard_False;
      }

      // Write header
      fprintf (pFile, "P6\n%d %d\n255\n", mySizeX, mySizeY);

      // Write pixel data
      Standard_Byte* aScanLine;
      RGBQuad_t* aPixel;
      // image stored upside-down
      for (Standard_Integer aRow = mySizeY - 1; aRow >= 0; --aRow)
      {
        aScanLine = getScanLine (aRow);
        for (Standard_Integer aCol = 0; aCol < mySizeX; ++aCol)
        {
          aPixel = (RGBQuad_t* )&aScanLine[aCol * myBytesPerPixel];
          fwrite (&aPixel->rgbRed,   1, 1, pFile);
          fwrite (&aPixel->rgbGreen, 1, 1, pFile);
          fwrite (&aPixel->rgbBlue,  1, 1, pFile);
        }
      }

      // Close file
      fclose (pFile);
      return Standard_True;
    }

    Standard_Integer getMaxRowAligmentBytes() const
    {
      Standard_Integer aDeltaBytes = myBytesPerLine - myBytesPerPixel * mySizeX;
      for (Standard_Integer anAligment = 16; anAligment > 1; anAligment /= 2)
      {
        if (isRowAlignedTo (anAligment, aDeltaBytes))
        {
          return anAligment;
        }
      }
      return 1;
    }

  private:

    Standard_Boolean isRowAlignedTo (const Standard_Integer theAligmentBytes,
                                     const Standard_Integer theDeltaBytes) const
    {
      return (theDeltaBytes < theAligmentBytes) &&
             ((myBytesPerLine % theAligmentBytes) == 0);
    }

  private:

    Standard_Byte* myDataPtr;
    Standard_Integer mySizeX;
    Standard_Integer mySizeY;
    Standard_Integer myBytesPerLine;
    Standard_Integer myBytesPerPixel;

  };
#endif

#include <gp.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : Image_PixMap
//purpose  :
//=======================================================================
Image_PixMap::Image_PixMap (const Standard_Integer theWidth,
                            const Standard_Integer theHeight,
                            const Image_TypeOfImage theType)
:	Aspect_PixMap (theWidth, theHeight, 1),
	myImage()
{
#ifdef HAVE_FREEIMAGE
  FREE_IMAGE_TYPE aFIType = FIT_UNKNOWN;
  int aBitsPerPixel = 0;
  switch (theType)
  {
    case Image_TOI_RGBA:
      aFIType = FIT_BITMAP;
      aBitsPerPixel = 32;
      break;
    case Image_TOI_RGBF:
      aFIType = FIT_RGBF;
      aBitsPerPixel = 96;
      break;
    case Image_TOI_RGBAF:
      aFIType = FIT_RGBAF;
      aBitsPerPixel = 128;
      break;
    case Image_TOI_FLOAT:
      aFIType = FIT_FLOAT;
      aBitsPerPixel = 32;
      break;
    case Image_TOI_RGB:
    default:
      aFIType = FIT_BITMAP;
      aBitsPerPixel = 24;
      break;
  }
  myImage = new fipImage (aFIType, theWidth, theHeight, aBitsPerPixel);
#else
  Standard_Integer aBytesPerPixel = 0;
  switch (theType)
  {
    case Image_TOI_RGBAF:
      std::cerr << "Float formats not supported\n";
    case Image_TOI_RGBA:
      aBytesPerPixel = 4;
      break;
    case Image_TOI_RGBF:
    case Image_TOI_FLOAT:
      std::cerr << "Float formats not supported\n";
    case Image_TOI_RGB:
    default:
      aBytesPerPixel = 3;
      break;
  }
  myImage = new fipImage (theWidth, theHeight, 0, aBytesPerPixel);
  //
#endif
}

//=======================================================================
//function : Image_PixMap
//purpose  :
//=======================================================================
Image_PixMap::Image_PixMap (const Standard_PByte theDataPtr,
                            const Standard_Integer theWidth, const Standard_Integer theHeight,
                            const Standard_Integer thePitch, const Standard_Integer theBitsPerPixel,
                            const Standard_Boolean theIsTopDown)
:	Aspect_PixMap (theWidth, theHeight, 1),
	myImage (new fipImage())
{
#ifdef HAVE_FREEIMAGE
  *myImage = FreeImage_ConvertFromRawBits (theDataPtr,
                                           theWidth, theHeight,
                                           thePitch, theBitsPerPixel,
                                           0, 0, 0,
                                           theIsTopDown);
  if (theBitsPerPixel != 24)
  {
    myImage->convertTo24Bits();
  }
#else
  myImage = new fipImage (theWidth, theHeight, thePitch, theBitsPerPixel / 8);
  Standard_Integer aRowStart = !theIsTopDown ? 0 : (theHeight - 1);
  Standard_Integer aRowDelta = !theIsTopDown ? 1 : -1;
  for (Standard_Integer aRowFrom (aRowStart), aRowTo (0);
       aRowFrom >= 0 && aRowFrom < theHeight;
       aRowFrom += aRowDelta, ++aRowTo)
  {
    memcpy (myImage->getScanLine (aRowTo),
            &theDataPtr[aRowFrom * thePitch],
            myImage->getBytesPerLine());
  }
#endif
}

//=======================================================================
//function : Destroy
//purpose  :
//=======================================================================
void Image_PixMap::Destroy()
{
  myImage = Image_HPrivateImage();
}

//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
Standard_Boolean Image_PixMap::Dump (const Standard_CString theFilename,
                                     const Standard_Real theGammaCorr) const
{
#ifdef HAVE_FREEIMAGE
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename (theFilename);
  if (anImageFormat == FIF_UNKNOWN)
  {
    std::cerr << "Image_PixMap, image format doesn't supported!\n";
    return Standard_False;
  }

  Standard_Boolean isCopied = Standard_False;
  Image_HPrivateImage anImageToDump = myImage;
  if (Abs (theGammaCorr - 1.0) > gp::Resolution())
  {
    if (!isCopied)
    {
      isCopied = Standard_True;
      anImageToDump = new fipImage (*myImage);
    }
    anImageToDump->adjustGamma (theGammaCorr);
  }

  switch (anImageFormat)
  {
    case FIF_GIF:
      if (!isCopied)
      {
        isCopied = Standard_True;
        anImageToDump = new fipImage (*myImage);
      }
      // need convertion to image with pallete
      anImageToDump->colorQuantize (FIQ_NNQUANT);
      break;
    case FIF_EXR:
      if (myImage->getImageType() == FIT_BITMAP)
      {
        if (!isCopied)
        {
          isCopied = Standard_True;
          anImageToDump = new fipImage (*myImage);
        }
        anImageToDump->convertToType (FIT_RGBF);
      }
      break;
    default:
      if (myImage->getImageType() != FIT_BITMAP)
      {
        if (!isCopied)
        {
          isCopied = Standard_True;
          anImageToDump = new fipImage (*myImage);
        }
        anImageToDump->convertToType (FIT_BITMAP);
      }
      break;
  }
  return anImageToDump->save (theFilename);
#else
  return myImage->savePPM (theFilename);
#endif
}

Aspect_Handle Image_PixMap::PixmapID() const
{
  return Aspect_Handle();
}

void Image_PixMap::AccessBuffer (Image_CRawBufferData& theBuffer) const
{
  theBuffer.widthPx  = myImage->getWidth();
  theBuffer.heightPx = myImage->getHeight();
#ifdef HAVE_FREEIMAGE
  theBuffer.rowAligmentBytes = 4; // 32 bits according to FreeImage documentation
  switch (myImage->getImageType())
  {
    case FIT_FLOAT:
      theBuffer.format = TDepthComponent;
      theBuffer.type = TFloat;
      break;
    case FIT_RGBF:
      theBuffer.format = TRGB;
      theBuffer.type = TFloat;
      break;
    case FIT_RGBAF:
      theBuffer.format = TRGBA;
      theBuffer.type = TFloat;
      break;
    case FIT_BITMAP:
    default:
    #if defined(FREEIMAGE_BIGENDIAN)
      theBuffer.format = (myImage->getColorType() == FIC_RGBALPHA) ? TRGBA : TRGB;
    #else
      theBuffer.format = (myImage->getColorType() == FIC_RGBALPHA) ? TBGRA : TBGR;
    #endif
      theBuffer.type = TUByte;
      break;
  }
  theBuffer.dataPtr = myImage->accessPixels();
#else
  theBuffer.rowAligmentBytes = myImage->getMaxRowAligmentBytes();
  theBuffer.format = (myImage->getBytesPerPixel() == 4) ? TBGRA : TBGR;
  theBuffer.type = TUByte;
  theBuffer.dataPtr = myImage->getData();
#endif
}

Quantity_Color Image_PixMap::PixelColor (const Standard_Integer theX,
                                         const Standard_Integer theY) const
{
  Standard_Integer aScanlineId = myImage->getHeight() - theY - 1;
  if (theX < 0 || theX >= (unsigned int)myImage->getWidth() ||
      theY < 0 || theY >= (unsigned int)myImage->getHeight())
  {
    return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
  }
#ifdef HAVE_FREEIMAGE
  else if (myImage->getImageType() == FIT_BITMAP)
  {
    RGBQUAD aValue; memset (&aValue, 0, sizeof(aValue));
    myImage->getPixelColor (theX, aScanlineId, &aValue);
    return Quantity_Color (Standard_Real (aValue.rgbRed)   / 255.0,
                           Standard_Real (aValue.rgbGreen) / 255.0,
                           Standard_Real (aValue.rgbBlue)  / 255.0,
                           Quantity_TOC_RGB);
  }
  else
  {
    switch (myImage->getImageType())
    {
      case FIT_FLOAT:
      {
        float* aScanLine = (float* )myImage->getScanLine (aScanlineId);
        Quantity_Parameter aValue = Quantity_Parameter (aScanLine[theX]);
        return Quantity_Color (aValue, aValue, aValue, Quantity_TOC_RGB);
      }
      case FIT_RGBF:
      {
        FIRGBF* aScanLine = (FIRGBF* )myImage->getScanLine (aScanlineId);
        FIRGBF* aPixel = &aScanLine[theX];
        return Quantity_Color (Quantity_Parameter (aPixel->red),
                               Quantity_Parameter (aPixel->green),
                               Quantity_Parameter (aPixel->blue),
                               Quantity_TOC_RGB);
      }
      case FIT_RGBAF:
      {
        FIRGBAF* aScanLine = (FIRGBAF* )myImage->getScanLine (aScanlineId);
        FIRGBAF* aPixel = &aScanLine[theX];
        return Quantity_Color (Quantity_Parameter (aPixel->red),
                               Quantity_Parameter (aPixel->green),
                               Quantity_Parameter (aPixel->blue),
                               Quantity_TOC_RGB);
      }
      default:
      {
        // not supported image type
        return Quantity_Color (0.0, 0.0, 0.0, Quantity_TOC_RGB);
      }
    }
  }
#else
  return myImage->getPixelColor (theX, aScanlineId);
#endif
}
