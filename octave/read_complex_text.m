%% Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
%%
%% Licensed under the MIT License - see LICENSE file for details.

%% Read a text file of complex values of the form a +/- bj. They must all be on
%% one line (i.e., no newlines) separated by white space. 'a' and 'b' are
%% expected to be of type 'float'.
%%
%% Params:
%% fname: the name of the file to read.

function v = read_complex_text(fname)

  f = fopen(fname, 'r')

  if (f < 0)
    error('Error opening file for reading')
    return
  endif

  v = cell2mat(textscan(f, "%f"))'

endfunction

