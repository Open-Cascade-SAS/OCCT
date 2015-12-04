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

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Draw_Drawable3D.hxx>
class HLRBRep_Algo;
class Draw_Display;
class HLRBRep_Data;
class HLRBRep_EdgeData;


class HLRTest_DrawableEdgeTool;
DEFINE_STANDARD_HANDLE(HLRTest_DrawableEdgeTool, Draw_Drawable3D)

//! Used to display the results.
class HLRTest_DrawableEdgeTool : public Draw_Drawable3D
{

public:

  
  Standard_EXPORT HLRTest_DrawableEdgeTool(const Handle(HLRBRep_Algo)& Alg, const Standard_Boolean Visible, const Standard_Boolean IsoLine, const Standard_Boolean Rg1Line, const Standard_Boolean RgNLine, const Standard_Integer ViewId);
  
  Standard_EXPORT void DrawOn (Draw_Display& D) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(HLRTest_DrawableEdgeTool,Draw_Drawable3D)

protected:




private:

  
  Standard_EXPORT void InternalDraw (Draw_Display& D, const Standard_Integer typ) const;
  
  Standard_EXPORT void DrawFace (Draw_Display& D, const Standard_Integer typ, const Standard_Integer nCB, const Standard_Integer iface, Standard_Integer& e2, Standard_Integer& iCB, Handle(HLRBRep_Data)& DS) const;
  
  Standard_EXPORT void DrawEdge (Draw_Display& D, const Standard_Boolean inFace, const Standard_Integer typ, const Standard_Integer nCB, const Standard_Integer ie, Standard_Integer& e2, Standard_Integer& iCB, HLRBRep_EdgeData& ed) const;

  Handle(HLRBRep_Algo) myAlgo;
  Standard_Boolean myVisible;
  Standard_Boolean myIsoLine;
  Standard_Boolean myRg1Line;
  Standard_Boolean myRgNLine;
  Standard_Integer myViewId;


};







#endif // _HLRTest_DrawableEdgeTool_HeaderFile
