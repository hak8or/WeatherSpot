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
        private static List<double> light_y = new List<double>();

        private int tempLabel = 0;
        private int humLabel = 0;
        private int presLabel = 0;
        private int lightLabel = 0;

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
                }
                else if (jsonLabelArray[i].ToString() == "humidity")
                {
                    humLabel = i;
                }
                else if (jsonLabelArray[i].ToString() == "pressure")
                {
                    presLabel = i;
                }
                else if (jsonLabelArray[i].ToString() == "lighting")
                {
                    lightLabel = i;
                }
            }

            for (int i = 0; i < jsonDataArray.Count; i++)
            {
                time_x.Add(Convert.ToDouble(jsonDataArray[i][0].ToString()));
                temp_y.Add(Convert.ToDouble(jsonDataArray[i][tempLabel].ToString()));
                hum_y.Add(Convert.ToDouble(jsonDataArray[i][humLabel].ToString()));
                press_y.Add(Convert.ToDouble(jsonDataArray[i][presLabel].ToString()));
                light_y.Add(Convert.ToDouble(jsonDataArray[i][lightLabel].ToString()));             
            }
            /*

                TimeSpan time = TimeSpan.FromSeconds(Convert.ToDouble(jsonDataArray[2][0].ToString()));
                double hours = time.Hours;
                main.fetchTime.Text = hours.ToString();

            
                double unx = 1434585421459;
                System.DateTime dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
                dtDateTime = dtDateTime.AddSeconds(unx).ToLocalTime();
                main.fetchTime.Text = dtDateTime.ToString();
            */

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
            var yDataSourceTemp = temp_y.AsYDataSource();
            var yDataSourceHum = hum_y.AsYDataSource();
            var yDataSourcePress = press_y.AsYDataSource();
            var yDataSourceLight = light_y.AsYDataSource();


            CompositeDataSource compositeDataSource = xDataSource.Join(yDataSourceTemp);
           
            // Adding graph to plotter
            main.plotter.AddLineGraph(
                compositeDataSource,
                Colors.DarkRed,
                1,
                "Temp");

            CompositeDataSource compositeDataSourceHum = xDataSource.Join(yDataSourceHum);

            main.plotter2.AddLineGraph(
                compositeDataSourceHum,
                Colors.CadetBlue,
                1,
                "Hum");

            CompositeDataSource compositeDataSourcePress = xDataSource.Join(yDataSourcePress);

            main.plotter3.AddLineGraph(
                compositeDataSourcePress,
                Colors.DarkGreen,
                1,
                "Press");

            CompositeDataSource compositeDataSourceLight = xDataSource.Join(yDataSourceLight);

            main.plotter4.AddLineGraph(
                compositeDataSourceLight,
                Colors.Gold,
                1,
                "Light");

            main.plotter.FitToView();
            main.plotter2.FitToView();
            main.plotter3.FitToView();
            main.plotter4.FitToView();
        }

        public void RemoveGraph()
        {
            main.plotter.RemoveUserElements();
        }

        private double UnixTimeConverter(double unx)
        {
         

            // First make a System.DateTime equivalent to the UNIX Epoch.
            System.DateTime dateTime = new System.DateTime(1970, 1, 1, 0, 0, 0, 0);

            // Add the number of seconds in UNIX timestamp to be converted.
            dateTime = dateTime.AddSeconds(unx);

            // The dateTime now contains the right date/time so to format the string,
            // use the standard formatting methods of the DateTime object.
           


            return Convert.ToDouble(dateTime.Hour);
        }

        public void SetStatistics(bool setParameter)
        {
            if (setParameter == true)
            {
                main.temperatureAvgOut.Text = Convert.ToString(Math.Round(temp_y.Average()));
                main.temperatureMaxOut.Text = Convert.ToString(temp_y.Max());
                main.temperatureMinOut.Text = Convert.ToString(temp_y.Min());
                main.temperatureMedianOut.Text = Convert.ToString(this.GetMedian(temp_y));

                main.humidityAvgOut.Text = Convert.ToString(Math.Round(hum_y.Average()));
                main.humidityMaxOut.Text = Convert.ToString(hum_y.Max());
                main.humidityMinOut.Text = Convert.ToString(hum_y.Min());
                main.humidityMedianOut.Text = Convert.ToString(this.GetMedian(hum_y));

                main.pressureAvgOut.Text = Convert.ToString(Math.Round(press_y.Average()));
                main.pressureMaxOut.Text = Convert.ToString(press_y.Max());
                main.pressureMinOut.Text = Convert.ToString(press_y.Min());
                main.pressureMedianOut.Text = Convert.ToString(this.GetMedian(press_y));

                main.lightAvgOut.Text = Convert.ToString(Math.Round(light_y.Average()));
                main.lightMaxOut.Text = Convert.ToString(light_y.Max());
                main.lightMinOut.Text = Convert.ToString(light_y.Min());
                main.lightMedianOut.Text = Convert.ToString(this.GetMedian(light_y));                           
            }
            else
            {
                main.temperatureAvgOut.Text = "0.0";
                main.temperatureMaxOut.Text = "0.0";
                main.temperatureMinOut.Text = "0.0";
                main.temperatureMedianOut.Text = "0.0";
            }
        }

        private double GetStdDev(List<double> doubleList)
        {
            double average = doubleList.Average();
            double sumOfDerivation = 0;
            foreach (double value in doubleList)
            {
                sumOfDerivation += (value) * (value);
            }
            double sumOfDerivationAverage = sumOfDerivation / (doubleList.Count - 1);
            return Math.Sqrt(sumOfDerivationAverage - (average * average));
        }

        private double GetMedian(List<double> inputList)
        {
            double median = 0.0;
            int index = 0;

            List<double> medianList = new List<double>(inputList);

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
                     