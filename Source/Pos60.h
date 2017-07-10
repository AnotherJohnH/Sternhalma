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

#ifndef POS_60_H
#define POS_60_H

//! Representation of a 6 way direction
class Dir60
{
public:
   Dir60() = default;

   Dir60(signed n)
   {
      rotRight(n);
   }

   //! Returns the direction as an angle (degrees)
   operator unsigned() const { return (STEP / 2) + value * STEP; }

   //! Rotate the direction 'n' increments of 60 degrees to the right (clockwise)
   Dir60 rotRight(signed n=1)
   {
      value = (value + n + N) % N;
      return *this;
   }

   //! Rotate the direction 'n' increments of 60 degrees to the left (anti-clockwise)
   Dir60 rotLeft(signed n=1)
   {
      rotRight(-n);
      return *this;
   }

   //! Rotate the direction 180 degrees
   Dir60 rot180()
   {
      rotRight(N / 2);
      return *this;
   }

private:
   static const unsigned STEP = 60;
   static const unsigned N    = 360 / STEP;

   int8_t value{0};
};


//! Representation of a position on a 6-way grid
class Pos60
{
public:
   Pos60() = default;

   Pos60(Dir60 dir60, signed n)
      : Pos60()
   {
      move(dir60, n);
   }

   Pos60(const Pos60& from, Dir60 dir60, signed n)
      : Pos60(from)
   {
      move(dir60, n);
   }

   //! Get the abosolute position
   signed getX() const { return x; }
   signed getY() const { return y; }

   //! Test for equivalence
   bool operator==(const Pos60& p) const
   {
      return (x == p.x) && (y == p.y);
   }

   //! Move relative to the current position
   void move(Dir60 dir60, signed n = 1)
   {
      switch(dir60)
      {
      case  30: y += n; x += n;   break;
      case  90:         x += 2*n; break;
      case 150: y -= n; x += n;   break;
      case 210: y -= n; x -= n;   break;
      case 270:         x -= 2*n; break;
      case 330: y += n; x -= n;   break;
      }
   }

private:
   signed x{0}, y{0};
};


#endif
