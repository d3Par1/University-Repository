namespace Task9_Mediator;

// ─── Mediator Interface ──────────────────────────────────────────────────────

public interface IAirportMediator
{
    void Register(AirportParticipant p);
    void Send(AirportParticipant sender, string action, string payload);
}

// ─── Colleague (Participant) ─────────────────────────────────────────────────

public abstract class AirportParticipant
{
    public string Name { get; }
    protected IAirportMediator? Mediator;

    protected AirportParticipant(string name) => Name = name;

    public void SetMediator(IAirportMediator m) => Mediator = m;

    public abstract void Receive(string fromName, string action, string payload);
    protected void Send(string action, string payload) =>
        Mediator?.Send(this, action, payload);
}

// ─── Concrete Colleagues ─────────────────────────────────────────────────────

public sealed class Aircraft : AirportParticipant
{
    public Aircraft(string name) : base(name) { }

    public void RequestLanding()
    {
        Console.WriteLine($"  >> {Name}: запит на посадку");
        Send("REQUEST_LANDING", "паливо=10%, пасажирів=180");
    }

    public override void Receive(string from, string action, string payload)
    {
        Console.WriteLine($"     {Name,-12} ← [{from,-12}] {action} {payload}");
    }
}

public sealed class Dispatcher : AirportParticipant
{
    public Dispatcher() : base("Диспетчер") { }

    public override void Receive(string from, string action, string payload)
    {
        Console.WriteLine($"     {Name,-12} ← [{from,-12}] {action} {payload}");

        if (action == "REQUEST_LANDING")
        {
            Send("CHECK_RUNWAY", "потрібна смуга 27L");
        }
        else if (action == "RUNWAY_OK")
        {
            Send("CALL_GROUND_SERVICES", "паливо + техогляд");
        }
        else if (action == "GROUND_READY")
        {
            Send("CLEAR_TO_LAND", "смуга 27L, висота 300м");
        }
    }
}

public sealed class RunwayControl : AirportParticipant
{
    public RunwayControl() : base("Смуга") { }

    public override void Receive(string from, string action, string payload)
    {
        Console.WriteLine($"     {Name,-12} ← [{from,-12}] {action} {payload}");

        if (action == "CHECK_RUNWAY")
            Send("RUNWAY_OK", "смуга 27L вільна");
    }
}

public sealed class GroundServices : AirportParticipant
{
    public GroundServices() : base("Наземні") { }

    public override void Receive(string from, string action, string payload)
    {
        Console.WriteLine($"     {Name,-12} ← [{from,-12}] {action} {payload}");

        if (action == "CALL_GROUND_SERVICES")
            Send("GROUND_READY", "паливозаправник готовий");
    }
}

// ─── Concrete Mediator ───────────────────────────────────────────────────────

public sealed class AirportTowerMediator : IAirportMediator
{
    private readonly List<AirportParticipant> _participants = [];

    public void Register(AirportParticipant p)
    {
        _participants.Add(p);
        p.SetMediator(this);
    }

    public void Send(AirportParticipant sender, string action, string payload)
    {
        // Центральне місце — диспетчер знає, кому передавати
        foreach (var p in _participants)
            if (!ReferenceEquals(p, sender))
                p.Receive(sender.Name, action, payload);
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class MediatorDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 9 — Mediator: Авіадиспетчерська вежа       │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var tower      = new AirportTowerMediator();
        var aircraft   = new Aircraft("MAU-401");
        var dispatcher = new Dispatcher();
        var runway     = new RunwayControl();
        var ground     = new GroundServices();

        tower.Register(aircraft);
        tower.Register(dispatcher);
        tower.Register(runway);
        tower.Register(ground);

        aircraft.RequestLanding();
        Console.WriteLine();
    }
}
