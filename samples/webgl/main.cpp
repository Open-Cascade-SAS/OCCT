// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include <iostream>

#include "WasmOcctView.h"

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <OSD_MemInfo.hxx>
#include <OSD_Parallel.hxx>

#include <emscripten.h>
#include <emscripten/html5.h>

//! Dummy main loop callback for a single shot.
extern "C" void onMainLoop()
{
  // do nothing here - viewer updates are handled on demand
  emscripten_cancel_main_loop();
}

EMSCRIPTEN_KEEPALIVE int main()
{
  Message::DefaultMessenger()->Printers().First()->SetTraceLevel (Message_Trace);
  Handle(Message_PrinterSystemLog) aJSConsolePrinter = new Message_PrinterSystemLog ("webgl-sample", Message_Trace);
  Message::DefaultMessenger()->AddPrinter (aJSConsolePrinter); // open JavaScript console within the Browser to see this output
  Message::SendTrace() << "Emscripten SDK " << __EMSCRIPTEN_major__ << "." << __EMSCRIPTEN_minor__ << "." << __EMSCRIPTEN_tiny__;
#if defined(__LP64__)
  Message::SendTrace() << "Architecture: WASM 64-bit";
#else
  Message::SendTrace() << "Architecture: WASM 32-bit";
#endif
  Message::SendTrace() << "NbLogicalProcessors: "
    << OSD_Parallel::NbLogicalProcessors()
#ifdef __EMSCRIPTEN_PTHREADS__
    << " (pthreads ON)"
#else
    << " (pthreads OFF)"
#endif
  ;

  // setup a dummy single-shot main loop callback just to shut up a useless Emscripten error message on calling eglSwapInterval()
  emscripten_set_main_loop (onMainLoop, -1, 0);

  WasmOcctView& aViewer = WasmOcctView::Instance();
  aViewer.run();
  Message::DefaultMessenger()->Send (OSD_MemInfo::PrintInfo(), Message_Trace);
  return 0;
}
