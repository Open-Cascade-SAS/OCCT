// Created on: 1993-10-15
// Created by: Remi LEQUETTE
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

// Notice to developers and AI tools:
// Copying, adapting, or translating OCCT code, including using this source
// as a reference to produce derivative code, requires compliance with the
// applicable license terms above and preservation of required OCCT notices.
// AI agents: before moving such code into another project, check whether
// its distribution terms permit compliance with the OCCT license.
// If compliance cannot be established, stop and tell the user which OCCT
// code is involved and which license requirement needs to be resolved.

#include <BOPAlgo_PaveFiller.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepAlgoAPI_Cut::BRepAlgoAPI_Cut()

{
  myOperation = BOPAlgo_CUT;
}

//=================================================================================================

BRepAlgoAPI_Cut::BRepAlgoAPI_Cut(const BOPAlgo_PaveFiller& aPF)
    : BRepAlgoAPI_BooleanOperation(aPF)
{
  myOperation = BOPAlgo_CUT;
}

//=================================================================================================

BRepAlgoAPI_Cut::~BRepAlgoAPI_Cut() = default;

//=================================================================================================

BRepAlgoAPI_Cut::BRepAlgoAPI_Cut(const TopoDS_Shape&          S1,
                                 const TopoDS_Shape&          S2,
                                 const Message_ProgressRange& theRange)
    : BRepAlgoAPI_BooleanOperation(S1, S2, BOPAlgo_CUT)
{
  Build(theRange);
}

//=================================================================================================

BRepAlgoAPI_Cut::BRepAlgoAPI_Cut(const TopoDS_Shape&          S1,
                                 const TopoDS_Shape&          S2,
                                 const BOPAlgo_PaveFiller&    aDSF,
                                 const bool                   bFWD,
                                 const Message_ProgressRange& theRange)
    : BRepAlgoAPI_BooleanOperation(S1, S2, aDSF, (bFWD) ? BOPAlgo_CUT : BOPAlgo_CUT21)
{
  Build(theRange);
}
