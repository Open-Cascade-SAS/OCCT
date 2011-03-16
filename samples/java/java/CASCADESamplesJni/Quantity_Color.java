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

import CASCADESamplesJni.Standard_Storable;
import CASCADESamplesJni.Quantity_NameOfColor;
import jcas.Standard_Short;
import jcas.Standard_Real;
import CASCADESamplesJni.Quantity_TypeOfColor;
import jcas.Standard_Boolean;
import jcas.Standard_CString;


public class Quantity_Color extends CASCADESamplesJni.Standard_Storable {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Quantity_Color() {
   Quantity_Color_Create_1();
}

private final native void Quantity_Color_Create_1();

public Quantity_Color(short AName) {
   Quantity_Color_Create_2(AName);
}

private final native void Quantity_Color_Create_2(short AName);

public Quantity_Color(double R1,double R2,double R3,short AType) {
   Quantity_Color_Create_3(R1,R2,R3,AType);
}

private final native void Quantity_Color_Create_3(double R1,double R2,double R3,short AType);

native public final Quantity_Color Assign(Quantity_Color Other);
native public final void ChangeContrast(double ADelta);
native public final void ChangeIntensity(double ADelta);
final public void SetValues(short AName) {
    Quantity_Color_SetValues_1(AName);
}

private final native void Quantity_Color_SetValues_1(short AName);

final public void SetValues(double R1,double R2,double R3,short AType) {
    Quantity_Color_SetValues_2(R1,R2,R3,AType);
}

private final native void Quantity_Color_SetValues_2(double R1,double R2,double R3,short AType);

native public final void Delta(Quantity_Color AColor,Standard_Real DC,Standard_Real DI);
native public final double Distance(Quantity_Color AColor);
native public final double SquareDistance(Quantity_Color AColor);
native public final double Blue();
native public final double Green();
native public final double Hue();
native public final boolean IsDifferent(Quantity_Color Other);
native public final boolean IsEqual(Quantity_Color Other);
native public final double Light();
final public short Name() {
   return Quantity_Color_Name_1();
}

private final native short Quantity_Color_Name_1();

native public final double Red();
native public final double Saturation();
native public final void Values(Standard_Real R1,Standard_Real R2,Standard_Real R3,short AType);
native public static void SetEpsilon(double AnEpsilon);
native public static double Epsilon();
static public short Name(double R,double G,double B) {
   return Quantity_Color_Name_2(R,G,B);
}

private static native short Quantity_Color_Name_2(double R,double G,double B);

native public static Standard_CString StringName(short AColor);
native public static void HlsRgb(double H,double L,double S,Standard_Real R,Standard_Real G,Standard_Real B);
native public static void RgbHls(double R,double G,double B,Standard_Real H,Standard_Real L,Standard_Real S);
native public static void Test();


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
