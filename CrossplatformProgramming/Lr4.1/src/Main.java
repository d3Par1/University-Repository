public class Main {
    public static void main(String[] args) {
        String text = "I do not want to learnEnglish, I want to study Java!! I do not 0990 want to learn007* history, I911 want tostudy765 Java!! But, 0001 maybe, I do notwant to study666 at all=((";

        String modifiedText = text.replaceAll("[a-zA-Z]", "");
        int removedCount = text.length() - modifiedText.length();

        System.out.println("Початковий текст: " + text);
        System.out.println("Результат: " + modifiedText);
        System.out.println("Кількість видалених символів: " + removedCount);
    }

}