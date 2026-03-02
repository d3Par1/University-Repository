namespace CreationalPatterns.Patterns;

// ─── Enums ───────────────────────────────────────────────────────────────────

public enum Gender { Чоловік, Жінка }
public enum CharacterClass { Воїн, Маг }
public enum Weapon { Меч, Кинджал, Лук }

// ─── Product ─────────────────────────────────────────────────────────────────

public sealed class Character
{
    public string Name        { get; init; } = string.Empty;
    public Gender Gender      { get; init; }
    public CharacterClass Class { get; init; }
    public int Strength       { get; init; }
    public int Intelligence   { get; init; }
    public int Agility        { get; init; }
    public string EyeColor    { get; init; } = string.Empty;
    public string Hairstyle   { get; init; } = string.Empty;
    public Weapon Weapon      { get; init; }

    public override string ToString() =>
        $"""
           Ім'я:          {Name}
           Стать:         {Gender}
           Клас:          {Class}
           Сила:          {Strength}
           Інтелект:      {Intelligence}
           Спритність:    {Agility}
           Колір очей:    {EyeColor}
           Зачіска:       {Hairstyle}
           Зброя:         {Weapon}
         """;
}

// ─── Builder ─────────────────────────────────────────────────────────────────

public sealed class CharacterBuilder
{
    private string _name          = "Безіменний";
    private Gender _gender        = Gender.Чоловік;
    private CharacterClass _class = CharacterClass.Воїн;
    private int _strength         = 10;
    private int _intelligence     = 10;
    private int _agility          = 10;
    private string _eyeColor      = "Карий";
    private string _hairstyle     = "Короткий";
    private Weapon _weapon        = Weapon.Меч;

    public CharacterBuilder SetName(string name)               { _name = name; return this; }
    public CharacterBuilder SetGender(Gender gender)           { _gender = gender; return this; }
    public CharacterBuilder SetClass(CharacterClass cls)       { _class = cls; return this; }
    public CharacterBuilder SetStats(int str, int intel, int agi)
    {
        _strength     = str;
        _intelligence = intel;
        _agility      = agi;
        return this;
    }
    public CharacterBuilder SetEyeColor(string color)         { _eyeColor = color; return this; }
    public CharacterBuilder SetHairstyle(string hairstyle)    { _hairstyle = hairstyle; return this; }
    public CharacterBuilder SetWeapon(Weapon weapon)          { _weapon = weapon; return this; }

    public Character Build() => new()
    {
        Name         = _name,
        Gender       = _gender,
        Class        = _class,
        Strength     = _strength,
        Intelligence = _intelligence,
        Agility      = _agility,
        EyeColor     = _eyeColor,
        Hairstyle    = _hairstyle,
        Weapon       = _weapon,
    };
}

// ─── Director ────────────────────────────────────────────────────────────────

public sealed class CharacterDirector
{
    private readonly CharacterBuilder _builder;

    public CharacterDirector(CharacterBuilder builder) => _builder = builder;

    public Character CreateDefaultWarrior() =>
        _builder
            .SetName("Ґалад")
            .SetGender(Gender.Чоловік)
            .SetClass(CharacterClass.Воїн)
            .SetStats(str: 20, intel: 8, agi: 14)
            .SetEyeColor("Сірий")
            .SetHairstyle("Довгий, з косою")
            .SetWeapon(Weapon.Меч)
            .Build();

    public Character CreateDefaultMage() =>
        _builder
            .SetName("Сільвана")
            .SetGender(Gender.Жінка)
            .SetClass(CharacterClass.Маг)
            .SetStats(str: 6, intel: 24, agi: 12)
            .SetEyeColor("Фіолетовий")
            .SetHairstyle("Довгий, срібний")
            .SetWeapon(Weapon.Кинджал)
            .Build();
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class BuilderDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 1 — Патерн Builder: Творець персонажа гри  │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var builder  = new CharacterBuilder();
        var director = new CharacterDirector(builder);

        // Default warrior
        var warrior = director.CreateDefaultWarrior();
        Console.WriteLine("  >> Воїн за замовчуванням:");
        Console.WriteLine(warrior);
        Console.WriteLine();

        // Default mage
        var mage = director.CreateDefaultMage();
        Console.WriteLine("  >> Маг за замовчуванням:");
        Console.WriteLine(mage);
        Console.WriteLine();

        // Fully custom character
        var custom = new CharacterBuilder()
            .SetName("Орест")
            .SetGender(Gender.Чоловік)
            .SetClass(CharacterClass.Воїн)
            .SetStats(str: 17, intel: 11, agi: 18)
            .SetEyeColor("Зелений")
            .SetHairstyle("Стрижений, з татуюванням")
            .SetWeapon(Weapon.Лук)
            .Build();

        Console.WriteLine("  >> Повністю кастомний персонаж:");
        Console.WriteLine(custom);
        Console.WriteLine();
    }
}
