package pt.tecnico.blockchainist.client.grpc;

import java.security.PrivateKey;
import java.util.concurrent.TimeUnit;
import java.security.NoSuchAlgorithmException;
import java.security.SignatureException;
import java.security.InvalidKeyException;

import com.google.protobuf.ByteString;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Metadata;
import io.grpc.StatusRuntimeException;
import io.grpc.stub.MetadataUtils;
import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.common.Encryption;
import pt.tecnico.blockchainist.contract.CreateWalletRequest;
import pt.tecnico.blockchainist.contract.DeleteWalletRequest;
import pt.tecnico.blockchainist.contract.GetBlockchainStateRequest;
import pt.tecnico.blockchainist.contract.GetBlockchainStateResponse;
import pt.tecnico.blockchainist.contract.NodeServiceGrpc;
import pt.tecnico.blockchainist.contract.ReadBalanceRequest;
import pt.tecnico.blockchainist.contract.ReadBalanceResponse;
import pt.tecnico.blockchainist.contract.Signature;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.contract.TransactionResponse;
import pt.tecnico.blockchainist.contract.TransferRequest;
import pt.tecnico.blockchainist.contract.SignedTransaction;

/**
 * Class that represents the Client-Node communications (for 1 node
 * specifically)
 * It implements the Client's client side functionalities and has methods for
 * each of the client's operations
 */
public class ClientNodeService {
    private final ManagedChannel channel;
    private NodeServiceGrpc.NodeServiceBlockingStub stub;
    private NodeServiceGrpc.NodeServiceStub asyncStub;
    private String organization;
    private static final Metadata.Key<String> DELAY_KEY = Metadata.Key.of("delay", Metadata.ASCII_STRING_MARSHALLER);
    private static final int DEADLINE = 10;

    private final Encryption encryption = new Encryption();

    /**
     * Builder method for this class
     * 
     * @param host         address of target node
     * @param port         port of target node
     * @param organization organization of target node
     */
    public ClientNodeService(String host, int port, String organization) {
        String target = host + ":" + port;
        this.channel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
        this.stub = NodeServiceGrpc.newBlockingStub(channel);
        this.asyncStub = NodeServiceGrpc.newStub(channel);
        this.organization = organization;
    }

    /**
     * Channel getter
     * 
     * @return communications channel
     */
    public ManagedChannel getChannel() {
        return this.channel;
    }

    /**
     * Stub getter
     * 
     * @return blocking stub for communications
     */
    public NodeServiceGrpc.NodeServiceBlockingStub getStub() {
        return this.stub;
    }

    /**
     * Stub getter
     * 
     * @return non-blocking stub for communications
     */
    public NodeServiceGrpc.NodeServiceStub getAsyncStub() {
        return this.asyncStub;
    }

    /**
     * Organization getter
     * 
     * @return organization of target node
     */
    public String getOrganization() {
        return this.organization;
    }

    /**
     * Method to terminate all communications with the node gracefully
     */
    public void shutdown() {
        if (this.channel != null && !this.channel.isShutdown()) {
            this.channel.shutdown();
            try {
                if (!this.channel.awaitTermination(5, TimeUnit.SECONDS)) {
                    this.channel.shutdownNow();
                }
            } catch (InterruptedException e) {
                this.channel.shutdownNow();
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * @param delay amount of time the node sleeps for after receiving the request
     * @return blocking stub with metadata
     */
    public NodeServiceGrpc.NodeServiceBlockingStub createMetadataBlockingStub(Integer delay) {
        Metadata metadata = new Metadata();

        metadata.put(DELAY_KEY, String.valueOf(delay == null ? 0 : delay));

        return this.stub.withInterceptors(MetadataUtils.newAttachHeadersInterceptor((metadata)));
    }

    /**
     * @param delay amount of time the node sleeps for after receiving the request
     * @return async stub with metadata
     */
    public NodeServiceGrpc.NodeServiceStub createMetadataAsyncStub(Integer delay) {
        Metadata metadata = new Metadata();

        metadata.put(DELAY_KEY, String.valueOf(delay == null ? 0 : delay));

        return this.asyncStub.withInterceptors(MetadataUtils.newAttachHeadersInterceptor((metadata)));
    }

    private Signature signTransaction(String userId, Transaction request) {
        try {
            String path = String.format("%s.priv", userId);
            PrivateKey privateKey = encryption.loadPrivateKey(path);
            Debug.log("Private Key from client loaded successfully");

            java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
            sig.initSign(privateKey);
            sig.update(request.toByteArray());
            byte[] signatureBytes = sig.sign();

            Signature signature = Signature.newBuilder()
                    .setSignerIdentifier(userId)
                    .setSignatureValue(ByteString.copyFrom(signatureBytes))
                    .build();

            return signature;
        } catch(NoSuchAlgorithmException e) {
            Debug.log("Signature getInstance failed " + e.getMessage());
            return null;
        } catch(InvalidKeyException e) {
            Debug.log("Signature initialization failed " + e.getMessage());
            return null;
        } catch(SignatureException e) {
            Debug.log("Signature failed " + e.getMessage());
            return null;
        } catch(Exception e) {
            Debug.log("Failed to load the client's private key " + e.getMessage());
            return null;
        }
    }

    /**
     * Method to send a request for the createWallet operation and to receive the
     * node's response
     * 
     * @param userId     ID of the wallet's owner
     * @param walletId   ID for the new wallet
     * @param delay      amount of time the node sleeps for after receiving the
     *                   request
     * @param isBlocking true if the stub is blocking, false otherwise
     * @return the response for the createWallet operation, which may or may not be
     *         an error
     */
    public TransactionResponse createWallet(String userId, String walletId, Integer delay, boolean isBlocking,
            long commandNumber) {

        // Client -> Node
        CreateWalletRequest requestCreateWallet = CreateWalletRequest.newBuilder()
                .setUserId(userId)
                .setWalletId(walletId)
                .build();

        Transaction request = Transaction.newBuilder()
                .setCreateWallet(requestCreateWallet)
                .setCommandNumber(commandNumber)
                .build();

        Signature signature = signTransaction(userId, request);

        SignedTransaction signedRequest = SignedTransaction.newBuilder()
                .setTransaction(request)
                .setSignature(signature)
                .build();

        if (isBlocking) {
            // Create blocking stub with metadata
            NodeServiceGrpc.NodeServiceBlockingStub stub = createMetadataBlockingStub(delay).withDeadlineAfter(DEADLINE,
                    TimeUnit.SECONDS);

            try {
                return stub.executeTransaction(signedRequest);

            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }
        } else {
            // Create async stub with metadata
            NodeServiceGrpc.NodeServiceStub asyncStub = createMetadataAsyncStub(delay);

            try {
                // This already creates a new thread and then returns immediately
                asyncStub.executeTransaction(signedRequest, new TransactionObserver(commandNumber, "Create"));
                return TransactionResponse.newBuilder().setMessage("Async returned").build();
            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }
        }
    }

    /**
     * Method to send a request for the deleteWallet operation and to receive the
     * node's response
     * 
     * @param userId     ID of the wallet's owner
     * @param walletId   ID of the wallet to be deleted
     * @param delay      amount of time the node sleeps for after receiving the
     *                   request
     * @param isBlocking true if the stub is blocking, false otherwise
     * @return the response for the deleteWallet operation, which may or may not be
     *         an error
     */
    public TransactionResponse deleteWallet(String userId, String walletId, Integer delay, boolean isBlocking,
            long commandNumber) {

        // Client -> Node
        DeleteWalletRequest requestDeleteWallet = DeleteWalletRequest.newBuilder()
                .setUserId(userId)
                .setWalletId(walletId)
                .build();

        Transaction request = Transaction.newBuilder()
                .setDeleteWallet(requestDeleteWallet)
                .setCommandNumber(commandNumber)
                .build();

        Signature signature = signTransaction(userId, request);

        SignedTransaction signedRequest = SignedTransaction.newBuilder()
                .setTransaction(request)
                .setSignature(signature)
                .build();

        if (isBlocking) {
            // Create blocking stub with metadata
            NodeServiceGrpc.NodeServiceBlockingStub stub = createMetadataBlockingStub(delay).withDeadlineAfter(DEADLINE,
                    TimeUnit.SECONDS);

            try {
                return stub.executeTransaction(signedRequest);

            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }

        } else {
            NodeServiceGrpc.NodeServiceStub asyncStub = createMetadataAsyncStub(delay);

            try {
                asyncStub.executeTransaction(signedRequest, new TransactionObserver(commandNumber, "Delete"));
                return TransactionResponse.newBuilder().setMessage("Async returned").build();
            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }
        }
    }

    /**
     * Method to send a request for the readBalance operation and to receive the
     * node's response
     * 
     * @param walletId   ID of the wallet to get the balance from
     * @param delay      amount of time the node sleeps for after receiving the
     *                   request
     * @param isBlocking true if the stub is blocking, false otherwise
     * @return the response for the readBalance operartion, which may or may not be
     *         an error
     */
    public ReadBalanceResponse readBalance(String walletId, Integer delay, boolean isBlocking, long commandNumber) {

        // Client -> Node
        ReadBalanceRequest request = ReadBalanceRequest.newBuilder()
                .setWalletId(walletId)
                .setCommandNumber(commandNumber)
                .build();

        if (isBlocking) {
            // Create blocking stub with metadata
            NodeServiceGrpc.NodeServiceBlockingStub stub = createMetadataBlockingStub(delay).withDeadlineAfter(DEADLINE,
                    TimeUnit.SECONDS);

            try {
                return stub.readBalance(request);

            } catch (StatusRuntimeException e) {
                return ReadBalanceResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }

        } else {
            // Create async stub with metadata
            NodeServiceGrpc.NodeServiceStub asyncStub = createMetadataAsyncStub(delay);

            try {
                asyncStub.readBalance(request, new StreamObserver<ReadBalanceResponse>() {
                    /**
                     * Called when the server returns the balance
                     */
                    @Override
                    public void onNext(ReadBalanceResponse response) {
                        System.out.println("OK " + commandNumber);
                        Debug.log("Balance command completed successfully");
                    }

                    /**
                     * Called if the RPC fails
                     */
                    @Override
                    public void onError(Throwable t) {
                    }

                    /**
                     * Called when the call finishes
                     */
                    @Override
                    public void onCompleted() {
                    }
                });

                return ReadBalanceResponse.newBuilder().setMessage("Async returned").build();
            } catch (StatusRuntimeException e) {
                return ReadBalanceResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }
        }
    }

    /**
     * Method to send a request for the transfer operation and to receive the node's
     * response
     * 
     * @param sourceUserId        ID of the user that is providing the money
     * @param sourceWalletId      ID of the wallet that is providing the money
     * @param destinationWalletId ID of the wallet that is receiving the money
     * @param amount              money to be transferred
     * @param delay               amount of time the node sleeps for after receiving
     *                            the request
     * @param isBlocking          true if the stub is blocking, false otherwise
     * @return the response for the transfer operation, which may or may not be an
     *         error
     */
    public TransactionResponse transfer(String sourceUserId, String sourceWalletId,
            String destinationWalletId, long amount, Integer delay, boolean isBlocking, long commandNumber) {

        // Client -> Node
        TransferRequest requestTransferRequest = TransferRequest.newBuilder()
                .setSrcUserId(sourceUserId)
                .setSrcWalletId(sourceWalletId)
                .setDstWalletId(destinationWalletId)
                .setValue(amount)
                .build();

        Transaction request = Transaction.newBuilder()
                .setTransfer(requestTransferRequest)
                .setCommandNumber(commandNumber).build();

        Signature signature = signTransaction(sourceUserId, request);

        SignedTransaction signedRequest = SignedTransaction.newBuilder()
                .setTransaction(request)
                .setSignature(signature)
                .build();

        if (isBlocking) {
            // Create blocking stub with metadata
            NodeServiceGrpc.NodeServiceBlockingStub stub = createMetadataBlockingStub(delay).withDeadlineAfter(DEADLINE,
                    TimeUnit.SECONDS);

            try {
                return stub.executeTransaction(signedRequest);

            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }

        } else {
            NodeServiceGrpc.NodeServiceStub asyncStub = createMetadataAsyncStub(delay);

            try {
                // This already creates a new thread and then returns immediately
                asyncStub.executeTransaction(signedRequest, new TransactionObserver(commandNumber, "Transfer"));
                return TransactionResponse.newBuilder().setMessage("Async returned").build();
            } catch (StatusRuntimeException e) {
                return TransactionResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
            }
        }
    }

    /**
     * Method to send a request for the getBlockchainState operation and get the
     * node's response
     * 
     * @return the response for the getBlockchainState operation, which may or may
     *         not be an error
     */
    public GetBlockchainStateResponse getBlockchainState() {

        // Client -> Node
        GetBlockchainStateRequest request = GetBlockchainStateRequest.newBuilder().build();

        try {
            // Node -> Client
            return this.stub.getBlockchainState(request);
        } catch (StatusRuntimeException e) {
            // If fails, return a response with the error message
            return GetBlockchainStateResponse.newBuilder().setMessage("RPC failed: " + e.getStatus()).build();
        }
    }
}
