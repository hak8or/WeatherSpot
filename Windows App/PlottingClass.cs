/*
    Author:     Piotr Kapela    [https://github.com/pkapela]
    Program:    WeatherSpot
    Created:    Jun/5/2015
    File:       PlottingClass.cs
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
        private static List<double> time_x = new List<double>();
        private static List<double> temp_y = new List<double>();
        private static List<double> hum_y = new List<double>();
        private static List<double> press_y = new List<double>();

        private int tempLabel = 0;
     
        public PlottingClass(MainWindow win)
        {
            main = win;
        }

        public void AddData(ref JArray jsonDataArray, ref JArray jsonLabelArray)
        {
            time_x.Clear();
            temp_y.Clear();

            for(int i = 0; i < jsonLabelArray.Count; i++)
            {
                if(jsonLabelArray[i].ToString() == "temperature")
                {
                    tempLabel = i;
              //      main.textBoxOut.Text = jsonLabelArray[i].ToString();
                }
            }

            for (int i = 0; i < jsonDataArray.Count; i++)
            {
                time_x.Add(Convert.ToDouble(jsonDataArray[i][0].ToString()));
                temp_y.Add(Convert.ToDouble(jsonDataArray[i][tempLabel].ToString()));
               // hum_y.Add(Convert.ToDouble(jsonDataArray[i][2].ToString()));
            }
        }

        public void AddX(double inX)
        {
            time_x.Add(inX);
        }

        public void AddY(double inY)
        {
            temp_y.Add(inY);
        }

        public override string ToString()
        {
            string toStr = "";

            for (int i = 0; i < time_x.Count; i++)
            {
                if (i == (time_x.Count - 1))
                {
                    toStr += time_x[i] + ";" + temp_y[i];
                }
                else
                {
                    toStr += time_x[i] + ";" + temp_y[i] + ";";
                }
            }

            return toStr;
        }

        public void PlotGraph()
        {
            // Create data sources:
            var xDataSource = time_x.AsXDataSource();
            var yDataSource = temp_y.AsYDataSource();

            CompositeDataSource compositeDataSource = xDataSource.Join(yDataSource);
           
            // Adding graph to plotter
            main.plotter.AddLineGraph(
                compositeDataSource,
                Colors.DarkRed,
                3,
                "Temp");
        
            main.plotter.FitToView();
        }

        public void RemoveGraph()
        {
            main.plotter.RemoveUserElements();
        }

        public void SetStatistics(bool setParameter)
        {
            if (setParameter == true)
            {
                main.temperatureAvgOut.Text = Convert.ToString(this.GetAverage());
                main.temperatureMaxOut.Text = Convert.ToString(this.GetMax());
                main.temperatureMinOut.Text = Convert.ToString(this.GetMin());
                main.temperatureMedianOut.Text = Convert.ToString(this.GetMedian());
            }
            else
            {
                main.temperatureAvgOut.Text = "0.0";
                main.temperatureMaxOut.Text = "0.0";
                main.temperatureMinOut.Text = "0.0";
                main.temperatureMedianOut.Text = "0.0";
            }
        }

        private double GetAverage()
        {
            return temp_y.Average(); 
        }

        private double GetMax()
        {
            return temp_y.Max();
        }

        private double GetMin()
        {
            return temp_y.Min();
        }

        private double GetMedian()
        {
            double median = 0.0;
            int index = 0;
            List<double> medianList = new List<double>(temp_y);

            medianList.Sort();
                      
            if(medianList.Count % 2 == 0)
            {
                index = medianList.Count / 2;
                median = (medianList[index - 1] + medianList[index]) / 2;            
            }
            else
            {
                median = medianList[medianList.Count / 2];
            }
            
            return median;

        } // end of method

    } // end of class

} // end of namespace
                     