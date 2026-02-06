#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }
    coord(int i, int j) { x = i; y = j; }
    
    void get_xy(int &i, int &j) { i = x; j = y; }
    void show() { cout << "(" << x << ", " << y << ")"; }
    
    // Перевантаження оператора << (зсув вліво)
    coord operator<<(int shift) {
        coord temp;
        temp.x = x << shift;  // Побітовий зсув вліво для x
        temp.y = y << shift;  // Побітовий зсув вліво для y
        return temp;
    }
    
    // Перевантаження оператора >> (зсув вправо)
    coord operator>>(int shift) {
        coord temp;
        temp.x = x >> shift;  // Побітовий зсув вправо для x
        temp.y = y >> shift;  // Побітовий зсув вправо для y
        return temp;
    }
    
    // Оператори присвоєння зі зсувом
    coord& operator<<=(int shift) {
        x <<= shift;
        y <<= shift;
        return *this;
    }
    
    coord& operator>>=(int shift) {
        x >>= shift;
        y >>= shift;
        return *this;
    }
    
    // Допоміжні оператори
    coord& operator=(const coord &obj) {
        x = obj.x;
        y = obj.y;
        return *this;
    }
    
    bool operator==(const coord &obj) {
        return (x == obj.x) && (y == obj.y);
    }
    
    // Додаткові методи для демонстрації
    void show_binary() {
        cout << "x(" << x << ") = ";
        print_binary(x);
        cout << ", y(" << y << ") = ";
        print_binary(y);
    }
    
private:
    void print_binary(int num) {
        if(num == 0) {
            cout << "0";
            return;
        }
        
        // Для від'ємних чисел показуємо знак
        if(num < 0) {
            cout << "-";
            num = -num;
        }
        
        // Знаходимо найвищий біт
        int mask = 1;
        while(mask <= num) {
            mask <<= 1;
        }
        mask >>= 1;
        
        // Виводимо біти
        while(mask > 0) {
            cout << ((num & mask) ? '1' : '0');
            mask >>= 1;
        }
    }
};

// Глобальні функції для демонстрації бітових операцій
void show_bit_shift_demo() {
    cout << "ДЕМОНСТРАЦІЯ БІТОВИХ ЗСУВІВ:\n";
    cout << "===========================\n";
    
    int values[] = {1, 2, 3, 4, 8, 16};
    int shifts[] = {1, 2, 3};
    
    for(int i = 0; i < 6; i++) {
        int val = values[i];
        cout << "\nЧисло " << val << ":\n";
        
        for(int j = 0; j < 3; j++) {
            int shift = shifts[j];
            cout << val << " << " << shift << " = " << (val << shift) 
                 << " (множення на " << (1 << shift) << ")\n";
            cout << val << " >> " << shift << " = " << (val >> shift) 
                 << " (ділення на " << (1 << shift) << ")\n";
        }
    }
}

int main() {
    cout << "ДЕМОНСТРАЦІЯ ОПЕРАТОРІВ ЗСУВУ ДЛЯ КЛАСУ coord:\n";
    cout << "==============================================\n\n";
    
    // Основна демонстрація
    show_bit_shift_demo();
    
    cout << "\n\nРОБОТА З КООРДИНАТАМИ:\n";
    cout << "=====================\n\n";
    
    // Створення координат
    cout << "1. СТВОРЕННЯ КООРДИНАТ:\n";
    cout << "======================\n";
    
    coord obj1(8, 16);    // 8 = 1000₂, 16 = 10000₂
    coord obj2(12, 20);   // 12 = 1100₂, 20 = 10100₂
    coord obj3(3, 7);     // 3 = 11₂, 7 = 111₂
    coord result;
    
    cout << "obj1: "; obj1.show(); cout << " -> "; obj1.show_binary(); cout << "\n";
    cout << "obj2: "; obj2.show(); cout << " -> "; obj2.show_binary(); cout << "\n";
    cout << "obj3: "; obj3.show(); cout << " -> "; obj3.show_binary(); cout << "\n";
    
    // Демонстрація зсуву вліво
    cout << "\n2. ОПЕРАТОР ЗСУВУ ВЛІВО (<<):\n";
    cout << "============================\n";
    
    cout << "obj1 << 1 (множення на 2):\n";
    result = obj1 << 1;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    cout << "\nobj1 << 2 (множення на 4):\n";
    result = obj1 << 2;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    cout << "\nobj3 << 3 (множення на 8):\n";
    result = obj3 << 3;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    // Демонстрація зсуву вправо
    cout << "\n3. ОПЕРАТОР ЗСУВУ ВПРАВО (>>):\n";
    cout << "=============================\n";
    
    cout << "obj2 >> 1 (ділення на 2):\n";
    result = obj2 >> 1;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    cout << "\nobj2 >> 2 (ділення на 4):\n";
    result = obj2 >> 2;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    cout << "\nobj1 >> 3 (ділення на 8):\n";
    result = obj1 >> 3;
    cout << "Результат: "; result.show(); cout << " -> "; result.show_binary(); cout << "\n";
    
    // Перевірка правильності зсувів
    cout << "\n4. ПЕРЕВІРКА ПРАВИЛЬНОСТІ ЗСУВІВ:\n";
    cout << "================================\n";
    
    coord test_coord(32, 64);
    cout << "Тестова координата: "; test_coord.show(); cout << "\n";
    
    for(int shift = 1; shift <= 4; shift++) {
        coord left_result = test_coord << shift;
        coord right_result = test_coord >> shift;
        
        cout << "\nЗсув на " << shift << " позицій:\n";
        cout << "test_coord << " << shift << " = "; left_result.show(); 
        cout << " (x: " << 32 << "*" << (1<<shift) << "=" << (32<<shift) 
             << ", y: " << 64 << "*" << (1<<shift) << "=" << (64<<shift) << ")\n";
        
        cout << "test_coord >> " << shift << " = "; right_result.show(); 
        cout << " (x: " << 32 << "/" << (1<<shift) << "=" << (32>>shift) 
             << ", y: " << 64 << "/" << (1<<shift) << "=" << (64>>shift) << ")\n";
    }
    
    // Оператори присвоєння
    cout << "\n5. ОПЕРАТОРИ ПРИСВОЄННЯ ЗІ ЗСУВОМ:\n";
    cout << "=================================\n";
    
    coord obj4(16, 32);
    cout << "obj4 до зміни: "; obj4.show(); cout << "\n";
    
    obj4 <<= 1;  // Зсув вліво з присвоєнням
    cout << "obj4 <<= 1: "; obj4.show(); cout << "\n";
    
    obj4 >>= 2;  // Зсув вправо з присвоєнням
    cout << "obj4 >>= 2: "; obj4.show(); cout << "\n";
    
    // Ланцюгові операції
    cout << "\n6. ЛАНЦЮГОВІ ОПЕРАЦІЇ:\n";
    cout << "=====================\n";
    
    coord chain_test(4, 8);
    cout << "Оригінальна координата: "; chain_test.show(); cout << "\n";
    
    result = (chain_test << 2) >> 1;  // Спочатку зсув вліво, потім вправо
    cout << "(chain_test << 2) >> 1 = "; result.show(); cout << "\n";
    cout << "Пояснення: (4*4)/2=8, (8*4)/2=16 -> (8, 16)\n";
    
    result = (chain_test >> 1) << 3;  // Спочатку зсув вправо, потім вліво
    cout << "(chain_test >> 1) << 3 = "; result.show(); cout << "\n";
    cout << "Пояснення: (4/2)*8=16, (8/2)*8=32 -> (16, 32)\n";
    
    // Практичні застосування
    cout << "\n7. ПРАКТИЧНІ ЗАСТОСУВАННЯ:\n";
    cout << "=========================\n";
    
    // Швидке множення/ділення на степені двійки
    coord price(100, 250);
    cout << "Початкова ціна: "; price.show(); cout << "\n";
    
    cout << "Подвоєння ціни (price << 1): ";
    (price << 1).show(); cout << "\n";
    
    cout << "Збільшення в 4 рази (price << 2): ";
    (price << 2).show(); cout << "\n";
    
    cout << "Знижка вдвічі (price >> 1): ";
    (price >> 1).show(); cout << "\n";
    
    cout << "Знижка в 4 рази (price >> 2): ";
    (price >> 2).show(); cout << "\n";
    
    // Розрахунок координат сітки
    cout << "\n8. РОЗРАХУНОК КООРДИНАТ СІТКИ:\n";
    cout << "=============================\n";
    
    coord grid_size(2, 3);  // Розмір клітинки 2x3
    cout << "Розмір клітинки сітки: "; grid_size.show(); cout << "\n";
    
    cout << "Координати кутів сітки (зсув на різні рівні):\n";
    for(int level = 0; level < 4; level++) {
        coord corner = grid_size << level;
        cout << "Рівень " << level << ": "; corner.show(); 
        cout << " (клітинка " << (1<<level) << "x" << (1<<level) << ")\n";
    }
    
    // Обробка зсувів з нулем та від'ємними числами
    cout << "\n9. ОСОБЛИВІ ВИПАДКИ:\n";
    cout << "===================\n";
    
    coord zero_coord(0, 0);
    cout << "Зсуви нульової координати:\n";
    cout << "zero_coord: "; zero_coord.show(); cout << "\n";
    cout << "zero_coord << 5: "; (zero_coord << 5).show(); cout << "\n";
    cout << "zero_coord >> 3: "; (zero_coord >> 3).show(); cout << "\n";
    
    coord negative_coord(-8, -16);
    cout << "\nЗсуви від'ємних координат:\n";
    cout << "negative_coord: "; negative_coord.show(); cout << "\n";
    cout << "negative_coord << 1: "; (negative_coord << 1).show(); cout << "\n";
    cout << "negative_coord >> 2: "; (negative_coord >> 2).show(); cout << "\n";
    
    // Порівняння з арифметичними операціями
    cout << "\n10. ПОРІВНЯННЯ З АРИФМЕТИКОЮ:\n";
    cout << "============================\n";
    
    coord compare_coord(24, 48);
    cout << "Тестова координата: "; compare_coord.show(); cout << "\n";
    
    cout << "\nПорівняння зсуву та множення:\n";
    coord shift_result = compare_coord << 2;
    coord mult_result(compare_coord);
    int x, y;
    mult_result.get_xy(x, y);
    mult_result = coord(x * 4, y * 4);
    
    cout << "compare_coord << 2 = "; shift_result.show(); cout << "\n";
    cout << "compare_coord * 4 = "; mult_result.show(); cout << "\n";
    cout << "Результати " << (shift_result == mult_result ? "однакові" : "різні") << "\n";
    
    cout << "\nПорівняння зсуву та ділення:\n";
    shift_result = compare_coord >> 3;
    mult_result.get_xy(x, y);
    mult_result = coord(x / 8, y / 8);
    
    cout << "compare_coord >> 3 = "; shift_result.show(); cout << "\n";
    cout << "compare_coord / 8 = "; mult_result.show(); cout << "\n";
    cout << "Результати " << (shift_result == mult_result ? "однакові" : "різні") << "\n";
    
    cout << "\n11. ПЕРЕВАГИ БІТОВИХ ЗСУВІВ:\n";
    cout << "============================\n";
    cout << "+ Швидкість: бітові операції виконуються найшвидше\n";
    cout << "+ Ефективність: менше ресурсів процесора\n";
    cout << "+ Простота: легко множити/ділити на степені двійки\n";
    cout << "+ Точність: немає проблем з округленням\n";
    
    cout << "\nОБМЕЖЕННЯ:\n";
    cout << "- Працює тільки зі степенями двійки (1, 2, 4, 8, 16, ...)\n";
    cout << "- Для від'ємних чисел поведінка може бути непередбачуваною\n";
    cout << "- Зсув вправо відкидає дробову частину\n";
    
    return 0;
}