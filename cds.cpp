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

    struct srgba_light {
        union {
            struct {
                short r, g, b, a;
            };
            short s[4];
        };
        srgba_light() {}
        srgba_light( short r, short g, short b, short a ) : r(r), g(g), b(b), a(a) {}
        srgba_light( short s[4] ) : r(s[0]), g(s[1]), b(s[2]), a(s[3]) {}
        operator const short*() const {
            return s;
        }
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
            cout << "saving to " << filename << "..." << flush;
            unsigned error = lodepng::encode( filename, *this, w, h );
            if( error ) {
                cerr << "encoder error " << error << ": "
                    << lodepng_error_text(error) << endl;
            }
            cout << " done." << endl;
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
        const int ignoreits,
        const srgba_light light = {-5, -5, -5, 0}
    ) {
        
        cout << "rendering..." << flush;
        const unsigned width = img.w, height = img.h;
        for( unsigned col = 0; col < width; col++ ) {
            const double c = cmin + col * (cmax - cmin) / width;
            double x = x0;
            int its;
            for( its = 0; its < ignoreits; its++ ) x = fn( c, x );
            for( its = ignoreits; its < numits; its++ ) {
                const unsigned row = (height-1) * (x - xmin) / (xmax - xmin);
                if( row < height ) {
                    img.add( col, height - row - 1, light );
                }
                x = fn(c, x);
            }
        }
        cout << " done." << endl;

    }

    auto Fc( auto c, auto x ) {
        return c*x*(1. - x);
    }

    auto Qc( auto c, auto x ) {
        return x*x + c;
    }
}

int main() {

    const unsigned w = 2880 * 4, h = 1800;
    cout << "allocating memory for a " << w << " x " << h << " image buffer..." << flush;
    simg img( w, h, {0, 0, 0, 255} );
    cout << " done." << endl;

    char fname[1024]; // buffer for formatted file name

    // Fc full range is c:[1, 4] x:[0, 1]
    /*
    make_pretty_picture<double, Fc>( img, 3.63, 4., 0.5, 0., 1., 10000, 100 );
    sprintf( fname, "Fc_%ux%u.png", w, h );
    img.save( string(fname) );
    */

    // Qc full range is c:[-2, 0.25] x:[-2, 2] */
    make_pretty_picture<double, Qc>( img, -2, 0.25, 0., -2., 2., 20000, 100, {5, 5, 5, 0});
    sprintf( fname, "Qc_%ux%u.png", w, h );
    img.save( string(fname) );


    /*
    cout << "generating image..." << flush;
    //make_pretty_picture<double, Qc>( img, -2., .25, 0., -2., 2., 100000, 100 );
    make_pretty_picture<double, Qc>( img, -1.406, -1.39, 0., -1.406, -1.39, 500000, 100 );
    cout << " done." << endl;
    char name[256];
    sprintf( name, "Qc_%ux%u.png", w, h );
    cout << "saving image to file " << name << "..." << flush;
    img.save( string(name) );
    cout << " done." << endl;
    */

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
