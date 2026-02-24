


Лабораторна робота №1
Принципи SOLID
1. Порушення принципу єдиної відповідальності (SRP)
Принцип єдиної відповідальності (Single Responsibility Principle) стверджує, що клас повинен мати лише одну причину для зміни. Кожен клас має відповідати лише за одну частину функціональності.
Вихідний код із порушенням:
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
Аналіз порушення:
Клас Client виконує одразу чотири функції: зберігає дані моделі, валідує вхідні дані, зберігає запис у базу даних через SQL-запит, та надсилає вітальний email через SMTP. Якщо зміниться логіка валідації, спосіб збереження або формат листа - все це потребуватиме змін в одному класі.
Виправлений код:
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
            cmd.Parameters.AddWithValue("DateOfBirth",
                client.DateOfBirth);
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
        var smtp = new SmtpClient
            { Port = 25, Host = "smtp.system.net" };
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
Тепер кожен клас має єдину відповідальність: Client - модель даних, ClientValidator - валідація, ClientRepository - збереження, EmailService - надсилання пошти, ClientService - оркестрація процесу.
 
2. Порушення принципу відкритості/закритості (OCP)
Принцип відкритості/закритості (Open/Closed Principle) стверджує, що програмні сутності повинні бути відкритими для розширення, але закритими для модифікації.
Вихідний код із порушенням:
public enum RentType { Daily, Weekly }

public class Car
{
    public string Maker { get; set; }
    public Color Color { get; set; }
}

public class CarRental
{
    public decimal Rent(Car car, decimal baseValue,
        int amount, RentType rentType)
    {
        if (rentType == RentType.Daily)
            return baseValue * amount;
        if (rentType == RentType.Weekly)
            return baseValue * (7 * amount);
        return 0;
    }
}
Аналіз порушення:
Метод Rent() використовує ланцюжок if-перевірок для визначення типу оренди. Якщо потрібно додати новий тип (наприклад, погодинну або місячну оренду), доведеться модифікувати існуючий метод, що порушує принцип OCP.
Виправлений код:
public class Car
{
    public string Maker { get; set; }
    public Color Color { get; set; }
}

// Абстракція стратегії розрахунку оренди
public abstract class RentalStrategy
{
    public abstract decimal Calculate(decimal baseValue, int amount);
}

// Добова оренда
public class DailyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue * amount;
    }
}

// Тижнева оренда
public class WeeklyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue * 7 * amount;
    }
}

// Легко додати нові типи без зміни існуючого коду:
public class HourlyRental : RentalStrategy
{
    public override decimal Calculate(decimal baseValue, int amount)
    {
        return baseValue / 24 * amount;
    }
}

public class CarRental
{
    public decimal Rent(Car car, decimal baseValue,
        int amount, RentalStrategy strategy)
    {
        return strategy.Calculate(baseValue, amount);
    }
}
Тепер для додавання нового типу оренди достатньо створити новий клас-нащадок RentalStrategy, не змінюючи існуючий код CarRental.
 
3. Порушення принципу розділення інтерфейсів (ISP)
Принцип розділення інтерфейсів (Interface Segregation Principle) стверджує, що клієнти не повинні залежати від інтерфейсів, які вони не використовують. Краще мати кілька малих спеціалізованих інтерфейсів, ніж один великий.
Вихідний код із порушенням:
public interface IEnroll
{
    void Validate();
    void Persist();
    void SendEmail();
    void SendSMS();
}

class ProductEnroll : IEnroll
{
    public void Persist()
    { /* Persist to database */ }

    public void SendEmail()
    { throw new NotImplementedException(
        "Product don't have e-mail!"); }

    public void SendSMS()
    { throw new NotImplementedException(
        "Product don't have phone number!"); }

    public void Validate()
    { /* Check data */ }
}

class ContactEnroll : IEnroll
{
    public void Persist()    { /* Persist to database */ }
    public void SendEmail()  { /* Send email */ }
    public void SendSMS()    { /* Send SMS */ }
    public void Validate()   { /* Check data */ }
}
Аналіз порушення:
Інтерфейс IEnroll змушує клас ProductEnroll реалізовувати методи SendEmail() та SendSMS(), які не мають сенсу для продуктів. Це призводить до NotImplementedException - явної ознаки порушення ISP.
Виправлений код:
// Розділені інтерфейси
public interface IValidatable
{
    void Validate();
}

public interface IPersistable
{
    void Persist();
}

public interface IEmailNotifiable
{
    void SendEmail();
}

public interface ISmsNotifiable
{
    void SendSMS();
}

// Продукт реалізує лише потрібні інтерфейси
class ProductEnroll : IValidatable, IPersistable
{
    public void Validate()
    { /* Check data */ }

    public void Persist()
    { /* Persist to database */ }
}

// Контакт реалізує всі інтерфейси
class ContactEnroll : IValidatable, IPersistable,
    IEmailNotifiable, ISmsNotifiable
{
    public void Validate()   { /* Check data */ }
    public void Persist()    { /* Persist to database */ }
    public void SendEmail()  { /* Send email */ }
    public void SendSMS()    { /* Send SMS */ }
}
Тепер кожен клас реалізує лише ті інтерфейси, які йому дійсно потрібні. ProductEnroll не змушений реалізовувати методи надсилання повідомлень.
 
4. Порушення принципу підстановки Лісков (LSP)
Принцип підстановки Лісков (Liskov Substitution Principle) стверджує, що об'єкти підкласу повинні бути замінними на об'єкти базового класу без порушення коректності програми.
Вихідний код із порушенням:
public class Rectangle
{
    public virtual double Height { get; set; }
    public virtual double Width { get; set; }
    public double Area
    { get { return this.Height * this.Width; } }
}

class Square : Rectangle
{
    public override double Height
    { set => base.Width = base.Height = value; }

    public override double Width
    { set => base.Width = base.Height = value; }
}

public class Execution
{
    public Execution()
    {
        var r = new Rectangle()
            { Height = 10, Width = 5 };
        GetRectArea(r);   // 10 * 5 = 50 ✓

        var s = new Square()
        {
            Height = 10,  // стає 10
            Width = 5     // обидва стають 5
        };
        GetRectArea(s);   // 5 * 5 = 25 (очікувалось 50)
    }

    public double GetRectArea(Rectangle rect)
    {
        Debug.WriteLine($"{rect.Height} * {rect.Width}");
        Debug.WriteLine($"{rect.Area}");
        return rect.Area;
    }
}
Аналіз порушення:
Square перевизначає сетери Height та Width так, що обидва встановлюють однакове значення. Коли Square використовується замість Rectangle, поведінка стає непередбачуваною: встановлення Height=10, а потім Width=5 призводить до площі 25 замість очікуваних 50. Це порушує контракт базового класу Rectangle.
Виправлений код:
// Спільний інтерфейс для фігур
public interface IShape
{
    double Area { get; }
}

// Прямокутник — незалежні сторони
public class Rectangle : IShape
{
    public double Height { get; set; }
    public double Width { get; set; }
    public double Area => Height * Width;
}

// Квадрат — одна сторона
public class Square : IShape
{
    public double Side { get; set; }
    public double Area => Side * Side;
}

public class Execution
{
    public Execution()
    {
        var r = new Rectangle
            { Height = 10, Width = 5 };
        PrintArea(r);  // 50

        var s = new Square { Side = 5 };
        PrintArea(s);  // 25
    }

    public double PrintArea(IShape shape)
    {
        Debug.WriteLine($"Area = {shape.Area}");
        return shape.Area;
    }
}
Тепер Rectangle і Square - це окремі реалізації інтерфейсу IShape. Square більше не успадковує Rectangle, тому не може порушити його контракт. Кожна фігура коректно обчислює свою площу.
 
Висновок
У цій лабораторній роботі було проаналізовано чотири фрагменти коду на C# та визначено порушення принципів SOLID:
•	SRP: клас Client мав чотири відповідальності — модель, валідація, збереження та надсилання пошти. Виправлено розділенням на окремі класи.
•	OCP: метод Rent() вимагав модифікації при додаванні нових типів оренди. Виправлено застосуванням патерну Стратегія.
•	ISP: інтерфейс IEnroll змушував класи реалізовувати зайві методи. Виправлено розділенням на малі спеціалізовані інтерфейси.
•	LSP: Square порушував контракт Rectangle при поліморфному використанні. Виправлено заміною наслідування на реалізацію спільного інтерфейсу IShape.

Дотримання принципів SOLID забезпечує модульність, розширюваність та простоту супроводу програмного забезпечення.
