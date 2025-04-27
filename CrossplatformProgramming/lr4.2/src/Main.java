
    public static void main(String[] args) {
        String text = "I do not want to learn English, I want to study Java!";

        System.out.println("Початковий текст: " + text);

        task2(text);
    }

    public static void task2(String text) {
        int mid = text.length() / 2;
        String firstHalf = text.substring(0, mid);
        String secondHalf = text.substring(mid);

        String result = firstHalf + "***" + secondHalf;

        System.out.println("Розбитий текст з роздільником: " + result);
        System.out.println("Кількість символів у тексті: " + text.length());
    }

