namespace Task4_Memento;

// ─── Memento (immutable знімок стану) ────────────────────────────────────────

public sealed class EditorMemento
{
    // Внутрішні поля доступні тільки для відновлення — інкапсуляція збережена
    internal string Text  { get; }
    internal string Color { get; }
    internal DateTime CreatedAt { get; }

    internal EditorMemento(string text, string color)
    {
        Text      = text;
        Color     = color;
        CreatedAt = DateTime.Now;
    }
}

// ─── Originator ──────────────────────────────────────────────────────────────

public sealed class TextEditor
{
    private string _text  = string.Empty;
    private string _color = "чорний";

    public void Type(string addition)
    {
        _text += addition;
        Console.WriteLine($"     ✎ ввід: '{addition}'   стан: \"{_text}\" [{_color}]");
    }

    public void DeleteLast(int n)
    {
        n = Math.Min(n, _text.Length);
        _text = _text[..^n];
        Console.WriteLine($"     ⌫ видалено {n} симв.  стан: \"{_text}\" [{_color}]");
    }

    public void SetColor(string color)
    {
        _color = color;
        Console.WriteLine($"     ✦ колір → {color}    стан: \"{_text}\" [{_color}]");
    }

    public EditorMemento CreateSnapshot() => new(_text, _color);

    public void Restore(EditorMemento m)
    {
        _text  = m.Text;
        _color = m.Color;
        Console.WriteLine($"     ↶ undo (знімок {m.CreatedAt:HH:mm:ss.fff})  стан: \"{_text}\" [{_color}]");
    }
}

// ─── Caretaker (історія) ─────────────────────────────────────────────────────

public sealed class History
{
    private readonly Stack<EditorMemento> _stack = new();

    public void Push(EditorMemento m) => _stack.Push(m);

    public EditorMemento? Pop() => _stack.Count > 0 ? _stack.Pop() : null;

    public int Depth => _stack.Count;
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class MementoDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 4 — Memento: Ctrl+Z у текстовому редакторі │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var editor  = new TextEditor();
        var history = new History();

        // Робимо знімок перед кожною дією
        history.Push(editor.CreateSnapshot()); editor.Type("Привіт, ");
        history.Push(editor.CreateSnapshot()); editor.Type("світ!");
        history.Push(editor.CreateSnapshot()); editor.SetColor("синій");
        history.Push(editor.CreateSnapshot()); editor.Type(" Як справи?");
        history.Push(editor.CreateSnapshot()); editor.DeleteLast(11);
        history.Push(editor.CreateSnapshot()); editor.SetColor("червоний");

        Console.WriteLine();
        Console.WriteLine($"  >> Натискаємо Ctrl+Z 3 рази (стек: {history.Depth}):");
        for (int i = 0; i < 3; i++)
        {
            var m = history.Pop();
            if (m is not null) editor.Restore(m);
        }
        Console.WriteLine();
    }
}
