// cds.cpp - 2016.04.22 - Atlee Brink
// Chaotic Dynamical Systems plotter

#include "lodepng.h"

#include <cstdio>
#include <iostream>
#include <vector>

namespace {
    using namespace std;

    struct srgba {
        union {
            struct {
                unsigned char r, g, b, a;
            };
            unsigned char c[4];
        };
        srgba() {}
        srgba( short r, short g, short b, short a ) : r(r), g(g), b(b), a(a) {}
        srgba( short c[4] ) : r(c[0]), g(c[1]), b(c[2]), a(c[3]) {}
    };

    struct simg {
        const unsigned w, h;
        vector<srgba> p;
        simg( unsigned w, unsigned h, srgba pel = {0, 0, 0, 0} )
            : w(w), h(h), p( vector<srgba>( w*h, pel ) ) {}
        operator const unsigned char*() const {
            return reinterpret_cast<const unsigned char*>(p.data());
        }
        void add( unsigned x, unsigned y, const short pel[4] ) {
            srgba &rp = p[ y*w + x ];
            for( int c = 0; c < 4; c++ ) {
                short v = pel[c] + rp.c[c];
                rp.c[c] = (unsigned char)( v < 0 ? 0 : v > 255 ? 255 : v );
            }
        }
        void plot( unsigned x, unsigned y, srgba pel ) { p[ y*w + x ] = pel; }
        void save( const string &filename ) const {
            unsigned error = lodepng::encode( filename, *this, w, h );
            if( error ) {
                cerr << "encoder error " << error << ": "
                    << lodepng_error_text(error) << endl;
            }
        }
    };


    template< double(*fn)(double, double) >
    auto test( auto c, auto x ) {
        return fn( c, x );
    }

    template< class T, T(*fn)(T, T) >
    void make_pretty_picture(
        simg &img,
        const T cmin,
        const T cmax,
        const T x0,
        const T xmin,
        const T xmax,
        const int numits,
        const int ignoreits
    ) {
        
        for( unsigned col = 0; col < img.w; col++ ) {
            const double c = cmin + col * (cmax - cmin) / img.w;
            double x = x0;
            int its;
            for( its = 0; its < ignoreits; its++ ) x = fn( c, x );
            for( its = ignoreits; its < numits; its++ ) {
                const unsigned row = (img.h-1) * (x - xmin) / (xmax - xmin);
                if( row < img.h ) {
                    short paint[4] = {2, 2, 2, 0}; //{-5, -5, -5, 0};
                    img.add( col, img.h - row - 1, paint );
                }
                x = fn(c, x);
            }
        }

    }

    auto Fc( auto c, auto x ) {
        return c*x*(1. - x);
    }

    auto Qc( auto c, auto x ) {
        return x*x + c;
    }
}

int main() {


    const unsigned w = 2880 * 5, h = 1800;
    cout << "allocating memory for a " << w << " x " << h << " image buffer..." << flush;
    simg img( w, h, {0, 0, 0, 255} );
    cout << " done." << endl;


    cout << "generating image..." << flush;
    make_pretty_picture<double, Qc>( img, -2., .25, 0., -2., 2., 100000, 100 );
    cout << " done." << endl;
    char name[256];
    sprintf( name, "Qc_%ux%u.png", w, h );
    cout << "saving image to file " << name << "..." << flush;
    img.save( string(name) );
    cout << " done." << endl;

    /*
    cout << "generating image..." << flush;
    make_pretty_picture<double, Fc>( img, 1., 4., 0.5, 0., 1., 100000, 100 );
    cout << " done." << endl;
    char name[256];
    sprintf( name, "Fc_%ux%u.png", w, h );
    cout << "saving image to file " << name << "..." << flush;
    img.save( string(name) );
    cout << " done." << endl;
    */

    return 0;
}
