clear all    % Clean all constant and variables
close all    % Close all windows
%clf         % Clean the figure window
clc          % Clean the command window
%echo off    % Inhibits the code view in the command window during its execution

%% Config
fs = 48000;
fc = 30000;
f1 = 750;
f2 = 5000;

L = 20000;
t = 0 : 1/fs : L/f2;

%% Generate real baseband
mx = zeros(1, length(t));

for cf = f1:50:f2
    mx = mx + cos(2 * pi * cf * t);
end

%% Design and analyze Hilbert FIR
Hd = designfilt('hilbertfir', 'FilterOrder', 80, 'TransitionWidth', 0.08, 'DesignMethod', 'equiripple');
hfv = fvtool(Hd, 'Analysis', 'Magnitude', 'MagnitudeDisplay', 'Zero-phase', 'FrequencyRange', '[-pi, pi)');
hfv.Color = 'white';

%% Filter to generate quadrature, delay
my = filter(Hd, mx);

G = filtord(Hd) / 2;
mx = [zeros(1,G), mx(1:end-G)];

%% Generate carrier
cx = cos(2 * pi * fc * t);
cy = cos(2 * pi * fc * t + pi/2);

%% Modulate
sx = cx .* mx;
sy = cy .* my;

s = sx - sy; % + yields LSB, - yields USB

%% Plot results
m = mx + j * my;

[M, fM] = fft_spectrum(m(G+1:end), 2048, fs);
[S, fS] = fft_spectrum(s(G+1:end), 2048, fs);

figure('Name', 'SSB Modulation', 'NumberTitle', 'off');
subplot(3, 1, 1);
plot(t(1:end-G), mx(G+1:end), t(1:end-G), my(G+1:end));
subplot(3, 1, 2);
plot(fM, 10 * log10(abs(M)));
subplot(3, 1, 3);
plot(fS, 10 * log10(abs(S)));

%% Dump coefficients
outfile = fopen('hilbert_filter.h', 'w');
fprintf(outfile, 'static const uint32_t hilbert_fir_taps_size = %u;\n', length(Hd.Coefficients));
fprintf(outfile, 'static const int16_t hilbert_fir_taps[] = {');
for i = 1 : length(Hd.Coefficients) - 1
    fprintf(outfile, '\n    %hi,', int16(Hd.Coefficients(length(Hd.Coefficients) - i + 1) * 32767));
end
fprintf(outfile, '\n    %hi', int16(Hd.Coefficients(1) * 32767));
fprintf(outfile, '\n};');