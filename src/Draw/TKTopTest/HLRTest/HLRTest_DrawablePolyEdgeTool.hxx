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

#ifndef _HLRTest_DrawablePolyEdgeTool_HeaderFile
#define _HLRTest_DrawablePolyEdgeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <HLRBRep_BiPoint.hxx>
#include <NCollection_List.hxx>
#include <Draw_Drawable3D.hxx>
class HLRBRep_PolyAlgo;
class Draw_Display;

//! Used to display the results.
class HLRTest_DrawablePolyEdgeTool : public Draw_Drawable3D
{

public:
  Standard_EXPORT HLRTest_DrawablePolyEdgeTool(const occ::handle<HLRBRep_PolyAlgo>& Alg,
                                               const int          ViewId,
                                               const bool Debug = false);

  void Show();

  void Hide();

  void DisplayRg1Line(const bool B);

  bool DisplayRg1Line() const;

  void DisplayRgNLine(const bool B);

  bool DisplayRgNLine() const;

  void DisplayHidden(const bool B);

  bool DisplayHidden() const;

  Standard_EXPORT void DrawOn(Draw_Display& D) const override;

  bool Debug() const;

  void Debug(const bool B);

  DEFINE_STANDARD_RTTIEXT(HLRTest_DrawablePolyEdgeTool, Draw_Drawable3D)

private:
  occ::handle<HLRBRep_PolyAlgo> myAlgo;
  bool         myDispRg1;
  bool         myDispRgN;
  bool         myDispHid;
  int         myViewId;
  NCollection_List<HLRBRep_BiPoint>     myBiPntVis;
  NCollection_List<HLRBRep_BiPoint>     myBiPntHid;
  bool         myDebug;
  bool         myHideMode;
};

#include <HLRTest_DrawablePolyEdgeTool.lxx>

#endif // _HLRTest_DrawablePolyEdgeTool_HeaderFile
