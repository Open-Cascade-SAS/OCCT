// Created on: 1993-08-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Draw_Chronometer.hxx>

#include <Draw_Display.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Chronometer, Draw_Drawable3D)

//=================================================================================================

Draw_Chronometer::Draw_Chronometer()
{
  //
}

//=================================================================================================

void Draw_Chronometer::DrawOn(Draw_Display&) const
{
  //
}

//=================================================================================================

Handle(Draw_Drawable3D) Draw_Chronometer::Copy() const
{
  Handle(Draw_Chronometer) C = new Draw_Chronometer();
  return C;
}

//=================================================================================================

void Draw_Chronometer::Dump(Standard_OStream& S) const
{
  S << "Chronometer, ";
  myTimer.Show(S);
}

//=================================================================================================

void Draw_Chronometer::Whatis(Draw_Interpretor& i) const
{
  i << "chronometer";
}

//=================================================================================================

void Draw_Chronometer::DumpJson(Standard_OStream& theOStream, Standard_Integer ) const
{
  const Standard_Real ElapsedTime = myTimer.ElapsedTime();
  Standard_Real UserTimeCPU = 0.0, SystemTimeCPU = 0.0;
  myTimer.OSD_Chronometer::Show(UserTimeCPU, SystemTimeCPU);

  OCCT_DUMP_CLASS_BEGIN(theOStream, Draw_Chronometer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ElapsedTime)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, UserTimeCPU)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, SystemTimeCPU)
}
