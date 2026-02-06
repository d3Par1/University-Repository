#include <iostream>

// Клас planet
class planet
{
  int moons;
  double dist_from_sun; // у милях
  double diameter;
  double mass;

public:
  // Конструктор з параметрами
  planet(int m, double dist, double diam, double ms) : moons(m), dist_from_sun(dist), diameter(diam), mass(ms) {}

  // Отримання відстані від Сонця у милях
  double get_miles() { return dist_from_sun; }

  // Метод для відображення інформації про планету
  void show()
  {
    std::cout << "Відстань від Сонця (милі): " << dist_from_sun << std::endl;
    std::cout << "Діаметр (милі): " << diameter << std::endl;
    std::cout << "Маса: " << mass << std::endl;
    std::cout << "Кількість супутників: " << moons << std::endl;
  }

  // Оголошуємо функцію light() як дружню
  friend double light(const planet &p);
};

// Функція для обчислення часу, за який світло доходить до планети
double light(const planet &p)
{
  // Швидкість світла (миль за секунду)
  const double SPEED_OF_LIGHT = 186000.0;

  // Обчислення часу в секундах
  return p.dist_from_sun / SPEED_OF_LIGHT;
}

int main()
{
  // Створюємо об'єкти планет з приблизними даними
  // Параметри: (кількість супутників, відстань від Сонця у милях, діаметр, маса)
  planet mercury(0, 36000000, 3032, 0.055);
  planet venus(0, 67200000, 7521, 0.815);
  planet earth(1, 93000000, 7926, 1.0);
  planet mars(2, 141600000, 4222, 0.107);
  planet jupiter(79, 483600000, 88846, 318.0);
  planet saturn(82, 886700000, 74900, 95.0);

  // Виводимо інформацію про планети та час проходження світла
  std::cout << "МЕРКУРІЙ:" << std::endl;
  mercury.show();
  std::cout << "Час проходження світла до планети: "
            << light(mercury) << " секунд ("
            << light(mercury) / 60 << " хвилин)" << std::endl;

  std::cout << "\nВЕНЕРА:" << std::endl;
  venus.show();
  std::cout << "Час проходження світла до планети: "
            << light(venus) << " секунд ("
            << light(venus) / 60 << " хвилин)" << std::endl;

  std::cout << "\nЗЕМЛЯ:" << std::endl;
  earth.show();
  std::cout << "Час проходження світла до планети: "
            << light(earth) << " секунд ("
            << light(earth) / 60 << " хвилин)" << std::endl;

  std::cout << "\nМАРС:" << std::endl;
  mars.show();
  std::cout << "Час проходження світла до планети: "
            << light(mars) << " секунд ("
            << light(mars) / 60 << " хвилин)" << std::endl;

  std::cout << "\nЮПІТЕР:" << std::endl;
  jupiter.show();
  std::cout << "Час проходження світла до планети: "
            << light(jupiter) << " секунд ("
            << light(jupiter) / 60 << " хвилин)" << std::endl;

  std::cout << "\nСАТУРН:" << std::endl;
  saturn.show();
  std::cout << "Час проходження світла до планети: "
            << light(saturn) << " секунд ("
            << light(saturn) / 60 << " хвилин) ("
            << light(saturn) / 3600 << " годин)" << std::endl;

  return 0;
}