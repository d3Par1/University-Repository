namespace Task1_Observer;

// ─── Observer Interface ──────────────────────────────────────────────────────

public interface ISubscriber
{
    string Name { get; }
    void OnNewVideo(string channel, string videoTitle);
}

// ─── Subject ─────────────────────────────────────────────────────────────────

public sealed class YouTubeChannel
{
    public string Name { get; }
    private readonly List<ISubscriber> _subscribers = [];

    public YouTubeChannel(string name) => Name = name;

    public void Subscribe(ISubscriber sub)
    {
        if (_subscribers.Contains(sub)) return;
        _subscribers.Add(sub);
        Console.WriteLine($"     {sub.Name} підписався на '{Name}'");
    }

    public void Unsubscribe(ISubscriber sub)
    {
        if (_subscribers.Remove(sub))
            Console.WriteLine($"     {sub.Name} відписався від '{Name}'");
    }

    public void UploadVideo(string title)
    {
        Console.WriteLine();
        Console.WriteLine($"  >> '{Name}' завантажує нове відео: '{title}'");
        Console.WriteLine($"     Автоматичне сповіщення для {_subscribers.Count} підписників:");
        foreach (var sub in _subscribers)
            sub.OnNewVideo(Name, title);
    }
}

// ─── Concrete Observers ──────────────────────────────────────────────────────

public sealed class EmailSubscriber : ISubscriber
{
    public string Name { get; }
    public EmailSubscriber(string name) => Name = name;

    public void OnNewVideo(string channel, string videoTitle) =>
        Console.WriteLine($"       [EMAIL → {Name,-10}] '{channel}' опублікував '{videoTitle}'");
}

public sealed class PushSubscriber : ISubscriber
{
    public string Name { get; }
    public PushSubscriber(string name) => Name = name;

    public void OnNewVideo(string channel, string videoTitle) =>
        Console.WriteLine($"       [PUSH  → {Name,-10}] 🔔 {channel}: {videoTitle}");
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class ObserverDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 1 — Observer: YouTube-сповіщення           │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var channel = new YouTubeChannel("TechWithKPI");

        var alice = new EmailSubscriber("Аліса");
        var bob   = new PushSubscriber ("Богдан");
        var carol = new EmailSubscriber("Кароліна");

        Console.WriteLine("  >> Підписка користувачів:");
        channel.Subscribe(alice);
        channel.Subscribe(bob);
        channel.Subscribe(carol);

        channel.UploadVideo("Як працюють патерни проектування за 10 хвилин");

        Console.WriteLine();
        Console.WriteLine("  >> Богдан відписується:");
        channel.Unsubscribe(bob);

        channel.UploadVideo("SOLID на пальцях: SRP та OCP");
        Console.WriteLine();
    }
}
