package pt.tecnico.blockchainist.node;

import static io.grpc.Status.INTERNAL;
import static io.grpc.Status.INVALID_ARGUMENT;
import static io.grpc.Status.UNAVAILABLE;

import java.security.NoSuchAlgorithmException;
import java.security.InvalidKeyException;
import java.security.SignatureException;
import java.security.PublicKey;
import java.util.ArrayList;
import java.util.List;

import io.grpc.StatusRuntimeException;
import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.common.Encryption;
import pt.tecnico.blockchainist.contract.BroadcastResponse;
import pt.tecnico.blockchainist.contract.DeliverBlockResponse;
import pt.tecnico.blockchainist.contract.GetBlockchainStateRequest;
import pt.tecnico.blockchainist.contract.GetBlockchainStateResponse;
import pt.tecnico.blockchainist.contract.NodeServiceGrpc;
import pt.tecnico.blockchainist.contract.ReadBalanceRequest;
import pt.tecnico.blockchainist.contract.ReadBalanceResponse;
import pt.tecnico.blockchainist.contract.Signature;
import pt.tecnico.blockchainist.contract.SignedTransaction;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.contract.TransactionResponse;
import pt.tecnico.blockchainist.node.domain.NodeState;

// Class that implements the Node's server functionalities (such as listening for requests)
/**
 * Class that represents a Node's server side functionalities, such as listening
 * for requests
 * Has methods for each available operations
 */
public class NodeServiceImpl extends NodeServiceGrpc.NodeServiceImplBase {

    private final NodeState node = new NodeState();
    private final NodeSequencerService nodeSequencerService;
    private String host;
    private String organization;

    private final Encryption encryption = new Encryption();

    /**
     * Builder for this class
     * 
     * @param host         address of target sequencer to which we'll send each
     *                     transaction before processing
     * @param organization of the node
     */
    public NodeServiceImpl(String host, String organization) {
        this.host = host;
        this.organization = organization;
        this.nodeSequencerService = new NodeSequencerService(host, node);
    }

    /**
     * Getter for this node's organization
     * 
     * @return organization
     */
    public String getOrganization() {
        return this.organization;
    }

    /**
     * Getter for this node's sequencer host address
     * 
     * @return sequencer address
     */
    public String getHost() {
        return this.host;
    }

    private boolean checkMembership(String signerId) {
        return node.getMembershipList().get(organization).contains(signerId);

    }

    /**
     * Receives the client Request and check is the signature is valid
     * 
     * @param clientRequest
     * @return 0 if the client signature is valid and user belongs to node's
     *         organization
     *         1 if client signature is invalid, 2 if user does not belongs to
     *         node's organization
     */
    private int checkClientValidity(SignedTransaction clientRequest) {
        PublicKey publicKey = null;

        Signature receivedSignature = clientRequest.getSignature();
        String signerId = receivedSignature.getSignerIdentifier();
        String path = String.format("%s.pub", signerId);

        if (checkMembership(signerId)) {
            try {
                publicKey = encryption.loadPublicKey(path);
                Debug.log("Client " + signerId + " public key loaded");
            } catch (Exception e) {
                Debug.log("Failed to load public key of " + signerId);
            }

            try {
                java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
                sig.initVerify(publicKey);

                sig.update(clientRequest.getTransaction().toByteArray());
                boolean isValid = sig.verify(receivedSignature.getSignatureValue().toByteArray());

                // same as return isValid but to return 0
                if (isValid)
                    return 0;
            } catch (NoSuchAlgorithmException e) {
                Debug.log("No such algorithm found: SHA256withRSA");
            } catch (InvalidKeyException e) {
                Debug.log("IInvalid public key for signer: " + signerId);
            } catch (SignatureException e) {
                Debug.log("Signature exception while verifying signature for signer: " + signerId);
            }

            return 1;
        } else {
            return 2;
        }
    }

    /**
     * Client now calls a generic executeTransaction function that only after
     * speaking to the sequencer redirects to createWallet or deleteWallet, etc
     */
    @Override
    public void executeTransaction(SignedTransaction signedTransaction,
            StreamObserver<TransactionResponse> responseObserver) {
        switch (checkClientValidity(signedTransaction)) {
            case 0:
                Debug.log("Signature verified, processing transaction");
                Transaction transaction = signedTransaction.getTransaction();
                verifyAbsArguments(transaction, responseObserver);

                // flag to check if transfer was done in node's blockchain
                boolean transferDone = false;
                Long transferDoneCommandNumber = transaction.getCommandNumber();

                if (transaction.getOperationCase() == Transaction.OperationCase.TRANSFER) {
                    Debug.log("(t" + Thread.currentThread().getId()
                            + ") Transfer detected, attempting to execute immediately");
                    try {
                        node.parseTransaction(transaction);
                        transferDone = true;
                        TransactionResponse response = TransactionResponse.newBuilder().build();

                        responseObserver.onNext(response);
                        Debug.log("(t" + Thread.currentThread().getId()
                                + ") Transfer completed and responded to client, notifying sequencer");
                        responseObserver.onCompleted();
                    } catch (IllegalArgumentException e) {
                        if (!e.getMessage().equals("Insufficient funds")) {
                            Debug.log("(t" + Thread.currentThread().getId() + ") Failed to execute transfer due to "
                                    + e.getMessage());

                        } else {
                            Debug.log("(t" + Thread.currentThread().getId()
                                    + ") Immediate transfer failed due to insufficient funds, sending to sequencer");
                        }
                    }
                }

                // Send transaction to sequencer
                BroadcastResponse broadcastResponse = nodeSequencerService.broadcast(transaction,
                        transaction.getCommandNumber());
                Debug.log(
                        "(t" + Thread.currentThread().getId() + ") Transaction broadcasted, received sequence number: "
                                + broadcastResponse.getSequenceNumber());

                synchronized (this) {
                    // Sequencer returns sequence number of the block the sent transaction belongs
                    // to
                    // We must update this node's blockchain up until that block before processing
                    // that block
                    int targetBlock = broadcastResponse.getSequenceNumber(); // received sequence number
                    if (targetBlock == -1) {
                        // transaction already handled
                        Debug.log("(t" + Thread.currentThread().getId() + ") Request already handled by another node");
                        return;
                    }
                    int nextBlock = node.getBlockchain().size() + 1;

                    try {
                        // The last iteration pulls and applies the targetBlock
                        while (nextBlock <= targetBlock) {
                            // Get block
                            DeliverBlockResponse deliverBlockResponse = nodeSequencerService.deliverBlock(nextBlock);
                            node.addBlockToLocalBlockchain(deliverBlockResponse.getTransactionsList());
                            Debug.log("(t" + Thread.currentThread().getId() + ") Block delivered with sequence number: "
                                    + deliverBlockResponse.getSequenceNumber());

                            // Parse and apply transactions
                            for (Transaction t : deliverBlockResponse.getTransactionsList()) {

                                if (!transferDone || transferDoneCommandNumber != t.getCommandNumber()) {
                                    Debug.log(
                                            "(t" + Thread.currentThread().getId() + ") Applying transaction: "
                                                    + t.toString());
                                    node.parseTransaction(t);

                                    // Only skip the transaction if the transfer was already done AND we're parsing
                                    // the exact transaction that was done
                                } else {
                                    Debug.log(
                                            "(t" + Thread.currentThread().getId() + ") Skipping already done transfer: "
                                                    + t.toString());
                                    continue;
                                }
                            }
                            nextBlock++;
                        }

                        // Only respond to client if the response hasnt been sent already
                        if (!transferDone || transferDoneCommandNumber != transaction.getCommandNumber()) {
                            // This is to respond to the client the transaction that they asked for
                            TransactionResponse response = TransactionResponse.newBuilder().build();

                            // Send a single response through the stream.
                            responseObserver.onNext(response);
                            Debug.log("(t" + Thread.currentThread().getId() + ") Responded to client");
                            // Notify the client that the operation has been completed.
                            responseObserver.onCompleted();
                        }
                    } catch (IllegalArgumentException e) {
                        // Catch specific exception
                        Debug.log("(t" + Thread.currentThread().getId() + ") IllegalArgumentException detected");
                        responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
                    } catch (StatusRuntimeException e) {
                        // Catch exception when the Sequencer is unavailable
                        Debug.log("(t" + Thread.currentThread().getId() + ") StatusRuntimeException detected");
                        responseObserver
                                .onError(UNAVAILABLE.withDescription("Sequencer is unavailable").withCause(e)
                                        .asRuntimeException());
                    }
                }
                break;
            case 1:
                Debug.log("The signature was not valid");
                TransactionResponse response1 = TransactionResponse.newBuilder()
                    .setMessage("Signature failed").build();
                responseObserver.onNext(response1);
                responseObserver.onCompleted();
                break;
            case 2:
                Debug.log("User does not belong to this organization");
                TransactionResponse response2 = TransactionResponse.newBuilder()
                    .setMessage("Membership failed").build();
                responseObserver.onNext(response2);
                responseObserver.onCompleted();
                break;
        }
    }

    /**
     * Verifies a transaction's arguments that don't depend on the Application's
     * state, such as numbers being negative
     * or arguments being blank
     * 
     * @param transaction      that has the arguments to verify
     * @param responseObserver stream to end the error outputs to
     */
    private void verifyAbsArguments(Transaction transaction, StreamObserver<TransactionResponse> responseObserver) {
        try {
            // Check arguments
            switch (transaction.getOperationCase()) {

                case CREATE_WALLET:
                    node.requireNonBlankId(transaction.getCreateWallet().getUserId(), "userId");
                    node.requireNonBlankId(transaction.getCreateWallet().getWalletId(), "walletId");
                    break;

                case DELETE_WALLET:
                    node.requireNonBlankId(transaction.getDeleteWallet().getUserId(), "userId");
                    node.requireNonBlankId(transaction.getDeleteWallet().getWalletId(), "walletId");

                    if (node.getBcUserId().equals(transaction.getDeleteWallet().getWalletId())) {
                        throw new IllegalArgumentException("Cannot delete central bank wallet");
                    }

                    break;

                case TRANSFER:
                    node.requireNonBlankId(transaction.getTransfer().getSrcUserId(), "srcUserId");
                    node.requireNonBlankId(transaction.getTransfer().getSrcWalletId(), "srcWalletId");
                    node.requireNonBlankId(transaction.getTransfer().getDstWalletId(), "dstWalletId");

                    Long amount = transaction.getTransfer().getValue();
                    if (amount == null) {
                        throw new IllegalArgumentException("Amount is required in transfer");
                    }
                    if (amount <= 0L) {
                        throw new IllegalArgumentException("Transfer amount must be positive");
                    }

                    break;
            }
        } catch (IllegalArgumentException e) {
            Debug.log("(t" + Thread.currentThread().getId() + ") IllegalArgumentException detected");
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
            responseObserver.onCompleted();
        }
    }

    /**
     * Method that executes the readBalance operation and responds to the client
     */
    @Override
    public void readBalance(ReadBalanceRequest request, StreamObserver<ReadBalanceResponse> responseObserver) {
        Debug.log("readBalance request received");

        try {
            // Node executes the transaction (Client -> Node)
            node.requireNonBlankId(request.getWalletId(), "walletId");
            Long balance = node.readBalance(request.getWalletId());

            ReadBalanceResponse response = ReadBalanceResponse.newBuilder().setBalance(balance).build();

            // Send a single response through the stream.
            responseObserver.onNext(response);
            // Notify the client that the operation has been completed.
            responseObserver.onCompleted();

            Debug.log("readBalance response sent to client");

            // Catch specific exception (from method readBalance)
        } catch (IllegalArgumentException e) {
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        }
    }

    /**
     * Method that executes the getBlockchainState operation and responds to the
     * client
     */
    @Override
    public void getBlockchainState(GetBlockchainStateRequest request,
            StreamObserver<GetBlockchainStateResponse> responseObserver) {
        Debug.log("getBlockchainState request received");

        try {
            // Node executes the transaction (Client -> Node)
            List<List<Transaction>> blocksList = node.getBlockchainState();
            List<Transaction> transactions = new ArrayList<>();

            // Flatten -> verificar se é preciso ou n devemos
            for (int block = 0; block < blocksList.size(); block++) {
                for (Transaction t : blocksList.get(block)) {
                    transactions.add(t);
                }
            }

            GetBlockchainStateResponse response = GetBlockchainStateResponse.newBuilder()
                    .addAllTransactions(transactions).build();

            // Send a single response through the stream.
            responseObserver.onNext(response);
            // Notify the client that the operation has been completed.
            responseObserver.onCompleted();

            Debug.log("getBlockchainState response sent to client");

            // Catch specific exception (from method getBlockchainState)
        } catch (IllegalArgumentException e) {
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
            // Catch any other unexpected exception
        } catch (Exception e) {
            responseObserver
                    .onError(INTERNAL.withDescription("Failed to get Blockchain State").withCause(e)
                            .asRuntimeException());
        }
    }
}
