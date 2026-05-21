namespace Task10_Visitor;

// ─── Visitor Interface ───────────────────────────────────────────────────────

public interface IFinancialVisitor
{
    void Visit(Invoice doc);
    void Visit(Contract doc);
    void Visit(TaxReport doc);
}

// ─── Element Interface ───────────────────────────────────────────────────────

public abstract class FinancialDocument
{
    public string Title { get; }
    public DateTime Date { get; }

    protected FinancialDocument(string title, DateTime date)
    {
        Title = title;
        Date  = date;
    }

    public abstract void Accept(IFinancialVisitor visitor);
}

// ─── Concrete Elements (структура стабільна) ─────────────────────────────────

public sealed class Invoice : FinancialDocument
{
    public decimal Amount { get; }
    public string  Currency { get; }
    public Invoice(string title, DateTime date, decimal amount, string currency)
        : base(title, date) { Amount = amount; Currency = currency; }

    public override void Accept(IFinancialVisitor v) => v.Visit(this);
}

public sealed class Contract : FinancialDocument
{
    public decimal MonthlyValue { get; }
    public int     DurationMonths { get; }
    public Contract(string title, DateTime date, decimal monthly, int months)
        : base(title, date) { MonthlyValue = monthly; DurationMonths = months; }

    public override void Accept(IFinancialVisitor v) => v.Visit(this);
}

public sealed class TaxReport : FinancialDocument
{
    public decimal TaxableIncome { get; }
    public decimal TaxRate { get; }
    public TaxReport(string title, DateTime date, decimal income, decimal rate)
        : base(title, date) { TaxableIncome = income; TaxRate = rate; }

    public override void Accept(IFinancialVisitor v) => v.Visit(this);
}

// ─── Concrete Visitors (нові операції без зміни Element-класів) ──────────────

public sealed class TotalCostVisitor : IFinancialVisitor
{
    public decimal Total { get; private set; }

    public void Visit(Invoice doc)
    {
        Total += doc.Amount;
        Console.WriteLine($"     [Total]    Invoice  '{doc.Title}': +{doc.Amount} {doc.Currency}");
    }

    public void Visit(Contract doc)
    {
        var sum = doc.MonthlyValue * doc.DurationMonths;
        Total  += sum;
        Console.WriteLine($"     [Total]    Contract '{doc.Title}': +{sum} грн ({doc.DurationMonths} міс.)");
    }

    public void Visit(TaxReport doc)
    {
        var tax = doc.TaxableIncome * doc.TaxRate;
        Total  += tax;
        Console.WriteLine($"     [Total]    TaxReport '{doc.Title}': +{tax} грн (податок)");
    }
}

public sealed class RiskAuditVisitor : IFinancialVisitor
{
    public List<string> Risks { get; } = new();

    public void Visit(Invoice doc)
    {
        if (doc.Amount > 100_000m)
            Risks.Add($"Invoice '{doc.Title}' > 100k {doc.Currency}");
    }

    public void Visit(Contract doc)
    {
        if (doc.DurationMonths >= 24)
            Risks.Add($"Contract '{doc.Title}' має дуже довгу дію ({doc.DurationMonths} міс.)");
    }

    public void Visit(TaxReport doc)
    {
        if (doc.TaxRate > 0.30m)
            Risks.Add($"TaxReport '{doc.Title}' має податкову ставку {doc.TaxRate:P0}");
    }
}

public sealed class ExportVisitor : IFinancialVisitor
{
    public void Visit(Invoice doc)   => Console.WriteLine($"     [Export]   <invoice  title=\"{doc.Title}\"  amount=\"{doc.Amount}\" currency=\"{doc.Currency}\"/>");
    public void Visit(Contract doc)  => Console.WriteLine($"     [Export]   <contract title=\"{doc.Title}\"  monthly=\"{doc.MonthlyValue}\" months=\"{doc.DurationMonths}\"/>");
    public void Visit(TaxReport doc) => Console.WriteLine($"     [Export]   <tax      title=\"{doc.Title}\"  income=\"{doc.TaxableIncome}\" rate=\"{doc.TaxRate}\"/>");
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class VisitorDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 10 — Visitor: Аудит фінансових документів  │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        FinancialDocument[] documents =
        [
            new Invoice  ("Інвойс #1024",  new DateTime(2026, 1, 15),  82500m,  "грн"),
            new Contract ("Контракт KPI-А", new DateTime(2026, 2,  1),  35000m,    36),
            new TaxReport("Декларація Q1",  new DateTime(2026, 4, 30), 980000m, 0.18m),
            new Invoice  ("Інвойс #1025",  new DateTime(2026, 3, 12), 145000m,  "грн"),
            new Contract ("Контракт ТОВ-Б", new DateTime(2026, 3, 20),  12000m,    12),
        ];

        Console.WriteLine("  >> Операція 1: підрахунок загальної вартості");
        var totalizer = new TotalCostVisitor();
        foreach (var d in documents) d.Accept(totalizer);
        Console.WriteLine($"     ▶ Підсумок: {totalizer.Total} грн");
        Console.WriteLine();

        Console.WriteLine("  >> Операція 2: аудит ризиків");
        var auditor = new RiskAuditVisitor();
        foreach (var d in documents) d.Accept(auditor);
        Console.WriteLine($"     ▶ Знайдено {auditor.Risks.Count} ризиків:");
        foreach (var r in auditor.Risks) Console.WriteLine($"        ⚠ {r}");
        Console.WriteLine();

        Console.WriteLine("  >> Операція 3: експорт у XML");
        var exporter = new ExportVisitor();
        foreach (var d in documents) d.Accept(exporter);
        Console.WriteLine();
    }
}
