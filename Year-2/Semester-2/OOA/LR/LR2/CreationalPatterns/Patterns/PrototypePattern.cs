namespace CreationalPatterns.Patterns;

// ─── Generic Clone Interface ──────────────────────────────────────────────────

public interface ICloneable<T>
{
    T Clone();
}

// ─── Shared (Flyweight) Resources ────────────────────────────────────────────

public sealed class OrcTexture
{
    public string Name   { get; init; } = string.Empty;
    public string Color  { get; init; } = string.Empty;

    public override string ToString() => $"{Name} ({Color})";
}

public sealed class OrcEffects
{
    public string[] ActiveEffects { get; init; } = [];

    public override string ToString() => string.Join(", ", ActiveEffects);
}

// ─── Orc Weapon Enum ─────────────────────────────────────────────────────────

public enum OrcWeapon { Сокира, Молот, Спис, Кулак, Булава }

// ─── Prototype ───────────────────────────────────────────────────────────────

public sealed class OrcPrototype : ICloneable<OrcPrototype>
{
    // --- Спільні ресурси (flyweight — не копіюються, а передаються за посиланням)
    public OrcTexture Texture     { get; set; } = null!;
    public OrcEffects Effects     { get; set; } = null!;
    public string     MovementType { get; set; } = string.Empty;

    // --- Індивідуальні властивості (копіюються при клонуванні)
    public (int X, int Y) Position { get; set; }
    public OrcWeapon Weapon        { get; set; }
    public int Level               { get; set; }
    public int Strength            { get; set; }

    /// <summary>
    /// Глибоке копіювання: індивідуальні дані копіюються,
    /// але Texture та Effects — лише посилання (flyweight).
    /// </summary>
    public OrcPrototype Clone() => new()
    {
        // Спільні — той самий об'єкт
        Texture      = Texture,
        Effects      = Effects,
        MovementType = MovementType,

        // Індивідуальні — нові значення (value types копіюються автоматично)
        Position = Position,
        Weapon   = Weapon,
        Level    = Level,
        Strength = Strength,
    };

    public override string ToString() =>
        $"  Позиція: ({Position.X,3},{Position.Y,3}) | " +
        $"Зброя: {Weapon,-8} | " +
        $"Рівень: {Level,2} | " +
        $"Сила: {Strength,3} | " +
        $"Текстура: {Texture} | " +
        $"Ефекти: [{Effects}]";
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class PrototypeDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 4 — Prototype: Клонування орків            │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Спільні ресурси (один екземпляр на всіх орків — flyweight)
        var sharedTexture = new OrcTexture { Name = "OrcSkin_v3", Color = "Зелений" };
        var sharedEffects = new OrcEffects { ActiveEffects = ["Лють", "Кровопролиття"] };

        // Шаблонний орк
        var template = new OrcPrototype
        {
            Texture      = sharedTexture,
            Effects      = sharedEffects,
            MovementType = "Наземний",
            Position     = (0, 0),
            Weapon       = OrcWeapon.Сокира,
            Level        = 1,
            Strength     = 50,
        };

        Console.WriteLine("  >> Шаблонний орк:");
        Console.WriteLine(template);
        Console.WriteLine();

        // Дані для 5 клонів
        (int X, int Y)[] positions = [(10, 5), (20, 15), (35, 2), (50, 40), (5, 60)];
        OrcWeapon[]      weapons   = [OrcWeapon.Молот, OrcWeapon.Спис, OrcWeapon.Булава, OrcWeapon.Кулак, OrcWeapon.Сокира];
        int[]            levels    = [3, 5, 7, 4, 6];
        int[]            strengths = [60, 75, 90, 65, 80];

        Console.WriteLine("  >> 5 клонованих орків:");
        for (int i = 0; i < 5; i++)
        {
            var orc = template.Clone();
            orc.Position = positions[i];
            orc.Weapon   = weapons[i];
            orc.Level    = levels[i];
            orc.Strength = strengths[i];

            Console.WriteLine($"  Орк #{i + 1}:");
            Console.WriteLine(orc);
        }

        Console.WriteLine();

        // Перевірка flyweight: всі орки мають той самий об'єкт Texture
        var orc1 = template.Clone();
        var orc2 = template.Clone();
        bool sharedRef = ReferenceEquals(orc1.Texture, orc2.Texture);
        Console.WriteLine($"  >> Texture у клонів — один об'єкт (flyweight): {sharedRef}");
        Console.WriteLine();
    }
}
