namespace Task3_Proxy;

// ─── Subject Interface ───────────────────────────────────────────────────────

public interface IMovie
{
    string Title       { get; }
    string Description { get; }       // важкий
    string[] Cast      { get; }       // важкий
    double  Rating     { get; }
    bool    IsPremium  { get; }
}

// ─── Real Subject (важкий об'єкт) ────────────────────────────────────────────

public sealed class Movie : IMovie
{
    public string Title       { get; }
    public string Description { get; }
    public string[] Cast      { get; }
    public double  Rating     { get; }
    public bool    IsPremium  { get; }

    public Movie(string title, string description, string[] cast, double rating, bool isPremium)
    {
        Console.WriteLine($"     [MovieDB] *** ЗАВАНТАЖЕННЯ деталей фільму '{title}' з бази ***");
        Thread.Sleep(20); // імітація важкого запиту
        Title       = title;
        Description = description;
        Cast        = cast;
        Rating      = rating;
        IsPremium   = isPremium;
    }
}

// ─── Proxy: lazy + caching + access control ──────────────────────────────────

public sealed class MovieProxy : IMovie
{
    private readonly string _title;
    private readonly bool   _isPremiumCatalog;
    private readonly bool   _userHasPremium;
    private Movie? _cached;

    public MovieProxy(string title, bool isPremiumCatalog, bool userHasPremium)
    {
        _title             = title;
        _isPremiumCatalog  = isPremiumCatalog;
        _userHasPremium    = userHasPremium;
    }

    private Movie Load()
    {
        // Контроль доступу — перевіряємо ДО завантаження
        if (_isPremiumCatalog && !_userHasPremium)
            throw new UnauthorizedAccessException(
                $"Фільм '{_title}' доступний лише користувачам з преміум-підпискою");

        // Lazy + cache
        if (_cached is null)
        {
            _cached = new Movie(
                title:       _title,
                description: $"Опис фільму '{_title}' із 1500 символів...",
                cast:        ["Актор А", "Актор Б", "Актор В"],
                rating:      8.4,
                isPremium:   _isPremiumCatalog);
        }
        else
        {
            Console.WriteLine($"     [Cache] Деталі '{_title}' взято з кеша");
        }
        return _cached;
    }

    // Легкі дані доступні без завантаження
    public string Title => _title;

    // Важкі дані тригерять завантаження через Load()
    public string   Description => Load().Description;
    public string[] Cast        => Load().Cast;
    public double   Rating      => Load().Rating;
    public bool     IsPremium   => _isPremiumCatalog;
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class ProxyDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 3 — Proxy: Lazy-завантаження фільмів       │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Каталог: 5 фільмів, частина — преміум
        var catalog = new List<MovieProxy>
        {
            new("Дюна: Частина третя", isPremiumCatalog: false, userHasPremium: false),
            new("Inception",          isPremiumCatalog: false, userHasPremium: false),
            new("Шеренга свободи",    isPremiumCatalog: true,  userHasPremium: false),
            new("Інтерстеллар",       isPremiumCatalog: false, userHasPremium: false),
            new("Преміум-Прем'єра",   isPremiumCatalog: true,  userHasPremium: false),
        };

        // 1) Перегляд лише списку (Title) — завантаження НЕ відбувається
        Console.WriteLine("  >> Користувач переглядає каталог (тільки назви):");
        foreach (var m in catalog)
            Console.WriteLine($"     • {m.Title} {(m.IsPremium ? "[PREMIUM]" : "")}");
        Console.WriteLine();

        // 2) Відкриття деталей одного фільму — спрацьовує lazy load
        Console.WriteLine("  >> Користувач відкриває 'Inception':");
        Console.WriteLine($"     Опис: {catalog[1].Description}");
        Console.WriteLine($"     Каст: {string.Join(", ", catalog[1].Cast)}");
        Console.WriteLine();

        // 3) Повторне звернення — спрацьовує кеш
        Console.WriteLine("  >> Користувач повертається до 'Inception':");
        Console.WriteLine($"     Рейтинг: {catalog[1].Rating}");
        Console.WriteLine();

        // 4) Преміум-контент без підписки — Proxy блокує
        Console.WriteLine("  >> Користувач без підписки відкриває преміум-фільм:");
        try
        {
            _ = catalog[2].Description;
        }
        catch (UnauthorizedAccessException ex)
        {
            Console.WriteLine($"     [BLOCKED] {ex.Message}");
        }
        Console.WriteLine();
    }
}
