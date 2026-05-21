# ЛР1 — Завдання 2. Порушення принципу відкритості/закритості (OCP)

Принцип відкритості/закритості (Open/Closed Principle) стверджує, що програмні сутності повинні бути відкритими для розширення, але закритими для модифікації.

## Вихідний код із порушенням

```csharp
public enum RentType { Daily, Weekly }

public class Car
{
    public string Maker { get; set; }
    public Color Color { get; set; }
}

public class CarRental
{
    public decimal Rent(Car car, decimal baseValue,
        int amount, RentType rentType)
    {
        if (rentType == RentType.Daily)
            return baseValue * amount;
        if (rentType == RentType.Weekly)
            return baseValue * (7 * amount);
        return 0;
    }
}
```

## Аналіз порушення

Метод `Rent()` використовує ланцюжок `if`-перевірок для визначення типу оренди. Якщо потрібно додати новий тип (наприклад, погодинну або місячну оренду), доведеться модифікувати існуючий метод, що порушує принцип OCP.

## Виправлений код

```csharp
public class Car
{
    public string Maker { get; set; }
    public Color Color { get; set; }
}

// Абстракція стратегії розрахунку оренди
public abstract class RentalStrategy
{
    public abstract decimal Calculate(decimal baseValue, int amount);
}

// Добова оренда
public class DailyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue * amount;
    }
}

// Тижнева оренда
public class WeeklyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue * 7 * amount;
    }
}

// Легко додати нові типи без зміни існуючого коду:
public class HourlyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue / 24 * amount;
    }
}

public class CarRental
{
    public decimal Rent(Car car, decimal baseValue,
        int amount, RentalStrategy strategy)
    {
        return strategy.Calculate(baseValue, amount);
    }
}
```

## Висновок

Тепер для додавання нового типу оренди достатньо створити новий клас-нащадок `RentalStrategy`, не змінюючи існуючий код `CarRental`. Класична реалізація патерну Strategy.
