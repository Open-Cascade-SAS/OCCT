// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Graphic3d_TransformPers.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TransformPers, Standard_Transient)

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_TransformPers::PersParams3d::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  gp_Pnt anAttachPoint (PntX, PntY, PntZ);
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &anAttachPoint)
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_TransformPers::PersParams2d::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, OffsetX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, OffsetY)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Corner)
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_TransformPers::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMode)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myParams.Params3d)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myParams.Params2d)
}
