// Generated Hanami Code (JavaScript)

// Garden: SimpleGarden
class Rose {
    constructor() {
      this.secretNumber = 42;
      this.isFriendly = true;
    }

    sayHello() {
        console.log("Hello from Hanami Rose!\n");
        return;
    }

}

function mainGarden() {
    let userName = undefined;
    console.log("What's your name?");
    // Basic input using prompt:
    userName = parseFloat(prompt()); // Reads string, parses to float
    let g = new Rose();
    g.sayHello();
    if ((userName === "Rose")) {
        console.log("You have a lovely name!\n");
    }
    else if ((userName === "Lily")) {
        console.log("Another beautiful flower name!\n");
    }
    else {
        console.log("Nice to meet you," + userName + "!\n");
    }
    return 0;
}

