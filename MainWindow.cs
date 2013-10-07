using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using DriverLevelEditor.Driver;
using DriverLevelEditor.Driver.FileIO;

namespace DriverLevelEditor
{
    public partial class MainWindow : Form
    {
        TexturesWidget texWidget = new TexturesWidget();

        public MainWindow()
        {
            InitializeComponent();

            texWidget.Dock = DockStyle.Fill;

            texWidget.Parent = this;
            texWidget.Show();

            Level level = new Level();

            // event handler tests
            level.LevelOpened += (s, e) => { Console.WriteLine("LEVEL OPENED:: {0}", ((Driver.DriverEventArgs)e).Message); };
            level.LevelDestroyed += (s, e) => { Console.WriteLine("LEVEL DESTROYED:: {0}", ((Driver.DriverEventArgs)e).Message); };

            level.OpenLevel();
            level.Dispose();
        }

        private string GetFileNames(string[] names)
        {
            if (names.Length == 4)
            {
                return String.Format(
                    " - .\\Levels\\{0}.lev\n" +
                    " - .\\Levels\\{1}.d3d\n" +
                    " - .\\Levels\\{0}.den\n" +
                    " - .\\Levels\\{2}.den\n" +
                    " - .\\Levels\\{0}.cos\n" +
                    " - .\\Levels\\{2}.cos\n" +
                    " - .\\Levels\\{3}.wdf\n\n" +
                    "NOTE: No file check in place yet",
                    names[0], names[1], names[2], names[3]);
            }
            else
            {
                return "Insufficient amount of information.";
            }
        }

        private void LoadCustomFiles(object sender, EventArgs e)
        {
            MessageBox.Show("Load Custom files");
        }

        private void LoadMiamiFiles(object sender, EventArgs e)
        {
            string[] names = new []
            {
                "miami_01",     // lev
                "miami",        // d3d
                                
                "pcar_miami",   // cos/den (player)
                //"miami_01",   // den (civ)
                                
                //"pcar_miami", // cos (player)
                //"Miami_01",   // cos (civ)
                                
                "miami_wheels"  // wdf
            };

            MessageBox.Show(String.Format(
                "Load Miami files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadFriscoFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "frisco_01",    // lev
                "frisco",       // d3d

                "pcar_frisco",  // cos/den (player)
                
                "frisco_wheels" // wdf
            };

            MessageBox.Show(String.Format(
                "Load San Francisco files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadLosAngelesFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "LA_01",        // lev
                "LA",           // d3d

                "pcar_LA",       // cos/den (player)
                                
                "LA_wheels"     // wdf
            };

            MessageBox.Show(String.Format(
                "Load Los Angeles files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadNewYorkFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "ny_01",        // lev
                "newyork",      // d3d
                                
                "pcar_ny",       // cos/den (player)
                                
                "ny_wheels"     // wdf
            };

            MessageBox.Show(String.Format(
                "Load New York files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadNewcastleFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "nc_01",        // lev
                "nc_01",        // d3d
                                
                "pcar_nc",      // cos/den (player)
                
                "nc_wheels"     // wdf
            };

            MessageBox.Show(String.Format(
                "Load Newcastle files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadTrainFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "train",        // lev
                "train",        // d3d
                                
                "pcar_train",   // cos/den (player)
                
                "train_wheels"  // wdf
            };

            MessageBox.Show(String.Format(
                "Load Desert files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void LoadIViewFiles(object sender, EventArgs e)
        {
            string[] names = new[]
            {
                "iview",        // lev
                "iview",        // d3d
                                
                "pcar_iview",   // cos/den (player)
                
                "iview_wheels"  // wdf
            };

            MessageBox.Show(String.Format(
                "Load Car Park files:\n\n" +
                "{0}", GetFileNames(names)));
        }

        private void ViewTextures(object sender, EventArgs e)
        {
            this.WidgetPanel.Controls.Add(texWidget);
        }

        private void ViewModels(object sender, EventArgs e)
        {
            this.WidgetPanel.Controls.Clear();
        }
    }
}
