// File:	CASCADEView3d.cxx
// Created:	Wed Jul 18 11:11:22 2001
// Author:	
//		<avo@TIREX>


#ifdef WNT
# include <windows.h>
#endif

#include <Standard_Stream.hxx>

#include <jawt_md.h>

#include <CASCADEView3d.hxx>

#include <jcas.hxx>
#include <V3d_View.hxx>
#ifdef WNT
# include <WNT_Window.hxx>
# include <Graphic3d_WNTGraphicDevice.hxx>
#else
# include <Xw_Window.hxx>
# include <Graphic3d_GraphicDevice.hxx>
#endif  // WNT

#define RETURN_ERROR(msg) {cout << "Error: '"<< msg <<"'" << endl;return;}

// #include "Aspect_Handle.hxx>

/*
 * Class:     CASCADEView3d
 * Method:    paint
 * Signature: (Ljava/awt/Graphics;)V
 */
JNIEXPORT void JNICALL Java_CASCADEView3d_paint (JNIEnv *env, jobject theCanvas, jobject theGraphics){
//   cout << "Info: -------------------------- Java_CASCADEView3d_paint ----------------------" << endl;

  jclass jViewPort3dClass = env->FindClass("CASCADEView3d");
  if(!jViewPort3dClass)RETURN_ERROR("cant find canvas class");
  jfieldID jIsWindowID = env->GetFieldID(jViewPort3dClass, "hasWindow", "Z");
  if(!jIsWindowID)RETURN_ERROR("cant find the boolean field id");
  jboolean jIsWindow = env->GetBooleanField(theCanvas, jIsWindowID);

  Handle(V3d_View) theView;
  Handle(V3d_Viewer) theViewer;

  if(!jIsWindow){
    // get the window view and viewer
//     cout << "Info: First time, init fields." << endl;
    jfieldID jViewID = env->GetFieldID(jViewPort3dClass, "myView", "LCASCADESamplesJni/V3d_View;");
    if(!jViewID) RETURN_ERROR("cant find the view field id");
    jobject jView = env->GetObjectField(theCanvas, jViewID);

    if(!jView) { // viewer and view are not created, create them now.
      Standard_ExtString aViewerName = TCollection_ExtendedString ("V3d_Viewer in SimpleViewer").ToExtString();
#ifdef WNT
      static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
      if (defaultDevice.IsNull()) 
	defaultDevice = new Graphic3d_WNTGraphicDevice();

      theViewer = new V3d_Viewer(defaultDevice, aViewerName);
#else
      static Handle(Graphic3d_GraphicDevice) defaultDevice;
    
      if (defaultDevice.IsNull()) 
	defaultDevice = new Graphic3d_GraphicDevice("");
      theViewer = new V3d_Viewer(defaultDevice, aViewerName);
#endif //WNT
      theView = theViewer->CreateView();
      // store the values into the fields
    }else {
      void *ptrView = jcas_GetHandle(env, jView);
      if (ptrView != NULL) {
	theView = *((Handle(V3d_View)*)ptrView);
      }else{
	cout << "Error: cant get V3d_View from canvas field" << endl;
      }
    }

// start paste
    if (!theView.IsNull()){
	  // get drawing surface
	  //-----------------------------------------------
	  JAWT awt;
	  JAWT_DrawingSurface* ds;
	  JAWT_DrawingSurfaceInfo* dsi;
#ifdef WNT
	  JAWT_Win32DrawingSurfaceInfo* dsi_win;
#else
	  JAWT_X11DrawingSurfaceInfo* dsi_x11;
#endif //WNT
	  jboolean result;
	  jint lock;

	  // Get the AWT
	  awt.version = JAWT_VERSION_1_3;
	  result = JAWT_GetAWT(env, &awt);
	  if (result == JNI_FALSE)
	    {
	      cout << "AWT not found" << endl;
	      jcas_ThrowException(env, "AWT not found\n");
//	      alock.Release();
	      return;
	    }

	  // Get the drawing surface
	  ds = awt.GetDrawingSurface(env, theCanvas);
	  if (ds == NULL)
	    {
	      cout << "NULL drawing surface" << endl;
	      jcas_ThrowException(env, "NULL drawing surface\n");
	      return;
	    }

	  // Lock the drawing surface
	  lock = ds->Lock(ds);
	  if ((lock & JAWT_LOCK_ERROR) != 0)
	    {
	      cout << "Error locking surface" << endl;
	      jcas_ThrowException(env, "Error locking surface\n");
	      awt.FreeDrawingSurface(ds);
	      return;
	    }

	  // Get the drawing surface info
	  dsi = ds->GetDrawingSurfaceInfo(ds);
	  if (dsi == NULL)
	    {
	      cout << "Error getting surface info" << endl;
	      jcas_ThrowException(env, "Error getting surface info\n");
	      ds->Unlock(ds);
	      awt.FreeDrawingSurface(ds);
//	      alock.Release();
	      return;
	    }

	  // Get the platform-specific drawing info
	  Aspect_Handle theWindow;
#ifdef WNT
	  dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
	  theWindow = dsi_win->hwnd;
#else
	  dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;
	  theWindow = dsi_x11->drawable;
#endif

	      if (jIsWindow == JNI_FALSE)
		{
#ifdef WNT
		  long wd = GetWindowLong (( HWND )theWindow, GWL_USERDATA);
		  long wProc = GetWindowLong (( HWND )theWindow, GWL_WNDPROC);
#endif


#ifdef WNT
		  Handle(WNT_Window) w = new WNT_Window(Handle(Graphic3d_WNTGraphicDevice)::DownCast(theView->Viewer()->Device()),theWindow);
#else
		  Handle(Xw_Window) w = new Xw_Window(Handle(Graphic3d_GraphicDevice)::DownCast(theView->Viewer()->Device()),theWindow,Xw_WQ_SAMEQUALITY);
#endif
//		  cout << "Info: Finally, set window:" << endl;
		  theView->SetWindow(w); 
//		  cout << "Info: set window done" << endl;

#ifdef WNT
		  long wd1 = SetWindowLong ((HWND) theWindow, GWL_USERDATA, wd);
		  long wProc1 = SetWindowLong ((HWND) theWindow, GWL_WNDPROC, wProc);
#endif

		  jIsWindow = JNI_TRUE;
		}
	      
	  env->SetBooleanField(theCanvas, jIsWindowID, jIsWindow);

// 	  jobject jView = env->GetObjectField(theCanvas, jViewID);
// 	  jobject jNewView = ;
// 	  env->SetObjectField(theCanvas, jViewID, jNewView); // remember V3d_View
/******************************
fill fields with viewer and viev
*******************************/

// 	  jclass testClass = env->FindClass("CASCADESamplesJni/V2d_Viewer");
// 	  cout << "Info: V3d fil lthe fields : get V2d_Viewer class : "<<testClass << endl;

	  Handle(V3d_View)* theViewPtr = new Handle(V3d_View);
	  *theViewPtr = theView;
	  jobject aJavaView = jcas_CreateObject(env,"CASCADESamplesJni/V3d_View", theViewPtr);
	  env->SetObjectField(theCanvas, jViewID, aJavaView);

	  Handle(V3d_Viewer)* theViewerPtr = new Handle(V3d_Viewer);
	  *theViewerPtr = theViewer;
	  jfieldID jViewerID = env->GetFieldID(jViewPort3dClass, "myViewer", "LCASCADESamplesJni/V3d_Viewer;");
	  jobject aJavaViewer = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Viewer", theViewerPtr);
	  env->SetObjectField(theCanvas, jViewerID, aJavaViewer);

	  // fill the myView and myViewer fields.

	  // Free the AWT
	  //-----------------------------------------------

	  // Free the drawing surface info
	  ds->FreeDrawingSurfaceInfo(dsi);

	  // Unlock the drawing surface
	  ds->Unlock(ds);

	  // Free the drawing surface
	  awt.FreeDrawingSurface(ds);
	}else{
	  cout << "Error: Cant obtain V3d_View" << endl;
	}
/////////////////////////// end paste
//   cout << "Info: Init viewer done" << endl;
} // end !hasWindow. 

// get stored view field
  jfieldID jViewID = env->GetFieldID(jViewPort3dClass, "myView", "LCASCADESamplesJni/V3d_View;");
  jobject jView = env->GetObjectField(theCanvas, jViewID);
  if(!jView){
    jcas_ThrowException(env, "Error getting viewer while has window ?\n");
    return;
  }
  void *ptrView = jcas_GetHandle(env, jView);
  if (ptrView != NULL)
    theView = *((Handle(V3d_View)*)ptrView);

// paint routine.
  if(theView.IsNull()){
    jcas_ThrowException(env, "Error getting V3d_View ?\n");
    return;
  }

	  JAWT awt;
	  JAWT_DrawingSurface* ds;
	  jboolean result;
	  jint lock;

	  // Get the AWT
	  awt.version = JAWT_VERSION_1_3;
	  result = JAWT_GetAWT(env, &awt);
	  if (result == JNI_FALSE)
	    {
	      cout << "AWT not found" << endl;
	      jcas_ThrowException(env, "AWT not found\n");
//	      alock.Release();
	      return;
	    }

	  // Get the drawing surface
	  ds = awt.GetDrawingSurface(env, theCanvas);
	  if (ds == NULL)
	    {
	      cout << "NULL drawing surface" << endl;
	      jcas_ThrowException(env, "NULL drawing surface\n");
//	      alock.Release();
	      return;
	    }

	  // Lock the drawing surface
	  lock = ds->Lock(ds);
//	  cout << "Result of locking is " << lock << endl;
	  if ((lock & JAWT_LOCK_ERROR) != 0)
	    {
	      cout << "Error locking surface" << endl;
	      jcas_ThrowException(env, "Error locking surface\n");
	      awt.FreeDrawingSurface(ds);
//	      alock.Release();
	      return;
	    }

	  // Redraw V3d_View after locking AWT drawing surface
//  cout << " CASCADEView3d.cxx: redraw "<<endl;

	  jfieldID jNeedResizeID = env->GetFieldID(jViewPort3dClass, "needResize", "Z");
	  if(!jNeedResizeID){ 
		  ds->Unlock(ds);
		  awt.FreeDrawingSurface(ds);
		  jcas_ThrowException(env, "cant find the needREsize field id"); 
		  return; 
	  }

	  jboolean jNeedResize = env->GetBooleanField(theCanvas, jNeedResizeID);
	  if(jNeedResize == JNI_TRUE){  
//     cout << "Info: bef update, need resize." << endl;
		  theView->MustBeResized();
		  jNeedResize = JNI_FALSE;
		  env->SetBooleanField(theCanvas, jNeedResizeID, jNeedResize);
	  }
          theView->Redraw();
	  ds->Unlock(ds);

	  // Free the drawing surface
	  awt.FreeDrawingSurface(ds);
}
