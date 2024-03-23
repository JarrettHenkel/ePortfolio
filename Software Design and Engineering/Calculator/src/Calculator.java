import java.util.InputMismatchException;
import java.util.Scanner;

public class Calculator {

    private static Scanner scanner = new Scanner(System.in); // Scanner for input

    public static void main(String[] args) {
        int operation;

        // Loop until '0' is entered
        do {
            operation = showMenu(); // Show operation menu
            processOperation(operation); // Process the user's chosen operation
        } while (operation != 0);

        System.out.println("Program Finished"); // End program message
        scanner.close(); // Close scanner
    }

    // Display the calculator's main menu
    private static int showMenu() {
        System.out.println("\nSelect operation:");
        System.out.println("1. Addition (+)");
        System.out.println("2. Subtraction (-)");
        System.out.println("3. Multiplication (*)");
        System.out.println("4. Division (/)");
        System.out.println("5. Nth Root (√)");
        System.out.println("6. Exponential (^)");
        System.out.println("7. Logarithmic (log)");
        System.out.println("8. Trigonometric Functions");
        System.out.println("0. Quit Application.");
        return readInt(); // Read user selection
    }

    // Process the operation selected by the user
    private static void processOperation(int operation) {
        switch (operation) {
            case 1:
                processAddition();
                break;
            case 2:
                processSubtraction();
                break;
            case 3:
                processMultiplication();
                break;
            case 4:
                processDivision();
                break;
            case 5:
                processNthRoot();
                break;
            case 6:
                processExponential();
                break;
            case 7:
                processLogarithmicOperation();
                break;
            case 8:
                showTrigMenu();
                break;
            case 0:
                System.out.println("Program Finished");
                break;
            default:
                System.out.println("Invalid operation.");
                break;
        }
    }

    // Process case 1 for processOperation
    private static void processAddition() {
        System.out.println("Enter the first value:");
        double op1 = readDouble();
        System.out.println("Enter the second value:");
        double op2 = readDouble();
        System.out.println(op1 + " + " + op2 + " = " + (op1 + op2));
    }

    // Process case 2 for processOperation
    private static void processSubtraction() {
        System.out.println("Enter the first value:");
        double op1 = readDouble();
        System.out.println("Enter the second value:");
        double op2 = readDouble();
        System.out.println(op1 + " - " + op2 + " = " + (op1 - op2));
    }

    // Process case 3 for processOperation
    private static void processMultiplication() {
        System.out.println("Enter the first factor:");
        double op1 = readDouble();
        System.out.println("Enter the second factor:");
        double op2 = readDouble();
        System.out.println(op1 + " * " + op2 + " = " + (op1 * op2));
    }

    // Process case 4 for processOperation
    private static void processDivision() {
        System.out.println("Enter the dividend:");
        double op1 = readDouble();
        System.out.println("Enter the divisor:");
        double op2 = readDouble();
        if (op2 != 0) {
            System.out.println(op1 + " / " + op2 + " = " + (op1 / op2));
        } else {
            System.out.println("Cannot divide by zero.");
        }
    }

    // Process case 5 for processOperation
    private static void processNthRoot() {
        System.out.println("Enter the degree of the root:");
        double op1 = readDouble();
        System.out.println("Enter the radicand:");
        double op2 = readDouble();
        if (op1 > 0 && op2 >= 0) {
            System.out.println(op1 + "√" + op2 + " = " + (Math.pow(op2, 1 / op1)));
        } else {
            System.out.println("Invalid input for Nth root.");
        }
    }

    // Process case 6 for processOperation
    private static void processExponential() {
        System.out.println("Enter the base:");
        double op1 = readDouble();
        System.out.println("Enter the exponent:");
        double op2 = readDouble();
        System.out.println(op1 + " ^ " + op2 + " = " + (Math.pow(op1, op2)));
    }

    // Process case 7 for processOperation
    private static void processLogarithmicOperation() {
        System.out.println("Enter the base of the logarithm (use 'e' for the natural log):");
        scanner.nextLine(); // Clearing buffer
        String baseInput = scanner.nextLine();
        double op2, base;
        if (baseInput.equalsIgnoreCase("e")) {
            System.out.println("Enter the number for which you want the natural logarithm:");
            op2 = readDouble();
            System.out.println("ln(" + op2 + ") = " + Math.log(op2));
        } else {
            try {
                base = Double.parseDouble(baseInput);
                System.out.println("Enter the number for which you want the logarithm:");
                op2 = readDouble();
                if (base > 0 && op2 > 0 && base != 1) {
                    System.out.println("log_" + base + "(" + op2 + ") = " + (Math.log(op2) / Math.log(base)));
                } else {
                    System.out.println("Invalid input for logarithm.");
                }
            } catch (NumberFormatException e) {
                System.out.println("Invalid input for logarithm base.");
            }
        }
    }

    // Process case 7 for processOperation -> Shows menu for trigonometric functions
    private static void showTrigMenu() {
        System.out.println("\nSelect Trigonometric Function:");
        System.out.println("1. Sine (sin)");
        System.out.println("2. Cosine (cos)");
        System.out.println("3. Tangent (tan)");
        System.out.println("4. Cotangent (cot)");
        System.out.println("5. Secant (sec)");
        System.out.println("6. Cosecant (csc)");
        System.out.println("0. Return to Main Menu.");

        int trigOperation = readInt(); // Read trig function selection
        processTrigOperation(trigOperation); // Process the selected trig function
    }

    // Process the selected trigonometric operation
    private static void processTrigOperation(int trigOperation) {
        System.out.println("Enter the angle:");
        double angle = readDouble(); // Read angle
        System.out.println("Is this angle in radians or degrees?");
        System.out.println("1. Radians");
        System.out.println("2. Degrees");
        int unit = readInt(); // Read unit of angle

        double angleInRadians = angle; // Initialize angle in radians
        if (unit == 2) { // Convert to radians if needed
            angleInRadians = Math.toRadians(angle);
        }

        // Specify angle unit in output
        String angleInput = angle + (unit == 1 ? " radians" : " degrees");
        performTrigOperation(trigOperation, angleInRadians, angleInput); // Perform trig operation
    }

    // Perform and display the result of the trigonometric operation
    private static void performTrigOperation(int trigOperation, double angleInRadians, String angleInput) {
        switch (trigOperation) {
            case 1:
                System.out.println("sin(" + angleInput + ") = " + Math.sin(angleInRadians));
                break;
            case 2:
                System.out.println("cos(" + angleInput + ") = " + Math.cos(angleInRadians));
                break;
            case 3:
                System.out.println("tan(" + angleInput + ") = " + Math.tan(angleInRadians));
                break;
            case 4:
                if (Math.tan(angleInRadians) != 0) {
                    System.out.println("cot(" + angleInput + ") = " + (1 / Math.tan(angleInRadians)));
                } else {
                    System.out.println("cot(" + angleInput + ") is undefined");
                }
                break;
            case 5:
                if (Math.cos(angleInRadians) != 0) {
                    System.out.println("sec(" + angleInput + ") = " + (1 / Math.cos(angleInRadians)));
                } else {
                    System.out.println("sec(" + angleInput + ") is undefined");
                }
                break;
            case 6:
                if (Math.sin(angleInRadians) != 0) {
                    System.out.println("csc(" + angleInput + ") = " + (1 / Math.sin(angleInRadians)));
                } else {
                    System.out.println("csc(" + angleInput + ") is undefined");
                }
                break;
            case 0:
                break;
            default:
                System.out.println("Invalid Trigonometric Operation.");
                break;
        }
    }

    // Reads an integer safely from the user
    private static int readInt() {
        while (true) {
            try {
                return scanner.nextInt(); // Return scanned int
            } catch (InputMismatchException e) {
                System.out.println("Invalid input. Please enter a valid integer."); // Handle non-integer input
                scanner.nextLine(); // Clear buffer
            }
        }
    }

    // Reads a double safely from the user
    private static double readDouble() {
        while (true) {
            try {
                return scanner.nextDouble(); // Return scanned double
            } catch (InputMismatchException e) {
                System.out.println("Invalid input. Please enter a valid number."); // Handle non-double input
                scanner.nextLine(); // Clear buffer
            }
        }
    }
}