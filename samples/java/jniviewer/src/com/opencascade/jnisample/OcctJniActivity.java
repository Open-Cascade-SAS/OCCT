// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

package com.opencascade.jnisample;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.content.Context;

import android.content.Intent;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;

import android.text.Html;
import android.text.Html.ImageGetter;
import android.text.Spanned;
import android.util.TypedValue;
import android.view.Display;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.view.ViewGroup.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;

//! Main activity
public class OcctJniActivity extends Activity implements OnClickListener
{

  //! Auxiliary method to print temporary info messages
  public static void printShortInfo (Activity     theActivity,
                                     CharSequence theInfo)
  {
    Context aCtx   = theActivity.getApplicationContext();
    Toast   aToast = Toast.makeText (aCtx, theInfo, Toast.LENGTH_LONG);
    aToast.show();
  }

  //! Load single native library
  private static boolean loadLibVerbose (String        theLibName,
                                         StringBuilder theLoadedInfo,
                                         StringBuilder theFailedInfo)
  {
    try
    {
      System.loadLibrary (theLibName);
      theLoadedInfo.append ("Info:  native library \"");
      theLoadedInfo.append (theLibName);
      theLoadedInfo.append ("\" has been loaded\n");
      return true;
    }
    catch (java.lang.UnsatisfiedLinkError theError)
    {
      theFailedInfo.append ("Error: native library \"");
      theFailedInfo.append (theLibName);
      theFailedInfo.append ("\" is unavailable:\n  " + theError.getMessage());
      return false;
    }
    catch (SecurityException theError)
    {
      theFailedInfo.append ("Error: native library \"");
      theFailedInfo.append (theLibName);
      theFailedInfo.append ("\" can not be loaded for security reasons:\n  " + theError.getMessage());
      return false;
    }
  }

  public static boolean wasNativesLoadCalled = false;
  public static boolean areNativeLoaded      = false;
  public static String  nativeLoaded         = "";
  public static String  nativeFailed         = "";

  //! Auxiliary method to load native libraries
  public boolean loadNatives()
  {
    if (wasNativesLoadCalled)
    {
      return areNativeLoaded;
    }
    wasNativesLoadCalled = true;
    StringBuilder aLoaded = new StringBuilder();
    StringBuilder aFailed = new StringBuilder();

    // copy OCCT resources
    String aResFolder = getFilesDir().getAbsolutePath();
    copyAssetFolder (getAssets(), "Shaders",   aResFolder + "/Shaders");
    copyAssetFolder (getAssets(), "SHMessage", aResFolder + "/SHMessage");
    copyAssetFolder (getAssets(), "XSMessage", aResFolder + "/XSMessage");
    copyAssetFolder (getAssets(), "TObj",      aResFolder + "/TObj");
    copyAssetFolder (getAssets(), "UnitsAPI",  aResFolder + "/UnitsAPI");

    // C++ runtime
    loadLibVerbose ("gnustl_shared", aLoaded, aFailed);

    // 3rd-parties
    loadLibVerbose ("freetype",  aLoaded, aFailed);
    loadLibVerbose ("freeimage", aLoaded, aFailed);

    if (// OCCT modeling
        !loadLibVerbose ("TKernel",      aLoaded, aFailed)
     || !loadLibVerbose ("TKMath",       aLoaded, aFailed)
     || !loadLibVerbose ("TKG2d",        aLoaded, aFailed)
     || !loadLibVerbose ("TKG3d",        aLoaded, aFailed)
     || !loadLibVerbose ("TKGeomBase",   aLoaded, aFailed)
     || !loadLibVerbose ("TKBRep",       aLoaded, aFailed)
     || !loadLibVerbose ("TKGeomAlgo",   aLoaded, aFailed)
     || !loadLibVerbose ("TKTopAlgo",    aLoaded, aFailed)
     || !loadLibVerbose ("TKShHealing",  aLoaded, aFailed)
     || !loadLibVerbose ("TKMesh",       aLoaded, aFailed)
        // exchange
     || !loadLibVerbose ("TKPrim",       aLoaded, aFailed)
     || !loadLibVerbose ("TKBO",         aLoaded, aFailed)
     || !loadLibVerbose ("TKBool",       aLoaded, aFailed)
     || !loadLibVerbose ("TKFillet",     aLoaded, aFailed)
     || !loadLibVerbose ("TKOffset",     aLoaded, aFailed)
     || !loadLibVerbose ("TKXSBase",     aLoaded, aFailed)
     || !loadLibVerbose ("TKIGES",       aLoaded, aFailed)
     || !loadLibVerbose ("TKSTEPBase",   aLoaded, aFailed)
     || !loadLibVerbose ("TKSTEPAttr",   aLoaded, aFailed)
     || !loadLibVerbose ("TKSTEP209",    aLoaded, aFailed)
     || !loadLibVerbose ("TKSTEP",       aLoaded, aFailed)
        // OCCT Visualization
     || !loadLibVerbose ("TKService",    aLoaded, aFailed)
     || !loadLibVerbose ("TKHLR",        aLoaded, aFailed)
     || !loadLibVerbose ("TKV3d",        aLoaded, aFailed)
     || !loadLibVerbose ("TKOpenGl",     aLoaded, aFailed)
        // application code
     || !loadLibVerbose ("TKJniSample",  aLoaded, aFailed))
    {
      nativeLoaded = aLoaded.toString();
      nativeFailed = aFailed.toString();
      areNativeLoaded = false;
      //exitWithError (theActivity, "Broken apk?\n" + theFailedInfo);
      return false;
    }
    nativeLoaded = aLoaded.toString();
    areNativeLoaded = true;
    return true;
  }

  //! Create activity
  @Override protected void onCreate (Bundle theBundle)
  {
    super.onCreate (theBundle);

    boolean isLoaded = loadNatives();
    if (!isLoaded)
    {
      printShortInfo (this, nativeFailed);
      OcctJniLogger.postMessage (nativeLoaded + "\n" + nativeFailed);
    }

    setContentView (R.layout.activity_main);
    
    myOcctView        = (OcctJniView )findViewById (R.id.custom_view);
    myMessageTextView = (TextView    )findViewById (R.id.message_view);
    OcctJniLogger.setTextView (myMessageTextView);

    createViewAndButtons (Configuration.ORIENTATION_LANDSCAPE);

    myButtonPreferSize = defineButtonSize ((LinearLayout )findViewById (R.id.panel_menu));
    ImageButton aScrollBtn = (ImageButton )findViewById (R.id.scroll_btn);
    aScrollBtn.setY (myButtonPreferSize);
    aScrollBtn.setOnTouchListener (new View.OnTouchListener()
    {
      @Override
      public boolean onTouch (View theView, MotionEvent theEvent)
      {
        return onScrollBtnTouch (theView, theEvent);
      }
    });

    onConfigurationChanged (getResources().getConfiguration());

    Intent anIntent = getIntent();
    Uri    aDataUrl  = anIntent != null ? anIntent.getData() : null;
    String aDataPath = aDataUrl != null ? aDataUrl.getPath() : "";
    myOcctView.open (aDataPath);
    myLastPath = aDataPath;
  }

  //! Handle scroll events
  private boolean onScrollBtnTouch (View        theView,
                                    MotionEvent theEvent)
  {
    switch (theEvent.getAction())
    {
      case MotionEvent.ACTION_DOWN:
      {
        LinearLayout aPanelMenu = (LinearLayout )findViewById (R.id.panel_menu);
        boolean isLandscape = (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE);
        if (aPanelMenu.getVisibility() == View.VISIBLE)
        {
          aPanelMenu.setVisibility (View.GONE);
          if (!isLandscape)
          {
            ((ImageButton )theView).setImageResource (R.drawable.open_p);
            theView.setY (0);
          }
          else
          {
            ((ImageButton )theView).setImageResource (R.drawable.open_l);
            theView.setX (0);
          }
        }
        else
        {
          aPanelMenu.setVisibility (View.VISIBLE);
          if (!isLandscape)
          {
            ((ImageButton )theView).setImageResource (R.drawable.close_p);
            theView.setY (myButtonPreferSize);
          }
          else
          {
            ((ImageButton )theView).setImageResource (R.drawable.close_l);
            theView.setX (myButtonPreferSize);
          }
        }
        break;
      }
    }
    return false;
  }

  //! Initialize views and buttons
  private void createViewAndButtons (int theOrientation)
  {
    // open button
    ImageButton anOpenButton = (ImageButton )findViewById (R.id.open);
    anOpenButton.setOnClickListener (this);

    // fit all
    ImageButton aFitAllButton = (ImageButton )findViewById (R.id.fit);
    aFitAllButton.setOnClickListener (this);
    aFitAllButton.setOnTouchListener (new View.OnTouchListener()
    {
      @Override
      public boolean onTouch (View theView, MotionEvent theEvent)
      {
        return onTouchButton (theView, theEvent);
      }
    });
    
    // message
    ImageButton aMessageButton = (ImageButton )findViewById (R.id.message);
    aMessageButton.setOnClickListener (this);

    // info
    ImageButton anInfoButton = (ImageButton )findViewById (R.id.info);
    anInfoButton.setOnClickListener (this);
    
    // font for text view
    TextView anInfoView = (TextView )findViewById (R.id.info_view);
    anInfoView.setTextSize (TypedValue.COMPLEX_UNIT_SP, 18);

    // add submenu buttons
    createSubmenuBtn (R.id.view, R.id.view_group,
                      Arrays.asList (R.id.proj_front, R.id.proj_top,    R.id.proj_left,
                                     R.id.proj_back,  R.id.proj_bottom, R.id.proj_right),
                      Arrays.asList (R.drawable.proj_front, R.drawable.proj_top,    R.drawable.proj_left,
                                     R.drawable.proj_back,  R.drawable.proj_bottom, R.drawable.proj_right),
                      4);
  }

  @Override protected void onNewIntent (Intent theIntent)
  {
    super.onNewIntent (theIntent);
    setIntent (theIntent);
  }

  @Override protected void onDestroy()
  {
    super.onDestroy();
    OcctJniLogger.setTextView (null);
  }

  @Override protected void onPause()
  {
    super.onPause();
    myOcctView.onPause();
  }

  @Override protected void onResume()
  {
    super.onResume();
    myOcctView.onResume();

    Intent anIntent = getIntent();
    Uri    aDataUrl  = anIntent != null ? anIntent.getData() : null;
    String aDataPath = aDataUrl != null ? aDataUrl.getPath() : "";
    if (!aDataPath.equals (myLastPath))
    {
      myOcctView.open (aDataPath);
      myLastPath = aDataPath;
    }
  }

  //! Copy folder from assets
  private boolean copyAssetFolder (AssetManager theAssetMgr,
                                   String       theAssetFolder,
                                   String       theFolderPathTo)
  {
    try
    {
      String[] aFiles  = theAssetMgr.list (theAssetFolder);
      File     aFolder = new File (theFolderPathTo);
      aFolder.mkdirs();
      boolean isOk = true;
      for (String aFileIter : aFiles)
      {
        if (aFileIter.contains ("."))
        {
          isOk &= copyAsset (theAssetMgr,
                             theAssetFolder  + "/" + aFileIter,
                             theFolderPathTo + "/" + aFileIter);
        }
        else
        {
          isOk &= copyAssetFolder (theAssetMgr,
                                   theAssetFolder  + "/" + aFileIter,
                                   theFolderPathTo + "/" + aFileIter);
        }
      }
      return isOk;
    }
    catch (Exception theError)
    {
      theError.printStackTrace();
      return false;
    }
  }

  //! Copy single file from assets
  private boolean copyAsset (AssetManager theAssetMgr,
                             String       thePathFrom,
                             String       thePathTo)
  {
    try
    {
      InputStream aStreamIn = theAssetMgr.open (thePathFrom);
      File aFileTo = new File (thePathTo);
      aFileTo.createNewFile();
      OutputStream aStreamOut = new FileOutputStream (thePathTo);
      copyStreamContent (aStreamIn, aStreamOut);
      aStreamIn.close();
      aStreamIn = null;
      aStreamOut.flush();
      aStreamOut.close();
      aStreamOut = null;
      return true;
    }
    catch (Exception theError)
    {
      theError.printStackTrace();
      return false;
    }
  }

  //! Copy single file
  private static void copyStreamContent (InputStream  theIn,
                                         OutputStream theOut) throws IOException
  {
    byte[] aBuffer = new byte[1024];
    int aNbReadBytes = 0;
    while ((aNbReadBytes = theIn.read (aBuffer)) != -1)
    {
      theOut.write (aBuffer, 0, aNbReadBytes);
    }
  }
  
  //! Show/hide text view
  private void switchTextView (TextView    theTextView,
                               ImageButton theClickedBtn,
                               boolean     theToSwitchOn)
  {
    if (theTextView != null
     && theTextView.getVisibility() == View.GONE
     && theToSwitchOn)
    {
      theTextView.setVisibility (View.VISIBLE);
      theClickedBtn.setBackgroundColor (getResources().getColor(R.color.pressedBtnColor));
      setTextViewPosition (theTextView);
    }
    else
    {
      theTextView.setVisibility (View.GONE);
      theClickedBtn.setBackgroundColor (getResources().getColor (R.color.btnColor));
    }
  }

  //! Setup text view position
  private void setTextViewPosition (TextView theTextView)
  {
    if (theTextView.getVisibility() != View.VISIBLE)
    {
      return;
    }

    if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE)
    {
      theTextView.setX (myButtonPreferSize);
      theTextView.setY (0);
    }
    else
    {
      theTextView.setX (0);
      theTextView.setY (myButtonPreferSize);
    }
  }

  @Override
  public void onClick (View theButton)
  {
    ImageButton aClickedBtn = (ImageButton )theButton;
    switch (aClickedBtn.getId())
    {
      case R.id.message:
      {
        switchTextView ((TextView    )findViewById (R.id.info_view),
                        (ImageButton )findViewById (R.id.info), false);
        switchTextView (myMessageTextView, aClickedBtn, true);
        return;
      }
      case R.id.info:
      {
        String aText = getString (R.string.info_html);
        aText = String.format (aText, cppOcctMajorVersion(), cppOcctMinorVersion(), cppOcctMicroVersion());
        Spanned aSpanned = Html.fromHtml (aText, new ImageGetter()
        {
          @Override
          public Drawable getDrawable (String theSource)
          {
            Resources aResources = getResources();
            int anId = aResources.getIdentifier (theSource, "drawable", getPackageName());
            Drawable aRes = aResources.getDrawable (anId);
            aRes.setBounds (0, 0, aRes.getIntrinsicWidth(), aRes.getIntrinsicHeight());
            return aRes;
          }
        }, null);

        TextView anInfoView = (TextView )findViewById (R.id.info_view);
        anInfoView.setText (aSpanned);
        switchTextView (myMessageTextView, (ImageButton ) findViewById (R.id.message), false);
        switchTextView (anInfoView,        aClickedBtn,                                true);
        return;
      }
      case R.id.fit:
      {
        myOcctView.fitAll();
        return;
      }
      case R.id.proj_front:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Xpos);
        return;
      }
      case R.id.proj_left:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Yneg);
        return;
      }
      case R.id.proj_top:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Zpos);
        return;
      }
      case R.id.proj_back:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Xneg);
        return;
      }
      case R.id.proj_right:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Ypos);
        return;
      }
      case R.id.proj_bottom:
      {
        myOcctView.setProj (OcctJniRenderer.TypeOfOrientation.Zneg);
        return;
      }
      case R.id.open:
      {
        File aPath = Environment.getExternalStorageDirectory();
        aClickedBtn.setBackgroundColor (getResources().getColor(R.color.pressedBtnColor));
        if (myFileOpenDialog == null)
        {
          myFileOpenDialog = new OcctJniFileDialog (this, aPath);
          myFileOpenDialog.setFileEndsWith (".brep");
          myFileOpenDialog.setFileEndsWith (".rle");
          myFileOpenDialog.setFileEndsWith (".iges");
          myFileOpenDialog.setFileEndsWith (".igs");
          myFileOpenDialog.setFileEndsWith (".step");
          myFileOpenDialog.setFileEndsWith (".stp");
          myFileOpenDialog.addFileListener (new OcctJniFileDialog.FileSelectedListener()
          {
            public void fileSelected (File theFile)
            {
              if (theFile != null && myOcctView != null)
              {
                myOcctView.open (theFile.getPath());
              }
            }
          });
          myFileOpenDialog.addDialogDismissedListener (new OcctJniFileDialog.DialogDismissedListener()
          {
            @Override
            public void dialogDismissed()
            {
              ImageButton openButton = (ImageButton )findViewById (R.id.open);
              openButton.setBackgroundColor (getResources().getColor(R.color.btnColor));
            }
          });
        }
        myFileOpenDialog.showDialog();
        return;
      }
    }
  }

  private void createSubmenuBtn (int     theParentBtnId,
                                 int     theParentLayoutId,
                                 final List<Integer> theNewButtonIds,
                                 final List<Integer> theNewButtonImageIds,
                                 int     thePosition)
  {
    int aPosInList = 0;
    final ImageButton aParentBtn = (ImageButton )findViewById (theParentBtnId);

    ViewGroup.LayoutParams aParams = null;
    LinearLayout parentLayout = (LinearLayout ) findViewById (theParentLayoutId);
    for (Integer newButtonId : theNewButtonIds)
    {
      ImageButton aNewButton = (ImageButton )findViewById (newButtonId);
      if (aNewButton == null)
      {
        aNewButton = (ImageButton )new ImageButton (this);
        aNewButton.setId (newButtonId);
        aNewButton.setImageResource (theNewButtonImageIds.get (aPosInList));
        aNewButton.setLayoutParams (aParams);
        parentLayout.addView (aNewButton);
      }

      aNewButton.setOnClickListener (this);
      aNewButton.setVisibility (View.GONE);

      aNewButton.setOnTouchListener (new View.OnTouchListener()
      {
        @Override
        public boolean onTouch (View theView, MotionEvent theEvent)
        {
          return onTouchButton (theView, theEvent);
        }
      });
      ++aPosInList;
    }

    if (aParentBtn != null)
    {
      aParentBtn.setOnTouchListener (null);
      aParentBtn.setOnTouchListener (new View.OnTouchListener()
      {
        @Override
        public boolean onTouch (View theView, MotionEvent theEvent)
        {
          if (theEvent.getAction () == MotionEvent.ACTION_DOWN)
          {
            Boolean isVisible = false;
            for (Integer aNewButtonId : theNewButtonIds)
            {
              ImageButton anBtn = (ImageButton )findViewById (aNewButtonId);
              if (anBtn != null)
              {
                if (anBtn.getVisibility() == View.GONE)
                {
                  anBtn.setVisibility (View.VISIBLE);
                  isVisible = true;
                }
                else
                {
                  anBtn.setVisibility (View.GONE);
                }
              }
            }
            aParentBtn.setBackgroundColor (!isVisible ? getResources().getColor(R.color.btnColor) : getResources().getColor(R.color.pressedBtnColor));
          }
          return false;
        }
      });
    }
  }

  //! Implements onTouch functionality
  private boolean onTouchButton (View        theView,
                                 MotionEvent theEvent)
  {
    switch (theEvent.getAction())
    {
      case MotionEvent.ACTION_DOWN:
        ((ImageButton )theView).setBackgroundColor (getResources().getColor (R.color.pressedBtnColor));
        break;
      case MotionEvent.ACTION_UP:
        ((ImageButton )theView).setBackgroundColor (getResources().getColor (R.color.btnColor));
        break;
    }
    return false;
  }

  //! Handle configuration change event
  @Override
  public void onConfigurationChanged (Configuration theNewConfig)
  {
    super.onConfigurationChanged (theNewConfig);
    LinearLayout aLayoutPanelMenu       = (LinearLayout )findViewById (R.id.panel_menu);
    LayoutParams aPanelMenuLayoutParams = aLayoutPanelMenu.getLayoutParams();

    LinearLayout aLayoutViewGroup       = (LinearLayout )findViewById (R.id.view_group);
    LayoutParams aViewGroupLayoutParams = aLayoutViewGroup.getLayoutParams();
    ImageButton  aScrollBtn             = (ImageButton )findViewById (R.id.scroll_btn);
    LayoutParams aScrollBtnLayoutParams = aScrollBtn.getLayoutParams();

    myButtonPreferSize = defineButtonSize ((LinearLayout )findViewById (R.id.panel_menu));
    defineButtonSize ((LinearLayout )findViewById (R.id.view_group));

    switch (theNewConfig.orientation)
    {
      case Configuration.ORIENTATION_PORTRAIT:
      {
        setHorizontal (aLayoutPanelMenu, aPanelMenuLayoutParams);
        setHorizontal (aLayoutViewGroup, aViewGroupLayoutParams);
        aLayoutViewGroup.setGravity (Gravity.BOTTOM);

        aScrollBtnLayoutParams.height = LayoutParams.WRAP_CONTENT;
        aScrollBtnLayoutParams.width  = LayoutParams.MATCH_PARENT;
        aScrollBtn.setLayoutParams (aScrollBtnLayoutParams);
        if (aLayoutPanelMenu.getVisibility() == View.VISIBLE)
        {
          aScrollBtn.setImageResource (R.drawable.close_p);
          aScrollBtn.setY (myButtonPreferSize);
          aScrollBtn.setX (0);
        }
        else
        {
          aScrollBtn.setImageResource (R.drawable.open_p);
          aScrollBtn.setY (0);
          aScrollBtn.setX (0);
        }
        break;
      }
      case Configuration.ORIENTATION_LANDSCAPE:
      {
        setVertical (aLayoutPanelMenu, aPanelMenuLayoutParams);
        setVertical (aLayoutViewGroup, aViewGroupLayoutParams);
        aLayoutViewGroup.setGravity (Gravity.RIGHT);

        aScrollBtnLayoutParams.height = LayoutParams.MATCH_PARENT;
        aScrollBtnLayoutParams.width  = LayoutParams.WRAP_CONTENT;
        aScrollBtn.setLayoutParams (aScrollBtnLayoutParams);
        if (aLayoutPanelMenu.getVisibility() == View.VISIBLE)
        {
          aScrollBtn.setImageResource (R.drawable.close_l);
          aScrollBtn.setX (myButtonPreferSize);
          aScrollBtn.setY (0);
        }
        else
        {
          aScrollBtn.setImageResource (R.drawable.open_l);
          aScrollBtn.setY (0);
          aScrollBtn.setX (0);
        }
        break;
      }
    }
    setTextViewPosition (myMessageTextView);
    setTextViewPosition ((TextView )findViewById (R.id.info_view));
  }

  private void setHorizontal (LinearLayout theLayout,
                              LayoutParams theLayoutParams)
  {
    theLayout.setOrientation (LinearLayout.HORIZONTAL);
    theLayoutParams.height = LayoutParams.WRAP_CONTENT;
    theLayoutParams.width  = LayoutParams.MATCH_PARENT;
    theLayout.setLayoutParams (theLayoutParams);
  }

  private void setVertical (LinearLayout theLayout,
                            LayoutParams theLayoutParams)
  {
    theLayout.setOrientation (LinearLayout.VERTICAL);
    theLayoutParams.height = LayoutParams.MATCH_PARENT;
    theLayoutParams.width  = LayoutParams.WRAP_CONTENT;
    theLayout.setLayoutParams (theLayoutParams);
  }

  //! Define button size
  private int defineButtonSize (LinearLayout theLayout)
  {
    boolean isLandscape = getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE;
    Display aDisplay    = getWindowManager().getDefaultDisplay();
    Point   aDispPnt    = new Point();
    aDisplay.getSize (aDispPnt);

    int aNbChildren = theLayout.getChildCount();
    int aHeight     = aDispPnt.y / aNbChildren;
    int aWidth      = aDispPnt.x / aNbChildren;
    int aResultSize = 0;
    for (int aChildIter = 0; aChildIter < aNbChildren; ++aChildIter)
    {
      View aView = theLayout.getChildAt (aChildIter);
      if (aView instanceof ImageButton)
      {
        ImageButton aButton = (ImageButton )aView;
        if (isLandscape)
        {
          aButton.setMinimumWidth (aHeight);
        }
        else
        {
          aButton.setMinimumHeight (aWidth);
        }
      }
    }
    if (isLandscape)
    {
      aResultSize = aHeight;
    }
    else
    {
      aResultSize = aWidth;
    }
    return aResultSize;
  }

  //! OCCT major version
  private native long cppOcctMajorVersion();

  //! OCCT minor version
  private native long cppOcctMinorVersion();

  //! OCCT micro version
  private native long cppOcctMicroVersion();

  private OcctJniView       myOcctView;
  private TextView          myMessageTextView;
  private String            myLastPath;
  private OcctJniFileDialog myFileOpenDialog;
  private int               myButtonPreferSize = 65;

}
