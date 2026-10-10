#include <vector>
#include "StableMatching.hpp"
#include <queue>

void invertPrefs(std::vector<std::vector<std::size_t>> &prefs)
{
    const std::size_t n = prefs.size();

    for (std::size_t i = 0; i < n; i++)
    {
        for (std::size_t j = 0; j < n; j++)
        {
            const std::size_t k = prefs[i][j] % n;
            prefs[i][k] += j * n;
        }
        for (std::size_t j = 0; j < n; j++)
        {
            prefs[i][j] /= n;
        }
    }
}

std::vector<std::size_t> GaleShapley(const std::vector<std::vector<std::size_t>> &malePrefs,
                                     std::vector<std::vector<std::size_t>> &femalePrefs)
{
    invertPrefs(femalePrefs);
    std::size_t n = malePrefs.size();
    std::queue<std::size_t> q;
    std::vector<std::size_t> engagements(n, n);
    std::vector<std::size_t> currentRank(n, 0);
    for (std::size_t i = 0; i < n; i++)
    {
        q.push(i);
    }
    while (!q.empty())
    {
        std::size_t man = q.front();
        q.pop();
        std::size_t woman = malePrefs[man][currentRank[man]];
        if (engagements[woman] == n)
        {
            engagements[woman] = man;
        }
        else
        {
            std::size_t partner = engagements[woman];
            if (femalePrefs[woman][partner] < femalePrefs[woman][man])
            {
                currentRank[man]++;
                q.push(man);
            }
            else
            {
                engagements[woman] = man;
                currentRank[partner]++;
                q.push(partner);
            }
        }
    }
    return engagements;
}