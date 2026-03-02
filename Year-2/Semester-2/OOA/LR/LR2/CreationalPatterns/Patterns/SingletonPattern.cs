namespace CreationalPatterns.Patterns;

// ─── Singleton: Сервіс логування ─────────────────────────────────────────────

public sealed class LogService
{
    private static LogService? _instance;
    private static readonly object _lock = new();

    // Приватний конструктор — ніхто не може створити екземпляр ззовні
    private LogService() { }

    public static LogService Instance
    {
        get
        {
            if (_instance is null)
            {
                lock (_lock)
                {
                    _instance ??= new LogService();
                }
            }
            return _instance;
        }
    }

    public void Log(string module, string message)
    {
        var timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
        Console.WriteLine($"  [{timestamp}] [{module,-10}] {message}");
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class SingletonDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 3 — Singleton: Сервіс логування           │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        Console.WriteLine("  >> Три модулі використовують один і той самий LogService:");
        Console.WriteLine();

        // Модуль автентифікації
        var authLogger = LogService.Instance;
        authLogger.Log("Auth",     "Користувач admin намагається увійти");
        authLogger.Log("Auth",     "Перевірка паролю...");
        authLogger.Log("Auth",     "Вхід успішний");

        // Модуль бази даних
        var dbLogger = LogService.Instance;
        dbLogger.Log("Database",   "Підключення до бази даних встановлено");
        dbLogger.Log("Database",   "Запит SELECT виконано, отримано 42 рядки");

        // Модуль платежів
        var payLogger = LogService.Instance;
        payLogger.Log("Payment",   "Ініціалізація платіжного шлюзу");
        payLogger.Log("Payment",   "Транзакція #TXN-00847 підтверджена");
        payLogger.Log("Payment",   "Квитанцію надіслано на email");

        Console.WriteLine();

        // Перевірка що всі три посилання — один і той самий об'єкт
        bool allSame = ReferenceEquals(authLogger, dbLogger)
                    && ReferenceEquals(dbLogger,   payLogger);

        Console.WriteLine($"  >> Перевірка ReferenceEquals (Auth == DB == Payment): {allSame}");
        Console.WriteLine($"     authLogger  HashCode: {authLogger.GetHashCode()}");
        Console.WriteLine($"     dbLogger    HashCode: {dbLogger.GetHashCode()}");
        Console.WriteLine($"     payLogger   HashCode: {payLogger.GetHashCode()}");
        Console.WriteLine();
    }
}
