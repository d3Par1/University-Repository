#include <iostream>
using namespace std;

template<int SIZE>
class arraytype {
    int array[SIZE];
    
public:
    arraytype() {
        // Ініціалізуємо масив нулями
        for(int i = 0; i < SIZE; i++) {
            array[i] = 0;
        }
    }
    
    // Перевантаження оператора [] з контролем кордонів
    int& operator[](int i) {
        if(i < 0 || i >= SIZE) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << SIZE-1 << "]\n";
            cout << "Програма буде завершена...\n";
            exit(1);  // Генеруємо помилку виконання
        }
        return array[i];
    }
    
    // Константна версія оператора []
    const int& operator[](int i) const {
        if(i < 0 || i >= SIZE) {
            cout << "Помилка: індекс " << i << " виходить за межі масиву [0.." << SIZE-1 << "]\n";
            cout << "Програма буде завершена...\n";
            exit(1);
        }
        return array[i];
    }
    
    // Допоміжні методи
    int size() const { return SIZE; }
    
    void fill_with_value(int value) {
        for(int i = 0; i < SIZE; i++) {
            array[i] = value;
        }
    }
    
    void fill_with_sequence(int start = 1) {
        for(int i = 0; i < SIZE; i++) {
            array[i] = start + i;
        }
    }
    
    void print() const {
        cout << "Масив[" << SIZE << "]: ";
        for(int i = 0; i < SIZE; i++) {
            cout << array[i];
            if(i < SIZE - 1) cout << ", ";
        }
        cout << "\n";
    }
    
    // Метод для безпечного доступу без завершення програми
    bool safe_set(int index, int value) {
        if(index < 0 || index >= SIZE) {
            cout << "Попередження: індекс " << index << " виходить за межі\n";
            return false;
        }
        array[index] = value;
        return true;
    }
    
    bool safe_get(int index, int &value) const {
        if(index < 0 || index >= SIZE) {
            cout << "Попередження: індекс " << index << " виходить за межі\n";
            return false;
        }
        value = array[index];
        return true;
    }
};

// Альтернативна реалізація з винятками (більш сучасний підхід)
template<int SIZE>
class safe_array {
    int array[SIZE];
    
public:
    class index_error {
        int bad_index;
        int array_size;
    public:
        index_error(int idx, int size) : bad_index(idx), array_size(size) {}
        
        void show_error() const {
            cout << "Виняток: індекс " << bad_index 
                 << " виходить за межі масиву [0.." << array_size-1 << "]\n";
        }
        
        int get_index() const { return bad_index; }
        int get_size() const { return array_size; }
    };
    
    safe_array() {
        for(int i = 0; i < SIZE; i++) {
            array[i] = 0;
        }
    }
    
    int& operator[](int i) {
        if(i < 0 || i >= SIZE) {
            throw index_error(i, SIZE);
        }
        return array[i];
    }
    
    const int& operator[](int i) const {
        if(i < 0 || i >= SIZE) {
            throw index_error(i, SIZE);
        }
        return array[i];
    }
    
    int size() const { return SIZE; }
    
    void print() const {
        cout << "Safe_array[" << SIZE << "]: ";
        for(int i = 0; i < SIZE; i++) {
            cout << array[i];
            if(i < SIZE - 1) cout << ", ";
        }
        cout << "\n";
    }
};

int main() {
    cout << "ДЕМОНСТРАЦІЯ БЕЗПЕЧНОГО МАСИВУ З ОПЕРАТОРОМ []:\n";
    cout << "===============================================\n\n";
    
    // Створення безпечного масиву
    cout << "1. СТВОРЕННЯ ТА ІНІЦІАЛІЗАЦІЯ:\n";
    cout << "==============================\n";
    
    arraytype<5> safe_arr;
    cout << "Створено безпечний масив розміром " << safe_arr.size() << "\n";
    safe_arr.print();
    
    // Заповнення масиву
    cout << "\n2. ЗАПОВНЕННЯ МАСИВУ:\n";
    cout << "====================\n";
    
    cout << "Заповнення через оператор []:\n";
    for(int i = 0; i < safe_arr.size(); i++) {
        safe_arr[i] = (i + 1) * 10;
    }
    safe_arr.print();
    
    // Читання з масиву
    cout << "\n3. ЧИТАННЯ З МАСИВУ:\n";
    cout << "===================\n";
    
    cout << "Читання через оператор []:\n";
    for(int i = 0; i < safe_arr.size(); i++) {
        cout << "safe_arr[" << i << "] = " << safe_arr[i] << "\n";
    }
    
    // Використання в лівій частині присвоєння
    cout << "\n4. ВИКОРИСТАННЯ В ЛІВІЙ ЧАСТИНІ ПРИСВОЄННЯ:\n";
    cout << "==========================================\n";
    
    cout << "Змінюємо safe_arr[2] з " << safe_arr[2] << " на 999:\n";
    safe_arr[2] = 999;
    safe_arr.print();
    
    cout << "Збільшуємо safe_arr[1] на 5:\n";
    safe_arr[1] += 5;
    safe_arr.print();
    
    // Демонстрація контролю кордонів
    cout << "\n5. ДЕМОНСТРАЦІЯ КОНТРОЛЮ КОРДОНІВ:\n";
    cout << "=================================\n";
    
    cout << "Спроба доступу до коректних індексів:\n";
    cout << "safe_arr[0] = " << safe_arr[0] << "\n";
    cout << "safe_arr[4] = " << safe_arr[4] << "\n";
    
    cout << "\nСпроба доступу до некоректних індексів:\n";
    cout << "УВАГА: Наступні операції призведуть до завершення програми!\n";
    
    // Безпечні альтернативи
    cout << "\nВикористання безпечних методів:\n";
    int value;
    if(safe_arr.safe_get(10, value)) {
        cout << "safe_arr[10] = " << value << "\n";
    } else {
        cout << "Не вдалося отримати safe_arr[10]\n";
    }
    
    if(safe_arr.safe_set(10, 777)) {
        cout << "Встановлено safe_arr[10] = 777\n";
    } else {
        cout << "Не вдалося встановити safe_arr[10]\n";
    }
    
    // Демонстрація версії з винятками
    cout << "\n6. ДЕМОНСТРАЦІЯ ВЕРСІЇ З ВИНЯТКАМИ:\n";
    cout << "==================================\n";
    
    safe_array<3> arr_with_exceptions;
    
    cout << "Заповнення масиву з винятками:\n";
    try {
        for(int i = 0; i < 3; i++) {
            arr_with_exceptions[i] = i * 100;
        }
        arr_with_exceptions.print();
        
        cout << "\nСпроба доступу до некоректного індексу:\n";
        arr_with_exceptions[5] = 500;  // Це призведе до винятку
        
    } catch(const safe_array<3>::index_error &e) {
        e.show_error();
        cout << "Виняток оброблено, програма продовжує роботу\n";
    }
    
    cout << "\nМасив після винятку:\n";
    arr_with_exceptions.print();
    
    // Практичний приклад використання
    cout << "\n7. ПРАКТИЧНИЙ ПРИКЛАД:\n";
    cout << "=====================\n";
    
    arraytype<10> scores;
    
    cout << "Введення балів студентів (симуляція):\n";
    int student_scores[] = {85, 92, 78, 96, 89, 73, 88, 91, 84, 87};
    
    for(int i = 0; i < 10; i++) {
        scores[i] = student_scores[i];
    }
    
    scores.print();
    
    // Пошук максимального та мінімального балу
    int max_score = scores[0];
    int min_score = scores[0];
    int max_index = 0, min_index = 0;
    
    for(int i = 1; i < scores.size(); i++) {
        if(scores[i] > max_score) {
            max_score = scores[i];
            max_index = i;
        }
        if(scores[i] < min_score) {
            min_score = scores[i];
            min_index = i;
        }
    }
    
    cout << "Максимальний бал: " << max_score << " (студент " << max_index << ")\n";
    cout << "Мінімальний бал: " << min_score << " (студент " << min_index << ")\n";
    
    // Підрахунок середнього балу
    int sum = 0;
    for(int i = 0; i < scores.size(); i++) {
        sum += scores[i];
    }
    double average = double(sum) / scores.size();
    cout << "Середній бал: " << average << "\n";
    
    cout << "\n8. ПЕРЕВАГИ ТА НЕДОЛІКИ БЕЗПЕЧНОГО МАСИВУ:\n";
    cout << "=========================================\n";
    
    cout << "ПЕРЕВАГИ:\n";
    cout << "+ Автоматичний контроль кордонів\n";
    cout << "+ Запобігання сегментаційним помилкам\n";
    cout << "+ Зручний синтаксис (як звичайний масив)\n";
    cout << "+ Можна використовувати в лівій частині присвоєння\n";
    cout << "+ Підтримка константних об'єктів\n";
    
    cout << "\nНЕДОЛІКИ:\n";
    cout << "- Додаткові витрати на перевірку індексів\n";
    cout << "- Може бути повільнішим за звичайні масиви\n";
    cout << "- У версії з exit() програма аварійно завершується\n";
    cout << "- Потребує обережності при використанні\n";
    
    cout << "\nРЕКОМЕНДАЦІЇ:\n";
    cout << "- Використовувати в режимі налагодження\n";
    cout << "- Враховувати вплив на продуктивність\n";
    cout << "- Розглянути використання винятків замість exit()\n";
    cout << "- Комбінувати з безпечними методами доступу\n";
    
    // Демонстрація, що призведе до помилки (закоментовано для безпеки)
    /*
    cout << "\nСпроба доступу до неіснуючого елемента:\n";
    cout << "safe_arr[100] - це призведе до завершення програми!\n";
    int bad_access = safe_arr[100];  // ПОМИЛКА!
    */
    
    return 0;
}