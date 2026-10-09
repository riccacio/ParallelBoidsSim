#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "BoidSeq.h" // Assicurati che il percorso sia corretto in base alla tua struttura cartelle

int main() {
    // 1. Parametri di simulazione
    const int numBoids = 1500; // Valore di partenza consigliato per i test
    const int windowWidth = 1280;
    const int windowHeight = 720;
    const int numOfRuns = 10;

    // FLAG FONDAMENTALE: Imposta a 'false' quando vorrai raccogliere i dati di speedup per il report
    const bool ENABLE_GRAPHICS = true;

    // 2. Inizializzazione dello stormo (Approccio AoS sequenziale)
    std::vector<Boid> flock(numBoids);

    // Usiamo un seme fisso per avere esperimenti sempre riproducibili tra sequenziale e parallelo
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> disX(0.0f, (float)windowWidth);
    std::uniform_real_distribution<float> disY(0.0f, (float)windowHeight);
    std::uniform_real_distribution<float> disV(-3.0f, 3.0f);

    for (int i = 0; i < numBoids; ++i) {
        flock[i].x = disX(gen);
        flock[i].y = disY(gen);
        flock[i].vx = disV(gen);
        flock[i].vy = disV(gen);
    }

    // 3. Configurazione SFML 3.0.2
    sf::RenderWindow window;
    if (ENABLE_GRAPHICS) {
        // Sintassi aggiornata SFML 3: Uso delle parentesi graffe per sf::VideoMode
        window.create(sf::VideoMode({(unsigned int)windowWidth, (unsigned int)windowHeight}), "Boids Sequential");
        window.setFramerateLimit(60);
    }

    // Sintassi aggiornata SFML 3: Uso di PrimitiveType::Triangles al posto di Quads (richiede 3 vertici ad agente)[cite: 48]
    sf::VertexArray boidsTriangles(sf::PrimitiveType::Triangles, numBoids * 3);

    float deltaTime = 1.0f;
    int frames = 0;
    double totalTime = 0.0;

    // 4. Ciclo principale della simulazione
    while (!ENABLE_GRAPHICS || window.isOpen()) {

        if (ENABLE_GRAPHICS) {
            // Sintassi aggiornata SFML 3: pollEvent restituisce std::optional ed usa i template[cite: 48]
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
            }
        }

        // --- INIZIO PROFILAZIONE ---
        // Utilizziamo high_resolution_clock per misurare il tempo di calcolo puro
        auto start = std::chrono::high_resolution_clock::now();

        // Algoritmo Sequenziale O(N^2): aggiornamento di ogni boid nel singolo thread
        for (int i = 0; i < numBoids; ++i) {


            //TODO finire di scrivere il for e misurare il numero medio delle run
            for (int j=0; j<numOfRuns; ++j) {
                updateBoidPosition(&flock[i], flock.data(), numBoids, deltaTime, windowWidth, windowHeight);
            }




        }

        auto end = std::chrono::high_resolution_clock::now();
        // --- FINE PROFILAZIONE ---

        // Raccolta delle metriche per il calcolo della media
        std::chrono::duration<double, std::milli> frameTime = end - start;
        totalTime += frameTime.count();
        frames++;

        // Condizione di uscita automatica per i benchmark puri (es. 300 frame di test)
        if (!ENABLE_GRAPHICS && frames >= 300) {
            std::cout << "[SEQUENZIALE] Benchmark completato su " << numBoids << " boids.\n";
            std::cout << "Tempo medio per frame: " << totalTime / frames << " ms\n";
            break;
        }

        // 5. Rendering Grafico (Ignorato durante i benchmark)
        if (ENABLE_GRAPHICS) {
            window.clear(sf::Color::Black);

            // Costruiamo i 3 vertici per ogni agente per visualizzarlo come una freccia/triangolo[cite: 48]
            for (int i = 0; i < numBoids; ++i) {
                float angle = std::atan2(flock[i].vy, flock[i].vx);
                float size = 3.0f;

                // Punta del boid
                boidsTriangles[i * 3 + 0].position = sf::Vector2f(flock[i].x + std::cos(angle) * size * 2.5f, flock[i].y + std::sin(angle) * size * 2.5f);
                // Angolo inferiore sinistro
                boidsTriangles[i * 3 + 1].position = sf::Vector2f(flock[i].x + std::cos(angle + 2.5f) * size, flock[i].y + std::sin(angle + 2.5f) * size);
                // Angolo inferiore destro
                boidsTriangles[i * 3 + 2].position = sf::Vector2f(flock[i].x + std::cos(angle - 2.5f) * size, flock[i].y + std::sin(angle - 2.5f) * size);

                boidsTriangles[i * 3 + 0].color = sf::Color::White;
                boidsTriangles[i * 3 + 1].color = sf::Color::White;
                boidsTriangles[i * 3 + 2].color = sf::Color::White;
            }

            window.draw(boidsTriangles);
            window.display();
        }
    }

    return 0;
}