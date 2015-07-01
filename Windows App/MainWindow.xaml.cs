/*
    Author: Piotr Kapela
    Name:   WeatherSpot 
*/

using System;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using System.Net;
using System.IO;
using Newtonsoft.Json.Linq;

namespace WeatherSpot
{
    public partial class MainWindow : Window
    {
        // Data used by the graph
        private double[] x;
        private double[] y;
        private const int N = 4;

        public MainWindow()
        {
            InitializeComponent();
           // Loaded += new RoutedEventHandler(MainWindow_Loaded);
        }

        private void fetchClick(object sender, RoutedEventArgs e)
        {
            string sURL;
            sURL = "http://weatherspot.us/db/query.php?db=weather&query=SELECT%20*%20FROM%20Temperature";

            WebRequest wrGETURL;
            wrGETURL = WebRequest.Create(sURL);
            /*
                    WebProxy myProxy = new WebProxy("myproxy", 80);
                    myProxy.BypassProxyOnLocal = true;

                    wrGETURL.Proxy = WebProxy.GetDefaultProxy();    
           */
            Stream objStream;
            objStream = wrGETURL.GetResponse().GetResponseStream();

            StreamReader objReader = new StreamReader(objStream);
            string sLine = "";

            sLine = objReader.ReadLine();
            sLine = objReader.ReadLine();

            helperClass.stringFormatter(ref sLine);

            JObject results = JObject.Parse(sLine);
            JArray jarr = (JArray)results.SelectToken("points");

            // textBoxOut.Text = jarr[2][2].ToString();
            textBoxOut.Text = sLine;

             // Prepare data in arrays
            x = new double[N];
            y = new double[N];

            for (int i = 0; i < N; i++)
            {
                x[i] = Convert.ToDouble(jarr[i][0].ToString());
                y[i] = Convert.ToDouble(jarr[i][2].ToString());
            }

            // Create data sources:
            var xDataSource = x.AsXDataSource();
            var yDataSource = y.AsYDataSource();

            CompositeDataSource compositeDataSource = xDataSource.Join(yDataSource);
            // Adding graph to plotter
            plotter.AddLineGraph(
                compositeDataSource,
                Colors.DarkRed,
                3,
                "Temp");

            // Force evertyhing plotted to be visible
            plotter.FitToView();

            // Preparing Statistics data
            double avgTemperature = 0.0;

            for(int i = 0; i < N; i++)
            {
                avgTemperature += y[i];
            }

            avgTemperature /= N;
            temperatureOut.Text = Convert.ToString(avgTemperature);
        }

        private void ExportClick(object sender, RoutedEventArgs e)
        {
            string fileOutput = "";
            string userHomePath = "";

            for (int i = 0; i < N; i++)
            {
                fileOutput += "time:" + x[i] + ";temp:" + y[i] + "\n";
            }

            userHomePath = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            userHomePath += @"\Documents\ws_export.txt"; 

            System.IO.File.WriteAllText(userHomePath, fileOutput);
            MessageBox.Show("Success");
        }

        private void closeClick(object sender, RoutedEventArgs e)
        {
            // Shutdown the application
            Application.Current.Shutdown();
        }

    } // end of class

} // end of namespace
