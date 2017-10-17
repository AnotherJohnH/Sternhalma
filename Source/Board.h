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

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cassert>
#include <cstdio>
#include <vector>

#include "PLT/Curses.h"

#include "Pos60.h"


enum Action : char
{
   ACT_NONE,
   ACT_PICK,
   ACT_HOP,
   ACT_DROP
};


template <unsigned N>
class Board
{
public:
   Board(PLT::Curses& win_)
      : win(win_)
   {
      offset_x = (win.cols - X_SIZE) / 2;
      offset_y = 1 + (win.lines - Y_SIZE) / 2;

      clear();
   }

   bool isEmpty(const Pos60& pos) const
   {
      uint8_t c = getCell(pos);
      return (c != INVALID) && !isOccupied(pos);
   }

   bool isOccupied(const Pos60& pos) const
   {
      uint8_t peg = getCell(pos) & 0x0F;
      return peg != 0;
   }

   bool isPegHome(const Pos60& peg_pos) const
   {
      uint8_t player = getCell(peg_pos) & 0x0F;
      return (getCell(peg_pos) >> 4) == player;
   }

   void setEmpty(const Pos60& pos)
   {
      assert(isOccupied(pos));
      setCell(pos, 0xF0, 0);
   }

   void setHome(const Pos60& pos, unsigned player)
   {
      setCell(pos, 0x0F, uint8_t(player << 4));
   }

   void setPeg(const Pos60& pos, unsigned player)
   {
      assert(isEmpty(pos));
      setCell(pos, 0xF0, uint8_t(player));
   }

   void showAction(const Pos60& pos, Action action)
   {
      unsigned x, y;
      if(!getXY(pos, x, y)) return;

      char lch{}, rch{};

      x += offset_x;
      y += offset_y;

      switch(action)
      {
      case ACT_NONE: lch = ' '; rch = ' '; break;
      case ACT_PICK: lch = '<'; rch = '>'; break;
      case ACT_HOP:  lch = '['; rch = ']'; break;
      case ACT_DROP: lch = '>'; rch = '<'; break;
      }

      win.fgcolour(7);
      win.mvaddch(y, x - 1, lch);
      win.mvaddch(y, x + 1, rch);
   }

   void clear()
   {
      for(unsigned y = 0; y < Y_SIZE; y++)
      {
         for(unsigned x = 0; x < X_SIZE; x++)
         {
            cell[x][y] = INVALID;
         }

         // 'n' the number of empty cells in this row
         unsigned n;

              if (y <  N)        n =      y + 1;
         else if (y < (N*2 + 1)) n = Y_SIZE - y;
         else if (y < (N*3 + 1)) n =      y + 1;
         else                    n = Y_SIZE - y;

         unsigned middle_x = N * 3;
         unsigned x        = middle_x + 1 - n;

         for(unsigned i = 0; i < n; i++)
         {
            cell[x][y] = EMPTY;
            x += 2;
         }
      }

      for(unsigned player = 1; player <= 6; player++)
      {
         Dir60 dir(player + 2);
         Pos60 row(dir, N);

         dir.rotLeft();
         row.move(dir);

         Dir60 across = dir;
         across.rotLeft();

         for(unsigned j = N; j >= 1; j--)
         {
            Pos60 pos = row;

            for(unsigned k = 0; k < j; k++)
            {
               setHome(pos, player);

               pos.move(across);
            }

            row.move(dir);
         }
      }
   }

   void setWait(bool wait) const
   {
      static int prev_timeout{0};
      if(wait)
      {
         prev_timeout = win.timeout(0);
      }
      else
      {
         win.timeout(prev_timeout);
      }
   }

   void refresh() const
   {
      for(unsigned y = 0; y < Y_SIZE; y++)
      {
         win.move(y + offset_y, offset_x);

         for(unsigned x = 0; x < X_SIZE; x++)
         {
            uint8_t c    = cell[x][y];
            uint8_t peg  = c & 0x0F;
            uint8_t hole = (c & 0xF0) >> 4;

            if(peg != 0)
            {
               win.fgcolour(peg);
               win.addch('o');
            }
            else if(hole != 0)
            {
               win.fgcolour(hole);
               win.addch('.');
            }
            else
            {
               win.fgcolour(7);
               win.addch(' ');
            }
         }
         win.addch('\n');
      }
   }

private:
   static bool getXY(const Pos60& pos, unsigned& x, unsigned& y)
   {
      x = OFFSET_X + pos.getX();
      y = OFFSET_Y - pos.getY();

      return (x < X_SIZE) && (y < Y_SIZE);
   }

   void setCell(const Pos60& pos, uint8_t mask, uint8_t value)
   {
      unsigned x, y;
      if(getXY(pos, x, y))
      {
         cell[x][y] = (cell[x][y] & mask) | value;
      }
   }

   const uint8_t getCell(const Pos60& pos) const
   {
      unsigned x, y;
      return getXY(pos, x, y) ? cell[x][y] : INVALID;
   }

   static const uint8_t INVALID = 0x00;
   static const uint8_t EMPTY   = 0x70;

   static const unsigned OFFSET_X = N * 3;
   static const unsigned OFFSET_Y = N * 2;

   static const unsigned X_SIZE = OFFSET_X * 2 + 1;
   static const unsigned Y_SIZE = OFFSET_Y * 2 + 1;

   PLT::Curses& win;
   unsigned     offset_x, offset_y;
   uint8_t      cell[X_SIZE][Y_SIZE];
};

#endif
