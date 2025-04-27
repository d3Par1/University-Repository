#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <regex>
#include <algorithm>
#include <sstream>
#include <tuple>

int main(int argc, char* argv[]) {
  std::setlocale(LC_ALL, "");
  
  if (argc < 2) {
      std::cout << "Використання: " << argv[0] << " <шлях_до_файлу>" << std::endl;
      
      std::cout << "\nДемонстраційний режим з вбудованим прикладом..." << std::endl;
      std::string sample_text = 
          "As I started up the undershirt onto his chest black, and big stomach muscles "
          "bulging under the light. \"You see them?\" Below the line where his ribs stopped were "
          "two raised white welts. \"See on the forehead.\" \"Oh, Brett, I love you.\" \"Let's not talk. "
          "Talking's all bilge. I'm going away tomorrow.\" \"Tomorrow?\" \"Yes. Didn't I say so? I "
          "am.\" \"Let's have a drink, then.\"";
      
      for (int order : {1, 2, 3}) {
          if (order == 1) {
              std::cout << "\nДля ланцюга порядку 1 потрібна окрема реалізація. Пропускаємо." << std::endl;
              continue;
          }
          
          HemingwayMarkovGenerator generator(order);
          generator.build_model(sample_text);
          
          std::cout << "\nГенерація тексту з порядком ланцюга " << order << ":" << std::endl;
          std::string generated_text = generator.generate(50);
          std::cout << generated_text << std::endl;
          
          generator.print_stats();
      }
      
      return 0;
  }
  
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
      std::cerr << "Помилка відкриття файлу: " << argv[1] << std::endl;
      return 1;
  }
  
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string text = buffer.str();
  
  for (int order : {2, 3}) {
      HemingwayMarkovGenerator generator(order);
      generator.build_model(text);
      
      std::cout << "\nГенерація тексту з порядком ланцюга " << order << ":" << std::endl;
      std::string generated_text = generator.generate(100);
      std::cout << generated_text << std::endl;
      
      generator.print_stats();
  }
  
  return 0;
}

class HemingwayMarkovGenerator {
private:
    struct PrefixHash {
        template <typename... T>
        std::size_t operator()(const std::tuple<T...>& t) const {
            return std::apply([](const auto&... args) {
                std::size_t seed = 0;
                (..., (seed ^= std::hash<std::decay_t<decltype(args)>>()(args) + 0x9e3779b9 + (seed << 6) + (seed >> 2)));
                return seed;
            }, t);
        }
    };
    
    using Prefix2 = std::tuple<std::string, std::string>;
    using Prefix3 = std::tuple<std::string, std::string, std::string>;
    
    std::unordered_map<Prefix2, std::vector<std::string>, PrefixHash> prefix2_table;
    std::unordered_map<Prefix3, std::vector<std::string>, PrefixHash> prefix3_table;
    
    int order;
    const std::string NONWORD = "\n";
    

    std::mt19937 rng;
    
    // Допоміжні функції для роботи з текстом
    bool is_punctuation(const std::string& word) {
        return (word == "." || word == "," || word == "!" || 
                word == "?" || word == ";" || word == ":" || word == "\"");
    }
    

    std::string format_text(const std::vector<std::string>& words) {
        if (words.empty()) return "";
        
        std::string text;
        bool quote_open = false;
        bool new_sentence = true;
        
        for (size_t i = 0; i < words.size(); ++i) {
            std::string word = words[i];
            
            if (word == "\"") {
                if (!quote_open) {

                    if (!text.empty() && !std::isspace(text.back())) {
                        text += " ";
                    }
                    text += word;
                    quote_open = true;
                } else {

                    text += word;
                    quote_open = false;
                }
            } else if (is_punctuation(word)) {

                text += word;
                

                if (word == "." || word == "!" || word == "?") {
                    new_sentence = true;
                }
            } else {

                if (!text.empty() && !std::isspace(text.back()) && text.back() != '\"') {
                    text += " ";
                }
                

                if (new_sentence && !word.empty() && std::isalpha(word[0])) {
                    word[0] = std::toupper(word[0]);
                    new_sentence = false;
                }
                
                text += word;
            }
        }
        

        if (!text.empty() && !is_punctuation(std::string(1, text.back()))) {
            text += ".";
        }
        
        return text;
    }
    
public:
    HemingwayMarkovGenerator(int chain_order = 2) : 
        order(chain_order),
        rng(std::random_device{}()) {
    }

    void build_model(const std::string& text) {

        std::string processed_text = text;
        processed_text = std::regex_replace(processed_text, std::regex("\\s+"), " ");

        std::vector<std::string> words;
        std::regex word_regex("\\w+|[.,!?;:\"]");
        
        auto words_begin = std::sregex_iterator(processed_text.begin(), processed_text.end(), word_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::string word = i->str();
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            words.push_back(word);
        }
        
        if (words.size() <= order) {
            std::cout << "Попередження: текст занадто короткий для ланцюга порядку " << order << "." << std::endl;
            return;
        }

        for (int i = 0; i < order; ++i) {
            words.insert(words.begin(), NONWORD);
            words.push_back(NONWORD);
        }

        if (order == 2) {
            for (size_t i = 0; i <= words.size() - order - 1; ++i) {
                Prefix2 prefix = std::make_tuple(words[i], words[i+1]);
                std::string suffix = words[i+2];
                prefix2_table[prefix].push_back(suffix);
            }
        } else if (order == 3) {
            for (size_t i = 0; i <= words.size() - order - 1; ++i) {
                Prefix3 prefix = std::make_tuple(words[i], words[i+1], words[i+2]);
                std::string suffix = words[i+3];
                prefix3_table[prefix].push_back(suffix);
            }
        } else {
            std::cout << "Підтримуються тільки ланцюги порядку 2 і 3." << std::endl;
        }
    }
    

    std::string generate(int num_words = 100) {
        if ((order == 2 && prefix2_table.empty()) || 
            (order == 3 && prefix3_table.empty())) {
            return "Модель не побудована. Спочатку викличте build_model().";
        }
        
        std::vector<std::string> result;
        

        if (order == 2) {
            Prefix2 state = std::make_tuple(NONWORD, NONWORD);
            result.push_back(std::get<1>(state));

            for (int i = 0; i < num_words; ++i) {
                if (prefix2_table.count(state) == 0) {
                    break;
                }
                
                const auto& suffixes = prefix2_table[state];
                std::uniform_int_distribution<size_t> dist(0, suffixes.size() - 1);
                std::string next = suffixes[dist(rng)];
                
                if (next == NONWORD) {
                    break;
                }
                
                result.push_back(next);
                state = std::make_tuple(std::get<1>(state), next);
            }
        } else if (order == 3) {
            Prefix3 state = std::make_tuple(NONWORD, NONWORD, NONWORD);
            result.push_back(std::get<1>(state));
            result.push_back(std::get<2>(state));

            for (int i = 0; i < num_words; ++i) {
                if (prefix3_table.count(state) == 0) {
                    break;
                }
                
                const auto& suffixes = prefix3_table[state];
                std::uniform_int_distribution<size_t> dist(0, suffixes.size() - 1);
                std::string next = suffixes[dist(rng)];
                
                if (next == NONWORD) {
                    break;
                }
                
                result.push_back(next);
                state = std::make_tuple(std::get<1>(state), std::get<2>(state), next);
            }
        }

        return format_text(result);
    }

    void print_stats() {
        if (order == 2) {
            std::cout << "Ланцюг порядку 2: " << prefix2_table.size() << " станів" << std::endl;
        } else if (order == 3) {
            std::cout << "Ланцюг порядку 3: " << prefix3_table.size() << " станів" << std::endl;
        }
    }
};

