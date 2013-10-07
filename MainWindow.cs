using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DriverLevelEditor
{
    public partial class MainWindow : Form
    {
        public MainWindow()
        {
            InitializeComponent();

            Driver.Level level = new Driver.Level();

            // event handler tests
            level.LevelOpened += (s, e) => { Console.WriteLine("LEVELOPENED:: {0}", ((Driver.DriverEventArgs)e).Message); };
            level.LevelDestroyed += (s, e) => { Console.WriteLine("LEVELDESTROYED:: {0}", ((Driver.DriverEventArgs)e).Message); };

            level.OpenLevel();
            level.Dispose();
        }
    }
}
