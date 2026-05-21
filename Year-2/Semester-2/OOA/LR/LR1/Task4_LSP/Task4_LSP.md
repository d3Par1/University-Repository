# ЛР1 — Завдання 4. Порушення принципу підстановки Лісков (LSP)

Принцип підстановки Лісков (Liskov Substitution Principle) стверджує, що об'єкти підкласу повинні бути замінними на об'єкти базового класу без порушення коректності програми.

## Вихідний код із порушенням

```csharp
public class Rectangle
{
    public virtual double Height { get; set; }
    public virtual double Width { get; set; }
    public double Area
    { get { return this.Height * this.Width; } }
}

class Square : Rectangle
{
    public override double Height
    { set => base.Width = base.Height = value; }

    public override double Width
    { set => base.Width = base.Height = value; }
}

public class Execution
{
    public Execution()
    {
        var r = new Rectangle()
            { Height = 10, Width = 5 };
        GetRectArea(r);   // 10 * 5 = 50 ✓

        var s = new Square()
        {
            Height = 10,  // стає 10
            Width = 5     // обидва стають 5
        };
        GetRectArea(s);   // 5 * 5 = 25 (очікувалось 50)
    }

    public double GetRectArea(Rectangle rect)
    {
        Debug.WriteLine($"{rect.Height} * {rect.Width}");
        Debug.WriteLine($"{rect.Area}");
        return rect.Area;
    }
}
```

## Аналіз порушення

`Square` перевизначає сетери `Height` та `Width` так, що обидва встановлюють однакове значення. Коли `Square` використовується замість `Rectangle`, поведінка стає непередбачуваною: встановлення `Height=10`, а потім `Width=5` призводить до площі 25 замість очікуваних 50. Це порушує контракт базового класу `Rectangle`.

## Виправлений код

```csharp
// Спільний інтерфейс для фігур
public interface IShape
{
    double Area { get; }
}

// Прямокутник — незалежні сторони
public class Rectangle : IShape
{
    public double Height { get; set; }
    public double Width { get; set; }
    public double Area => Height * Width;
}

// Квадрат — одна сторона
public class Square : IShape
{
    public double Side { get; set; }
    public double Area => Side * Side;
}

public class Execution
{
    public Execution()
    {
        var r = new Rectangle
            { Height = 10, Width = 5 };
        PrintArea(r);  // 50

        var s = new Square { Side = 5 };
        PrintArea(s);  // 25
    }

    public double PrintArea(IShape shape)
    {
        Debug.WriteLine($"Area = {shape.Area}");
        return shape.Area;
    }
}
```

## Висновок

Тепер `Rectangle` і `Square` — це окремі реалізації інтерфейсу `IShape`. `Square` більше не успадковує `Rectangle`, тому не може порушити його контракт. Кожна фігура коректно обчислює свою площу.
