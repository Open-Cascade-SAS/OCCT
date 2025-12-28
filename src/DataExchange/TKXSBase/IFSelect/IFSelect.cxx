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

#include <IFSelect.hxx>
#include <IFSelect_SessionFile.hxx>
#include <IFSelect_WorkSession.hxx>

//  Convenience methods, avoiding having to know SessionFile, which is a
//  Tool not intended for export (in particular, not a Handle)
bool IFSelect::SaveSession(const occ::handle<IFSelect_WorkSession>& WS, const char* file)
{
  IFSelect_SessionFile sesfile(WS, file);
  return sesfile.IsDone();
}

bool IFSelect::RestoreSession(const occ::handle<IFSelect_WorkSession>& WS, const char* file)
{
  IFSelect_SessionFile sesfile(WS);
  return (sesfile.Read(file) == 0);
}
