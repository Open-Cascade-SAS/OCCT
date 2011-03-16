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
import CASCADESamplesJni.Aspect_ColorMap;
import CASCADESamplesJni.Aspect_TypeMap;
import CASCADESamplesJni.Aspect_WidthMap;
import CASCADESamplesJni.Aspect_FontMap;
import CASCADESamplesJni.Aspect_MarkMap;
import jcas.Standard_Integer;
import jcas.Standard_Real;
import CASCADESamplesJni.Standard_Transient;
import jcas.Standard_CString;
import jcas.Standard_ShortReal;
import CASCADESamplesJni.TShort_Array1OfShortReal;
import CASCADESamplesJni.TCollection_ExtendedString;
import CASCADESamplesJni.Aspect_TypeOfText;
import jcas.Standard_Short;


public class Aspect_Driver extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public  void EndDraw(boolean Synchronize);
native public final void SetColorMap(Aspect_ColorMap aColorMap);
native public final void SetTypeMap(Aspect_TypeMap aTypeMap);
native public final void SetWidthMap(Aspect_WidthMap aWidthMap);
native public final void SetFontMap(Aspect_FontMap aFontMap,boolean useMFT);
native public final void SetMarkMap(Aspect_MarkMap aMarkMap);
native public  void SetLineAttrib(int ColorIndex,int TypeIndex,int WidthIndex);
 public void SetTextAttrib(int ColorIndex,int FontIndex) {
    Aspect_Driver_SetTextAttrib_1(ColorIndex,FontIndex);
}

private  native void Aspect_Driver_SetTextAttrib_1(int ColorIndex,int FontIndex);

 public void SetTextAttrib(int ColorIndex,int FontIndex,double aSlant,double aHScale,double aWScale,boolean isUnderlined) {
    Aspect_Driver_SetTextAttrib_2(ColorIndex,FontIndex,aSlant,aHScale,aWScale,isUnderlined);
}

private  native void Aspect_Driver_SetTextAttrib_2(int ColorIndex,int FontIndex,double aSlant,double aHScale,double aWScale,boolean isUnderlined);

native public  void SetPolyAttrib(int ColorIndex,int TileIndex,boolean DrawEdge);
native public  void SetMarkerAttrib(int ColorIndex,int WidthIndex,boolean FillMarker);
native public  boolean IsKnownImage(Standard_Transient anImage);
native public  boolean SizeOfImageFile(Standard_CString anImageFile,Standard_Integer aWidth,Standard_Integer aHeight);
native public  void ClearImage(Standard_Transient anImageId);
native public  void ClearImageFile(Standard_CString anImageFile);
native public  void DrawImage(Standard_Transient anImageId,float aX,float aY);
native public  void DrawImageFile(Standard_CString anImageFile,float aX,float aY,double aScale);
native public  void DrawPolyline(TShort_Array1OfShortReal aListX,TShort_Array1OfShortReal aListY);
native public  void DrawPolygon(TShort_Array1OfShortReal aListX,TShort_Array1OfShortReal aListY);
native public  void DrawSegment(float X1,float Y1,float X2,float Y2);
native public  void DrawText(TCollection_ExtendedString aText,float Xpos,float Ypos,float anAngle,short aType);
native public  void DrawPolyText(TCollection_ExtendedString aText,float Xpos,float Ypos,double aMarge,float anAngle,short aType);
native public  void DrawPoint(float X,float Y);
native public  void DrawMarker(int aMarker,float Xpos,float Ypos,float Width,float Height,float Angle);
native public  boolean DrawArc(float X,float Y,float anXradius,float anYradius,float aStartAngle,float anOpenAngle);
native public  boolean DrawPolyArc(float X,float Y,float anXradius,float anYradius,float aStartAngle,float anOpenAngle);
native public  void BeginPolyline(int aNumber);
native public  void BeginPolygon(int aNumber);
native public  void BeginSegments();
native public  void BeginArcs();
native public  void BeginPolyArcs();
native public  void BeginMarkers();
native public  void BeginPoints();
native public  void ClosePrimitive();
native public final Aspect_ColorMap ColorMap();
native public final Aspect_TypeMap TypeMap();
native public final Aspect_WidthMap WidthMap();
native public final Aspect_FontMap FontMap();
native public final Aspect_MarkMap MarkMap();
native public  void WorkSpace(Standard_Real Width,Standard_Real Heigth);
 public double Convert(int PV) {
   return Aspect_Driver_Convert_1(PV);
}

private  native double Aspect_Driver_Convert_1(int PV);

 public int Convert(double DV) {
   return Aspect_Driver_Convert_2(DV);
}

private  native int Aspect_Driver_Convert_2(double DV);

 public void Convert(int PX,int PY,Standard_Real DX,Standard_Real DY) {
    Aspect_Driver_Convert_3(PX,PY,DX,DY);
}

private  native void Aspect_Driver_Convert_3(int PX,int PY,Standard_Real DX,Standard_Real DY);

 public void Convert(double DX,double DY,Standard_Integer PX,Standard_Integer PY) {
    Aspect_Driver_Convert_4(DX,DY,PX,PY);
}

private  native void Aspect_Driver_Convert_4(double DX,double DY,Standard_Integer PX,Standard_Integer PY);

native public final boolean UseMFT();
public Aspect_Driver() {
}




}
