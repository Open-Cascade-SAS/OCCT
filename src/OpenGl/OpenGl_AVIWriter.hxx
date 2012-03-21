// Created on: 2007-04-15
// Created by: Alexey MORENOV & Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#ifndef __OPENGL_AVIWRITER_H
#define __OPENGL_AVIWRITER_H

#ifdef WNT
#define THIS void 

#include <InterfaceGraphic_WNT.hxx>
#include <stdlib.h>
#include <vfw.h>
#include <TCollection_AsciiString.hxx>

/**
 * Class providing the API to record AVI files using a codec installed in the
 * system -- Only on Windows NT/2k/XP/Vista platform under MS Visual Studio.
 * The following tasks are supported:
 * <ol>
 *
 * <li> Creation of AVI data stream: launched by the constructor.<br>
 * The constructor accepts the filename, FOURCC video code and the frame rate
 * setting as parameters.<br>
 * The default codec name used here is MPG4. To use a different codec, pass
 * its FOURCC value as the input parameter for dwCodec.
 * For example,
 * <ul>
 * <li>pass mmioFOURCC('D','I','V','X') to use DIVX codec, or</li>
 * <li>pass mmioFOURCC('I','V','5','0') to use IV50 codec etc...</li>
 * </ul>
 * Also, you can pass just 0 to avoid the codecs altogether. In that case,
 * the frames would be saved as they are without any compression; However,
 * the output movie file size would be very huge in that case.
 * <br>
 * Finally, make sure you have the codec installed on the machine before
 * passing its Fourcc here.
 * </li>
 * <li>
 * Start the recording: call the method StartRecording(). This method should be
 * called at least once; execution of the constructor does not begin the
 * process.
 * </li>
 * <li>
 * Stop the recording: call the method StopRecording(). Can be omitted if the
 * next to execute would be the destructor.
 * </li>
 * <li>
 * Close the AVI file and exit the process of recording. This is done
 * automatically by the destructor.
 * </li>
 * </ol>
 */
class OpenGl_AVIWriter
{
public:

  /**
   * Constructor. Initializes the internal data structures, prepares for the
   * creation of an AVI stream when the first frame is ready to be captured.
   * @param theFileName
   *   Name of the output movie file to create.
   * @param theCodec
   *   FourCC of the Video Codec to be used for compression
   * @param theFrameRate
   *   The Frames Per Second (FPS) setting to be used for the movie
   */
  Standard_EXPORT OpenGl_AVIWriter(const char * theFileName, 
                                   DWORD theCodec = mmioFOURCC('M','P','G','4'),
                                   Standard_Integer theFrameRate = 25);

  /**
   * Destructor: closes the movie file and flushes all the frames
   */
  Standard_EXPORT ~OpenGl_AVIWriter     ();

  /**
   * Begin the recording.
   */
  Standard_EXPORT void    StartRecording(const HANDLE hWin = NULL);

  /**
   * Stop the recording (can be restarted using StartRecording()).
   */
  Standard_EXPORT void    StopRecording ();

  /**
   * Query the state of recording.
   */
  inline Standard_Boolean IsRecording   () const { return myIsAllowRecord; }

  /**
   * Returns the last error message, if any.
   */
  inline Standard_EXPORT const TCollection_AsciiString&
                          GetLastErrorMessage() const
  { return myErrMsg; }

  /**
   * Get the instance of AVI Writer class.
   */ 
  static Standard_EXPORT OpenGl_AVIWriter *
                          GetInstance   ();

  /**
   * Get the Window handle that contains the actual OpenGl context.
   */
  inline HANDLE           HWindow () const
  { return myhWindow; }

  /// Inserts the given HBitmap into the movie as a new Frame at the end.
  HRESULT         AppendNewFrame(HBITMAP hBitmap);

  /// Inserts the given bitmap bits into the movie as a new Frame at the end.
  /// The width, height and nBitsPerPixel are the width, height and bits per pixel
  /// of the bitmap pointed to by the input pBits.
  HRESULT         AppendNewFrame(int nWidth,
                                 int nHeight,
                                 LPVOID pBits,
                                 int nBitsPerPixel);

private:

  void call_avi();

private:
  static OpenGl_AVIWriter       * MyAVIWriterInstance;
  Standard_Boolean                myIsAllowRecord;

  BYTE                          * mypBits;
  UINT                            myWidth;
  UINT                            myHeight;

  HDC                   myhAviDC;
  HANDLE                myhHeap;
  HANDLE                myhWindow;   // window containing the OGL context 
  LPVOID                mylpBits;    // Useful to hold the bitmap content if any
  LONG                  mylSample;   // Keeps track of the current Frame Index
  PAVIFILE              mypAviFile;
  PAVISTREAM            mypAviStream;
  PAVISTREAM            mypAviCompressedStream;
  AVISTREAMINFO         myAviStreamInfo;
  AVICOMPRESSOPTIONS    myAviCompressOptions;
  char *                myFileName; // Holds the Output Movie File Name
  TCollection_AsciiString myErrMsg; // Holds the Last Error Message, if any

  int                   myAppendFuncSelector;  //0=Dummy 1=FirstTime 2=Usual

  HRESULT               AppendFrameFirstTime(HBITMAP );
  HRESULT               AppendFrameUsual(HBITMAP);
  HRESULT               AppendDummy(HBITMAP);
  HRESULT       (OpenGl_AVIWriter::*pAppendFrame[3])(HBITMAP hBitmap);

  HRESULT               AppendFrameBitsFirstTime(int, int, LPVOID,int );
  HRESULT               AppendFrameBitsUsual(int, int, LPVOID,int );
  HRESULT               AppendDummyBits(int, int, LPVOID,int );
  HRESULT       (OpenGl_AVIWriter::*pAppendFrameBits[3])(int, int, LPVOID, int);

  /// Takes care of creating the memory, streams, compression options etc.
  /// required for the movie
  HRESULT InitMovieCreation(int nFrameWidth,int nFrameHeight,int nBitsPerPixel);

  /// Takes care of releasing the memory and movie related handles
  void ReleaseMemory();

  /// Sets the Error Message
  void SetErrorMessage(const char * theErrMsg);
};

Standard_EXPORT void OpenGl_AVIWriter_AVIWriter(void * pp,
                                                int  nWidth,
                                                int  nHeight,
                                                int  nBitsPerPixel);

Standard_EXPORT Standard_Boolean OpenGl_AVIWriter_AllowWriting(void * hWin);

#endif // WNT
#endif
