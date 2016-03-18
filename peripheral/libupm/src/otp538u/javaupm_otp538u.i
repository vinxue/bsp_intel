%module javaupm_otp538u
%include "../upm.i"

%{
    #include "otp538u.h"
%}

%include "otp538u.h"

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("javaupm_otp538u");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}