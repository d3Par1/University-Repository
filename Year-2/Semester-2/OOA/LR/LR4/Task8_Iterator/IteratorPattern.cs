using System.Collections;

namespace Task8_Iterator;

// ─── Domain ──────────────────────────────────────────────────────────────────

public sealed record Employee(string FullName, string Role, bool Active);

// ─── Custom collections з різною внутрішньою структурою ──────────────────────

// 1) Відділ — звичайний список
public sealed class Department : IEnumerable<Employee>
{
    public string Name { get; }
    private readonly List<Employee> _staff = [];

    public Department(string name) => Name = name;
    public Department Add(Employee e) { _staff.Add(e); return this; }

    public IEnumerator<Employee> GetEnumerator() => _staff.GetEnumerator();
    IEnumerator IEnumerable.GetEnumerator()      => GetEnumerator();
}

// 2) Проєктна команда — словник за роллю (інша структура!)
public sealed class ProjectTeam : IEnumerable<Employee>
{
    public string ProjectName { get; }
    private readonly Dictionary<string, List<Employee>> _byRole = new();

    public ProjectTeam(string projectName) => ProjectName = projectName;

    public ProjectTeam Assign(Employee e)
    {
        if (!_byRole.TryGetValue(e.Role, out var list))
            _byRole[e.Role] = list = [];
        list.Add(e);
        return this;
    }

    // Власний ітератор обходить мапу — клієнт цього не знає
    public IEnumerator<Employee> GetEnumerator()
    {
        foreach (var role in _byRole.Keys.OrderBy(k => k))
            foreach (var e in _byRole[role])
                yield return e;
    }
    IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
}

// 3) Composite collection — обходить кілька внутрішніх колекцій по черзі
public sealed class CompanyEmployees : IEnumerable<Employee>
{
    private readonly List<IEnumerable<Employee>> _sources = [];

    public CompanyEmployees Include(IEnumerable<Employee> source)
    {
        _sources.Add(source);
        return this;
    }

    public IEnumerator<Employee> GetEnumerator()
    {
        foreach (var src in _sources)
            foreach (var e in src)
                yield return e;
    }
    IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
}

// ─── Specialised iterators ───────────────────────────────────────────────────

public static class EmployeeIterators
{
    public static IEnumerable<Employee> OnlyActive(IEnumerable<Employee> source)
    {
        foreach (var e in source)
            if (e.Active) yield return e;
    }

    public static IEnumerable<Employee> ByRole(IEnumerable<Employee> source, string role)
    {
        foreach (var e in source)
            if (e.Role.Equals(role, StringComparison.OrdinalIgnoreCase))
                yield return e;
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class IteratorDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 8 — Iterator: HRM-система                  │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var rnd = new Department("R&D")
            .Add(new Employee("Іваненко О.",  "Розробник",     true))
            .Add(new Employee("Петренко В.",  "Розробник",     true))
            .Add(new Employee("Сидорчук К.",  "Тестувальник",  true))
            .Add(new Employee("Гриценко Ю.",  "Аналітик",      false));

        var team = new ProjectTeam("Apollo")
            .Assign(new Employee("Степаненко Н.","Розробник",     true))
            .Assign(new Employee("Шевченко М.", "Дизайнер",      true))
            .Assign(new Employee("Лиман Т.",    "Менеджер",      true));

        var adminGroup = new List<Employee>
        {
            new("Бондар І.",   "Адмін",      true),
            new("Кравчук Р.",  "Бухгалтер",  true),
        };

        var company = new CompanyEmployees()
            .Include(rnd)
            .Include(team)
            .Include(adminGroup);

        Console.WriteLine("  >> Усі співробітники компанії (клієнт НЕ знає внутрішньої структури):");
        foreach (var e in company)
            Console.WriteLine($"     • {e.FullName,-16} | {e.Role,-13} | {(e.Active ? "активний" : "неактивний")}");
        Console.WriteLine();

        Console.WriteLine("  >> Лише розробники (через ByRole):");
        foreach (var e in EmployeeIterators.ByRole(company, "Розробник"))
            Console.WriteLine($"     • {e.FullName}");
        Console.WriteLine();

        Console.WriteLine("  >> Лише активні (через OnlyActive):");
        foreach (var e in EmployeeIterators.OnlyActive(company))
            Console.WriteLine($"     • {e.FullName} ({e.Role})");
        Console.WriteLine();
    }
}
