package pt.tecnico.blockchainist.node.domain;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import pt.tecnico.blockchainist.contract.CreateWalletRequest;
import pt.tecnico.blockchainist.contract.DeleteWalletRequest;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.contract.TransferRequest;

/**
 * Class that represents the state of a node at a given time
 * Has the node's information, its local blockchain and methods to operate on
 * the node's blockchain
 */
public class NodeState {

    // Constants for the central bank wallet
    // This is a special wallet that is created at the initialization of the node
    private static final String CENTRAL_BANK_USER_ID = "BC";
    private static final String CENTRAL_BANK_WALLET_ID = "bc";
    private static final long CENTRAL_BANK_INITIAL_BALANCE = 1000L;

    private static final Map<String, List<String>> membershipList = new HashMap<>();

    /**
     * Class that represents a wallet
     * It is private to the Node State's existence
     */
    private static final class Wallet {
        private final String userId;
        private long balance;

        private Wallet(String userId, long balance) {
            this.userId = userId;
            // balance mustn't be negative
            if (balance < 0L) {
                throw new IllegalArgumentException("Balance cannot be negative");
            }
            this.balance = balance;
        }
    }

    // Here we store the global state of the app: wallets (with 'bc'),
    // the users that own them and the balance of each wallet
    private Map<String, Wallet> wallets = new HashMap<>();
    private List<List<Transaction>> blockchain = new ArrayList<>();

    public NodeState() {
        this.wallets.put(CENTRAL_BANK_WALLET_ID, new Wallet(CENTRAL_BANK_USER_ID, CENTRAL_BANK_INITIAL_BALANCE));

        // initialize the membership with the known users and organizations
        this.membershipList.put("OrgA", List.of("BC", "Alice", "Bob", "Charlie"));
        this.membershipList.put("OrgB", List.of("David", "Emma", "Fred"));
        this.membershipList.put("OrgC", List.of("Ginger", "Henry", "Iris"));
    }

    public Map<String, List<String>> getMembershipList() {
        return this.membershipList;
    }

    public void setBlockchain(List<List<Transaction>> blockchain) {
        if (blockchain == null) {
            return;
        }
        this.blockchain = new ArrayList<>(blockchain);
    }

    public String getBcUserId() {
        return CENTRAL_BANK_USER_ID;
    }

    public String getBcWalletId() {
        return CENTRAL_BANK_WALLET_ID;
    }

    public long getBcInitialBalance() {
        return CENTRAL_BANK_INITIAL_BALANCE;
    }

    // get blockchain
    public List<List<Transaction>> getBlockchain() {
        return blockchain;
    }

    public void addBlockToLocalBlockchain(List<Transaction> block) {
        this.blockchain.add(block);
    }

    public void parseTransaction(Transaction transaction) {
        switch (transaction.getOperationCase()) {
            case CREATE_WALLET:
                CreateWalletRequest createWalletRequest = transaction.getCreateWallet();
                createWallet(createWalletRequest.getUserId(), createWalletRequest.getWalletId());
                break;
            case DELETE_WALLET:
                DeleteWalletRequest deleteWalletRequest = transaction.getDeleteWallet();
                deleteWallet(deleteWalletRequest.getUserId(), deleteWalletRequest.getWalletId());
                break;
            case TRANSFER:
                TransferRequest transferRequest = transaction.getTransfer();
                transfer(transferRequest.getSrcUserId(), transferRequest.getSrcWalletId(),
                        transferRequest.getDstWalletId(), transferRequest.getValue());
                break;
        }
    }

    /**
     * Create a new wallet which belong to the given user.
     * This method should fail if the user already has a wallet with the same id
     * 
     * @param userId
     * @param walletId
     */
    public synchronized void createWallet(String userId, String walletId) {

        if (this.wallets.containsKey(walletId)) {
            throw new IllegalArgumentException("Wallet already exists: " + walletId);
        }

        this.wallets.put(walletId, new Wallet(userId, 0L));
    }

    /**
     * Delete an existing wallet which belong to the given user.
     * This method should fail if the wallet does not exist, if the balance is
     * different from zero or if the wallet does not belong to the user.
     * 
     * @param userId
     * @param walletId
     */
    public synchronized void deleteWallet(String userId, String walletId) {

        // If the wallet does not exist, getExistingWallet will throw an exception
        Wallet wallet = getExistingWallet(walletId);

        if (!wallet.userId.equals(userId)) {
            throw new IllegalArgumentException("Wallet does not belong to user");
        }
        if (wallet.balance != 0L) {
            throw new IllegalArgumentException("Wallet balance must be zero to delete");
        }

        this.wallets.remove(walletId);
    }

    /**
     * Transfer from the wallet srcwalletID which belongs to user srcUserID,
     * to the wallet dstWalletID, an amount of cryptocurrency units.
     * This method should fail if:
     * - any of the wallets does not exist;
     * - if the source wallet does not belong to the user;
     * - if the source balance is insufficient;
     * - if the amount is not positive.
     * 
     * @param srcUserId
     * @param srcWalletId
     * @param dstWalletId
     * @param amount
     */
    public synchronized void transfer(String srcUserId, String srcWalletId, String dstWalletId, Long amount) {
        Wallet src = getExistingWallet(srcWalletId);
        Wallet dst = getExistingWallet(dstWalletId);

        if (!src.userId.equals(srcUserId)) {
            throw new IllegalArgumentException("Source wallet does not belong to user");
        }
        if (src.balance < amount) {
            throw new IllegalArgumentException("Insufficient funds");
        }

        src.balance -= amount;
        dst.balance += amount;
    }

    /**
     * Read the balance of the wallet with the given id.
     * 
     * @param walletId
     * @return the balance of the wallet
     */
    public synchronized long readBalance(String walletId) {
        return getExistingWallet(walletId).balance;
    }

    /**
     * Get the list of transactions that have been delivered to this node.
     * 
     * @return the list of transactions
     */
    public synchronized List<List<Transaction>> getBlockchainState() {
        return this.blockchain;
    }

    /**
     * Method to make sure the ID provided is not blank
     * 
     * @param value
     * @param fieldName
     */
    public void requireNonBlankId(String value, String fieldName) {
        if (value == null || value.isBlank()) {
            throw new IllegalArgumentException(fieldName + " is required");
        }
    }

    /**
     * Get a given wallet through its ID
     * 
     * @param walletId
     * @return
     */
    private Wallet getExistingWallet(String walletId) {
        Wallet wallet = this.wallets.get(walletId);
        if (wallet == null) {
            System.err.println(wallets);
            throw new IllegalArgumentException("Wallet does not exist: " + walletId);
        }
        return wallet;
    }
}
