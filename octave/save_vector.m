
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
