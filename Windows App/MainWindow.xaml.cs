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
using System.Windows.Input;
//using System.Windows.Forms;

namespace WeatherSpot
{
    public partial class MainWindow : Window
    {
        private PlottingClass fetchGraph;
        private List<string> consoleHistory = new List<string>();

        private int historyCounter = 0;
        private int moduloCounter = 0;
        private const int HISTORY_LIMIT = 50;

        private string dayAfter = "";
        private string dayBefore = "";
        private string sensorLocation = "";

        public MainWindow()
        {
            InitializeComponent();
            fetchGraph = new PlottingClass(this);
        }

        private void fetchClick(object sender, RoutedEventArgs e)
        {
            string dbQuery = "SELECT temperature FROM " + sensorLocation + " WHERE time > '" + dayBefore + "' AND time < '" + dayAfter + "'";

            try
            {               
                JObject parseResults = JObject.Parse(NetworkClass.serverResponse(dbQuery));             
                JArray jsonArray = (JArray)parseResults.SelectToken("points");
                JArray jsonArrayLabels = (JArray)parseResults.SelectToken("columns");

                textBoxOut.Text = dbQuery;

                fetchGraph.RemoveGraph();
                fetchGraph.AddData(ref jsonArray, ref jsonArrayLabels);
                fetchGraph.PlotGraph();
                fetchGraph.SetStatistics(true);
            }
            catch(Exception)
            {
                MessageBox.Show("Fetch Error, Try Again");
                textBoxOut.Text = dbQuery;
            } 
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
            // Save dialog menu
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
                if(consoleInBox.Text != "") // Processing only non-empty strings
                {                    
                    try
                    {
                        consoleOutBox.Text += ">" + consoleInBox.Text.ToUpper() + "\n";

                        JObject parseResults = JObject.Parse(NetworkClass.serverResponse(consoleInBox.Text));
                        JArray jsonArray = (JArray)parseResults.SelectToken("columns");

                        // Saving input for the history reference      
                        consoleHistory.Insert(moduloCounter % HISTORY_LIMIT, consoleInBox.Text);
                        moduloCounter++;
                        historyCounter = consoleHistory.Count;

                        // This part of the code properly displays the data in the console window 
                        int rowCount = jsonArray.Count;

                        for (int i = 0; i < jsonArray.Count; i++)
                        {
                            consoleOutBox.Text += jsonArray[i].ToString() + "\t\t";
                        }

                        jsonArray = (JArray)parseResults.SelectToken("points");
                        consoleOutBox.Text += "\n";

                        for (int i = 0; i < jsonArray.Count; i++)
                        {
                            for (int j = 0; j < rowCount; j++)
                            {
                                consoleOutBox.Text += jsonArray[i][j].ToString() + "\t\t";
                            }

                            consoleOutBox.Text += "\n";
                        }

                        consoleOutBox.Text += "\n";
                        consoleInBox.Text = "";
                    }
                    catch(Exception)
                    {
                        consoleOutBox.Text += "UNRECOGNIZED COMMAND\n\n";

                        // Saving input for the history reference                        
                        consoleHistory.Insert(moduloCounter % HISTORY_LIMIT, consoleInBox.Text);
                        moduloCounter++;
                        historyCounter = consoleHistory.Count;
                        consoleInBox.Text = "";
                    }                    
                }                
            }           
        } 
        
        /*
            BASH-like history feature which enables a user to browse previous commands
            by pressing either down or up arrow keys. 
        */
        private void consolePreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Up)
            {
                historyCounter++;

                if ((historyCounter < consoleHistory.Count) && (historyCounter >= 0))
                {
                    consoleInBox.Text = consoleHistory[historyCounter];
                }
                else
                {
                    consoleInBox.Text = "";
                    historyCounter = consoleHistory.Count;
                }
            }
            if (e.Key == Key.Down)
            {
                historyCounter--;
                      
                if((historyCounter < consoleHistory.Count) && (historyCounter >= 0))
                {
                    consoleInBox.Text = consoleHistory[historyCounter];
                }
                else
                {
                    consoleInBox.Text = consoleHistory[0];
                    historyCounter++;
                }                
            }
        }

        /*  
            The event clears the input box out of the initial label (Enter query)
            for the convenience of a user.  
        */
        private void consoleMouseClick(object sender, MouseButtonEventArgs e)
        {
            consoleInBox.Text = "";
        } 
        
        /*
            By using the calendar a user can choose a specific day to fetch the data.
            The event creates two dates used for SQL query which is used by HTTP get
            routine in NetworkClass.cs. 
        */
        private void CalendarSelectedDatesChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {          
            if (FetchCalendar.SelectedDate.HasValue)
            {                
                DateTime date = FetchCalendar.SelectedDate.Value;

                dayBefore = date.AddDays(-1).ToString("yyyy-MM-dd");
                dayAfter = date.AddDays(1).ToString("yyyy-MM-dd");

            }
        }

        /*
            The following two events are responsible for sensors selection.
            The first entry in the drop menu box is used as the default value
            for the query.
        */
        private void ComboBoxLoaded(object sender, RoutedEventArgs e)
        {
            // Adding values for the drop menu box
            List<string> data = new List<string>();
            data.Add("Downtown");
            data.Add("Sun");
            data.Add("Black Hole");

            // Binding data with locationSelection xaml element
            locationSelector.ItemsSource = data;

            // Selection of the first item
            locationSelector.SelectedIndex = 0;
        }

        private void ComboBoxSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            sensorLocation = locationSelector.SelectedItem as string;
                        
        } // end of method

    } // end of class

} // end of namespace
