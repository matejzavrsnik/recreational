//
// Created by matej on 09/11/23.
//

#ifndef RECREATIONAL_REUSABLES_GRABBAG_H_APPLY_DRAWING_H
#define RECREATIONAL_REUSABLES_GRABBAG_H_APPLY_DRAWING_H

#include "grabbag.h"
#include "move_to_next_pixel.h"
#include "drawing_operation.h"

inline
bool
apply_drawing(
   mzlib::grid::type<int>& canvas,
   const mzlib::grid::type<int>& drawing,
   const mzlib::grid::cell& coor,
   const drawing_operation operation = drawing_operation::add
)
{
   // todo: if drawing too big, draw what fits and ignore the rest. currently I crash instead.
   mzlib::grid::cell drawing_pixel{0,0};
   const mzlib::grid::cell drawing_size = mzlib::grid::size(drawing);
   do
   {
      auto canvas_pixel = drawing_pixel+coor;
      const auto pixel_value = mzlib::grid::access(drawing, drawing_pixel);
      if (operation == drawing_operation::add)
         mzlib::grid::access(canvas, canvas_pixel) += pixel_value;
      else if (operation == drawing_operation::subtract)
         mzlib::grid::access(canvas, canvas_pixel) -= pixel_value;
   }
   while(move_to_next_cell(drawing_pixel, {0, 0}, drawing_size));

   return true;
}

#endif //RECREATIONAL_REUSABLES_GRABBAG_H_APPLY_DRAWING_H
