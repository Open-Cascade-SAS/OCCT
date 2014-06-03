using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace IE_WPF_WinForms
{
    public class IECommands
    {
        public static RoutedUICommand New { get; private set; }
        public static RoutedUICommand Close { get; private set; }
        public static RoutedUICommand Quit { get; private set; }
        public static RoutedUICommand About { get; private set; }
        public static RoutedUICommand AboutOk { get; private set; }

        static IECommands()
        {
            #region menu

            InputGestureCollection inputsNew = new InputGestureCollection();
            inputsNew.Add( new KeyGesture( Key.N, ModifierKeys.Control, "Ctrl + N" ) );
            New = new RoutedUICommand( "New", "New", typeof(IECommands), inputsNew );
            
            Close = new RoutedUICommand( "Close", "Close", typeof(IECommands) );

            InputGestureCollection inputsQuit = new InputGestureCollection();
            inputsQuit.Add( new KeyGesture( Key.F4, ModifierKeys.Alt, "Alt + F4" ) );
            Quit = new RoutedUICommand( "Quit", "Quit", typeof(IECommands), inputsQuit );

            InputGestureCollection inputsAbout = new InputGestureCollection();
            inputsAbout.Add( new KeyGesture( Key.F1 ) );
            About = new RoutedUICommand( "About", "About", typeof(IECommands), inputsAbout );

            #endregion

            #region aboutDlg
            InputGestureCollection inputsAboutOk = new InputGestureCollection();
            inputsAboutOk.Add( new KeyGesture( Key.Enter ) );
            AboutOk = new RoutedUICommand( "AboutOk", "AboutOk", typeof(IECommands), inputsAboutOk );
            #endregion
        }
    }
}
