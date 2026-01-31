%% Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
%%
%% Licensed under the MIT License - see LICENSE file for details.

%% Write a vector to a text in the form of a C/C++ array.
%%
%% Params:
%% 'fname'  - the filename to use.
%% 'vname'  - the name of the array.
%% 'v'      - the vector to use.
%% 'type'   - either 'real' or 'complex'. If complex, then the values will be
%%            of the form '{a , 0}' which is compatible with the C++
%%            std::complex<float> type.
%% 'cols'   - The number of values per row after which a newline is inserted.

function save_vector(fname, vname, v, type, cols)

  f = fopen(fname, 'w')

  if (f < 0)
    error('Error opening file for writing')
    return
  endif

  is_complex = strcmp(type, 'complex')

  if is_complex == 0
    fprintf(f, 'const float %s[%d] = \n{\n', vname, length(v))
  else
    fprintf(f, 'const std::complex<float> %s[%d] = \n{\n', vname, length(v))
  endif

  count = 0
  for i = 1:length(v)

    if count == 0
      fprintf(f, '\t')
    endif

    if is_complex == 0
      fprintf(f, '%.6f', v(i))
    else
      fprintf(f, '{%.6f, 0}', v(i))
    endif

    ++count
    if i < length(v)
      fprintf(f, ',')
      if count == cols
        fprintf(f, '\n')
        count = 0
      else
        fprintf(f, ' ')
      endif
    endif
  endfor

  fprintf(f, '\n};')
  fclose(f)
end
