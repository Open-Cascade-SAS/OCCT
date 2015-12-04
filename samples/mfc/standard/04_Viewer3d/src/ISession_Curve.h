// ISession_Curve.h: interface for the ISession_Curve class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISESSION_CURVE_H__F981CB93_A3CC_11D1_8DA3_0800369C8A03__INCLUDED_)
#define AFX_ISESSION_CURVE_H__F981CB93_A3CC_11D1_8DA3_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Geom_Curve.hxx>
class ISession_Curve;
DEFINE_STANDARD_HANDLE(ISession_Curve,AIS_InteractiveObject)

class ISession_Curve : public AIS_InteractiveObject  
{
public:
	ISession_Curve(const Handle(Geom_Curve)& aCurve);
	virtual ~ISession_Curve();
DEFINE_STANDARD_RTTIEXT(ISession_Curve,AIS_InteractiveObject)
private:

Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode = 0) ;
Standard_EXPORT virtual  void Compute(const Handle(Prs3d_Projector)& aProjector,const Handle(Prs3d_Presentation)& aPresentation) ;
void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,const Standard_Integer aMode) ;

Handle(Geom_Curve) myCurve;
};

#endif // !defined(AFX_ISESSION_CURVE_H__F981CB93_A3CC_11D1_8DA3_0800369C8A03__INCLUDED_)
