// ISession_Point.h: interface for the ISession_Point class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISESSION_POINT_H__A9B277C3_A69E_11D1_8DA4_0800369C8A03__INCLUDED_)
#define AFX_ISESSION_POINT_H__A9B277C3_A69E_11D1_8DA4_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

class ISession_Point;
DEFINE_STANDARD_HANDLE(ISession_Point,AIS_InteractiveObject)
class ISession_Point : public AIS_InteractiveObject  
{
public:
  ISession_Point();
  ISession_Point(Standard_Real X,Standard_Real Y ,Standard_Real Z);
  ISession_Point(const gp_Pnt2d& aPoint,Standard_Real Elevation = 0);
  ISession_Point(const gp_Pnt& aPoint);
  virtual ~ISession_Point();
  DEFINE_STANDARD_RTTIEXT(ISession_Point,AIS_InteractiveObject)

private :

  void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                const Handle(Prs3d_Presentation)& aPresentation,
                const Standard_Integer aMode);
  void Compute (const Handle(Prs3d_Projector)& aProjector,
                const Handle(Prs3d_Presentation)& aPresentation);

  void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection,
                        const Standard_Integer unMode);

  gp_Pnt myPoint;

};

#endif // !defined(AFX_ISESSION_POINT_H__A9B277C3_A69E_11D1_8DA4_0800369C8A03__INCLUDED_)
