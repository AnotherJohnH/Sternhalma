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

#ifndef PLAYER_H
#define PLAYER_H

#include <array>

#include "PLT/KeyCode.h"

#include "Board.h"
#include "Peg.h"

template <unsigned N>
class Player
{
public:
   //! Put a players pieces into their initial positions
   void initialise(Board<N>& board_, unsigned id_, bool human_)
   {
      board = &board_;
      human = human_;

      // Compute a direction orthogonal to the way home
      across.rotRight(id_ + 3);

      // Find the home position
      Pos60 home;

      Dir60 back(id_ + 2);
      home.move(back, N);

      back.rotLeft();
      home.move(back, N);

      // Find one end of the front starting row
      Pos60  row;

      Dir60  dir(id_ - 1);
      row.move(dir, N);

      dir.rotLeft();
      row.move(dir);

      unsigned i = 0;

      for(unsigned j=N; j>=1; j--)
      {
         Pos60 pos = row;

         for(unsigned k=0; k<j; k++)
         {
            peg_list[i++].initialise(board_, id_, home, pos);

            pos.move(across);
         }

         row.move(dir);
      }
   }

   //! Player takes a turn
   void takeATurn()
   {
      if (human)
      {
         humanTurn();
      }
      else
      {
         computerTurn();
      }
   }

   bool areAllPegsHome() const
   {
      bool home = true;

      for(auto& peg : peg_list)
      {
         if (!peg.isHome()) return false;
      }

      return home;
   }

private:
   void humanTurn()
   {
      enum { START, STEP, HOP, DONE } state = START;

      size_t index = 0;
      Pos60  start_pos;

      while(state != DONE)
      {
         Peg<N>& peg = peg_list[index];

         switch(state)
         {
         case START: board->showAction(peg.getPos(), ACT_PICK); break;
         case STEP:  board->showAction(peg.getPos(), ACT_DROP); break;
         case HOP:   board->showAction(peg.getPos(), ACT_HOP);  break;
         default: break;
         }

         uint8_t ch = board->getch();

         board->showAction(peg.getPos(), ACT_NONE);

         Dir60 dir;

         switch(ch)
         {
         case 'q':
            return;

         case PLT::LEFT:
            if (state != START)
            {
               //pos = start_pos;
            }
            index = (index == peg_list.size() - 1) ? 0 : index + 1;
            break;

         case PLT::RIGHT:
            index = (index == 0) ? peg_list.size() - 1 : index - 1;
            break;

         case PLT::RETURN:
            if ((state == STEP) || (state == HOP))
            {
               state = DONE;
            }
            break;

         case 'r': dir.rotRight(); // fall through ...
         case 'd': dir.rotRight(); // fall through ...
         case 'c': dir.rotRight(); // fall through ...
         case 'v': dir.rotRight(); // fall through ...
         case 'g': dir.rotRight(); // fall through ...
         case 't':
            switch(state)
            {
            case START:
               //start_pos = pos;
                    if (peg.tryStep(dir)) { state = STEP; }
               else if (peg.tryHop(dir))  { state = HOP;  }
               break;

            case HOP:
               peg.tryHop(dir);
               break;

            default:
               break;
            }
            break;

         default:
            break;
         }
      }
   }

   void computerTurn()
   {
      unsigned best_move_score = 0;
      Peg<N>*  best_peg_to_move{nullptr};

      for(auto& peg : peg_list)
      {
         unsigned score = peg.findMoves(/* keep_all_moves */ false);

         if (score > best_move_score)
         {
            best_move_score = score;
            best_peg_to_move = &peg;
         }
      }

      assert(best_peg_to_move != nullptr);

      best_peg_to_move->doBestMove();
   }

   static constexpr unsigned triangularNumber(unsigned n)
   {
      return n == 0 ? 0 : n + triangularNumber(n - 1);
   }

   static const unsigned COUNTERS = triangularNumber(N);

   Board<N>*                    board{nullptr};
   bool                         human{false};
   Dir60                        across;
   std::array<Peg<N>,COUNTERS>  peg_list;
};

#endif
