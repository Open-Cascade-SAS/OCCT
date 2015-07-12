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

//  Methodes de confort, evitant de devoir connaitre SessionFile, qui est un
//  Tool non destine a l export (en particulier, pas un Handle)
Standard_Boolean  IFSelect::SaveSession
  (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file)
{
  IFSelect_SessionFile sesfile(WS,file);
  return sesfile.IsDone();
}

    Standard_Boolean  IFSelect::RestoreSession
  (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file)
{
  IFSelect_SessionFile sesfile(WS);
  return (sesfile.Read(file) == 0);
}
