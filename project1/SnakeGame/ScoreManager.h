#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API ScoreManager {
public:
    //Funzione per salvare il punteggio
    static void SaveScore(const std::string& fileName, int score, int elapsedTime);
};

#endif // SCOREMANAGER_H
