namespace StructuralPatterns.Patterns;

// ─── Component (спільний інтерфейс для листя і композитів) ───────────────────

public abstract class CrmModule
{
    public string Name { get; }
    public bool   Enabled { get; private set; } = true;

    protected CrmModule(string name) => Name = name;

    public abstract decimal GetTotalPrice();
    public abstract void    Display(int indent = 0);

    public virtual void Enable()  { Enabled = true;  }
    public virtual void Disable() { Enabled = false; }

    protected string Prefix(int indent) => new(' ', indent * 2);
    protected string StatusTag()        => Enabled ? "" : " (вимкнено)";
}

// ─── Leaf (окремий модуль) ───────────────────────────────────────────────────

public sealed class LeafModule : CrmModule
{
    private readonly decimal _price;

    public LeafModule(string name, decimal price) : base(name) => _price = price;

    public override decimal GetTotalPrice() => Enabled ? _price : 0m;

    public override void Display(int indent = 0) =>
        Console.WriteLine($"  {Prefix(indent)}• {Name,-28} {_price,8} грн{StatusTag()}");
}

// ─── Composite (група модулів) ───────────────────────────────────────────────

public sealed class CompositeModule : CrmModule
{
    private readonly List<CrmModule> _children = [];

    public CompositeModule(string name) : base(name) { }

    public CompositeModule Add(CrmModule module)
    {
        _children.Add(module);
        return this;
    }

    public override decimal GetTotalPrice() =>
        Enabled ? _children.Sum(c => c.GetTotalPrice()) : 0m;

    public override void Display(int indent = 0)
    {
        Console.WriteLine($"  {Prefix(indent)}+ {Name}{StatusTag()}  [сума: {GetTotalPrice()} грн]");
        foreach (var c in _children)
            c.Display(indent + 1);
    }

    // Каскадне вмикання/вимикання
    public override void Disable()
    {
        base.Disable();
        foreach (var c in _children) c.Disable();
    }

    public override void Enable()
    {
        base.Enable();
        foreach (var c in _children) c.Enable();
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class CompositeDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 4 — Composite: CRM-модулі                 │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Побудова ієрархії
        var analytics = new CompositeModule("Модуль аналітики")
            .Add(new LeafModule("Дашборди",              5000))
            .Add(new LeafModule("Звіти PDF/Excel",       3500))
            .Add(new LeafModule("Прогнози ML",          12000));

        var integrations = new CompositeModule("Інтеграції")
            .Add(new LeafModule("Інтеграція з Slack",    2500))
            .Add(new LeafModule("Інтеграція з Telegram", 2500))
            .Add(new LeafModule("Інтеграція з 1С",       8000));

        var extensions = new CompositeModule("Розширення")
            .Add(new LeafModule("Кастомний брендинг",    4000))
            .Add(new LeafModule("Білі мітки",            6000));

        var crm = new CompositeModule("CRM-система")
            .Add(new LeafModule("Базове ядро",          20000))
            .Add(analytics)
            .Add(integrations)
            .Add(extensions);

        // Клієнтський код працює однаково з модулем та групою
        crm.Display();
        Console.WriteLine();
        Console.WriteLine($"  >> Загальна вартість ліцензії: {crm.GetTotalPrice()} грн");
        Console.WriteLine();

        // Вимикаємо цілу гілку — каскадно вимикає її дітей
        Console.WriteLine("  >> Користувач вимикає всі 'Розширення':");
        extensions.Disable();
        crm.Display();
        Console.WriteLine();
        Console.WriteLine($"  >> Нова сума ліцензії: {crm.GetTotalPrice()} грн");
        Console.WriteLine();
    }
}
