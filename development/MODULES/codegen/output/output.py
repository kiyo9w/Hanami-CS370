class Pet:
    def __init__(self):
      self.name = "Unknown"
      self.age = 0

    def setDetails(self, newName, newAge):
        self.name = newName
        self.age = newAge
        print("Pet details updated: Name=", self.name, ", Age=", self.age, "\n", end='')
        return

    def introduce(self):
        print("Woof! My name is ", self.name, " and I am ", self.age, " year(s) old.\n", end='')
        return

    def getAgeInDogYears(self):
        return (self.age * 0)


def mainGarden():
    print("--- Hanami Pet Program Starting ---\n", end='')
    ownerName = None
    nameIsHanami = False
    calculatedValue = 0
    myDog = None
    myDog.setDetails("Buddy", 0)
    myDog.introduce()
    print("What is your name? ", end='')
    ownerName = input()
    if (ownerName == "Hanami"):
        print("Welcome, Creator Hanami!\n", end='')
        nameIsHanami = True
        calculatedValue = 0
    elif (ownerName == "Buddy"):
        print("Hey, that's my name!\n", end='')
        calculatedValue = 0
    else:
        print("Nice to meet you, ", ownerName, "!\n", end='')
        nameIsHanami = False
        calculatedValue = myDog.getAgeInDogYears()
    calculatedValue = (calculatedValue + 0)
    print("Calculated value: ", calculatedValue, "\n", end='')
    print("Is owner Hanami? ", nameIsHanami, "\n", end='')
    print("--- Hanami Pet Program Ending ---\n", end='')
    return 0



if __name__ == "__main__":
    mainGarden() # Or main() depending on the Hanami code
