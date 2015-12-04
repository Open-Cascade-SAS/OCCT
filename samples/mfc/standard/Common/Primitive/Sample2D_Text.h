#ifndef _Sample2D_Text_HeaderFile
#define _Sample2D_Text_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <TCollection_AsciiString.hxx>

class Sample2D_Text;
DEFINE_STANDARD_HANDLE(Sample2D_Text,AIS_InteractiveObject)
class Sample2D_Text : public AIS_InteractiveObject 
{

public:

Standard_EXPORT 
  Sample2D_Text  (const TCollection_AsciiString& theText,const gp_Pnt&  thePosition,
                 const Quantity_PlaneAngle      theAngle      = 0.0 ,
                 const Quantity_Color           theColor       = Quantity_NOC_YELLOW,
                 const Font_FontAspect          theFontAspect  = Font_FA_Regular,
                 const Standard_CString         theFont        = "Courier",
                 const Standard_Real          theHeight       = 1,
                 const Graphic3d_HorizontalTextAlignment  theHAlign      = Graphic3d_HTA_LEFT,
                 const Graphic3d_VerticalTextAlignment  theVAlign      = Graphic3d_VTA_BOTTOM,
                 const Standard_Boolean         theIsZoomable  = Standard_True);

inline   TCollection_AsciiString GetText() const {  return myText ; }
inline   void                    SetText(const TCollection_AsciiString& theText) {  myText = theText; }
inline   void                    GetCoord(gp_Pnt& thePosition) const {  thePosition=gp_Pnt(myPosition); }
inline   void                    SetCoord(const gp_Pnt& thePosition) {  myPosition = thePosition; }
inline   Standard_Real           GetAngle() const {  return myAngle; }
inline   void                    SetAngle(const Standard_Real theAngle) {  myAngle = theAngle; }
inline   Quantity_Color          GetColor() const {  return myColor; }
inline   void                    SetColor(const Quantity_Color theColor) {  myColor = theColor; }
inline   Quantity_Factor         GetScale() const {  return myScale; }
inline   void                    SetScale  (const Quantity_Factor theScale) {  myScale  = theScale; }
inline   Standard_Boolean        GetIsZoomable() const { return myIsZoomable; }
inline   void                    SetIsZoomable(const Standard_Boolean theIsZoomable) { myIsZoomable = theIsZoomable;}


DEFINE_STANDARD_RTTIEXT(Sample2D_Text,AIS_InteractiveObject)

private: 
 
 void Compute (  const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                  const Handle(Prs3d_Presentation)& aPresentation,
                  const Standard_Integer aMode);

 void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                        const Standard_Integer /*aMode*/)
 {

 }


TCollection_AsciiString   myText            ; 
gp_Pnt                    myPosition        ;
Standard_Real             myAngle           ;
Quantity_Color            myColor           ;
Standard_CString          myFont            ;
Quantity_Factor           myScale           ;
Standard_Real             myHeight          ;
Font_FontAspect           myFontAspect      ; 
Standard_Boolean          myIsZoomable      ;
Graphic3d_HorizontalTextAlignment   myHAlign ;
Graphic3d_VerticalTextAlignment     myVAlign ;

};

#endif
