%module javaupm_waterlevel
%include "../upm.i"

%{
    #include "waterlevel.h"
%}

%include "waterlevel.h"

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_waterlevel");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}