#include <iostream>
#include <string>
using namespace std;

template <typename T>
class Input {
private:
    string prompt;
    T min_value;
    T max_value;
    
public:
    Input(string prompt_msg, T min_val, T max_val) : 
        prompt(prompt_msg), min_value(min_val), max_value(max_val) {}
    
    T getValue() {
        T input_value;
        
        while(true) {
            cout << prompt;
            cin >> input_value;
            
            if(input_value >= min_value && input_value <= max_value) {
                return input_value;
            } else {
                cout << "Значення має бути в діапазоні від " << min_value 
                     << " до " << max_value << ". Спробуйте ще раз.\n";
            }
        }
    }
};

int main() {
    Input<int> intInput("Введіть ціле число (1-100) > ", 1, 100);
    int int_val = intInput.getValue();
    cout << "Ви ввели: " << int_val << endl;
    
    Input<double> doubleInput("Введіть дійсне число (0.0-10.0) > ", 0.0, 10.0);
    double double_val = doubleInput.getValue();
    cout << "Ви ввели: " << double_val << endl;
    
    Input<char> charInput("Введіть символ (a-z) > ", 'a', 'z');
    char char_val = charInput.getValue();
    cout << "Ви ввели: " << char_val << endl;
    
    return 0;
}