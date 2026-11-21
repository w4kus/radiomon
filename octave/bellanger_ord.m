%% BELLANGER_ORD Estimate FIR filter order using Bellanger's formula.
%%
%% Inputs:
%%   wp          - Passband edge frequency (normalized, 0 to 1, where 1 is Nyquist)
%%   ws          - Stopband edge frequency (normalized, 0 to 1, where 1 is Nyquist)
%%   ripple_pass - Passband specification (linear ripple dp OR peak ripple in dB)
%%   ripple_stop - Stopband specification (linear ripple ds OR min attenuation in dB)
%%   is_db       - (Optional) Set to true (1) if ripple inputs are in dB. Default is false (0).
%%
%% Outputs:
%%   N           - Estimated minimum FIR filter order
%%   taps        - Total number of required filter taps (N + 1)

function [N, taps] = bellanger_ord(wp, ws, ripple_pass, ripple_stop, is_db)

    if nargin < 5
        is_db = false;
    end

    % 1. Convert dB values to linear values if flag is raised
    if is_db
        % ripple_pass in dB represents peak passband ripple (alpha_p)
        dp = 1 - 10^(-ripple_pass / 20);
        % ripple_stop in dB represents stopband attenuation (alpha_s, passed as positive dB)
        ds = 10^(-abs(ripple_stop) / 20);
    else
        dp = ripple_pass;
        ds = ripple_stop;
    end

    % 2. Calculate transition bandwidth in normalized radians/sample
    % Octave frequencies are typically 0 to 1 (normalized to Nyquist).
    % We convert them to radians (0 to pi).
    dw = abs(ws - wp) * pi;

    % 3. Apply Bellanger's Formula
    % N = (-2 * log10(10 * dp * ds)) / (3 * dw / (2 * pi)) - 1
    numerator = -2 * log10(10 * dp * ds);
    denominator = (3 * dw) / (2 * pi);

    N_exact = (numerator / denominator) - 1;

    % 4. Ceil to the nearest integer since order must be a whole number
    N = ceil(N_exact);
    taps = N + 1;
end

