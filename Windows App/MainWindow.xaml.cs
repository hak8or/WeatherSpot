/*
    Author:     Piotr Kapela    [https://github.com/pkapela]
    Program:    WeatherSpot
    Created:    Jun/5/2015
    File:       MainWindow.xaml.cs
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
using System.IO;
using Newtonsoft.Json.Linq;
using Microsoft.Win32;
using System.Collections.Generic;
//using System.Windows.Forms;

namespace WeatherSpot
{
    public partial class MainWindow : Window
    {
        private PlottingClass fetchGraph;         

        public MainWindow()
        {
            InitializeComponent();
            fetchGraph = new PlottingClass(this);
        }

        private void fetchClick(object sender, RoutedEventArgs e)
        {
            const string dbQuery = "SELECT * FROM Downtown";

            JObject parseResults = JObject.Parse(NetworkClass.serverResponse(dbQuery));
            JArray jsonArray = (JArray)parseResults.SelectToken("points");

            fetchGraph.RemoveGraph();
            fetchGraph.AddData(ref jsonArray);
            fetchGraph.PlotGraph();
            fetchGraph.SetStatistics(true);
        }

        private void importClick(object sender, RoutedEventArgs e)
        {
            string initialData = "";
            List<double> rawData = new List<double>();
            PlottingClass importGraph = new PlottingClass(this);              

            // Open dialog menu
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "WeatherSpot files (*.ws)|*.ws";

            // User successfully opened a file
            if (openFileDialog.ShowDialog() == true)
            {
                importGraph.RemoveGraph();
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
                importGraph.SetStatistics(true);              
            }
        }

        private void exportClick(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "WeatherSpot files (*.ws)|*.ws";

            if (saveFileDialog.ShowDialog() == true)
            {
                File.WriteAllText(saveFileDialog.FileName, fetchGraph.ToString());
            } 
        }

        private void closeClick(object sender, RoutedEventArgs e)
        {
            // Shutdown the application
            Application.Current.Shutdown();
        }

        private void consoleKeyPressed(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if(e.Key.ToString() == "Return")
            {
                if(consoleInBox.Text == "clr")
                {
                    consoleOutBox.Text = "";
                    consoleInBox.Text = "";
                }
                else
                {
                    consoleOutBox.Text += ">" + consoleInBox.Text.ToUpper() + "\n";
                    
                    JObject parseResults = JObject.Parse(NetworkClass.serverResponse(consoleInBox.Text));
                    JArray jsonArray = (JArray)parseResults.SelectToken("columns");

                    int rowCount = jsonArray.Count;

                    for (int i = 0; i < jsonArray.Count; i++)
                    {
                        consoleOutBox.Text += jsonArray[i].ToString() + "\t\t";

                    }

                    jsonArray = (JArray)parseResults.SelectToken("points");                    
                    consoleOutBox.Text += "\n";

                    for (int i = 0; i < jsonArray.Count ; i++)
                    {                       
                        for(int j = 0; j < rowCount; j++)
                        {
                            consoleOutBox.Text += jsonArray[i][j].ToString() + "\t\t";
                        }

                        consoleOutBox.Text += "\n";
                    }
            
                    consoleOutBox.Text += "\n";
                    consoleInBox.Text = "";                    
                }                
            }
        } // end of method
                 
    } // end of class

} // end of namespace
