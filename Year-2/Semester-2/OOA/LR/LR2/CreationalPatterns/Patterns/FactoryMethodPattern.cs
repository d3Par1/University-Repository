namespace CreationalPatterns.Patterns;

// ─── Product Interface ────────────────────────────────────────────────────────

public interface IReport
{
    void Generate(string data);
}

// ─── Concrete Products ────────────────────────────────────────────────────────

public sealed class PdfReport : IReport
{
    public void Generate(string data)
    {
        Console.WriteLine("  [PDF] Генерація звіту у форматі PDF...");
        Console.WriteLine($"  [PDF] Вбудовування шрифтів та векторної графіки");
        Console.WriteLine($"  [PDF] Дані: {data}");
        Console.WriteLine("  [PDF] Збережено: звіт.pdf (захищений паролем)");
    }
}

public sealed class ExcelReport : IReport
{
    public void Generate(string data)
    {
        Console.WriteLine("  [Excel] Генерація звіту у форматі XLSX...");
        Console.WriteLine("  [Excel] Побудова таблиць та діаграм");
        Console.WriteLine($"  [Excel] Дані: {data}");
        Console.WriteLine("  [Excel] Збережено: звіт.xlsx (5 аркушів)");
    }
}

public sealed class HtmlReport : IReport
{
    public void Generate(string data)
    {
        Console.WriteLine("  [HTML] Генерація звіту у форматі HTML...");
        Console.WriteLine("  [HTML] Застосування CSS-стилів та адаптивної верстки");
        Console.WriteLine($"  [HTML] Дані: {data}");
        Console.WriteLine("  [HTML] Збережено: звіт.html (відкривається у браузері)");
    }
}

public sealed class JsonReport : IReport
{
    public void Generate(string data)
    {
        Console.WriteLine("  [JSON] Генерація звіту у форматі JSON...");
        Console.WriteLine("  [JSON] Серіалізація даних із відступами");
        Console.WriteLine($"  [JSON] Дані: {data}");
        Console.WriteLine("  [JSON] Збережено: звіт.json (UTF-8, без BOM)");
    }
}

// ─── Abstract Creator (Factory Method) ───────────────────────────────────────

public abstract class ReportFactory
{
    // Фабричний метод — підкласи визначають, який продукт створювати
    public abstract IReport CreateReport();

    // Шаблонний метод — загальний алгоритм
    public void ExportReport(string data)
    {
        var report = CreateReport();
        report.Generate(data);
    }
}

// ─── Concrete Creators ────────────────────────────────────────────────────────

public sealed class PdfReportFactory  : ReportFactory { public override IReport CreateReport() => new PdfReport(); }
public sealed class ExcelReportFactory: ReportFactory { public override IReport CreateReport() => new ExcelReport(); }
public sealed class HtmlReportFactory : ReportFactory { public override IReport CreateReport() => new HtmlReport(); }
public sealed class JsonReportFactory : ReportFactory { public override IReport CreateReport() => new JsonReport(); }

// ─── Static Resolver ─────────────────────────────────────────────────────────

public static class ReportFactoryResolver
{
    public static ReportFactory GetFactory(string format) =>
        format.ToUpperInvariant() switch
        {
            "PDF"   => new PdfReportFactory(),
            "EXCEL" => new ExcelReportFactory(),
            "HTML"  => new HtmlReportFactory(),
            "JSON"  => new JsonReportFactory(),
            _       => throw new ArgumentException($"Непідтримуваний формат звіту: '{format}'"),
        };
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class FactoryMethodDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 5 — Factory Method: Експорт звітів        │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        // Симуляція «налаштувань» — список форматів, отриманих ззовні
        string[] formats      = ["PDF", "EXCEL", "HTML", "JSON"];
        string   reportData   = "Продажі Q1-2026: Північ=142000, Центр=98700, Південь=231500 грн";

        foreach (var format in formats)
        {
            Console.WriteLine($"  >> Формат: {format}");
            var factory = ReportFactoryResolver.GetFactory(format);
            factory.ExportReport(reportData);
            Console.WriteLine();
        }
    }
}
