#pragma once

#include "filesystem/read_write_file.h"

#include <string>
#include <queue>

namespace adventofcode2022::day1
{

inline long
part1 (std::string input_file)
{
   const auto food_calories = mzlib::read_file_lines(input_file);

   long max_calories = 0, tally_calories = 0;

   for (auto food_calorie: food_calories)
   {
      if (food_calorie.empty())
      {
         tally_calories = 0;
      }
      else if ((tally_calories += std::stol(food_calorie)) > max_calories)
      {
         max_calories = tally_calories;
      }
   }

   return max_calories;
}

inline long
part2 (std::string input_file)
{
   const auto food_calories = mzlib::read_file_lines(input_file);

   long tally_calories = 0;
   std::priority_queue<long> top;

   for (auto food_calorie: food_calories)
   {
      if (food_calorie.empty())
      {
         top.push(tally_calories);
         tally_calories = 0;
      }
      else
      {
         tally_calories += std::stol(food_calorie);
      }
   }

   long top_three = 0;
   top_three += top.top();
   top.pop();
   top_three += top.top();
   top.pop();
   top_three += top.top();
   top.pop();

   return top_three;
}

}