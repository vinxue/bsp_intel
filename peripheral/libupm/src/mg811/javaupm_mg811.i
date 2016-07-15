%module javaupm_mg811
%include "../upm.i"

%{
    #include "mg811.h"
%}

%include "mg811.h"

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_mg811");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}