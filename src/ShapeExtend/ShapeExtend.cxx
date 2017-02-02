// Created on: 1998-06-10
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#include <ShapeExtend.hxx>

#include <Message_MsgFile.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>

#include "../SHMessage/SHMessage_SHAPE_us.pxx"

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void ShapeExtend::Init()
{
  static Standard_Boolean init = Standard_False;
  if (init) return;

  init = Standard_True;
  
  // load Message File for Shape Healing
  if (!Message_MsgFile::HasMsg ("ShapeFix.FixSmallSolid.MSG0"))
  {
    if (!Message_MsgFile::LoadFromEnv ("CSF_SHMessage", "SHAPE"))
    {
      Message_MsgFile::LoadFromString (SHMessage_SHAPE_us, sizeof(SHMessage_SHAPE_us) - 1);
    }
    if (!Message_MsgFile::HasMsg ("ShapeFix.FixSmallSolid.MSG0"))
    {
      throw Standard_ProgramError("Critical Error - message resources for ShapeExtend are invalid or undefined!");
    }
  }
}

//=======================================================================
//function : EncodeStatus
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend::EncodeStatus (const ShapeExtend_Status status)
{
  switch ( status ) {
  case ShapeExtend_OK   : return 0x0000;
  case ShapeExtend_DONE1: return 0x0001;
  case ShapeExtend_DONE2: return 0x0002;
  case ShapeExtend_DONE3: return 0x0004;
  case ShapeExtend_DONE4: return 0x0008;
  case ShapeExtend_DONE5: return 0x0010;
  case ShapeExtend_DONE6: return 0x0020;
  case ShapeExtend_DONE7: return 0x0040;
  case ShapeExtend_DONE8: return 0x0080;
  case ShapeExtend_DONE : return 0x00ff;
  case ShapeExtend_FAIL1: return 0x0100;
  case ShapeExtend_FAIL2: return 0x0200;
  case ShapeExtend_FAIL3: return 0x0400;
  case ShapeExtend_FAIL4: return 0x0800;
  case ShapeExtend_FAIL5: return 0x1000;
  case ShapeExtend_FAIL6: return 0x2000;
  case ShapeExtend_FAIL7: return 0x4000;
  case ShapeExtend_FAIL8: return 0x8000;
  case ShapeExtend_FAIL : return 0xff00;
  }
  return 0;
}

//=======================================================================
//function : DecodeStatus
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend::DecodeStatus (const Standard_Integer flag, 
					   const ShapeExtend_Status status)
{
  if ( status == ShapeExtend_OK ) return ( flag == 0 );
  return ( flag & ShapeExtend::EncodeStatus ( status ) ? Standard_True : Standard_False );
}
