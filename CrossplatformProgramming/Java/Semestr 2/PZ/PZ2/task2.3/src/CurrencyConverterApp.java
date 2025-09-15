import java.util.Scanner;

// Інтерфейс для конвертації валюти
interface CurrencyConverter {
    double convertToUAH(double amount);
    String getCurrencyCode();
    double getExchangeRate();
}

// Клас для конвертації долара США в гривню
class USDConverter implements CurrencyConverter {
    private static final double USD_TO_UAH_RATE = 38.5; // Приклад курсу долара до гривні

    @Override
    public double convertToUAH(double amount) {
        return amount * USD_TO_UAH_RATE;
    }

    @Override
    public String getCurrencyCode() {
        return "USD";
    }

    @Override
    public double getExchangeRate() {
        return USD_TO_UAH_RATE;
    }
}

// Клас для конвертації євро в гривню
class EURConverter implements CurrencyConverter {
    private static final double EUR_TO_UAH_RATE = 41.2; // Приклад курсу євро до гривні

    @Override
    public double convertToUAH(double amount) {
        return amount * EUR_TO_UAH_RATE;
    }

    @Override
    public String getCurrencyCode() {
        return "EUR";
    }

    @Override
    public double getExchangeRate() {
        return EUR_TO_UAH_RATE;
    }
}

// Головний клас програми
public class CurrencyConverterApp {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("Конвертер валют в гривню (UAH)");
        System.out.println("Доступні валюти: USD (долар США), EUR (євро)");
        System.out.print("Введіть суму та валюту (наприклад, 15 USD або 20 EUR): ");

        String input = scanner.nextLine().trim();

        try {
            // Розбиваємо введений рядок на суму та код валюти
            String[] parts = input.split("\\s+");
            if (parts.length != 2) {
                throw new IllegalArgumentException("Невірний формат введення. Приклад: 15 USD");
            }

            double amount = Double.parseDouble(parts[0]);
            String currencyCode = parts[1].toUpperCase();

            // Вибір конвертера в залежності від коду валюти
            CurrencyConverter converter;
            switch (currencyCode) {
                case "USD":
                    converter = new USDConverter();
                    break;
                case "EUR":
                    converter = new EURConverter();
                    break;
                default:
                    throw new IllegalArgumentException("Непідтримувана валюта: " + currencyCode);
            }

            // Виконуємо конвертацію
            double result = converter.convertToUAH(amount);

            // Виводимо результат
            System.out.printf("%.2f %s = %.2f UAH (курс: 1 %s = %.2f UAH)%n",
                    amount, converter.getCurrencyCode(), result,
                    converter.getCurrencyCode(), converter.getExchangeRate());

        } catch (NumberFormatException e) {
            System.out.println("Помилка: Невірний формат числа");
        } catch (IllegalArgumentException e) {
            System.out.println("Помилка: " + e.getMessage());
        }

        scanner.close();
    }
}