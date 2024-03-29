#pragma once

#include <unordered_set>
#include "filesystem/read_write_file.h"
#include "string/split.h"
#include "iterators/copy_modify.h"
#include "nature/vector.h"
#include "abstract/direction.h"
#include "grid/move_cartesian.h"
#include "grid/get_direction_cartesian.h"
#include "grid/are_touching.h"
#include "abstract/translate_to_direction.h"

namespace adventofcode2022::day9
{

/*
 *                 METHOD DECLARATIONS
 */


std::set<mzlib::coordinates2d>
get_visited (std::vector<std::pair<char, int>>& directions, int rope_length);

/*
 *                 FOR SUBMISSIONS
 */

inline long
part1 (std::string input_file)
{
   auto input = mzlib::read_file_lines(input_file);

   std::vector<std::pair<char, int>> directions;
   mzlib::copy_modify(input, directions, [](auto e){
      auto s = mzlib::split(e, " ");
      return std::make_pair(s[0][0], std::stoi(s[1].data()));
   });

   std::set<mzlib::coordinates2d> visited = get_visited(directions, 2);

   return visited.size();
}

inline long
part2 (std::string input_file)
{
   auto input = mzlib::read_file_lines(input_file);

   std::vector<std::pair<char, int>> directions;
   mzlib::copy_modify(input, directions, [](auto e){
      auto s = mzlib::split(e, " ");
      return std::make_pair(s[0][0], std::stoi(s[1].data()));
   });

   std::set<mzlib::coordinates2d> visited = get_visited(directions, 10);

   return visited.size();
}

/*
 *                 METHOD DEFINITIONS
 */



inline std::set<mzlib::coordinates2d>
get_visited (std::vector<std::pair<char, int>>& directions, int rope_length)
{
   std::set<mzlib::coordinates2d> visited;
   std::vector<mzlib::coordinates2d> knots(rope_length, {0,0});
   visited.insert(knots.back());
   for(auto& [c_dir, steps] : directions)
   {
      auto dir_h = mzlib::translate_to_direction(c_dir);
      for(int step=0; step<steps; ++step)
      {
         knots[0] = mzlib::grid::move_cartesian(knots[0], dir_h);
         for(int i_knot=0; i_knot<knots.size()-1; ++i_knot)
         {
            if (!mzlib::grid::are_touching(knots[i_knot], knots[i_knot+1]))
            {
               auto dir_t = mzlib::grid::get_direction_cartesian(knots[i_knot+1], knots[i_knot]);
               knots[i_knot+1] = mzlib::grid::move_cartesian(knots[i_knot + 1], dir_t);
            }
         }
         visited.insert(knots.back());
      }
   }
   return visited;
}

/*
 *                 EXTRA
 */

namespace bonus
{

}

}
