// Created on: 2013-02-01
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Element.hxx>

#include <Graphic3d_FrameStatsData.hxx>
#include <Standard_Dump.hxx>

// =======================================================================
// function : OpenGl_Element
// purpose  :
// =======================================================================
OpenGl_Element::OpenGl_Element()
{
  //
}

// =======================================================================
// function : ~OpenGl_Element
// purpose  :
// =======================================================================
OpenGl_Element::~OpenGl_Element()
{
  //
}

// =======================================================================
// function : UpdateMemStats
// purpose  :
// =======================================================================
void OpenGl_Element::UpdateMemStats(Graphic3d_FrameStatsDataTmp& theStats) const
{
  theStats[Graphic3d_FrameStatsCounter_EstimatedBytesGeom] += EstimatedDataSize();
}

// =======================================================================
// function : UpdateDrawStats
// purpose  :
// =======================================================================
void OpenGl_Element::UpdateDrawStats(Graphic3d_FrameStatsDataTmp&, bool) const
{
  //
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Element::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, OpenGl_Element)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, this)
}
