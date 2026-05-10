package pt.tecnico.blockchainist.client;

import java.util.ArrayList;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicLong;
import java.util.regex.Pattern;

import io.grpc.Status;
import io.grpc.StatusRuntimeException;
import pt.tecnico.blockchainist.client.grpc.ClientNodeService;
import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.contract.GetBlockchainStateResponse;
import pt.tecnico.blockchainist.contract.ReadBalanceResponse;
import pt.tecnico.blockchainist.contract.TransactionResponse;

/**
 * Class that represents a processor that parses and processes the application's
 * commands
 * Has methods for each operation and to ask input from the user
 */
public class CommandProcessor {

    // Constants defined as valid commands for the application
    private static final String SPACE = " ";
    private static final String CREATE_BLOCKING = "C";
    private static final String CREATE_ASYNC = "c";
    private static final String DELETE_BLOCKING = "E";
    private static final String DELETE_ASYNC = "e";
    private static final String BALANCE_BLOCKING = "S";
    private static final String BALANCE_ASYNC = "s";
    private static final String TRANSFER_BLOCKING = "T";
    private static final String TRANSFER_ASYNC = "t";
    private static final String DEBUG_BLOCKCHAIN_STATE = "B";
    private static final String PAUSE = "P";
    private static final String EXIT = "X";

    private static final Pattern ID_PATTERN = Pattern.compile("^[a-zA-Z0-9]+$");

    private final Long pid = ProcessHandle.current().pid();
    private final AtomicLong commandCounter = new AtomicLong(0);
    private final ArrayList<ClientNodeService> nodes;

    /**
     * Builder for this class, assigns the received nodes to an internal list of
     * nodes
     * 
     * @param nodes list of available nodes for connections
     */
    public CommandProcessor(ArrayList<ClientNodeService> nodes) {
        this.nodes = nodes;
    }

    /**
     * Method that awaits user input and calls other methods accordingly
     */
    void userInputLoop() {

        Scanner scanner = new Scanner(System.in);
        boolean exit = false;

        while (!exit) {
            System.out.print("\n> ");
            String line = scanner.nextLine().trim();
            String[] split = line.split(SPACE);
            try {
                switch (split[0]) {
                    case CREATE_BLOCKING:
                        this.create(split, true);
                        break;

                    case CREATE_ASYNC:
                        this.create(split, false);
                        break;

                    case DELETE_BLOCKING:
                        this.delete(split, true);
                        break;

                    case DELETE_ASYNC:
                        this.delete(split, false);
                        break;

                    case BALANCE_BLOCKING:
                        this.balance(split, true);
                        break;

                    case BALANCE_ASYNC:
                        this.balance(split, false);
                        break;

                    case TRANSFER_BLOCKING:
                        this.transfer(split, true);
                        break;

                    case TRANSFER_ASYNC:
                        this.transfer(split, false);
                        break;

                    case DEBUG_BLOCKCHAIN_STATE:
                        this.debugBlockchainState(split);
                        break;

                    case PAUSE:
                        this.pause(split);
                        break;

                    case EXIT:
                        exit = true;
                        break;

                    default:
                        printUsage();
                        break;
                }
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
                printUsage();
            }
        }
        scanner.close();
    }

    /**
     * Shifts the PID to the left and appends the command counter
     * @return the command number that will serve to uniquely identify the sent transaction
     */
    private Long generateCommandNumber() {
        Long currentCounter = this.commandCounter.incrementAndGet();
        Long commandNumber = pid * 10 * String.valueOf(currentCounter).length() + currentCounter;

        return commandNumber;
    }

    public String formattedCommandNumber() {
        AtomicLong currentCounter = this.commandCounter;

        return currentCounter + " (PID: " + pid + ")";
    }

    /**
     * Prints helpful error messages according to the error's information
     * @param currentNode the number identifying the node with which the exception happened
     * @param e Received StatusRuntimeException
     */
    private void handleError(Integer currentNode, StatusRuntimeException e) {
        if (e.getStatus().getCode() == Status.Code.DEADLINE_EXCEEDED) {
            Debug.log("Something wrong occured with node " + currentNode + " trying next node...");
        } else if (e.getStatus().getCode() == Status.Code.UNAVAILABLE) {
            Debug.log("Node " + currentNode + " is unavailable");
        } else {
            // If fails and is not DEADLINE
            Debug.log("Node " + currentNode + " failed: " + e.getStatus());
        }
    }

    private TransactionResponse execute(String transaction, String userId, String walletId, String sourceUserId,
            String sourceWalletId, String destinationWalletId, int amount,
            Integer nodeIndex, Integer nodeDelay, boolean isBlocking, Long commandNumber) {
        int numNodes = this.nodes.size();

        // Iterate nodes array
        // If current fails, try the next node
        for (int i = 0; i < numNodes; i++) {
            int current = (nodeIndex + i) % numNodes;

            try {
                // Try the current node
                // The first onde will be the node that we received as nodeIndex
                switch (transaction) {
                    case "create":
                        return this.nodes.get(current).createWallet(userId, walletId, nodeDelay, isBlocking,
                                commandNumber);
                    case "delete":
                        return this.nodes.get(current).deleteWallet(userId, walletId, nodeDelay, isBlocking,
                                commandNumber);
                    case "transfer":
                        return this.nodes.get(current).transfer(sourceUserId, sourceWalletId, destinationWalletId,
                                amount, nodeDelay, isBlocking, commandNumber);
                }
            } catch (StatusRuntimeException e) {
                handleError(current, e);
            }
        }

        // If all nodes fail
        return TransactionResponse.newBuilder().setMessage("Failed in all nodes").build();
    }

    /**
     * Method that parses arguments, calls the createWallet method to send the
     * request for this operation, receives the node's response and outputs the
     * state accordingly
     * 
     * @param split      list of arguments received from the user -> the user and
     *                   wallet IDs of the wallet to create
     * @param isBlocking true if the stub is to be blocking, false otherwise
     */
    private void create(String[] split, boolean isBlocking) {
        this.checkCreateCommandArgs(split);

        Long commandNumber = generateCommandNumber();

        String userId = split[1];
        String walletId = split[2];
        Integer nodeIndex = Integer.parseInt(split[3]);
        Integer nodeDelay = Integer.parseInt(split[4]);

        TransactionResponse response = execute("create", userId, walletId, "", "", "", 0, nodeIndex, nodeDelay,
                isBlocking, commandNumber);

        if (response.getMessage().isEmpty()) {
            System.out.println("OK " + formattedCommandNumber());
            Debug.log("Create command completed successfully");
        } else if (response.getMessage().equals("Async returned")) {
            Debug.log("Async command returned");
        } else if (response.getMessage().equals("Failed in all nodes")) {
            Debug.log("Create command failed: " + response.getMessage());
        } else if (response.getMessage().equals("Membership failed")) {
            Debug.log("User does not belong to this organization.");
        } else if (response.getMessage().equals("Signature failed")) {
            Debug.log("The signature was not valid. Please check");
        } else {
            Debug.log("Create command failed: " + response.getMessage());
        }
    }

    /**
     * Method that parses arguments, calls the deleteWallet method to send the
     * request for this operation, receives the node's response and outputs the
     * state accordingly
     * 
     * @param split      list of arguments received from the user -> the user and
     *                   wallet IDs of the wallet to delete
     * @param isBlocking true if the stub is to be blocking, false otherwise
     */
    private void delete(String[] split, boolean isBlocking) {
        this.checkDeleteCommandArgs(split);

        Long commandNumber = generateCommandNumber();

        String userId = split[1];
        String walletId = split[2];
        Integer nodeIndex = Integer.parseInt(split[3]);
        Integer nodeDelay = Integer.parseInt(split[4]);

        TransactionResponse response = execute("delete", userId, walletId, "", "", "", 0, nodeIndex, nodeDelay,
                isBlocking, commandNumber);

        // TransactionResponse response = this.nodes.get(nodeIndex).deleteWallet(userId,
        // walletId, nodeDelay, isBlocking,
        // commandNumber);

        if (response.getMessage().isEmpty()) {
            System.out.println("OK " + formattedCommandNumber());
            Debug.log("Delete command completed successfully");
        } else if (response.getMessage().equals("Async returned")) {
            Debug.log("Async command returned");
        } else if (response.getMessage().equals("Failed in all nodes")) {
            Debug.log("Delete command failed: " + response.getMessage());
        } else {
            Debug.log("Delete command failed: " + response.getMessage());
        }
    }

    /**
     * Method that parses arguments, calls the balance method to send the request
     * for this operation, receives the node's response and outputs the state
     * accordingly
     * 
     * @param split      list of arguments received from the user -> wallet ID to
     *                   get the balance from
     * @param isBlocking true if the stub is to be blocking, false otherwise
     */
    private void balance(String[] split, boolean isBlocking) {
        this.checkBalanceCommandArgs(split);

        Long commandNumber = generateCommandNumber();

        String walletId = split[1];
        Integer nodeIndex = Integer.parseInt(split[2]);
        Integer nodeDelay = Integer.parseInt(split[3]);

        ReadBalanceResponse response = null;
        int numNodes = this.nodes.size();

        // Iterate nodes array
        // If current fails, try the next node
        for (int i = 0; i < numNodes; i++) {
            int current = (nodeIndex + i) % numNodes;
            try {
                response = this.nodes.get(current).readBalance(walletId, nodeDelay, isBlocking, commandNumber);
            } catch (StatusRuntimeException e) {
                handleError(current, e);
            }
        }

        // If all nodes fail
        if (response == null) {
            response = ReadBalanceResponse.newBuilder().setMessage("Failed in all nodes").build();
            Debug.log("The request could not be attended.");
            return;
        }

        // If response.getMessage has nothing, means that the operation occured
        // successfully
        if (response.getMessage().isEmpty()) {
            System.out.println("OK " + formattedCommandNumber());
            // We just want the value of balance, not the whole response
            // (response.toString())
            System.out.println(response.getBalance());
            Debug.log("Balance command completed successfully");
        } else if (response.getMessage().equals("Async returned")) {
            Debug.log("Async command returned");
        } else if (response.getMessage().equals("Failed in all nodes")) {
            Debug.log("Balance command failed: " + response.getMessage());
        } else {
            Debug.log("Balance command failed: " + response.getMessage());
        }
    }

    /**
     * Method that parses arguments, calls the transfer method to send the request
     * for this operation, receives the node's response and outputs the state
     * accordingly
     * 
     * @param split      list of arguments received from the user -> the user and
     *                   wallet IDs involved, the index of the node to connect to
     *                   and the delay before connecting
     * @param isBlocking true if the stub is to be blocking, false otherwise
     */
    private void transfer(String[] split, boolean isBlocking) {
        this.checkTransferCommandArgs(split);

        Long commandNumber = generateCommandNumber();

        String sourceUserId = split[1];
        String sourceWalletId = split[2];
        String destinationWalletId = split[3];
        Long amount = Long.parseLong(split[4]);
        Integer nodeIndex = Integer.parseInt(split[5]);
        Integer nodeDelay = Integer.parseInt(split[6]);

        TransactionResponse response = this.nodes.get(nodeIndex).transfer(sourceUserId, sourceWalletId,
                destinationWalletId, amount, nodeDelay, isBlocking, commandNumber);

        if (response.getMessage().isEmpty()) {
            System.out.println("OK " + formattedCommandNumber());
            Debug.log("Transfer command completed successfully");
        } else if (response.getMessage().equals("Async returned")) {
            Debug.log("Async command returned");
        } else {
            Debug.log("Transfer command failed: " + response.getMessage());
        }
    }

    /**
     * Method that parses arguments, calls the getBlockchainState method to send the
     * request for this operation, receives the node's response and outputs the
     * state accordingly
     * 
     * @param split list of arguments received from the user -> index of node to
     *              connect to
     */
    private void debugBlockchainState(String[] split) {
        this.checkDebugBlockchainStateArgs(split);

        Long commandNumber = generateCommandNumber();

        Integer nodeIndex = Integer.parseInt(split[1]);

        GetBlockchainStateResponse response = this.nodes.get(nodeIndex).getBlockchainState();
        if (response.getMessage().isEmpty()) {
            System.out.println("OK " + formattedCommandNumber());
            Debug.log("Blockchain State command completed successfully");
            for (int i = 0; i < response.getTransactionsCount(); i++) {
                System.out.println(response.getTransactions(i));
            }
        } else {
            Debug.log("Blockchain State command failed: " + response.getMessage());
        }
    }

    /**
     * Method that pauses the client for a certain number of a seconds
     * 
     * @param split list of arguments -> number of seconds to block the program for
     */
    private void pause(String[] split) {
        this.checkPauseArgs(split);

        Integer time;

        time = Integer.parseInt(split[1]);

        try {
            Debug.log("Processing pause command for " + time + " seconds");
            Thread.sleep(time * 1000);
        } catch (InterruptedException e) {
            Debug.log("Pause command interrupted: " + e.getMessage());
            throw new RuntimeException(e);
        }
    }

    /**
     * Method that checks the arguments for the create command
     * 
     * @param split list of arguments received from user
     */
    private void checkCreateCommandArgs(String[] split) {
        // C|c <user_id> <wallet_id> <node_index> <node_delay>
        if (split.length != 5) {
            throw new IllegalArgumentException("Expected 5 arguments, got " + split.length);
        }

        if (!ID_PATTERN.matcher(split[1]).matches()) {
            throw new IllegalArgumentException(
                    "Expected User ID to be composed of ASCII alphanumeric characters, got \"" + split[1] + "\"");
        }

        if (!ID_PATTERN.matcher(split[2]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Wallet ID to be composed of ASCII alphanumeric characters, got \"" + split[2] + "\"");
        }

        try {
            int nodeIndex = Integer.parseInt(split[3]);
            if (nodeIndex < 0 || nodeIndex >= this.nodes.size()) {
                throw new IllegalArgumentException("Node index must be between 0 and " + (this.nodes.size() - 1));
            }
            if (Integer.parseInt(split[4]) < 0) {
                throw new IllegalArgumentException("Node delay cannot be negative");
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected initial balance, node number, and node delay to be integers");
        }
    }

    /**
     * Method that checks the arguments for the delete command
     * 
     * @param split list of arguments received from user
     */
    private void checkDeleteCommandArgs(String[] split) {
        // E|e <user_id> <wallet_id> <node_index> <node_delay>
        if (split.length != 5) {
            throw new IllegalArgumentException("Expected 5 arguments, got " + split.length);
        }

        if (!ID_PATTERN.matcher(split[1]).matches()) {
            throw new IllegalArgumentException(
                    "Expected User ID to be composed of ASCII alphanumeric characters, got \"" + split[1] + "\"");
        }

        if (!ID_PATTERN.matcher(split[2]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Wallet ID to be composed of ASCII alphanumeric characters, got \"" + split[2] + "\"");
        }

        try {
            int nodeIndex = Integer.parseInt(split[3]);
            if (nodeIndex < 0 || nodeIndex >= this.nodes.size()) {
                throw new IllegalArgumentException("Node index must be between 0 and " + (this.nodes.size() - 1));
            }
            if (Integer.parseInt(split[4]) < 0) {
                throw new IllegalArgumentException("Node delay cannot be negative");
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected node number and node delay to be integers");
        }
    }

    /**
     * Method that checks the arguments for the balance command
     * 
     * @param split list of arguments received from user
     */
    private void checkBalanceCommandArgs(String[] split) {
        // S|s <wallet_id> <node_index> <node_delay>
        if (split.length != 4) {
            throw new IllegalArgumentException("Expected 4 arguments, got " + split.length);
        }

        if (!ID_PATTERN.matcher(split[1]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Wallet ID to be composed of ASCII alphanumeric characters, got \"" + split[1] + "\"");
        }

        try {
            int nodeIndex = Integer.parseInt(split[2]);
            if (nodeIndex < 0 || nodeIndex >= this.nodes.size()) {
                throw new IllegalArgumentException("Node index must be between 0 and " + (this.nodes.size() - 1));
            }
            if (Integer.parseInt(split[3]) < 0) {
                throw new IllegalArgumentException("Node delay cannot be negative");
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected node number and node delay to be integers");
        }
    }

    /**
     * Method that checks the arguments for the transfer command
     * 
     * @param split list of arguments received from user
     */
    private void checkTransferCommandArgs(String[] split) {
        // T|t <source_user_id> <source_wallet_id> <destination_wallet_id> <amount>
        // <node_index> <node_delay>
        if (split.length != 7) {
            throw new IllegalArgumentException("Expected 7 arguments, got " + split.length);
        }

        if (!ID_PATTERN.matcher(split[1]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Source User ID to be composed of ASCII alphanumeric characters, got \"" + split[1]
                            + "\"");
        }

        if (!ID_PATTERN.matcher(split[2]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Source Wallet ID to be composed of ASCII alphanumeric characters, got \"" + split[2]
                            + "\"");
        }

        if (!ID_PATTERN.matcher(split[3]).matches()) {
            throw new IllegalArgumentException(
                    "Expected Destination Wallet ID to be composed of ASCII alphanumeric characters, got \"" + split[1]
                            + "\"");
        }

        try {
            if (Long.parseLong(split[4]) < 0) {
                throw new IllegalArgumentException("Amount cannot be negative");
            }
            int nodeIndex = Integer.parseInt(split[5]);
            if (nodeIndex < 0 || nodeIndex >= this.nodes.size()) {
                throw new IllegalArgumentException("Node index must be between 0 and " + (this.nodes.size() - 1));
            }
            if (Integer.parseInt(split[6]) < 0) {
                throw new IllegalArgumentException("Node delay cannot be negative");
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected amount, node number, and node delay to be integers");
        }
    }

    /**
     * Method that checks the arguments for the debugBlockchainState command
     * 
     * @param split list of arguments received from user
     */
    private void checkDebugBlockchainStateArgs(String[] split) {
        // B <node_index>
        if (split.length != 2) {
            throw new IllegalArgumentException("Expected 2 arguments, got " + split.length);
        }

        try {
            int nodeIndex = Integer.parseInt(split[1]);
            if (nodeIndex < 0 || nodeIndex >= this.nodes.size()) {
                throw new IllegalArgumentException("Node index must be between 0 and " + (this.nodes.size() - 1));
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected node index to be an integer");
        }
    }

    /**
     * Method that checks the arguments for the pause command
     * 
     * @param split list of arguments received from user
     */
    private void checkPauseArgs(String[] split) {
        // P <integer>
        if (split.length != 2) {
            throw new IllegalArgumentException("Expected 2 arguments, got " + split.length);
        }

        try {
            if (Integer.parseInt(split[1]) < 0) {
                throw new IllegalArgumentException("Pause time cannot be negative");
            }
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Expected pause time to be an integer");
        }
    }

    /**
     * Method that prints the available commands and their expected arguments
     */
    private static void printUsage() {
        System.err.println("Usage:\n" +
                "- C|c <user_id> <wallet_id> <node_index> <node_delay>\n" +
                "- E|e <user_id> <wallet_id> <node_index> <node_delay>\n" +
                "- S|s <wallet_id> <node_index> <node_delay>\n" +
                "- T|t <source_user_id> <source_wallet_id> <destination_wallet_id> <amount> <node_index> <node_delay>\n"
                +
                "- B <node_index>\n" +
                "- P <integer>\n" +
                "- X\n");
    }
}
