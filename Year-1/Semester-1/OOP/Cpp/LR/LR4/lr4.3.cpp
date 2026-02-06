#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <regex>
#include <algorithm>
#include <cctype>
#include <tuple>
#include <sstream>

// Forward declaration
void test_markov_chain();

// Only define VectorHash once
struct VectorHash
{
    template <typename T>
    std::size_t operator()(const std::vector<T> &v) const
    {
        std::size_t seed = v.size();
        for (const auto &i : v)
        {
            seed ^= std::hash<T>()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class MarkovChain
{
private:
    int order;
    bool use_delimiters;
    std::unordered_map<std::vector<std::string>, std::vector<std::string>, VectorHash> state_transitions;
    std::vector<std::vector<std::string>> starting_states;
    std::vector<std::vector<std::string>> ending_states;
    const std::string NONWORD = "\n";

    std::mt19937 rng;

    bool is_end_of_sentence(const std::string &word)
    {
        return (word == "." || word == "!" || word == "?");
    }

    bool ends_with_punctuation(const std::string &word)
    {
        if (word.empty())
            return false;
        char last_char = word.back();
        return (last_char == '.' || last_char == '!' || last_char == '?');
    }

    std::string format_text(const std::vector<std::string> &words)
    {
        if (words.empty())
            return "";

        std::string text;
        bool new_sentence = true;

        for (size_t i = 0; i < words.size(); ++i)
        {
            std::string word = words[i];

            if (new_sentence && !word.empty() && std::isalpha(word[0]))
            {
                word[0] = std::toupper(word[0]);
                new_sentence = false;
            }

            if (i > 0 && !is_end_of_sentence(word) && word != "," && word != ";" && word != ":")
            {
                text += " ";
            }
            text += word;

            if (is_end_of_sentence(word))
            {
                new_sentence = true;
            }
        }

        return text;
    }

public:
    MarkovChain(int chain_order = 2, bool use_delims = false) : order(chain_order),
                                                                use_delimiters(use_delims),
                                                                rng(std::random_device{}())
    {
    }

    void build_from_text(const std::string &text)
    {
        // Очищення та токенізація тексту
        std::vector<std::string> words;
        std::regex word_regex("\\w+|[.,!?;:]");

        auto words_begin = std::sregex_iterator(text.begin(), text.end(), word_regex);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
        {
            std::string word = i->str();
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            words.push_back(word);
        }

        if (words.empty())
        {
            return;
        }

        if (use_delimiters)
        {
            for (int i = 0; i < order; ++i)
            {
                words.insert(words.begin(), NONWORD);
                words.push_back(NONWORD);
            }
        }

        for (size_t i = 0; i <= words.size() - order - 1; ++i)
        {
            std::vector<std::string> state(words.begin() + i, words.begin() + i + order);
            std::string next_word = words[i + order];

            state_transitions[state].push_back(next_word);

            if (!use_delimiters && i == 0)
            {
                starting_states.push_back(state);
            }
            else if (!use_delimiters && i > 0 && ends_with_punctuation(words[i - 1]))
            {
                starting_states.push_back(state);
            }

            if (!use_delimiters && is_end_of_sentence(next_word))
            {
                ending_states.push_back(state);
            }
        }

        if (!use_delimiters && starting_states.empty())
        {
            for (const auto &pair : state_transitions)
            {
                starting_states.push_back(pair.first);
            }
        }

        if (!use_delimiters && ending_states.empty())
        {
            for (const auto &pair : state_transitions)
            {
                ending_states.push_back(pair.first);
            }
        }
    }

    std::string generate(int max_words = 100)
    {
        if (state_transitions.empty())
        {
            return "Недостатньо даних для генерації тексту.";
        }

        std::vector<std::string> result;
        std::vector<std::string> state;

        if (use_delimiters)
        {
            state = std::vector<std::string>(order, NONWORD);
        }
        else
        {
            if (!starting_states.empty())
            {
                std::uniform_int_distribution<size_t> dist(0, starting_states.size() - 1);
                state = starting_states[dist(rng)];
            }
            else
            {
                auto it = state_transitions.begin();
                std::advance(it, std::uniform_int_distribution<size_t>(0, state_transitions.size() - 1)(rng));
                state = it->first;
            }
        }

        result.insert(result.end(), state.begin(), state.end());

        size_t word_count = state.size();

        while (word_count < max_words)
        {
            if (state_transitions.count(state))
            {
                const auto &possible_next = state_transitions[state];
                std::uniform_int_distribution<size_t> dist(0, possible_next.size() - 1);
                std::string next_word = possible_next[dist(rng)];

                if (use_delimiters && next_word == NONWORD)
                {
                    break;
                }

                result.push_back(next_word);
                word_count++;

                state.erase(state.begin());
                state.push_back(next_word);

                if (!use_delimiters)
                {
                    bool is_ending_state = std::find(ending_states.begin(), ending_states.end(), state) != ending_states.end();
                    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

                    if (is_ending_state && prob_dist(rng) < 0.2)
                    {
                        if (!is_end_of_sentence(result.back()))
                        {
                            result.push_back(".");
                        }
                        break;
                    }
                }
            }
            else
            {
                if (!result.empty() && !is_end_of_sentence(result.back()))
                {
                    result.push_back(".");
                }
                break;
            }
        }

        return format_text(result);
    }

    void print_stats()
    {
        std::cout << "Кількість станів: " << state_transitions.size() << std::endl;
        std::cout << "Початкові стани: " << starting_states.size() << std::endl;
        std::cout << "Кінцеві стани: " << ending_states.size() << std::endl;
    }
};

// Define test_markov_chain function before it's used in main
void test_markov_chain()
{
    std::cout << "Порівняння Марківських ланцюгів з та без роздільників" << std::endl;

    std::vector<std::string> test_inputs = {
        "",
        "word",
        "two words",
        "one two three",
        "first second third fourth",
        "This is a test sentence. This is another sentence. This sentence has words from previous ones.",
    };

    for (size_t i = 0; i < test_inputs.size(); ++i)
    {
        const auto &test_input = test_inputs[i];
        std::cout << "\nТест " << (i + 1) << ": " << test_input.substr(0, 30)
                  << (test_input.length() > 30 ? "..." : "") << std::endl;
        std::cout << "Довжина вхідного тексту: "
                  << (test_input.empty() ? 0 : std::count(test_input.begin(), test_input.end(), ' ') + 1)
                  << " слів" << std::endl;

        MarkovChain with_delimiters(2, true);
        with_delimiters.build_from_text(test_input);

        MarkovChain without_delimiters(2, false);
        without_delimiters.build_from_text(test_input);

        std::string with_text = with_delimiters.generate(20);
        std::string without_text = without_delimiters.generate(20);

        std::cout << "\nВихідний текст з роздільниками:" << std::endl;
        std::cout << with_text << std::endl;

        std::cout << "\nВихідний текст без роздільників:" << std::endl;
        std::cout << without_text << std::endl;

        std::cout << "\nСтатистика для версії з роздільниками:" << std::endl;
        with_delimiters.print_stats();

        std::cout << "\nСтатистика для версії без роздільників:" << std::endl;
        without_delimiters.print_stats();
    }
}

int main(int argc, char *argv[])
{
    std::setlocale(LC_ALL, "");

    test_markov_chain();

    if (argc > 1)
    {
        std::ifstream file(argv[1]);
        if (!file.is_open())
        {
            std::cerr << "Помилка відкриття файлу: " << argv[1] << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string text = buffer.str();

        // Complete the main function by creating and using a MarkovChain
        MarkovChain chain(2, false);
        chain.build_from_text(text);

        std::cout << "\nСтатистика для тексту з файлу:" << std::endl;
        chain.print_stats();

        std::string generated = chain.generate(100);
        std::cout << "\nЗгенерований текст:" << std::endl;
        std::cout << generated << std::endl;
    }

    return 0;
}
