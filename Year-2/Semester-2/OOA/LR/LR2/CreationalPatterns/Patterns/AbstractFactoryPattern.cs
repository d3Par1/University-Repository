namespace CreationalPatterns.Patterns;

// ─── Interfaces ───────────────────────────────────────────────────────────────

public interface IButton    { void Render(); }
public interface ITextField { void Render(); }
public interface IDropdown  { void Render(); }

// ─── Dark Theme Components ───────────────────────────────────────────────────

public sealed class DarkButton : IButton
{
    public void Render() =>
        Console.WriteLine("  [Темна тема] Кнопка:     фон=#1E1E1E, текст=#E0E0E0, рамка=#555555");
}

public sealed class DarkTextField : ITextField
{
    public void Render() =>
        Console.WriteLine("  [Темна тема] Текстове поле: фон=#2D2D2D, текст=#FFFFFF, курсор=#00FFAA");
}

public sealed class DarkDropdown : IDropdown
{
    public void Render() =>
        Console.WriteLine("  [Темна тема] Список:     фон=#252525, текст=#CCCCCC, стрілка=#888888");
}

// ─── Light Theme Components ──────────────────────────────────────────────────

public sealed class LightButton : IButton
{
    public void Render() =>
        Console.WriteLine("  [Світла тема] Кнопка:     фон=#F5F5F5, текст=#212121, рамка=#BDBDBD");
}

public sealed class LightTextField : ITextField
{
    public void Render() =>
        Console.WriteLine("  [Світла тема] Текстове поле: фон=#FFFFFF, текст=#000000, курсор=#1976D2");
}

public sealed class LightDropdown : IDropdown
{
    public void Render() =>
        Console.WriteLine("  [Світла тема] Список:     фон=#FAFAFA, текст=#424242, стрілка=#9E9E9E");
}

// ─── Abstract Factory ────────────────────────────────────────────────────────

public interface IUIFactory
{
    IButton    CreateButton();
    ITextField CreateTextField();
    IDropdown  CreateDropdown();
}

public sealed class DarkThemeFactory : IUIFactory
{
    public IButton    CreateButton()    => new DarkButton();
    public ITextField CreateTextField() => new DarkTextField();
    public IDropdown  CreateDropdown()  => new DarkDropdown();
}

public sealed class LightThemeFactory : IUIFactory
{
    public IButton    CreateButton()    => new LightButton();
    public ITextField CreateTextField() => new LightTextField();
    public IDropdown  CreateDropdown()  => new LightDropdown();
}

// ─── Application (Client) ────────────────────────────────────────────────────

public sealed class Application
{
    private readonly IButton    _button;
    private readonly ITextField _textField;
    private readonly IDropdown  _dropdown;

    public Application(IUIFactory factory)
    {
        _button    = factory.CreateButton();
        _textField = factory.CreateTextField();
        _dropdown  = factory.CreateDropdown();
    }

    public void RenderUI()
    {
        _button.Render();
        _textField.Render();
        _dropdown.Render();
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class AbstractFactoryDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 2 — Abstract Factory: UI-теми             │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        Console.WriteLine("  >> Темна тема інтерфейсу:");
        var darkApp = new Application(new DarkThemeFactory());
        darkApp.RenderUI();
        Console.WriteLine();

        Console.WriteLine("  >> Світла тема інтерфейсу:");
        var lightApp = new Application(new LightThemeFactory());
        lightApp.RenderUI();
        Console.WriteLine();
    }
}
