// Converting Hanami code to Java
import java.util.Scanner;

public class TestGarden {
    private static Scanner inputScanner = new Scanner(System.in);

    static class Pet {
        private String name = "Unknown";
        private int age = 0;
        public void setDetails(String newName, int newAge) {
            name = newName;
            age = newAge;
            System.out.print("Pet details updated: Name=" + name + ", Age=" + age + "\n");
            return;
        }

        public void introduce() {
            System.out.print("Woof! My name is " + name + " and I am " + age + " year(s) old.\n");
            return;
        }

        protected int getAgeInDogYears() {
            return (age * 7);
        }

    }

    public static void main(String[] args) {
        System.out.print("--- Hanami Pet Program Starting ---\n");
        String ownerName;
        boolean nameIsHanami = false;
        int calculatedValue = 0;
        Pet myDog = new Pet();
        myDog.setDetails("Buddy", 3);
        myDog.introduce();
        System.out.print("What is your name? ");
        ownerName = inputScanner.nextLine(); // Reads as String
        if ((ownerName.equals("Hanami"))) {
            System.out.print("Welcome, Creator Hanami!\n");
            nameIsHanami = true;
            calculatedValue = 100;
        }
        else if ((ownerName.equals("Buddy"))) {
            System.out.print("Hey, that's my name!\n");
            calculatedValue = 50;
        }
        else {
            System.out.print("Nice to meet you, " + ownerName + "!\n");
            nameIsHanami = false;
            calculatedValue = myDog.getAgeInDogYears();
        }
        calculatedValue = (calculatedValue + 5);
        System.out.print("Calculated value: " + calculatedValue + "\n");
        System.out.print("Is owner Hanami? " + nameIsHanami + "\n");
        System.out.print("--- Hanami Pet Program Ending ---\n");
        System.exit(0);
    }

}
