# rye register abstraction library

## Goal




## Usage

Rye basically consists of two things: "Bitfield.hpp", which defines the functions and structures used in C++ and "make_cppheader.py", which is a tool to create C++ headers from SVD files.
Strictly speaking, you don't need the python tool and could just create registers definitions by hand, but it will make things a lot easier for you to let the script do the tedious work.
That's also less error prone, provided the SVD is correct (which isn't always the case, unfortunately).

A header file created will look something like this:

"namespace *devicename* {
    namespace *peripheralname* {
        register definitions...
    }
}"
