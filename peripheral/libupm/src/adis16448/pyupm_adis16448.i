// Include doxygen-generated documentation
%include "pyupm_doxy2swig.i"
%module pyupm_adis16448
%include "../upm.i"

%{
    #include "adis16448.h"
%}

%include "adis16448.h"
