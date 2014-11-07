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
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import com.opencascade.jnisample.ListenerList.FireHandler;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Environment;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Spinner;

//! Simple open file dialog
public class OcctJniFileDialog
{

  public enum DialogMode
  {
    FileOpen, FileExport, FileSave
  }

  private static final String PARENT_DIR = "..";
  private String[]   myFileList;
  private File       myCurrentPath;
  private DialogMode myDialogMode = DialogMode.FileOpen;

  private ListenerList<FileSelectedListener>    myFileListenerList    = new ListenerList<OcctJniFileDialog.FileSelectedListener>();
  private ListenerList<DialogDismissedListener> myDialogDismissedList = new ListenerList<DialogDismissedListener>();
  private final Activity myActivity;
  private List<String>   myFileEndsWith;
  private EditText myFileNameInput;
  private Spinner  myFileExtSpinner;
  int myCurrentExtPositionInList = 0;

  public interface FileSelectedListener
  {
    void fileSelected (File theFile);
  }

  public interface DialogDismissedListener
  {
    void dialogDismissed();
  }

  //! Main constructor.
  public OcctJniFileDialog (Activity theActivity,
                            File     thePath)
  {
    myActivity = theActivity;
    if (!thePath.exists())
    {
      thePath = Environment.getExternalStorageDirectory();
    }
    loadFileList (thePath);
  }

  //! Create new dialog
  public Dialog createFileDialog()
  {
    final Object[] anObjWrapper = new Object[1];
    Dialog aDialog = null;
    AlertDialog.Builder aBuilder = new AlertDialog.Builder (myActivity);

    aBuilder.setTitle (myCurrentPath.getPath());
    LinearLayout aTitleLayout = new LinearLayout (myActivity);
    aTitleLayout.setLayoutParams (new LinearLayout.LayoutParams (LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
    aTitleLayout.setOrientation (LinearLayout.VERTICAL);

    ListView list = new ListView (myActivity);
    list.setScrollingCacheEnabled(false);
    list.setBackgroundColor (Color.parseColor ("#33B5E5"));

    list.setAdapter (new ArrayAdapter<String> (myActivity, android.R.layout.select_dialog_item, myFileList));
    list.setOnItemClickListener (new AdapterView.OnItemClickListener ()
    {

      public void onItemClick (AdapterView<?> arg0, View view, int pos, long id)
      {
        String fileChosen = myFileList[pos];
        File aChosenFile = getChosenFile (fileChosen);
        if (aChosenFile.isDirectory())
        {
          loadFileList (aChosenFile);
          ((Dialog )anObjWrapper[0]).cancel();
          ((Dialog )anObjWrapper[0]).dismiss();
          showDialog();
        }
        else
        {
          if (myDialogMode == DialogMode.FileOpen)
          {
            ((Dialog )anObjWrapper[0]).cancel();
            ((Dialog )anObjWrapper[0]).dismiss();
            fireFileSelectedEvent (aChosenFile);
          }
          else
          {
            myFileNameInput.setText (aChosenFile.getName());
          }
        }
      }
    });
    list.setLayoutParams (new LinearLayout.LayoutParams (LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT, 0.6f));
    aTitleLayout.addView (list);

    if (myDialogMode == DialogMode.FileSave
     || myDialogMode == DialogMode.FileExport)
    {
      myFileNameInput  = new EditText (myActivity);
      myFileExtSpinner = new Spinner  (myActivity);
      ArrayAdapter<CharSequence> adapter = null;
      if (myDialogMode == DialogMode.FileExport)
      {
        adapter = ArrayAdapter.createFromResource (myActivity, R.array.ext_to_exp,
            android.R.layout.simple_spinner_item);
      }
      else
      {
        adapter = ArrayAdapter.createFromResource (myActivity, R.array.ext_to_save,
            android.R.layout.simple_spinner_item);
      }
      // Specify the layout to use when the list of choices appears
      adapter.setDropDownViewResource (android.R.layout.simple_spinner_dropdown_item);
      // Apply the adapter to the spinner
      myFileExtSpinner.setAdapter (adapter);
      myFileExtSpinner.setSelection (myCurrentExtPositionInList);

      myFileExtSpinner.setOnItemSelectedListener (new AdapterView.OnItemSelectedListener()
      {

        @Override
        public void onNothingSelected (AdapterView<?> theParentView)
        {
          // your code here
        }

        @Override
        public void onItemSelected (AdapterView<?> theParent, View theView, int thePosition, long theId)
        {
          if (myCurrentExtPositionInList != thePosition)
          {
            myCurrentExtPositionInList = thePosition;
            setFileEndsWith (Arrays.asList (myFileExtSpinner.getSelectedItem().toString()));
            loadFileList (myCurrentPath);
            ((Dialog )anObjWrapper[0]).cancel();
            ((Dialog )anObjWrapper[0]).dismiss();
            showDialog();
          }
        }
      });

      myFileExtSpinner.setLayoutParams (new LinearLayout.LayoutParams (LayoutParams.MATCH_PARENT,
          LayoutParams.WRAP_CONTENT, 0.2f));
      // titleLayout.addView(fileExtSpinner);
      myFileNameInput.setLayoutParams (new LinearLayout.LayoutParams (LayoutParams.MATCH_PARENT,
          LayoutParams.WRAP_CONTENT, 0.2f));
      LinearLayout aControlsView = new LinearLayout (myActivity);

      aControlsView.addView (myFileNameInput);
      aControlsView.addView (myFileExtSpinner);

      aTitleLayout.addView (aControlsView);
      aBuilder.setView (aTitleLayout);
      aBuilder.setPositiveButton ("OK", new DialogInterface.OnClickListener()
      {
        @Override
        public void onClick (DialogInterface theDialog, int theWhich)
        {
          if (theWhich >= 0)
          {
            String aFileChosen = myFileList[theWhich];
            File aChosenFile = getChosenFile (aFileChosen);
            fireFileSelectedEvent (aChosenFile);
          }
        }
      }).setNegativeButton ("Cancel", null);
    }
    else
    {
      aBuilder.setNegativeButton ("Cancel", null);
    }

    aBuilder.setView (aTitleLayout);

    aDialog = aBuilder.show();
    aDialog.setOnDismissListener (new DialogInterface.OnDismissListener()
    {
      @Override
      public void onDismiss (DialogInterface theDialog)
      {
        fireDialogDismissedEvent();
      }
    });
    anObjWrapper[0] = aDialog;
    return aDialog;
  }

  public void addFileListener (FileSelectedListener theListener)
  {
    myFileListenerList.add (theListener);
  }

  public void addDialogDismissedListener (DialogDismissedListener theListener)
  {
    myDialogDismissedList.add (theListener);
  }

  //! Show file dialog
  public void showDialog()
  {
    createFileDialog().show();
  }

  private void fireFileSelectedEvent (final File theFile)
  {
    myFileListenerList.fireEvent (new FireHandler<OcctJniFileDialog.FileSelectedListener>()
    {
      public void fireEvent (FileSelectedListener theListener)
      {
        theListener.fileSelected (theFile);
      }
    });
  }

  private void fireDialogDismissedEvent()
  {
    myDialogDismissedList.fireEvent (new FireHandler<OcctJniFileDialog.DialogDismissedListener>()
    {
      public void fireEvent (DialogDismissedListener theListener)
      {
        theListener.dialogDismissed();
      }
    });
  }

  private void loadFileList (File thePath)
  {
    myCurrentPath = thePath;
    List<String> aList = new ArrayList<String>();
    if (thePath.exists())
    {
      if (thePath.getParentFile() != null)
      {
        aList.add (PARENT_DIR);
      }
      FilenameFilter aFilter = new FilenameFilter()
      {
        public boolean accept (File theDir, String theFilename)
        {
          File aSel = new File (theDir, theFilename);
          if (!aSel.canRead())
          {
            return false;
          }
          boolean isEndWith = false;
          if (myFileEndsWith != null)
          {
            for (String aFileExtIter : myFileEndsWith)
            {
              if (theFilename.toLowerCase().endsWith (aFileExtIter))
              {
                isEndWith = true;
                break;
              }
            }
          }
          return isEndWith || aSel.isDirectory();
        }
      };
      String[] aFileList1 = thePath.list (aFilter);
      if (aFileList1 != null)
      {
        for (String aFileIter : aFileList1)
        {
          aList.add (aFileIter);
        }
      }
    }
    myFileList = (String[] )aList.toArray (new String[] {});
  }

  private File getChosenFile (String theFileChosen)
  {
    if (theFileChosen.equals (PARENT_DIR))
      return myCurrentPath.getParentFile();
    else
      return new File (myCurrentPath, theFileChosen);
  }

  public void setFileEndsWith (String fileEndsWith)
  {
    if (myFileEndsWith == null)
    {
      myFileEndsWith = new ArrayList<String>();
    }
    if (myFileEndsWith.indexOf (fileEndsWith) == -1)
    {
      myFileEndsWith.add (fileEndsWith);
    }
  }

  public void setFileEndsWith (List<String> theFileEndsWith)
  {
    myFileEndsWith = theFileEndsWith;
  }

  public DialogMode DialogMode()
  {
    return myDialogMode;
  }

  public void DialogMode (DialogMode theMode)
  {
    myDialogMode = theMode;
  }
}

class ListenerList<L>
{
  private List<L> myListenerList = new ArrayList<L>();

  public interface FireHandler<L>
  {
    void fireEvent (L theListener);
  }

  public void add (L theListener)
  {
    myListenerList.add (theListener);
  }

  public void fireEvent (FireHandler<L> theFireHandler)
  {
    List<L> aCopy = new ArrayList<L> (myListenerList);
    for (L anIter : aCopy)
    {
      theFireHandler.fireEvent (anIter);
    }
  }

  public void remove (L theListener)
  {
    myListenerList.remove (theListener);
  }

  public List<L> getListenerList()
  {
    return myListenerList;
  }
}
