// Created on: 2007-04-15
// Created by: Alexey MORENOV & Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <OpenGl_AVIWriter.hxx>

#if (defined(_WIN32) || defined(__WIN32__)) && defined(HAVE_VIDEOCAPTURE)
  #ifdef _MSC_VER
    #pragma comment (lib, "vfw32.lib")
  #endif

OpenGl_AVIWriter* OpenGl_AVIWriter::MyAVIWriterInstance = 0L;

OpenGl_AVIWriter * OpenGl_AVIWriter::GetInstance()
{
  return MyAVIWriterInstance;
}

//=======================================================================
//function : OpenGl_AVIWriter
//purpose  : 
//=======================================================================

Standard_EXPORT OpenGl_AVIWriter::OpenGl_AVIWriter
                 (const char *     theFileName, 
                  DWORD            dwCodec /* = mmioFOURCC('M','P','G','4') */,
                  Standard_Integer theFrameRate /* = 25 */)
  : myhHeap             (0L),
    myhWindow           (0L),
    myhAviDC            (0L),
    mylpBits            (0L),
    mylSample           (0L),
    mypAviFile          (0L),
    mypAviStream        (0L),
    mypAviCompressedStream(0L),
    myFileName          (0L),
    myIsAllowRecord     (Standard_False),
    myAppendFuncSelector(1)         //0=Dummy       1=FirstTime     2=Usual
{
  ::AVIFileInit();
  if (theFileName != 0L && theFileName[0] != '\0') {

    const size_t aLen = strlen(theFileName) + 1;
    myFileName = new char [aLen];
    memcpy(myFileName, theFileName, aLen);
    myErrMsg = (Standard_CString)"Method Succeeded";

    pAppendFrame[0]= &OpenGl_AVIWriter::AppendDummy;
    pAppendFrame[1]= &OpenGl_AVIWriter::AppendFrameFirstTime;
    pAppendFrame[2]= &OpenGl_AVIWriter::AppendFrameUsual;

    pAppendFrameBits[0]=&OpenGl_AVIWriter::AppendDummyBits;
    pAppendFrameBits[1]=&OpenGl_AVIWriter::AppendFrameBitsFirstTime;
    pAppendFrameBits[2]=&OpenGl_AVIWriter::AppendFrameBitsUsual;

    MyAVIWriterInstance = this;

    ZeroMemory(&myAviStreamInfo,sizeof(AVISTREAMINFO));
    myAviStreamInfo.fccType             = streamtypeVIDEO;
    myAviStreamInfo.fccHandler          = dwCodec;
    myAviStreamInfo.dwScale             = 1;
    myAviStreamInfo.dwRate              = theFrameRate; // Frames Per Second;
    myAviStreamInfo.dwQuality           = 100;/*//-1;  // Default Quality*/

    ZeroMemory(&myAviCompressOptions,sizeof(AVICOMPRESSOPTIONS));
    myAviCompressOptions.fccType        = streamtypeVIDEO;
    myAviCompressOptions.fccHandler     = myAviStreamInfo.fccHandler;
    myAviCompressOptions.dwFlags        =
      AVICOMPRESSF_KEYFRAMES|AVICOMPRESSF_VALID|AVICOMPRESSF_DATARATE;
    myAviCompressOptions.dwKeyFrameEvery  = 1;
    myAviCompressOptions.dwBytesPerSecond = 125000;
    myAviCompressOptions.dwQuality      = 100;
  }
}

//=======================================================================
//function : ~OpenGl_AVIWriter
//purpose  : 
//=======================================================================

Standard_EXPORT OpenGl_AVIWriter::~OpenGl_AVIWriter(void)
{
  ReleaseMemory();
  AVIFileExit();
  if (myFileName)
    delete [] myFileName;
  MyAVIWriterInstance = 0L;
}

//=======================================================================
//function : StartRecording
//purpose  : 
//=======================================================================

void OpenGl_AVIWriter::StartRecording(const HANDLE hWin)
{
  if (hWin != NULL)
    myhWindow = hWin;
  myIsAllowRecord = Standard_True;
}

//=======================================================================
//function : StopRecording
//purpose  : 
//=======================================================================

void OpenGl_AVIWriter::StopRecording()
{
  myIsAllowRecord = Standard_False;
}

//=======================================================================
//function : ReleaseMemory
//purpose  : 
//=======================================================================

void OpenGl_AVIWriter::ReleaseMemory()
{
  myAppendFuncSelector=0;      //Point to DummyFunction

  if(myhAviDC)
  {
    DeleteDC(myhAviDC);
    myhAviDC=NULL;
  }
  if(mypAviCompressedStream)
  {
    AVIStreamRelease(mypAviCompressedStream);
    mypAviCompressedStream=NULL;
  }
  if(mypAviStream)
  {
    AVIStreamRelease(mypAviStream);
    mypAviStream=NULL;
  }
  if(mypAviFile)
  {
    AVIFileRelease(mypAviFile);
    mypAviFile=NULL;
  }
  if(mylpBits)
  {
    HeapFree(myhHeap,HEAP_NO_SERIALIZE,mylpBits);
    mylpBits=NULL;
  }
  if(myhHeap)
  {
    HeapDestroy(myhHeap);
    myhHeap=NULL;
  }
}

//=======================================================================
//function : SetErrorMessage
//purpose  : 
//=======================================================================

void OpenGl_AVIWriter::SetErrorMessage(const char * theErrorMessage)
{
  myErrMsg = (Standard_CString)theErrorMessage;
}

//=======================================================================
//function : InitMovieCreation
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::InitMovieCreation (int nFrameWidth,
                                             int nFrameHeight,
                                             int nBitsPerPixel)
{
  int  nMaxWidth=GetSystemMetrics(SM_CXSCREEN),
       nMaxHeight=GetSystemMetrics(SM_CYSCREEN);

  myhAviDC = CreateCompatibleDC(NULL);
  if(myhAviDC==NULL)
  {
    SetErrorMessage("Unable to Create Compatible DC");
    return E_FAIL;
  }

  if (nFrameWidth > nMaxWidth)
    nMaxWidth= nFrameWidth;
  if (nFrameHeight > nMaxHeight)
    nMaxHeight = nFrameHeight;

  myhHeap=HeapCreate(HEAP_NO_SERIALIZE, nMaxWidth*nMaxHeight*4, 0);
  if(myhHeap==NULL)
  {
    SetErrorMessage("Unable to Create Heap");
    return E_FAIL;
  }

  mylpBits=HeapAlloc(myhHeap, HEAP_ZERO_MEMORY|HEAP_NO_SERIALIZE,
                     nMaxWidth*nMaxHeight*4);
  if(mylpBits==NULL)
  {
    SetErrorMessage("Unable to Allocate Memory on Heap");
    return E_FAIL;
  }

  HRESULT hr;
  hr = ::AVIFileOpen(&mypAviFile, myFileName, OF_CREATE|OF_WRITE, NULL);
  if (!hr == AVIERR_OK)
  {
    SetErrorMessage("Unable to Create the Movie File");
    return E_FAIL;
  }
  /*
  if(FAILED(::AVIFileOpen(&mypAviFile, myszFileName, OF_CREATE|OF_WRITE, NULL)))
  {
    SetErrorMessage("Unable to Create the Movie File");
    return E_FAIL;
  }
  */

  myAviStreamInfo.dwSuggestedBufferSize = nMaxWidth * nMaxHeight * 4;
  SetRect(&myAviStreamInfo.rcFrame, 0, 0, nFrameWidth, nFrameHeight);
  strncpy(myAviStreamInfo.szName, "Video Stream", 64);

  if(FAILED(AVIFileCreateStream(mypAviFile,&mypAviStream,&myAviStreamInfo)))
  {
    SetErrorMessage("Unable to Create Video Stream in the Movie File");
    return E_FAIL;
  }

  if(FAILED(AVIMakeCompressedStream(&mypAviCompressedStream,
                                    mypAviStream,
                                    &myAviCompressOptions,
                                    NULL)))
  {
    // One reason this error might occur is if you are using a Codec that is not
    // available on your system. Check the mmioFOURCC() code you are using and
    // make sure you have that codec installed properly on your machine.
    SetErrorMessage("Unable to Create Compressed Stream: "
                    "Check your CODEC options");
    return E_FAIL;
  }

  BITMAPINFO bmpInfo;
  ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
  bmpInfo.bmiHeader.biPlanes            = 1;
  bmpInfo.bmiHeader.biWidth             = nFrameWidth;
  bmpInfo.bmiHeader.biHeight            = nFrameHeight;
  bmpInfo.bmiHeader.biCompression       = BI_RGB;
  bmpInfo.bmiHeader.biBitCount          = nBitsPerPixel;
  bmpInfo.bmiHeader.biSize              = sizeof(BITMAPINFOHEADER);
  bmpInfo.bmiHeader.biSizeImage         = (bmpInfo.bmiHeader.biWidth *
                                           bmpInfo.bmiHeader.biHeight*
                                           bmpInfo.bmiHeader.biBitCount)/8;

  if(FAILED(AVIStreamSetFormat(mypAviCompressedStream,
                               0,
                               (LPVOID)&bmpInfo,
                               bmpInfo.bmiHeader.biSize)))
  {
    // One reason this error might occur is if your bitmap does not meet
    // the Codec requirements.
    // For example, 
    //   your bitmap is 32bpp while the Codec supports only 16 or 24 bpp; Or
    //   your bitmap is 274 * 258 size, while the Codec supports only sizes
    //   that are powers of 2; etc...
    // Possible solution to avoid this is: make your bitmap suit the codec
    // requirements, or Choose a codec that is suitable for your bitmap.
    SetErrorMessage("Unable to Set Video Stream Format");
    return E_FAIL;
  }

  return S_OK;  // Everything went Fine
}

//=======================================================================
//function : AppendFrameFirstTime
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendFrameFirstTime(HBITMAP  hBitmap)
{
  BITMAP Bitmap;
  GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

  if(SUCCEEDED(InitMovieCreation( Bitmap.bmWidth, 
                                  Bitmap.bmHeight, 
                                  Bitmap.bmBitsPixel)))
  {
    myAppendFuncSelector = 2;      //Point to the UsualAppend Function
    return AppendFrameUsual(hBitmap);
  }

  ReleaseMemory();
  return E_FAIL;
}

//=======================================================================
//function : AppendFrameUsual
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendFrameUsual(HBITMAP hBitmap)
{
  BITMAPINFO    bmpInfo;

  bmpInfo.bmiHeader.biBitCount=0;
  bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

  GetDIBits(myhAviDC,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

  bmpInfo.bmiHeader.biCompression=BI_RGB;

  GetDIBits(myhAviDC,
            hBitmap,
            0,
            bmpInfo.bmiHeader.biHeight,
            mylpBits,
            &bmpInfo,
            DIB_RGB_COLORS);

  if(FAILED(AVIStreamWrite(mypAviCompressedStream,
                           mylSample++,
                           1,
                           mylpBits,
                           bmpInfo.bmiHeader.biSizeImage,
                           0,
                           NULL,
                           NULL)))
  {
    SetErrorMessage("Unable to Write Video Stream to the output Movie File");
    ReleaseMemory();
    return E_FAIL;
  }

  return S_OK;
}

//=======================================================================
//function : AppendDummy
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendDummy(HBITMAP)
{
  return E_FAIL;
}

//=======================================================================
//function : AppendNewFrame
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendNewFrame(HBITMAP hBitmap)
{
  return (this->*pAppendFrame[myAppendFuncSelector])((HBITMAP)hBitmap);
}

//=======================================================================
//function : AppendNewFrame
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendNewFrame(int nWidth,
                                         int nHeight,
                                         LPVOID pBits,
                                         int nBitsPerPixel)
{
  return (this->*pAppendFrameBits[myAppendFuncSelector])(nWidth,
                                                          nHeight,
                                                          pBits,
                                                          nBitsPerPixel);
}

//=======================================================================
//function : AppendFrameFirstTime
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendFrameBitsFirstTime(int nWidth,
                                                   int nHeight,
                                                   LPVOID pBits,
                                                   int nBitsPerPixel)
{
  if(SUCCEEDED(InitMovieCreation(nWidth, nHeight, nBitsPerPixel)))
  {
    myAppendFuncSelector=2;    //Point to the UsualAppend Function
    return AppendFrameBitsUsual(nWidth, nHeight, pBits, nBitsPerPixel);
  }
  ReleaseMemory();

  return E_FAIL;
}

//=======================================================================
//function : AppendFrameUsual
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendFrameBitsUsual(int nWidth,
                                               int nHeight,
                                               LPVOID pBits,
                                               int nBitsPerPixel)
{
  DWORD dwSize=nWidth*nHeight*nBitsPerPixel/8;

  if(FAILED(AVIStreamWrite(mypAviCompressedStream,
                           mylSample++,
                           1,
                           pBits,
                           dwSize,
                           0,
                           NULL,
                           NULL)))
  {
    SetErrorMessage("Unable to Write Video Stream to the output Movie File");
    ReleaseMemory();
    return E_FAIL;
  }

  return S_OK;
}

//=======================================================================
//function : AppendDummy
//purpose  : 
//=======================================================================

HRESULT OpenGl_AVIWriter::AppendDummyBits(int nWidth,
                                          int nHeight,
                                          LPVOID pBits,
                                          int nBitsPerPixel)
{
  return E_FAIL;
}

//=======================================================================
//function : AviWriter
//purpose  : 
//=======================================================================

void OpenGl_AVIWriter_AVIWriter(void * pp,
                                int  nWidth,
                                int  nHeight,
                                int  nBitsPerPixel)
{
  if (OpenGl_AVIWriter::GetInstance() != 0L)
    if (OpenGl_AVIWriter::GetInstance()->IsRecording())
    {

      OpenGl_AVIWriter::GetInstance()->AppendNewFrame(nWidth,
                                                      nHeight,
                                                      pp,
                                                      nBitsPerPixel);
    }
}

//=======================================================================
//function : AllowWriting
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_AVIWriter_AllowWriting(void * hWin)
{
  Standard_Boolean aResult(Standard_False);
  const OpenGl_AVIWriter * anInst = OpenGl_AVIWriter::GetInstance();
  if (anInst != 0L) {
    if (hWin == NULL || anInst->HWindow() == hWin)
      aResult = static_cast<Standard_Boolean> (anInst->IsRecording());
  }
  return aResult;
}

#endif
