namespace Task2_Facade;

// ─── Subsystems (складна внутрішня логіка) ───────────────────────────────────

public sealed class PricingService
{
    public decimal CalculateTotal(string[] products)
    {
        var basePrice = products.Length * 12500m;
        Console.WriteLine($"  [Pricing]   Базова вартість {products.Length} товарів = {basePrice} грн");
        return basePrice;
    }
}

public sealed class DiscountService
{
    public decimal ApplyDiscount(decimal total, string promoCode)
    {
        var discount = promoCode == "SUMMER10" ? total * 0.10m : 0m;
        Console.WriteLine($"  [Discount]  Промокод '{promoCode}' → знижка {discount} грн");
        return total - discount;
    }
}

public sealed class InventoryService
{
    public bool CheckAvailability(string[] products)
    {
        Console.WriteLine($"  [Inventory] Перевірка наявності {products.Length} товарів на складі: OK");
        return true;
    }

    public void Reserve(string[] products)
    {
        Console.WriteLine($"  [Inventory] Заброньовано {products.Length} товарів");
    }
}

public sealed class PaymentGateway
{
    public string Charge(string card, decimal amount)
    {
        var txId = $"TX-{Random.Shared.Next(100000, 999999)}";
        Console.WriteLine($"  [Payment]   Списано {amount} грн з картки ****{card[^4..]} → транзакція {txId}");
        return txId;
    }
}

public sealed class ShippingService
{
    public string Schedule(string address)
    {
        var trackingId = $"SHIP-{Random.Shared.Next(10000, 99999)}";
        Console.WriteLine($"  [Shipping]  Доставку заплановано на адресу '{address}' → трек-номер {trackingId}");
        return trackingId;
    }
}

public sealed class NotificationService
{
    public void NotifyCustomer(string email, string txId, string trackingId)
    {
        Console.WriteLine($"  [Notify]    Лист до {email}: підтвердження транзакції {txId} + трек-номер {trackingId}");
    }
}

// ─── Facade ──────────────────────────────────────────────────────────────────

public sealed record OrderRequest(
    string[] Products,
    string   PromoCode,
    string   CardNumber,
    string   ShippingAddress,
    string   CustomerEmail);

public sealed record OrderResult(string TransactionId, string TrackingId, decimal FinalAmount);

public sealed class OrderFacade
{
    private readonly PricingService     _pricing      = new();
    private readonly DiscountService    _discount     = new();
    private readonly InventoryService   _inventory    = new();
    private readonly PaymentGateway     _payment      = new();
    private readonly ShippingService    _shipping     = new();
    private readonly NotificationService _notification = new();

    public OrderResult PlaceOrder(OrderRequest req)
    {
        Console.WriteLine("  --- Початок оформлення замовлення ---");

        if (!_inventory.CheckAvailability(req.Products))
            throw new InvalidOperationException("Деяких товарів немає на складі");

        var subtotal = _pricing.CalculateTotal(req.Products);
        var total    = _discount.ApplyDiscount(subtotal, req.PromoCode);

        _inventory.Reserve(req.Products);
        var txId      = _payment.Charge(req.CardNumber, total);
        var trackId   = _shipping.Schedule(req.ShippingAddress);
        _notification.NotifyCustomer(req.CustomerEmail, txId, trackId);

        Console.WriteLine("  --- Замовлення оформлено ---");
        return new OrderResult(txId, trackId, total);
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class FacadeDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 2 — Facade: Оформлення замовлення         │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Клієнт (веб-контролер) робить ОДИН виклик замість координації 6 сервісів
        var facade = new OrderFacade();
        var order  = new OrderRequest(
            Products:        ["Ноутбук Lenovo", "Миша Logitech", "Чохол"],
            PromoCode:       "SUMMER10",
            CardNumber:      "4149 4393 1234 5678",
            ShippingAddress: "м. Київ, вул. Перемоги, 37",
            CustomerEmail:   "client@kpi.ua");

        var result = facade.PlaceOrder(order);

        Console.WriteLine();
        Console.WriteLine($"  >> Підсумок: транзакція={result.TransactionId}, трек={result.TrackingId}, сума={result.FinalAmount} грн");
        Console.WriteLine();
    }
}
