// ISession_Surface.h: interface for the ISession_Surface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISESSION_SURFACE_H__27F86F5A_A6A4_11D1_8DA4_0800369C8A03__INCLUDED_)
#define AFX_ISESSION_SURFACE_H__27F86F5A_A6A4_11D1_8DA4_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
class ISession_Surface;
DEFINE_STANDARD_HANDLE(ISession_Surface,AIS_InteractiveObject)
class ISession_Surface : public AIS_InteractiveObject  
{
public:
	ISession_Surface();
	ISession_Surface(const Handle(Geom_Surface)& aSurface);
	virtual ~ISession_Surface();

DEFINE_STANDARD_RTTIEXT(ISession_Surface,AIS_InteractiveObject)
private:

Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode = 0);
Standard_EXPORT virtual  void Compute(const Handle(Prs3d_Projector)& aProjector,const Handle(Prs3d_Presentation)& aPresentation);
void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,const Standard_Integer aMode);

Handle(Geom_Surface) mySurface;

};

#endif // !defined(AFX_ISESSION_SURFACE_H__27F86F5A_A6A4_11D1_8DA4_0800369C8A03__INCLUDED_)
