namespace Task2_Strategy;

// ─── Strategy Interface ──────────────────────────────────────────────────────

public interface IPaymentStrategy
{
    void Pay(decimal amount);
}

// ─── Concrete Strategies ─────────────────────────────────────────────────────

public sealed class PayPalStrategy : IPaymentStrategy
{
    private readonly string _email;
    public PayPalStrategy(string email) => _email = email;

    public void Pay(decimal amount) =>
        Console.WriteLine($"     [PayPal]  Списано {amount} грн з акаунта {_email}");
}

public sealed class CreditCardStrategy : IPaymentStrategy
{
    private readonly string _cardNumber;
    private readonly string _holderName;
    public CreditCardStrategy(string cardNumber, string holderName)
    {
        _cardNumber = cardNumber;
        _holderName = holderName;
    }

    public void Pay(decimal amount) =>
        Console.WriteLine($"     [Card]    Списано {amount} грн з картки ****{_cardNumber[^4..]} ({_holderName})");
}

public sealed class CryptoStrategy : IPaymentStrategy
{
    private readonly string _wallet;
    private readonly string _ticker;
    public CryptoStrategy(string wallet, string ticker)
    {
        _wallet = wallet;
        _ticker = ticker;
    }

    public void Pay(decimal amount) =>
        Console.WriteLine($"     [Crypto]  Переказано еквівалент {amount} грн у {_ticker} з гаманця {_wallet[..6]}...");
}

// ─── Context ─────────────────────────────────────────────────────────────────

public sealed class ShoppingCart
{
    private readonly List<(string Name, decimal Price)> _items = [];

    public void Add(string name, decimal price)
    {
        _items.Add((name, price));
        Console.WriteLine($"     + {name,-20} {price,8} грн");
    }

    public decimal Total => _items.Sum(i => i.Price);

    public void Checkout(IPaymentStrategy strategy)
    {
        Console.WriteLine($"     Підсумок: {Total} грн");
        strategy.Pay(Total);                          // НЕМАЄ switch — лише поліморфний виклик
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class StrategyDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 2 — Strategy: Способи оплати кошика        │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        IPaymentStrategy[] strategies =
        [
            new PayPalStrategy    ("nazar@kpi.ua"),
            new CreditCardStrategy("4149 4393 1234 5678", "STEPANENKO N."),
            new CryptoStrategy    ("0xC4F3B4B3D1FFAA2B3E", "ETH"),
        ];

        string[] labels = ["PayPal", "Кредитна картка", "Криптовалюта"];

        for (int i = 0; i < strategies.Length; i++)
        {
            Console.WriteLine($"  >> Спосіб оплати: {labels[i]}");
            var cart = new ShoppingCart();
            cart.Add("Книга 'Чистий код'", 850);
            cart.Add("Мишка ігрова",       1450);
            cart.Checkout(strategies[i]);
            Console.WriteLine();
        }
    }
}
