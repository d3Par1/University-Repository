namespace StructuralPatterns.Patterns;

// ─── Target Interface (очікуваний ядром системи) ─────────────────────────────

public interface IPaymentProvider
{
    decimal GetAmount();
    string  GetCurrency();
    string  GetStatus();
}

// ─── Adaptee #1: JSON-провайдер ──────────────────────────────────────────────

public sealed class JsonPaymentResponse
{
    public decimal amount_value  { get; init; }
    public string  currency_code { get; init; } = string.Empty;
    public string  state         { get; init; } = string.Empty;
}

public sealed class JsonPaymentService
{
    public JsonPaymentResponse FetchPayment(string txId) => new()
    {
        amount_value  = 1499.99m,
        currency_code = "USD",
        state         = "completed",
    };
}

// ─── Adaptee #2: XML-провайдер (синхронний, легасі) ──────────────────────────

public sealed class LegacyXmlPaymentProvider
{
    public string GetXmlPayload(string txId) =>
        "<payment><sum>2350</sum><cur>EUR</cur><result>SUCCESS</result></payment>";
}

// ─── Adaptee #3: Асинхронний провайдер з власними назвами ────────────────────

public sealed class AsyncCryptoProvider
{
    public async Task<(decimal price, string ticker, string ok)> RetrieveTransactionAsync(string txId)
    {
        await Task.Delay(15); // імітація мережевого виклику
        return (0.0421m, "BTC", "CONFIRMED");
    }
}

// ─── Adapters ────────────────────────────────────────────────────────────────

public sealed class JsonPaymentAdapter : IPaymentProvider
{
    private readonly JsonPaymentResponse _response;

    public JsonPaymentAdapter(JsonPaymentService svc, string txId)
    {
        _response = svc.FetchPayment(txId);
    }

    public decimal GetAmount()   => _response.amount_value;
    public string  GetCurrency() => _response.currency_code;
    public string  GetStatus()   => _response.state.ToUpperInvariant();
}

public sealed class XmlPaymentAdapter : IPaymentProvider
{
    private readonly decimal _amount;
    private readonly string  _currency;
    private readonly string  _status;

    public XmlPaymentAdapter(LegacyXmlPaymentProvider provider, string txId)
    {
        var xml = provider.GetXmlPayload(txId);
        _amount   = decimal.Parse(Between(xml, "<sum>",    "</sum>"),    System.Globalization.CultureInfo.InvariantCulture);
        _currency = Between(xml, "<cur>",    "</cur>");
        _status   = Between(xml, "<result>", "</result>");
    }

    public decimal GetAmount()   => _amount;
    public string  GetCurrency() => _currency;
    public string  GetStatus()   => _status;

    private static string Between(string source, string a, string b)
    {
        int i = source.IndexOf(a) + a.Length;
        int j = source.IndexOf(b, i);
        return source[i..j];
    }
}

public sealed class CryptoPaymentAdapter : IPaymentProvider
{
    private readonly decimal _amount;
    private readonly string  _currency;
    private readonly string  _status;

    // Адаптер ховає асинхронність — викликає .GetAwaiter().GetResult() у конструкторі
    public CryptoPaymentAdapter(AsyncCryptoProvider provider, string txId)
    {
        var (price, ticker, ok) = provider.RetrieveTransactionAsync(txId).GetAwaiter().GetResult();
        _amount   = price;
        _currency = ticker;
        _status   = ok;
    }

    public decimal GetAmount()   => _amount;
    public string  GetCurrency() => _currency;
    public string  GetStatus()   => _status;
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class AdapterDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 1 — Adapter: Уніфікація платіжних API     │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Ядро системи працює тільки з IPaymentProvider — про конкретних
        // провайдерів воно не знає нічого
        IPaymentProvider[] providers =
        [
            new JsonPaymentAdapter (new JsonPaymentService(),       "TX-001"),
            new XmlPaymentAdapter  (new LegacyXmlPaymentProvider(), "TX-002"),
            new CryptoPaymentAdapter(new AsyncCryptoProvider(),     "TX-003"),
        ];

        string[] names = ["JSON-провайдер", "XML-провайдер (legacy)", "Crypto-провайдер (async)"];

        for (int i = 0; i < providers.Length; i++)
        {
            var p = providers[i];
            Console.WriteLine($"  >> {names[i]}:");
            Console.WriteLine($"     Сума:    {p.GetAmount()}");
            Console.WriteLine($"     Валюта:  {p.GetCurrency()}");
            Console.WriteLine($"     Статус:  {p.GetStatus()}");
            Console.WriteLine();
        }
    }
}
