class Rose:
    def __init__(self):
      self.secretNumber = 42
      self.isFriendly = True

    def sayHello(self):
        print("Hello from Hanami Rose!\n", end='')
        return


def mainGarden():
    userName = None
    print("What's your name?", end='')
    userName = input() # TODO: Add type conversion if needed (e.g., int(), float())
    g = Rose()
    g.sayHello()
    if (userName == "Rose"):
        print("You have a lovely name!\n", end='')
    elif (userName == "Lily"):
        print("Another beautiful flower name!\n", end='')
    else:
        print("Nice to meet you,", userName, "!\n", end='')
    return 0



if __name__ == "__main__":
    mainGarden()
