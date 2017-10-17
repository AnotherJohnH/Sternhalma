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
      // Allow re-initialisation
      new(this) Player();

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
      Pos60 row;

      Dir60 dir(id_ - 1);
      row.move(dir, N);

      dir.rotLeft();
      row.move(dir);

      unsigned i = 0;

      for(unsigned j = N; j >= 1; j--)
      {
         Pos60 pos = row;

         for(unsigned k = 0; k < j; k++)
         {
            peg_list[i++].initialise(board_, id_, home, pos);

            pos.move(across);
         }

         row.move(dir);
      }
   }

   //! Player takes a turn
   bool takeATurn(bool start_turn, uint8_t ch)
   {
      return human ? humanTurn(start_turn, ch)
                   : computerTurn(start_turn);
   }

   bool areAllPegsHome() const
   {
      bool home = true;

      for(auto& peg : peg_list)
      {
         if(!peg.isHome()) return false;
      }

      return home;
   }

private:
   enum
   {
      START,
      STEP,
      HOP
   } move_state{START};

   size_t peg_index{0};

   bool humanTurn(bool start_turn, uint8_t ch)
   {
      if(start_turn)
      {
         move_state  = START;
         peg_index   = 0;
         Peg<N>& peg = peg_list[peg_index];
         board->showAction(peg.getPos(), ACT_PICK);
         board->setWait(true);
         return false;
      }

      Peg<N>* peg = &peg_list[peg_index];

      board->showAction(peg->getPos(), ACT_NONE);

      Dir60 dir;

      switch(ch)
      {
      case PLT::LEFT:
         peg_index = (peg_index == peg_list.size() - 1) ? 0 : peg_index + 1;
         peg       = &peg_list[peg_index];
         break;

      case PLT::RIGHT:
         peg_index = (peg_index == 0) ? peg_list.size() - 1 : peg_index - 1;
         peg       = &peg_list[peg_index];
         break;

      case PLT::RETURN:
         if((move_state == STEP) || (move_state == HOP))
         {
            board->setWait(false);
            return true;
         }
         break;

      case 'r': dir.rotRight(); // fall through ...
      case 'd': dir.rotRight(); // fall through ...
      case 'c': dir.rotRight(); // fall through ...
      case 'v': dir.rotRight(); // fall through ...
      case 'g': dir.rotRight(); // fall through ...
      case 't':
         switch(move_state)
         {
         case START:
                 if (peg->tryStep(dir)) { move_state = STEP; }
            else if (peg->tryHop(dir))  { move_state = HOP;  }
            break;

         case HOP:
            peg->tryHop(dir);
            break;

         default:
            break;
          }
         break;

      default:
         break;
      }

      switch(move_state)
      {
      case START: board->showAction(peg->getPos(), ACT_PICK); break;
      case STEP:  board->showAction(peg->getPos(), ACT_DROP); break;
      case HOP:   board->showAction(peg->getPos(), ACT_HOP);  break;
      default: break;
      }

      return false;
   }

   bool computerTurn(bool start_turn)
   {
      if(start_turn)
      {
         best_peg_to_move = nullptr;

         unsigned best_move_score = 0;

         for(auto& peg : peg_list)
         {
            unsigned score = peg.findMoves(/* keep_all_moves */ false);

            if(score > best_move_score)
            {
               best_move_score  = score;
               best_peg_to_move = &peg;
            }
         }
      }

      assert(best_peg_to_move != nullptr);

      return best_peg_to_move->doBestMove(start_turn);
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
   Peg<N>*                      best_peg_to_move{nullptr};
};

#endif
