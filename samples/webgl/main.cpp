#include <iostream>

#include "WasmOcctView.h"

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <OSD_MemInfo.hxx>
#include <OSD_Parallel.hxx>

#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <Standard_ArrayStreamBuffer.hxx>

#include <emscripten.h>
#include <emscripten/html5.h>

//! Global viewer instance.
static WasmOcctView aViewer;

//! Dummy main loop callback for a single shot.
extern "C" void onMainLoop()
{
  // do nothing here - viewer updates are handled on demand
  emscripten_cancel_main_loop();
}

//! File data read event.
extern "C" void onFileDataRead (void* theOpaque, void* theBuffer, int theDataLen)
{
  const char* aName = theOpaque != NULL ? (const char* )theOpaque : "";
  {
    AIS_ListOfInteractive aShapes;
    aViewer.Context()->DisplayedObjects (AIS_KOI_Shape, -1, aShapes);
    for (AIS_ListOfInteractive::Iterator aShapeIter (aShapes); aShapeIter.More(); aShapeIter.Next())
    {
      aViewer.Context()->Remove (aShapeIter.Value(), false);
    }
  }

  Standard_ArrayStreamBuffer aStreamBuffer ((const char* )theBuffer, theDataLen);
  std::istream aStream (&aStreamBuffer);
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  BRepTools::Read (aShape, aStream, aBuilder);

  Handle(AIS_Shape) aShapePrs = new AIS_Shape (aShape);
  aShapePrs->SetMaterial (Graphic3d_NameOfMaterial_Silver);
  aViewer.Context()->Display (aShapePrs, AIS_Shaded, 0, false);
  aViewer.View()->FitAll (0.01, false);
  aViewer.View()->Redraw();
  Message::DefaultMessenger()->Send (TCollection_AsciiString("Loaded file ") + aName, Message_Info);
  Message::DefaultMessenger()->Send (OSD_MemInfo::PrintInfo(), Message_Trace);
}

//! File read error event.
static void onFileReadFailed (void* theOpaque)
{
  const char* aName = (const char* )theOpaque;
  Message::DefaultMessenger()->Send (TCollection_AsciiString("Error: unable to load file ") + aName, Message_Fail);
}

int main()
{
  Message::DefaultMessenger()->Printers().First()->SetTraceLevel (Message_Trace);
  Handle(Message_PrinterSystemLog) aJSConsolePrinter = new Message_PrinterSystemLog ("webgl-sample", Message_Trace);
  Message::DefaultMessenger()->AddPrinter (aJSConsolePrinter); // open JavaScript console within the Browser to see this output
  Message::DefaultMessenger()->Send (TCollection_AsciiString("NbLogicalProcessors: ") + OSD_Parallel::NbLogicalProcessors(), Message_Trace);

  // setup a dummy single-shot main loop callback just to shut up a useless Emscripten error message on calling eglSwapInterval()
  emscripten_set_main_loop (onMainLoop, -1, 0);

  aViewer.run();
  Message::DefaultMessenger()->Send (OSD_MemInfo::PrintInfo(), Message_Trace);

  // load some file
  emscripten_async_wget_data ("samples/Ball.brep", (void* )"samples/Ball.brep", onFileDataRead, onFileReadFailed);
  return 0;
}
