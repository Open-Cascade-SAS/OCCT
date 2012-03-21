// Created on: 1998-06-10
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <ShapeExtend.ixx>
#include <Message_MsgFile.hxx>

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
  Message_MsgFile::LoadFromEnv ("CSF_SHMessage", "SHAPE");
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
