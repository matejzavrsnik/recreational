#pragma once

#include <abstract/direction.h>

template<class VectorT>
VectorT move(VectorT c, const mzlib::direction d)
{
   switch (d)
   {
      case mzlib::direction::n: c += VectorT{-1,0}; break;
      case mzlib::direction::s: c += VectorT{1,0}; break;
      case mzlib::direction::e: c += VectorT{0,1}; break;
      case mzlib::direction::w: c += VectorT{0,-1}; break;

      case mzlib::direction::ne: c += VectorT{-1,1}; break;
      case mzlib::direction::nw: c += VectorT{-1,-1}; break;
      case mzlib::direction::se: c += VectorT{1,1}; break;
      case mzlib::direction::sw: c += VectorT{1,-1}; break;
   }
   return c;
}

