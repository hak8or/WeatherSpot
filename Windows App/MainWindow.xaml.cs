/*
    Author:     Piotr Kapela    [https://github.com/pkapela]
    Name:       WeatherSpot
    Created:    Jun/5/2015
    Desc:       WeatherSpot is a client type application which connects to weatherspot.us
                website to fetch and display the data on a graph. The data itself is gathered 
                by embedded devices which monitor and record into Influx Database properties 
                like temperature, humidity or pressure. In addition, the application includes 
                console feature which allows manual querries into the database. The elementary
                statistics section was added for the convinience of a user. This program 
                uses two open source libraries JSON.NET for deserialization of JSON data and 
                Dynamic Data Display for graphs plotting.  
*/

using System;
using System.Windows;
using System.Net;
using System.IO;
using Newtonsoft.Json.Linq;
using Microsoft.Win32;
using System.Collections.Generic;

namespace WeatherSpot
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
           // Loaded += new RoutedEventHandler(MainWindow_Loaded);
        }

        private void fetchClick(object sender, RoutedEventArgs e)
        {
            string sURL = "";
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

            HelperClass.stringFormatter(ref sLine);

            JObject results = JObject.Parse(sLine);
            JArray jarr = (JArray)results.SelectToken("points");

            PlottingClass fetchGraph = new PlottingClass(this); 

            fetchGraph.AddData(ref jarr);
            fetchGraph.PlotGraph();
        }

        private void importClick(object sender, RoutedEventArgs e)
        {
            string initialData = "";
            List<double> rawData = new List<double>();
            PlottingClass importGraph = new PlottingClass(this);

            importGraph.RemoveGraph();

            // Open dialog menu
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "WeatherSpot files (*.ws)|*.ws";

            // User successfully opened a file
            if (openFileDialog.ShowDialog() == true)
            {                
                initialData =  File.ReadAllText(openFileDialog.FileName);
                string[] stringTokens = initialData.Split(';');
                
                for(int i = 0; i < stringTokens.Length; i++)
                {
                    initialData = stringTokens[i];
                    if(i % 2 != 0)
                    {
                        importGraph.AddY(Convert.ToDouble(initialData));
                    }
                    else
                    {
                        importGraph.AddX(Convert.ToDouble(initialData));
                    }
                }

                importGraph.PlotGraph();
            }
            else
            {
                MessageBox.Show("Failed to Import Data");
            }
        }

        private void exportClick(object sender, RoutedEventArgs e)
        {
            /*
            string fileOutput = "";
            string userHomePath = "";

            for (int i = 0; i < N; i++)
            {
                if( i == (N-1))
                {
                    fileOutput += x[i] + ";" + y[i];
                }
                else
                {
                    fileOutput += x[i] + ";" + y[i] + ";";
                }       
            }

            userHomePath = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            userHomePath += @"\Documents\export_data.ws"; 

            System.IO.File.WriteAllText(userHomePath, fileOutput);
            MessageBox.Show("Success");
            */
        }

        private void closeClick(object sender, RoutedEventArgs e)
        {
            // Shutdown the application
            Application.Current.Shutdown();
        }

    } // end of class

} // end of namespace
