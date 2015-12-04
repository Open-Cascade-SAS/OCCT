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


#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_Planar.hxx>
#include <IGESDraw_ReadWriteModule.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
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
#include <IGESDraw_View.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_ReadWriteModule,IGESData_ReadWriteModule)

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
