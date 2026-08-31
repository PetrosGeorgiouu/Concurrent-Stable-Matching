#include <vector>
#include "GaleShapley.hpp"
#include <queue>

using namespace std;

vector<vector<size_t>> invertPrefs(vector<vector<size_t>>& prefs) {
    size_t n = prefs.size();
    vector<vector<size_t>> invertedPrefs(n, vector<size_t>(n));

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            invertedPrefs[i][prefs[i][j]] = j;
        }
    }
    return invertedPrefs;
}

vector<size_t> GaleShapley(vector<vector<size_t>>& malePref, vector<vector<size_t>>& femalePrefs) {
    vector<vector<size_t>> invertedPrefs = invertPrefs(femalePrefs);
    size_t n = malePref.size();
    queue<size_t> q;
    vector<size_t> engagements(n, n);
    vector<size_t> currentRank(n, 0);
    for (size_t i = 0; i < n; i++) {
        q.push(i);
    }
    while (!q.empty()) {
        size_t man = q.front();
        q.pop();
        size_t woman = malePref[man][currentRank[man]];
        if (engagements[woman] == n) {
            engagements[woman] = man;
        }
        else {
            size_t partner = engagements[woman];
            if (invertedPrefs[woman][partner] < invertedPrefs[woman][man]) {
                currentRank[man]++;
                q.push(man);
            }
            else {
                engagements[woman] = man;
                currentRank[partner]++;
                q.push(partner);
            }
        }
    }
    return engagements;
}