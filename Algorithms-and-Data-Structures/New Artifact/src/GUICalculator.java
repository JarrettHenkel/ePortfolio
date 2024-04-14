import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Scanner;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;

class BSTNode {
    CalculationRecord record;
    BSTNode left, right;

    public BSTNode(CalculationRecord record) {
        this.record = record;
        this.left = null;
        this.right = null;
    }
}

class BinarySearchTree {
    private BSTNode root;
    private Comparator<CalculationRecord> comparator;

    public BinarySearchTree(Comparator<CalculationRecord> comp) {
        this.comparator = comp;
    }

    public void insert(CalculationRecord record) {
        root = insertRec(root, record);
    }

    private BSTNode insertRec(BSTNode root, CalculationRecord record) {
        if (root == null) {
            root = new BSTNode(record);
            return root;
        }
        int cmp = comparator.compare(record, root.record);
        if (cmp < 0) {
            root.left = insertRec(root.left, record);
        } else if (cmp > 0) {
            root.right = insertRec(root.right, record);
        }
        return root;
    }

    public void remove(CalculationRecord record) {
        root = removeRec(root, record);
    }

    private BSTNode removeRec(BSTNode root, CalculationRecord record) {
        if (root == null) return root;

        int cmp = comparator.compare(record, root.record);
        if (cmp < 0) {
            root.left = removeRec(root.left, record);
        } else if (cmp > 0) {
            root.right = removeRec(root.right, record);
        } else {
            if (root.left == null)
                return root.right;
            else if (root.right == null)
                return root.left;

            root.record = minValue(root.right);
            root.right = removeRec(root.right, root.record);
        }
        return root;
    }

    private CalculationRecord minValue(BSTNode root) {
        CalculationRecord minv = root.record;
        while (root.left != null) {
            minv = root.left.record;
            root = root.left;
        }
        return minv;
    }

    public void inorderTraversal(ArrayList<CalculationRecord> sortedList) {
        inorderRec(root, sortedList);
    }

    private void inorderRec(BSTNode root, ArrayList<CalculationRecord> sortedList) {
        if (root != null) {
            inorderRec(root.left, sortedList);
            sortedList.add(root.record);
            inorderRec(root.right, sortedList);
        }
    }
}

class CalculationRecord {
    String operation;
    double operand1;
    double operand2;
    double result;
    Instant timestamp;

    public CalculationRecord(String operation, double operand1, double operand2, double result) {
        this.operation = operation;
        this.operand1 = operand1;
        this.operand2 = operand2;
        this.result = result;
        this.timestamp = Instant.now();
    }

    public CalculationRecord(String operation, double operand1, double operand2, double result, Instant timestamp) {
        this.operation = operation;
        this.operand1 = operand1;
        this.operand2 = operand2;
        this.result = result;
        this.timestamp = timestamp;
    }

    public String toCSV() {
        DateTimeFormatter formatter = DateTimeFormatter.ISO_INSTANT;
        return String.format("%s,%f,%f,%f,%s", operation, operand1, operand2, result, formatter.format(timestamp));
    }

    public static CalculationRecord fromCSV(String csv) {
        String[] parts = csv.split(",");
        return new CalculationRecord(parts[0], Double.parseDouble(parts[1]), Double.parseDouble(parts[2]), Double.parseDouble(parts[3]), Instant.parse(parts[4]));
    }

    @Override
    public String toString() {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss").withZone(ZoneId.systemDefault());
        return String.format("%s: %s (%.2f and %.2f) = %.2f", formatter.format(timestamp), operation, operand1, operand2, result);
    }
}

public class GUICalculator extends JFrame implements ActionListener {
    private JTextField display;
    private ArrayList<CalculationRecord> memory = new ArrayList<>();
    private double operand1 = Double.NaN;
    private String currentOp = "";
    private JTextArea textArea;
    private JDialog memoryDialog;
    private static final String FILENAME = "calculator_memory.csv";
    private JDialog trigDialog;
    private BinarySearchTree memoryTree;
    private ArrayList<CalculationRecord> memoryList = new ArrayList<>();

    // Sorting flags
    private boolean sortAscendingDate = true;
    private boolean sortAscendingOP1 = true;
    private boolean sortAscendingOP2 = true;
    private boolean sortAscendingSum = true;
    private boolean sortAscendingOperation = true;

    public GUICalculator() {
        setDefaultComparator();  
        loadMemoryFromFile();    
        createGUI();
        prepareMemoryDialog();
    }

    private void setDefaultComparator() {
        // Default sorting by result
        Comparator<CalculationRecord> comp = Comparator.comparingDouble((CalculationRecord r) -> r.result);
        memoryTree = new BinarySearchTree(comp);
    }

    private void changeComparator(Comparator<CalculationRecord> newComp) {
        BinarySearchTree newTree = new BinarySearchTree(newComp);
        memoryTree.inorderTraversal(memoryList);
        for (CalculationRecord rec : memoryList) {
            newTree.insert(rec);
        }
        memoryTree = newTree;
        updateMemoryText();
    }

    private void createGUI() {
        setTitle("Calculator");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(480, 640);
        setLayout(new BorderLayout());

        display = new JTextField();
        display.setEditable(false);
        display.setHorizontalAlignment(JTextField.RIGHT);
        display.setPreferredSize(new Dimension(0, 70));
        add(display, BorderLayout.NORTH);
    
        // Button grid panel
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new GridLayout(5, 4, 5, 5)); 
    
        // Buttons in the main calculator area
        String[] buttons = {
            "1", "2", "3", "+",
            "4", "5", "6", "-",
            "7", "8", "9", "*",
            "Clear", "0", "=", "/",
            "Root", "Exponent", "Logorithm", "Trigonometric"
        };
    
        for (String b : buttons) {
            JButton button = new JButton(b);
            button.addActionListener(this);
            buttonPanel.add(button);
        }
        add(buttonPanel, BorderLayout.CENTER);
    
        // Bottom panel with memory and control buttons
        JPanel bottomPanel = new JPanel();
        bottomPanel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
        String[] controlButtons = {"Mem+", "Mem-", "View Mem", "Clear Mem", "Quit"};
        for (String b : controlButtons) {
            JButton button = new JButton(b);
            button.addActionListener(this);
            bottomPanel.add(button);
        }
        add(bottomPanel, BorderLayout.SOUTH);

        setVisible(true);
    }

    private void sortMemory(String sortType) {
        switch (sortType) {
            case "Date":
                if (sortAscendingDate) {
                    changeComparator(Comparator.comparing((CalculationRecord r) -> r.timestamp));
                } else {
                    changeComparator(Comparator.comparing((CalculationRecord r) -> r.timestamp).reversed());
                }
                sortAscendingDate = !sortAscendingDate;
                break;
            case "OP1":
                if (sortAscendingOP1) {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.operand1));
                } else {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.operand1).reversed());
                }
                sortAscendingOP1 = !sortAscendingOP1;
                break;
            case "OP2":
                if (sortAscendingOP2) {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.operand2));
                } else {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.operand2).reversed());
                }
                sortAscendingOP2 = !sortAscendingOP2;
                break;
            case "Sum":
                if (sortAscendingSum) {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.result));
                } else {
                    changeComparator(Comparator.comparingDouble((CalculationRecord r) -> r.result).reversed());
                }
                sortAscendingSum = !sortAscendingSum;
                break;
            case "Operation":
                if (sortAscendingOperation) {
                    changeComparator(Comparator.comparing((CalculationRecord r) -> r.operation));
                } else {
                    changeComparator(Comparator.comparing((CalculationRecord r) -> r.operation).reversed());
                }
                sortAscendingOperation = !sortAscendingOperation;
                break;
        }
    }

    private void prepareTrigDialog() {
        if (trigDialog == null) {
            trigDialog = new JDialog(this, "Trigonometric Functions in Degrees", Dialog.ModalityType.APPLICATION_MODAL);
            trigDialog.setSize(300, 200);
            trigDialog.setLocationRelativeTo(this);
            
            JPanel trigPanel = new JPanel(new GridLayout(2, 3, 5, 5));
            String[] functions = {"Sin", "Cos", "Tan", "Csc", "Sec", "Cot"};
            
            for (String func : functions) {
                JButton funcButton = new JButton(func);
                funcButton.addActionListener(this);
                trigPanel.add(funcButton);
            }
        
            trigDialog.add(trigPanel);
        }
    }

    public void actionPerformed(ActionEvent e) {
        String command = e.getActionCommand();
        try {
            switch (command) {
                case "Quit":
                    saveMemoryToFile();
                    System.exit(0);
                    break;
                case "Clear":
                    display.setText("");
                    operand1 = Double.NaN;
                    currentOp = "";
                    break;
                case "=":
                    if (!Double.isNaN(operand1) && !currentOp.isEmpty() && !display.getText().isEmpty()) {
                        String[] parts = display.getText().split(" ");
                        if (parts.length >= 2) {
                            double operand2 = Double.parseDouble(parts[2]);  
                            double result = calculateResult(operand1, operand2, currentOp);
                            display.setText(String.valueOf(result));
                            memoryTree.insert(new CalculationRecord(currentOp, operand1, operand2, result));  
                            operand1 = Double.NaN;
                            currentOp = "";
                        }
                    }
                    break;
                case "+":
                case "-":
                case "*":
                case "/":
                case "Exponent":
                case "Root":
                case "Logorithm":
                    if (!display.getText().isEmpty()) {
                        operand1 = Double.parseDouble(display.getText());
                        currentOp = command;
                        display.setText(display.getText() + " " + command + " ");
                    }
                    break;
                case "Trigonometric":
                    prepareTrigDialog();
                    trigDialog.setVisible(true);
                    break;
                case "Sin":
                case "Cos":
                case "Tan":
                case "Csc":
                case "Sec":
                case "Cot":
                    if (!display.getText().isEmpty()) {
                        double operand = Double.parseDouble(display.getText());
                        double result = calculateTrigFunction(operand, command);
                        display.setText(String.valueOf(result));
                        trigDialog.setVisible(false); 
                    }
                break;
                case "Mem+":
                    if (!display.getText().isEmpty()) {
                        double result = Double.parseDouble(display.getText());
                        if (!currentOp.isEmpty() && !Double.isNaN(operand1)) {
                            memoryTree.insert(new CalculationRecord(currentOp, operand1, result - operand1, result)); 
                            updateMemoryText();
                            display.setText("");
                        }
                    }
                    break;
                case "Mem-":
                    if (!display.getText().isEmpty()) {
                        double result = Double.parseDouble(display.getText());
                        CalculationRecord toRemove = findRecordByResult(result);
                        if (toRemove != null) {
                            memoryTree.remove(toRemove);
                            updateMemoryText();
                            display.setText("");
                        }
                    }
                    break;
                case "View Mem":
                    showMemoryDialog();
                    break;
                case "Clear Mem":
                    memory.clear();
                    updateMemoryText();
                    break;
                case "Sort Date":
                case "Sort OP1":
                case "Sort OP2":
                case "Sort Sum":
                case "Sort Operation":
                    sortMemory(command.substring(5)); 
                    updateMemoryText();
                    break;
                default:
                    if (!currentOp.isEmpty() && !Double.isNaN(operand1)) {
                        display.setText(operand1 + " " + currentOp + " " + command);
                    } else {
                        display.setText(display.getText() + command);
                    }
                    break;
            }
        } catch (NumberFormatException ex) {
            display.setText("Error");
            operand1 = Double.NaN;
            currentOp = "";
        }
    }

    private double calculateResult(double op1, double op2, String operation) {
        switch (operation) {
            case "+":
                return op1 + op2;
            case "-":
                return op1 - op2;
            case "*":
                return op1 * op2;
            case "/":
                return op2 == 0 ? Double.NaN : op1 / op2;
            case "Exponent":
                return Math.pow(op1, op2);
            case "Root":
                return Math.pow(op2, 1.0 / op1);
            case "Logorithm":  
                if (op1 <= 0 || op1 == 1 || op2 <= 0) {
                    return Double.NaN;  
                }
                return Math.log(op2) / Math.log(op1);
            default:
                return Double.NaN;
        }
    }

    private double calculateTrigFunction(double operand, String function) {
        switch (function) {
            case "Sin":
                return Math.sin(Math.toRadians(operand));
            case "Cos":
                return Math.cos(Math.toRadians(operand));
            case "Tan":
                return Math.tan(Math.toRadians(operand));
            case "Csc":
                return 1.0 / Math.sin(Math.toRadians(operand));
            case "Sec":
                return 1.0 / Math.cos(Math.toRadians(operand));
            case "Cot":
                return 1.0 / Math.tan(Math.toRadians(operand));
            default:
                return Double.NaN;
        }
    }

    private CalculationRecord findRecordByResult(double result) {
        ArrayList<CalculationRecord> records = new ArrayList<>();
        memoryTree.inorderTraversal(records);
        for (CalculationRecord record : records) {
            if (Math.abs(record.result - result) < 0.00001) {  
                return record;
            }
        }
        return null;
    }

    private void prepareMemoryDialog() {
        if (memoryDialog == null) {
            memoryDialog = new JDialog(this, "Memory Records", Dialog.ModalityType.MODELESS);
            memoryDialog.setSize(480, 400);
            memoryDialog.setLocationRelativeTo(this);
    
            // Ribbon Panel for sorting buttons
            JPanel ribbonPanel = new JPanel(new GridLayout(1, 5));
            String[] labels = {"Sort Date", "Sort Operation", "Sort OP1", "Sort OP2", "Sort Sum"};
            for (String label : labels) {
                JButton button = new JButton(label);
                button.addActionListener(this);
                ribbonPanel.add(button);
            }
    
            textArea = new JTextArea();
            textArea.setEditable(false);
            JScrollPane scrollPane = new JScrollPane(textArea);
            scrollPane.setPreferredSize(new Dimension(280, 350));
    
            memoryDialog.setLayout(new BorderLayout());
            memoryDialog.add(ribbonPanel, BorderLayout.NORTH);
            memoryDialog.add(scrollPane, BorderLayout.CENTER);
        }
    }

    private void showMemoryDialog() {
        updateMemoryText();
        memoryDialog.setVisible(true);
    }

    private void updateMemoryText() {
        ArrayList<CalculationRecord> sortedRecords = new ArrayList<>();
        memoryTree.inorderTraversal(sortedRecords);
        textArea.setText("");
        for (CalculationRecord record : sortedRecords) {
            textArea.append(record.toString() + "\n");
        }
    }

    private void loadMemoryFromFile() {
        Path path = Paths.get(FILENAME);
        if (Files.exists(path)) {
            try (Scanner scanner = new Scanner(path)) {
                while (scanner.hasNextLine()) {
                    String line = scanner.nextLine();
                    CalculationRecord record = CalculationRecord.fromCSV(line);
                    memoryTree.insert(record);  
                }
            } catch (IOException e) {
                System.out.println("Error reading the file: " + e.getMessage());
            }
        } else {
            System.out.println("No previous memory found.");
        }
    }

    private void saveMemoryToFile() {
        try (PrintWriter out = new PrintWriter(new FileWriter(FILENAME))) {
            ArrayList<CalculationRecord> records = new ArrayList<>();
            memoryTree.inorderTraversal(records);  
            for (CalculationRecord record : records) {
                out.println(record.toCSV());
            }
        } catch (IOException e) {
            JOptionPane.showMessageDialog(this, "Error saving memory records: " + e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new GUICalculator());
    }
}