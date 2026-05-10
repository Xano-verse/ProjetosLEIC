package pt.tecnico.blockchainist.sequencer;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.contract.Transaction;

public class Block {

    private List<Transaction> transactions;
    private volatile boolean isOpen; // volatile is important so all threads always know the updated value

    /**
     * Constructor of Block's class
     * @param transaction the first transaction that opens the block
     * @param maxTransactions the maximum number of transactions the block can have
     * @param timer the amount of time until the block closes automatically
     */
    public Block(Transaction transaction, int maxTransactions, int timer) {
        this.transactions = new ArrayList<>();
        addTransaction(transaction, maxTransactions);

        this.isOpen = true;
        Debug.log("Opened block with transaction " + transaction);

        ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
        Debug.log("Started block timer");
        scheduler.schedule(this::close, timer, TimeUnit.SECONDS);

    }

    /**
     * @return the transactions of this block
     */
    public List<Transaction> getTransactions() {
        return transactions;
    }

    /**
     * @return this block's state
     */
    public boolean getIsOpen() {
        return isOpen;
    }

    /**
     * Adds a transaction to the block, checking if it is the last allowed transaction
     * If so, it also closes the block
     * @param transaction the transaction to be added
     * @param maxTransactions the maximum number of allowed transactions
     */
    public synchronized void addTransaction(Transaction transaction, int maxTransactions) {

        this.transactions.add(transaction);
        Debug.log("Added transaction " + transaction + " to block");

        // As soon as the block is full, close it
        if (this.transactions.size() == maxTransactions) {
            close();
        }
    }

    public synchronized void close() {
        isOpen = false;
        Debug.log("Closed block " + transactions);
        notifyAll();
    }

}