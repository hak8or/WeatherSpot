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
using System.Linq;
using System.Windows.Media;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;

namespace WeatherSpot
{
    class PlottingClass
    {
        // MainWindow main is used to access XAML elements from PlottingClass
        private MainWindow main;

        // Data used by the graph
        private List<double> x = new List<double>();
        private List<double> y = new List<double>();
        private const int N = 4;

        public PlottingClass(MainWindow win)
        {
            main = win;
        }

        public void AddData(ref JArray jsonDataArray)
        {
            for (int i = 0; i < N; i++)
            {
                x.Add(Convert.ToDouble(jsonDataArray[i][0].ToString()));
                y.Add(Convert.ToDouble(jsonDataArray[i][2].ToString()));
            }
        }

        public void AddX(double x)
        {
            this.x.Add(x);
        }

        public void AddY(double y)
        {
            this.y.Add(y);
        }

        public void PlotGraph()
        {
            // Create data sources:
            var xDataSource = x.AsXDataSource();
            var yDataSource = y.AsYDataSource();

            CompositeDataSource compositeDataSource = xDataSource.Join(yDataSource);
           
            // Adding graph to plotter
            main.plotter.AddLineGraph(
                compositeDataSource,
                Colors.DarkRed,
                3,
                "Temp");

            // Force evertyhing plotted to be visible
            main.plotter.FitToView();          
        }

        public void RemoveGraph()
        {
            main.plotter.RemoveUserElements();
        }

    } // end of class

} // end of namespace