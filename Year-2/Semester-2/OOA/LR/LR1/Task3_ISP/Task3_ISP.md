# ЛР1 — Завдання 3. Порушення принципу розділення інтерфейсів (ISP)

Принцип розділення інтерфейсів (Interface Segregation Principle) стверджує, що клієнти не повинні залежати від інтерфейсів, які вони не використовують. Краще мати кілька малих спеціалізованих інтерфейсів, ніж один великий.

## Вихідний код із порушенням

```csharp
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
```

## Аналіз порушення

Інтерфейс `IEnroll` змушує клас `ProductEnroll` реалізовувати методи `SendEmail()` та `SendSMS()`, які не мають сенсу для продуктів. Це призводить до `NotImplementedException` — явної ознаки порушення ISP.

## Виправлений код

```csharp
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
```

## Висновок

Тепер кожен клас реалізує лише ті інтерфейси, які йому дійсно потрібні. `ProductEnroll` не змушений реалізовувати методи надсилання повідомлень. Жодних `NotImplementedException`.
