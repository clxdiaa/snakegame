#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#ifdef SCOREMANAGER_EXPORTS
#define SCOREMANAGER_API __declspec(dllexport)
#else
#define SCOREMANAGER_API __declspec(dllimport)
#endif

extern "C" {
    SCOREMANAGER_API int GetScore(int score); // Funzione esistente

}

#endif
