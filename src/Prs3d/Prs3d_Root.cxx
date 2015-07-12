// Copyright (c) 1995-1999 Matra Datavision
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


#include <Graphic3d_Group.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

Handle (Graphic3d_Group) Prs3d_Root::CurrentGroup (const Handle (Prs3d_Presentation)& Prs3d) 
{
  return Prs3d->CurrentGroup();
}
Handle (Graphic3d_Group) Prs3d_Root::NewGroup (const Handle (Prs3d_Presentation)& Prs3d) 
{
  return Prs3d->NewGroup();
}
