//------------------------------------------------------------------------------
// Copyright (c) 2017 John D. Haughton
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#ifndef MOVE_H
#define MOVE_H

#include <cassert>
#include <vector>

#include "Pos60.h"

//! Representation of a single move by a player
class Move
{
public:
   Move(Pos60& start_)
      : start(start_)
      , end(start_)
   {}

   bool         empty()    const { return directions.empty(); }
   bool         isStep()   const { return is_step; }
   bool         isHop()    const { return !empty() && !isStep(); }
   const Pos60& getStart() const { return start; }
   const Pos60& getEnd()   const { return end; }

   const std::vector<Dir60>& getDirections() const { return directions; }

   bool isAnotherHopOk(Dir60 next_dir) const
   {
      assert(isHop());

      Pos60 new_end = end;
      new_end.move(next_dir, 2);

      Pos60 pos = start;

      if(new_end == pos) return false;

      for(const auto& dir : directions)
      {
         pos.move(dir, 2);

         if(new_end == pos) return false;
      }

      return true;
   }

   //! Make the move a single step
   void step(Dir60 dir)
   {
      assert(directions.empty());

      is_step = true;
      directions.push_back(dir);

      end.move(dir, 1);
   }

   //! Make a move a single hop or add an extra hop to a list hops
   void hop(Dir60 dir)
   {
      assert(!is_step);

      directions.push_back(dir);

      end.move(dir, 2);
   }

private:
   Pos60              start;
   bool               is_step{false};
   std::vector<Dir60> directions;
   Pos60              end;
};

using MoveList = std::vector<Move>;

#endif
