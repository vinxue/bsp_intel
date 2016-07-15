%module javaupm_ina132
%include "../upm.i"

%{
    #include "ina132.h"
%}

%include "ina132.h"

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_ina132");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}