% Tape Hysteresis Simulation
% --------------------------
clear; clc; close all;

% Parámetros de la función
a = 2.0;   % drive
b = 0.8;   % memoria (hysteresis)

% Señal de entrada: barrido lento tipo rampa
N = 2000;
x = linspace(-2, 2, N);

% Variables
y = zeros(1, N);
prev = 0;

% Procesar muestra a muestra con la ecuación
for n = 1:N
    y(n) = tanh(a * x(n) + b * prev);
    prev = y(n);
end

% Graficar la curva B-H (entrada vs salida)
figure;
plot(x, y, 'LineWidth', 2);
grid on;
xlabel('Entrada x[n]');
ylabel('Salida y[n]');
title(sprintf('Modelo de Histeresis Simplificado (a=%.2f, b=%.2f)', a, b));

% Graficar la función de transferencia completa (histéresis loop)
figure;
plot(x, y, 'LineWidth', 2);
hold on;
plot(x, tanh(a * x), '--r', 'LineWidth', 1.2); % referencia sin memoria
grid on;
xlabel('Entrada');
ylabel('Salida');
legend('Con histéresis', 'tanh sin memoria');
title('Comparación saturación simple vs histéresis');
