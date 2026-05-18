namespace BehavioralPatterns.Patterns;

// ─── State Interface ─────────────────────────────────────────────────────────

public interface IVacuumState
{
    string Name { get; }
    void Tick(VacuumRobot robot);
}

// ─── Context ─────────────────────────────────────────────────────────────────

public sealed class VacuumRobot
{
    public int  Battery  { get; set; } = 100;
    public bool Stuck    { get; set; }
    private IVacuumState _state;

    public VacuumRobot()
    {
        _state = new CleaningState();
        Console.WriteLine($"     [Robot] Стартовий стан: {_state.Name}, батарея={Battery}%");
    }

    public void SetState(IVacuumState newState)
    {
        Console.WriteLine($"     [Robot] {_state.Name} → {newState.Name}");
        _state = newState;
    }

    public void Tick() => _state.Tick(this);
}

// ─── Concrete States ─────────────────────────────────────────────────────────

public sealed class CleaningState : IVacuumState
{
    public string Name => "ПРИБИРАННЯ";

    public void Tick(VacuumRobot robot)
    {
        robot.Battery -= 8;
        Console.WriteLine($"     [Clean]   жжжж... батарея={robot.Battery}%");

        if (robot.Stuck)              robot.SetState(new StuckState());
        else if (robot.Battery <= 20) robot.SetState(new SeekingBaseState());
    }
}

public sealed class SeekingBaseState : IVacuumState
{
    public string Name => "ПОШУК ДОКУ";

    public void Tick(VacuumRobot robot)
    {
        robot.Battery -= 3;
        Console.WriteLine($"     [Seek]    шукаю док... батарея={robot.Battery}%");

        if (robot.Battery <= 0)
            robot.SetState(new StuckState());
        else if (robot.Battery <= 5)
        {
            Console.WriteLine($"     [Seek]    >>> прибув до доку, заряджаюсь <<<");
            robot.Battery = 100;
            robot.SetState(new CleaningState());
        }
    }
}

public sealed class StuckState : IVacuumState
{
    public string Name => "ЗАСТРЯГ";

    public void Tick(VacuumRobot robot)
    {
        Console.WriteLine($"     [Stuck]   ⚠ ПІІІ-ПІІІ-ПІІІ батарея={robot.Battery}%");
        if (!robot.Stuck && robot.Battery > 20)
            robot.SetState(new CleaningState());
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class StateDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 3 — State: Робот-пилосос                   │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var robot = new VacuumRobot();

        // 10 тіків звичайної роботи
        for (int t = 1; t <= 12; t++)
        {
            Console.WriteLine($"  >> Тік {t}");
            robot.Tick();
        }
        Console.WriteLine();

        // Симулюємо застрягання
        Console.WriteLine("  >> Спричиняємо застрягання:");
        robot.Stuck = true;
        robot.Tick();
        robot.Tick();

        // Звільнюємо і даємо заряд
        Console.WriteLine();
        Console.WriteLine("  >> Користувач звільняє робота і заряджає батарею:");
        robot.Stuck    = false;
        robot.Battery  = 100;
        robot.Tick();
        Console.WriteLine();
    }
}
