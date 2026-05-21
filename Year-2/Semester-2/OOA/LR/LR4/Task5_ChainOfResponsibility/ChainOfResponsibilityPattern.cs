namespace Task5_ChainOfResponsibility;

// ─── Handler ─────────────────────────────────────────────────────────────────

public abstract class SupportHandler
{
    private SupportHandler? _next;

    public SupportHandler SetNext(SupportHandler next)
    {
        _next = next;
        return next;
    }

    public void Handle(SupportRequest request)
    {
        if (CanHandle(request))
            Process(request);
        else if (_next is not null)
        {
            Console.WriteLine($"     ↳ {GetType().Name} не може обробити — передає далі");
            _next.Handle(request);
        }
        else
            Console.WriteLine($"     ✗ Ніхто не зміг обробити запит '{request.Topic}'");
    }

    protected abstract bool CanHandle(SupportRequest request);
    protected abstract void Process(SupportRequest request);
}

public sealed record SupportRequest(string User, string Topic, int ComplexityLevel);

// ─── Concrete Handlers ───────────────────────────────────────────────────────

public sealed class ChatBotHandler : SupportHandler
{
    private static readonly string[] KnownTopics = ["пароль", "вхід", "реєстрація", "ціна"];

    protected override bool CanHandle(SupportRequest r) =>
        r.ComplexityLevel <= 1 && KnownTopics.Any(t => r.Topic.Contains(t, StringComparison.OrdinalIgnoreCase));

    protected override void Process(SupportRequest r) =>
        Console.WriteLine($"     [🤖 Bot]      '{r.Topic}' — стандартна відповідь з бази знань для {r.User}");
}

public sealed class L1OperatorHandler : SupportHandler
{
    protected override bool CanHandle(SupportRequest r) => r.ComplexityLevel <= 3;

    protected override void Process(SupportRequest r) =>
        Console.WriteLine($"     [👤 L1]       Оператор взяв запит '{r.Topic}' від {r.User}");
}

public sealed class TechSpecialistHandler : SupportHandler
{
    protected override bool CanHandle(SupportRequest r) => r.ComplexityLevel <= 5;

    protected override void Process(SupportRequest r) =>
        Console.WriteLine($"     [🛠 Tech]     Технічний спеціаліст розбирає '{r.Topic}' від {r.User}");
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class ChainOfResponsibilityDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 5 — Chain of Responsibility: підтримка     │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var bot   = new ChatBotHandler();
        var l1    = new L1OperatorHandler();
        var tech  = new TechSpecialistHandler();
        bot.SetNext(l1).SetNext(tech);

        var requests = new[]
        {
            new SupportRequest("user_01", "Як змінити пароль?",                ComplexityLevel: 1),
            new SupportRequest("user_02", "Відмова банку при оплаті",          ComplexityLevel: 3),
            new SupportRequest("user_03", "TLS-handshake падає на проді",       ComplexityLevel: 5),
            new SupportRequest("user_04", "Невідомий складний баг",             ComplexityLevel: 9),
        };

        foreach (var req in requests)
        {
            Console.WriteLine($"  >> Запит від {req.User}: '{req.Topic}' (складність {req.ComplexityLevel})");
            bot.Handle(req);
            Console.WriteLine();
        }
    }
}
