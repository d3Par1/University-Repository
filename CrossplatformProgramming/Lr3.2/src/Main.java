
public static void main(String[] args) {
    int[] sequence1 = { -5, 3, 0, -2, 4, 0, -1, 7, -3, 8 };
    int[] sequence2 = { 6, -4, 0, 0, 2, -8, 1, -1, 5, -6, 3 };

    int[] result1 = task2(sequence1);
    int[] result2 = task2(sequence2); 
    System.out.printf("Sequence 1 - Negatives: %d, Zeros: %d, Positives: %d%n",
            result1[0], result1[1], result1[2]);
    System.out.printf("Sequence 2 - Negatives: %d, Zeros: %d, Positives: %d%n",
            result2[0], result2[1], result2[2]); 
}

public static int[] task2(int[] sequence) {
    int negatives = 0;
    int zeros = 0;
    int positives = 0;

    for (int num : sequence) {
        if (num < 0) {
            negatives++;
        } else if (num == 0) {
            zeros++;
        } else {
            positives++;
        }
    }

    return new int[]{negatives, zeros, positives};
}

