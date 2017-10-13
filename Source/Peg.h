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

#ifndef PEG_H
#define PEG_H

#include "Board.h"
#include "Move.h"

template <unsigned N>
class Peg
{
public:
   const Pos60& getPos() const { return pos; }
         Pos60& getPos()       { return pos; }

   bool isHome() const
   {
      return board->isPegHome(pos);
   }

   //! Initialise a peg and place it in it's starting position
   void initialise(Board<N>&    board_,
                   uint8_t      id_,
                   const Pos60& target_,
                   const Pos60& start_pos_)
   {
      board  = &board_;
      id     = id_;
      target = target_;

      move(start_pos_);
   }

   //! Perform a step if possible
   bool tryStep(Dir60 dir)
   {
      Pos60 to(pos, dir, 1);

      if (!board->isEmpty(to)) return false;

      move(to);
      return true;
   }

   //! Perform a hop if possible
   bool tryHop(Dir60 dir)
   {
      Pos60 to(pos, dir, 1);

      if (!board->isOccupied(to)) return false;

      to.move(dir, 1);

      if (!board->isEmpty(to)) return false;

      move(to);
      return true;
   }

   //! Build the list of possible moves for this peg
   unsigned findMoves(bool keep_all_moves)
   {
      move_list.clear();

      best_move_score = 0;
      best_move       = nullptr;

      for(Dir60 dir; true; dir.rotRight())
      {
         Pos60 to(pos, dir, 1);

         if (board->isEmpty(to))
         {
            // Can step
            Move  move(pos);

            move.step(dir);

            addMove(move, keep_all_moves);
         }
         else if (board->isOccupied(to))
         {
             // Might be able to hop
             to.move(dir, 1);

             if (board->isEmpty(to))
             {
                // Can hop
                Move  move(pos);

                move.hop(dir);

                addMove(move, keep_all_moves);

                tryAnotherHop(move, keep_all_moves);
             }
         }

         if (dir == 330) break;
      }

      return best_move_score;
   }

   //! Do the best move
   //  Returns true when the move is complete
   bool doBestMove(bool start_move)
   {
      if (start_move)
      {
         board->showAction(pos, ACT_PICK);

         move_it = best_move->getDirections().begin();
         return false;
      }

      board->showAction(pos, ACT_NONE);

      if (pos == best_move->getEnd())
      {
         return true;
      }

      Pos60 next_pos = pos;
      Dir60 dir = *move_it;
      next_pos.move(dir, best_move->isStep() ? 1 : 2);
      move(next_pos);

      if (pos == best_move->getEnd())
      {
         board->showAction(pos, ACT_DROP);
      }
      else
      {
         move_it++;
         board->showAction(pos, ACT_HOP);
      }
      return false;
   }

private:
   void move(const Pos60& pos_)
   {
      if (board->isOccupied(pos))
      {
         board->setEmpty(pos);
      }
      pos = pos_;
      board->setPeg(pos, id);
      board->refresh();
   }

   void tryAnotherHop(const Move& move, bool keep_all_moves)
   {
      for(Dir60 dir; true; dir.rotRight())
      {
         if (move.isAnotherHopOk(dir))
         {
            Pos60 to(move.getEnd(), dir, 1);

            if (board->isOccupied(to))
            {
               // Might be able to hop
               to.move(dir, 1);

               if (board->isEmpty(to))
               {
                  // Can hop
                  Move  another_move = move;

                  another_move.hop(dir);

                  addMove(another_move, keep_all_moves);

                  tryAnotherHop(another_move, keep_all_moves);
               }
            }
         }

         if (dir == 330) break;
      }
   }

   void addMove(const Move& move, bool keep_all_moves)
   {
      unsigned score   = evaluate(move);
      bool     is_best = score > best_move_score;

      if (keep_all_moves || is_best)
      {
         if (!keep_all_moves)
         {
            move_list.clear();
         }

         move_list.push_back(move);

         if (is_best)
         {
            best_move_score = score;
            best_move       = &move_list.back();
         }
      }
   }

   static unsigned distSquared(const Pos60& from, const Pos60& to)
   {
      signed delta_x = to.getX() - from.getX();
      signed delta_y = to.getY() - from.getY();

      return delta_x * delta_x + delta_y * delta_y * 3;
   }

   unsigned evaluate(const Move& move)
   {
      unsigned dist_before = distSquared(target, move.getStart());
      unsigned dist_after  = distSquared(target, move.getEnd());

      return 1000000 + dist_before - dist_after;
   }

   Board<N>*  board{nullptr};
   uint8_t    id{0};
   Pos60      target;
   Pos60      pos;
   MoveList   move_list;
   unsigned   best_move_score{0};
   Move*      best_move{nullptr};

   std::vector<Dir60>::const_iterator  move_it;
};

#endif
