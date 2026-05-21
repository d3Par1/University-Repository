namespace Task5_Decorator;

// ─── Component ───────────────────────────────────────────────────────────────

public interface IDocument
{
    string Process(string content);
    string Describe();
}

// ─── Concrete Component ──────────────────────────────────────────────────────

public sealed class PlainDocument : IDocument
{
    private readonly string _fileName;
    public PlainDocument(string fileName) => _fileName = fileName;

    public string Process(string content) => content;
    public string Describe() => $"Документ '{_fileName}'";
}

// ─── Base Decorator ──────────────────────────────────────────────────────────

public abstract class DocumentDecorator : IDocument
{
    protected readonly IDocument Inner;
    protected DocumentDecorator(IDocument inner) => Inner = inner;

    public abstract string Process(string content);
    public abstract string Describe();
}

// ─── Concrete Decorators ─────────────────────────────────────────────────────

public sealed class EncryptionDecorator : DocumentDecorator
{
    private readonly string _key;
    public EncryptionDecorator(IDocument inner, string key) : base(inner) => _key = key;

    public override string Process(string content)
    {
        var processed = Inner.Process(content);
        var encrypted = Convert.ToBase64String(System.Text.Encoding.UTF8.GetBytes(processed));
        Console.WriteLine($"     [Encrypt]   ключ='{_key}', результат={encrypted[..Math.Min(20, encrypted.Length)]}...");
        return encrypted;
    }

    public override string Describe() => $"{Inner.Describe()} + шифрування[{_key}]";
}

public sealed class CompressionDecorator : DocumentDecorator
{
    public CompressionDecorator(IDocument inner) : base(inner) { }

    public override string Process(string content)
    {
        var processed = Inner.Process(content);
        var compressed = $"<gzip>{processed.Length}b → {(int)(processed.Length * 0.4)}b</gzip>";
        Console.WriteLine($"     [Compress]  {compressed}");
        return processed; // концептуально стиснуто
    }

    public override string Describe() => $"{Inner.Describe()} + стиснення";
}

public sealed class WatermarkDecorator : DocumentDecorator
{
    private readonly string _watermark;
    public WatermarkDecorator(IDocument inner, string watermark) : base(inner) => _watermark = watermark;

    public override string Process(string content)
    {
        var processed = Inner.Process(content);
        Console.WriteLine($"     [Watermark] нанесено водяний знак: '{_watermark}'");
        return $"[{_watermark}] {processed}";
    }

    public override string Describe() => $"{Inner.Describe()} + водяний знак '{_watermark}'";
}

public sealed class AuditLogDecorator : DocumentDecorator
{
    private readonly string _user;
    public AuditLogDecorator(IDocument inner, string user) : base(inner) => _user = user;

    public override string Process(string content)
    {
        Console.WriteLine($"     [Audit]     {_user} → доступ до документа о {DateTime.Now:HH:mm:ss}");
        return Inner.Process(content);
    }

    public override string Describe() => $"{Inner.Describe()} + аудит-лог";
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class DecoratorDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 5 — Decorator: SaaS-документи             │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        const string content = "Конфіденційний договір між сторонами А та Б, версія 12";

        // Варіант 1: тільки водяний знак
        IDocument doc1 = new WatermarkDecorator(new PlainDocument("contract.docx"), "ВНУТРІШНЄ");
        Console.WriteLine($"  >> {doc1.Describe()}:");
        _ = doc1.Process(content);
        Console.WriteLine();

        // Варіант 2: аудит → стиснення → шифрування → водяний знак
        IDocument doc2 =
            new AuditLogDecorator(
                new EncryptionDecorator(
                    new CompressionDecorator(
                        new WatermarkDecorator(
                            new PlainDocument("contract.docx"),
                            "КОНФІДЕНЦІЙНО")),
                    key: "AES-256"),
                user: "stepanenko@kpi.ua");

        Console.WriteLine($"  >> {doc2.Describe()}:");
        _ = doc2.Process(content);
        Console.WriteLine();

        // Варіант 3: інший порядок — спочатку шифрування, потім стиснення
        IDocument doc3 =
            new CompressionDecorator(
                new EncryptionDecorator(
                    new PlainDocument("data.json"),
                    key: "ChaCha20"));

        Console.WriteLine($"  >> {doc3.Describe()}:");
        _ = doc3.Process(content);
        Console.WriteLine();
    }
}
