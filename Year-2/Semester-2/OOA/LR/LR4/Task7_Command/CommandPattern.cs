namespace Task7_Command;

// ─── Receiver ────────────────────────────────────────────────────────────────

public sealed class DocumentRepository
{
    private readonly Dictionary<int, string> _docs = new();
    private int _nextId = 1;

    public int Create(string content)
    {
        var id = _nextId++;
        _docs[id] = content;
        Console.WriteLine($"     [Repo] CREATE  #{id}: \"{content}\"");
        return id;
    }

    public void Delete(int id, out string backup)
    {
        backup = _docs[id];
        _docs.Remove(id);
        Console.WriteLine($"     [Repo] DELETE  #{id} (backup збережено)");
    }

    public void Restore(int id, string content)
    {
        _docs[id] = content;
        Console.WriteLine($"     [Repo] RESTORE #{id}: \"{content}\"");
    }

    public void Update(int id, string newContent, out string oldContent)
    {
        oldContent = _docs[id];
        _docs[id]  = newContent;
        Console.WriteLine($"     [Repo] UPDATE  #{id}: \"{oldContent}\" → \"{newContent}\"");
    }

    public int Count => _docs.Count;
}

// ─── Command Interface ───────────────────────────────────────────────────────

public interface IDocumentCommand
{
    string Name { get; }
    void Execute();
    void Undo();
}

// ─── Concrete Commands ───────────────────────────────────────────────────────

public sealed class CreateDocumentCommand : IDocumentCommand
{
    private readonly DocumentRepository _repo;
    private readonly string _content;
    private int _createdId;

    public CreateDocumentCommand(DocumentRepository repo, string content)
    {
        _repo    = repo;
        _content = content;
    }

    public string Name => $"CREATE «{_content[..Math.Min(15, _content.Length)]}…»";

    public void Execute() => _createdId = _repo.Create(_content);

    public void Undo()
    {
        _repo.Delete(_createdId, out _);
        Console.WriteLine($"     ↶ undo CREATE → видалено #{_createdId}");
    }
}

public sealed class DeleteDocumentCommand : IDocumentCommand
{
    private readonly DocumentRepository _repo;
    private readonly int _id;
    private string _backup = string.Empty;

    public DeleteDocumentCommand(DocumentRepository repo, int id)
    {
        _repo = repo;
        _id   = id;
    }

    public string Name => $"DELETE #{_id}";

    public void Execute() => _repo.Delete(_id, out _backup);

    public void Undo()
    {
        _repo.Restore(_id, _backup);
        Console.WriteLine($"     ↶ undo DELETE → відновлено #{_id}");
    }
}

public sealed class UpdateDocumentCommand : IDocumentCommand
{
    private readonly DocumentRepository _repo;
    private readonly int _id;
    private readonly string _newContent;
    private string _oldContent = string.Empty;

    public UpdateDocumentCommand(DocumentRepository repo, int id, string newContent)
    {
        _repo       = repo;
        _id         = id;
        _newContent = newContent;
    }

    public string Name => $"UPDATE #{_id}";

    public void Execute() => _repo.Update(_id, _newContent, out _oldContent);

    public void Undo()
    {
        _repo.Update(_id, _oldContent, out _);
        Console.WriteLine($"     ↶ undo UPDATE → повернуто старий вміст");
    }
}

// ─── Invoker ─────────────────────────────────────────────────────────────────

public sealed class DocumentCommandInvoker
{
    private readonly Stack<IDocumentCommand> _executed = new();

    public void Execute(IDocumentCommand cmd)
    {
        Console.WriteLine($"  >> Виконую: {cmd.Name}");
        cmd.Execute();
        _executed.Push(cmd);
    }

    public void UndoLast()
    {
        if (_executed.Count == 0)
        {
            Console.WriteLine("  >> Нічого скасовувати");
            return;
        }
        var cmd = _executed.Pop();
        Console.WriteLine($"  >> Скасовую: {cmd.Name}");
        cmd.Undo();
    }
}

// ─── Demo ─────────────────────────────────────────────────────────────────────

public static class CommandDemo
{
    public static void Run()
    {
        Console.WriteLine("┌──────────────────────────────────────────────────────┐");
        Console.WriteLine("│  Завдання 7 — Command: Документообіг з undo          │");
        Console.WriteLine("└──────────────────────────────────────────────────────┘");
        Console.WriteLine();

        var repo    = new DocumentRepository();
        var invoker = new DocumentCommandInvoker();

        invoker.Execute(new CreateDocumentCommand(repo, "Договір №1 з ТОВ 'Альфа'"));
        invoker.Execute(new CreateDocumentCommand(repo, "Договір №2 з ТОВ 'Бета'"));
        invoker.Execute(new UpdateDocumentCommand(repo, 1, "Договір №1 (ред.) з ТОВ 'Альфа'"));
        invoker.Execute(new DeleteDocumentCommand(repo, 2));

        Console.WriteLine();
        Console.WriteLine($"  >> Документів у репо: {repo.Count}");
        Console.WriteLine();

        Console.WriteLine("  >> Скасовуємо останні 3 дії:");
        invoker.UndoLast();
        invoker.UndoLast();
        invoker.UndoLast();

        Console.WriteLine();
        Console.WriteLine($"  >> Документів у репо після undo: {repo.Count}");
        Console.WriteLine();
    }
}
