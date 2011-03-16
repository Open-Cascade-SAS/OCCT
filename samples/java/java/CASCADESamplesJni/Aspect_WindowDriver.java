// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package CASCADESamplesJni;

import jcas.Object;
import jcas.Standard_Boolean;
import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_TypeOfResize;
import CASCADESamplesJni.Aspect_Window;
import CASCADESamplesJni.Aspect_TypeOfDrawMode;
import jcas.Standard_Short;
import jcas.Standard_ShortReal;
import jcas.Standard_Real;
import CASCADESamplesJni.TCollection_ExtendedString;
import jcas.Standard_CString;


public class Aspect_WindowDriver extends CASCADESamplesJni.Aspect_Driver {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public  void BeginDraw(boolean DoubleBuffer,int aRetainBuffer);
native public  short ResizeSpace();
native public final Aspect_Window Window();
native public  void SetDrawMode(short aMode);
native public  boolean OpenBuffer(int aRetainBuffer,float aPivotX,float aPivotY,int aWidthIndex,int aColorIndex,int aFontIndex,short aDrawMode);
native public  void CloseBuffer(int aRetainBuffer);
native public  void ClearBuffer(int aRetainBuffer);
native public  void DrawBuffer(int aRetainBuffer);
native public  void EraseBuffer(int aRetainBuffer);
native public  void MoveBuffer(int aRetainBuffer,float aPivotX,float aPivotY);
native public  void ScaleBuffer(int aRetainBuffer,double aScaleX,double aScaleY);
native public  void RotateBuffer(int aRetainBuffer,double anAngle);
native public  boolean BufferIsOpen(int aRetainBuffer);
native public  boolean BufferIsEmpty(int aRetainBuffer);
native public  boolean BufferIsDrawn(int aRetainBuffer);
native public  void AngleOfBuffer(int aRetainBuffer,Standard_Real anAngle);
native public  void ScaleOfBuffer(int aRetainBuffer,Standard_Real aScaleX,Standard_Real aScaleY);
native public  void PositionOfBuffer(int aRetainBuffer,Standard_ShortReal aPivotX,Standard_ShortReal aPivotY);
 public void TextSize(TCollection_ExtendedString aText,Standard_ShortReal aWidth,Standard_ShortReal aHeight,int aFontIndex) {
    Aspect_WindowDriver_TextSize_1(aText,aWidth,aHeight,aFontIndex);
}

private  native void Aspect_WindowDriver_TextSize_1(TCollection_ExtendedString aText,Standard_ShortReal aWidth,Standard_ShortReal aHeight,int aFontIndex);

 public void TextSize(TCollection_ExtendedString aText,Standard_ShortReal aWidth,Standard_ShortReal aHeight,Standard_ShortReal anXoffset,Standard_ShortReal anYoffset,int aFontIndex) {
    Aspect_WindowDriver_TextSize_2(aText,aWidth,aHeight,anXoffset,anYoffset,aFontIndex);
}

private  native void Aspect_WindowDriver_TextSize_2(TCollection_ExtendedString aText,Standard_ShortReal aWidth,Standard_ShortReal aHeight,Standard_ShortReal anXoffset,Standard_ShortReal anYoffset,int aFontIndex);

native public  Standard_CString FontSize(Standard_Real aSlant,Standard_ShortReal aSize,Standard_ShortReal aBheight,int aFontIndex);
native public  void ColorBoundIndexs(Standard_Integer aMinIndex,Standard_Integer aMaxIndex);
native public  int LocalColorIndex(int anIndex);
native public  void FontBoundIndexs(Standard_Integer aMinIndex,Standard_Integer aMaxIndex);
native public  int LocalFontIndex(int anIndex);
native public  void TypeBoundIndexs(Standard_Integer aMinIndex,Standard_Integer aMaxIndex);
native public  int LocalTypeIndex(int anIndex);
native public  void WidthBoundIndexs(Standard_Integer aMinIndex,Standard_Integer aMaxIndex);
native public  int LocalWidthIndex(int anIndex);
native public  void MarkBoundIndexs(Standard_Integer aMinIndex,Standard_Integer aMaxIndex);
native public  int LocalMarkIndex(int anIndex);
public Aspect_WindowDriver() {
}




}
