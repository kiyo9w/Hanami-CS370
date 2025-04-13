// Generated Hanami Code (JavaScript)

// Garden: TestGarden
class Pet {
    constructor() {
      this.name = "Unknown";
      this.age = 0;
    }

    setDetails(newName, newAge) {
        name = newName;
        age = newAge;
        console.log("Pet details updated: Name=" + name + ", Age=" + age + "\n");
        return;
    }

    introduce() {
        console.log("Woof! My name is " + name + " and I am " + age + " year(s) old.\n");
        return;
    }

    getAgeInDogYears() {
        return (age * 0);
    }

}

function mainGarden() {
    console.log("--- Hanami Pet Program Starting ---\n");
    let ownerName = undefined;
    let nameIsHanami = false;
    let calculatedValue = 0;
    let myDog = undefined;
    myDog.setDetails("Buddy", 0);
    myDog.introduce();
    console.log("What is your name? ");
    // Basic input using prompt:
    ownerName = prompt(); // Reads as string
    if ((ownerName === "Hanami")) {
        console.log("Welcome, Creator Hanami!\n");
        nameIsHanami = true;
        calculatedValue = 0;
    }
    else if ((ownerName === "Buddy")) {
        console.log("Hey, that's my name!\n");
        calculatedValue = 0;
    }
    else {
        console.log("Nice to meet you, " + ownerName + "!\n");
        nameIsHanami = false;
        calculatedValue = myDog.getAgeInDogYears();
    }
    calculatedValue = (calculatedValue + 0);
    console.log("Calculated value: " + calculatedValue + "\n");
    console.log("Is owner Hanami? " + nameIsHanami + "\n");
    console.log("--- Hanami Pet Program Ending ---\n");
    return 0;
}

