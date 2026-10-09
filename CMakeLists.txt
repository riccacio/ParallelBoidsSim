cmake_minimum_required(VERSION 3.20)
# Imposta il nome del progetto
project(ParallelBoidsSim)

# Utilizza lo standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trova le dipendenze necessarie
find_package(OpenMP REQUIRED)
find_package(SFML COMPONENTS Graphics Window System REQUIRED)

set(SOURCE_SEQ
        sequential/mainSeq.cpp
        sequential/boidSeq.cpp
        sequential/boidSeq.h
)

# Definisci il tuo eseguibile (aggiungi qui tutti i tuoi file .cpp, ad esempio src/boids.cpp)
add_executable(ParallelBoidsSim_seq ${SOURCE_SEQ})

# Collega le librerie (OpenMP e SFML) all'eseguibile
target_link_libraries(ParallelBoidsSim_seq PRIVATE
        OpenMP::OpenMP_CXX
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Configurazione dei flag di compilazione per GCC o Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    # 1. Flag per le ottimizzazioni generali (Release/Benchmark)
    # Riducono i tempi di esecuzione istruendo il compilatore a ottimizzare i cicli[cite: 2].
    # -ffast-math e -fno-math-errno abilitano ottimizzazioni aggressive sui calcoli in virgola mobile[cite: 2].
    target_compile_options(ParallelBoidsSim_seq PRIVATE $<$<CONFIG:Release>:-O3 -ffast-math -fno-math-errno>)

    # 2. Flag specifici per l'architettura x86_64 (Il tuo PC Fisso)
    # -march=native abilita i set di istruzioni supportati dalla macchina[cite: 2].
    # -mavx2 e -mfma abilitano le istruzioni SIMD a 256 bit e le operazioni di moltiplicazione e addizione fuse[cite: 2].
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        target_compile_options(ParallelBoidsSim_seq PRIVATE $<$<CONFIG:Release>:-march=native -mavx2 -mfma>)
    endif()

    # 3. Sanity Checks per la modalità Debug
    # Utili per identificare memory leak, overflow e comportamenti indefiniti durante lo sviluppo[cite: 2].
    # Si utilizza -O1 per mitigare il rallentamento causato dai sanitizer[cite: 1].
    target_compile_options(ParallelBoidsSim_seq PRIVATE
            $<$<CONFIG:Debug>:-g -O1 -fsanitize=address,leak,undefined>
    )
    target_link_options(ParallelBoidsSim_seq PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,leak,undefined>
    )
endif()