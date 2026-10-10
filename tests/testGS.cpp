#include <cstddef>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "StableMatching.hpp"

namespace
{
    class TestFailure : public std::runtime_error
    {
    public:
        explicit TestFailure(const std::string &message)
            : std::runtime_error{message}
        {
        }
    };

    void expect(bool condition, const std::string &message)
    {
        if (!condition)
        {
            throw TestFailure{message};
        }
    }

    std::string formatMatching(const std::vector<std::size_t> &engagements)
    {
        std::ostringstream output;
        output << '[';

        for (std::size_t woman{0}; woman < engagements.size(); ++woman)
        {
            if (woman != 0)
            {
                output << ", ";
            }

            output << "woman " << woman << " -> man " << engagements[woman];
        }

        output << ']';
        return output.str();
    }

    void expectCompleteMatching(
        const std::vector<std::size_t> &engagements,
        std::size_t participantCount)
    {
        expect(
            engagements.size() == participantCount,
            "Expected " + std::to_string(participantCount) +
                " engagements, but received " +
                std::to_string(engagements.size()));

        std::vector<bool> matchedMen(participantCount, false);

        for (std::size_t woman{0}; woman < participantCount; ++woman)
        {
            const std::size_t man{engagements[woman]};

            expect(
                man < participantCount,
                "Woman " + std::to_string(woman) +
                    " was matched to out-of-range man " +
                    std::to_string(man));

            expect(
                !matchedMen[man],
                "Man " + std::to_string(man) +
                    " was matched to more than one woman");

            matchedMen[man] = true;
        }
    }

    void expectStable(
        const std::vector<std::size_t> &engagements,
        const std::vector<std::vector<std::size_t>> &malePref,
        const std::vector<std::vector<std::size_t>> &femalePrefs)
    {
        const std::size_t participantCount{malePref.size()};

        std::vector<std::size_t> partnerOfMan(participantCount);
        for (std::size_t woman{0}; woman < participantCount; ++woman)
        {
            partnerOfMan[engagements[woman]] = woman;
        }

        std::vector<std::vector<std::size_t>> femaleRank(
            participantCount,
            std::vector<std::size_t>(participantCount));

        for (std::size_t woman{0}; woman < participantCount; ++woman)
        {
            for (std::size_t rank{0}; rank < participantCount; ++rank)
            {
                femaleRank[woman][femalePrefs[woman][rank]] = rank;
            }
        }

        for (std::size_t man{0}; man < participantCount; ++man)
        {
            for (std::size_t preferredWoman : malePref[man])
            {
                if (preferredWoman == partnerOfMan[man])
                {
                    break;
                }

                const std::size_t herCurrentPartner{
                    engagements[preferredWoman]};

                const bool shePrefersThisMan{
                    femaleRank[preferredWoman][man] <
                    femaleRank[preferredWoman][herCurrentPartner]};

                expect(
                    !shePrefersThisMan,
                    "Matching is unstable: man " + std::to_string(man) +
                        " and woman " + std::to_string(preferredWoman) +
                        " form a blocking pair");
            }
        }
    }

    void expectMatching(
        const std::vector<std::size_t> &actual,
        const std::vector<std::size_t> &expected,
        const std::vector<std::vector<std::size_t>> &malePref,
        const std::vector<std::vector<std::size_t>> &femalePrefs)
    {
        expectCompleteMatching(actual, malePref.size());
        expectStable(actual, malePref, femalePrefs);

        expect(
            actual == expected,
            "Expected " + formatMatching(expected) +
                ", but received " + formatMatching(actual));
    }

    class TestSuite
    {
    public:
        explicit TestSuite(std::string name)
            : name_{std::move(name)}
        {
        }

        template <typename TestFunction>
        void run(const std::string &testName, TestFunction &&testFunction)
        {
            ++total_;

            try
            {
                const std::string details{testFunction()};
                ++passed_;

                std::cout
                    << "  [PASS] " << testName << '\n'
                    << "         " << details << '\n';
            }
            catch (const std::exception &exception)
            {
                std::cout
                    << "  [FAIL] " << testName << '\n'
                    << "         " << exception.what() << '\n';
            }
            catch (...)
            {
                std::cout
                    << "  [FAIL] " << testName << '\n'
                    << "         Unknown exception\n";
            }
        }

        void printSummary() const
        {
            std::cout
                << "\n============================================\n"
                << " Final Result: " << passed_ << '/' << total_
                << " test cases passed\n"
                << "============================================\n"
                << (passed_ == total_ ? "ALL TESTS PASSED\n"
                                      : "SOME TESTS FAILED\n");
        }

        [[nodiscard]] bool allPassed() const
        {
            return passed_ == total_;
        }

    private:
        std::string name_;
        std::size_t passed_{0};
        std::size_t total_{0};
    };

} // namespace

int main()
{
    std::cout
        << "============================================\n"
        << " Gale-Shapley Correctness Test Suite\n"
        << " engagements[woman] = man\n"
        << "============================================\n";

    TestSuite suite{"Gale-Shapley"};

    suite.run("one man and one woman", []
              {
        std::vector<std::vector<std::size_t>> malePref{{0}};
        std::vector<std::vector<std::size_t>> femalePrefs{{0}};
        const std::vector<std::size_t> expected{0};

        // GaleShapley converts femalePrefs into rank tables in place.
        const auto originalFemalePrefs = femalePrefs;
        const auto engagements{GaleShapley(malePref, femalePrefs)};
        expectMatching(engagements, expected, malePref, originalFemalePrefs);
        return formatMatching(engagements); });

    suite.run("rejections produce the expected stable matching", []
              {
        std::vector<std::vector<std::size_t>> malePref{
            {0, 1, 2},
            {0, 1, 2},
            {1, 0, 2}
        };

        std::vector<std::vector<std::size_t>> femalePrefs{
            {1, 0, 2},
            {0, 2, 1},
            {0, 1, 2}
        };

        // woman 0 -> man 1, woman 1 -> man 0, woman 2 -> man 2
        const std::vector<std::size_t> expected{1, 0, 2};

        // GaleShapley converts femalePrefs into rank tables in place.
        const auto originalFemalePrefs = femalePrefs;
        const auto engagements{GaleShapley(malePref, femalePrefs)};
        expectMatching(engagements, expected, malePref, originalFemalePrefs);
        return formatMatching(engagements); });

    suite.run("men receive the proposer-optimal stable matching", []
              {
        std::vector<std::vector<std::size_t>> malePref{
            {0, 1},
            {1, 0}
        };

        std::vector<std::vector<std::size_t>> femalePrefs{
            {1, 0},
            {0, 1}
        };

        // Both {0, 1} and {1, 0} are stable when indexed by woman.
        // Because the men propose, each man receives his first choice.
        const std::vector<std::size_t> expected{0, 1};

        // GaleShapley converts femalePrefs into rank tables in place.
        const auto originalFemalePrefs = femalePrefs;
        const auto engagements{GaleShapley(malePref, femalePrefs)};
        expectMatching(engagements, expected, malePref, originalFemalePrefs);
        return formatMatching(engagements); });

    suite.run("a chain of displaced engagements is handled correctly", []
              {
        std::vector<std::vector<std::size_t>> malePref{
            {0, 1, 2, 3},
            {0, 1, 2, 3},
            {1, 0, 2, 3},
            {1, 2, 0, 3}
        };

        std::vector<std::vector<std::size_t>> femalePrefs{
            {2, 1, 0, 3},
            {1, 3, 0, 2},
            {0, 3, 2, 1},
            {0, 1, 2, 3}
        };

        const std::vector<std::size_t> expected{2, 1, 0, 3};

        // GaleShapley converts femalePrefs into rank tables in place.
        const auto originalFemalePrefs = femalePrefs;
        const auto engagements{GaleShapley(malePref, femalePrefs)};
        expectMatching(engagements, expected, malePref, originalFemalePrefs);
        return formatMatching(engagements); });

    suite.printSummary();
    return suite.allPassed() ? 0 : 1;
}
