// ColoredShape.h: interface for the CColoredShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLOREDSHAPE_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_)
#define AFX_COLOREDSHAPE_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CColoredShape : public CObject  
{
public:
	CColoredShape( const Quantity_NameOfColor aColor, const TopoDS_Shape& aShape);

	void Display( Handle(AIS_InteractiveContext)& anAIScontext);

	// fields
	Quantity_NameOfColor m_colorName;
	TopoDS_Shape         m_shapeObject;

protected:
	CColoredShape();

	// Declare CArchive >> operator
	DECLARE_SERIAL(CColoredShape);

	// mute CObject::Serialize
	void Serialize(CArchive& ar);

};

#endif // !defined(AFX_COLOREDSHAPE_H__C6419AF3_A78A_11D1_8C93_00AA00D10994__INCLUDED_)
