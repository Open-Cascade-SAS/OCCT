// ColoredShapes.cpp: implementation of the CColoredShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <afxtempl.h>

#include "ColoredShapes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColoredShapes::CColoredShapes()
{
}


void CColoredShapes::Add(const Quantity_NameOfColor aColor, const TopoDS_Shape& aShape)
{
	m_shapeList.Append(aShape);
	m_colorMap.Bind(aShape, aColor);
}

void CColoredShapes::Remove(const TopoDS_Shape& aShape)
{
	m_colorMap.UnBind(aShape);
	for ( TopoDS_ListIteratorOfListOfShape iter(m_shapeList); iter.More(); iter.Next() ) {
		if(iter.Value() == aShape) {
			m_shapeList.Remove(iter);
			break;
		}
	}
}

IMPLEMENT_SERIAL(CColoredShapes, CObject,1);

#include <TopoDS_Shape.hxx>

// Tools to put Persistent Object in an archive

void CColoredShapes::Display(Handle(AIS_InteractiveContext)& anAIScontext)
{
	for ( TopoDS_ListIteratorOfListOfShape iter(m_shapeList); iter.More(); iter.Next() )
	{
		Handle(AIS_Shape) ais = new AIS_Shape(iter.Value());
		anAIScontext->SetColor(ais, (Quantity_NameOfColor)m_colorMap.Find(iter.Value()), Standard_False);
    anAIScontext->SetMaterial(ais, Graphic3d_NOM_GOLD, Standard_False);
		anAIScontext->Display(ais, Standard_False);
	}
  anAIScontext->UpdateCurrentViewer();
}
