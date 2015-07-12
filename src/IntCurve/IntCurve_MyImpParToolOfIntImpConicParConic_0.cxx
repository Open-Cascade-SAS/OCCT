// Created on: 1992-03-04
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <IntCurve_MyImpParToolOfIntImpConicParConic.hxx>

#include <IntCurve_IConicTool.hxx>
#include <IntCurve_PConic.hxx>
#include <IntCurve_PConicTool.hxx>
 

#define ImpTool IntCurve_IConicTool
#define ImpTool_hxx <IntCurve_IConicTool.hxx>
#define ParCurve IntCurve_PConic
#define ParCurve_hxx <IntCurve_PConic.hxx>
#define ParTool IntCurve_PConicTool
#define ParTool_hxx <IntCurve_PConicTool.hxx>
#define IntImpParGen_ImpParTool IntCurve_MyImpParToolOfIntImpConicParConic
#define IntImpParGen_ImpParTool_hxx <IntCurve_MyImpParToolOfIntImpConicParConic.hxx>
#include <IntImpParGen_ImpParTool.gxx>

