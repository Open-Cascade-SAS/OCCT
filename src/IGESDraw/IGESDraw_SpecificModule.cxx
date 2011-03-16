#include <IGESDraw_SpecificModule.ixx>
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


    IGESDraw_SpecificModule::IGESDraw_SpecificModule()    {  }


    void  IGESDraw_SpecificModule::OwnDump
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const IGESData_IGESDumper& dumper, const Handle(Message_Messenger)& S,
   const Standard_Integer own) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDraw_CircArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolCircArraySubfigure tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDraw_ConnectPoint,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolConnectPoint tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDraw_Drawing,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawing tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDraw_DrawingWithRotation,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolDrawingWithRotation tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDraw_LabelDisplay,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolLabelDisplay tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDraw_NetworkSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigure tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDraw_NetworkSubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolNetworkSubfigureDef tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESDraw_PerspectiveView,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPerspectiveView tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESDraw_Planar,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolPlanar tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESDraw_RectArraySubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolRectArraySubfigure tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESDraw_SegmentedViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolSegmentedViewsVisible tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESDraw_View,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolView tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESDraw_ViewsVisible,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisible tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESDraw_ViewsVisibleWithAttr,anent,ent);
      if (anent.IsNull()) return;
      IGESDraw_ToolViewsVisibleWithAttr tool;
      tool.OwnDump(anent,dumper,S,own);
    }
      break;
    default : break;
  }
}


    Standard_Boolean  IGESDraw_SpecificModule::OwnCorrect
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const
{
//   Applies only on some types
  switch (CN) {
    case  3 : {
      DeclareAndCast(IGESDraw_Drawing,anent,ent);
      if (anent.IsNull()) break;
      IGESDraw_ToolDrawing tool;
      return tool.OwnCorrect(anent);
    }
    case  4 : {
      DeclareAndCast(IGESDraw_DrawingWithRotation,anent,ent);
      if (anent.IsNull()) break;
      IGESDraw_ToolDrawingWithRotation tool;
      return tool.OwnCorrect(anent);
    }
    case  9 : {
      DeclareAndCast(IGESDraw_Planar,anent,ent);
      if (anent.IsNull()) break;
      IGESDraw_ToolPlanar tool;
      return tool.OwnCorrect(anent);
    }
    case 13 : {
      DeclareAndCast(IGESDraw_ViewsVisible,anent,ent);
      if (anent.IsNull()) break;
      IGESDraw_ToolViewsVisible tool;
      return tool.OwnCorrect(anent);
    }
    case 14 : {
      DeclareAndCast(IGESDraw_ViewsVisibleWithAttr,anent,ent);
      if (anent.IsNull()) break;
      IGESDraw_ToolViewsVisibleWithAttr tool;
      return tool.OwnCorrect(anent);
    }
    default : break;
  }
  return Standard_False;
}
