/*
  ==============================================================================

    TapeHysteresis.h
    Created: 13 Sep 2025 6:11:00pm
    Author:  Marcos

  ==============================================================================
*/

#pragma once
#include <cmath>

// Modelo simplificado de saturación con histeresis magnética
class TapeHysteresis
{
public:
    TapeHysteresis() = default;

    // Procesar una muestra
    float process(float x)
    {
        // a: drive (ganancia de entrada)
        // b: memoria (simula lazo de histeresis)
        float y = std::tanh(a * x + b * prev);
        prev = y;

        // compensación de ganancia
        float compGanan = 1.0f / (1.0f + 0.3f * (a - 1.0f));
        // compensación según memoria
        float compMemo = 1.0f / (1.0f + 2.0f * b);
        return y * compMemo;
      
    }

    // Resetear memoria
    void reset() { prev = 0.0f; }

    // Setters para parámetros ajustables
    void setDrive(float newDrive) { a = newDrive; }
    void setMemory(float newMemory) { b = newMemory; }

private:
    float prev = 0.0f;  // memoria interna
    float a = 2.0f;     // drive por defecto
    float b = 0.8f;     // memoria por defecto
};
