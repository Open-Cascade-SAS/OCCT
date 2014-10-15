// Created on: 2008-06-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#if (defined(_WIN32) || defined(__WIN32__)) && defined(HAVE_VIDEOCAPTURE)
  #include <windows.h>
  #include <Aspect_Window.hxx>
  #include <OpenGl_AVIWriter.hxx>
  #include <V3d_View.hxx>
#endif

#include <ViewerTest.hxx>
#include <Draw_Interpretor.hxx>

static Standard_Integer avi_record(Draw_Interpretor& /*di*/,
                                   Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    cout << "Syntax: " << argv[0] << " file | start | stop | save" << endl;
    return 1;
  }

  Standard_Integer aResult = 1;
#if (defined(_WIN32) || defined(__WIN32__))
  #ifdef HAVE_VIDEOCAPTURE
    Handle(V3d_View) aView = ViewerTest::CurrentView ();
    if (aView.IsNull())
    {
      std::cout << "Call vinit before!\n";
      return 1;
    }

    static OpenGl_AVIWriter * pAviWriter = 0L;

    if (strncmp(argv[1], "file", 5) == 0) {
      if (argc < 3) {
        cout << "Please define the name of AVI file to create ..." << endl;
      } else {
        const char * aFilename = argv[2];
        DWORD aFormat = mmioFOURCC('X','V','I','D');
        if (argc > 3) {
          const char * aFourcc = argv[3];
          aFormat = mmioFOURCC(aFourcc[0], aFourcc[1], aFourcc[2], aFourcc[3]);
        }
        pAviWriter = new OpenGl_AVIWriter (aFilename, aFormat, 25);
        aResult = 0;
      }
    } else if (pAviWriter == 0L) {
      cout << "AVI Writer instance has not been initialized. Use command "
           << argv[0] << " file ..." << endl;
    } else if (strncmp(argv[1], "start", 6) == 0) {
      pAviWriter->StartRecording (aView->Window()->NativeHandle());
      aResult = 0;
    } else if (strncmp(argv[1], "stop", 5) == 0) {
      pAviWriter->StopRecording();
      aResult = 0;
    } else if (strncmp(argv[1], "save", 5) == 0) {
      pAviWriter->StopRecording();
      delete pAviWriter;
      pAviWriter = 0L;
      aResult = 0;
    } else if (strncmp(argv[1], "status", 7) == 0) {
      cout << pAviWriter->GetLastErrorMessage() << endl;
      aResult = 0;
    }
  #else
    cout << "AVI writer capability was disabled\n";
  #endif
#else
  cout << "AVI writer is implemented only in Windows version\n";
#endif
  return aResult;
}

//=======================================================================
//function : AviCommands
//purpose  : 
//=======================================================================

void ViewerTest::AviCommands(Draw_Interpretor& theCommands)
{
  const char* group = "ViewerTest AVI commands";

  theCommands.Add("vrecord", "vrecord [option]\n"
                  "where [option] can be:\n"
                  "\tfile <filename.avi> <FOURCC=VIDX': Create AVI file "
                                         "for recording,\n"
                  "\tstart              : begin/restart recording,\n"
                  "\tstop               : stop recording,\n"
                  "\tstatus             : log error message,\n"
                  "\tsave               : close the AVI file\n",
		  __FILE__,
		  &avi_record, group); //Draft_Modification
}
