%% Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
%%
%% Licensed under the MIT License - see LICENSE file for details.

%% Given a vector 'h' of FIR filter coefficents and a value 'M' which signifies
%% the number of branches (e.g, subfilters or components), decompose
%% the vector into a 'M x length(h)' matrix representing a polyphase structure.
%% For a vector with an odd number of taps (type I and III), a zero is inserted
%% into the last coefficent slot. Note that the number of columns printed will
%% be equal to the number of elements in a branch after polyphase decomposition.
%%
%% Params:
%% 'h'        - the vector of coefficents.
%% 'M'        - the number of branches.
%% 'polyName' - the name of the C/C++ 2-D array [optional, required if 'fname'
%%              is specified].
%% 'fname'    - the file to which to save the array [optional].

function E = poly_decompose(h, M, polyName, fname)

  E = reshape([h zeros(1, mod(-length(h), M))], M, []);
  E = E;

  if nargin == 3
    error("\"polyName\" also needs a filename")
    return
  endif

  if nargin > 3
    f = fopen(fname, "w")

    if (f < 0)
      error("Error opening file for writing");
      return
    endif

    [rows cols] = size(E);
    count = 0;

    fprintf(f, "const float %s[%d][%d] = \n{\n", polyName, rows, cols);
    for i = 1:rows
      fprintf(f, "\t{ ");
      for j = 1:cols
        if j < cols
          fprintf(f, "%.6f, ", E(i,j));
        else
          fprintf(f, "%.6f ", E(i,j));
        endif
        ++count;
      endfor

      if count < rows * cols
        fprintf(f, "},\n");
      else
        fprintf(f, "}\n");
      endif

    endfor

    fprintf(f, "};\n");
    fclose(f);
  endif

endfunction

