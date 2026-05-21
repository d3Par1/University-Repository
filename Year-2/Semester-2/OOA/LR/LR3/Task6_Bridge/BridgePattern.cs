namespace Task6_Bridge;

// ─── Implementor: спосіб відображення ────────────────────────────────────────

public interface IRenderer
{
    void Render(string dataLabel, IReadOnlyList<(string Key, double Value)> data);
}

// ─── Concrete Implementors ───────────────────────────────────────────────────

public sealed class WebDashboardRenderer : IRenderer
{
    public void Render(string dataLabel, IReadOnlyList<(string Key, double Value)> data)
    {
        Console.WriteLine($"  [Web]      Відображення '{dataLabel}' як інтерактивний дашборд:");
        foreach (var (k, v) in data)
        {
            int bar = (int)Math.Round(v / 5);
            Console.WriteLine($"    {k,-15} {new string('█', bar)} {v}");
        }
    }
}

public sealed class MobileAppRenderer : IRenderer
{
    public void Render(string dataLabel, IReadOnlyList<(string Key, double Value)> data)
    {
        Console.WriteLine($"  [Mobile]   Відображення '{dataLabel}' у мобільному додатку:");
        foreach (var (k, v) in data)
            Console.WriteLine($"    📱 {k}: {v}");
    }
}

public sealed class PdfExportRenderer : IRenderer
{
    public void Render(string dataLabel, IReadOnlyList<(string Key, double Value)> data)
    {
        Console.WriteLine($"  [PDF]      Експорт '{dataLabel}' у PDF-документ:");
        Console.WriteLine($"    ┌── Звіт: {dataLabel} ──┐");
        foreach (var (k, v) in data)
            Console.WriteLine($"    │ {k,-15} {v,8:F2} │");
        Console.WriteLine($"    └{new string('─', 28)}┘");
    }
}

// ─── Abstraction: тип даних ──────────────────────────────────────────────────

public abstract class AnalyticsView
{
    protected readonly IRenderer Renderer;
    protected AnalyticsView(IRenderer renderer) => Renderer = renderer;

    public abstract void Show();
}

// ─── Refined Abstractions ────────────────────────────────────────────────────

public sealed class FinancialMetricsView : AnalyticsView
{
    public FinancialMetricsView(IRenderer renderer) : base(renderer) { }

    public override void Show()
    {
        var data = new List<(string, double)>
        {
            ("Виручка",    245.5),
            ("Витрати",    132.8),
            ("Прибуток",   112.7),
            ("EBITDA",      87.3),
        };
        Renderer.Render("Фінансові показники Q1-2026 (млн грн)", data);
    }
}

public sealed class UserMetricsView : AnalyticsView
{
    public UserMetricsView(IRenderer renderer) : base(renderer) { }

    public override void Show()
    {
        var data = new List<(string, double)>
        {
            ("DAU",         42.1),
            ("MAU",        128.4),
            ("Retention",   71.2),
            ("Churn",        8.7),
        };
        Renderer.Render("Метрики користувачів (тис.)", data);
    }
}

public sealed class LogVolumeView : AnalyticsView
{
    public LogVolumeView(IRenderer renderer) : base(renderer) { }

    public override void Show()
    {
        var data = new List<(string, double)>
        {
            ("INFO",        50.0),
            ("WARN",        15.0),
            ("ERROR",        4.5),
            ("FATAL",        0.3),
        };
        Renderer.Render("Обсяг логів за добу (тис. записів)", data);
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class BridgeDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 6 — Bridge: Аналітика × Відображення      │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // 3 типи даних × 3 способи відображення = 9 комбінацій
        // БЕЗ створення 9 окремих класів — Bridge розділяє ці дві ієрархії
        AnalyticsView[] views =
        [
            new FinancialMetricsView(new WebDashboardRenderer()),
            new UserMetricsView     (new MobileAppRenderer()),
            new LogVolumeView       (new PdfExportRenderer()),
            new FinancialMetricsView(new PdfExportRenderer()),    // повторне використання
        ];

        foreach (var v in views)
        {
            v.Show();
            Console.WriteLine();
        }
    }
}
