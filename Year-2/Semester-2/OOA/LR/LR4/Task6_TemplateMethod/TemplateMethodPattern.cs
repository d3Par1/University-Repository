namespace Task6_TemplateMethod;

// ─── Abstract Class з template method ────────────────────────────────────────

public abstract class OnlineCourse
{
    public string Name { get; }
    protected OnlineCourse(string name) => Name = name;

    // ── Template Method — алгоритм зафіксовано, послідовність змінювати не можна
    public void Complete()
    {
        Console.WriteLine($"  >> Старт курсу '{Name}'");
        WatchLectures();
        DoExercises();
        ExtraVerificationStep();      // hook (опціонально перевизначається)
        TakeQuiz();
        DefendFinalProject();
        Console.WriteLine($"  >> Курс '{Name}' завершено");
        Console.WriteLine();
    }

    // ── Спільні кроки (final-методи у мові, де це можливо)
    private void WatchLectures()       => Console.WriteLine($"     [1/5] 📺 Перегляд лекцій курсу '{Name}'");
    private void DoExercises()         => Console.WriteLine($"     [2/5] ✍ Виконання практичних завдань");
    private void TakeQuiz()            => Console.WriteLine($"     [4/5] ❓ Проходження тестів");

    // ── Кроки, які переозначають підкласи
    protected abstract void DefendFinalProject();

    // ── Hook: опціональна додаткова перевірка
    protected virtual void ExtraVerificationStep()
    {
        // Базова реалізація — нічого не робить
    }
}

// ─── Concrete Courses ────────────────────────────────────────────────────────

public sealed class ProgrammingCourse : OnlineCourse
{
    public ProgrammingCourse() : base("Програмування") { }

    protected override void ExtraVerificationStep() =>
        Console.WriteLine($"     [3/5] 🤖 Автоматична перевірка коду через лінтер та тести");

    protected override void DefendFinalProject() =>
        Console.WriteLine($"     [5/5] 💻 Захист фінального коду через GitHub-PR + рев'ю наставника");
}

public sealed class DesignCourse : OnlineCourse
{
    public DesignCourse() : base("Дизайн") { }

    protected override void ExtraVerificationStep() =>
        Console.WriteLine($"     [3/5] 🎨 Презентація макета перед викладачем");

    protected override void DefendFinalProject() =>
        Console.WriteLine($"     [5/5] 🖼 Захист фінального макета у Figma");
}

public sealed class ManagementCourse : OnlineCourse
{
    public ManagementCourse() : base("Менеджмент") { }

    // ExtraVerificationStep не переозначаємо — у менеджерів його немає

    protected override void DefendFinalProject() =>
        Console.WriteLine($"     [5/5] 📋 Захист стратегічного плану перед комітетом");
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class TemplateMethodDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 6 — Template Method: онлайн-навчання       │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        OnlineCourse[] courses =
        [
            new ProgrammingCourse(),
            new DesignCourse(),
            new ManagementCourse(),
        ];

        foreach (var c in courses) c.Complete();
    }
}
