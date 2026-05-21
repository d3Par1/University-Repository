namespace Task7_Flyweight;

// ─── Flyweight: спільні (intrinsic) дані ─────────────────────────────────────

public sealed class BlockType
{
    public string Name      { get; }     // "Дуб", "Сосна", "Граніт"
    public string Texture   { get; }     // важка текстура — спільна
    public string Model     { get; }     // важка 3D-модель — спільна
    public int    BaseDurability { get; }
    public bool   IsInteractable  { get; }

    public BlockType(string name, string texture, string model, int durability, bool interactable)
    {
        Name            = name;
        Texture         = texture;
        Model           = model;
        BaseDurability  = durability;
        IsInteractable  = interactable;
    }

    public override string ToString() =>
        $"{Name} (tex={Texture}, model={Model}, dur={BaseDurability})";
}

// ─── Flyweight Factory ───────────────────────────────────────────────────────

public static class BlockTypeFactory
{
    private static readonly Dictionary<string, BlockType> Pool = new();
    public  static int CreatedCount => Pool.Count;

    public static BlockType Get(string name)
    {
        if (!Pool.TryGetValue(name, out var bt))
        {
            bt = name switch
            {
                "Дуб"     => new BlockType("Дуб",     "oak_planks_v2.png",   "tree_oak.obj",    60,  true),
                "Сосна"   => new BlockType("Сосна",   "pine_bark.png",       "tree_pine.obj",   55,  true),
                "Граніт"  => new BlockType("Граніт",  "granite_diff.png",    "stone_block.obj", 200, false),
                "Залізо"  => new BlockType("Залізо",  "iron_ore.png",        "ore_block.obj",   150, true),
                "Алмаз"   => new BlockType("Алмаз",   "diamond_ore.png",     "ore_block.obj",   400, true),
                _         => throw new ArgumentException($"Невідомий тип блоку: {name}"),
            };
            Pool[name] = bt;
            Console.WriteLine($"     [Factory] *** СТВОРЕНО новий BlockType: '{name}' ***");
        }
        return bt;
    }
}

// ─── Context: унікальні (extrinsic) дані для кожного блоку у світі ───────────

public sealed class WorldBlock
{
    private readonly BlockType _type;             // посилання, не копія

    public (int X, int Y, int Z) Position { get; }
    public bool   IsHarvested { get; private set; }
    public int    CurrentDurability { get; private set; }

    public WorldBlock(BlockType type, (int X, int Y, int Z) position)
    {
        _type             = type;
        Position          = position;
        CurrentDurability = type.BaseDurability;
        IsHarvested       = false;
    }

    public void TakeDamage(int dmg)
    {
        CurrentDurability -= dmg;
        if (CurrentDurability <= 0 && _type.IsInteractable)
            IsHarvested = true;
    }

    public string TypeName => _type.Name;
    public override string ToString() =>
        $"{_type.Name,-7} ({Position.X,3},{Position.Y,3},{Position.Z,3}) dur={CurrentDurability,3}{(IsHarvested ? " ◷ HARVESTED" : "")}";
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class FlyweightDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 7 — Flyweight: Minecraft-світ              │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var rng       = new Random(42);
        var typeNames = new[] { "Дуб", "Сосна", "Граніт", "Залізо", "Алмаз" };
        var world     = new List<WorldBlock>();

        Console.WriteLine("  >> Заповнення світу 100 000 блоками...");
        for (int i = 0; i < 100_000; i++)
        {
            var name = typeNames[rng.Next(typeNames.Length)];
            var type = BlockTypeFactory.Get(name);   // <-- спільний об'єкт
            world.Add(new WorldBlock(type, (rng.Next(1000), rng.Next(256), rng.Next(1000))));
        }

        Console.WriteLine();
        Console.WriteLine($"  >> Створено блоків у світі:       {world.Count:N0}");
        Console.WriteLine($"  >> Унікальних BlockType у пулі:   {BlockTypeFactory.CreatedCount}");
        Console.WriteLine($"  >> Економія: замість 100 000 копій важких текстур — лише {BlockTypeFactory.CreatedCount}");
        Console.WriteLine();

        // Перші 5 блоків — для демонстрації
        Console.WriteLine("  >> Перші 5 блоків у світі:");
        for (int i = 0; i < 5; i++)
            Console.WriteLine($"     {world[i]}");
        Console.WriteLine();

        // Один блок отримує пошкодження — індивідуальний стан не впливає на інших
        Console.WriteLine("  >> Гравець видобуває перший блок:");
        world[0].TakeDamage(world[0].CurrentDurability);
        Console.WriteLine($"     {world[0]}");

        // Перевіряємо, що сусідній блок того самого типу не змінив стан
        var neighborSameType = world.First(b => b.TypeName == world[0].TypeName && !b.IsHarvested);
        Console.WriteLine($"     Сусідній блок того ж типу не змінився: {neighborSameType}");
        Console.WriteLine();
    }
}
