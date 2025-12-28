// Created on: 1992-08-27
// Created by: Christophe MARION
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

#ifndef _HLRTest_DrawableEdgeTool_HeaderFile
#define _HLRTest_DrawableEdgeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Draw_Drawable3D.hxx>
class HLRBRep_Algo;
class Draw_Display;
class HLRBRep_Data;
class HLRBRep_EdgeData;

//! Used to display the results.
class HLRTest_DrawableEdgeTool : public Draw_Drawable3D
{

public:
  Standard_EXPORT HLRTest_DrawableEdgeTool(const occ::handle<HLRBRep_Algo>& Alg,
                                           const bool                       Visible,
                                           const bool                       IsoLine,
                                           const bool                       Rg1Line,
                                           const bool                       RgNLine,
                                           const int                        ViewId);

  Standard_EXPORT void DrawOn(Draw_Display& D) const override;

  DEFINE_STANDARD_RTTIEXT(HLRTest_DrawableEdgeTool, Draw_Drawable3D)

private:
  Standard_EXPORT void InternalDraw(Draw_Display& D, const int typ) const;

  Standard_EXPORT void DrawFace(Draw_Display&              D,
                                const int                  typ,
                                const int                  nCB,
                                const int                  iface,
                                int&                       e2,
                                int&                       iCB,
                                occ::handle<HLRBRep_Data>& DS) const;

  Standard_EXPORT void DrawEdge(Draw_Display&     D,
                                const bool        inFace,
                                const int         typ,
                                const int         nCB,
                                const int         ie,
                                int&              e2,
                                int&              iCB,
                                HLRBRep_EdgeData& ed) const;

  occ::handle<HLRBRep_Algo> myAlgo;
  bool                      myVisible;
  bool                      myIsoLine;
  bool                      myRg1Line;
  bool                      myRgNLine;
  int                       myViewId;
};

#endif // _HLRTest_DrawableEdgeTool_HeaderFile
