// Copyright (c) 2021 OPEN CASCADE SAS
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

#include "WasmOcctPixMap.h"

#include <Message.hxx>

#include <emscripten.h>

// ================================================================
// Function : WasmOcctPixMap
// Purpose  :
// ================================================================
WasmOcctPixMap::WasmOcctPixMap()
: myRawDataPtr (nullptr) {}

// ================================================================
// Function : ~WasmOcctPixMap
// Purpose  :
// ================================================================
WasmOcctPixMap::~WasmOcctPixMap()
{
  Clear();
}

// ================================================================
// Function : Clear
// Purpose  :
// ================================================================
void WasmOcctPixMap::Clear()
{
  if (myRawDataPtr != nullptr) { free (myRawDataPtr); }
  myRawDataPtr = nullptr;
  Image_PixMap::Clear();
}

// ================================================================
// Function : Init
// Purpose  :
// ================================================================
bool WasmOcctPixMap::Init (const char* theFilePath)
{
  Clear();
  int aSizeX = 0, aSizeY = 0;
  char* anImgData = emscripten_get_preloaded_image_data (theFilePath, &aSizeX, &aSizeY);
  if (anImgData == nullptr)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString("Error: invalid image ") + theFilePath, Message_Fail);
    return false;
  }

  Message::DefaultMessenger()->Send (TCollection_AsciiString("Loaded image ") + theFilePath + "@" + aSizeX + "x" + aSizeY, Message_Info);
  InitWrapper (Image_Format_RGBA, (Standard_Byte* )anImgData, aSizeX, aSizeY);
  SetTopDown (true);
  myRawDataPtr = anImgData;
  return true;
}
