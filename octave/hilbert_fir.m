%% HILBERT_FIR Designs a linear-phase Hilbert Transform FIR filter.
%%
%% Usage:
%%   h = hilbert_fir(N, type_num, window_type)
%%
%% Inputs:
%%   N           - Filter order (Note: Number of taps = N + 1)
%%   type_num    - 3 for Type 3 FIR (N must be even -> odd number of taps)
%%               - 4 for Type 4 FIR (N must be odd -> even number of taps)
%%   window_type - String specifying the window function (e.g., 'hamming', 'blackman', 'rectwin')
%%
%% Outputs:
%%   h           - Vector of impulse response coefficients

function h = hilbert_fir(N, type_num, window_type)
    % 1. Validate inputs and enforce structural constraints
    if nargin < 3
        window_type = 'hamming'; % Default window
    end

    if type_num == 3
        % Type 3 requires an even filter order (odd number of taps)
        if mod(N, 2) ~= 0
            N = N + 1;
            warning('Type 3 FIR requires an even order. N has been bumped up to %d.', N);
        end
    elseif type_num == 4
        % Type 4 requires an odd filter order (even number of taps)
        if mod(N, 2) == 0
            N = N + 1;
            warning('Type 4 FIR requires an odd order. N has been bumped up to %d.', N);
        end
    else
        error('Invalid type_num. Choose either 3 or 4.');
    end

    % 2. Calculate ideal anti-symmetric impulse response
    % Ideal response: h_ideal(n) = (2/pi) * (sin^2(pi*n/2) / n) for n != 0
    % Centered around the midpoint M = N / 2
    M = N / 2;
    n = 0:N;

    % Initialize coefficient array
    h_ideal = zeros(1, N + 1);

    % Compute non-zero entries to avoid division by zero at the center
    idx = (n ~= M);
    h_ideal(idx) = (2 / pi) * (sin(pi * (n(idx) - M) / 2).^2) ./ (n(idx) - M);

    % 3. Apply the selected smoothing window
    % Evaluate the window function dynamically from its string name
    try
        win_func = str2func(window_type);
        win = win_func(N + 1).'; % Ensure it is a row vector
    catch
        error('Window type "%s" is not recognized or available in Octave.', window_type);
    end

    % Final windowed impulse response
    h = h_ideal .* win;
end

