%% HALF_BAND_HILBERT_FIR Design a Half-Band Hilbert Transform FIR filter.
%%
%% Usage:
%%   h = half_band_hilbert_fir(num_taps)
%%   h = half_band_hilbert_fir(num_taps, window_type)
%%
%% Parameters:
%%   num_taps    : Total number of filter coefficients (taps).
%%                 MUST be an ODD integer (Type 3 FIR) to satisfy the
%%                 half-band condition where every second tap equals 0.
%%   window_type : String specifying the window function (default: 'hamming').
%%                 Options: 'rect', 'hamming', 'hanning', 'blackman'.
%%
%% Returns:
%%   h           : Vector of filter coefficients.

function h = half_band_hilbert_fir(num_taps, window_type)
    % Set default window if not provided
    if nargin < 2
        window_type = 'hamming';
    end

    % 1. Enforce Odd Taps for Half-Band Hilbert constraints (Type 3 FIR)
    if mod(num_taps, 2) == 0
        error('Half-band Hilbert transformers require an ODD number of taps.');
    end

    % Filter order M
    M = num_taps - 1;

    % Shifted time index centered around the midpoint
    n = (0:M) - M/2;

    % 2. Calculate the Ideal Half-Band Hilbert Impulse Response
    % For a half-band variant, the ideal response simplifies mathematically:
    % h_ideal(n) = (2/pi) * (sin(pi*n/2)^2 / n) for n != 0
    % This forces every even value of 'n' to become zero.
    h = zeros(1, num_taps);

    % Avoid division by zero at the center tap (n = 0)
    nonzero_idx = (n ~= 0);
    h(nonzero_idx) = (2 / pi) * (sin(pi * n(nonzero_idx) / 2).^2) ./ n(nonzero_idx);

    % Center tap for a Hilbert filter is always exactly 0 due to anti-symmetry
    h(n == 0) = 0;

    % 3. Apply the Window Function
    switch lower(window_type)
        case {'rect', 'boxcar'}
            w = ones(1, num_taps);
        case 'hamming'
            w = hamming(num_taps)';
        case 'hanning'
            w = hanning(num_taps)';
        case 'blackman'
            w = blackman(num_taps)';
        otherwise
            error('Unknown window type. Use "rect", "hamming", "hanning", or "blackman".');
    end

    % Multiply the ideal impulse response by the window
    h = h .* w;

    % Force exact structural zeros to eliminate floating-point precision noise.
    % For a half-band Hilbert, every even index relative to the center is 0.
    h(mod(n, 2) == 0) = 0;
end

