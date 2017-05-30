#ifndef _ISession2D_Curve_HeaderFile
#define _ISession2D_Curve_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

#include <Aspect_TypeOfline.hxx>
#include <Aspect_WidthOfline.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_CString.hxx>

#include <TColGeom2d_HSequenceOfCurve.hxx>
class PrsMgr_PresentationManager2d;
class Graphic2d_GraphicObject;
class SelectMgr_Selection;

#include "Geom2d_Curve.hxx"

#include "AIS_InteractiveObject.hxx"
class ISession2D_Curve;
DEFINE_STANDARD_HANDLE(ISession2D_Curve,AIS_InteractiveObject)
class ISession2D_Curve : public AIS_InteractiveObject {

public:

  // Methods PUBLIC
  // 

  ISession2D_Curve
    (const Handle(Geom2d_Curve) aGeom2dCurve,
    const Aspect_TypeOfLine aTypeOfline = Aspect_TOL_SOLID,
    const Aspect_WidthOfLine aWidthOfLine = Aspect_WOL_MEDIUM,
    const Standard_Integer aColorIndex = 4);

  inline   Standard_Integer   NbPossibleSelection() const;

  inline   Aspect_TypeOfLine  GetTypeOfLine() const;
  inline   void SetTypeOfLine(const Aspect_TypeOfLine aNewTypeOfLine);

  inline   Aspect_WidthOfLine GetWidthOfLine() const;
  inline   void SetWidthOfLine(const Aspect_WidthOfLine aNewWidthOfLine);

  inline   Standard_Integer GetColorIndex() const;
  inline   void SetColorIndex(const Standard_Integer aNewColorIndex) ;

  inline   Standard_Boolean GetDisplayPole() const;
  inline   void SetDisplayPole(const Standard_Boolean aNewDisplayPole) ;

  inline   Standard_Boolean ISession2D_Curve::GetDisplayCurbure() const;
  inline   void ISession2D_Curve::SetDisplayCurbure
                (const Standard_Boolean aNewDisplayCurbure);

  inline   Standard_Real GetDiscretisation() const;
  inline   void SetDiscretisation(const Standard_Real aNewDiscretisation) ;

  DEFINE_STANDARD_RTTIEXT(ISession2D_Curve,AIS_InteractiveObject)

private: 

  // Methods PRIVATE
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const { return theMode == 0; }
  virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode = 0) ;
  void ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer ) {}

  // Fields PRIVATE
  //
  Handle(Geom2d_Curve) myGeom2dCurve;
  Aspect_TypeOfLine   myTypeOfLine;
  Aspect_WidthOfLine  myWidthOfLine;
  Standard_Integer    myColorIndex;
  Standard_Boolean    myDisplayPole;
  Standard_Boolean    myDisplayCurbure;
  Standard_Real       myDiscretisation;
  Standard_Real       myradiusmax ;
  Standard_Real       myradiusratio ;
};



// other inCurve functions and methods (like "C++: function call" methods)
//

inline   Standard_Integer  ISession2D_Curve::NbPossibleSelection() const
{
  return 1;
}

inline  Aspect_TypeOfLine ISession2D_Curve::GetTypeOfLine() const
{
  return myTypeOfLine ;
}

inline  void ISession2D_Curve::SetTypeOfLine(const Aspect_TypeOfLine aNewTypeOfLine) 
{
  myTypeOfLine = aNewTypeOfLine;
}

inline  Aspect_WidthOfLine ISession2D_Curve::GetWidthOfLine() const
{
  return myWidthOfLine ;
}

inline  void ISession2D_Curve::SetWidthOfLine(const Aspect_WidthOfLine aNewWidthOfLine) 
{
  myWidthOfLine = aNewWidthOfLine;
}

inline  Standard_Integer ISession2D_Curve::GetColorIndex() const
{
  return myColorIndex ;
}

inline  void ISession2D_Curve::SetColorIndex(const Standard_Integer aNewColorIndex) 
{
  myColorIndex = aNewColorIndex;
}

inline   Standard_Boolean  ISession2D_Curve::GetDisplayPole
() const
{
  return myDisplayPole;
}
inline   void               ISession2D_Curve::SetDisplayPole
(const Standard_Boolean aNewDisplayPole)
{
  myDisplayPole = aNewDisplayPole;
}

inline   Standard_Boolean  ISession2D_Curve::GetDisplayCurbure
() const
{
  return myDisplayCurbure;
}
inline   void               ISession2D_Curve::SetDisplayCurbure
(const Standard_Boolean aNewDisplayCurbure)
{
  myDisplayCurbure = aNewDisplayCurbure;
}


inline   Standard_Real  ISession2D_Curve::GetDiscretisation
() const
{
  return myDiscretisation;
}
inline   void               ISession2D_Curve::SetDiscretisation
(const Standard_Real aNewDiscretisation) 
{
  myDiscretisation = aNewDiscretisation;
}



#endif

