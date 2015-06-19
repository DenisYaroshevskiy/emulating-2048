# emulating-2048
Recreation of 2048 game in C++, in console

Original game: http://gabrielecirulli.github.io/2048/

I had a task to write smth, so I wrote this.

One can pretty easily add his own solvers, I've tried to comment pretty heavily.

Of course, overall design migh be improved)

Everything is build in VS2015, and you need boost

The basic structure is:
Board - the Game basically. Inside - it's a one dimentional array of Cells. Operated either directly, or through MatrixView

MatrixView - template, that gives interface to iterators range, so one can look at them as double dimntional + 
you can actually get rows and columns, which is nice. Should be rewritten using some range library.

Boost actually has multidimentional array, but after 40 minutes I gave up and made my own bycicle.

For running automatic solvers, an adaptor was written, that does everything concurrently and gives the current situation, 
if you mash a button.

