#include "ScoreManager.h"

static int score = 0; // Variabile condivisa

extern "C" {
    // Ottiene il punteggio corrente
    SCOREMANAGER_API int GetScore(int score) {
    return score;
}

}
//dopo prova a chiedere a chatgpt se tornando al codice iniziale senza getscore si puo modificare il comportamento di getscore e usarla solo nella ultima agina della restart per stampare il punteggop
