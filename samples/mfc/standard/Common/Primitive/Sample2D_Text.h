#ifndef _Sample2D_Text_HeaderFile
#define _Sample2D_Text_HeaderFile
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>



DEFINE_STANDARD_HANDLE(Sample2D_Text,AIS2D_InteractiveObject)
class Sample2D_Text : public AIS2D_InteractiveObject {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT Sample2D_Text   (const TCollection_AsciiString& aText,
                 const Standard_Real            anX          = 0   ,
                 const Standard_Real            anY          = 0   ,
                 const Aspect_TypeOfText        aType        = Aspect_TOT_SOLID,
                 const Quantity_PlaneAngle      anAngle      = 0.0 ,
                 const Standard_Real            aSlant       = 0.0 ,
                 const Standard_Integer         aColorIndex  = 0   ,
                 const Standard_Integer         aFontIndex   = 1   ,
                 const Quantity_Factor          aScale       = 1   ,
                 const Standard_Boolean         aIsUnderline = Standard_False,
                 const Standard_Boolean         aIsZoomable  = Standard_True,
                 const Graphic2d_TypeOfAlignment aTypeOfAlignment = Graphic2d_TOA_LEFT);

Standard_EXPORT virtual void SetContext(const Handle(AIS2D_InteractiveContext)& theContext);

inline   TCollection_AsciiString GetText() const {  return myText ; }
inline   void                    SetText(const TCollection_AsciiString& atext) {  myText = atext; }
inline   void                    GetCoord(Standard_Real& X, Standard_Real& Y) const {  X = myX;  Y = myY; }
inline   void                    SetCoord(const Standard_Real X, const Standard_Real Y) {  myX = X ;  myY = Y ; }
inline   Aspect_TypeOfText       GetTypeOfText() const {  return myTypeOfText; }
inline   void                    SetTypeOfText(const Aspect_TypeOfText aNewTypeOfText) {  myTypeOfText = aNewTypeOfText; }
inline   Standard_Real           GetAngle() const {  return myAngle; }
inline   void                    SetAngle(const Standard_Real aNewAngle) {  myAngle = aNewAngle; }
inline   Standard_Real           GetSlant() const {  return mySlant; }
inline   void                    SetSlant(const Standard_Real aNewSlant) {  mySlant = aNewSlant; }
inline   Standard_Integer        GetColorIndex() const {  return myColorIndex; }
inline   void                    SetColorIndex(const Standard_Integer aNewColorIndex) {  myColorIndex = aNewColorIndex; }
inline   Standard_Integer        GetFontIndex() const { return myFontIndex; }
inline   void                    SetFontIndex(const Standard_Integer aNewFontIndex) {  myFontIndex = aNewFontIndex; }
inline   Quantity_Factor         GetScale() const {  return myScale; }
inline   void                    SetScale  (const Quantity_Factor aNewScale) {  myScale  = aNewScale; }
inline   Standard_Boolean        GetIsUnderline() const {return myIsUnderline;}
inline   void                    SetIsUnderline(const Standard_Boolean aNewIsUnderline) { myIsUnderline = aNewIsUnderline;}
inline   Standard_Boolean        GetIsZoomable() const { return myIsZoomable; }
inline   void                    SetIsZoomable(const Standard_Boolean aNewIsZoomable) { myIsZoomable = aNewIsZoomable;}
inline   Graphic2d_TypeOfAlignment GetTypeOfAlignment() const { return myTypeOfAlignment;}
inline   void                    SetTypeOfAlignment(const Graphic2d_TypeOfAlignment aNewTypeOfAlignment) { myTypeOfAlignment= aNewTypeOfAlignment;}



DEFINE_STANDARD_RTTI(Sample2D_Text)


protected:


 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 

 // Fields PRIVATE
 //
TCollection_AsciiString   myText            ; 
Standard_Real             myX               ;
Standard_Real             myY               ;
Aspect_TypeOfText         myTypeOfText      ;
Standard_Real             myAngle           ;
Standard_Real             mySlant           ;
Standard_Integer          myColorIndex      ;
Standard_Integer          myFontIndex       ;
Quantity_Factor           myScale           ;
Standard_Real             myWidth           ;
Standard_Real             myHeight          ;
Standard_Real             myXoffset         ;
Standard_Real             myYoffset         ;

Standard_Boolean          myIsUnderline     ; 
Standard_Boolean          myIsZoomable      ;
Graphic2d_TypeOfAlignment myTypeOfAlignment ;
};

// other inline functions and methods (like "C++: function call" methods)
//


#endif
