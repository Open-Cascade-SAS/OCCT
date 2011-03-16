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
import CASCADESamplesJni.Aspect_Background;
import CASCADESamplesJni.Quantity_NameOfColor;
import jcas.Standard_Short;
import jcas.Standard_Boolean;
import jcas.Standard_CString;
import CASCADESamplesJni.Aspect_FillMethod;
import CASCADESamplesJni.Aspect_TypeOfResize;
import jcas.Standard_Integer;
import jcas.Standard_Real;
import CASCADESamplesJni.Aspect_GraphicDevice;


public class Aspect_Window extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
 public void SetBackground(Aspect_Background ABack) {
    Aspect_Window_SetBackground_1(ABack);
}

private  native void Aspect_Window_SetBackground_1(Aspect_Background ABack);

 public void SetBackground(short BackColor) {
    Aspect_Window_SetBackground_2(BackColor);
}

private  native void Aspect_Window_SetBackground_2(short BackColor);

 public boolean SetBackground(Standard_CString aName,short aMethod) {
   return Aspect_Window_SetBackground_3(aName,aMethod);
}

private  native boolean Aspect_Window_SetBackground_3(Standard_CString aName,short aMethod);

native public  void SetDoubleBuffer(boolean DBmode);
native public  void Flush();
native public  void Map();
native public  void Unmap();
native public  short DoResize();
native public  boolean DoMapping();
native public  void Destroy();
native public  void Clear();
native public  void ClearArea(int XCenter,int YCenter,int Width,int Height);
native public  void Restore();
native public  void RestoreArea(int XCenter,int YCenter,int Width,int Height);
native public  boolean Dump(Standard_CString aFilename,double aGammaValue);
native public  boolean DumpArea(Standard_CString aFilename,int Xc,int Yc,int Width,int Height,double aGammaValue);
native public  boolean Load(Standard_CString aFilename);
native public  boolean LoadArea(Standard_CString aFilename,int Xc,int Yc,int Width,int Height);
native public final Aspect_Background Background();
native public final Standard_CString BackgroundImage();
native public final short BackgroundFillMethod();
native public final Aspect_GraphicDevice GraphicDevice();
native public  boolean IsMapped();
native public  double Ratio();
 public void Position(Standard_Real X1,Standard_Real Y1,Standard_Real X2,Standard_Real Y2) {
    Aspect_Window_Position_1(X1,Y1,X2,Y2);
}

private  native void Aspect_Window_Position_1(Standard_Real X1,Standard_Real Y1,Standard_Real X2,Standard_Real Y2);

 public void Position(Standard_Integer X1,Standard_Integer Y1,Standard_Integer X2,Standard_Integer Y2) {
    Aspect_Window_Position_2(X1,Y1,X2,Y2);
}

private  native void Aspect_Window_Position_2(Standard_Integer X1,Standard_Integer Y1,Standard_Integer X2,Standard_Integer Y2);

 public void Size(Standard_Real Width,Standard_Real Height) {
    Aspect_Window_Size_1(Width,Height);
}

private  native void Aspect_Window_Size_1(Standard_Real Width,Standard_Real Height);

 public void Size(Standard_Integer Width,Standard_Integer Height) {
    Aspect_Window_Size_2(Width,Height);
}

private  native void Aspect_Window_Size_2(Standard_Integer Width,Standard_Integer Height);

native public  void MMSize(Standard_Real Width,Standard_Real Height);
 public double Convert(int PV) {
   return Aspect_Window_Convert_1(PV);
}

private  native double Aspect_Window_Convert_1(int PV);

 public int Convert(double DV) {
   return Aspect_Window_Convert_2(DV);
}

private  native int Aspect_Window_Convert_2(double DV);

 public void Convert(int PX,int PY,Standard_Real DX,Standard_Real DY) {
    Aspect_Window_Convert_3(PX,PY,DX,DY);
}

private  native void Aspect_Window_Convert_3(int PX,int PY,Standard_Real DX,Standard_Real DY);

 public void Convert(double DX,double DY,Standard_Integer PX,Standard_Integer PY) {
    Aspect_Window_Convert_4(DX,DY,PX,PY);
}

private  native void Aspect_Window_Convert_4(double DX,double DY,Standard_Integer PX,Standard_Integer PY);

native public  boolean BackingStore();
native public  boolean DoubleBuffer();
public Aspect_Window() {
}




}
