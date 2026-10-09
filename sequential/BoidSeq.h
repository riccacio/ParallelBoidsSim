#pragma once

struct Boid {
    float x, y;   // Posizione
    float vx, vy; // Velocità
};

// Funzione per aggiornare la posizione di un singolo boid.
// Prende in input il boid da aggiornare, l'intero stormo (flock), il numero di boids
// e i parametri per calcolare lo spostamento e i rimbalzi sui bordi.
void updateBoidPosition(Boid* boid, const Boid* flock, int numBoids, float deltaTime, int windowWidth, int windowHeight);

