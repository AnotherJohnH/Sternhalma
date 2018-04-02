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

#include "Game.h"

#include "TRM/App.h"

#include "PLT/Event.h"

#define  PROGRAM         "Sternhalma"
#define  DESCRIPTION     "Sternhalma (aka Chinese Chequers)"
#define  LINK            "https://github.com/AnotherJohnH/Platform"
#define  COPYRIGHT_YEAR  "2017"
#define  AUTHOR          "John D. Haughton"
#define  VERSION         PROJ_VERSION


class SternhalmaApp : public TRM::App
{
private:
   GameOptions options;

   template <unsigned SIZE>
   void play(TRM::Curses& win)
   {
      Game<SIZE> game(win, options);

      PLT::Event::mainLoop(Game<SIZE>::doIterate, &game);
   }

   virtual int startTerminalApp(TRM::Device& term) override
   {
      TRM::Curses win(&term);

      win.clear();

      switch(options.size)
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
   SternhalmaApp(int argc, const char* argv[])
      : TRM::App(PROGRAM, DESCRIPTION, LINK, AUTHOR, VERSION, COPYRIGHT_YEAR)
   {
      parseArgsAndStart(argc, argv);
   }
};


int main(int argc, const char* argv[])
{
   SternhalmaApp(argc, argv);
}
