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

#include "Player.h"
#include "Board.h"

#include  "TerminalApp.h"

#include  "STB/License.h"


#define  PROGRAM         "Sternhalma"
#define  DESCRIPTION     "Sternhalma (aka Chinese Chequers)"
#define  COPYRIGHT_YEAR  "2017"
#define  AUTHOR          "John D. Haughton"
#define  VERSION         PROJ_VERSION


class Sternhalma : public TerminalApp
{
private:
   STB::Option<unsigned>   num_players{  'p', "players", "Number of players",    2};
   STB::Option<unsigned>   size{         's', "size",    "Size [3-7]",           5};
   STB::Option<unsigned>   speed{        'T', "speed",   "Speed of play (ms)", 500};
   STB::Option<unsigned>   human_players{'H', "humans",  "Number of humans",     0};

   template <unsigned SIZE>
   void play(PLT::Curses& win)
   {
      while(true)
      {
         Board<SIZE>   board(win);
         Player<SIZE>  players[6];

         for(unsigned i=0; i<num_players; i++)
         {
            players[i].initialise(board, 1 + i*(6.0/num_players),
                                  i < human_players);
         }

         win.timeout(speed);

         unsigned turn = 0;

         for(unsigned i=0; true; )
         {
            char text[16];
            sprintf(text, "Player %d", i + 1);
            win.mvaddstr(1, win.cols - 15, text);

            sprintf(text, "%3u", ++turn);
            win.mvaddstr(1, win.cols - 3, text);

            char ch = win.getch();
            if ((ch == -1) || (ch == 'q')) return;

            players[i].takeATurn();

            if (players[i].areAllPegsHome())
            {
               break;
            }

            if (++i == num_players) i = 0;
         }

         win.timeout(0);

         char ch = win.getch();
         if ((ch == -1) || (ch == 'q')) return;
      }
   }

   virtual int startWithTerminal(PLT::Device& term) override
   {
      PLT::Curses  win(&term);

      win.clear();

      switch(size)
      {
      case 3: play<3>(win); break;
      case 4: play<4>(win); break;
      case 5: play<5>(win); break;
      case 6: play<6>(win); break;
      case 7: play<7>(win); break;
      case 8: play<8>(win); break;
      case 9: play<9>(win); break;
      }

      return 0;
   }

public:
   Sternhalma(int argc_, const char* argv_[])
      : TerminalApp(argc_, argv_,
                    PROGRAM, AUTHOR, DESCRIPTION, VERSION, COPYRIGHT_YEAR, MIT_LICENSE)
   {
      parseArgsAndStart();
   }
};


int main(int argc, const char *argv[])
{
   Sternhalma(argc, argv);
}

