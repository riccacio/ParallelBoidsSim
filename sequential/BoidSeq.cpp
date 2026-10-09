#include "BoidSeq.h"
#include <cmath>

// Parametri di calibrazione dell'algoritmo
const float TURN_FACTOR = 0.2f;         // Fattore di virata per rientrare nei bordi
const float VISUAL_RANGE = 40.0f;       // Raggio visivo per allineamento e coesione
const float PROTECTED_RANGE = 8.0f;     // Raggio per la separazione (evitare collisioni)
const float CENTERING_FACTOR = 0.0005f; // Fattore di coesione
const float MATCHING_FACTOR = 0.05f;    // Fattore di allineamento
const float AVOID_FACTOR = 0.05f;       // Fattore di separazione
const float MAX_SPEED = 6.0f;           // Velocità massima consentita
const float MIN_SPEED = 3.0f;           // Velocità minima consentita
const float MARGIN = 100.0f;            // Margine dai bordi della finestra


void updateBoidPosition(Boid* boid, const Boid* flock, int numBoids, float deltaTime, int windowWidth, int windowHeight) {
    float xpos_avg = 0.0f, ypos_avg = 0.0f;
    float xvel_avg = 0.0f, yvel_avg = 0.0f;
    float close_dx = 0.0f, close_dy = 0.0f;
    int neighboring_boids = 0;

    // Itera su tutti gli altri boids per calcolare le interazioni
    for (int i = 0; i < numBoids; ++i) {
        const Boid& other = flock[i];

        // Calcolo della distanza euclidea al quadrato per ottimizzare le performance
        float dx = boid->x - other.x;
        float dy = boid->y - other.y;

        // Se il boid è all'interno del raggio visivo (e non è se stesso)
        if (std::fabs(dx) < VISUAL_RANGE && std::fabs(dy) < VISUAL_RANGE) {

            float squared_dist = (dx * dx) + (dy * dy);

            if (squared_dist < PROTECTED_RANGE * PROTECTED_RANGE) {
                // Regola 1: Separation
                close_dx += dx;
                close_dy += dy;
            }
            else if (squared_dist < VISUAL_RANGE * VISUAL_RANGE) {
                // Raccolta dati per Alignment e Cohesion
                xpos_avg += other.x;
                ypos_avg += other.y;
                xvel_avg += other.vx;
                yvel_avg += other.vy;
                neighboring_boids++;
            }
        }
    }

    // Se ci sono vicini, applica Allineamento e Coesione
    if (neighboring_boids > 0) {
        xpos_avg /= static_cast<float>(neighboring_boids);
        ypos_avg /= static_cast<float>(neighboring_boids);
        xvel_avg /= static_cast<float>(neighboring_boids);
        yvel_avg /= static_cast<float>(neighboring_boids);

        // Cohesion (muoversi verso il centro di massa dei vicini)
        boid->vx += (xpos_avg - boid->x) * CENTERING_FACTOR;
        boid->vy += (ypos_avg - boid->y) * CENTERING_FACTOR;

        //Alignment (allinearsi alla velocità media dei vicini)
        boid->vx += (xvel_avg - boid->vx) * MATCHING_FACTOR;
        boid->vy += (yvel_avg - boid->vy) * MATCHING_FACTOR;
    }

    // Applica il fattore di repulsione calcolato nella Separazione
    boid->vx += (close_dx * AVOID_FACTOR);
    boid->vy += (close_dy * AVOID_FACTOR);

    // Regole per mantenere i boids all'interno dello schermo
    if (boid->x < MARGIN) //top
        boid->vx += TURN_FACTOR;
    if (boid->x > windowWidth - MARGIN) //right
        boid->vx -= TURN_FACTOR;
    if (boid->y < MARGIN) //left
        boid->vy += TURN_FACTOR;
    if (boid->y > windowHeight - MARGIN) //bottom
        boid->vy -= TURN_FACTOR;

    // Limitazione della velocità tra il range MIN_SPEED e MAX_SPEED
    float speed = std::sqrt(boid->vx * boid->vx + boid->vy * boid->vy);
    if (speed < MIN_SPEED) {
        boid->vx = (boid->vx / speed) * MIN_SPEED;
        boid->vy = (boid->vy / speed) * MIN_SPEED;
    } else if (speed > MAX_SPEED) {
        boid->vx = (boid->vx / speed) * MAX_SPEED;
        boid->vy = (boid->vy / speed) * MAX_SPEED;
    }

    // Aggiornamento finale della posizione basato sulla nuova velocità e il tempo trascorso
    boid->x += boid->vx * deltaTime;
    boid->y += boid->vy * deltaTime;
}