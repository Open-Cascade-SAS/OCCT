// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
//standard OCC types
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#pragma warning( disable : 4267 )
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Version.hxx>
#include <Standard_Persistent.hxx>
//collections
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <TCollection_AsciiString.hxx>
//for OCC graphic
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
//for object display
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_DisplayMode.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <V3d_TypeOfShadingModel.hxx>
#include <V3d_TypeOfUpdate.hxx>
//topology
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
//brep tools
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <MgtBRep.hxx>
//geometry
#include <gp_Ax2.hxx>
#include <Geom_Axis2Placement.hxx>
//csfdb I/E
#include <FSD_File.hxx>
#include <ShapeSchema.hxx>
#include <Storage_Data.hxx>
#include <Storage_Error.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <PTopoDS_HShape.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
// iges I/E
#include <IGESControl_Reader.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Interface_Static.hxx>
//step I/E
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
//for stl export
#include <StlAPI_Writer.hxx>
//for vrml export
#include <VrmlAPI_Writer.hxx>