#pragma once

#include "filesystem/read_write_file.h"
#include "string/split.h"
#include "iterators/copy_modify.h"
#include "tools/nested_vector.h"
#include "iterators/circular_next.h"
#include "../../reusables/grabbag.h"

using namespace std;

namespace adventofcode2022::day17
{

/*
 *                 METHODS
 */

using mnvt = mzlib::nested_vector::type<int>;
namespace mnv = mzlib::nested_vector;
using mc2d = mzlib::coordinates2d;

const mnvt rock_m = {
   {1,1,1,1}
};
const mnvt rock_p = {
   {0,1,0},
   {1,1,1},
   {0,1,0},
};
const mnvt rock_l = {
   {0,0,1},
   {0,0,1},
   {1,1,1},
};
const mnvt rock_i = {
   {1},
   {1},
   {1},
   {1},
};
const mnvt rock_d = {
   {1,1},
   {1,1},
};

const vector rocks = {
   rock_m,
   rock_p,
   rock_l,
   rock_i,
   rock_d
};

// return: did it advance the coordinate? To be able to tell when to stop.
inline bool iterate(mc2d& coordinate, const mc2d& from, const mc2d& to)
{
   if(coordinate[1] == to[1]-1 && coordinate[0] == to[0]-1)
      return false;

   if(++coordinate[0] >= to[0])
   {
      coordinate[0] = from[0];
      ++coordinate[1];
   }

   return true;
}



inline bool is_in(
   const mnvt& canvas,
   const mnvt& drawing,
   const mc2d& coor)
{
   return coor[0]>=0
      && coor[1]>=0
      && mnv::width(canvas) >= coor[0] + mnv::width(drawing)
      && mnv::height(canvas) >= coor[1] + mnv::height(drawing);
}

enum class drawing_op
{
   add,
   subtract
};

inline bool apply_drawing(
   mnvt& canvas,
   const mnvt& drawing,
   const mc2d& coor,
   const drawing_op operation = drawing_op::add
   )
{
   // todo: if drawing too big, draw what fits and ignore the rest. currently I crash instead.
   mc2d drawing_pixel{0,0};
   const mc2d drawing_size = mnv::size(drawing);
   do
   {
      auto canvas_pixel = drawing_pixel+coor;
      const auto pixel_value = mnv::access(drawing, drawing_pixel);
      if (operation == drawing_op::add)
         mnv::access(canvas, canvas_pixel) += pixel_value;
      else if (operation == drawing_op::subtract)
         mnv::access(canvas, canvas_pixel) -= pixel_value;
   }
   while(iterate(drawing_pixel, {0,0}, drawing_size));

   return true;
}

inline bool move_drawing(
   mnvt& canvas,
   const mnvt& drawing,
   const mc2d& coor_from,
   const mc2d& coor_to
)
{
   return apply_drawing(canvas, drawing, coor_from, drawing_op::subtract)
      && apply_drawing(canvas, drawing, coor_to, drawing_op::add);
}

inline mc2d coordinates_after_push(
   char jet,
   const mnvt& chamber,
   const mnvt& rock,
   const mc2d& coor)
{
   mc2d new_coor = coor;
   switch(jet)
   {
   case '>':
      new_coor = move_screen(coor, mzlib::direction::e);
      break;
   case '<':
      new_coor = move_screen(coor, mzlib::direction::w);
      break;
   }
   return new_coor;
}


inline bool anything_overlaps(const mnvt& chamber)
{
   for(const auto& row : chamber)
      for(const auto& cell : row)
         if(cell > 1)
            return true;
   return false;
}

// return: new coordinates of the stone
inline mc2d move_if_not_blocked(
   mnvt& chamber,
   const mnvt& rock,
   const mc2d& from_coor,
   const mc2d& to_coor
   )
{
   if (is_in(chamber, rock, to_coor))
   {
      move_drawing(chamber, rock, from_coor, to_coor);
      // jet push can make it overlap with another stone
      if (anything_overlaps(chamber))
         // in that case move back
         move_drawing(chamber, rock, to_coor, from_coor);
      else
         // if not, this is new coordinates
         return to_coor;
   }
   // couldn't move
   return from_coor;
}

inline int get_highest_rock(const mnvt& chamber)
{
   for (int row = 0; row < chamber.size(); row++)
      for (int col = 0; col < chamber[0].size(); col++)
         if (chamber[row][col] > 0)
            return chamber.size() - row;
   return chamber.size();
}



/*
 *                 FOR SUBMISSIONS
 */

inline long
part1 (std::string input_file)
{
   auto input = mzlib::read_file_lines(input_file);

   auto chamber = mzlib::nested_vector::construct(1, 7, 0);
   auto jet_it = input[0].begin();

   auto rock_it = rocks.begin();
   int highest_rock=0;
   for(auto rock_count : std::views::iota(0, 2022))
   {
      // increase chamber to accomodate rock
      auto needed_height = highest_rock+1+3+mnv::height(*rock_it);
      if(needed_height > mnv::height(chamber))
      {
         mzlib::nested_vector::add_top(chamber, needed_height-mnv::height(chamber)-1, 0);
      }

      mc2d rock_coor{2, static_cast<long>(mnv::height(chamber)-highest_rock-3-mnv::height(*rock_it))};
      bool rock_fell = true;

      apply_drawing(chamber, *rock_it, rock_coor);

      while(rock_fell)
      {
         // jet push
         auto pushed_coor = coordinates_after_push(*jet_it, chamber, *rock_it, rock_coor);
         rock_coor = move_if_not_blocked(chamber, *rock_it, rock_coor, pushed_coor);

         // fall
         auto fallen_coor = move_screen(rock_coor, mzlib::direction::s);
         rock_coor = move_if_not_blocked(chamber, *rock_it, rock_coor, fallen_coor);
         rock_fell = rock_coor == fallen_coor;

         mzlib::circular_next_iterator(jet_it, input[0].begin(), input[0].end());
      }

      highest_rock = get_highest_rock(chamber);
      mzlib::circular_next_iterator(rock_it, rocks.begin(), rocks.end());
   }

   // 3233 too high, 3442 too high, 3212 not correct, 2958 is not correct
   return highest_rock;
}


// when a collection of elements starts repeating:
// example:
// a,k,d,h,f,r,n,n,s,m,d,l,v,n,g,f,k,s,n,s,c,n,h,g,d,k,s,w,r,h,g,b,c,x,i,3,d,n,h,g,d,k,s,w,r,h,g,b,c,x,i,3,d
// | start of the collection                 |starts repeating here          |this is repeated part
template<typename Container, typename Iterator>
std::optional<std::pair<Iterator, Iterator>> did_it_start_repeating(Container& container)
{
   using ReverseIterator = decltype(container.rbegin());
   std::set<ReverseIterator> candidates;
/*
   auto size = container.size()/2;
   for(auto compare_with = std::advance(container.rbegin(), size))
   auto final_element = container.rbegin();
*/

   return std::nullopt;
}

inline long
part2 (std::string input_file)
{
   auto input = mzlib::read_file_lines(input_file);

   auto chamber = mzlib::nested_vector::construct(1, 7, 0);
   auto jet_it = input[0].begin();

   auto rock_it = rocks.begin();
   int highest_rock=0;
   vector<int> highest_rocks{0};
   vector<int> highest_rocks_diff{0};
   for(auto rock_count : std::views::iota(0, 10'000))
   {
      //cout << endl << "Rock: " << rock_count+1 << "th" << endl;
      //print_field(*rock_it, {{0, "."},{1, "#"}}, 1);

      // increase chamber to accomodate rock
      auto needed_height = highest_rock+1+3+mnv::height(*rock_it);
      if(needed_height > mnv::height(chamber))
      {
         mzlib::nested_vector::add_top(chamber, needed_height-mnv::height(chamber)-1, 0);
      }

      mc2d rock_coor{2, static_cast<long>(mnv::height(chamber)-highest_rock-3-mnv::height(*rock_it))};
      bool rock_fell = true;

      apply_drawing(chamber, *rock_it, rock_coor);
      //cout << endl;
      //print_field(chamber, {{0, "."},{1, "#"}}, 1);

      while(rock_fell)
      {
         // jet push
         //cout << endl << "Jet: " << *jet_it << " (" << std::distance(input[0].begin(), jet_it)+1 << "th jet in pattern " << input[0] << ")";
         auto pushed_coor = coordinates_after_push(*jet_it, chamber, *rock_it, rock_coor);
         rock_coor = move_if_not_blocked(chamber, *rock_it, rock_coor, pushed_coor);

         //cout << endl;
         //print_field(chamber, {{0, "."},{1, "#"}}, 1);

         // fall
         auto fallen_coor = move_screen(rock_coor, mzlib::direction::s);
         rock_coor = move_if_not_blocked(chamber, *rock_it, rock_coor, fallen_coor);
         rock_fell = rock_coor == fallen_coor;

         //cout << endl << "Fall" << endl;
         //print_field(chamber, {{0, "."},{1, "#"}}, 1);

         //if(rock_fell)
         mzlib::circular_next_iterator(jet_it, input[0].begin(), input[0].end());
      }

      highest_rock = get_highest_rock(chamber);
      highest_rocks_diff.push_back(highest_rock-highest_rocks.back());
      highest_rocks.push_back(highest_rock);
      mzlib::circular_next_iterator(rock_it, rocks.begin(), rocks.end());
   }

   print(highest_rocks_diff);

   // 3233 too high, 3442 too high, 3212 not correct, 2958 is not correct
   return highest_rock;
}

}