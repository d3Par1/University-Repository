# ЛР1 — Завдання 1. Порушення принципу єдиної відповідальності (SRP)

Принцип єдиної відповідальності (Single Responsibility Principle) стверджує, що клас повинен мати лише одну причину для зміни. Кожен клас має відповідати лише за одну частину функціональності.

## Вихідний код із порушенням

```csharp
public class Client
{
    public int Id { get; set; }
    public string Name { get; set; }
    public string Email { get; set; }
    public DateTime DateOfBirth { get; set; }

    public (bool status, string errorMessage) Add()
    {
        // Validations
        if (string.IsNullOrEmpty(this.Name))
            return (false, "Name is invalid");
        if (!this.Email.Contains("@"))
            return (false, "Email is invalid");
        if (DateOfBirth > DateTime.Now)
            return (false, "Date of Birth is invalid");

        // Persist Data
        using (var cn = new SqlConnection("cnString"))
        {
            var cmd = new SqlCommand(
                "INSERT INTO clients(Name, Email, DateOfBirth) "
                + "VALUES(@Name, @Email, @DateOfBirth)", cn);
            cmd.Parameters.AddWithValue("Name", this.Name);
            cmd.Parameters.AddWithValue("Email", this.Email);
            cmd.Parameters.AddWithValue("DateOfBirth", this.DateOfBirth);
            cmd.ExecuteNonQuery();
        }

        // Send e-mail
        var mail = new MailMessage("no-reply@system.net", this.Email);
        var smtpClient = new SmtpClient
            { Port = 25, Host = "smtp.system.net" };
        mail.Subject = "[System.NET] Welcome";
        mail.Body = "Congrats!";
        smtpClient.Send(mail);

        return (true, string.Empty);
    }
}
```

## Аналіз порушення

Клас `Client` виконує одразу чотири функції: зберігає дані моделі, валідує вхідні дані, зберігає запис у базу даних через SQL-запит, та надсилає вітальний email через SMTP. Якщо зміниться логіка валідації, спосіб збереження або формат листа — все це потребуватиме змін в одному класі.

## Виправлений код

```csharp
// Модель даних
public class Client
{
    public int Id { get; set; }
    public string Name { get; set; }
    public string Email { get; set; }
    public DateTime DateOfBirth { get; set; }
}

// Валідація
public class ClientValidator
{
    public (bool isValid, string error) Validate(Client client)
    {
        if (string.IsNullOrEmpty(client.Name))
            return (false, "Name is invalid");
        if (!client.Email.Contains("@"))
            return (false, "Email is invalid");
        if (client.DateOfBirth > DateTime.Now)
            return (false, "Date of Birth is invalid");
        return (true, string.Empty);
    }
}

// Збереження у БД
public class ClientRepository
{
    public void Save(Client client)
    {
        using (var cn = new SqlConnection("cnString"))
        {
            var cmd = new SqlCommand(
                "INSERT INTO clients(Name, Email, DateOfBirth) "
                + "VALUES(@Name, @Email, @DateOfBirth)", cn);
            cmd.Parameters.AddWithValue("Name", client.Name);
            cmd.Parameters.AddWithValue("Email", client.Email);
            cmd.Parameters.AddWithValue("DateOfBirth", client.DateOfBirth);
            cmd.ExecuteNonQuery();
        }
    }
}

// Надсилання email
public class EmailService
{
    public void SendWelcome(string email)
    {
        var mail = new MailMessage("no-reply@system.net", email);
        var smtp = new SmtpClient { Port = 25, Host = "smtp.system.net" };
        mail.Subject = "[System.NET] Welcome";
        mail.Body = "Congrats!";
        smtp.Send(mail);
    }
}

// Оркестрація
public class ClientService
{
    private readonly ClientValidator _validator = new();
    private readonly ClientRepository _repository = new();
    private readonly EmailService _emailService = new();

    public (bool status, string error) AddClient(Client client)
    {
        var (isValid, error) = _validator.Validate(client);
        if (!isValid) return (false, error);

        _repository.Save(client);
        _emailService.SendWelcome(client.Email);
        return (true, string.Empty);
    }
}
```

## Висновок

Тепер кожен клас має єдину відповідальність: `Client` — модель даних, `ClientValidator` — валідація, `ClientRepository` — збереження, `EmailService` — надсилання пошти, `ClientService` — оркестрація процесу.
