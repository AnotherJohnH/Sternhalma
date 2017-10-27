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

#ifndef GAME_H
#define GAME_H

#include "STB/Option.h"

#include "Board.h"
#include "Player.h"


struct GameOptions
{
   STB::Option<unsigned> num_players{  'p', "players", "Number of players", 2};
   STB::Option<unsigned> size{         's', "size",    "Size [3-9]", 5};
   STB::Option<unsigned> speed{        'T', "speed",   "Speed of play (ms)", 500};
   STB::Option<unsigned> human_players{'H', "humans",  "Number of humans", 0};
};


template <unsigned SIZE> class Game
{
public:
   enum Mode
   {
      START_GAME,
      START_TURN,
      MID_TURN
   };

   TRM::Curses&       win;
   const GameOptions& options;
   Board<SIZE>        board;
   Player<SIZE>       players[6];
   int8_t             ch{'\0'};
   unsigned           i{0};
   unsigned           turn{0};
   Mode               mode{START_GAME};

   Game(TRM::Curses& win_, const GameOptions& options_)
      : win(win_)
      , options(options_)
      , board(win_)
   {}

   bool iterate()
   {
      char text[16];

      switch(mode)
      {
      case START_GAME:
         board.clear();

         for(i = 0; i < options.num_players; i++)
         {
            players[i].initialise(board, 1 + i * (6.0 / options.num_players),
                                  i < options.human_players);
         }

         turn = 0;
         i    = 0;

         win.timeout(options.speed);

         mode = START_TURN;
         break;

      case START_TURN:
         sprintf(text, "Player %d", i + 1);
         win.mvaddstr(1, win.cols - 15, text);

         sprintf(text, "%3u", ++turn);
         win.mvaddstr(1, win.cols - 3, text);

      case MID_TURN:
         if(players[i].takeATurn(mode == START_TURN, ch))
         {
            if(players[i].areAllPegsHome())
            {
               win.timeout(0);
               mode = START_GAME;
            }
            else
            {
               if(++i == options.num_players) i = 0;
               mode = START_TURN;
            }
         }
         else
         {
            mode = MID_TURN;
         }
         break;
      }

      ch = win.getch();
      return (ch != -1) && (ch != 'q');
   }

   static bool doIterate(void* that)
   {
      return reinterpret_cast<Game<SIZE>*>(that)->iterate();
   }
};


#endif
