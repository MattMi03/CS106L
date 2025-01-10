/*
 * CS106L Assignment 4: Weather Forecast
 * Created by Haven Whitney with modifications by Fabio Ibanez & Jacob Roberts-Baca.
 */

#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

/* #### Please feel free to use these values, but don't change them! #### */
double kMaxTempRequirement = 5;
double uAvgTempRequirement = 60;

struct Forecast
{
  double min_temp;
  double max_temp;
  double avg_temp;
};

Forecast compute_forecast(const std::vector<double> &dailyWeather)
{
  // STUDENT TODO 1: return a forecast for the daily weather that is passed in.
  Forecast f;
  f.min_temp = *std::min_element(dailyWeather.begin(), dailyWeather.end());
  f.max_temp = *std::max_element(dailyWeather.begin(), dailyWeather.end());
  f.avg_temp = std::accumulate(dailyWeather.begin(), dailyWeather.end(), 0) / dailyWeather.size();
  return f;
}

std::vector<Forecast> get_forecasts(const std::vector<std::vector<double>> &weatherData)
{
  /*
   * STUDENT TODO 2: returns a vector of Forecast structs for the weatherData which contains
   * std::vector<double> which contain values for the weather on that day.
   */
  std::vector<Forecast> f(weatherData.size());
  std::transform(weatherData.begin(), weatherData.end(), f.begin(), [](std::vector<double> weather)
                 { return compute_forecast(weather); });
  return f;
}

std::vector<Forecast> get_filtered_data(const std::vector<Forecast> &forecastData)
{
  // STUDENT TODO 3: return a vector with Forecasts filtered for days with specific weather.
  std::vector<Forecast> fe = forecastData;
  auto it = std::remove_if(fe.begin(), fe.end(), [](Forecast f)
                           { return !(f.max_temp - f.min_temp > kMaxTempRequirement && f.avg_temp >= uAvgTempRequirement); });
  fe.erase(it, fe.end());
  return fe;
}

std::vector<Forecast> get_shuffled_data(const std::vector<Forecast> &forecastData)
{
  // STUDENT TODO 4: Make use of a standard library algorithm to shuffle the data!
  std::vector<Forecast> f = forecastData;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(f.begin(), f.end(), gen);
  return f;
}

std::vector<Forecast> run_weather_pipeline(const std::vector<std::vector<double>> &weatherData)
{
  // STUDENT TODO 5: Put your functions together to run the weather pipeline!
  std::vector<Forecast> f = get_shuffled_data(get_filtered_data(get_forecasts(weatherData)));

  return f;
}

/* #### Please don't change this line! #### */
#include "utils.cpp"