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

import java.util.concurrent.locks.ReentrantLock;

import android.util.Log;
import android.widget.TextView;

//! Auxiliary class for logging messages
public class OcctJniLogger
{

  //! Setup text view
  public static void setTextView (TextView theTextView)
  {
    if (myTextView != null)
    {
      myLog = myTextView.getText().toString();
    }

    myTextView = theTextView;
    if (myTextView != null)
    {
      myTextView.setText (myLog);
      myLog = "";
    }
  }

  //! Interface implementation
  public static void postMessage (String theText)
  {
    final String aCopy = new String (theText);
    Log.e (myTag, theText);

    myMutex.lock();
    final TextView aView = myTextView;
    if (aView == null)
    {
      myLog += aCopy;
      myMutex.unlock();
      return;
    }

    aView.post (new Runnable()
    {
      public void run()
      {
        aView.setText (aView.getText() + aCopy + "\n");
      }
    });
    myMutex.unlock();
  }

  private static final String        myTag      = "occtJniViewer";
  private static final ReentrantLock myMutex    = new ReentrantLock (true);
  private static TextView            myTextView = null;
  private static String              myLog      = "";

}
