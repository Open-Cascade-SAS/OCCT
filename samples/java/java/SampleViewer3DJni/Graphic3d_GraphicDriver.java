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


package SampleViewer3DJni;

import jcas.Object;
import jcas.Standard_CString;
import jcas.Standard_Boolean;
import jcas.Standard_Integer;
import jcas.Standard_ShortReal;
import CASCADESamplesJni.TColStd_Array2OfReal;
import SampleViewer3DJni.Graphic3d_TypeOfComposition;
import jcas.Standard_Short;
import SampleViewer3DJni.Graphic3d_TypeOfTexture;
import SampleViewer3DJni.AlienImage_AlienImage;


public class Graphic3d_GraphicDriver extends SampleViewer3DJni.Aspect_GraphicDriver {

 static {
    System.loadLibrary("SampleViewer3DJni");
 }
            
public Graphic3d_GraphicDriver(Standard_CString AShrName) {
   Graphic3d_GraphicDriver_Create_0(AShrName);
}

private final native void Graphic3d_GraphicDriver_Create_0(Standard_CString AShrName);

native public  boolean Begin(Standard_CString ADisplay);
native public  void End();
native public  int InquireLightLimit();
native public  int InquirePlaneLimit();
native public  int InquireViewLimit();
native public  boolean InquireTextureAvailable();
native public  void InitPick();
native public  void EndAddMode();
native public  void BeginPolyline();
 public void Draw(float X,float Y,float Z) {
    Graphic3d_GraphicDriver_Draw_1(X,Y,Z);
}

private  native void Graphic3d_GraphicDriver_Draw_1(float X,float Y,float Z);

native public  void EndImmediatMode(int Synchronize);
native public  void EndPolyline();
 public void Move(float X,float Y,float Z) {
    Graphic3d_GraphicDriver_Move_1(X,Y,Z);
}

private  native void Graphic3d_GraphicDriver_Move_1(float X,float Y,float Z);

native public  void SetLineColor(float R,float G,float B);
native public  void SetLineType(int Type);
native public  void SetLineWidth(float Width);
native public  void SetMinMax(float X1,float Y1,float Z1,float X2,float Y2,float Z2);
native public  void Transform(TColStd_Array2OfReal AMatrix,short AType);
native public  int CreateTexture(short Type,AlienImage_AlienImage Image,Standard_CString FileName);
native public  void DestroyTexture(int TexId);
native public  void BeginPolygon2d();
native public  void BeginPolyline2d();
 public void Draw(float X,float Y) {
    Graphic3d_GraphicDriver_Draw_2(X,Y);
}

private  native void Graphic3d_GraphicDriver_Draw_2(float X,float Y);

native public  void Edge(float X,float Y);
native public  void EndLayer();
native public  void EndPolygon2d();
native public  void EndPolyline2d();
 public void Move(float X,float Y) {
    Graphic3d_GraphicDriver_Move_2(X,Y);
}

private  native void Graphic3d_GraphicDriver_Move_2(float X,float Y);

native public  void Rectangle(float X,float Y,float Width,float Height);
native public  void SetColor(float R,float G,float B);
native public  void SetLineAttributes(int Type,float Width);
native public  void SetTextAttributes(int Font,int Type,float R,float G,float B);
 public void Text(Standard_CString AText,float X,float Y,float AHeight) {
    Graphic3d_GraphicDriver_Text_5(AText,X,Y,AHeight);
}

private  native void Graphic3d_GraphicDriver_Text_5(Standard_CString AText,float X,float Y,float AHeight);

native public final void PrintBoolean(Standard_CString AComment,boolean AValue);
native public final void PrintFunction(Standard_CString AFunc);
native public final void PrintInteger(Standard_CString AComment,int AValue);
native public final void PrintIResult(Standard_CString AFunc,int AResult);
native public final void PrintShortReal(Standard_CString AComment,float AValue);
native public final void PrintMatrix(Standard_CString AComment,TColStd_Array2OfReal AMatrix);
native public final void PrintString(Standard_CString AComment,Standard_CString AString);
native public final void SetTrace(int ALevel);
native public final int Trace();
public Graphic3d_GraphicDriver() {
}




}
