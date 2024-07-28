//#include "AftrUtil_numeric.h"
#include "gtest/gtest.h"
#include "fmt/core.h"
#include "fmt/ostream.h" //must include this header to fmt::print( fout ... is found by compiler!
#include "Vector.h"
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "AftrUtilities.h"
#include <string>

using namespace Aftr;
namespace
{
   TEST( GLViewFishGame, simple_test )
   {
      std::ofstream fout;
      fout.open( "./GLViewDefenseDaemon_simple_test.txt" );
      if( !fout )
         EXPECT_TRUE( false ); //fail if cannot write to file

      {
         Vector v{ 1,2,3 };

         for( auto& x : v )
            x += 1.1f;

         Vector s{ 2.1f, 3.1f, 4.1f };
         EXPECT_TRUE( v == s );
         fmt::print( fout, "{:s}\n", s.toString() );

         std::fill( v.begin(), v.end(), 99.9f );
         Vector nn = { 99.9f ,99.9f ,99.9f };
         EXPECT_TRUE( v == nn );
         fmt::print( fout, "{:s}\n", v.toString() );
      }

      {
         Vector a{ 1,2,3 };
         auto beg = a.begin();
         Vector::iterator end = a.end();
         EXPECT_TRUE( &(a.x) == &(*beg) );
         fmt::print( fout, "[x]={}[0]={}\n", fmt::ptr( &a.x ), fmt::ptr( (&(*beg)) ) ); beg++;
         //ss << "[x]=" << &a.x;    ss << "[0]=" << (&(*beg)) << "\n"; beg++;
         EXPECT_TRUE( &(a.y) == &(*beg) );
         fmt::print( fout, "[y]={}[1]={}\n", fmt::ptr( &a.y ), fmt::ptr( (&(*beg)) ) ); beg++;
         //ss << "[y]=" << &a.y;    ss << "[1]=" << (&(*beg)) << "\n"; beg++;

         EXPECT_TRUE( &(a.z) == &(*beg) );
         fmt::print( fout, "[z]={}[2]={}\n", fmt::ptr( &a.z ), fmt::ptr( (&(*beg)) ) ); beg++;
         //ss << "[z]=" << &a.z;    ss << "[2]=" << (&(*beg)) << "\n"; beg++;
         
         EXPECT_TRUE( (&a.z)+1 == &(*beg) );
         //ss << "[3]=" << (&(*beg));
         fmt::print( fout, "[3]={}\n", fmt::ptr( (&(*beg)) ) );
      }

      {
         VectorD v{ 1.1,22.2,3.3 };
         for( const auto& x : v )
            fmt::print( fout, "{},", x );

         fmt::print( fout, "\nafter sorting:\n" );
         std::sort( v.begin(), v.end(), []( const auto& a, const auto& b ) { return a > b; } );
         for( const auto& x : v )
            fmt::print( fout, "{},", x );

         auto maxElem = std::max_element( v.begin(), v.end() );
         EXPECT_TRUE( *maxElem == *v.begin() );
      }
   }
}