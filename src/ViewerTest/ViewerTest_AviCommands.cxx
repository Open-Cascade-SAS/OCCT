// File:      ViewerTest_AviCommands.cxx
// Created:   30.06.08 18:02
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2008


#ifdef WNT
#include <windows.h>
#endif

#include <ViewerTest.hxx>
#include <Draw_Interpretor.hxx>
#include <OpenGl_AVIWriter.hxx>

static Standard_Integer avi_record(Draw_Interpretor& di,
                                   Standard_Integer argc, const char** argv)
{
  Standard_Integer aResult(1);
  if (argc < 2) {
    cout << "Syntax: " << argv[0] << " file | start | stop | save" << endl;
  } else {

#ifndef WNT
    cout << "AVI writer is implemented only in Windows version" << endl;
#else  
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
      pAviWriter->StartRecording();
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
#endif
  }
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



