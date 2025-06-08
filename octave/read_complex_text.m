function v = read_complex_text(fname)

  f = fopen(fname, 'r')

  if (f < 0)
    error('Error opening file for reading')
    return
  endif

  v = cell2mat(textscan(f, "%f"))'

endfunction

