// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IGESDraw_ReadWriteModule.ixx>
#include <Interface_Macros.hxx>

#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_Planar.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>

#include <IGESDraw_ToolCircArraySubfigure.hxx>
#include <IGESDraw_ToolConnectPoint.hxx>
#include <IGESDraw_ToolDrawing.hxx>
#include <IGESDraw_ToolDrawingWithRotation.hxx>
#include <IGESDraw_ToolLabelDisplay.hxx>
#include <IGESDraw_ToolNetworkSubfigure.hxx>
#include <IGESDraw_ToolNetworkSubfigureDef.hxx>
#include <IGESDraw_ToolPerspectiveView.hxx>
#include <IGESDraw_ToolPlanar.hxx>
#include <IGESDraw_ToolRectArraySubfigure.hxx>
#include <IGESDraw_ToolSegmentedViewsVisible.hxx>
#include <IGESDraw_ToolView.hxx>
#include <IGESDraw_ToolViewsVisible.hxx>
#include <IGESDraw_ToolViewsVisibleWithAttr.hxx>



//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol


IGESDraw_ReadWriteModule::IGESDraw_ReadWriteModule ()    {  }


    Standard_Integer  IGESDraw_ReadWriteModule::CaseIGES
  (const Standard_Integer typenum, const Standard_Integer formnum) const 
{
  switch (typenum) {
    case 132 : return  2;
    case 320 : return  7;
    case 402 :
      switch (formnum) {
        case  3 : return 13;
        case  4 : return 14;
        case  5 : return  5;
        case 16 : return  9;
        case 19 : return 11;
	default : break;
      }
      break;
    case 404 : return formnum+3;  // 0->3  1->4
    case 410 :
      if      (formnum == 0) return 12;
      else if (formnum == 1) return  8;
      break;
    case 412 : return 10;
    case 414 : return  1;
    case 420 : return  6;
    default : break;
  }
  return 0;
}


    void  IGESDraw_ReadWriteModule::ReadOwnParams
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDraw_CircArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolCircArraySubfigure tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDraw_ConnectPoint,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolConnectPoint tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDraw_Drawing,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawing tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDraw_DrawingWithRotation,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawingWithRotation tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDraw_LabelDisplay,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolLabelDisplay tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDraw_NetworkSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigure tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDraw_NetworkSubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigureDef tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESDraw_PerspectiveView,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPerspectiveView tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESDraw_Planar,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPlanar tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESDraw_RectArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolRectArraySubfigure tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESDraw_SegmentedViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolSegmentedViewsVisible tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESDraw_View,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolView tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESDraw_ViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisible tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESDraw_ViewsVisibleWithAttr,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisibleWithAttr tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    default : break;
  }
}


    void  IGESDraw_ReadWriteModule::WriteOwnParams
  (const Standard_Integer CN,  const Handle(IGESData_IGESEntity)& ent,
   IGESData_IGESWriter& IW) const
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDraw_CircArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolCircArraySubfigure tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDraw_ConnectPoint,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolConnectPoint tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDraw_Drawing,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawing tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDraw_DrawingWithRotation,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawingWithRotation tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDraw_LabelDisplay,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolLabelDisplay tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDraw_NetworkSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigure tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDraw_NetworkSubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigureDef tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESDraw_PerspectiveView,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPerspectiveView tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESDraw_Planar,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPlanar tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESDraw_RectArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolRectArraySubfigure tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESDraw_SegmentedViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolSegmentedViewsVisible tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESDraw_View,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolView tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESDraw_ViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisible tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESDraw_ViewsVisibleWithAttr,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisibleWithAttr tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    default : break;
  }
}
