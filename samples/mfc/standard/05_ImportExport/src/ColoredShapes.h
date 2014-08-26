// ColoredShape.h: interface for the CColoredShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLOREDSHAPES_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_)
#define AFX_COLOREDSHAPES_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CColoredShapes : public CObject  
{
public:
	CColoredShapes();
	void Add(const Quantity_NameOfColor aColor, const TopoDS_Shape& aShape);
	void Remove(const TopoDS_Shape& aShape);

	void Display( Handle(AIS_InteractiveContext)& anAIScontext);

	TopoDS_ListOfShape getShapes();
	Quantity_NameOfColor getShapeColor(TopoDS_Shape aShape);

protected:
	// Declare CArchive >> operator
	DECLARE_SERIAL(CColoredShapes);

private:
	TopTools_DataMapOfShapeInteger m_colorMap;
	TopoDS_ListOfShape   m_shapeList;
};

#endif // !defined(AFX_COLOREDSHAPES_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_)
