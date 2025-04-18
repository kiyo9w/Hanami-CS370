#include <iostream>
#include <string>
#include <vector>

// Garden: TestGarden
struct Pet {
    private:
        std::string name = std::string("Unknown");
        int age = 0;
    public:
        void setDetails(std::string newName, int newAge) {
            name = newName;
            age = newAge;
            std::cout << std::string("Pet details updated: Name=") << name << std::string(", Age=") << age << std::string("\n");
            return;
        }

        void introduce() {
            std::cout << std::string("Woof! My name is ") << name << std::string(" and I am ") << age << std::string(" year(s) old.\n");
            return;
        }

    public:
        int getAgeInDogYears() {
            return (age * 7);
        }

};

int main() {
    std::cout << std::string("--- Hanami Pet Program Starting ---\n");
    std::string ownerName;
    bool nameIsHanami = false;
    int calculatedValue = 0;
    Pet myDog;
    myDog.setDetails(std::string("Buddy"), 3);
    myDog.introduce();
    std::cout << std::string("What is your name? ");
    std::cin >> ownerName;
    if ((ownerName == std::string("Hanami"))) {
        std::cout << std::string("Welcome, Creator Hanami!\n");
        nameIsHanami = true;
        calculatedValue = 100;
    }
    else if ((ownerName == std::string("Buddy"))) {
        std::cout << std::string("Hey, that's my name!\n");
        calculatedValue = 50;
    }
    else {
        std::cout << std::string("Nice to meet you, ") << ownerName << std::string("!\n");
        nameIsHanami = false;
        calculatedValue = myDog.getAgeInDogYears();
    }
    calculatedValue = (calculatedValue + 5);
    std::cout << std::string("Calculated value: ") << calculatedValue << std::string("\n");
    std::cout << std::string("Is owner Hanami? ") << nameIsHanami << std::string("\n");
    std::cout << std::string("--- Hanami Pet Program Ending ---\n");
    return 0;
}

