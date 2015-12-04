// ISession_Direction.h: interface for the ISession_Direction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISession_Direction_H__767C0DB3_A3B0_11D1_8DA3_0800369C8A03__INCLUDED_)
#define AFX_ISession_Direction_H__767C0DB3_A3B0_11D1_8DA3_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include "gp_Dir.hxx"
#include "gp_Vec.hxx"
#include "AIS_InteractiveObject.hxx"
class ISession_Direction;
DEFINE_STANDARD_HANDLE(ISession_Direction,AIS_InteractiveObject)
class ISession_Direction : public AIS_InteractiveObject
{
public:
  ISession_Direction();
    ISession_Direction(const gp_Pnt& aPnt,const gp_Pnt& aPnt2);
    ISession_Direction(const gp_Pnt& aPnt,const gp_Vec& aVec);

  DEFINE_STANDARD_RTTIEXT(ISession_Direction,AIS_InteractiveObject)

private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                const Handle(Prs3d_Presentation)& aPresentation,
                const Standard_Integer aMode);

  void ComputeSelection (const class Handle(SelectMgr_Selection) &,const int){};

private:

  gp_Pnt myStartPnt;
  gp_Pnt myEndPnt;
};

#endif // !defined(AFX_ISession_Direction_H__767C0DB3_A3B0_11D1_8DA3_0800369C8A03__INCLUDED_)
