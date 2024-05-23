
#ifndef NMEA_ROS_BRIDGE_UTIL_HPP
#define NMEA_ROS_BRIDGE_UTIL_HPP

#include <vector>
#include <string>
#include <sstream>

#include "ros/ros.h"

std::vector<std::string> split(const std::string &input)
{
  std::istringstream stream(input);
  std::string field;
  std::vector<std::string> result;
  while (std::getline(stream, field, ','))
  {
    result.push_back(field);
  }
  return result;
}

ros::Time getGPSTime(const std::string &sentence, const ros::Time ros_time)
{
  ros::Time gps_time;
  std::string str_gps_time;

  std::vector<std::string> sentence_vec = split(sentence);
  if (sentence_vec[0].find("PSSN") != std::string::npos)
  {
    if (!sentence_vec[2].empty())
    {
      str_gps_time = sentence_vec[2];
    }
  }
  else
  {
    if (!sentence_vec[1].empty())
    {
      str_gps_time = sentence_vec[1];
    }
  }

  if (str_gps_time.empty())
  {
    return ros_time;
  }

  time_t time;
  struct tm *tm_localtime;
  struct tm tm_gps;
  double decimal_sec;

  memset(&tm_gps, 0x00, sizeof(struct tm));

  time = ros_time.toSec();
  tm_localtime = localtime(&time);
  tm_gps.tm_year = tm_localtime->tm_year;
  tm_gps.tm_mon = tm_localtime->tm_mon;
  tm_gps.tm_mday = tm_localtime->tm_mday;

  tm_gps.tm_hour = stoi(str_gps_time.substr(0, 2)) + 9;
  tm_gps.tm_min = stoi(str_gps_time.substr(2, 2));
  tm_gps.tm_sec = stoi(str_gps_time.substr(4, 2));
  decimal_sec = stod(str_gps_time.substr(6));

  gps_time.sec = mktime(&tm_gps);
  gps_time.nsec = decimal_sec * 1000000000;

  return gps_time;
}

#endif  // NMEA_ROS_BRIDGE_UTIL_HPP
