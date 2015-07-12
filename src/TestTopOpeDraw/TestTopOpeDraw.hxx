// Created on: 1997-01-20
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TestTopOpeDraw_HeaderFile
#define _TestTopOpeDraw_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
class TestTopOpeDraw_DrawableSHA;
class TestTopOpeDraw_DrawableSUR;
class TestTopOpeDraw_DrawableC3D;
class TestTopOpeDraw_DrawableC2D;
class TestTopOpeDraw_DrawableP3D;
class TestTopOpeDraw_DrawableP2D;
class TestTopOpeDraw_DrawableMesure;



class TestTopOpeDraw 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static void OtherCommands (Draw_Interpretor& I);




protected:





private:




friend class TestTopOpeDraw_DrawableSHA;
friend class TestTopOpeDraw_DrawableSUR;
friend class TestTopOpeDraw_DrawableC3D;
friend class TestTopOpeDraw_DrawableC2D;
friend class TestTopOpeDraw_DrawableP3D;
friend class TestTopOpeDraw_DrawableP2D;
friend class TestTopOpeDraw_DrawableMesure;

};







#endif // _TestTopOpeDraw_HeaderFile
